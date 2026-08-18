#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/gds/grp.hpp"
#include "starcraft/gds/tileset.hpp"
#include "starcraft/lang/count.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_init.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/lang/place_unit.hpp"
#include "starcraft/runtime/storm.hpp"
#include "starcraft/runtime/asset_archives.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool build_terrain_preview(
    const starcraft::gds::TilesetData &tileset,
    const starcraft::game::MultiplayerScenario &scenario,
    const std::uint16_t camera_x, const std::uint16_t camera_y,
    SpritePreviewFrame &output,
    const std::vector<std::uint8_t> *const creep_tiles,
    const std::vector<std::uint8_t> *const creep_visual_tiles,
    const std::vector<std::uint8_t> *const creep_edge_frames) {
  output = {};
  const std::uint32_t map_width =
      static_cast<std::uint32_t>(scenario.width()) * 32U;
  const std::uint32_t map_height =
      static_cast<std::uint32_t>(scenario.height()) * 32U;
  if (!scenario.valid() || camera_x + kMapViewportWidth > map_width ||
      camera_y + kMapViewportHeight > map_height) {
    return false;
  }
  const std::uint16_t camera_tile_x = camera_x / 32U;
  const std::uint16_t camera_tile_y = camera_y / 32U;
  const std::uint16_t offset_x = camera_x % 32U;
  const std::uint16_t offset_y = camera_y % 32U;
  const std::uint16_t tile_columns =
      static_cast<std::uint16_t>((kMapViewportWidth + offset_x + 31U) / 32U);
  const std::uint16_t tile_rows =
      static_cast<std::uint16_t>((kMapViewportHeight + offset_y + 31U) / 32U);
  output.width = kMapViewportWidth;
  output.height = kMapViewportHeight;
  output.bgra.resize(static_cast<std::size_t>(output.width) * output.height,
                     0xFF000000U);
  output.palette_indices.resize(
      static_cast<std::size_t>(output.width) * output.height, 0U);
  output.opacity.resize(static_cast<std::size_t>(output.width) * output.height,
                        0xFFU);
  const auto &palette = tileset.palette();
  if (!tileset.valid() || palette.size() != 1024) {
    return false;
  }

  for (std::uint16_t tile_y = 0; tile_y < tile_rows; ++tile_y) {
    for (std::uint16_t tile_x = 0; tile_x < tile_columns; ++tile_x) {
      std::uint16_t map_tile_id{};
      starcraft::gds::IndexedMapTile field_tile{};
      if (!scenario.tile(camera_tile_x + tile_x, camera_tile_y + tile_y,
                         map_tile_id) ||
          camera_tile_x + tile_x >= scenario.width() ||
          camera_tile_y + tile_y >= scenario.height()) {
        return false;
      }
      const std::size_t map_index =
          static_cast<std::size_t>(camera_tile_y + tile_y) * scenario.width() +
          camera_tile_x + tile_x;
      const std::size_t map_tile_count =
          static_cast<std::size_t>(scenario.width()) * scenario.height();
      if (creep_tiles != nullptr && creep_visual_tiles != nullptr &&
          creep_tiles->size() == map_tile_count &&
          creep_visual_tiles->size() == map_tile_count &&
          (*creep_tiles)[map_index] != 0U &&
          (*creep_visual_tiles)[map_index] >= 16U) {
        // staredit mapcomn creep.cpp::sub_4D0DD2 writes rand()%9+16 into
        // TILE when a tile first becomes creep. Those values select CV5
        // group 1, members 0..8, which are the actual tileset creep art.
        map_tile_id = (*creep_visual_tiles)[map_index];
      }
      if (!tileset.render(map_tile_id, field_tile)) {
        return false;
      }
      for (std::size_t y = 0; y < starcraft::gds::IndexedMapTile::height; ++y) {
        for (std::size_t x = 0; x < starcraft::gds::IndexedMapTile::width;
             ++x) {
          const std::uint8_t palette_index =
              field_tile.pixels[y * starcraft::gds::IndexedMapTile::width + x];
          const std::size_t color = 4U * palette_index;
          const std::uint32_t red = palette[color];
          const std::uint32_t green = palette[color + 1];
          const std::uint32_t blue = palette[color + 2];
          const int destination_y =
              static_cast<int>(tile_y * 32U + y) - offset_y;
          const int destination_x =
              static_cast<int>(tile_x * 32U + x) - offset_x;
          if (destination_x < 0 || destination_y < 0 ||
              destination_x >= output.width || destination_y >= output.height) {
            continue;
          }
          const std::size_t destination =
              static_cast<std::size_t>(destination_y) * output.width +
              destination_x;
          output.bgra[destination] =
              0xFF000000U | blue | (green << 8U) | (red << 16U);
          output.palette_indices[destination] = palette_index;
        }
      }
      if (creep_edge_frames != nullptr &&
          creep_edge_frames->size() == map_tile_count &&
          (*creep_edge_frames)[map_index] != 0U) {
        starcraft::gds::DecodedGrpFrame edge{};
        if (!tileset.creep_edge_frame((*creep_edge_frames)[map_index], edge)) {
          return false;
        }
        // sub_4BE36B draws the selected tileset .grp descriptor at the
        // current megatile's framebuffer address using its GRP offsets.
        for (std::size_t y = 0; y < edge.height; ++y) {
          for (std::size_t x = 0; x < edge.width; ++x) {
            const std::size_t source = y * edge.width + x;
            if (source >= edge.opacity.size() || edge.opacity[source] == 0U) {
              continue;
            }
            const int destination_x = static_cast<int>(tile_x * 32U) -
                                      offset_x + edge.x_offset +
                                      static_cast<int>(x);
            const int destination_y = static_cast<int>(tile_y * 32U) -
                                      offset_y + edge.y_offset +
                                      static_cast<int>(y);
            if (destination_x < 0 || destination_y < 0 ||
                destination_x >= output.width ||
                destination_y >= output.height) {
              continue;
            }
            const std::uint8_t palette_index = edge.pixels[source];
            const std::size_t color = 4U * palette_index;
            const std::size_t destination =
                static_cast<std::size_t>(destination_y) * output.width +
                destination_x;
            output.bgra[destination] =
                0xFF000000U | palette[color + 2U] |
                (static_cast<std::uint32_t>(palette[color + 1U]) << 8U) |
                (static_cast<std::uint32_t>(palette[color]) << 16U);
            output.palette_indices[destination] = palette_index;
          }
        }
      }
    }
  }
  return true;
}

BootstrapStatus probe_assets(
    const std::filesystem::path &selected_map,
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>
        *const ownership_override,
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>
        *const race_override,
    const std::uint32_t melee_start_seed,
    const std::uint8_t requested_local_player) {
  BootstrapStatus status{};
  status.local_player =
      requested_local_player < starcraft::data::chk_player_slot_count
          ? requested_local_player
          : 0U;
  status.command_player = status.local_player;
  // Offline skirmish currently has one authored baseline difficulty. Keep
  // new matches explicitly pinned to Medium even if a previous match changed
  // its live console cvar.
  status.ai_difficulty = AiDifficulty::medium;
  status.weapon_asset_indices.fill(SIZE_MAX);
  const std::filesystem::path root = locate_input_root();
  if (root.empty()) {
    status.primary = "Licensed input directory was not found.";
    status.detail = "Run beside storm.dll and a supported StarCraft MPQ set, or from the "
                    "configured build tree.";
    return status;
  }

  starcraft::runtime::StormModule storm{root / L"storm.dll"};
  if (!storm.loaded()) {
    status.primary = "The supplied storm.dll could not be loaded.";
    return status;
  }

  starcraft::runtime::AssetArchives asset_archives{};
  if (!asset_archives.open(storm, root)) {
    status.primary = "Storm loaded, but no beta or retail asset MPQs could be opened.";
    return status;
  }

  // lang\maphdr.cpp::sub_46C3D0 at 0x0046C3D0 opens an SCM as a Storm
  // archive and then reads staredit\scenario.chk from it. Keep the supplied
  // map read-only and use the same archive path here.
  const std::filesystem::path relative_map =
      selected_map.empty()
          ? std::filesystem::path{L"maps"} / L"96x96_space4.scm"
          : selected_map;
  const std::filesystem::path map_path =
      relative_map.is_absolute() ? relative_map : root / relative_map;
  status.map_name = relative_map.generic_string();
  std::replace(status.map_name.begin(), status.map_name.end(), '/', '\\');
  void *map_archive{};
  const bool map_opened = storm.open_archive(map_path, &map_archive, 4000);
  std::vector<std::uint8_t> map_chk;
  const bool map_loaded =
      map_opened && storm.load_file_from_archive(
                        map_archive, R"(staredit\scenario.chk)", map_chk);
  const starcraft::data::ChkView chk{map_chk.data(), map_chk.size()};
  starcraft::game::MultiplayerScenario scenario{};
  bool scenario_loaded = map_loaded && scenario.load(chk);
  if (scenario_loaded && ownership_override != nullptr &&
      race_override != nullptr) {
    for (std::size_t player = 0; player < ownership_override->size();
         ++player) {
      scenario_loaded =
          scenario.configure_player(player, (*ownership_override)[player],
                                    (*race_override)[player]) &&
          scenario_loaded;
    }
    scenario_loaded = scenario_loaded && scenario.active_player_count() >= 2U;
  }
  const bool randomized_melee_starts =
      scenario_loaded && melee_start_seed != 0U &&
      scenario.randomize_melee_start_locations(melee_start_seed);
  if (scenario_loaded && melee_start_seed != 0U &&
      !randomized_melee_starts) {
    scenario_loaded = false;
  }
  // The recovered game keeps g_hScenArchive open, but this bootstrap's
  // transitional global Storm lookup must not search the map MPQ for every
  // DAT/GRP asset. The complete CHK is already resident, so close it now.
  const bool map_closed =
      map_archive == nullptr || storm.close_archive(map_archive);
  map_archive = nullptr;

  starcraft::data::CoreDataSet data{};
  const bool data_loaded = data.load(storm);
  // Preserve the recovered binary script table as an optional compatibility
  // fallback. New computer players are driven by scripts/ai through CAI.
  status.ai_script_bytes.clear();
  (void)storm.load_file(R"(scripts\aiscript.bin)", status.ai_script_bytes);
  status.unit_traits_ready =
      data_loaded && data.extract_unit_traits(status.unit_traits);
  bool research_data_ready = data_loaded;
  if (research_data_ready) {
    try {
      status.weapon_display_traits.resize(status.weapon_traits.size());
      status.upgrade_display_traits.resize(status.upgrade_traits.size());
    } catch (...) {
      research_data_ready = false;
    }
  }
  for (std::size_t technology = 0;
       research_data_ready && technology < status.technology_traits.size();
       ++technology) {
    research_data_ready = data.technology_research_traits(
        static_cast<std::uint16_t>(technology),
        status.technology_traits[technology]);
  }
  for (std::size_t weapon = 0;
       research_data_ready && weapon < status.weapon_traits.size(); ++weapon) {
    research_data_ready = data.weapon_simulation_traits(
                              static_cast<std::uint16_t>(weapon),
                              status.weapon_traits[weapon]) &&
                          data.weapon_display_traits(
                              static_cast<std::uint16_t>(weapon),
                              status.weapon_display_traits[weapon]
                                  .label_string_id,
                              status.weapon_display_traits[weapon].icon);
  }
  for (std::size_t order = 0;
       research_data_ready && order < status.order_weapons.size(); ++order) {
    research_data_ready = data.order_spell_traits(
        static_cast<std::uint16_t>(order), status.order_weapons[order],
        status.order_technologies[order], status.order_animations[order]);
  }
  for (std::size_t upgrade = 0;
       research_data_ready && upgrade < status.upgrade_traits.size();
       ++upgrade) {
    research_data_ready = data.upgrade_research_traits(
                              static_cast<std::uint16_t>(upgrade),
                              status.upgrade_traits[upgrade]) &&
                          data.upgrade_display_traits(
                              static_cast<std::uint16_t>(upgrade),
                              status.upgrade_display_traits[upgrade]
                                  .label_string_id,
                              status.upgrade_display_traits[upgrade].icon);
  }
  const bool unit_sound_ranges_ready =
      data_loaded && extract_unit_sound_ranges(data, status);
  starcraft::gds::TilesetData terrain_tileset{};
  std::vector<std::uint8_t> palette;
  std::vector<std::uint8_t> ui_palette;
  char local_race_code{};
  bool focus_unit_found{};
  if (scenario_loaded) {
    status.scenario_width = scenario.width();
    status.scenario_height = scenario.height();
    status.scenario_unit_count = scenario.units().size();
    status.scenario_sprite_count = chk.sprite_count();
    status.active_player_count = scenario.active_player_count();
    for (std::size_t player = 0; player < status.active_players.size();
         ++player) {
      status.active_players[player] = scenario.players()[player].ownership != 0;
    }

    // maphdr.cpp::sub_46BC40 at 0x0046BC40 derives the initial camera from
    // the local player's type-214 marker as (x / 32 - 10, y / 32 - 6),
    // clamped at zero. Network clients use their assigned stable slot.
    std::uint16_t focus_x = static_cast<std::uint16_t>(scenario.width() * 16U);
    std::uint16_t focus_y = static_cast<std::uint16_t>(scenario.height() * 16U);
    const starcraft::game::ScenarioStartLocation &local_start =
        scenario.start_locations()[status.local_player];
    if (local_start.present) {
      focus_x = local_start.x;
      focus_y = local_start.y;
    }
    const std::uint16_t viewport_columns =
        (kMapViewportWidth + starcraft::gds::IndexedMapTile::width - 1) /
        starcraft::gds::IndexedMapTile::width;
    const std::uint16_t viewport_rows =
        (kMapViewportHeight + starcraft::gds::IndexedMapTile::height - 1) /
        starcraft::gds::IndexedMapTile::height;
    const std::uint16_t centered_x =
        focus_x / 32U > 10U ? focus_x / 32U - 10U : 0U;
    const std::uint16_t centered_y =
        focus_y / 32U > 6U ? focus_y / 32U - 6U : 0U;
    status.camera_tile_x = centered_x < scenario.width() - viewport_columns
                               ? centered_x
                               : scenario.width() - viewport_columns;
    status.camera_tile_y = centered_y < scenario.height() - viewport_rows
                               ? centered_y
                               : scenario.height() - viewport_rows;
    status.camera_x = static_cast<std::uint16_t>(32U * status.camera_tile_x);
    status.camera_y = static_cast<std::uint16_t>(32U * status.camera_tile_y);

    const std::string_view recovered_tileset_name =
        starcraft::gds::tileset_name(scenario.tileset_id());
    status.tileset_name.assign(recovered_tileset_name);
    if (!recovered_tileset_name.empty() &&
        terrain_tileset.load(storm, recovered_tileset_name)) {
      palette = terrain_tileset.palette();
      status.game_palette = palette;
      status.terrain_ready =
          build_terrain_preview(terrain_tileset, scenario, status.camera_x,
                                status.camera_y, status.terrain);
      // CImage.cpp::sub_409B00 first loads the tileset's shift.pcx into
      // gColorShifts[0]. gamedata.cpp then initializes the remaining four
      // entries from the names at 0x004F8208. CImage::sub_410F60 stores the
      // selected row-major lookup pointer for draw function 8/15 images.
      // sub_46A6F0 sizes each table as (PCX height + 1) * 256 and passes
      // destination +256 to Storm's PCX decoder. Row zero is deliberately
      // reserved; GRP source values 1..height select PCX rows 0..height-1.
      // This matters for pb1Glow.grp, whose valid bfire source range reaches
      // 40 even though bfire.pcx itself is 256x40.
      constexpr std::array<const char *, 5> shift_names{{
          "shift", "ofire", "gfire", "bfire", "trans50",
      }};
      for (std::size_t shift = 0; shift < shift_names.size(); ++shift) {
        std::string path = "Tileset\\";
        path.append(recovered_tileset_name);
        path.push_back('\\');
        path.append(shift_names[shift]);
        path.append(".pcx");
        starcraft::runtime::DecodedPcx lookup{};
        if (storm.load_pcx(path.c_str(), lookup) && lookup.width == 256U &&
            lookup.height != 0U && lookup.height <= 256U &&
            lookup.pixels.size() ==
                static_cast<std::size_t>(lookup.width) * lookup.height) {
          std::vector<std::uint8_t> table(
              static_cast<std::size_t>(lookup.width) * (lookup.height + 1U),
              0U);
          std::copy(lookup.pixels.begin(), lookup.pixels.end(),
                    table.begin() + 256U);
          status.image_color_shifts[shift] = std::move(table);
        }
      }
      // light.cpp::sub_46A670 loads this exact 256x32 palette-translation
      // surface. mask.cpp::sub_46D780 selects level 0 for black shroud, 15
      // for explored fog, and 31 for fully visible terrain.
      std::string dark_path = "Tileset\\";
      dark_path.append(recovered_tileset_name);
      dark_path += R"(\dark.pcx)";
      starcraft::runtime::DecodedPcx dark{};
      if (storm.load_pcx(dark_path.c_str(), dark) && dark.width == 256U &&
          dark.height == 32U && dark.pixels.size() == 32U * 256U) {
        status.terrain_dark_levels = std::move(dark.pixels);
      }
      // CImage.cpp::sub_40FB60 loads game\tselect.pcx as a 24-byte table.
      // Renderer 13 (sub_409820) consumes it as three eight-shade outline
      // rows selected by CUnitColor's local/allied/enemy value 0..2.
      starcraft::runtime::DecodedPcx selection_colors{};
      if (storm.load_pcx(R"(game\tselect.pcx)", selection_colors) &&
          selection_colors.width == 24U && selection_colors.height == 1U &&
          selection_colors.pixels.size() == 24U) {
        for (std::size_t relation = 0;
             relation < status.selection_color_indices.size(); ++relation) {
          std::copy_n(selection_colors.pixels.begin() + relation * 8U, 8U,
                      status.selection_color_indices[relation].begin());
        }
        status.selection_colors_ready = true;
      }
    }
    status.terrain_group_count = terrain_tileset.group_count();
    status.terrain_megatile_count = terrain_tileset.megatile_count();
    status.terrain_minitile_count = terrain_tileset.minitile_count();

    // status.cpp::sub_4ABFC0 at 0x004ABFC0 loads game\%cconsole.pcx.
    // StarCraft.exe's byte table at 0x004F260C is exactly "ztp".
    constexpr std::array<char, 3> race_codes{{'z', 't', 'p'}};
    constexpr std::array<const char *, 3> mineral_error_paths{{
        R"(sound\Zerg\Advisor\ZAdErr00.WAV)",
        R"(sound\Terran\Advisor\tAdErr00.WAV)",
        R"(sound\Protoss\Advisor\PAdErr00.WAV)",
    }};
    constexpr std::array<const char *, 3> gas_error_paths{{
        R"(sound\Zerg\Advisor\ZAdErr01.WAV)",
        R"(sound\Terran\Advisor\tAdErr01.WAV)",
        R"(sound\Protoss\Advisor\PAdErr01.WAV)",
    }};
    const std::uint8_t local_race =
        scenario.players()[status.local_player].race;
    status.local_race = local_race;
    if (local_race < race_codes.size()) {
      local_race_code = race_codes[local_race];
      status.hud_path = "game\\";
      status.hud_path.push_back(local_race_code);
      status.hud_path += "console.pcx";
      starcraft::runtime::DecodedPcx console{};
      status.hud_ready = storm.load_pcx(status.hud_path.c_str(), console) &&
                         decode_pcx_frame(console, true, status.hud);
      if (status.hud_ready) {
        ui_palette = console.palette;
        status.hud_palette = console.palette;
      }
      status.resource_error_wave_paths[0] = mineral_error_paths[local_race];
      status.resource_error_wave_paths[1] = gas_error_paths[local_race];
      (void)storm.load_file(status.resource_error_wave_paths[0].c_str(),
                            status.resource_error_waves[0]);
      (void)storm.load_file(status.resource_error_wave_paths[1].c_str(),
                            status.resource_error_waves[1]);
      // Music.cpp::sub_470720 at 0x00470720 opens the loose path stored in
      // sgSongs[nSong].szName. Gameplay entries in the executable's table are
      // music\terran1.wav (and the parallel Zerg/Protoss tracks). The supplied
      // licensed music is loose beside the MPQs, so try Storm first and then
      // read that exact original filesystem path without modifying it.
      constexpr std::array<const char *, 3> first_gameplay_tracks{{
          R"(music\zerg1.wav)",
          R"(music\terran1.wav)",
          R"(music\protoss1.wav)",
      }};
      status.music_path = first_gameplay_tracks[local_race];
      if (!storm.load_file(status.music_path.c_str(), status.music_wave)) {
        (void)read_loose_file(root / std::filesystem::path{status.music_path},
                              status.music_wave);
      }
      PcmWaveView music_view{};
      status.music_available = parse_pcm_wave(status.music_wave, music_view);
      starcraft::runtime::DecodedPcx unit_colors{};
      // CUnitColor initialization at 0x00424E20 asks the PCX decoder for
      // exactly 96 bytes from the literal game\tunit.pcx path (12 owners * 8
      // translated indices), even though its source row is 128 pixels wide.
      // This remap is shared by all three race consoles.
      status.team_colors_ready =
          storm.load_pcx(R"(game\tunit.pcx)", unit_colors) &&
          unit_colors.width == 128 && unit_colors.height == 1 &&
          unit_colors.pixels.size() >= 96 && palette.size() == 1024;
      if (status.team_colors_ready) {
        status.game_palette = palette;
        for (std::size_t owner = 0; owner < status.team_color_indices.size();
             ++owner) {
          std::copy_n(unit_colors.pixels.begin() + owner * 8U, 8,
                      status.team_color_indices[owner].begin());
        }
      }
    }

    // statwire.cpp::sub_4AC600 at 0x004AC600 loads exactly 228 frames and
    // decodes game\twire.pcx into the 24-byte health/energy color table used
    // by sub_4AC130/sub_4AC210/sub_4AC2B0. Preserve both dependencies; the
    // GRP itself still uses the full active console palette.
    std::vector<std::uint8_t> wireframe_group;
    std::vector<std::uint8_t> group_wireframe_group;
    starcraft::runtime::DecodedPcx wireframe_remap{};
    status.wireframe_ready =
        storm.load_file(R"(unit\wirefram\wirefram.grp)", wireframe_group) &&
        starcraft::gds::grp_frame_count(wireframe_group.data(),
                                        wireframe_group.size()) == 228 &&
        storm.load_pcx(R"(game\twire.pcx)", wireframe_remap) &&
        wireframe_remap.width == 24 && wireframe_remap.height == 1 &&
        decode_preview_frames(
            wireframe_group, ui_palette, status.wireframe_frames,
            status.wireframe_canvas_width, status.wireframe_canvas_height);
    if (status.wireframe_ready) {
      status.wireframe_color_table = wireframe_remap.pixels;
    }
    // statdata.cpp::sub_4A6200 at 0x004A6200 separately loads grpwire.grp
    // for controls 33..44. Those are the compact 12-selection wireframes;
    // the 64-pixel selected-unit control above continues to use wirefram.grp.
    status.group_wireframe_ready =
        storm.load_file(R"(unit\wirefram\grpwire.grp)",
                        group_wireframe_group) &&
        starcraft::gds::grp_frame_count(group_wireframe_group.data(),
                                        group_wireframe_group.size()) != 0 &&
        decode_preview_frames(group_wireframe_group, ui_palette,
                              status.group_wireframe_frames,
                              status.group_wireframe_canvas_width,
                              status.group_wireframe_canvas_height);

    // statdata.cpp::sub_4A6200 loads rez\statdata.bin, while sub_4A4C60 and
    // sub_4A4DA0 load the race-specific empty/full progress strips. Retain
    // the signed control IDs because the status mode dispatcher addresses
    // those exact IDs when switching between ordinary, queue, and build UI.
    std::vector<std::uint8_t> status_layout;
    status.status_panel_ready =
        storm.load_file(R"(rez\statdata.bin)", status_layout) &&
        parse_status_panel_controls(status_layout, status);
    (void)storm.load_file(R"(rez\stat_txt.tbl)", status.stat_text_table);
    std::vector<std::uint8_t> resource_layout;
    std::vector<std::uint8_t> resource_icons;
    status.resource_panel_ready =
        storm.load_file(R"(rez\statres.bin)", resource_layout) &&
        parse_resource_panel_controls(resource_layout, status);
    status.resource_icons_ready =
        storm.load_file(R"(game\icons.grp)", resource_icons) &&
        starcraft::gds::grp_frame_count(resource_icons.data(),
                                        resource_icons.size()) == 12U &&
        decode_preview_frames(resource_icons, ui_palette,
                              status.resource_icon_frames,
                              status.resource_icon_canvas_width,
                              status.resource_icon_canvas_height);
    if (local_race_code != 0) {
      std::string empty_path = "game\\";
      empty_path.push_back(local_race_code);
      empty_path += "pbrempt.pcx";
      std::string full_path = "game\\";
      full_path.push_back(local_race_code);
      full_path += "pbrfull.pcx";
      starcraft::runtime::DecodedPcx empty_progress{};
      starcraft::runtime::DecodedPcx full_progress{};
      status.status_progress_art_ready =
          storm.load_pcx(empty_path.c_str(), empty_progress) &&
          storm.load_pcx(full_path.c_str(), full_progress) &&
          decode_pcx_frame(empty_progress, false,
                           status.status_progress_empty) &&
          decode_pcx_frame(full_progress, false, status.status_progress_full);
    }

    // statcmd.cpp::sub_4A46B0 at 0x004A46B0 loads the race button GRP,
    // the 363-frame cmdicons.grp, the 96-byte race icon color table, and the
    // linked rez\statbtn%c.bin DLG layout. Parse the nine original rectangles
    // rather than inferring them from the console bitmap.
    if (local_race_code != 0) {
      std::string panel_path = R"(unit\cmdbtns\)";
      panel_path.push_back(local_race_code);
      panel_path += "cmdbtns.grp";
      std::string layout_path = R"(rez\statbtn)";
      layout_path.push_back(local_race_code);
      layout_path += ".bin";
      std::vector<std::uint8_t> panel_group;
      std::vector<std::uint8_t> icon_group;
      std::vector<std::uint8_t> layout;
      starcraft::runtime::DecodedPcx icon_remap{};
      std::vector<std::uint8_t> normal_icon_remap;
      std::uint16_t panel_width{};
      std::uint16_t panel_height{};
      std::uint16_t icon_width{};
      std::uint16_t icon_height{};
      const bool icon_remap_loaded =
          // statcmd.cpp::sub_4A46B0 at 0x004A46B0 varies the panel GRP and
          // statbtn layout by race, but loads the literal shared path
          // unit\cmdbtns\ticon.pcx for the 96-byte icon color table.
          storm.load_pcx(R"(unit\cmdbtns\ticon.pcx)", icon_remap) &&
          icon_remap.width == 96 && icon_remap.height == 1;
      if (icon_remap_loaded) {
        normal_icon_remap.assign(icon_remap.pixels.begin(),
                                 icon_remap.pixels.begin() + 16);
      }
      status.command_panel_ready =
          storm.load_file(panel_path.c_str(), panel_group) &&
          storm.load_file(R"(unit\cmdbtns\cmdicons.grp)", icon_group) &&
          storm.load_file(layout_path.c_str(), layout) && icon_remap_loaded &&
          decode_preview_frames(panel_group, ui_palette,
                                status.command_panel_frames, panel_width,
                                panel_height) &&
          decode_preview_frames(icon_group, ui_palette,
                                status.command_icon_frames, icon_width,
                                icon_height, &normal_icon_remap) &&
          status.command_icon_frames.size() == 363 &&
          parse_command_controls(layout, status.command_controls);
    }

    // statport.cpp::sub_4AB0D0 at 0x004AB0D0 looks up control 1 from
    // rez\statport.bin and uses its exact rectangle as the Smacker surface.
    std::vector<std::uint8_t> portrait_layout;
    status.portrait_panel_ready =
        storm.load_file(R"(rez\statport.bin)", portrait_layout) &&
        parse_dialog_control(portrait_layout, 1, status.portrait_control);

    std::vector<std::uint8_t> minimap_layout;
    status.minimap_ready =
        storm.load_file(R"(rez\minimap.bin)", minimap_layout) &&
        parse_dialog_control(minimap_layout, 1, status.minimap_control);

    if (data_loaded) {
      const std::uint32_t view_left = status.camera_x;
      const std::uint32_t view_top = status.camera_y;
      for (const starcraft::game::ScenarioUnit &unit : scenario.units()) {
        std::uint16_t unit_image{};
        if (unit.owner == status.local_player && unit.x >= view_left &&
            unit.x < view_left + kMapViewportWidth && unit.y >= view_top &&
            unit.y < view_top + kMapViewportHeight &&
            data.unit_image_id(unit.unit_type, unit_image)) {
          status.image_id = unit_image;
          focus_unit_found = true;
          break;
        }
      }
    }
  }

  const bool palette_loaded = palette.size() == 1024;
  bool focus_asset_ready{};
  bool scv_asset_ready{};
  bool geyser_asset_ready{};
  bool geyser_smoke_assets_ready{};
  bool cargo_assets_ready{};
  bool worker_mining_effects_ready{};
  bool working_overlay_asset_ready{};
  bool protoss_construction_assets_ready{};
  bool build_assets_ready{};
  bool production_assets_ready{};
  bool portrait_assets_ready{};
  bool melee_start_ready{};
  std::string protoss_asset_failure;
  if (data_loaded && palette_loaded && scenario.valid()) {
    status.iscript_bytes = data.iscript();
    const starcraft::data::DatField *const sprite_image_table =
        data.sprites().field(0U);
    bool sprite_images_ready = sprite_image_table != nullptr;
    for (std::size_t sprite = 0U;
         sprite_images_ready && sprite < status.sprite_image_ids.size();
         ++sprite) {
      sprite_images_ready = sprite_image_table->value(
          sprite, status.sprite_image_ids[sprite]);
    }
    status.unit_assets.reserve(64);
    const auto ensure_asset = [&](const std::uint16_t image_id) -> std::size_t {
      const auto existing =
          std::find_if(status.unit_assets.begin(), status.unit_assets.end(),
                       [image_id](const UnitRenderAsset &asset) {
                         return asset.image_id == image_id;
                       });
      if (existing != status.unit_assets.end()) {
        return static_cast<std::size_t>(existing - status.unit_assets.begin());
      }
      UnitRenderAsset asset{};
      if (!load_unit_render_asset(storm, data, palette, status.iscript_bytes,
                                  image_id, scenario.tileset_id(), asset)) {
        return SIZE_MAX;
      }
      status.unit_assets.push_back(std::move(asset));
      return status.unit_assets.size() - 1U;
    };

    const auto ensure_runtime_unit_type =
        [&](const std::uint16_t unit_type) -> RuntimeUnitType * {
      if (unit_type >= status.runtime_unit_types.size()) {
        return nullptr;
      }
      RuntimeUnitType &runtime = status.runtime_unit_types[unit_type];
      if (runtime.ready) {
        return &runtime;
      }
      starcraft::lang::UnitInitializationData initialization{};
      if (!starcraft::lang::recover_unit_initialization(data, unit_type,
                                                        initialization)) {
        return nullptr;
      }
      const std::size_t asset_index = ensure_asset(initialization.image_id);
      if (asset_index == SIZE_MAX) {
        return nullptr;
      }
      const std::size_t selection_circle_asset_index =
          initialization.has_selection_circle
              ? ensure_asset(initialization.selection_circle_image_id)
              : SIZE_MAX;
      if (initialization.has_selection_circle &&
          selection_circle_asset_index == SIZE_MAX) {
        return nullptr;
      }
      runtime.initialization = initialization;
      runtime.asset_index = asset_index;
      runtime.selection_circle_asset_index = selection_circle_asset_index;
      runtime.ready = true;
      return &runtime;
    };

    // CBullet.cpp::sub_402940 follows weapons.dat[1] through flingy.dat and
    // sprites.dat to create the projectile image. Cache every nonzero weapon
    // graphic while the licensed archives are open so any race's attack
    // IScript can materialize its real projectile at runtime.
    for (std::uint16_t weapon = 0U;
         weapon < status.weapon_asset_indices.size(); ++weapon) {
      std::uint16_t projectile_image{};
      if (data.weapon_image_id(weapon, projectile_image)) {
        status.weapon_asset_indices[weapon] = ensure_asset(projectile_image);
      }
    }

    // CSprite.cpp::sub_41CC30/sub_41CD30 attach the light and heavy building
    // damage images 422..443 and 444..465. Keep them resident while Storm is
    // open; the damage transition itself is driven by images.dat field 11.
    for (std::uint16_t image = 422U; image <= 465U; ++image) {
      (void)ensure_asset(image);
    }

    if (!focus_unit_found &&
        scenario.players()[status.local_player].ownership != 0U) {
      // A melee CHK is allowed to contain only start-location markers and
      // resources. CUnitInit/place_unit then creates the local race's base
      // and four workers. Do not make map launch depend on an unrelated
      // preplaced owner-zero unit happening to lie inside the first camera
      // rectangle (96x96_ash4 has no such unit).
      starcraft::lang::MeleeUnitTypes local_types{};
      focus_unit_found = starcraft::lang::melee_unit_types(
                             scenario.players()[status.local_player].race,
                             local_types) &&
                         data.unit_image_id(local_types.worker,
                                            status.image_id);
    }

    if (focus_unit_found) {
      status.focus_asset_index = ensure_asset(status.image_id);
      focus_asset_ready = status.focus_asset_index != SIZE_MAX;
      if (focus_asset_ready) {
        const UnitRenderAsset &focus =
            status.unit_assets[status.focus_asset_index];
        status.image_draw_function = focus.image_draw_function;
        status.iscript_id = focus.iscript_id;
      }
    }

    const auto append_unit_preview =
        [&](const std::uint16_t unit_type, const std::uint16_t x,
            const std::uint16_t y, const std::uint8_t owner) -> bool {
      RuntimeUnitType *const runtime = ensure_runtime_unit_type(unit_type);
      if (runtime == nullptr ||
          runtime->asset_index >= status.unit_assets.size()) {
        return false;
      }
      const starcraft::lang::UnitInitializationData &initialization =
          runtime->initialization;
      const UnitRenderAsset &asset = status.unit_assets[runtime->asset_index];
      ScenarioUnitPreview preview{};
      preview.x = x;
      preview.y = y;
      preview.unit_type = unit_type;
      preview.owner = owner;
      preview.asset_index = runtime->asset_index;
      preview.selection_circle_asset_index =
          runtime->selection_circle_asset_index;
      preview.selection_circle_y_offset =
          initialization.selection_circle_y_offset;
      preview.selection_width = initialization.placement_width;
      preview.selection_height = initialization.placement_height;
      preview.collision_left =
          static_cast<std::uint16_t>(initialization.collision.left);
      preview.collision_top =
          static_cast<std::uint16_t>(initialization.collision.top);
      preview.collision_right =
          static_cast<std::uint16_t>(initialization.collision.right);
      preview.collision_bottom =
          static_cast<std::uint16_t>(initialization.collision.bottom);
      preview.iscript_state = asset.initial_iscript_state;
      preview.overlay_iscript_state = asset.initial_overlay_iscript_state;
      preview.current_sprite_frame = asset.initial_iscript_state.frame;
      preview.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
      preview.x_fixed = static_cast<std::int32_t>(preview.x) << 8U;
      preview.y_fixed = static_cast<std::int32_t>(preview.y) << 8U;
      preview.iscript_ready = asset.iscript_ready;
      preview.overlay_ready = asset.overlay_ready;
      preview.is_building = initialization.is_building;
      preview.unit_id = status.next_unit_id++;
      apply_initialization_traits(preview, initialization);
      initialize_unit_energy(status, preview);
      if (preview.is_building) {
        // CUnitInit.cpp::sub_42EBB0 at 0x0042EBB0 dispatches animation 16
        // when a completed building is initialized.
        (void)restart_unit_animation(status, preview, 16U);
      }
      status.units.push_back(std::move(preview));
      return true;
    };

    for (int owner_pass = 0; owner_pass < 2; ++owner_pass) {
      for (const starcraft::game::ScenarioUnit &unit : scenario.units()) {
        if ((owner_pass == 0 && unit.owner != 0) ||
            (owner_pass == 1 && unit.owner == 0)) {
          continue;
        }
        // The supplied beta melee maps include one base and one worker for
        // each slot's initial race. place_unit.cpp::sub_480B60/sub_480C80
        // derive the runtime start from the lobby race instead. When a slot
        // changes race, discard only those stale starting identities before
        // the recovered melee plan supplies the selected race's base/workers.
        if (unit.owner < 8U &&
            starcraft::lang::is_melee_starting_unit_type(unit.unit_type) &&
            (randomized_melee_starts ||
             !starcraft::lang::melee_starting_unit_matches_race(
                 unit.unit_type, scenario.players()[unit.owner].race))) {
          continue;
        }
        (void)append_unit_preview(unit.unit_type, unit.x, unit.y, unit.owner);
      }
    }

    std::array<starcraft::lang::MeleeBaseDimensions, 3> base_dimensions{};
    melee_start_ready = true;
    for (std::uint8_t race = 0; race < base_dimensions.size(); ++race) {
      starcraft::lang::MeleeUnitTypes types{};
      melee_start_ready =
          starcraft::lang::melee_unit_types(race, types) &&
          data.unit_placement_size(types.base, base_dimensions[race].width,
                                   base_dimensions[race].height) &&
          melee_start_ready;
    }
    std::vector<starcraft::lang::ExistingMeleeUnit> existing_melee_units;
    existing_melee_units.reserve(status.units.size());
    for (const ScenarioUnitPreview &unit : status.units) {
      existing_melee_units.push_back({unit.unit_type, unit.owner, unit.alive});
    }
    std::vector<starcraft::lang::MeleeStartSpawn> melee_spawns;
    melee_start_ready =
        melee_start_ready &&
        starcraft::lang::build_retail_melee_start_plan(
            scenario.players(), scenario.start_locations(),
            existing_melee_units.data(), existing_melee_units.size(),
            base_dimensions, melee_spawns);
    for (const starcraft::lang::MeleeStartSpawn &spawn : melee_spawns) {
      melee_start_ready =
          append_unit_preview(spawn.unit_type, spawn.x, spawn.y, spawn.owner) &&
          melee_start_ready;
    }

    // Recover every unit type reachable from the original building/larva
    // command records while the licensed Storm archives are still open.
    production_assets_ready =
        ensure_runtime_unit_type(starcraft::lang::zerg_larva_type) != nullptr &&
        ensure_runtime_unit_type(starcraft::lang::zerg_egg_type) != nullptr;
    if (!production_assets_ready) {
      status.failed_runtime_unit_type = starcraft::lang::zerg_larva_type;
    }
    const starcraft::lang::UnitProductionProducerView production_producers =
        starcraft::lang::production_producer_types();
    for (std::size_t producer_index = 0;
         producer_index < production_producers.count; ++producer_index) {
      const std::uint16_t producer_type =
          production_producers.unit_types[producer_index];
      if (ensure_runtime_unit_type(producer_type) == nullptr) {
        status.failed_runtime_unit_type = producer_type;
        production_assets_ready = false;
      }
      const starcraft::lang::UnitProductionButtonView buttons =
          starcraft::lang::production_buttons_for(producer_type);
      for (std::size_t index = 0; index < buttons.count; ++index) {
        if (ensure_runtime_unit_type(buttons.buttons[index].product_type) ==
            nullptr) {
          status.failed_runtime_unit_type = buttons.buttons[index].product_type;
          production_assets_ready = false;
        }
      }
    }
    // netcmd.cpp::sub_4762B0 and CUnitProtoss.cpp::sub_43C5B0 transform a
    // reciprocal pair of type-67 Templars directly into type 68.  Because the
    // result has no ordinary production-card entry, cache it explicitly while
    // the read-only archives are open.
    if (ensure_runtime_unit_type(68U) == nullptr) {
      status.failed_runtime_unit_type = 68U;
      production_assets_ready = false;
    }
    // CUnitZerg.cpp::sub_448940 and sub_4495C0 spawn larva for each of the
    // three Zerg town-hall stages. They are part of the production runtime
    // even though the actual morph card belongs to the child larva CUnit.
    const starcraft::lang::ZergLarvaSourceView larva_sources =
        starcraft::lang::zerg_larva_source_types();
    for (std::size_t source_index = 0; source_index < larva_sources.count;
         ++source_index) {
      const std::uint16_t town_hall_type =
          larva_sources.unit_types[source_index];
      if (ensure_runtime_unit_type(town_hall_type) == nullptr) {
        status.failed_runtime_unit_type = town_hall_type;
        production_assets_ready = false;
      }
    }
    const starcraft::lang::ZergUnitTypeView morph_targets =
        starcraft::lang::zerg_building_morph_target_types();
    for (std::size_t target_index = 0; target_index < morph_targets.count;
         ++target_index) {
      const std::uint16_t target_type = morph_targets.unit_types[target_index];
      if (ensure_runtime_unit_type(target_type) == nullptr) {
        status.failed_runtime_unit_type = target_type;
        production_assets_ready = false;
      }
    }

    std::uint16_t scv_image{};
    std::uint16_t scv_build_time{};
    if (data.unit_image_id(7, scv_image)) {
      status.scv_asset_index = ensure_asset(scv_image);
      scv_asset_ready =
          status.scv_asset_index != SIZE_MAX &&
          data.unit_placement_size(7, status.scv_selection_width,
                                   status.scv_selection_height) &&
          data.unit_build_time(7, scv_build_time) &&
          starcraft::lang::production_total_ticks(scv_build_time) != 0U &&
          data.unit_movement_traits(7, status.scv_movement) &&
          data.unit_simulation_traits(7, status.scv_simulation);
    }
    std::uint16_t geyser_image{};
    if (data.unit_image_id(188, geyser_image)) {
      status.geyser_asset_index = ensure_asset(geyser_image);
      geyser_asset_ready =
          status.geyser_asset_index != SIZE_MAX &&
          data.unit_placement_size(188, status.geyser_selection_width,
                                   status.geyser_selection_height) &&
          data.unit_simulation_traits(188, status.geyser_simulation);
    }
    geyser_smoke_assets_ready = geyser_asset_ready;
    for (std::uint16_t image = 402U; image <= 411U; ++image) {
      geyser_smoke_assets_ready =
          ensure_asset(image) != SIZE_MAX && geyser_smoke_assets_ready;
    }
    if (geyser_smoke_assets_ready) {
      const UnitRenderAsset &geyser_asset =
          status.unit_assets[status.geyser_asset_index];
      geyser_smoke_assets_ready =
          !geyser_asset.special_overlay_path.empty() &&
          geyser_asset.special_overlay_frame_count != 0U &&
          geyser_asset.special_overlay_point_count >= 3U &&
          !geyser_asset.special_overlay_points.empty();
    }
    // CUnitHarvest.cpp::sub_42D3C0 passes source inventory types 220 and 222
    // to CUnitInv.cpp::sub_430FB0. That function adds 137 to obtain the
    // attached image IDs for carried minerals and Terran gas.
    status.mineral_cargo_asset_index = ensure_asset(220U + 137U);
    status.terran_gas_cargo_asset_index = ensure_asset(222U + 137U);
    cargo_assets_ready = status.mineral_cargo_asset_index != SIZE_MAX &&
                         status.terran_gas_cargo_asset_index != SIZE_MAX;
    std::uint16_t worker_effect_image{};
    if (data.weapon_image_id(8U, worker_effect_image)) {
      status.scv_mining_effect_asset_index = ensure_asset(worker_effect_image);
    }
    if (data.weapon_image_id(42U, worker_effect_image)) {
      status.probe_mining_effect_asset_index =
          ensure_asset(worker_effect_image);
    }
    worker_mining_effects_ready =
        status.scv_mining_effect_asset_index != SIZE_MAX &&
        status.probe_mining_effect_asset_index != SIZE_MAX;

    // CUnitBuild.cpp::sub_423020 dispatches Working (19) to the producer.
    // The Command Center image-246 script creates image 247 at that point;
    // its ordinary idle action instead creates the unrelated image 248.
    // Cache 247 independently so action-created attachments can be rendered.
    status.command_center_working_asset_index = ensure_asset(247U);
    working_overlay_asset_ready =
        status.command_center_working_asset_index != SIZE_MAX;

    // CUnitZBuild.cpp::sub_4475E0 attaches image 314 only to the new
    // Extractor CUnit after the Drone is consumed on a Vespene geyser.
    status.zerg_extractor_construction_asset_index = ensure_asset(314U);

    // CUnitPBuild.cpp::sub_43BBF0 attaches image 189 when the Probe reaches
    // the accepted footprint. CUnitProtoss.cpp::sub_43CF60 creates sprite
    // 198 for the same licensed power-field shape used by sub_43C200.
    std::uint16_t pylon_power_image{};
    const starcraft::data::DatField *const sprite_images =
        data.sprites().field(0);
    status.protoss_warp_asset_index = ensure_asset(189U);
    // Image 188 is inserted during the later warp-in state transition; keep
    // it available to restart_unit_animation's dynamic attachment path.
    status.protoss_materialize_asset_index = ensure_asset(188U);
    status.pylon_power_asset_index =
        sprite_images != nullptr &&
                sprite_images->value(198U, pylon_power_image)
            ? ensure_asset(pylon_power_image)
            : SIZE_MAX;
    // CUnitPSpells.cpp and CBullet.cpp bind these exact licensed effects:
    // weapon 56 Psionic Storm, weapon 55 Stasis Field, sprite 265 Recall,
    // and image 516 on a newly created Hallucination.
    std::uint16_t spell_image{};
    if (data.weapon_image_id(56U, spell_image)) {
      status.psionic_storm_asset_index = ensure_asset(spell_image);
    }
    if (data.weapon_image_id(55U, spell_image)) {
      status.stasis_field_asset_index = ensure_asset(spell_image);
    }
    status.recall_asset_index =
        sprite_images != nullptr && sprite_images->value(265U, spell_image)
            ? ensure_asset(spell_image)
            : SIZE_MAX;
    status.hallucination_asset_index = ensure_asset(516U);
    protoss_construction_assets_ready =
        status.protoss_warp_asset_index != SIZE_MAX &&
        status.protoss_materialize_asset_index != SIZE_MAX &&
        status.pylon_power_asset_index != SIZE_MAX;
    if (!protoss_construction_assets_ready) {
      starcraft::data::ImageRenderTraits warp_traits{};
      starcraft::data::ImageRenderTraits materialize_traits{};
      starcraft::data::ImageRenderTraits power_traits{};
      (void)data.image_render_traits(189U, warp_traits);
      (void)data.image_render_traits(188U, materialize_traits);
      (void)data.image_render_traits(pylon_power_image, power_traits);
      protoss_asset_failure =
          "Protoss assets 189/188/power: " +
          std::to_string(status.protoss_warp_asset_index) + "/" +
          std::to_string(status.protoss_materialize_asset_index) + "/" +
          std::to_string(status.pylon_power_asset_index) +
          "; sprite 198 image " + std::to_string(pylon_power_image) +
          "; draw " + std::to_string(warp_traits.draw_function) + "/" +
          std::to_string(materialize_traits.draw_function) + "/" +
          std::to_string(power_traits.draw_function) + ".";
    }

    build_assets_ready = true;
    const starcraft::lang::TerranUnitTypeView terran_buildables =
        starcraft::lang::terran_buildable_unit_types();
    const starcraft::lang::ZergUnitTypeView zerg_buildables =
        starcraft::lang::zerg_buildable_unit_types();
    const starcraft::lang::ProtossUnitTypeView protoss_buildables =
        starcraft::lang::protoss_buildable_unit_types();
    status.buildable_units.clear();
    status.buildable_units.reserve(terran_buildables.count +
                                   zerg_buildables.count +
                                   protoss_buildables.count);
    const auto cache_buildable = [&](const std::uint16_t unit_type) -> bool {
      BuildableUnitVisual buildable{};
      buildable.unit_type = unit_type;
      std::uint16_t image_id{};
      if (!data.unit_placement_size(unit_type, buildable.placement_width,
                                    buildable.placement_height) ||
          !data.unit_image_id(unit_type, image_id)) {
        return false;
      }
      buildable.asset_index = ensure_asset(image_id);
      if (buildable.asset_index == SIZE_MAX) {
        return false;
      }
      std::uint16_t construction_image_id{};
      if (data.unit_construction_image_id(unit_type,
                                          construction_image_id)) {
        buildable.construction_asset_index =
            ensure_asset(construction_image_id);
        if (buildable.construction_asset_index == SIZE_MAX) {
          return false;
        }
      }
      if (!data.unit_simulation_traits(unit_type, buildable.simulation) ||
          ensure_runtime_unit_type(unit_type) == nullptr) {
        status.failed_runtime_unit_type = unit_type;
        return false;
      }
      if ((buildable.simulation.dat_flags & 2U) != 0U) {
        buildable.addon_parent_type =
            starcraft::lang::terran_addon_parent_type(unit_type);
        if (buildable.addon_parent_type == 0xFFFFU ||
            !data.unit_addon_position(unit_type, buildable.addon_x,
                                      buildable.addon_y)) {
          return false;
        }
      }
      status.buildable_units.push_back(buildable);
      return true;
    };
    const auto cache_buildable_view = [&](const auto view) {
      for (std::size_t index = 0; index < view.count; ++index) {
        if (!cache_buildable(view.unit_types[index])) {
          return false;
        }
      }
      return true;
    };
    build_assets_ready = cache_buildable_view(terran_buildables) &&
                         cache_buildable_view(zerg_buildables) &&
                         cache_buildable_view(protoss_buildables);
    // CUnitPBuild.cpp::sub_43BDF0 phase two dispatches action 21 to the
    // restored primary image. Those scripts create a building-specific child
    // image (Nexus creates image 160, for example), and that child delivers
    // the final opcode-0x27 bit-1 signal. Cache every such child while the
    // read-only MPQs are open so restart_unit_animation can attach it.
    if (build_assets_ready) {
      const starcraft::lang::IScriptProgramView program{
          status.iscript_bytes.data(), status.iscript_bytes.size()};
      for (std::size_t index = 0; index < protoss_buildables.count; ++index) {
        const std::uint16_t unit_type = protoss_buildables.unit_types[index];
        const RuntimeUnitType &runtime = status.runtime_unit_types[unit_type];
        if (!runtime.ready || runtime.asset_index >= status.unit_assets.size()) {
          build_assets_ready = false;
          break;
        }
        const std::uint16_t script_id =
            status.unit_assets[runtime.asset_index].iscript_id;
        starcraft::lang::IScriptState completion{};
        if (!program.start(script_id, 21U, completion)) {
          build_assets_ready = false;
          break;
        }
        const auto result = program.tick(
            completion, 0U, 256U, nullptr, scenario.tileset_id());
        if ((result != starcraft::lang::IScriptTickResult::yielded &&
             result != starcraft::lang::IScriptTickResult::sleeping) ||
            completion.overlay_event_count == 0U ||
            ensure_asset(completion.overlay_image) == SIZE_MAX) {
          build_assets_ready = false;
          break;
        }
      }
    }
    if (!build_assets_ready) {
      status.buildable_units.clear();
    }

    // CUnitInit.cpp::sub_42EBB0 dispatches action 16 for every completed
    // building. That action can attach a building-specific idle image (the
    // Nexus creates image 161), distinct from both its static action-zero
    // shadow and its construction images. Discover those action events from
    // the licensed IScript and cache their GRPs before the MPQs are closed.
    std::vector<std::uint16_t> building_runtime_overlay_images;
    std::vector<std::uint16_t> building_runtime_sprite_images;
    {
      const starcraft::lang::IScriptProgramView program{
          status.iscript_bytes.data(), status.iscript_bytes.size()};
      for (const RuntimeUnitType &runtime : status.runtime_unit_types) {
        if (!runtime.ready || !runtime.initialization.is_building ||
            runtime.asset_index >= status.unit_assets.size()) {
          continue;
        }
        const UnitRenderAsset &building_asset =
            status.unit_assets[runtime.asset_index];
        // Action 1 is the recovered building-death entry and action 16 is
        // completed-building idle. Both attach licensed effect images which
        // must be cached before Storm closes the archives.
        for (const std::uint8_t action : {1U, 16U}) {
          starcraft::lang::IScriptState script{};
          if (!program.start(building_asset.iscript_id, action, script)) {
            continue;
          }
          std::uint32_t overlay_events{};
          std::uint32_t sprite_events{};
          for (std::size_t tick = 0U; tick < 128U; ++tick) {
            const auto result = program.tick(
                script, static_cast<std::uint32_t>(tick), 256U, nullptr,
                scenario.tileset_id());
            if (script.overlay_event_count != overlay_events) {
              overlay_events = script.overlay_event_count;
              if (script.overlay_image != building_asset.overlay_image_id &&
                  std::find(building_runtime_overlay_images.begin(),
                            building_runtime_overlay_images.end(),
                            script.overlay_image) ==
                      building_runtime_overlay_images.end()) {
                building_runtime_overlay_images.push_back(script.overlay_image);
              }
            }
            if (script.sprite_event_count != sprite_events) {
              sprite_events = script.sprite_event_count;
              if (script.sprite_id < status.sprite_image_ids.size()) {
                const std::uint16_t image =
                    status.sprite_image_ids[script.sprite_id];
                if (std::find(building_runtime_sprite_images.begin(),
                              building_runtime_sprite_images.end(), image) ==
                    building_runtime_sprite_images.end()) {
                  building_runtime_sprite_images.push_back(image);
                }
              }
            }
            if (result == starcraft::lang::IScriptTickResult::ended ||
                result ==
                    starcraft::lang::IScriptTickResult::unsupported_opcode ||
                result ==
                    starcraft::lang::IScriptTickResult::malformed_program ||
                result ==
                    starcraft::lang::IScriptTickResult::instruction_limit) {
              break;
            }
          }
        }
      }
    }
    for (const std::uint16_t image : building_runtime_overlay_images) {
      build_assets_ready = ensure_asset(image) != SIZE_MAX &&
                           build_assets_ready;
    }
    for (const std::uint16_t image : building_runtime_sprite_images) {
      build_assets_ready = ensure_asset(image) != SIZE_MAX &&
                           build_assets_ready;
    }
    build_assets_ready = sprite_images_ready && build_assets_ready;
    if (!build_assets_ready) {
      status.buildable_units.clear();
    }

    const auto ensure_portrait = [&](const std::uint16_t unit_type,
                                     const std::uint8_t owner) -> bool {
      const auto existing = std::find_if(
          status.portraits.begin(), status.portraits.end(),
          [unit_type, owner](const UnitPortraitAsset &asset) {
            return asset.unit_type == unit_type && asset.owner == owner;
          });
      if (existing != status.portraits.end()) {
        return existing->portrait.ready;
      }
      UnitPortraitAsset asset{};
      asset.unit_type = unit_type;
      asset.owner = owner;
      if (!load_unit_portrait(storm, data, unit_type, owner, asset.portrait)) {
        return false;
      }
      status.portraits.push_back(std::move(asset));
      return true;
    };
    // Require the two presently playable production/worker portraits and
    // opportunistically cache every supported scenario/build-card portrait
    // before the read-only Storm archives are closed.
    portrait_assets_ready = ensure_portrait(7, 0) && ensure_portrait(106, 0);
    for (const ScenarioUnitPreview &unit : status.units) {
      (void)ensure_portrait(unit.unit_type, unit.owner);
    }
    for (const BuildableUnitVisual &buildable : status.buildable_units) {
      (void)ensure_portrait(buildable.unit_type, 0);
    }
    for (std::size_t type = 0; type < status.runtime_unit_types.size();
         ++type) {
      if (status.runtime_unit_types[type].ready) {
        (void)ensure_portrait(static_cast<std::uint16_t>(type), 0);
      }
    }

    std::array<bool, starcraft::lang::kUnitTypeCount> wanted_sound_types{};
    wanted_sound_types[7] = true;
    wanted_sound_types[106] = true;
    for (const ScenarioUnitPreview &unit : status.units) {
      if (unit.unit_type < wanted_sound_types.size()) {
        wanted_sound_types[unit.unit_type] = true;
      }
    }
    for (const BuildableUnitVisual &buildable : status.buildable_units) {
      if (buildable.unit_type < wanted_sound_types.size()) {
        wanted_sound_types[buildable.unit_type] = true;
      }
    }
    for (std::size_t type = 0U; type < status.runtime_unit_types.size();
         ++type) {
      if (status.runtime_unit_types[type].ready) {
        wanted_sound_types[type] = true;
      }
    }
    std::vector<std::uint8_t> sfx_data;
    std::vector<std::uint8_t> sfx_table;
    status.unit_sounds_ready =
        unit_sound_ranges_ready &&
        storm.load_file(R"(arr\sfxdata.dat)", sfx_data) &&
        storm.load_file(R"(arr\sfxdata.tbl)", sfx_table) &&
        cache_unit_sound_assets(storm, sfx_data, sfx_table, wanted_sound_types,
                                status);
  }

  bool ai_runtime_ready = true;
  std::string ai_runtime_error{};
  if (scenario.valid() && terrain_tileset.valid()) {
    status.minimap_ready =
        status.minimap_ready &&
        build_minimap_preview(
            terrain_tileset, scenario, status.minimap, status.minimap_content_x,
            status.minimap_content_y, status.minimap_content_width,
            status.minimap_content_height);
    (void)status.pathing_map.build(terrain_tileset, scenario);
    status.scenario = std::move(scenario);
    status.terrain_tileset = std::move(terrain_tileset);
    // player_minerals/player_gas remain the recovered slot-zero aliases used
    // by simulation code. Every human starts with the same canonical melee
    // stock; the HUD chooses the local assigned slot when it renders.
    for (std::size_t player = 0U; player < 8U; ++player) {
      if (status.scenario.players()[player].ownership == 6U) {
        status.player_mineral_stock[player] = status.player_minerals;
        status.player_gas_stock[player] = status.player_gas;
      }
    }
    melee_start_ready =
        settle_melee_starting_workers(status) && melee_start_ready;
    (void)rebuild_creep_tiles(status);
    const bool fog_ready = initialize_fog_of_war(status);
    (void)initialize_ai_players(status);
    for (const AiPlayerRuntime &ai : status.ai_players) {
      if (!ai.enabled) continue;
      const bool controller_ready =
          ai.controller != nullptr && ai.controller->ready();
      ai_runtime_ready = ai_runtime_ready && controller_ready;
      if (!controller_ready && ai_runtime_error.empty()) {
        ai_runtime_error = ai.script_error;
      }
    }
    melee_start_ready = melee_start_ready && fog_ready;
  }

  const bool assets_closed = asset_archives.close(storm);
  status.assets_ready =
      map_loaded && scenario_loaded && data_loaded && focus_unit_found &&
      focus_asset_ready && scv_asset_ready && geyser_asset_ready &&
      geyser_smoke_assets_ready && palette_loaded &&
      cargo_assets_ready && worker_mining_effects_ready &&
      working_overlay_asset_ready && build_assets_ready &&
      protoss_construction_assets_ready &&
      production_assets_ready && portrait_assets_ready && melee_start_ready &&
      research_data_ready && ai_runtime_ready &&
      status.terrain_ready && status.hud_ready && status.wireframe_ready &&
      status.group_wireframe_ready && status.status_panel_ready &&
      status.resource_panel_ready && status.resource_icons_ready &&
      status.unit_traits_ready && status.status_progress_art_ready &&
      !status.stat_text_table.empty() &&
      !status.resource_error_waves[0].empty() &&
      !status.resource_error_waves[1].empty() && status.unit_sounds_ready &&
      status.music_available && status.command_panel_ready &&
      status.portrait_panel_ready && status.minimap_ready &&
      status.team_colors_ready && status.pathing_map.valid() &&
      status.selection_colors_ready &&
      status.terrain_dark_levels.size() == 32U * 256U &&
      status.fog_map_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.fog_render_surfaces_ready &&
      status.creep_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.creep_visual_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.creep_edge_frames.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.scenario.valid() && status.active_player_count >= 2 &&
      !status.units.empty() && map_closed && assets_closed;
  if (status.assets_ready) {
    char detail[300]{};
    std::snprintf(detail, sizeof(detail),
                  "%s: %ux%u %s, %zu players, %zu units/%zu THGY sprites; "
                  "camera %u,%u px; %s; image %u/script %u/draw %u.",
                  status.map_name.c_str(), status.scenario_width,
                  status.scenario_height, status.tileset_name.c_str(),
                  status.active_player_count, status.scenario_unit_count,
                  status.scenario_sprite_count,
                  status.camera_x, status.camera_y, status.hud_path.c_str(),
                  status.image_id, status.iscript_id,
                  status.image_draw_function);
    status.primary =
        "A supplied StarCraft multiplayer map is loaded and rendered.";
    status.detail = detail;
  } else {
    status.primary =
        "The supplied map or its recovered render path did not initialize.";
    if (!map_opened) {
      status.detail = "Could not open " + status.map_name +
                      " as a read-only Storm archive.";
    } else if (!scenario_loaded) {
      status.detail =
          "Could not parse the supported CHK sections from "
          "staredit\\scenario.chk.";
    } else if (!data_loaded) {
      status.detail = "Failed DAT/TBL asset: " + data.failed_asset();
    } else if (!focus_unit_found) {
      status.detail = "No local melee CUnit image could be selected.";
    } else if (!focus_asset_ready) {
      status.detail = "The selected local CUnit image did not decode.";
    } else if (!scv_asset_ready) {
      status.detail = "The SCV runtime asset did not initialize.";
    } else if (!geyser_asset_ready || !geyser_smoke_assets_ready) {
      status.detail =
          "The neutral geyser or its resource plume assets did not initialize.";
    } else if (!palette_loaded) {
      status.detail = "The selected ERA palette did not load.";
    } else if (!cargo_assets_ready || !worker_mining_effects_ready ||
               !working_overlay_asset_ready) {
      status.detail =
          "A worker cargo, mining effect, or building-working image failed.";
    } else if (!protoss_construction_assets_ready) {
      status.detail = protoss_asset_failure;
    } else if (!build_assets_ready || !production_assets_ready) {
      status.detail = "A production or building runtime asset failed.";
    } else if (!portrait_assets_ready) {
      status.detail = "A required unit portrait did not decode.";
    } else if (!melee_start_ready) {
      status.detail =
          "The recovered melee base/worker placement could not settle.";
    } else if (!ai_runtime_ready) {
      status.detail = "Lua AI initialization failed: " + ai_runtime_error;
    } else if (!status.terrain_ready) {
      status.detail = "The selected ERA terrain did not render.";
    } else if (!status.hud_ready) {
      status.detail = "The selected race console PCX did not decode.";
    } else if (!status.selection_colors_ready) {
      status.detail = "game\\tselect.pcx did not decode.";
    } else if (status.terrain_dark_levels.size() != 32U * 256U ||
               !status.fog_render_surfaces_ready) {
      status.detail = "The recovered dark.pcx fog mask did not initialize.";
    } else if (!status.wireframe_ready || !status.group_wireframe_ready) {
      status.detail = "A selected-unit wireframe asset did not decode.";
    } else if (!status.status_panel_ready || !status.resource_panel_ready ||
               !status.resource_icons_ready ||
               !status.status_progress_art_ready) {
      status.detail = "A selected-race status panel asset did not decode.";
    } else if (!status.unit_traits_ready || status.stat_text_table.empty()) {
      status.detail = "A unit trait or status text table did not load.";
    } else if (status.resource_error_waves[0].empty() ||
               status.resource_error_waves[1].empty()) {
      status.detail = "A selected-race advisor sound did not load.";
    } else if (!status.unit_sounds_ready) {
      status.detail = "A required unit response sound did not load.";
    } else if (!status.music_available) {
      status.detail = "The selected-race loose music track did not load.";
    } else if (!status.command_panel_ready) {
      status.detail = "The selected-race command panel did not decode.";
    } else if (!status.portrait_panel_ready) {
      status.detail = "The portrait panel layout did not decode.";
    } else if (!status.minimap_ready) {
      status.detail = "The selected map minimap did not render.";
    } else if (!status.team_colors_ready) {
      status.detail = "The selected-race unit color table did not decode.";
    } else if (!status.pathing_map.valid()) {
      status.detail = "The selected map pathing grid did not build.";
    } else if (status.creep_tiles.size() !=
                   static_cast<std::size_t>(status.scenario_width) *
                       status.scenario_height ||
               status.creep_visual_tiles.size() !=
                   static_cast<std::size_t>(status.scenario_width) *
                       status.scenario_height ||
               status.creep_edge_frames.size() !=
                   static_cast<std::size_t>(status.scenario_width) *
                       status.scenario_height) {
      status.detail = "The selected map creep grids did not initialize.";
    } else if (!status.scenario.valid() || status.active_player_count < 2U ||
               status.units.empty()) {
      status.detail = "The selected scenario runtime has no playable units.";
    } else if (!map_closed || !assets_closed) {
      status.detail = "A read-only Storm archive did not close cleanly.";
    } else {
      status.detail = "A HUD, status panel, sound, or team-color runtime "
                      "dependency did not initialize.";
    }
  }
  return status;
}

} // namespace starcraft::recovery
