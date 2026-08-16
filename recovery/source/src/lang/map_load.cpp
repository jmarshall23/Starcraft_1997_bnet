#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/gds/grp.hpp"
#include "starcraft/gds/tileset.hpp"
#include "starcraft/lang/count.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_init.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/lang/place_unit.hpp"
#include "starcraft/runtime/storm.hpp"

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
    const std::vector<std::uint8_t> *const creep_visual_tiles) {
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
        }
      }
    }
  }
  return true;
}

BootstrapStatus probe_assets() {
  BootstrapStatus status{};
  const std::filesystem::path root = locate_input_root();
  if (root.empty()) {
    status.primary = "Licensed input directory was not found.";
    status.detail = "Run beside storm.dll and StarDat.mpq, or from the "
                    "configured build tree.";
    return status;
  }

  starcraft::runtime::StormModule storm{root / L"storm.dll"};
  if (!storm.loaded()) {
    status.primary = "The supplied storm.dll could not be loaded.";
    return status;
  }

  void *archive{};
  if (!storm.open_archive(root / L"StarDat.mpq", &archive, 1000)) {
    status.primary = "Storm loaded, but StarDat.mpq could not be opened.";
    return status;
  }
  void *patch_archive{};
  const bool patch_opened =
      storm.open_archive(root / L"patch_rt.mpq", &patch_archive, 2000);
  if (!patch_opened) {
    const bool archive_closed = storm.close_archive(archive);
    (void)archive_closed;
    status.primary = "Storm loaded, but patch_rt.mpq could not be opened.";
    return status;
  }

  // lang\maphdr.cpp::sub_46C3D0 at 0x0046C3D0 opens an SCM as a Storm
  // archive and then reads staredit\scenario.chk from it. Keep the supplied
  // map read-only and use the same archive path here.
  const std::filesystem::path map_path = root / L"maps" / L"96x96_space4.scm";
  status.map_name = "maps\\96x96_space4.scm";
  void *map_archive{};
  const bool map_opened = storm.open_archive(map_path, &map_archive, 3000);
  std::vector<std::uint8_t> map_chk;
  const bool map_loaded =
      map_opened && storm.load_file_from_archive(
                        map_archive, R"(staredit\scenario.chk)", map_chk);
  const starcraft::data::ChkView chk{map_chk.data(), map_chk.size()};
  starcraft::game::MultiplayerScenario scenario{};
  const bool scenario_loaded = map_loaded && scenario.load(chk);
  // The recovered game keeps g_hScenArchive open, but this bootstrap's
  // transitional global Storm lookup must not search the map MPQ for every
  // DAT/GRP asset. The complete CHK is already resident, so close it now.
  const bool map_closed =
      map_archive == nullptr || storm.close_archive(map_archive);
  map_archive = nullptr;

  starcraft::data::CoreDataSet data{};
  const bool data_loaded = data.load(storm);
  status.unit_traits_ready =
      data_loaded && data.extract_unit_traits(status.unit_traits);
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
    // clamped at zero. Slot zero is the bootstrap's local player.
    std::uint16_t focus_x = static_cast<std::uint16_t>(scenario.width() * 16U);
    std::uint16_t focus_y = static_cast<std::uint16_t>(scenario.height() * 16U);
    if (scenario.start_locations()[0].present) {
      focus_x = scenario.start_locations()[0].x;
      focus_y = scenario.start_locations()[0].y;
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
        starcraft::gds::beta_tileset_name(scenario.tileset_id());
    status.tileset_name.assign(recovered_tileset_name);
    if (!recovered_tileset_name.empty() &&
        terrain_tileset.load(storm, recovered_tileset_name)) {
      palette = terrain_tileset.palette();
      status.terrain_ready =
          build_terrain_preview(terrain_tileset, scenario, status.camera_x,
                                status.camera_y, status.terrain);
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
    const std::uint8_t local_race = scenario.players()[0].race;
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
      std::string unit_color_path = "game\\";
      unit_color_path.push_back(local_race_code);
      unit_color_path += "unit.pcx";
      starcraft::runtime::DecodedPcx unit_colors{};
      // CUnitColor initialization at 0x00424E20 asks the PCX decoder for
      // exactly 96 bytes (12 owners * 8 translated indices), even though the
      // source tunit.pcx row is 128 pixels wide.
      status.team_colors_ready =
          storm.load_pcx(unit_color_path.c_str(), unit_colors) &&
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
      std::string icon_remap_path = R"(unit\cmdbtns\)";
      icon_remap_path.push_back(local_race_code);
      icon_remap_path += "icon.pcx";
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
          storm.load_pcx(icon_remap_path.c_str(), icon_remap) &&
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
        if (unit.owner == 0 && unit.x >= view_left &&
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
  bool cargo_assets_ready{};
  bool working_overlay_asset_ready{};
  bool build_assets_ready{};
  bool production_assets_ready{};
  bool portrait_assets_ready{};
  bool melee_start_ready{};
  if (data_loaded && palette_loaded && scenario.valid()) {
    status.iscript_bytes = data.iscript();
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
      runtime.initialization = initialization;
      runtime.asset_index = asset_index;
      runtime.ready = true;
      return &runtime;
    };

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
    // CUnitHarvest.cpp::sub_42D3C0 passes source inventory types 220 and 222
    // to CUnitInv.cpp::sub_430FB0. That function adds 137 to obtain the
    // attached image IDs for carried minerals and Terran gas.
    status.mineral_cargo_asset_index = ensure_asset(220U + 137U);
    status.terran_gas_cargo_asset_index = ensure_asset(222U + 137U);
    cargo_assets_ready = status.mineral_cargo_asset_index != SIZE_MAX &&
                         status.terran_gas_cargo_asset_index != SIZE_MAX;

    // CUnitBuild.cpp::sub_423020 dispatches Working (19) to the producer.
    // The Command Center image-246 script creates image 247 at that point;
    // its ordinary idle action instead creates the unrelated image 248.
    // Cache 247 independently so action-created attachments can be rendered.
    status.command_center_working_asset_index = ensure_asset(247U);
    working_overlay_asset_ready =
        status.command_center_working_asset_index != SIZE_MAX;

    build_assets_ready = true;
    const starcraft::lang::TerranUnitTypeView terran_buildables =
        starcraft::lang::terran_buildable_unit_types();
    build_assets_ready =
        terran_buildables.count == status.buildable_units.size();
    for (std::size_t index = 0;
         build_assets_ready && index < terran_buildables.count; ++index) {
      BuildableUnitVisual &buildable = status.buildable_units[index];
      buildable.unit_type = terran_buildables.unit_types[index];
      std::uint16_t image_id{};
      build_assets_ready = data.unit_placement_size(
                               buildable.unit_type, buildable.placement_width,
                               buildable.placement_height) &&
                           data.unit_image_id(buildable.unit_type, image_id) &&
                           build_assets_ready;
      if (!build_assets_ready) {
        break;
      }
      buildable.asset_index = ensure_asset(image_id);
      if (buildable.asset_index == SIZE_MAX) {
        build_assets_ready = false;
        break;
      }
      if (!data.unit_simulation_traits(buildable.unit_type,
                                       buildable.simulation)) {
        build_assets_ready = false;
        break;
      }
      if ((buildable.simulation.dat_flags & 2U) != 0U) {
        buildable.addon_parent_type =
            starcraft::lang::terran_addon_parent_type(buildable.unit_type);
        if (buildable.addon_parent_type == 0xFFFFU ||
            !data.unit_addon_position(buildable.unit_type, buildable.addon_x,
                                      buildable.addon_y)) {
          build_assets_ready = false;
          break;
        }
      }
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
    std::vector<std::uint8_t> sfx_data;
    std::vector<std::uint8_t> sfx_table;
    status.unit_sounds_ready =
        unit_sound_ranges_ready &&
        storm.load_file(R"(arr\sfxdata.dat)", sfx_data) &&
        storm.load_file(R"(arr\sfxdata.tbl)", sfx_table) &&
        cache_unit_sound_assets(storm, sfx_data, sfx_table, wanted_sound_types,
                                status);
  }

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
    melee_start_ready =
        settle_melee_starting_workers(status) && melee_start_ready;
    (void)rebuild_creep_tiles(status);
  }

  const bool patch_closed = storm.close_archive(patch_archive);
  const bool archive_closed = storm.close_archive(archive);
  status.assets_ready =
      map_loaded && scenario_loaded && data_loaded && focus_unit_found &&
      focus_asset_ready && scv_asset_ready && palette_loaded &&
      cargo_assets_ready && working_overlay_asset_ready && build_assets_ready &&
      production_assets_ready && portrait_assets_ready && melee_start_ready &&
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
      status.creep_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.creep_visual_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height &&
      status.scenario.valid() && status.active_player_count >= 2 &&
      !status.units.empty() && map_closed && patch_closed && archive_closed;
  if (status.assets_ready) {
    char detail[300]{};
    std::snprintf(detail, sizeof(detail),
                  "%s: %ux%u %s, four players, %zu units/%zu THGY sprites; "
                  "camera %u,%u px; %s; image %u/script %u/draw %u.",
                  status.map_name.c_str(), status.scenario_width,
                  status.scenario_height, status.tileset_name.c_str(),
                  status.scenario_unit_count, status.scenario_sprite_count,
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
      status.detail =
          "Could not open maps\\96x96_space4.scm as a read-only Storm archive.";
    } else if (!scenario_loaded) {
      status.detail =
          "Could not parse the beta CHK sections from staredit\\scenario.chk.";
    } else if (!data_loaded) {
      status.detail = "Failed DAT/TBL asset: " + data.failed_asset();
    } else {
      status.detail = "The ERA tileset, initial CUnit image, IScript, or "
                      "archive close did not verify.";
    }
  }
  return status;
}

} // namespace starcraft::recovery
