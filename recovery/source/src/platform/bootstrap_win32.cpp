#include "starcraft/data/chk.hpp"
#include "starcraft/data/dat.hpp"
#include "starcraft/game/image.hpp"
#include "starcraft/game/scenario.hpp"
#include "starcraft/gds/grp.hpp"
#include "starcraft/gds/tileset.hpp"
#include "starcraft/lang/count.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_init.hpp"
#include "starcraft/lang/cunit_path_collide.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/damage.hpp"
#include "starcraft/lang/flingy.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/lang/pathfinding.hpp"
#include "starcraft/lang/place_unit.hpp"
#include "starcraft/lang/tile2.hpp"
#include "starcraft/runtime/storm.hpp"

#include "smacker.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <windows.h>
#include <gl/GL.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr char kWindowClass[] = "StarcraftBetaRecovered";
constexpr char kWindowTitle[] = "Starcraft Beta - Source Recovery Bootstrap";
constexpr std::uint16_t kMapViewportWidth = 640;
constexpr std::uint16_t kMapViewportHeight = 400;
constexpr std::uint32_t kSimulationTickMilliseconds = 42;
constexpr GLenum kGlBgra = 0x80E1;

struct SpritePreviewFrame {
  std::uint8_t x_offset{};
  std::uint8_t y_offset{};
  std::uint16_t width{};
  std::uint16_t height{};
  std::vector<std::uint32_t> bgra{};
  std::vector<std::uint8_t> palette_indices{};
  std::vector<std::uint8_t> opacity{};
};

struct SmackerPortrait {
  smk decoder{};
  SpritePreviewFrame frame{};
  std::string path{};
  std::uint32_t frame_count{};
  std::uint32_t last_frame_tick{};
  std::uint32_t frame_delay_ms{1};
  bool ready{};

  SmackerPortrait() = default;
  ~SmackerPortrait() {
    if (decoder != nullptr) {
      smk_close(decoder);
    }
  }
  SmackerPortrait(const SmackerPortrait&) = delete;
  SmackerPortrait& operator=(const SmackerPortrait&) = delete;
  SmackerPortrait(SmackerPortrait&& other) noexcept
      : decoder(std::exchange(other.decoder, nullptr)),
        frame(std::move(other.frame)),
        path(std::move(other.path)),
        frame_count(other.frame_count),
        last_frame_tick(other.last_frame_tick),
        frame_delay_ms(other.frame_delay_ms),
        ready(other.ready) {
    other.ready = false;
  }
  SmackerPortrait& operator=(SmackerPortrait&& other) noexcept {
    if (this != &other) {
      if (decoder != nullptr) {
        smk_close(decoder);
      }
      decoder = std::exchange(other.decoder, nullptr);
      frame = std::move(other.frame);
      path = std::move(other.path);
      frame_count = other.frame_count;
      last_frame_tick = other.last_frame_tick;
      frame_delay_ms = other.frame_delay_ms;
      ready = other.ready;
      other.ready = false;
    }
    return *this;
  }
};

struct UnitPortraitAsset {
  std::uint16_t unit_type{};
  std::uint8_t owner{};
  SmackerPortrait portrait{};
};

struct UnitRenderAsset {
  std::string sprite_path{};
  std::uint16_t sprite_canvas_width{};
  std::uint16_t sprite_canvas_height{};
  std::uint16_t image_id{};
  std::uint8_t image_draw_function{};
  bool graphics_turns{};
  std::uint16_t iscript_id{};
  starcraft::lang::IScriptState initial_iscript_state{};
  bool iscript_ready{};
  std::vector<SpritePreviewFrame> sprite_frames{};
  std::string overlay_path{};
  std::uint16_t overlay_image_id{};
  std::uint8_t overlay_draw_function{};
  std::uint16_t overlay_iscript_id{};
  starcraft::lang::IScriptState initial_overlay_iscript_state{};
  bool overlay_ready{};
  bool overlay_above{};
  std::int8_t overlay_x_offset{};
  std::int8_t overlay_y_offset{};
  std::vector<SpritePreviewFrame> overlay_frames{};
};

enum class ActiveUnitOrder : std::uint8_t {
  none,
  move,
  attack,
  repair,
  construct,
  gather,
  return_cargo,
};

struct ScenarioUnitPreview {
  std::uint32_t unit_id{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t unit_type{};
  std::uint16_t selection_width{};
  std::uint16_t selection_height{};
  std::uint16_t collision_left{};
  std::uint16_t collision_top{};
  std::uint16_t collision_right{};
  std::uint16_t collision_bottom{};
  std::uint8_t owner{};
  std::size_t asset_index{};
  starcraft::lang::IScriptState iscript_state{};
  starcraft::lang::IScriptState overlay_iscript_state{};
  starcraft::lang::IScriptState dynamic_overlay_iscript_state{};
  std::size_t current_sprite_frame{};
  std::size_t current_overlay_frame{};
  std::size_t current_dynamic_overlay_frame{};
  std::size_t dynamic_overlay_asset_index{SIZE_MAX};
  std::int32_t x_fixed{};
  std::int32_t y_fixed{};
  std::uint32_t movement_speed{};
  std::uint32_t movement_top_speed{};
  std::uint16_t movement_acceleration{};
  std::uint16_t movement_target_x{};
  std::uint16_t movement_target_y{};
  std::uint8_t direction{};
  std::uint8_t movement_turn_speed{};
  std::uint8_t movement_control{};
  std::vector<starcraft::lang::PathPoint> movement_path{};
  std::size_t movement_path_index{};
  std::uint16_t movement_final_x{};
  std::uint16_t movement_final_y{};
  std::uint32_t max_hit_points{};
  std::uint32_t hit_points{};
  std::uint32_t dat_flags{};
  std::uint32_t weapon_range{};
  std::uint32_t order_target_id{};
  std::uint32_t harvest_source_id{};
  std::uint32_t repair_mineral_accumulator{};
  std::uint32_t repair_gas_accumulator{};
  std::uint32_t construction_builder_id{};
  std::uint32_t addon_parent_id{};
  std::uint32_t attached_addon_id{};
  std::uint32_t larva_parent_id{};
  std::uint16_t weapon_damage{};
  std::uint16_t repair_step{};
  std::uint16_t resource_amount{};
  std::uint16_t mineral_cost{};
  std::uint16_t gas_cost{};
  std::uint16_t action_timer{};
  std::uint16_t construction_ticks_total{};
  std::uint16_t construction_ticks_remaining{};
  std::uint8_t armor{};
  std::uint8_t armor_class{};
  std::uint8_t weapon_damage_class{};
  std::uint8_t weapon_cooldown{};
  std::uint8_t cargo_minerals{};
  std::uint8_t cargo_gas{};
  std::uint8_t action_phase{};
  std::uint8_t construction_animation_phase{};
  std::uint8_t last_animation{};
  std::int8_t dynamic_overlay_x_offset{};
  std::int8_t dynamic_overlay_y_offset{};
  std::uint8_t avoidance_ticks{};
  std::uint8_t collision_wait_ticks{};
  std::int8_t avoidance_turn{};
  starcraft::lang::UnitProductionQueue production_queue{};
  starcraft::lang::UnitHarvestQueue harvest_queue{};
  std::uint32_t production_started{};
  starcraft::lang::UnitProductionKind production_kind{
      starcraft::lang::UnitProductionKind::train};
  std::uint8_t larva_spawn_timer{};
  ActiveUnitOrder active_order{ActiveUnitOrder::none};
  bool iscript_ready{};
  bool dynamic_overlay_ready{};
  bool dynamic_overlay_above{};
  bool overlay_ready{};
  bool selected{};
  bool moving{};
  bool is_building{};
  bool construction_complete{true};
  bool has_ground_weapon{};
  bool alive{true};
  bool production_active{};
};

struct CommandControl {
  std::uint16_t position{};
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
};

struct CommandButtonVisual {
  std::uint16_t position{};
  std::uint16_t icon{};
  std::uint16_t argument{};
  enum class Action : std::uint8_t {
    none,
    train_unit,
    begin_move_target,
    stop,
    begin_attack_target,
    begin_repair_target,
    begin_gather_target,
    return_cargo,
    cancel_target,
    open_card,
    begin_building_placement,
    build_addon,
    close_card,
  } action{};
};

struct CommandCardView {
  const CommandButtonVisual* buttons{};
  std::size_t count{};
};

struct BuildableUnitVisual {
  std::uint16_t unit_type{};
  std::uint16_t placement_width{};
  std::uint16_t placement_height{};
  std::size_t asset_index{SIZE_MAX};
  starcraft::data::UnitSimulationTraits simulation{};
  std::uint16_t addon_parent_type{0xFFFFU};
  std::int16_t addon_x{};
  std::int16_t addon_y{};
};

struct RuntimeUnitType {
  starcraft::lang::UnitInitializationData initialization{};
  std::size_t asset_index{SIZE_MAX};
  bool ready{};
};

struct UnitSoundRanges {
  std::uint16_t ready{};
  std::uint16_t what_first{};
  std::uint16_t what_last{};
  std::uint16_t annoyed_first{};
  std::uint16_t annoyed_last{};
  std::uint16_t yes_first{};
  std::uint16_t yes_last{};
};

struct ArchivedSoundAsset {
  std::uint16_t sound_id{};
  std::string path{};
  std::vector<std::uint8_t> wave{};
};

struct BootstrapStatus {
  bool assets_ready{};
  std::string primary{"Asset probe has not run."};
  std::string detail{};
  std::string sprite_path{};
  std::uint16_t sprite_canvas_width{};
  std::uint16_t sprite_canvas_height{};
  std::uint16_t image_id{};
  std::uint8_t image_draw_function{};
  std::uint16_t iscript_id{};
  std::vector<std::uint8_t> iscript_bytes{};
  std::vector<UnitRenderAsset> unit_assets{};
  std::size_t focus_asset_index{SIZE_MAX};
  std::size_t scv_asset_index{SIZE_MAX};
  std::size_t geyser_asset_index{SIZE_MAX};
  std::size_t mineral_cargo_asset_index{SIZE_MAX};
  std::size_t terran_gas_cargo_asset_index{SIZE_MAX};
  std::size_t command_center_working_asset_index{SIZE_MAX};
  std::uint16_t scv_selection_width{};
  std::uint16_t scv_selection_height{};
  std::uint16_t geyser_selection_width{};
  std::uint16_t geyser_selection_height{};
  starcraft::data::FlingyMovementTraits scv_movement{};
  starcraft::data::UnitSimulationTraits scv_simulation{};
  starcraft::data::UnitSimulationTraits geyser_simulation{};
  SpritePreviewFrame terrain{};
  bool terrain_ready{};
  SpritePreviewFrame hud{};
  bool hud_ready{};
  std::string hud_path{};
  SpritePreviewFrame minimap{};
  bool minimap_ready{};
  CommandControl minimap_control{};
  std::uint16_t minimap_content_x{};
  std::uint16_t minimap_content_y{};
  std::uint16_t minimap_content_width{};
  std::uint16_t minimap_content_height{};
  bool wireframe_ready{};
  std::uint16_t wireframe_canvas_width{};
  std::uint16_t wireframe_canvas_height{};
  std::vector<SpritePreviewFrame> wireframe_frames{};
  bool group_wireframe_ready{};
  std::uint16_t group_wireframe_canvas_width{};
  std::uint16_t group_wireframe_canvas_height{};
  std::vector<SpritePreviewFrame> group_wireframe_frames{};
  std::vector<std::uint8_t> wireframe_color_table{};
  std::vector<std::uint8_t> hud_palette{};
  bool status_panel_ready{};
  CommandControl status_wireframe_control{};
  CommandControl status_name_control{};
  CommandControl status_health_control{};
  CommandControl status_aux_control{};
  CommandControl status_action_label_control{};
  CommandControl status_progress_control{};
  CommandControl status_construction_label_control{};
  CommandControl status_construction_progress_control{};
  std::array<CommandControl, 5> status_queue_controls{};
  std::array<CommandControl, 12> status_selection_controls{};
  bool status_progress_art_ready{};
  SpritePreviewFrame status_progress_empty{};
  SpritePreviewFrame status_progress_full{};
  std::vector<std::uint8_t> stat_text_table{};
  bool resource_panel_ready{};
  bool resource_icons_ready{};
  std::uint16_t resource_icon_canvas_width{};
  std::uint16_t resource_icon_canvas_height{};
  std::vector<SpritePreviewFrame> resource_icon_frames{};
  CommandControl resource_supply_control{};
  CommandControl resource_gas_control{};
  CommandControl resource_mineral_control{};
  std::uint32_t displayed_minerals{};
  std::uint32_t displayed_gas{};
  starcraft::lang::UnitTraitsTable unit_traits{};
  bool unit_traits_ready{};
  std::array<std::vector<std::uint8_t>, 2> resource_error_waves{};
  std::array<std::string, 2> resource_error_wave_paths{};
  std::array<UnitSoundRanges, starcraft::lang::kUnitTypeCount> unit_sound_ranges{};
  std::vector<ArchivedSoundAsset> archived_sounds{};
  std::uint16_t pending_game_sound{0xFFFFU};
  std::uint16_t last_game_sound{0xFFFFU};
  std::uint32_t last_voice_unit_id{};
  std::uint8_t voice_repeat_count{};
  std::uint32_t sound_choice_counter{};
  bool unit_sounds_ready{};
  std::string music_path{};
  std::vector<std::uint8_t> music_wave{};
  bool music_available{};
  std::string system_message{};
  std::uint32_t system_message_until{};
  std::uint32_t resource_error_count{};
  std::uint8_t pending_resource_error_sound{0xFFU};
  bool portrait_panel_ready{};
  CommandControl portrait_control{};
  std::vector<UnitPortraitAsset> portraits{};
  bool command_panel_ready{};
  std::vector<SpritePreviewFrame> command_panel_frames{};
  std::vector<SpritePreviewFrame> command_icon_frames{};
  std::array<CommandControl, 9> command_controls{};
  std::uint16_t last_command_position{};
  std::uint16_t active_command_card{};
  std::uint16_t placement_unit_type{0xFFFFU};
  std::uint16_t placement_x{};
  std::uint16_t placement_y{};
  bool placement_active{};
  bool placement_valid{};
  bool command_target_active{};
  std::uint8_t target_unit_order{};
  std::uint8_t target_terrain_order{};
  std::uint8_t last_issued_order{};
  std::uint8_t last_command_opcode{};
  std::array<BuildableUnitVisual, 18> buildable_units{};
  std::array<RuntimeUnitType, starcraft::lang::kUnitTypeCount> runtime_unit_types{};
  std::uint16_t failed_runtime_unit_type{0xFFFFU};
  std::uint32_t next_unit_id{1};
  std::uint32_t player_minerals{50};
  std::uint32_t player_gas{};
  std::uint8_t local_race{};
  bool team_colors_ready{};
  std::vector<std::uint8_t> game_palette{};
  std::array<std::array<std::uint8_t, 8>, 12> team_color_indices{};
  std::size_t terrain_group_count{};
  std::size_t terrain_megatile_count{};
  std::size_t terrain_minitile_count{};
  starcraft::game::MultiplayerScenario scenario{};
  starcraft::gds::TilesetData terrain_tileset{};
  starcraft::lang::PathingMap pathing_map{};
  std::uint16_t scenario_width{};
  std::uint16_t scenario_height{};
  std::uint16_t camera_x{};
  std::uint16_t camera_y{};
  std::uint16_t camera_tile_x{};
  std::uint16_t camera_tile_y{};
  std::size_t scenario_unit_count{};
  std::size_t scenario_sprite_count{};
  std::string map_name{};
  std::string tileset_name{};
  std::size_t active_player_count{};
  std::array<bool, starcraft::data::chk_player_slot_count> active_players{};
  std::vector<std::uint8_t> creep_tiles{};
  std::vector<std::uint8_t> creep_visual_tiles{};
  std::vector<ScenarioUnitPreview> units{};
};

struct RecoveryWindowState {
  BootstrapStatus* status{};
  HDC device_context{};
  HGLRC rendering_context{};
  bool selection_dragging{};
  bool minimap_dragging{};
  int selection_start_x{};
  int selection_start_y{};
  int selection_current_x{};
  int selection_current_y{};
  int mouse_game_x{};
  int mouse_game_y{};
  std::uint8_t camera_scroll_ramp{};
  std::array<bool, 256> keys_down{};
  bool mouse_in_client{};
  std::uint16_t pressed_command_position{};
  GLuint font_display_lists{};
  ALCdevice* audio_device{};
  ALCcontext* audio_context{};
  ALuint audio_source{};
  std::array<ALuint, 2> resource_error_buffers{};
  std::vector<ALuint> archived_sound_buffers{};
  ALuint music_source{};
  ALuint music_buffer{};
  std::uint32_t audio_play_count{};
  bool audio_ready{};
  bool music_playing{};
};

bool rebuild_creep_tiles(BootstrapStatus& status) noexcept;
bool settle_melee_starting_workers(BootstrapStatus& status) noexcept;
bool restart_unit_animation(
    const BootstrapStatus& status,
    ScenarioUnitPreview& unit,
    std::uint8_t animation) noexcept;

std::uint16_t read_u16(
    const std::vector<std::uint8_t>& bytes,
    const std::size_t offset) noexcept {
  return offset + 2U <= bytes.size()
             ? static_cast<std::uint16_t>(bytes[offset]) |
                   static_cast<std::uint16_t>(
                       static_cast<std::uint16_t>(bytes[offset + 1U]) << 8U)
             : 0;
}

std::uint32_t read_u32(
    const std::vector<std::uint8_t>& bytes,
    const std::size_t offset) noexcept {
  return offset + 4U <= bytes.size()
             ? static_cast<std::uint32_t>(bytes[offset]) |
                   (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U) |
                   (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U) |
                   (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U)
             : 0;
}

struct PcmWaveView {
  const std::uint8_t* samples{};
  ALsizei sample_bytes{};
  ALsizei sample_rate{};
  ALenum format{};
};

bool parse_pcm_wave(
    const std::vector<std::uint8_t>& bytes,
    PcmWaveView& wave) noexcept {
  // The archived advisor assets are ordinary RIFF PCM. Walk the chunks rather
  // than assuming a fixed 44-byte header so LIST/JUNK extensions remain valid.
  if (bytes.size() < 12U || read_u32(bytes, 0) != 0x46464952U ||
      read_u32(bytes, 8) != 0x45564157U) {
    return false;
  }
  std::uint16_t encoding{};
  std::uint16_t channels{};
  std::uint16_t bits{};
  std::uint32_t sample_rate{};
  const std::uint8_t* samples{};
  std::size_t sample_bytes{};
  for (std::size_t chunk = 12U; chunk + 8U <= bytes.size();) {
    const std::uint32_t id = read_u32(bytes, chunk);
    const std::size_t size = read_u32(bytes, chunk + 4U);
    const std::size_t payload = chunk + 8U;
    if (size > bytes.size() - payload) {
      return false;
    }
    if (id == 0x20746D66U && size >= 16U) {  // "fmt "
      encoding = read_u16(bytes, payload);
      channels = read_u16(bytes, payload + 2U);
      sample_rate = read_u32(bytes, payload + 4U);
      bits = read_u16(bytes, payload + 14U);
    } else if (id == 0x61746164U) {  // "data"
      samples = bytes.data() + payload;
      sample_bytes = size;
    }
    const std::size_t padded = size + (size & 1U);
    if (padded > bytes.size() - payload) {
      return false;
    }
    chunk = payload + padded;
  }
  if (encoding != 1U || samples == nullptr || sample_bytes == 0U ||
      sample_bytes > static_cast<std::size_t>(INT_MAX) || sample_rate == 0U ||
      sample_rate > static_cast<std::uint32_t>(INT_MAX)) {
    return false;
  }
  if (channels == 1U && bits == 8U) {
    wave.format = AL_FORMAT_MONO8;
  } else if (channels == 1U && bits == 16U) {
    wave.format = AL_FORMAT_MONO16;
  } else if (channels == 2U && bits == 8U) {
    wave.format = AL_FORMAT_STEREO8;
  } else if (channels == 2U && bits == 16U) {
    wave.format = AL_FORMAT_STEREO16;
  } else {
    return false;
  }
  wave.samples = samples;
  wave.sample_bytes = static_cast<ALsizei>(sample_bytes);
  wave.sample_rate = static_cast<ALsizei>(sample_rate);
  return true;
}

bool parse_command_controls(
    const std::vector<std::uint8_t>& layout,
    std::array<CommandControl, 9>& controls) noexcept {
  // DLG/dlg.cpp::sub_4D9CC0 at 0x004D9CC0 relocates the linked controls.
  // statcmd.cpp::sub_4A4150 reads each command position at control+32.
  if (layout.size() < 70 || read_u16(layout, 34) != 0) {
    return false;
  }
  const std::int16_t root_x = static_cast<std::int16_t>(read_u16(layout, 4));
  const std::int16_t root_y = static_cast<std::int16_t>(read_u16(layout, 6));
  std::uint32_t offset = read_u32(layout, 66);
  std::array<bool, 9> found{};
  std::size_t count{};
  while (offset != 0 && offset + 70U <= layout.size() && count < controls.size()) {
    if (read_u16(layout, offset + 34U) != 2) {
      return false;
    }
    const std::uint16_t position = read_u16(layout, offset + 32U);
    if (position == 0 || position > controls.size() || found[position - 1U]) {
      return false;
    }
    CommandControl control{};
    control.position = position;
    control.left = static_cast<std::int16_t>(
        root_x + static_cast<std::int16_t>(read_u16(layout, offset + 4U)));
    control.top = static_cast<std::int16_t>(
        root_y + static_cast<std::int16_t>(read_u16(layout, offset + 6U)));
    control.right = static_cast<std::int16_t>(
        root_x + static_cast<std::int16_t>(read_u16(layout, offset + 8U)));
    control.bottom = static_cast<std::int16_t>(
        root_y + static_cast<std::int16_t>(read_u16(layout, offset + 10U)));
    controls[position - 1U] = control;
    found[position - 1U] = true;
    ++count;
    offset = read_u32(layout, offset);
  }
  return count == controls.size() &&
         std::all_of(found.begin(), found.end(), [](const bool value) { return value; });
}

bool parse_dialog_control(
    const std::vector<std::uint8_t>& layout,
    const std::int16_t identifier,
    CommandControl& output) noexcept {
  if (layout.size() < 70 || read_u16(layout, 34) != 0) {
    return false;
  }
  const std::int16_t root_x = static_cast<std::int16_t>(read_u16(layout, 4));
  const std::int16_t root_y = static_cast<std::int16_t>(read_u16(layout, 6));
  std::uint32_t offset = read_u32(layout, 66);
  while (offset != 0 && offset + 70U <= layout.size()) {
    if (static_cast<std::int16_t>(read_u16(layout, offset + 32U)) == identifier) {
      output.position = static_cast<std::uint16_t>(identifier);
      output.left = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 4U)));
      output.top = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 6U)));
      output.right = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 8U)));
      output.bottom = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 10U)));
      return output.right >= output.left && output.bottom >= output.top;
    }
    offset = read_u32(layout, offset);
  }
  return false;
}

bool parse_status_panel_controls(
    const std::vector<std::uint8_t>& layout,
    BootstrapStatus& status) noexcept {
  if (!parse_dialog_control(layout, 1, status.status_wireframe_control) ||
      !parse_dialog_control(layout, -5, status.status_name_control) ||
      !parse_dialog_control(layout, -7, status.status_health_control) ||
      !parse_dialog_control(layout, -8, status.status_aux_control) ||
      !parse_dialog_control(layout, -15, status.status_action_label_control) ||
      !parse_dialog_control(layout, 7, status.status_progress_control) ||
      !parse_dialog_control(layout, -30, status.status_construction_label_control) ||
      !parse_dialog_control(layout, 13, status.status_construction_progress_control)) {
    return false;
  }
  for (std::size_t index = 0; index < status.status_queue_controls.size(); ++index) {
    if (!parse_dialog_control(
            layout,
            static_cast<std::int16_t>(2 + index),
            status.status_queue_controls[index])) {
      return false;
    }
  }
  for (std::size_t index = 0; index < status.status_selection_controls.size(); ++index) {
    if (!parse_dialog_control(
            layout,
            static_cast<std::int16_t>(33 + index),
            status.status_selection_controls[index])) {
      return false;
    }
  }
  return true;
}

bool parse_resource_panel_controls(
    const std::vector<std::uint8_t>& layout,
    BootstrapStatus& status) noexcept {
  // statres.cpp::sub_4ABA90 at 0x004ABA90 walks controls 1, 2, and 3 for
  // race indices 0 (Zerg), 2 (Protoss), and 1 (Terran), then advances to
  // control 4 for gas and control 5 for minerals. The bootstrap's local slot
  // is Terran, so its exact supply rectangle is control 3.
  return parse_dialog_control(layout, 3, status.resource_supply_control) &&
         parse_dialog_control(layout, 4, status.resource_gas_control) &&
         parse_dialog_control(layout, 5, status.resource_mineral_control);
}

using CommandAction = CommandButtonVisual::Action;

// The packed 19-byte records referenced by the original command-card table at
// 0x00500188 are preserved here after applying their initial-state conditions.
// Type 7 points to 0x004FF3D0, type 106 to 0x004FFA00, card 237 to 0x004FF480,
// and card 240 to 0x004FF530.
constexpr std::array<CommandButtonVisual, 9> kScvButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 232, 0, CommandAction::begin_repair_target},
    {5, 231, 0, CommandAction::begin_gather_target},
    {6, 233, 0, CommandAction::return_cargo},
    {7, 234, 237, CommandAction::open_card},
    {8, 235, 240, CommandAction::open_card},
    {9, 236, 0, CommandAction::cancel_target},
}};

// The first three packed records are shared by the ordinary mobile-unit cards
// (Marine card 0, Ghost card 1, Vulture card 2, and the Zerg/Protoss cards):
// action 0x0047EA20 move, 0x0047EA50 stop, and 0x0047EA80 attack.
constexpr std::array<CommandButtonVisual, 3> kMobileUnitButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
}};

constexpr std::array<CommandButtonVisual, 1> kTargetCancelButtons{{
    {9, 236, 0, CommandAction::cancel_target},
}};

constexpr std::array<CommandButtonVisual, 4> kCommandCenterButtons{{
    {1, 7, 7, CommandAction::train_unit},
    {7, 107, 107, CommandAction::build_addon},
    {8, 108, 108, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 2> kFactoryButtons{{
    {2, 117, 117, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 2> kStarportButtons{{
    {2, 118, 118, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 3> kScienceFacilityButtons{{
    {7, 117, 117, CommandAction::build_addon},
    {8, 118, 118, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 9> kTerranBasicBuildButtons{{
    {1, 106, 106, CommandAction::begin_building_placement},
    {2, 109, 109, CommandAction::begin_building_placement},
    {3, 110, 110, CommandAction::begin_building_placement},
    {4, 111, 111, CommandAction::begin_building_placement},
    {5, 122, 122, CommandAction::begin_building_placement},
    {6, 124, 124, CommandAction::begin_building_placement},
    {7, 112, 112, CommandAction::begin_building_placement},
    {8, 125, 125, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

constexpr std::array<CommandButtonVisual, 5> kTerranAdvancedBuildButtons{{
    {1, 113, 113, CommandAction::begin_building_placement},
    {2, 114, 114, CommandAction::begin_building_placement},
    {3, 116, 116, CommandAction::begin_building_placement},
    {4, 123, 123, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

// cellscrl input routine sub_49E140 indexes this byte table at original VA
// 0x004FD6B8 as [scroll-speed option][acceleration step 0..6].
constexpr std::array<std::array<std::uint8_t, 7>, 7> kCameraScrollSpeeds{{
    {{0, 0, 8, 8, 16, 16, 16}},
    {{0, 0, 8, 8, 16, 16, 24}},
    {{0, 8, 8, 16, 16, 24, 24}},
    {{0, 8, 8, 16, 24, 24, 32}},
    {{0, 8, 16, 16, 24, 32, 40}},
    {{0, 8, 16, 24, 32, 40, 48}},
    {{0, 8, 24, 32, 40, 56, 64}},
}};
constexpr std::size_t kDefaultCameraScrollSpeed = 3;

void apply_simulation_traits(
    ScenarioUnitPreview& unit,
    const starcraft::data::UnitSimulationTraits& traits) noexcept {
  unit.max_hit_points = traits.max_hit_points;
  unit.hit_points = traits.max_hit_points;
  unit.dat_flags = traits.dat_flags;
  unit.armor = traits.armor;
  unit.armor_class = traits.armor_class;
  unit.weapon_range = traits.ground_weapon_range;
  unit.weapon_damage = traits.ground_weapon_damage;
  unit.weapon_damage_class = traits.ground_weapon_damage_class;
  unit.weapon_cooldown = traits.ground_weapon_cooldown;
  unit.has_ground_weapon = traits.has_ground_weapon;
  unit.mineral_cost = traits.mineral_cost;
  unit.gas_cost = traits.gas_cost;
  const std::uint32_t build_ticks = (std::max)(
      1U, static_cast<std::uint32_t>(traits.build_time) >> 1U);
  unit.repair_step = static_cast<std::uint16_t>((std::min)(
      static_cast<std::uint32_t>(UINT16_MAX),
      (traits.max_hit_points + build_ticks - 1U) / build_ticks));
  unit.resource_amount = starcraft::lang::initial_resource_amount(unit.unit_type);
}

void apply_initialization_traits(
    ScenarioUnitPreview& unit,
    const starcraft::lang::UnitInitializationData& initialization) noexcept {
  apply_simulation_traits(unit, initialization.simulation);
  unit.movement_top_speed =
      initialization.has_movement ? initialization.movement.top_speed : 0U;
  unit.movement_acceleration =
      initialization.has_movement ? initialization.movement.acceleration : 0U;
  unit.movement_turn_speed =
      initialization.has_movement ? initialization.movement.turn_speed : 0U;
  unit.movement_control =
      initialization.has_movement ? initialization.movement.movement_control : 0U;
}

const ScenarioUnitPreview* first_selected_unit(const BootstrapStatus& status) noexcept {
  const auto selected = std::find_if(
      status.units.begin(),
      status.units.end(),
      [](const ScenarioUnitPreview& unit) { return unit.alive && unit.selected; });
  return selected == status.units.end() ? nullptr : &*selected;
}

CommandCardView command_card_for(const BootstrapStatus& status) noexcept {
  const ScenarioUnitPreview* const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return {};
  }
  if (status.command_target_active) {
    return {kTargetCancelButtons.data(), kTargetCancelButtons.size()};
  }
  if (starcraft::lang::is_terran_scv(selected->unit_type) &&
      status.active_command_card == 237) {
    return {kTerranBasicBuildButtons.data(), kTerranBasicBuildButtons.size()};
  }
  if (starcraft::lang::is_terran_scv(selected->unit_type) &&
      status.active_command_card == 240) {
    return {kTerranAdvancedBuildButtons.data(), kTerranAdvancedBuildButtons.size()};
  }
  if (starcraft::lang::is_terran_scv(selected->unit_type)) {
    return {kScvButtons.data(), kScvButtons.size()};
  }
  if (selected->construction_complete) {
    const starcraft::lang::UnitProductionButtonView production =
        starcraft::lang::production_buttons_for(selected->unit_type);
    if (production.count != 0U) {
      // The production identities live in CUnitBuild.cpp. Convert them to the
      // renderer's transitional visual records, then append the add-on buttons
      // that coexist on these exact primary-executable cards.
      static thread_local std::array<CommandButtonVisual, 9> recovered{};
      std::size_t count{};
      for (std::size_t index = 0; index < production.count && count < recovered.size();
           ++index) {
        const starcraft::lang::UnitProductionButton& button =
            production.buttons[index];
        recovered[count++] = {
            button.position,
            button.icon,
            button.product_type,
            CommandAction::train_unit,
        };
      }
      const auto append_addon = [&](const std::uint16_t position,
                                    const std::uint16_t type) {
        if (count < recovered.size()) {
          recovered[count++] = {position, type, type, CommandAction::build_addon};
        }
      };
      if (selected->unit_type == 106U) {
        append_addon(7U, 107U);
        append_addon(8U, 108U);
      } else if (selected->unit_type == 113U) {
        append_addon(7U, 120U);
      } else if (selected->unit_type == 114U) {
        append_addon(7U, 115U);
      }
      if (selected->unit_type != starcraft::lang::zerg_larva_type &&
          count < recovered.size()) {
        // Every non-Larva production card in the recovered table has its
        // condition-selected position-9 cancel record with icon 236.
        recovered[count++] = {9U, 236U, 0U, CommandAction::none};
      }
      return {recovered.data(), count};
    }
  }
  if (!selected->is_building && selected->movement_top_speed != 0U) {
    return {kMobileUnitButtons.data(), kMobileUnitButtons.size()};
  }
  if (selected->unit_type == 116 && selected->construction_complete) {
    return {kScienceFacilityButtons.data(), kScienceFacilityButtons.size()};
  }
  return {};
}

const BuildableUnitVisual* find_buildable_unit(
    const BootstrapStatus& status,
    const std::uint16_t unit_type) noexcept {
  const auto entry = std::find_if(
      status.buildable_units.begin(),
      status.buildable_units.end(),
      [unit_type](const BuildableUnitVisual& value) {
        return value.unit_type == unit_type;
      });
  return entry == status.buildable_units.end() ? nullptr : &*entry;
}

bool build_terrain_preview(
    const starcraft::gds::TilesetData& tileset,
    const starcraft::game::MultiplayerScenario& scenario,
    const std::uint16_t camera_x,
    const std::uint16_t camera_y,
    SpritePreviewFrame& output,
    const std::vector<std::uint8_t>* const creep_tiles = nullptr,
    const std::vector<std::uint8_t>* const creep_visual_tiles = nullptr) {
  output = {};
  const std::uint32_t map_width = static_cast<std::uint32_t>(scenario.width()) * 32U;
  const std::uint32_t map_height = static_cast<std::uint32_t>(scenario.height()) * 32U;
  if (!scenario.valid() || camera_x + kMapViewportWidth > map_width ||
      camera_y + kMapViewportHeight > map_height) {
    return false;
  }
  const std::uint16_t camera_tile_x = camera_x / 32U;
  const std::uint16_t camera_tile_y = camera_y / 32U;
  const std::uint16_t offset_x = camera_x % 32U;
  const std::uint16_t offset_y = camera_y % 32U;
  const std::uint16_t tile_columns = static_cast<std::uint16_t>(
      (kMapViewportWidth + offset_x + 31U) / 32U);
  const std::uint16_t tile_rows = static_cast<std::uint16_t>(
      (kMapViewportHeight + offset_y + 31U) / 32U);
  output.width = kMapViewportWidth;
  output.height = kMapViewportHeight;
  output.bgra.resize(static_cast<std::size_t>(output.width) * output.height, 0xFF000000U);
  const auto& palette = tileset.palette();
  if (!tileset.valid() || palette.size() != 1024) {
    return false;
  }

  for (std::uint16_t tile_y = 0; tile_y < tile_rows; ++tile_y) {
    for (std::uint16_t tile_x = 0; tile_x < tile_columns; ++tile_x) {
      std::uint16_t map_tile_id{};
      starcraft::gds::IndexedMapTile field_tile{};
      if (!scenario.tile(camera_tile_x + tile_x, camera_tile_y + tile_y, map_tile_id) ||
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
        for (std::size_t x = 0; x < starcraft::gds::IndexedMapTile::width; ++x) {
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
              static_cast<std::size_t>(destination_y) * output.width + destination_x;
          output.bgra[destination] =
              0xFF000000U | blue | (green << 8U) | (red << 16U);
        }
      }
    }
  }
  return true;
}

bool build_minimap_preview(
    const starcraft::gds::TilesetData& tileset,
    const starcraft::game::MultiplayerScenario& scenario,
    SpritePreviewFrame& output,
    std::uint16_t& content_x,
    std::uint16_t& content_y,
    std::uint16_t& content_width,
    std::uint16_t& content_height,
    const std::vector<std::uint8_t>* const creep_tiles = nullptr,
    const std::vector<std::uint8_t>* const creep_visual_tiles = nullptr) noexcept {
  output = {};
  if (!tileset.valid() || !scenario.valid() || tileset.palette().size() != 1024) {
    return false;
  }
  try {
    const std::uint16_t largest = (std::max)(scenario.width(), scenario.height());
    const std::uint16_t numerator = largest <= 64U ? 2U : 1U;
    const std::uint16_t denominator = largest > 128U ? 2U : 1U;
    content_width = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(scenario.width()) * numerator / denominator);
    content_height = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(scenario.height()) * numerator / denominator);
    if (content_width == 0 || content_height == 0 || content_width > 128U ||
        content_height > 128U) {
      return false;
    }
    content_x = static_cast<std::uint16_t>((128U - content_width) / 2U);
    content_y = static_cast<std::uint16_t>((128U - content_height) / 2U);
    output.width = 128;
    output.height = 128;
    output.bgra.assign(128U * 128U, 0xFF020202U);
    const auto& palette = tileset.palette();
    for (std::uint16_t minimap_y = 0; minimap_y < content_height; ++minimap_y) {
      const std::uint16_t tile_y = static_cast<std::uint16_t>((std::min)(
          static_cast<std::uint32_t>(scenario.height() - 1U),
          static_cast<std::uint32_t>(minimap_y) * denominator / numerator));
      for (std::uint16_t minimap_x = 0; minimap_x < content_width; ++minimap_x) {
        const std::uint16_t tile_x = static_cast<std::uint16_t>((std::min)(
            static_cast<std::uint32_t>(scenario.width() - 1U),
            static_cast<std::uint32_t>(minimap_x) * denominator / numerator));
        std::uint16_t map_tile{};
        starcraft::gds::IndexedMapTile rendered{};
        if (!scenario.tile(tile_x, tile_y, map_tile)) {
          return false;
        }
        const std::size_t map_index =
            static_cast<std::size_t>(tile_y) * scenario.width() + tile_x;
        const std::size_t map_tile_count =
            static_cast<std::size_t>(scenario.width()) * scenario.height();
        if (creep_tiles != nullptr && creep_visual_tiles != nullptr &&
            creep_tiles->size() == map_tile_count &&
            creep_visual_tiles->size() == map_tile_count &&
            (*creep_tiles)[map_index] != 0U &&
            (*creep_visual_tiles)[map_index] >= 16U) {
          map_tile = (*creep_visual_tiles)[map_index];
        }
        if (!tileset.render(map_tile, rendered)) {
          return false;
        }
        // minimap.cpp::sub_46FFE0 emits one representative palette index per
        // MTXM tile for 65..128-tile maps. The source chain terminates in the
        // tileset graphics; sample the center of the recovered 32x32 megatile.
        const std::uint8_t palette_index =
            rendered.pixels[16U * starcraft::gds::IndexedMapTile::width + 16U];
        const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
        const std::uint32_t red = palette[color];
        const std::uint32_t green = palette[color + 1U];
        const std::uint32_t blue = palette[color + 2U];
        output.bgra[static_cast<std::size_t>(content_y + minimap_y) * 128U +
                    content_x + minimap_x] =
            0xFF000000U | blue | (green << 8U) | (red << 16U);
      }
    }
    return true;
  } catch (...) {
    output = {};
    return false;
  }
}

bool decode_preview_frames(
    const std::vector<std::uint8_t>& group,
    const std::vector<std::uint8_t>& palette,
    std::vector<SpritePreviewFrame>& frames,
    std::uint16_t& canvas_width,
    std::uint16_t& canvas_height,
    const std::vector<std::uint8_t>* const index_remap = nullptr) {
  if (group.empty() || palette.size() != 1024) {
    return false;
  }
  const std::uint16_t available_frames =
      starcraft::gds::grp_frame_count(group.data(), group.size());
  const std::uint16_t frame_count = available_frames;
  if (frame_count == 0) {
    return false;
  }

  std::vector<SpritePreviewFrame> decoded_frames;
  decoded_frames.reserve(frame_count);
  for (std::uint16_t frame_index = 0; frame_index < frame_count; ++frame_index) {
    starcraft::gds::DecodedGrpFrame decoded{};
    if (!starcraft::gds::decode_grp_frame(
            group.data(), group.size(), frame_index, decoded)) {
      return false;
    }
    SpritePreviewFrame frame{};
    frame.x_offset = decoded.x_offset;
    frame.y_offset = decoded.y_offset;
    frame.width = decoded.width;
    frame.height = decoded.height;
    frame.palette_indices = decoded.pixels;
    frame.opacity = decoded.opacity;
    frame.bgra.resize(decoded.pixels.size());
    for (std::size_t pixel = 0; pixel < decoded.pixels.size(); ++pixel) {
      if (decoded.opacity[pixel] == 0) {
        frame.bgra[pixel] = 0;
        continue;
      }
      std::uint8_t palette_index = decoded.pixels[pixel];
      if (index_remap != nullptr && palette_index < index_remap->size()) {
        palette_index = (*index_remap)[palette_index];
      }
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = palette[color];
      const std::uint32_t green = palette[color + 1];
      const std::uint32_t blue = palette[color + 2];
      frame.bgra[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    canvas_width = decoded.canvas_width;
    canvas_height = decoded.canvas_height;
    decoded_frames.push_back(std::move(frame));
  }
  frames = std::move(decoded_frames);
  return true;
}

bool decode_pcx_frame(
    const starcraft::runtime::DecodedPcx& image,
    const bool transparent_zero,
    SpritePreviewFrame& frame) {
  frame = {};
  if (image.width == 0 || image.height == 0 || image.width > 0xFFFFU ||
      image.height > 0xFFFFU || image.palette.size() != 1024 ||
      image.pixels.size() != static_cast<std::size_t>(image.width) * image.height) {
    return false;
  }
  try {
    frame.width = static_cast<std::uint16_t>(image.width);
    frame.height = static_cast<std::uint16_t>(image.height);
    frame.bgra.resize(image.pixels.size());
    for (std::size_t pixel = 0; pixel < image.pixels.size(); ++pixel) {
      const std::uint8_t palette_index = image.pixels[pixel];
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = image.palette[color];
      const std::uint32_t green = image.palette[color + 1];
      const std::uint32_t blue = image.palette[color + 2];
      const std::uint32_t alpha =
          transparent_zero && palette_index == 0 ? 0U : 0xFF000000U;
      frame.bgra[pixel] = alpha | blue | (green << 8U) | (red << 16U);
    }
    return true;
  } catch (...) {
    frame = {};
    return false;
  }
}

bool decode_smacker_frame(SmackerPortrait& portrait) noexcept {
  if (portrait.decoder == nullptr || portrait.frame.width == 0 ||
      portrait.frame.height == 0) {
    return false;
  }
  const unsigned char* const palette = smk_get_palette(portrait.decoder);
  const unsigned char* const video = smk_get_video(portrait.decoder);
  if (palette == nullptr || video == nullptr) {
    return false;
  }
  try {
    const std::size_t pixels = static_cast<std::size_t>(portrait.frame.width) *
                               portrait.frame.height;
    portrait.frame.bgra.resize(pixels);
    portrait.frame.palette_indices.assign(video, video + pixels);
    portrait.frame.opacity.assign(pixels, 0xFFU);
    for (std::size_t pixel = 0; pixel < pixels; ++pixel) {
      const std::size_t color = static_cast<std::size_t>(video[pixel]) * 3U;
      const std::uint32_t red = palette[color];
      const std::uint32_t green = palette[color + 1U];
      const std::uint32_t blue = palette[color + 2U];
      portrait.frame.bgra[pixel] =
          0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool load_unit_portrait(
    starcraft::runtime::StormModule& storm,
    const starcraft::data::CoreDataSet& data,
    const std::uint16_t unit_type,
    const std::uint8_t owner,
    SmackerPortrait& output) noexcept {
  try {
    for (std::uint8_t variant = 0; variant < 5U; ++variant) {
      const std::string path =
          data.unit_portrait_path(unit_type, owner, false, variant);
      std::vector<std::uint8_t> bytes;
      if (path.empty() || !storm.load_file(path.c_str(), bytes) || bytes.empty() ||
          bytes.size() > ULONG_MAX) {
        continue;
      }
      SmackerPortrait portrait{};
      portrait.decoder = smk_open_memory(
          bytes.data(), static_cast<unsigned long>(bytes.size()));
      if (portrait.decoder == nullptr) {
        continue;
      }
      unsigned long width{};
      unsigned long height{};
      unsigned long frame_count{};
      double microseconds_per_frame{};
      unsigned char y_scale{};
      if (smk_info_video(portrait.decoder, &width, &height, &y_scale) != 0 ||
          smk_info_all(
              portrait.decoder, nullptr, &frame_count, &microseconds_per_frame) != 0 ||
          width == 0 || height == 0 || width > UINT16_MAX || height > UINT16_MAX ||
          frame_count == 0 || frame_count > UINT32_MAX) {
        continue;
      }
      portrait.frame.width = static_cast<std::uint16_t>(width);
      portrait.frame.height = static_cast<std::uint16_t>(height);
      portrait.frame_count = static_cast<std::uint32_t>(frame_count);
      portrait.frame_delay_ms = static_cast<std::uint32_t>((std::max)(
          1.0, std::abs(microseconds_per_frame) / 1000.0));
      portrait.path = path;
      (void)smk_enable_all(portrait.decoder, SMK_VIDEO_TRACK);
      const char first = smk_first(portrait.decoder);
      if (first == SMK_ERROR || first == SMK_DONE ||
          !decode_smacker_frame(portrait)) {
        continue;
      }
      portrait.ready = true;
      output = std::move(portrait);
      return true;
    }
  } catch (...) {
  }
  return false;
}

bool apply_preview_draw_function(
    const std::uint8_t draw_function,
    std::vector<SpritePreviewFrame>& frames) {
  if (draw_function == 0 || draw_function == 14) {
    // Renderer 14 is the worker-inventory image path. sub_411280 stores the
    // carried amount on the image; its GRP pixels and attached cargo overlay
    // remain ordinary palette imagery for the bootstrap's true-color pass.
    return true;
  }
  if (draw_function != 9) {
    return false;
  }

  // Original renderer 9 treats the GRP as a mask and replaces each covered
  // destination palette index through byte_72F8D0. A half-black source-over
  // mask preserves that shadow behavior on this temporary true-color surface.
  for (SpritePreviewFrame& frame : frames) {
    for (std::uint32_t& pixel : frame.bgra) {
      pixel = (pixel & 0xFF000000U) != 0 ? 0x80000000U : 0U;
    }
  }
  return true;
}

bool load_image_preview(
    starcraft::runtime::StormModule& storm,
    const starcraft::data::CoreDataSet& data,
    const std::uint16_t image_id,
    const std::vector<std::uint8_t>& palette,
    std::string& path,
    std::vector<SpritePreviewFrame>& frames,
    std::uint16_t& canvas_width,
    std::uint16_t& canvas_height) {
  path = data.image_grp_path(image_id);
  std::vector<std::uint8_t> group;
  return !path.empty() && storm.load_file(path.c_str(), group) &&
         decode_preview_frames(group, palette, frames, canvas_width, canvas_height);
}

bool start_image_script(
    const std::vector<std::uint8_t>& bytes,
    const std::uint16_t script_id,
    starcraft::lang::IScriptState& state,
    const starcraft::lang::IScriptState* const parent = nullptr,
    const std::uint16_t tileset_frame_offset = 0U) {
  const starcraft::lang::IScriptProgramView program{bytes.data(), bytes.size()};
  if (!program.valid() || !program.start(script_id, 0, state)) {
    return false;
  }
  const auto result =
      program.tick(state, 0, 256, parent, tileset_frame_offset);
  return result == starcraft::lang::IScriptTickResult::yielded ||
         result == starcraft::lang::IScriptTickResult::sleeping;
}

bool load_unit_render_asset(
    starcraft::runtime::StormModule& storm,
    const starcraft::data::CoreDataSet& data,
    const std::vector<std::uint8_t>& palette,
    const std::vector<std::uint8_t>& iscript_bytes,
    const std::uint16_t image_id,
    const std::uint16_t tileset_frame_offset,
    UnitRenderAsset& output) {
  UnitRenderAsset asset{};
  asset.image_id = image_id;
  starcraft::data::ImageRenderTraits main_traits{};
  if (!load_image_preview(
          storm,
          data,
          image_id,
          palette,
          asset.sprite_path,
          asset.sprite_frames,
          asset.sprite_canvas_width,
          asset.sprite_canvas_height) ||
      !data.image_render_traits(image_id, main_traits) ||
      !apply_preview_draw_function(main_traits.draw_function, asset.sprite_frames) ||
      !data.image_iscript_id(image_id, asset.iscript_id) ||
      !start_image_script(
          iscript_bytes,
          asset.iscript_id,
          asset.initial_iscript_state,
          nullptr,
          tileset_frame_offset) ||
      asset.initial_iscript_state.frame >= asset.sprite_frames.size()) {
    return false;
  }
  asset.image_draw_function = main_traits.draw_function;
  asset.graphics_turns = main_traits.graphics_turns;
  asset.iscript_ready = true;

  if (asset.initial_iscript_state.overlay_event_count != 0) {
    asset.overlay_image_id = asset.initial_iscript_state.overlay_image;
    asset.overlay_above = asset.initial_iscript_state.overlay_above;
    asset.overlay_x_offset = asset.initial_iscript_state.overlay_x_offset;
    asset.overlay_y_offset = asset.initial_iscript_state.overlay_y_offset;
    std::uint16_t overlay_canvas_width{};
    std::uint16_t overlay_canvas_height{};
    starcraft::data::ImageRenderTraits overlay_traits{};
    if (!load_image_preview(
            storm,
            data,
            asset.overlay_image_id,
            palette,
            asset.overlay_path,
            asset.overlay_frames,
            overlay_canvas_width,
            overlay_canvas_height) ||
        !data.image_render_traits(asset.overlay_image_id, overlay_traits) ||
        !apply_preview_draw_function(
            overlay_traits.draw_function, asset.overlay_frames) ||
        !data.image_iscript_id(asset.overlay_image_id, asset.overlay_iscript_id) ||
        !start_image_script(
            iscript_bytes,
            asset.overlay_iscript_id,
            asset.initial_overlay_iscript_state,
            &asset.initial_iscript_state,
            tileset_frame_offset) ||
        asset.initial_overlay_iscript_state.frame >= asset.overlay_frames.size()) {
      return false;
    }
    asset.overlay_draw_function = overlay_traits.draw_function;
    asset.overlay_ready = true;
  }
  output = std::move(asset);
  return true;
}

void draw_preview_frame_gl(
    const SpritePreviewFrame& frame,
    const float x,
    const float y,
    const float width,
    const float height,
    const std::uint32_t* const override_bgra = nullptr,
    const bool mirrored = false) {
  if (frame.width == 0 || frame.height == 0 || frame.bgra.empty()) {
    return;
  }

  GLuint texture{};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      frame.width,
      frame.height,
      0,
      kGlBgra,
      GL_UNSIGNED_BYTE,
      override_bgra == nullptr ? frame.bgra.data() : override_bgra);
  glBegin(GL_QUADS);
  glTexCoord2f(mirrored ? 1.0F : 0.0F, 0.0F);
  glVertex2f(x, y);
  glTexCoord2f(mirrored ? 0.0F : 1.0F, 0.0F);
  glVertex2f(x + width, y);
  glTexCoord2f(mirrored ? 0.0F : 1.0F, 1.0F);
  glVertex2f(x + width, y + height);
  glTexCoord2f(mirrored ? 1.0F : 0.0F, 1.0F);
  glVertex2f(x, y + height);
  glEnd();
  glDeleteTextures(1, &texture);
}

void draw_preview_frame_fraction_gl(
    const SpritePreviewFrame& frame,
    const float x,
    const float y,
    const float width,
    const float height,
    const float fraction) {
  if (frame.width == 0 || frame.height == 0 || frame.bgra.empty() ||
      fraction <= 0.0F) {
    return;
  }
  const float clamped = (std::clamp)(fraction, 0.0F, 1.0F);
  GLuint texture{};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      frame.width,
      frame.height,
      0,
      kGlBgra,
      GL_UNSIGNED_BYTE,
      frame.bgra.data());
  glBegin(GL_QUADS);
  glTexCoord2f(0.0F, 0.0F);
  glVertex2f(x, y);
  glTexCoord2f(clamped, 0.0F);
  glVertex2f(x + width * clamped, y);
  glTexCoord2f(clamped, 1.0F);
  glVertex2f(x + width * clamped, y + height);
  glTexCoord2f(0.0F, 1.0F);
  glVertex2f(x, y + height);
  glEnd();
  glDeleteTextures(1, &texture);
}

void draw_team_colored_frame_gl(
    const BootstrapStatus& status,
    const SpritePreviewFrame& frame,
    const std::uint8_t owner,
    const float x,
    const float y,
    const float width,
    const float height,
    const bool mirrored = false) {
  if (!status.team_colors_ready || owner >= status.team_color_indices.size() ||
      status.game_palette.size() != 1024 ||
      frame.palette_indices.size() != frame.bgra.size() ||
      frame.opacity.size() != frame.bgra.size()) {
    draw_preview_frame_gl(frame, x, y, width, height, nullptr, mirrored);
    return;
  }
  try {
    std::vector<std::uint32_t> translated = frame.bgra;
    for (std::size_t pixel = 0; pixel < translated.size(); ++pixel) {
      const std::uint8_t source = frame.palette_indices[pixel];
      if (frame.opacity[pixel] == 0 || source < 8 || source > 15) {
        continue;
      }
      const std::uint8_t palette_index = status.team_color_indices[owner][source - 8U];
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = status.game_palette[color];
      const std::uint32_t green = status.game_palette[color + 1U];
      const std::uint32_t blue = status.game_palette[color + 2U];
      translated[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    draw_preview_frame_gl(frame, x, y, width, height, translated.data(), mirrored);
  } catch (...) {
    draw_preview_frame_gl(frame, x, y, width, height, nullptr, mirrored);
  }
}

void draw_scenario_unit_gl(
    const BootstrapStatus& status,
    const ScenarioUnitPreview& unit) {
  if (!unit.alive || unit.asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset& asset = status.unit_assets[unit.asset_index];
  if (!unit.iscript_ready || unit.iscript_state.hidden || asset.sprite_frames.empty()) {
    return;
  }
  std::size_t sprite_frame = unit.current_sprite_frame;
  bool sprite_mirrored = unit.iscript_state.mirrored;
  if (asset.graphics_turns) {
    const starcraft::game::ImageFacingFrame facing =
        starcraft::game::image_facing_frame(unit.direction);
    sprite_mirrored = sprite_mirrored != facing.mirrored;
    if (sprite_frame + facing.frame < asset.sprite_frames.size()) {
      sprite_frame += facing.frame;
    }
  }
  if (sprite_frame >= asset.sprite_frames.size()) {
    return;
  }
  const int origin_x =
      unit.x - status.camera_x -
      static_cast<int>(asset.sprite_canvas_width) / 2;
  const int origin_y =
      unit.y - status.camera_y -
      static_cast<int>(asset.sprite_canvas_height) / 2;
  const auto draw_dynamic_overlay = [&](const bool above) {
    if (!unit.dynamic_overlay_ready || unit.dynamic_overlay_above != above ||
        unit.dynamic_overlay_iscript_state.hidden ||
        unit.dynamic_overlay_asset_index >= status.unit_assets.size()) {
      return;
    }
    const UnitRenderAsset& overlay_asset =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    if (unit.current_dynamic_overlay_frame >= overlay_asset.sprite_frames.size()) {
      return;
    }
    const SpritePreviewFrame& overlay =
        overlay_asset.sprite_frames[unit.current_dynamic_overlay_frame];
    const float overlay_x = static_cast<float>(
        origin_x + static_cast<int>(overlay.x_offset) +
        unit.dynamic_overlay_x_offset +
        unit.dynamic_overlay_iscript_state.x_offset);
    const float overlay_y = static_cast<float>(
        origin_y + static_cast<int>(overlay.y_offset) +
        unit.dynamic_overlay_y_offset +
        unit.dynamic_overlay_iscript_state.y_offset);
    if (overlay_asset.image_draw_function == 0) {
      draw_team_colored_frame_gl(
          status,
          overlay,
          unit.owner,
          overlay_x,
          overlay_y,
          overlay.width,
          overlay.height);
    } else {
      draw_preview_frame_gl(
          overlay, overlay_x, overlay_y, overlay.width, overlay.height);
    }
  };
  if (unit.selected) {
    constexpr float pi = 3.14159265358979323846F;
    const float center_x = static_cast<float>(unit.x - status.camera_x);
    const float center_y = static_cast<float>(unit.y - status.camera_y);
    const float radius_x = static_cast<float>(unit.selection_width) / 2.0F;
    const float radius_y = static_cast<float>(unit.selection_height) / 2.0F;
    glDisable(GL_TEXTURE_2D);
    glColor4ub(32, 255, 32, 255);
    glLineWidth(2.0F);
    glBegin(GL_LINE_LOOP);
    for (int segment = 0; segment < 48; ++segment) {
      const float angle = 2.0F * pi * static_cast<float>(segment) / 48.0F;
      glVertex2f(
          center_x + radius_x * std::cos(angle),
          center_y + radius_y * std::sin(angle));
    }
    glEnd();
    glLineWidth(1.0F);
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
  }
  if (unit.overlay_ready && !asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    const SpritePreviewFrame& overlay = asset.overlay_frames[unit.current_overlay_frame];
    const float overlay_x =
        static_cast<float>(origin_x + static_cast<int>(overlay.x_offset) +
                           asset.overlay_x_offset + unit.overlay_iscript_state.x_offset);
    const float overlay_y =
        static_cast<float>(origin_y + static_cast<int>(overlay.y_offset) +
                           asset.overlay_y_offset + unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(
          status, overlay, unit.owner, overlay_x, overlay_y, overlay.width, overlay.height);
    } else {
      draw_preview_frame_gl(overlay, overlay_x, overlay_y, overlay.width, overlay.height);
    }
  }
  draw_dynamic_overlay(false);
  const SpritePreviewFrame& frame = asset.sprite_frames[sprite_frame];
  const float frame_x = static_cast<float>(
      origin_x + static_cast<int>(frame.x_offset) + unit.iscript_state.x_offset);
  const float frame_y = static_cast<float>(
      origin_y + static_cast<int>(frame.y_offset) + unit.iscript_state.y_offset);
  if (asset.image_draw_function == 0) {
    draw_team_colored_frame_gl(
        status,
        frame,
        unit.owner,
        frame_x,
        frame_y,
        frame.width,
        frame.height,
        sprite_mirrored);
  } else {
    draw_preview_frame_gl(
        frame, frame_x, frame_y, frame.width, frame.height, nullptr, sprite_mirrored);
  }
  if (unit.overlay_ready && asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    const SpritePreviewFrame& overlay = asset.overlay_frames[unit.current_overlay_frame];
    const float overlay_x =
        static_cast<float>(origin_x + static_cast<int>(overlay.x_offset) +
                           asset.overlay_x_offset + unit.overlay_iscript_state.x_offset);
    const float overlay_y =
        static_cast<float>(origin_y + static_cast<int>(overlay.y_offset) +
                           asset.overlay_y_offset + unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(
          status, overlay, unit.owner, overlay_x, overlay_y, overlay.width, overlay.height);
    } else {
      draw_preview_frame_gl(overlay, overlay_x, overlay_y, overlay.width, overlay.height);
    }
  }
  draw_dynamic_overlay(true);
  const std::size_t cargo_asset_index =
      unit.cargo_minerals != 0U
          ? status.mineral_cargo_asset_index
          : unit.cargo_gas != 0U ? status.terran_gas_cargo_asset_index : SIZE_MAX;
  if (cargo_asset_index < status.unit_assets.size() &&
      cargo_asset_index != unit.asset_index) {
    // CUnitInv.cpp::sub_430FB0 at 0x00430FB0 attaches image
    // resource-unit-type + 137 to the worker sprite. Mineral patches map to
    // image 357 and a Terran Refinery maps to image 359. It is inserted as
    // the sprite's head image, so draw it over the worker body.
    const UnitRenderAsset& cargo_asset = status.unit_assets[cargo_asset_index];
    ScenarioUnitPreview cargo{};
    cargo.x = unit.x;
    cargo.y = unit.y;
    cargo.owner = unit.owner;
    cargo.asset_index = cargo_asset_index;
    cargo.direction = unit.direction;
    cargo.iscript_state = cargo_asset.initial_iscript_state;
    cargo.overlay_iscript_state = cargo_asset.initial_overlay_iscript_state;
    cargo.current_sprite_frame = cargo_asset.initial_iscript_state.frame;
    cargo.current_overlay_frame = cargo_asset.initial_overlay_iscript_state.frame;
    cargo.iscript_ready = cargo_asset.iscript_ready;
    cargo.overlay_ready = cargo_asset.overlay_ready;
    draw_scenario_unit_gl(status, cargo);
  }
  if (unit.max_hit_points != 0 &&
      (unit.selected || unit.hit_points < unit.max_hit_points)) {
    const int bar_width = (std::max)(16, static_cast<int>(unit.selection_width));
    const int left = unit.x - status.camera_x - bar_width / 2;
    const int top = unit.y - status.camera_y - unit.selection_height / 2 - 7;
    const int filled = static_cast<int>(
        static_cast<std::uint64_t>(bar_width - 2) * unit.hit_points /
        unit.max_hit_points);
    glDisable(GL_TEXTURE_2D);
    glColor4ub(0, 0, 0, 255);
    glBegin(GL_QUADS);
    glVertex2i(left, top);
    glVertex2i(left + bar_width, top);
    glVertex2i(left + bar_width, top + 5);
    glVertex2i(left, top + 5);
    glEnd();
    glColor4ub(
        unit.hit_points * 3U >= unit.max_hit_points * 2U ? 32 : 255,
        unit.hit_points * 3U >= unit.max_hit_points ? 224 : 48,
        32,
        255);
    glBegin(GL_QUADS);
    glVertex2i(left + 1, top + 1);
    glVertex2i(left + 1 + filled, top + 1);
    glVertex2i(left + 1 + filled, top + 4);
    glVertex2i(left + 1, top + 4);
    glEnd();
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
  }
}

void draw_building_placement_gl(const BootstrapStatus& status) {
  if (!status.placement_active) {
    return;
  }
  const BuildableUnitVisual* const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr || buildable->asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset& asset = status.unit_assets[buildable->asset_index];
  ScenarioUnitPreview ghost{};
  ghost.x = status.placement_x;
  ghost.y = status.placement_y;
  ghost.unit_type = buildable->unit_type;
  ghost.owner = 0;
  ghost.asset_index = buildable->asset_index;
  ghost.selection_width = buildable->placement_width;
  ghost.selection_height = buildable->placement_height;
  ghost.iscript_state = asset.initial_iscript_state;
  ghost.overlay_iscript_state = asset.initial_overlay_iscript_state;
  ghost.current_sprite_frame = asset.initial_iscript_state.frame;
  ghost.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  ghost.iscript_ready = asset.iscript_ready;
  ghost.overlay_ready = asset.overlay_ready;
  draw_scenario_unit_gl(status, ghost);

  const int left = static_cast<int>(status.placement_x) -
                   static_cast<int>(buildable->placement_width) / 2 -
                   status.camera_x;
  const int top = static_cast<int>(status.placement_y) -
                  static_cast<int>(buildable->placement_height) / 2 -
                  status.camera_y;
  const int right = left + buildable->placement_width;
  const int bottom = top + buildable->placement_height;
  glDisable(GL_TEXTURE_2D);
  if (status.placement_valid) {
    glColor4ub(32, 255, 32, 72);
  } else {
    glColor4ub(255, 48, 32, 96);
  }
  glBegin(GL_QUADS);
  glVertex2i(left, top);
  glVertex2i(right, top);
  glVertex2i(right, bottom);
  glVertex2i(left, bottom);
  glEnd();
  glColor4ub(
      status.placement_valid ? 32 : 255,
      status.placement_valid ? 255 : 48,
      32,
      255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(left, top);
  glVertex2i(right, top);
  glVertex2i(right, bottom);
  glVertex2i(left, bottom);
  glEnd();
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

constexpr float hud_vertical_scale() noexcept {
  return 400.0F / 480.0F;
}

std::string_view status_text(
    const BootstrapStatus& status,
    const std::uint16_t one_based_id) noexcept {
  const starcraft::data::StringTableView table{
      status.stat_text_table.data(), status.stat_text_table.size()};
  return table.one_based(one_based_id);
}

std::string printable_status_text(const std::string_view source) {
  std::string output;
  output.reserve(source.size());
  for (const unsigned char value : source) {
    if (value >= 32U && value < 127U) {
      output.push_back(static_cast<char>(value));
    }
  }
  return output;
}

void post_resource_error(
    BootstrapStatus& status,
    const bool gas) noexcept {
  // Resource checks at 0x00402000/0x00402240 prioritize minerals, then gas.
  // They call 0x00456180 with stat_txt IDs 781/782 and sound IDs race+124/
  // race+127. The local-player helper posts the message for 7000 ms.
  const std::uint16_t text_id = gas ? 782U : 781U;
  const std::string_view archived = status_text(status, text_id);
  status.system_message = archived.empty()
                              ? (gas ? "Not enough Vespene gas."
                                     : "Not enough minerals.")
                              : printable_status_text(archived);
  status.system_message_until = GetTickCount() + 7000U;
  status.pending_resource_error_sound = gas ? 1U : 0U;
  ++status.resource_error_count;
}

bool resource_cost_available(
    BootstrapStatus& status,
    const std::uint32_t minerals,
    const std::uint32_t gas) noexcept {
  if (status.player_minerals < minerals) {
    post_resource_error(status, false);
    return false;
  }
  if (status.player_gas < gas) {
    post_resource_error(status, true);
    return false;
  }
  return true;
}

void draw_status_text_gl(
    const RecoveryWindowState& state,
    const CommandControl& control,
    const std::string_view text,
    const std::uint8_t red = 220,
    const std::uint8_t green = 220,
    const std::uint8_t blue = 220) {
  if (state.font_display_lists == 0 || text.empty()) {
    return;
  }
  const std::size_t maximum_characters = static_cast<std::size_t>((std::max)(
      1, (control.right - control.left + 1) / 6));
  std::string printable = printable_status_text(text);
  if (printable.size() > maximum_characters) {
    printable.resize(maximum_characters);
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(red, green, blue, 255);
  glRasterPos2f(
      static_cast<float>(control.left),
      static_cast<float>(control.top) * hud_vertical_scale() + 9.0F);
  glListBase(state.font_display_lists - 32U);
  glCallLists(
      static_cast<GLsizei>(printable.size()),
      GL_UNSIGNED_BYTE,
      printable.data());
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

void draw_system_message_gl(
    const RecoveryWindowState& state,
    const std::uint32_t now) {
  const BootstrapStatus* const status = state.status;
  if (status == nullptr || status->system_message.empty() ||
      static_cast<std::int32_t>(status->system_message_until - now) <= 0) {
    return;
  }
  // mapmsg.cpp::sub_46C5C0 invalidates the 630-pixel-wide system-message row
  // at logical y=291 and expires it after seven seconds.
  CommandControl shadow{};
  shadow.left = 13;
  shadow.top = 292;
  shadow.right = 629;
  shadow.bottom = 306;
  draw_status_text_gl(state, shadow, status->system_message, 0, 0, 0);
  --shadow.left;
  --shadow.top;
  --shadow.right;
  --shadow.bottom;
  draw_status_text_gl(state, shadow, status->system_message, 255, 72, 48);
}

void advance_resource_display(BootstrapStatus& status) noexcept {
  // statres.cpp::sub_4ABC80 at 0x004ABC80 eases each displayed resource
  // counter by delta/16 plus its sign instead of snapping to the live total.
  const auto approach = [](std::uint32_t& displayed, const std::uint32_t actual) {
    if (displayed == actual) {
      return;
    }
    const std::int64_t delta = static_cast<std::int64_t>(actual) - displayed;
    const std::int64_t step = delta / 16 + (delta <= 0 ? -1 : 1);
    const std::int64_t next = static_cast<std::int64_t>(displayed) + step;
    displayed = static_cast<std::uint32_t>((std::clamp)(
        next,
        static_cast<std::int64_t>(0),
        static_cast<std::int64_t>(UINT32_MAX)));
  };
  approach(status.displayed_gas, status.player_gas);
  approach(status.displayed_minerals, status.player_minerals);
}

std::array<std::uint32_t, 2> local_supply(
    const BootstrapStatus& status) noexcept {
  if (!status.unit_traits_ready || status.local_race >= 3U) {
    return {{0U, 0U}};
  }
  starcraft::lang::UnitTraitsTable effective_traits = status.unit_traits;
  starcraft::lang::MeleeUnitTypes local_types{};
  if (!starcraft::lang::melee_unit_types(status.local_race, local_types)) {
    return {{0U, 0U}};
  }
  // Explicit requested retail compatibility override; the licensed beta DAT
  // predates the ten-supply starting-base balance.
  effective_traits[local_types.base].supply_provided =
      starcraft::lang::retail_melee_base_supply_internal;
  starcraft::lang::UnitCountState counts{};
  for (auto& race : counts.supply.maximum) {
    race.fill(400U);
  }
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive || unit.unit_type >= effective_traits.size() ||
        unit.owner >= starcraft::lang::kPlayerCount) {
      continue;
    }
    starcraft::game::Unit counted{};
    counted.owner = unit.owner;
    counted.unit_type = unit.unit_type;
    if (unit.construction_complete) {
      counted.status_flags |= starcraft::game::unit_status_complete;
    }
    starcraft::lang::mark_unit_counted_as_existing(
        counted, effective_traits, counts);
    if (unit.construction_complete) {
      starcraft::lang::mark_unit_counted_as_complete(
          counted, false, effective_traits, counts);
    }
  }
  const auto race = static_cast<starcraft::lang::Race>(status.local_race);
  std::uint32_t used = starcraft::lang::supply_used(race, 0U, counts.supply);
  for (const ScenarioUnitPreview& producer : status.units) {
    if (!producer.alive || producer.owner != 0U) {
      continue;
    }
    for (std::size_t index = 0; index < producer.production_queue.count(); ++index) {
      const std::uint16_t type = producer.production_queue.at(index);
      if (type < effective_traits.size()) {
        used += effective_traits[type].supply_required;
      }
    }
  }
  return {{
      used,
      starcraft::lang::supply_provided_capped(race, 0U, counts.supply),
  }};
}

void draw_resource_strip_gl(const RecoveryWindowState& state) {
  const BootstrapStatus* const status = state.status;
  if (status == nullptr || !status->resource_panel_ready) {
    return;
  }
  // game\icons.grp is the raw-frame group loaded by statres.cpp::sub_4AB900.
  // Frame 0 is minerals; frames 1..3 are Zerg/Terran/Protoss gas; frames
  // 4..6 are the matching supply icons. CHK race values use the same order.
  if (status->resource_icons_ready && status->local_race < 3U &&
      status->resource_icon_frames.size() >= 7U) {
    const auto draw_icon = [](const SpritePreviewFrame& frame,
                              const CommandControl& control) {
      draw_preview_frame_gl(
          frame,
          static_cast<float>(control.left + frame.x_offset),
          static_cast<float>(control.top + frame.y_offset) * hud_vertical_scale(),
          static_cast<float>(frame.width),
          static_cast<float>(frame.height) * hud_vertical_scale());
    };
    draw_icon(status->resource_icon_frames[0], status->resource_mineral_control);
    draw_icon(
        status->resource_icon_frames[1U + status->local_race],
        status->resource_gas_control);
    draw_icon(
        status->resource_icon_frames[4U + status->local_race],
        status->resource_supply_control);
  }
  const auto draw_right_aligned = [&state](
                                      const CommandControl& source,
                                      const std::string& text,
                                      const std::uint8_t red,
                                      const std::uint8_t green,
                                      const std::uint8_t blue) {
    CommandControl control = source;
    control.left = static_cast<std::int16_t>((std::max)(
        static_cast<int>(control.left),
        static_cast<int>(control.right) - static_cast<int>(text.size()) * 6));
    draw_status_text_gl(state, control, text, red, green, blue);
  };
  char value[32]{};
  const std::array<std::uint32_t, 2> supply = local_supply(*status);
  std::snprintf(
      value,
      sizeof(value),
      "%u/%u",
      (supply[0] + 1U) >> 1U,
      (supply[1] + 1U) >> 1U);
  draw_right_aligned(
      status->resource_supply_control,
      value,
      supply[0] <= supply[1] ? 220 : 255,
      supply[0] <= supply[1] ? 220 : 72,
      supply[0] <= supply[1] ? 220 : 48);
  std::snprintf(value, sizeof(value), "%u", status->displayed_gas);
  draw_right_aligned(status->resource_gas_control, value, 96, 230, 128);
  std::snprintf(value, sizeof(value), "%u", status->displayed_minerals);
  draw_right_aligned(status->resource_mineral_control, value, 80, 220, 255);

}

std::vector<std::uint32_t> translated_wireframe(
    const BootstrapStatus& status,
    const SpritePreviewFrame& frame,
    const ScenarioUnitPreview& unit) {
  // statwire.cpp::sub_4AC130/sub_4AC2B0 use the original tables at
  // 0x00501B08 and 0x00501B68. Their entries index the 24 bytes decoded from
  // game\twire.pcx and replace GRP palette ranges 208..211 and 216..219.
  constexpr std::array<std::uint8_t, 40> life_colors{{
      10, 10, 10, 10, 10, 10, 10, 0, 10, 10, 0, 0, 10, 10, 0, 1,
      10, 10, 1, 1, 10, 0, 1, 1, 10, 1, 1, 1, 0, 1, 1, 1,
      0, 1, 1, 1, 1, 1, 1, 1,
  }};
  constexpr std::array<std::uint8_t, 24> body_colors{{
      14, 6, 7, 8, 19, 11, 13, 6, 19, 11, 10, 16,
      1, 18, 20, 16, 1, 18, 20, 12, 0, 1, 18, 10,
  }};
  if (status.hud_palette.size() != 1024 ||
      status.wireframe_color_table.size() != 24 ||
      frame.palette_indices.size() != frame.bgra.size() ||
      frame.opacity.size() != frame.bgra.size() || unit.max_hit_points == 0) {
    return frame.bgra;
  }
  std::vector<std::uint32_t> translated = frame.bgra;
  const std::uint32_t rounded_life = (unit.hit_points + 255U) >> 8U;
  const std::uint32_t rounded_maximum = (std::max)(1U, unit.max_hit_points >> 8U);
  const std::size_t life_level = (std::min)(
      9U, static_cast<unsigned>(9U * rounded_life / rounded_maximum));
  const std::size_t body_level = (std::min)(
      5U, static_cast<unsigned>(5U * rounded_life / rounded_maximum));
  for (std::size_t pixel = 0; pixel < translated.size(); ++pixel) {
    if (frame.opacity[pixel] == 0) {
      continue;
    }
    const std::uint8_t source = frame.palette_indices[pixel];
    std::uint8_t table_index{0xFFU};
    if (source >= 208U && source <= 211U) {
      table_index = life_colors[4U * life_level + source - 208U];
    } else if (source >= 216U && source <= 219U) {
      table_index = body_colors[4U * body_level + source - 216U];
    }
    if (table_index >= status.wireframe_color_table.size()) {
      continue;
    }
    const std::uint8_t palette_index = status.wireframe_color_table[table_index];
    const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
    const std::uint32_t red = status.hud_palette[color];
    const std::uint32_t green = status.hud_palette[color + 1U];
    const std::uint32_t blue = status.hud_palette[color + 2U];
    translated[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
  }
  return translated;
}

void draw_wireframe_in_control_gl(
    const BootstrapStatus& status,
    const ScenarioUnitPreview& unit,
    const SpritePreviewFrame& frame,
    const CommandControl& control) {
  const std::vector<std::uint32_t> translated =
      translated_wireframe(status, frame, unit);
  draw_preview_frame_gl(
      frame,
      static_cast<float>(control.left + frame.x_offset),
      static_cast<float>(control.top + frame.y_offset) * hud_vertical_scale(),
      static_cast<float>(frame.width),
      static_cast<float>(frame.height) * hud_vertical_scale(),
      translated.empty() ? nullptr : translated.data());
}

void draw_status_slot_background_gl(
    const BootstrapStatus& status,
    const CommandControl& control) {
  if (!status.command_panel_frames.empty()) {
    const SpritePreviewFrame& panel = status.command_panel_frames.front();
    draw_preview_frame_gl(
        panel,
        static_cast<float>(control.left),
        static_cast<float>(control.top) * hud_vertical_scale(),
        static_cast<float>(control.right - control.left + 1),
        static_cast<float>(control.bottom - control.top + 1) * hud_vertical_scale());
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(24, 50, 96, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2f(control.left, control.top * hud_vertical_scale());
  glVertex2f(control.right + 1.0F, control.top * hud_vertical_scale());
  glVertex2f(control.right + 1.0F, (control.bottom + 1.0F) * hud_vertical_scale());
  glVertex2f(control.left, (control.bottom + 1.0F) * hud_vertical_scale());
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

void draw_status_progress_gl(
    const BootstrapStatus& status,
    const CommandControl& control,
    const float fraction) {
  const float x = static_cast<float>(control.left);
  const float y = static_cast<float>(control.top) * hud_vertical_scale();
  const float width = static_cast<float>(control.right - control.left + 1);
  const float height =
      static_cast<float>(control.bottom - control.top + 1) * hud_vertical_scale();
  if (status.status_progress_art_ready) {
    draw_preview_frame_gl(status.status_progress_empty, x, y, width, height);
    draw_preview_frame_fraction_gl(
        status.status_progress_full, x, y, width, height, fraction);
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(5, 12, 20, 255);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
  glColor4ub(32, 190, 64, 255);
  glBegin(GL_QUADS);
  glVertex2f(x + 1.0F, y + 1.0F);
  glVertex2f(x + 1.0F + (width - 2.0F) * (std::clamp)(fraction, 0.0F, 1.0F), y + 1.0F);
  glVertex2f(x + 1.0F + (width - 2.0F) * (std::clamp)(fraction, 0.0F, 1.0F), y + height - 1.0F);
  glVertex2f(x + 1.0F, y + height - 1.0F);
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

const char* active_order_text(const ActiveUnitOrder order) noexcept {
  switch (order) {
    case ActiveUnitOrder::move:
      return "Moving";
    case ActiveUnitOrder::attack:
      return "Attacking";
    case ActiveUnitOrder::repair:
      return "Repairing";
    case ActiveUnitOrder::construct:
      return "Building";
    case ActiveUnitOrder::gather:
      return "Gathering";
    case ActiveUnitOrder::return_cargo:
      return "Returning Cargo";
    default:
      return "Idle";
  }
}

void draw_selected_status_panel_gl(
    const RecoveryWindowState& state,
    const std::uint32_t now) {
  const BootstrapStatus* const status = state.status;
  if (status == nullptr || !status->status_panel_ready) {
    return;
  }
  std::array<const ScenarioUnitPreview*, 12> selected{};
  std::size_t selected_count{};
  for (const ScenarioUnitPreview& unit : status->units) {
    if (unit.alive && unit.selected && selected_count < selected.size()) {
      selected[selected_count++] = &unit;
    }
  }
  if (selected_count == 0) {
    return;
  }
  if (selected_count > 1) {
    if (!status->group_wireframe_ready) {
      return;
    }
    for (std::size_t index = 0; index < selected_count; ++index) {
      const ScenarioUnitPreview& unit = *selected[index];
      if (unit.unit_type >= status->group_wireframe_frames.size()) {
        continue;
      }
      const CommandControl& control = status->status_selection_controls[index];
      draw_status_slot_background_gl(*status, control);
      draw_wireframe_in_control_gl(
          *status,
          unit,
          status->group_wireframe_frames[unit.unit_type],
          control);
    }
    return;
  }

  const ScenarioUnitPreview& unit = *selected.front();
  if (status->wireframe_ready && unit.unit_type < status->wireframe_frames.size()) {
    draw_wireframe_in_control_gl(
        *status,
        unit,
        status->wireframe_frames[unit.unit_type],
        status->status_wireframe_control);
  }
  draw_status_text_gl(
      state,
      status->status_name_control,
      status_text(*status, static_cast<std::uint16_t>(unit.unit_type + 1U)),
      180,
      210,
      255);

  char health[48]{};
  const std::uint32_t current_life = (unit.hit_points + 255U) >> 8U;
  const std::uint32_t maximum_life = (std::max)(1U, unit.max_hit_points >> 8U);
  std::snprintf(health, sizeof(health), "%u/%u", current_life, maximum_life);
  const bool high_life = unit.hit_points * 3U >= unit.max_hit_points * 2U;
  const bool medium_life = unit.hit_points * 3U >= unit.max_hit_points;
  draw_status_text_gl(
      state,
      status->status_health_control,
      health,
      high_life ? 32 : 255,
      high_life ? 230 : (medium_life ? 205 : 64),
      48);

  char auxiliary[64]{};
  if (unit.unit_type >= 176U && unit.unit_type <= 178U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Minerals: %u", unit.resource_amount);
  } else if (unit.unit_type == 188U || unit.unit_type == 110U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Vespene Gas: %u", unit.resource_amount);
  } else if (unit.cargo_minerals != 0U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Minerals: %u", unit.cargo_minerals);
  } else if (unit.cargo_gas != 0U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Vespene Gas: %u", unit.cargo_gas);
  } else {
    std::snprintf(auxiliary, sizeof(auxiliary), "Armor: %u", unit.armor);
  }
  draw_status_text_gl(state, status->status_aux_control, auxiliary, 96, 170, 255);

  if (!unit.construction_complete && unit.construction_ticks_total != 0U) {
    draw_status_text_gl(
        state,
        status->status_construction_label_control,
        status_text(*status, 750),
        240,
        210,
        96);
    const float complete = 1.0F -
                           static_cast<float>(unit.construction_ticks_remaining) /
                               unit.construction_ticks_total;
    draw_status_progress_gl(
        *status, status->status_construction_progress_control, complete);
    return;
  }

  if (unit.production_active && !unit.production_queue.empty()) {
    draw_status_text_gl(
        state,
        status->status_action_label_control,
        status_text(*status, 725),
        240,
        210,
        96);
    for (std::size_t index = 0; index < unit.production_queue.count(); ++index) {
      const std::uint16_t queued_type = unit.production_queue.at(index);
      const CommandControl& control = status->status_queue_controls[index];
      draw_status_slot_background_gl(*status, control);
      if (queued_type < status->command_icon_frames.size()) {
        draw_preview_frame_gl(
            status->command_icon_frames[queued_type],
            static_cast<float>(control.left),
            static_cast<float>(control.top) * hud_vertical_scale(),
            static_cast<float>(control.right - control.left + 1),
            static_cast<float>(control.bottom - control.top + 1) *
                hud_vertical_scale());
      }
    }
    const std::uint16_t queued_type = unit.production_queue.front();
    const std::uint32_t duration =
        queued_type < status->runtime_unit_types.size()
            ? static_cast<std::uint32_t>(starcraft::lang::production_total_ticks(
                  status->runtime_unit_types[queued_type]
                      .initialization.simulation.build_time)) *
                  kSimulationTickMilliseconds
            : 0U;
    const float progress = duration == 0U
                               ? 0.0F
                               : static_cast<float>((std::min)(
                                     duration, now - unit.production_started)) /
                                     duration;
    draw_status_progress_gl(*status, status->status_progress_control, progress);
    return;
  }

  draw_status_text_gl(
      state,
      status->status_action_label_control,
      active_order_text(unit.active_order),
      175,
      175,
      175);
}

const UnitPortraitAsset* selected_portrait(const BootstrapStatus& status) noexcept {
  const ScenarioUnitPreview* const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return nullptr;
  }
  const auto portrait = std::find_if(
      status.portraits.begin(),
      status.portraits.end(),
      [selected](const UnitPortraitAsset& asset) {
        return asset.unit_type == selected->unit_type && asset.owner == selected->owner &&
               asset.portrait.ready;
      });
  return portrait == status.portraits.end() ? nullptr : &*portrait;
}

UnitPortraitAsset* selected_portrait(BootstrapStatus& status) noexcept {
  const ScenarioUnitPreview* const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return nullptr;
  }
  const auto portrait = std::find_if(
      status.portraits.begin(),
      status.portraits.end(),
      [selected](const UnitPortraitAsset& asset) {
        return asset.unit_type == selected->unit_type && asset.owner == selected->owner &&
               asset.portrait.ready;
      });
  return portrait == status.portraits.end() ? nullptr : &*portrait;
}

void draw_selected_portrait_gl(const BootstrapStatus& status) {
  const UnitPortraitAsset* const asset = selected_portrait(status);
  if (!status.portrait_panel_ready || asset == nullptr) {
    return;
  }
  constexpr float vertical_scale = 400.0F / 480.0F;
  const CommandControl& control = status.portrait_control;
  draw_preview_frame_gl(
      asset->portrait.frame,
      static_cast<float>(control.left),
      static_cast<float>(control.top) * vertical_scale,
      static_cast<float>(control.right - control.left + 1),
      static_cast<float>(control.bottom - control.top + 1) * vertical_scale);
}

bool advance_selected_portrait(
    BootstrapStatus& status,
    const std::uint32_t now) noexcept {
  UnitPortraitAsset* const asset = selected_portrait(status);
  if (asset == nullptr || asset->portrait.decoder == nullptr ||
      now - asset->portrait.last_frame_tick < asset->portrait.frame_delay_ms) {
    return false;
  }
  char result = smk_next(asset->portrait.decoder);
  if (result == SMK_DONE) {
    result = smk_first(asset->portrait.decoder);
  }
  if (result == SMK_ERROR || result == SMK_DONE ||
      !decode_smacker_frame(asset->portrait)) {
    asset->portrait.ready = false;
    return false;
  }
  asset->portrait.last_frame_tick = now;
  return true;
}

void draw_minimap_gl(const BootstrapStatus& status) {
  if (!status.minimap_ready || status.minimap.width != 128U ||
      status.minimap.height != 128U || status.scenario_width == 0 ||
      status.scenario_height == 0) {
    return;
  }
  constexpr float vertical_scale = 400.0F / 480.0F;
  const float left = static_cast<float>(status.minimap_control.left);
  const float top = static_cast<float>(status.minimap_control.top) * vertical_scale;
  const float width = static_cast<float>(status.minimap_control.right -
                                         status.minimap_control.left + 1);
  const float height = static_cast<float>(status.minimap_control.bottom -
                                          status.minimap_control.top + 1) *
                       vertical_scale;
  draw_preview_frame_gl(status.minimap, left, top, width, height);

  const float content_left = left + status.minimap_content_x * width / 128.0F;
  const float content_top = top + status.minimap_content_y * height / 128.0F;
  const float content_width = status.minimap_content_width * width / 128.0F;
  const float content_height = status.minimap_content_height * height / 128.0F;
  const float map_width = static_cast<float>(status.scenario_width) * 32.0F;
  const float map_height = static_cast<float>(status.scenario_height) * 32.0F;
  glDisable(GL_TEXTURE_2D);
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive) {
      continue;
    }
    std::uint8_t red{224};
    std::uint8_t green{224};
    std::uint8_t blue{224};
    if (status.team_colors_ready && unit.owner < status.team_color_indices.size() &&
        status.game_palette.size() == 1024) {
      const std::uint8_t palette_index = status.team_color_indices[unit.owner][3];
      const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
      red = status.game_palette[color];
      green = status.game_palette[color + 1U];
      blue = status.game_palette[color + 2U];
    }
    if (unit.selected) {
      red = green = blue = 255;
    }
    const float x = content_left + unit.x * content_width / map_width;
    const float y = content_top + unit.y * content_height / map_height;
    const float radius = unit.is_building ? 1.5F : 1.0F;
    glColor4ub(red, green, blue, 255);
    glBegin(GL_QUADS);
    glVertex2f(x - radius, y - radius);
    glVertex2f(x + radius, y - radius);
    glVertex2f(x + radius, y + radius);
    glVertex2f(x - radius, y + radius);
    glEnd();
  }

  const float camera_left =
      content_left + status.camera_x * content_width / map_width;
  const float camera_top =
      content_top + status.camera_y * content_height / map_height;
  const float camera_right = content_left +
                             (status.camera_x + kMapViewportWidth) *
                                 content_width / map_width;
  const float camera_bottom = content_top +
                              (status.camera_y + kMapViewportHeight) *
                                  content_height / map_height;
  glColor4ub(255, 255, 255, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2f(camera_left, camera_top);
  glVertex2f(camera_right, camera_top);
  glVertex2f(camera_right, camera_bottom);
  glVertex2f(camera_left, camera_bottom);
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

void draw_hud_control_frame_gl(
    const SpritePreviewFrame& frame,
    const CommandControl& control) {
  constexpr float vertical_scale = 400.0F / 480.0F;
  draw_preview_frame_gl(
      frame,
      static_cast<float>(control.left + frame.x_offset),
      static_cast<float>(control.top + frame.y_offset) * vertical_scale,
      static_cast<float>(frame.width),
      static_cast<float>(frame.height) * vertical_scale);
}

void draw_selected_command_panel_gl(const RecoveryWindowState& state) {
  const BootstrapStatus* const status = state.status;
  if (status == nullptr || !status->command_panel_ready ||
      status->command_panel_frames.empty()) {
    return;
  }
  const CommandCardView card = command_card_for(*status);
  for (std::size_t index = 0; index < card.count; ++index) {
    const CommandButtonVisual& button = card.buttons[index];
    if (button.position == 0 || button.position > status->command_controls.size() ||
        button.icon >= status->command_icon_frames.size()) {
      continue;
    }
    const CommandControl& control = status->command_controls[button.position - 1U];
    const std::size_t panel_frame =
        state.pressed_command_position == button.position &&
                status->command_panel_frames.size() > 1
            ? 1U
            : 0U;
    draw_hud_control_frame_gl(status->command_panel_frames[panel_frame], control);
    draw_hud_control_frame_gl(status->command_icon_frames[button.icon], control);
  }
}

void shutdown_audio(RecoveryWindowState& state) noexcept {
  if (state.audio_context != nullptr) {
    (void)alcMakeContextCurrent(state.audio_context);
    if (state.music_source != 0U) {
      alSourceStop(state.music_source);
      alDeleteSources(1, &state.music_source);
      state.music_source = 0U;
    }
    if (state.audio_source != 0U) {
      alSourceStop(state.audio_source);
      alDeleteSources(1, &state.audio_source);
      state.audio_source = 0U;
    }
    if (state.resource_error_buffers[0] != 0U ||
        state.resource_error_buffers[1] != 0U) {
      alDeleteBuffers(
          static_cast<ALsizei>(state.resource_error_buffers.size()),
          state.resource_error_buffers.data());
      state.resource_error_buffers.fill(0U);
    }
    if (!state.archived_sound_buffers.empty()) {
      alDeleteBuffers(
          static_cast<ALsizei>(state.archived_sound_buffers.size()),
          state.archived_sound_buffers.data());
      state.archived_sound_buffers.clear();
    }
    if (state.music_buffer != 0U) {
      alDeleteBuffers(1, &state.music_buffer);
      state.music_buffer = 0U;
    }
    (void)alcMakeContextCurrent(nullptr);
    alcDestroyContext(state.audio_context);
    state.audio_context = nullptr;
  }
  if (state.audio_device != nullptr) {
    (void)alcCloseDevice(state.audio_device);
    state.audio_device = nullptr;
  }
  state.audio_ready = false;
  state.music_playing = false;
}

bool initialize_audio(RecoveryWindowState& state) noexcept {
  if (state.status == nullptr) {
    return false;
  }
  std::array<PcmWaveView, 2> waves{};
  for (std::size_t index = 0; index < waves.size(); ++index) {
    if (!parse_pcm_wave(state.status->resource_error_waves[index], waves[index])) {
      return false;
    }
  }
  std::vector<PcmWaveView> archived_waves(state.status->archived_sounds.size());
  for (std::size_t index = 0; index < archived_waves.size(); ++index) {
    if (!parse_pcm_wave(
            state.status->archived_sounds[index].wave, archived_waves[index])) {
      return false;
    }
  }
  PcmWaveView music_wave{};
  if (!parse_pcm_wave(state.status->music_wave, music_wave)) {
    return false;
  }
  state.audio_device = alcOpenDevice(nullptr);
  if (state.audio_device == nullptr) {
    return false;
  }
  state.audio_context = alcCreateContext(state.audio_device, nullptr);
  if (state.audio_context == nullptr || !alcMakeContextCurrent(state.audio_context)) {
    shutdown_audio(state);
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }
  alGenBuffers(
      static_cast<ALsizei>(state.resource_error_buffers.size()),
      state.resource_error_buffers.data());
  for (std::size_t index = 0; index < waves.size(); ++index) {
    alBufferData(
        state.resource_error_buffers[index],
        waves[index].format,
        waves[index].samples,
        waves[index].sample_bytes,
        waves[index].sample_rate);
  }
  state.archived_sound_buffers.resize(archived_waves.size());
  if (!state.archived_sound_buffers.empty()) {
    alGenBuffers(
        static_cast<ALsizei>(state.archived_sound_buffers.size()),
        state.archived_sound_buffers.data());
  }
  for (std::size_t index = 0; index < archived_waves.size(); ++index) {
    alBufferData(
        state.archived_sound_buffers[index],
        archived_waves[index].format,
        archived_waves[index].samples,
        archived_waves[index].sample_bytes,
        archived_waves[index].sample_rate);
  }
  alGenBuffers(1, &state.music_buffer);
  alBufferData(
      state.music_buffer,
      music_wave.format,
      music_wave.samples,
      music_wave.sample_bytes,
      music_wave.sample_rate);
  alGenSources(1, &state.audio_source);
  alSourcei(state.audio_source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcef(state.audio_source, AL_ROLLOFF_FACTOR, 0.0F);
  alSourcef(state.audio_source, AL_GAIN, 1.0F);
  alGenSources(1, &state.music_source);
  alSourcei(state.music_source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcef(state.music_source, AL_ROLLOFF_FACTOR, 0.0F);
  alSourcef(state.music_source, AL_GAIN, 0.45F);
  alSourcei(state.music_source, AL_LOOPING, AL_TRUE);
  alSourcei(
      state.music_source, AL_BUFFER, static_cast<ALint>(state.music_buffer));
  state.audio_ready = alGetError() == AL_NO_ERROR && state.audio_source != 0U &&
                      state.resource_error_buffers[0] != 0U &&
                      state.resource_error_buffers[1] != 0U &&
                      !state.archived_sound_buffers.empty() &&
                      state.music_source != 0U && state.music_buffer != 0U;
  if (!state.audio_ready) {
    shutdown_audio(state);
  } else {
    alSourcePlay(state.music_source);
    ALint music_state{};
    alGetSourcei(state.music_source, AL_SOURCE_STATE, &music_state);
    state.music_playing =
        alGetError() == AL_NO_ERROR && music_state == AL_PLAYING;
  }
  return state.audio_ready;
}

bool play_pending_resource_error(RecoveryWindowState& state) noexcept {
  if (state.status == nullptr ||
      state.status->pending_resource_error_sound >=
          state.resource_error_buffers.size()) {
    return false;
  }
  const std::size_t sound = state.status->pending_resource_error_sound;
  state.status->pending_resource_error_sound = 0xFFU;
  if (!state.audio_ready || state.audio_context == nullptr ||
      !alcMakeContextCurrent(state.audio_context)) {
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }
  alSourceStop(state.audio_source);
  alSourcei(
      state.audio_source,
      AL_BUFFER,
      static_cast<ALint>(state.resource_error_buffers[sound]));
  alSourcePlay(state.audio_source);
  if (alGetError() != AL_NO_ERROR) {
    return false;
  }
  ++state.audio_play_count;
  return true;
}

bool play_pending_game_sound(RecoveryWindowState& state) noexcept {
  if (state.status == nullptr || state.status->pending_game_sound == 0xFFFFU) {
    return false;
  }
  const std::uint16_t sound_id = state.status->pending_game_sound;
  state.status->pending_game_sound = 0xFFFFU;
  const auto sound = std::find_if(
      state.status->archived_sounds.begin(),
      state.status->archived_sounds.end(),
      [sound_id](const ArchivedSoundAsset& asset) {
        return asset.sound_id == sound_id;
      });
  if (sound == state.status->archived_sounds.end()) {
    return false;
  }
  const std::size_t index = static_cast<std::size_t>(
      sound - state.status->archived_sounds.begin());
  if (!state.audio_ready || index >= state.archived_sound_buffers.size() ||
      state.audio_context == nullptr || !alcMakeContextCurrent(state.audio_context)) {
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }
  alSourceStop(state.audio_source);
  alSourcei(
      state.audio_source,
      AL_BUFFER,
      static_cast<ALint>(state.archived_sound_buffers[index]));
  alSourcePlay(state.audio_source);
  if (alGetError() != AL_NO_ERROR) {
    return false;
  }
  state.status->last_game_sound = sound_id;
  ++state.audio_play_count;
  return true;
}

std::uint16_t choose_unit_sound(
    BootstrapStatus& status,
    const std::uint16_t first,
    const std::uint16_t last) noexcept {
  if (first == 0U || last < first) {
    return 0U;
  }
  if (first == last) {
    status.last_game_sound = first;
    return first;
  }
  const std::uint32_t span = static_cast<std::uint32_t>(last - first) + 1U;
  std::uint16_t chosen = static_cast<std::uint16_t>(
      first + (++status.sound_choice_counter % span));
  if (chosen == status.last_game_sound && ++chosen > last) {
    chosen = first;
  }
  status.last_game_sound = chosen;
  return chosen;
}

bool queue_unit_response(
    BootstrapStatus& status,
    const ScenarioUnitPreview& unit,
    const bool order_acknowledgement) noexcept {
  if (!unit.alive || unit.owner != 0 ||
      unit.unit_type >= status.unit_sound_ranges.size()) {
    return false;
  }
  const UnitSoundRanges& ranges = status.unit_sound_ranges[unit.unit_type];
  std::uint16_t first = ranges.what_first;
  std::uint16_t last = ranges.what_last;
  if (order_acknowledgement) {
    first = ranges.yes_first;
    last = ranges.yes_last;
  } else if (status.last_voice_unit_id == unit.unit_id) {
    status.voice_repeat_count = static_cast<std::uint8_t>((std::min)(
        255U, static_cast<unsigned>(status.voice_repeat_count) + 1U));
    // gamesnd.cpp::sub_455860 enters the sequential annoyed range after the
    // fourth repeated selection response, then wraps back to What sounds.
    if (status.voice_repeat_count >= 4U && ranges.annoyed_first != 0U &&
        ranges.annoyed_last >= ranges.annoyed_first) {
      first = ranges.annoyed_first;
      last = ranges.annoyed_last;
      const std::uint32_t span =
          static_cast<std::uint32_t>(last - first) + 1U;
      first = last = static_cast<std::uint16_t>(
          first + (status.voice_repeat_count - 4U) % span);
    }
  } else {
    status.last_voice_unit_id = unit.unit_id;
    status.voice_repeat_count = 0U;
  }
  const std::uint16_t chosen = choose_unit_sound(status, first, last);
  if (chosen == 0U) {
    return false;
  }
  status.pending_game_sound = chosen;
  return true;
}

bool initialize_opengl(const HWND window, RecoveryWindowState& state) noexcept {
  state.device_context = GetDC(window);
  if (state.device_context == nullptr) {
    return false;
  }
  PIXELFORMATDESCRIPTOR descriptor{};
  descriptor.nSize = sizeof(descriptor);
  descriptor.nVersion = 1;
  descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  descriptor.iPixelType = PFD_TYPE_RGBA;
  descriptor.cColorBits = 32;
  descriptor.cAlphaBits = 8;
  descriptor.iLayerType = PFD_MAIN_PLANE;
  const int format = ChoosePixelFormat(state.device_context, &descriptor);
  if (format == 0 || !SetPixelFormat(state.device_context, format, &descriptor)) {
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
    return false;
  }
  state.rendering_context = wglCreateContext(state.device_context);
  if (state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    if (state.rendering_context != nullptr) {
      wglDeleteContext(state.rendering_context);
      state.rendering_context = nullptr;
    }
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
    return false;
  }
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  state.font_display_lists = glGenLists(96);
  const HFONT font = CreateFontA(
      -10,
      0,
      0,
      0,
      FW_NORMAL,
      FALSE,
      FALSE,
      FALSE,
      DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS,
      NONANTIALIASED_QUALITY,
      FIXED_PITCH | FF_DONTCARE,
      "Arial");
  if (state.font_display_lists == 0 || font == nullptr) {
    if (font != nullptr) {
      DeleteObject(font);
    }
    return false;
  }
  const HGDIOBJ previous = SelectObject(state.device_context, font);
  const bool font_ready =
      wglUseFontBitmapsA(state.device_context, 32, 96, state.font_display_lists) != FALSE;
  SelectObject(state.device_context, previous);
  DeleteObject(font);
  return font_ready;
}

void shutdown_opengl(const HWND window, RecoveryWindowState& state) noexcept {
  if (state.rendering_context != nullptr) {
    if (state.font_display_lists != 0) {
      glDeleteLists(state.font_display_lists, 96);
      state.font_display_lists = 0;
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(state.rendering_context);
    state.rendering_context = nullptr;
  }
  if (state.device_context != nullptr) {
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
  }
}

void draw_selection_drag_gl(const RecoveryWindowState& state) noexcept {
  if (!state.selection_dragging) {
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(32, 255, 32, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(state.selection_start_x, state.selection_start_y);
  glVertex2i(state.selection_current_x, state.selection_start_y);
  glVertex2i(state.selection_current_x, state.selection_current_y);
  glVertex2i(state.selection_start_x, state.selection_current_y);
  glEnd();
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

void draw_command_target_gl(const RecoveryWindowState& state) noexcept {
  if (state.status == nullptr || !state.status->command_target_active ||
      !state.mouse_in_client) {
    return;
  }
  // target.cpp::sub_4B0120 at 0x004B0120 switches to the target cursor after
  // preserving the unit-target and terrain-target order IDs. The original
  // cursor bitmap is not decoded yet, so retain the recovered state transition
  // and expose it with a compact OpenGL reticle.
  constexpr int outer_radius = 11;
  constexpr int inner_radius = 4;
  glDisable(GL_TEXTURE_2D);
  glColor4ub(64, 255, 64, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(state.mouse_game_x, state.mouse_game_y - outer_radius);
  glVertex2i(state.mouse_game_x + outer_radius, state.mouse_game_y);
  glVertex2i(state.mouse_game_x, state.mouse_game_y + outer_radius);
  glVertex2i(state.mouse_game_x - outer_radius, state.mouse_game_y);
  glEnd();
  glBegin(GL_LINES);
  glVertex2i(state.mouse_game_x - outer_radius, state.mouse_game_y);
  glVertex2i(state.mouse_game_x - inner_radius, state.mouse_game_y);
  glVertex2i(state.mouse_game_x + inner_radius, state.mouse_game_y);
  glVertex2i(state.mouse_game_x + outer_radius, state.mouse_game_y);
  glVertex2i(state.mouse_game_x, state.mouse_game_y - outer_radius);
  glVertex2i(state.mouse_game_x, state.mouse_game_y - inner_radius);
  glVertex2i(state.mouse_game_x, state.mouse_game_y + inner_radius);
  glVertex2i(state.mouse_game_x, state.mouse_game_y + outer_radius);
  glEnd();
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

bool render_opengl(const HWND window, RecoveryWindowState& state) noexcept {
  if (state.device_context == nullptr || state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    return false;
  }
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) {
    return false;
  }
  while (glGetError() != GL_NO_ERROR) {
  }
  glViewport(0, 0, client.right, client.bottom);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, kMapViewportWidth, kMapViewportHeight, 0.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  const BootstrapStatus* const status = state.status;
  if (status != nullptr && status->terrain_ready) {
    draw_preview_frame_gl(
        status->terrain,
        0.0F,
        0.0F,
        static_cast<float>(kMapViewportWidth),
        static_cast<float>(kMapViewportHeight));
    for (const ScenarioUnitPreview& unit : status->units) {
      draw_scenario_unit_gl(*status, unit);
    }
    draw_building_placement_gl(*status);
    draw_selection_drag_gl(state);
    if (status->hud_ready) {
      draw_preview_frame_gl(
          status->hud,
          0.0F,
          0.0F,
          static_cast<float>(kMapViewportWidth),
          static_cast<float>(kMapViewportHeight));
    }
    draw_resource_strip_gl(state);
    draw_system_message_gl(state, GetTickCount());
    draw_minimap_gl(*status);
    draw_selected_portrait_gl(*status);
    draw_selected_status_panel_gl(state, GetTickCount());
    draw_selected_command_panel_gl(state);
    draw_command_target_gl(state);
  }
  glFlush();
  std::array<std::array<GLint, 2>, 4> sample_points{{
      {{client.right / 4, client.bottom / 4}},
      {{3 * client.right / 4, client.bottom / 4}},
      {{client.right / 4, 3 * client.bottom / 4}},
      {{3 * client.right / 4, 3 * client.bottom / 4}},
  }};
  bool has_colored_pixel{};
  glReadBuffer(GL_BACK);
  for (const auto& point : sample_points) {
    std::array<GLubyte, 4> pixel{};
    glReadPixels(point[0], point[1], 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());
    has_colored_pixel = has_colored_pixel || pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0;
  }
  const bool commands_valid = glGetError() == GL_NO_ERROR;
  return SwapBuffers(state.device_context) != FALSE && commands_valid && has_colored_pixel;
}

bool capture_opengl_bmp(
    const HWND window,
    RecoveryWindowState& state,
    const char* const path) noexcept {
  if (path == nullptr || *path == '\0' || state.device_context == nullptr ||
      state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    return false;
  }
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) {
    return false;
  }
  try {
    const std::size_t byte_count =
        static_cast<std::size_t>(client.right) * client.bottom * 4U;
    if (byte_count > UINT32_MAX) {
      return false;
    }
    std::vector<std::uint8_t> pixels(byte_count);
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(
        0,
        0,
        client.right,
        client.bottom,
        kGlBgra,
        GL_UNSIGNED_BYTE,
        pixels.data());
    if (glGetError() != GL_NO_ERROR) {
      return false;
    }

    BITMAPFILEHEADER file_header{};
    BITMAPINFOHEADER info_header{};
    file_header.bfType = 0x4D42;
    file_header.bfOffBits = sizeof(file_header) + sizeof(info_header);
    file_header.bfSize = file_header.bfOffBits + static_cast<DWORD>(byte_count);
    info_header.biSize = sizeof(info_header);
    info_header.biWidth = client.right;
    info_header.biHeight = client.bottom;
    info_header.biPlanes = 1;
    info_header.biBitCount = 32;
    info_header.biCompression = BI_RGB;
    info_header.biSizeImage = static_cast<DWORD>(byte_count);

    const HANDLE file = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
      return false;
    }
    DWORD written{};
    const bool okay =
        WriteFile(file, &file_header, sizeof(file_header), &written, nullptr) != FALSE &&
        written == sizeof(file_header) &&
        WriteFile(file, &info_header, sizeof(info_header), &written, nullptr) != FALSE &&
        written == sizeof(info_header) &&
        WriteFile(
            file,
            pixels.data(),
            static_cast<DWORD>(pixels.size()),
            &written,
            nullptr) != FALSE &&
        written == pixels.size();
    CloseHandle(file);
    return okay;
  } catch (...) {
    return false;
  }
}

std::filesystem::path executable_directory() {
  std::array<wchar_t, 32768> path{};
  const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
  if (length == 0 || length >= path.size()) {
    return {};
  }
  return std::filesystem::path{path.data(), path.data() + length}.parent_path();
}

std::filesystem::path locate_input_root() {
  try {
    const auto executable = executable_directory();
    std::array<std::filesystem::path, 3> candidates{{
        std::filesystem::current_path(),
        executable,
        executable.parent_path().parent_path().parent_path(),
    }};
    for (const auto& candidate : candidates) {
      if (std::filesystem::is_regular_file(candidate / L"storm.dll") &&
          std::filesystem::is_regular_file(candidate / L"StarDat.mpq")) {
        return candidate;
      }
    }
  } catch (...) {
  }
  return {};
}

bool read_loose_file(
    const std::filesystem::path& path,
    std::vector<std::uint8_t>& output) noexcept {
  output.clear();
  const HANDLE file = CreateFileW(
      path.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
      nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  LARGE_INTEGER size{};
  bool okay = GetFileSizeEx(file, &size) != FALSE && size.QuadPart > 0 &&
              static_cast<unsigned long long>(size.QuadPart) <= SIZE_MAX;
  try {
    if (okay) {
      output.resize(static_cast<std::size_t>(size.QuadPart));
      std::size_t cursor{};
      while (cursor < output.size()) {
        const DWORD request = static_cast<DWORD>((std::min)(
            output.size() - cursor,
            static_cast<std::size_t>(1U << 20U)));
        DWORD received{};
        if (!ReadFile(file, output.data() + cursor, request, &received, nullptr) ||
            received == 0U) {
          okay = false;
          break;
        }
        cursor += received;
      }
    }
  } catch (...) {
    okay = false;
  }
  CloseHandle(file);
  if (!okay) {
    output.clear();
  }
  return okay;
}

bool extract_unit_sound_ranges(
    const starcraft::data::CoreDataSet& data,
    BootstrapStatus& status) noexcept {
  // units.dat descriptor fields 26..32 are the exact arrays used by
  // gamesnd.cpp::sub_455790, sub_455860, and sub_455640. The 106-entry
  // arrays apply only to unit IDs 0..105; What sounds cover all 228 types.
  const starcraft::data::DatField* const ready = data.units().field(26);
  const starcraft::data::DatField* const what_first = data.units().field(27);
  const starcraft::data::DatField* const what_last = data.units().field(28);
  const starcraft::data::DatField* const annoyed_first = data.units().field(29);
  const starcraft::data::DatField* const annoyed_last = data.units().field(30);
  const starcraft::data::DatField* const yes_first = data.units().field(31);
  const starcraft::data::DatField* const yes_last = data.units().field(32);
  if (ready == nullptr || what_first == nullptr || what_last == nullptr ||
      annoyed_first == nullptr || annoyed_last == nullptr || yes_first == nullptr ||
      yes_last == nullptr) {
    return false;
  }
  for (std::size_t type = 0; type < status.unit_sound_ranges.size(); ++type) {
    UnitSoundRanges& ranges = status.unit_sound_ranges[type];
    if (!what_first->value(type, ranges.what_first) ||
        !what_last->value(type, ranges.what_last)) {
      return false;
    }
    if (type < 106U &&
        (!ready->value(type, ranges.ready) ||
         !annoyed_first->value(type, ranges.annoyed_first) ||
         !annoyed_last->value(type, ranges.annoyed_last) ||
         !yes_first->value(type, ranges.yes_first) ||
         !yes_last->value(type, ranges.yes_last))) {
      return false;
    }
  }
  return true;
}

bool cache_unit_sound_assets(
    starcraft::runtime::StormModule& storm,
    const std::vector<std::uint8_t>& sfx_data,
    const std::vector<std::uint8_t>& sfx_table,
    const std::array<bool, starcraft::lang::kUnitTypeCount>& wanted_types,
    BootstrapStatus& status) noexcept {
  constexpr std::size_t sound_count = 944U;
  if (sfx_data.size() != 8496U) {
    return false;
  }
  const starcraft::data::StringTableView paths{sfx_table.data(), sfx_table.size()};
  if (!paths.valid()) {
    return false;
  }
  const auto cache_sound = [&](const std::uint16_t sound_id) -> bool {
    if (sound_id == 0U) {
      return true;
    }
    if (sound_id >= sound_count) {
      return false;
    }
    if (std::any_of(
            status.archived_sounds.begin(),
            status.archived_sounds.end(),
            [sound_id](const ArchivedSoundAsset& asset) {
              return asset.sound_id == sound_id;
            })) {
      return true;
    }
    const std::uint32_t path_id = read_u32(sfx_data, sound_id * 4U);
    if (path_id == 0U || path_id > UINT16_MAX) {
      return false;
    }
    const std::string_view relative = paths.one_based(
        static_cast<std::uint16_t>(path_id));
    if (relative.empty()) {
      return false;
    }
    ArchivedSoundAsset asset{};
    asset.sound_id = sound_id;
    asset.path = R"(sound\)";
    asset.path.append(relative.data(), relative.size());
    if (!storm.load_file(asset.path.c_str(), asset.wave) || asset.wave.empty()) {
      return false;
    }
    status.archived_sounds.push_back(std::move(asset));
    return true;
  };
  const auto cache_range = [&cache_sound, sound_count](
                               const std::uint16_t first,
                               const std::uint16_t last) -> bool {
    if (first == 0U && last == 0U) {
      return true;
    }
    if (first == 0U || last < first || last >= sound_count) {
      return false;
    }
    for (std::uint32_t sound = first; sound <= last; ++sound) {
      if (!cache_sound(static_cast<std::uint16_t>(sound))) {
        return false;
      }
    }
    return true;
  };
  try {
    for (std::size_t type = 0; type < wanted_types.size(); ++type) {
      if (!wanted_types[type]) {
        continue;
      }
      const UnitSoundRanges& ranges = status.unit_sound_ranges[type];
      if (!cache_range(ranges.what_first, ranges.what_last) ||
          !cache_range(ranges.annoyed_first, ranges.annoyed_last) ||
          !cache_range(ranges.yes_first, ranges.yes_last)) {
        return false;
      }
    }
    // The SCV's explicit weapon-8 event creates the cutter projectile. Its
    // image-498 IScript plays one of SFX 23..27 (EDrRep00..04), the exact
    // working sound range used for harvesting and repair.
    return cache_range(23U, 27U) && !status.archived_sounds.empty();
  } catch (...) {
    status.archived_sounds.clear();
    return false;
  }
}

BootstrapStatus probe_assets() {
  BootstrapStatus status{};
  const std::filesystem::path root = locate_input_root();
  if (root.empty()) {
    status.primary = "Licensed input directory was not found.";
    status.detail = "Run beside storm.dll and StarDat.mpq, or from the configured build tree.";
    return status;
  }

  starcraft::runtime::StormModule storm{root / L"storm.dll"};
  if (!storm.loaded()) {
    status.primary = "The supplied storm.dll could not be loaded.";
    return status;
  }

  void* archive{};
  if (!storm.open_archive(root / L"StarDat.mpq", &archive, 1000)) {
    status.primary = "Storm loaded, but StarDat.mpq could not be opened.";
    return status;
  }
  void* patch_archive{};
  const bool patch_opened = storm.open_archive(root / L"patch_rt.mpq", &patch_archive, 2000);
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
  void* map_archive{};
  const bool map_opened = storm.open_archive(map_path, &map_archive, 3000);
  std::vector<std::uint8_t> map_chk;
  const bool map_loaded = map_opened && storm.load_file_from_archive(
                                           map_archive, R"(staredit\scenario.chk)", map_chk);
  const starcraft::data::ChkView chk{map_chk.data(), map_chk.size()};
  starcraft::game::MultiplayerScenario scenario{};
  const bool scenario_loaded = map_loaded && scenario.load(chk);
  // The recovered game keeps g_hScenArchive open, but this bootstrap's
  // transitional global Storm lookup must not search the map MPQ for every
  // DAT/GRP asset. The complete CHK is already resident, so close it now.
  const bool map_closed = map_archive == nullptr || storm.close_archive(map_archive);
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
    for (std::size_t player = 0; player < status.active_players.size(); ++player) {
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
    const std::uint16_t centered_x = focus_x / 32U > 10U ? focus_x / 32U - 10U : 0U;
    const std::uint16_t centered_y = focus_y / 32U > 6U ? focus_y / 32U - 6U : 0U;
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
      status.terrain_ready = build_terrain_preview(
          terrain_tileset,
          scenario,
          status.camera_x,
          status.camera_y,
          status.terrain);
    }
    status.terrain_group_count = terrain_tileset.group_count();
    status.terrain_megatile_count = terrain_tileset.megatile_count();
    status.terrain_minitile_count = terrain_tileset.minitile_count();

    // status.cpp::sub_4ABFC0 at 0x004ABFC0 loads game\%cconsole.pcx.
    // StarCraft.exe's byte table at 0x004F260C is exactly "ztp".
    constexpr std::array<char, 3> race_codes{{'z', 't', 'p'}};
    constexpr std::array<const char*, 3> mineral_error_paths{{
        R"(sound\Zerg\Advisor\ZAdErr00.WAV)",
        R"(sound\Terran\Advisor\tAdErr00.WAV)",
        R"(sound\Protoss\Advisor\PAdErr00.WAV)",
    }};
    constexpr std::array<const char*, 3> gas_error_paths{{
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
      (void)storm.load_file(
          status.resource_error_wave_paths[0].c_str(),
          status.resource_error_waves[0]);
      (void)storm.load_file(
          status.resource_error_wave_paths[1].c_str(),
          status.resource_error_waves[1]);
      // Music.cpp::sub_470720 at 0x00470720 opens the loose path stored in
      // sgSongs[nSong].szName. Gameplay entries in the executable's table are
      // music\terran1.wav (and the parallel Zerg/Protoss tracks). The supplied
      // licensed music is loose beside the MPQs, so try Storm first and then
      // read that exact original filesystem path without modifying it.
      constexpr std::array<const char*, 3> first_gameplay_tracks{{
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
        for (std::size_t owner = 0; owner < status.team_color_indices.size(); ++owner) {
          std::copy_n(
              unit_colors.pixels.begin() + owner * 8U,
              8,
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
        starcraft::gds::grp_frame_count(
            wireframe_group.data(), wireframe_group.size()) == 228 &&
        storm.load_pcx(R"(game\twire.pcx)", wireframe_remap) &&
        wireframe_remap.width == 24 && wireframe_remap.height == 1 &&
        decode_preview_frames(
            wireframe_group,
            ui_palette,
            status.wireframe_frames,
            status.wireframe_canvas_width,
            status.wireframe_canvas_height);
    if (status.wireframe_ready) {
      status.wireframe_color_table = wireframe_remap.pixels;
    }
    // statdata.cpp::sub_4A6200 at 0x004A6200 separately loads grpwire.grp
    // for controls 33..44. Those are the compact 12-selection wireframes;
    // the 64-pixel selected-unit control above continues to use wirefram.grp.
    status.group_wireframe_ready =
        storm.load_file(R"(unit\wirefram\grpwire.grp)", group_wireframe_group) &&
        starcraft::gds::grp_frame_count(
            group_wireframe_group.data(), group_wireframe_group.size()) != 0 &&
        decode_preview_frames(
            group_wireframe_group,
            ui_palette,
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
        starcraft::gds::grp_frame_count(
            resource_icons.data(), resource_icons.size()) == 12U &&
        decode_preview_frames(
            resource_icons,
            ui_palette,
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
          decode_pcx_frame(empty_progress, false, status.status_progress_empty) &&
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
        normal_icon_remap.assign(
            icon_remap.pixels.begin(), icon_remap.pixels.begin() + 16);
      }
      status.command_panel_ready =
          storm.load_file(panel_path.c_str(), panel_group) &&
          storm.load_file(R"(unit\cmdbtns\cmdicons.grp)", icon_group) &&
          storm.load_file(layout_path.c_str(), layout) &&
          icon_remap_loaded &&
          decode_preview_frames(
              panel_group,
              ui_palette,
              status.command_panel_frames,
              panel_width,
              panel_height) &&
          decode_preview_frames(
              icon_group,
              ui_palette,
              status.command_icon_frames,
              icon_width,
              icon_height,
              &normal_icon_remap) &&
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
      for (const starcraft::game::ScenarioUnit& unit : scenario.units()) {
        std::uint16_t unit_image{};
        if (unit.owner == 0 && unit.x >= view_left && unit.x < view_left + kMapViewportWidth &&
            unit.y >= view_top && unit.y < view_top + kMapViewportHeight &&
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
      const auto existing = std::find_if(
          status.unit_assets.begin(),
          status.unit_assets.end(),
          [image_id](const UnitRenderAsset& asset) { return asset.image_id == image_id; });
      if (existing != status.unit_assets.end()) {
        return static_cast<std::size_t>(existing - status.unit_assets.begin());
      }
      UnitRenderAsset asset{};
      if (!load_unit_render_asset(
              storm,
              data,
              palette,
              status.iscript_bytes,
              image_id,
              scenario.tileset_id(),
              asset)) {
        return SIZE_MAX;
      }
      status.unit_assets.push_back(std::move(asset));
      return status.unit_assets.size() - 1U;
    };

    const auto ensure_runtime_unit_type =
        [&](const std::uint16_t unit_type) -> RuntimeUnitType* {
      if (unit_type >= status.runtime_unit_types.size()) {
        return nullptr;
      }
      RuntimeUnitType& runtime = status.runtime_unit_types[unit_type];
      if (runtime.ready) {
        return &runtime;
      }
      starcraft::lang::UnitInitializationData initialization{};
      if (!starcraft::lang::recover_unit_initialization(
              data, unit_type, initialization)) {
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
        const UnitRenderAsset& focus = status.unit_assets[status.focus_asset_index];
        status.image_draw_function = focus.image_draw_function;
        status.iscript_id = focus.iscript_id;
      }
    }

    const auto append_unit_preview = [&](const std::uint16_t unit_type,
                                         const std::uint16_t x,
                                         const std::uint16_t y,
                                         const std::uint8_t owner) -> bool {
      RuntimeUnitType* const runtime = ensure_runtime_unit_type(unit_type);
      if (runtime == nullptr || runtime->asset_index >= status.unit_assets.size()) {
        return false;
      }
      const starcraft::lang::UnitInitializationData& initialization =
          runtime->initialization;
      const UnitRenderAsset& asset = status.unit_assets[runtime->asset_index];
      ScenarioUnitPreview preview{};
      preview.x = x;
      preview.y = y;
      preview.unit_type = unit_type;
      preview.owner = owner;
      preview.asset_index = runtime->asset_index;
      preview.selection_width = initialization.placement_width;
      preview.selection_height = initialization.placement_height;
      preview.collision_left = static_cast<std::uint16_t>(initialization.collision.left);
      preview.collision_top = static_cast<std::uint16_t>(initialization.collision.top);
      preview.collision_right = static_cast<std::uint16_t>(initialization.collision.right);
      preview.collision_bottom = static_cast<std::uint16_t>(initialization.collision.bottom);
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
      for (const starcraft::game::ScenarioUnit& unit : scenario.units()) {
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
          data.unit_placement_size(
              types.base,
              base_dimensions[race].width,
              base_dimensions[race].height) &&
          melee_start_ready;
    }
    std::vector<starcraft::lang::ExistingMeleeUnit> existing_melee_units;
    existing_melee_units.reserve(status.units.size());
    for (const ScenarioUnitPreview& unit : status.units) {
      existing_melee_units.push_back({unit.unit_type, unit.owner, unit.alive});
    }
    std::vector<starcraft::lang::MeleeStartSpawn> melee_spawns;
    melee_start_ready =
        melee_start_ready &&
        starcraft::lang::build_retail_melee_start_plan(
            scenario.players(),
            scenario.start_locations(),
            existing_melee_units.data(),
            existing_melee_units.size(),
            base_dimensions,
            melee_spawns);
    for (const starcraft::lang::MeleeStartSpawn& spawn : melee_spawns) {
      melee_start_ready =
          append_unit_preview(
              spawn.unit_type, spawn.x, spawn.y, spawn.owner) &&
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
         producer_index < production_producers.count;
         ++producer_index) {
      const std::uint16_t producer_type =
          production_producers.unit_types[producer_index];
      if (ensure_runtime_unit_type(producer_type) == nullptr) {
        status.failed_runtime_unit_type = producer_type;
        production_assets_ready = false;
      }
      const starcraft::lang::UnitProductionButtonView buttons =
          starcraft::lang::production_buttons_for(producer_type);
      for (std::size_t index = 0; index < buttons.count; ++index) {
        if (ensure_runtime_unit_type(buttons.buttons[index].product_type) == nullptr) {
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
      const std::uint16_t town_hall_type = larva_sources.unit_types[source_index];
      if (ensure_runtime_unit_type(town_hall_type) == nullptr) {
        status.failed_runtime_unit_type = town_hall_type;
        production_assets_ready = false;
      }
    }

    std::uint16_t scv_image{};
    std::uint16_t scv_build_time{};
    if (data.unit_image_id(7, scv_image)) {
      status.scv_asset_index = ensure_asset(scv_image);
      scv_asset_ready = status.scv_asset_index != SIZE_MAX &&
                         data.unit_placement_size(
                             7, status.scv_selection_width, status.scv_selection_height) &&
                         data.unit_build_time(7, scv_build_time) &&
                         starcraft::lang::production_total_ticks(scv_build_time) != 0U &&
                         data.unit_movement_traits(7, status.scv_movement) &&
                         data.unit_simulation_traits(7, status.scv_simulation);
    }
    std::uint16_t geyser_image{};
    if (data.unit_image_id(188, geyser_image)) {
      status.geyser_asset_index = ensure_asset(geyser_image);
      geyser_asset_ready = status.geyser_asset_index != SIZE_MAX &&
                           data.unit_placement_size(
                               188,
                               status.geyser_selection_width,
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
    build_assets_ready = terran_buildables.count == status.buildable_units.size();
    for (std::size_t index = 0;
         build_assets_ready && index < terran_buildables.count;
         ++index) {
      BuildableUnitVisual& buildable = status.buildable_units[index];
      buildable.unit_type = terran_buildables.unit_types[index];
      std::uint16_t image_id{};
      build_assets_ready =
          data.unit_placement_size(
              buildable.unit_type,
              buildable.placement_width,
              buildable.placement_height) &&
          data.unit_image_id(buildable.unit_type, image_id) && build_assets_ready;
      if (!build_assets_ready) {
        break;
      }
      buildable.asset_index = ensure_asset(image_id);
      if (buildable.asset_index == SIZE_MAX) {
        build_assets_ready = false;
        break;
      }
      if (!data.unit_simulation_traits(buildable.unit_type, buildable.simulation)) {
        build_assets_ready = false;
        break;
      }
      if ((buildable.simulation.dat_flags & 2U) != 0U) {
        buildable.addon_parent_type =
            starcraft::lang::terran_addon_parent_type(buildable.unit_type);
        if (buildable.addon_parent_type == 0xFFFFU ||
            !data.unit_addon_position(
                buildable.unit_type, buildable.addon_x, buildable.addon_y)) {
          build_assets_ready = false;
          break;
        }
      }
    }

    const auto ensure_portrait = [&](const std::uint16_t unit_type,
                                     const std::uint8_t owner) -> bool {
      const auto existing = std::find_if(
          status.portraits.begin(),
          status.portraits.end(),
          [unit_type, owner](const UnitPortraitAsset& asset) {
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
    for (const ScenarioUnitPreview& unit : status.units) {
      (void)ensure_portrait(unit.unit_type, unit.owner);
    }
    for (const BuildableUnitVisual& buildable : status.buildable_units) {
      (void)ensure_portrait(buildable.unit_type, 0);
    }
    for (std::size_t type = 0; type < status.runtime_unit_types.size(); ++type) {
      if (status.runtime_unit_types[type].ready) {
        (void)ensure_portrait(static_cast<std::uint16_t>(type), 0);
      }
    }

    std::array<bool, starcraft::lang::kUnitTypeCount> wanted_sound_types{};
    wanted_sound_types[7] = true;
    wanted_sound_types[106] = true;
    for (const ScenarioUnitPreview& unit : status.units) {
      if (unit.unit_type < wanted_sound_types.size()) {
        wanted_sound_types[unit.unit_type] = true;
      }
    }
    for (const BuildableUnitVisual& buildable : status.buildable_units) {
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
        cache_unit_sound_assets(
            storm, sfx_data, sfx_table, wanted_sound_types, status);
  }

  if (scenario.valid() && terrain_tileset.valid()) {
    status.minimap_ready =
        status.minimap_ready &&
        build_minimap_preview(
            terrain_tileset,
            scenario,
            status.minimap,
            status.minimap_content_x,
            status.minimap_content_y,
            status.minimap_content_width,
            status.minimap_content_height);
    (void)status.pathing_map.build(terrain_tileset, scenario);
    status.scenario = std::move(scenario);
    status.terrain_tileset = std::move(terrain_tileset);
    melee_start_ready = settle_melee_starting_workers(status) && melee_start_ready;
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
      status.unit_traits_ready &&
      status.status_progress_art_ready && !status.stat_text_table.empty() &&
      !status.resource_error_waves[0].empty() &&
      !status.resource_error_waves[1].empty() &&
      status.unit_sounds_ready && status.music_available &&
      status.command_panel_ready && status.portrait_panel_ready &&
      status.minimap_ready && status.team_colors_ready && status.pathing_map.valid() &&
      status.creep_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) * status.scenario_height &&
      status.creep_visual_tiles.size() ==
          static_cast<std::size_t>(status.scenario_width) * status.scenario_height &&
      status.scenario.valid() &&
      status.active_player_count >= 2 &&
      !status.units.empty() && map_closed && patch_closed && archive_closed;
  if (status.assets_ready) {
    char detail[300]{};
    std::snprintf(
        detail,
        sizeof(detail),
        "%s: %ux%u %s, four players, %zu units/%zu THGY sprites; camera %u,%u px; %s; image %u/script %u/draw %u.",
        status.map_name.c_str(),
        status.scenario_width,
        status.scenario_height,
        status.tileset_name.c_str(),
        status.scenario_unit_count,
        status.scenario_sprite_count,
        status.camera_x,
        status.camera_y,
        status.hud_path.c_str(),
        status.image_id,
        status.iscript_id,
        status.image_draw_function);
    status.primary = "A supplied StarCraft multiplayer map is loaded and rendered.";
    status.detail = detail;
  } else {
    status.primary = "The supplied map or its recovered render path did not initialize.";
    if (!map_opened) {
      status.detail = "Could not open maps\\96x96_space4.scm as a read-only Storm archive.";
    } else if (!scenario_loaded) {
      status.detail = "Could not parse the beta CHK sections from staredit\\scenario.chk.";
    } else if (!data_loaded) {
      status.detail = "Failed DAT/TBL asset: " + data.failed_asset();
    } else {
      status.detail = "The ERA tileset, initial CUnit image, IScript, or archive close did not verify.";
    }
  }
  return status;
}

bool update_building_placement(
    BootstrapStatus& status,
    int game_x,
    int game_y) noexcept;

bool set_camera_position(
    BootstrapStatus& status,
    const int requested_x,
    const int requested_y) noexcept {
  if (!status.scenario.valid() || !status.terrain_tileset.valid()) {
    return false;
  }
  const int maximum_x = (std::max)(
      0, static_cast<int>(status.scenario_width) * 32 - kMapViewportWidth);
  const int maximum_y = (std::max)(
      0, static_cast<int>(status.scenario_height) * 32 - kMapViewportHeight);
  // The common camera setter at 0x00405BB0 clamps both axes and clears the
  // low three bits. Keep the original eight-pixel camera granularity.
  const int camera_x = (std::clamp)(requested_x, 0, maximum_x) & ~7;
  const int camera_y = (std::clamp)(requested_y, 0, maximum_y) & ~7;
  if (camera_x == status.camera_x && camera_y == status.camera_y) {
    return false;
  }
  SpritePreviewFrame terrain{};
  if (!build_terrain_preview(
          status.terrain_tileset,
          status.scenario,
          static_cast<std::uint16_t>(camera_x),
          static_cast<std::uint16_t>(camera_y),
          terrain,
          &status.creep_tiles,
          &status.creep_visual_tiles)) {
    return false;
  }
  status.camera_x = static_cast<std::uint16_t>(camera_x);
  status.camera_y = static_cast<std::uint16_t>(camera_y);
  status.camera_tile_x = static_cast<std::uint16_t>(camera_x / 32);
  status.camera_tile_y = static_cast<std::uint16_t>(camera_y / 32);
  status.terrain = std::move(terrain);
  status.terrain_ready = true;
  return true;
}

bool camera_key_down(
    const RecoveryWindowState& state,
    const int key) noexcept {
  return key >= 0 && key < static_cast<int>(state.keys_down.size()) &&
         state.keys_down[static_cast<std::size_t>(key)];
}

bool camera_input_key(const WPARAM key) noexcept {
  switch (key) {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
      return true;
    default:
      return false;
  }
}

bool advance_camera_scroll(RecoveryWindowState& state) noexcept {
  BootstrapStatus* const status = state.status;
  if (status == nullptr) {
    return false;
  }
  int vertical{};
  int horizontal{};
  const bool down =
      camera_key_down(state, VK_DOWN) || camera_key_down(state, VK_END) ||
      camera_key_down(state, VK_NEXT) || camera_key_down(state, VK_NUMPAD1) ||
      camera_key_down(state, VK_NUMPAD2) || camera_key_down(state, VK_NUMPAD3);
  const bool up =
      camera_key_down(state, VK_UP) || camera_key_down(state, VK_HOME) ||
      camera_key_down(state, VK_PRIOR) || camera_key_down(state, VK_NUMPAD7) ||
      camera_key_down(state, VK_NUMPAD8) || camera_key_down(state, VK_NUMPAD9);
  const bool right =
      camera_key_down(state, VK_RIGHT) || camera_key_down(state, VK_PRIOR) ||
      camera_key_down(state, VK_NEXT) || camera_key_down(state, VK_NUMPAD3) ||
      camera_key_down(state, VK_NUMPAD6) || camera_key_down(state, VK_NUMPAD9);
  const bool left =
      camera_key_down(state, VK_LEFT) || camera_key_down(state, VK_HOME) ||
      camera_key_down(state, VK_END) || camera_key_down(state, VK_NUMPAD1) ||
      camera_key_down(state, VK_NUMPAD4) || camera_key_down(state, VK_NUMPAD7);
  vertical = down ? 1 : (up ? -1 : 0);
  horizontal = right ? 1 : (left ? -1 : 0);

  // cursor.cpp::sub_44AA10 at 0x0044AA10 uses a two-pixel edge band.
  // Its 478 bottom edge belonged to the original 480-line surface; this
  // stretched recovery surface has the corresponding logical edge at 398.
  if (horizontal == 0 && vertical == 0 && state.mouse_in_client) {
    horizontal = state.mouse_game_x <= 1
                     ? -1
                     : (state.mouse_game_x >= kMapViewportWidth - 2 ? 1 : 0);
    vertical = state.mouse_game_y <= 1
                   ? -1
                   : (state.mouse_game_y >= kMapViewportHeight - 2 ? 1 : 0);
  }
  if (horizontal == 0 && vertical == 0) {
    state.camera_scroll_ramp = 0;
    return false;
  }
  if (state.camera_scroll_ramp < 6U) {
    ++state.camera_scroll_ramp;
  }
  const int distance =
      kCameraScrollSpeeds[kDefaultCameraScrollSpeed][state.camera_scroll_ramp];
  const bool moved = set_camera_position(
      *status,
      static_cast<int>(status->camera_x) + horizontal * distance,
      static_cast<int>(status->camera_y) + vertical * distance);
  if (moved && status->placement_active && state.mouse_in_client) {
    (void)update_building_placement(
        *status, state.mouse_game_x, state.mouse_game_y);
  }
  return moved;
}

bool client_to_game(
    const HWND window,
    const LPARAM lparam,
    int& game_x,
    int& game_y) noexcept {
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 || client.bottom <= 0) {
    return false;
  }
  const int client_x = static_cast<short>(LOWORD(lparam));
  const int client_y = static_cast<short>(HIWORD(lparam));
  game_x = client_x * kMapViewportWidth / client.right;
  game_y = client_y * kMapViewportHeight / client.bottom;
  return game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
         game_y < kMapViewportHeight;
}

bool hud_pixel_opaque(
    const BootstrapStatus& status,
    const int game_x,
    const int game_y) noexcept {
  if (!status.hud_ready || game_x < 0 || game_y < 0 || game_x >= kMapViewportWidth ||
      game_y >= kMapViewportHeight) {
    return false;
  }
  const std::size_t source_x =
      static_cast<std::size_t>(game_x) * status.hud.width / kMapViewportWidth;
  const std::size_t source_y =
      static_cast<std::size_t>(game_y) * status.hud.height / kMapViewportHeight;
  const std::size_t pixel = source_y * status.hud.width + source_x;
  return pixel < status.hud.bgra.size() && (status.hud.bgra[pixel] & 0xFF000000U) != 0;
}

bool center_camera_from_minimap(
    BootstrapStatus& status,
    const int game_x,
    const int game_y) noexcept {
  if (!status.minimap_ready || status.minimap_content_width == 0 ||
      status.minimap_content_height == 0) {
    return false;
  }
  constexpr int vertical_numerator = 5;
  constexpr int vertical_denominator = 6;
  const int control_width = status.minimap_control.right -
                            status.minimap_control.left + 1;
  const int control_height = status.minimap_control.bottom -
                             status.minimap_control.top + 1;
  const int left = status.minimap_control.left +
                   status.minimap_content_x * control_width / 128;
  const int right = status.minimap_control.left +
                    (status.minimap_content_x + status.minimap_content_width) *
                        control_width / 128 -
                    1;
  const int top = (status.minimap_control.top * vertical_numerator /
                       vertical_denominator) +
                  status.minimap_content_y * control_height * vertical_numerator /
                      (128 * vertical_denominator);
  const int bottom = (status.minimap_control.top * vertical_numerator /
                          vertical_denominator) +
                     (status.minimap_content_y + status.minimap_content_height) *
                         control_height * vertical_numerator /
                         (128 * vertical_denominator) -
                     1;
  if (game_x < left || game_x > right || game_y < top || game_y > bottom) {
    return false;
  }
  const int world_x = static_cast<int>(
      static_cast<std::int64_t>(game_x - left) * status.scenario_width * 32 /
      (std::max)(1, right - left + 1));
  const int world_y = static_cast<int>(
      static_cast<std::int64_t>(game_y - top) * status.scenario_height * 32 /
      (std::max)(1, bottom - top + 1));
  // minimap.cpp::sub_46F520 records the scaled control rectangle and the
  // shared camera setter keeps the resulting origin on an eight-pixel grid.
  (void)set_camera_position(
      status,
      world_x - static_cast<int>(kMapViewportWidth) / 2,
      world_y - static_cast<int>(kMapViewportHeight) / 2);
  return true;
}

std::uint16_t command_position_at(
    const BootstrapStatus& status,
    const int game_x,
    const int game_y) noexcept {
  const CommandCardView card = command_card_for(status);
  if (!status.command_panel_ready || card.count == 0) {
    return 0;
  }
  constexpr int vertical_numerator = 5;
  constexpr int vertical_denominator = 6;
  for (std::size_t index = 0; index < card.count; ++index) {
    const CommandButtonVisual& button = card.buttons[index];
    const CommandControl& control = status.command_controls[button.position - 1U];
    const int top = control.top * vertical_numerator / vertical_denominator;
    const int bottom = (control.bottom + 1) * vertical_numerator / vertical_denominator - 1;
    if (game_x >= control.left && game_x <= control.right && game_y >= top &&
        game_y <= bottom) {
      return button.position;
    }
  }
  return 0;
}

std::size_t status_selection_unit_at(
    const BootstrapStatus& status,
    const int game_x,
    const int game_y) noexcept {
  if (!status.status_panel_ready) {
    return SIZE_MAX;
  }
  std::size_t selected_count{};
  for (const ScenarioUnitPreview& unit : status.units) {
    selected_count += unit.alive && unit.selected ? 1U : 0U;
  }
  if (selected_count <= 1U) {
    return SIZE_MAX;
  }
  std::size_t slot{};
  for (std::size_t unit_index = 0;
       unit_index < status.units.size() && slot < status.status_selection_controls.size();
       ++unit_index) {
    const ScenarioUnitPreview& unit = status.units[unit_index];
    if (!unit.alive || !unit.selected) {
      continue;
    }
    const CommandControl& control = status.status_selection_controls[slot++];
    const int top = static_cast<int>(control.top * hud_vertical_scale());
    const int bottom =
        static_cast<int>((control.bottom + 1) * hud_vertical_scale()) - 1;
    if (game_x >= control.left && game_x <= control.right && game_y >= top &&
        game_y <= bottom) {
      return unit_index;
    }
  }
  return SIZE_MAX;
}

bool restart_unit_animation(
    const BootstrapStatus& status,
    ScenarioUnitPreview& unit,
    const std::uint8_t animation) noexcept {
  if (unit.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset& asset = status.unit_assets[unit.asset_index];
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  starcraft::lang::IScriptState next{};
  if (!program.valid() || !program.start(asset.iscript_id, animation, next)) {
    return false;
  }
  const auto result =
      program.tick(next, 0, 256, nullptr, status.scenario.tileset_id());
  if ((result != starcraft::lang::IScriptTickResult::yielded &&
       result != starcraft::lang::IScriptTickResult::sleeping) ||
      next.frame >= asset.sprite_frames.size()) {
    return false;
  }
  unit.iscript_state = next;
  if (next.flingy_velocity_event_count != 0U) {
    unit.movement_speed = next.flingy_velocity;
  }
  unit.current_sprite_frame = next.frame;
  unit.iscript_ready = true;
  unit.last_animation = animation;

  // sub_41C060 dispatches the requested action to every image currently
  // attached to the sprite. Action 20 makes the Command Center's image-247
  // working layer execute its terminating script while image 246 returns to
  // idle. Preserve the layer for its first yielded tick; WM_TIMER removes it
  // when the script reaches END on the following tick.
  if (unit.dynamic_overlay_ready &&
      unit.dynamic_overlay_asset_index < status.unit_assets.size()) {
    const UnitRenderAsset& dynamic_asset =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    starcraft::lang::IScriptState dynamic{};
    if (program.start(dynamic_asset.iscript_id, animation, dynamic)) {
      const auto dynamic_result =
          program.tick(
              dynamic,
              0,
              256,
              &unit.iscript_state,
              status.scenario.tileset_id());
      if ((dynamic_result == starcraft::lang::IScriptTickResult::yielded ||
           dynamic_result == starcraft::lang::IScriptTickResult::sleeping) &&
          dynamic.frame < dynamic_asset.sprite_frames.size()) {
        unit.dynamic_overlay_iscript_state = dynamic;
        unit.current_dynamic_overlay_frame = dynamic.frame;
      } else {
        unit.dynamic_overlay_ready = false;
      }
    }
  }
  if (next.overlay_event_count != 0U && asset.overlay_ready &&
      next.overlay_image == asset.overlay_image_id) {
    // sub_41C060 dispatches an action over the sprite image list. Restart a
    // known attached image created by the new main-image action as well; the
    // Command Center working action recreates its image-247 activity layer.
    starcraft::lang::IScriptState overlay{};
    if (program.start(asset.overlay_iscript_id, 0, overlay)) {
      const auto overlay_result =
          program.tick(
              overlay,
              0,
              256,
              &unit.iscript_state,
              status.scenario.tileset_id());
      if ((overlay_result == starcraft::lang::IScriptTickResult::yielded ||
           overlay_result == starcraft::lang::IScriptTickResult::sleeping) &&
          overlay.frame < asset.overlay_frames.size()) {
        unit.overlay_iscript_state = overlay;
        unit.current_overlay_frame = overlay.frame;
        unit.overlay_ready = true;
      }
    }
  } else if (next.overlay_event_count != 0U) {
    const auto dynamic_asset = std::find_if(
        status.unit_assets.begin(),
        status.unit_assets.end(),
        [&next](const UnitRenderAsset& candidate) {
          return candidate.image_id == next.overlay_image;
        });
    if (dynamic_asset != status.unit_assets.end()) {
      const std::size_t dynamic_index = static_cast<std::size_t>(
          dynamic_asset - status.unit_assets.begin());
      starcraft::lang::IScriptState dynamic{};
      if (program.start(dynamic_asset->iscript_id, 0U, dynamic)) {
        const auto dynamic_result =
            program.tick(
                dynamic,
                0,
                256,
                &unit.iscript_state,
                status.scenario.tileset_id());
        if ((dynamic_result == starcraft::lang::IScriptTickResult::yielded ||
             dynamic_result == starcraft::lang::IScriptTickResult::sleeping) &&
            dynamic.frame < dynamic_asset->sprite_frames.size()) {
          unit.dynamic_overlay_asset_index = dynamic_index;
          unit.dynamic_overlay_iscript_state = dynamic;
          unit.current_dynamic_overlay_frame = dynamic.frame;
          unit.dynamic_overlay_x_offset = next.overlay_x_offset;
          unit.dynamic_overlay_y_offset = next.overlay_y_offset;
          unit.dynamic_overlay_above = next.overlay_above;
          unit.dynamic_overlay_ready = true;
        }
      }
    }
  }
  return true;
}

bool advance_building_construction_animation(
    const BootstrapStatus& status,
    ScenarioUnitPreview& building) noexcept {
  if (!building.alive || !building.is_building ||
      building.construction_complete || building.max_hit_points == 0U) {
    return false;
  }
  // CUnitBuild.cpp::sub_423210 at 0x00423210 advances its byte +90 state
  // one case per update. Life crossings at 1/5, 2/5, and 3/5 dispatch
  // IScript animations 13, 14, and 15; the fourth crossing is state-only.
  switch (building.construction_animation_phase) {
    case 0:
      if (building.hit_points > building.max_hit_points / 5U) {
        building.construction_animation_phase = 1U;
        return true;
      }
      break;
    case 1:
      (void)restart_unit_animation(status, building, 13U);
      building.construction_animation_phase = 2U;
      return true;
    case 2:
      if (building.hit_points > 2U * building.max_hit_points / 5U) {
        building.construction_animation_phase = 3U;
        return true;
      }
      break;
    case 3:
      (void)restart_unit_animation(status, building, 14U);
      building.construction_animation_phase = 4U;
      return true;
    case 4:
      if (building.hit_points > 3U * building.max_hit_points / 5U) {
        building.construction_animation_phase = 5U;
        return true;
      }
      break;
    case 5:
      (void)restart_unit_animation(status, building, 15U);
      building.construction_animation_phase = 6U;
      return true;
    case 6:
      if (building.hit_points > 4U * building.max_hit_points / 5U) {
        building.construction_animation_phase = 7U;
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

bool collect_building_obstacles(
    const BootstrapStatus& status,
    const ScenarioUnitPreview* ignored_unit,
    std::vector<starcraft::lang::PathObstacle>& output) noexcept {
  output.clear();
  try {
    output.reserve(status.units.size());
    for (const ScenarioUnitPreview& unit : status.units) {
      if (&unit == ignored_unit || !unit.alive || !unit.is_building ||
          unit.selection_width == 0 ||
          unit.selection_height == 0) {
        continue;
      }
      const int left = static_cast<int>(unit.x) - unit.selection_width / 2;
      const int top = static_cast<int>(unit.y) - unit.selection_height / 2;
      output.push_back({
          left,
          top,
          left + unit.selection_width,
          top + unit.selection_height,
      });
    }
    return true;
  } catch (...) {
    output.clear();
    return false;
  }
}

starcraft::lang::UnitCollisionBody collision_body_for(
    const ScenarioUnitPreview& mover,
    const int mover_x,
    const int mover_y) noexcept {
  const bool has_dat_extents = mover.collision_left != 0U ||
                               mover.collision_top != 0U ||
                               mover.collision_right != 0U ||
                               mover.collision_bottom != 0U;
  return {
      mover.unit_id,
      mover.unit_type,
      mover_x,
      mover_y,
      {
          has_dat_extents ? mover.collision_left : mover.selection_width / 2,
          has_dat_extents ? mover.collision_top : mover.selection_height / 2,
          has_dat_extents ? mover.collision_right
                          : mover.selection_width - mover.selection_width / 2,
          has_dat_extents ? mover.collision_bottom
                          : mover.selection_height - mover.selection_height / 2,
      },
      starcraft::lang::cunit_status_collision_enabled |
          (mover.is_building ? starcraft::lang::cunit_status_building : 0U),
      mover.dat_flags,
      0,
      false,
  };
}

bool unit_footprints_overlap_at(
    const ScenarioUnitPreview& mover,
    const int mover_x,
    const int mover_y,
    const ScenarioUnitPreview& obstacle) noexcept {
  if (!obstacle.alive || mover.selection_width == 0 || mover.selection_height == 0 ||
      obstacle.selection_width == 0 || obstacle.selection_height == 0) {
    return false;
  }
  const starcraft::lang::UnitCollisionRect moving =
      starcraft::lang::unit_collision_rect(
          collision_body_for(mover, mover_x, mover_y));
  const starcraft::lang::UnitCollisionRect fixed =
      starcraft::lang::unit_collision_rect(
          collision_body_for(obstacle, obstacle.x, obstacle.y));
  return fixed.right > moving.left && fixed.left < moving.right &&
         fixed.bottom > moving.top && fixed.top < moving.bottom;
}

bool unit_rectangles_overlap_at(
    const ScenarioUnitPreview& mover,
    const int mover_x,
    const int mover_y,
    const ScenarioUnitPreview& obstacle) noexcept {
  if (!obstacle.alive || mover.selection_width == 0 || mover.selection_height == 0 ||
      obstacle.selection_width == 0 || obstacle.selection_height == 0) {
    return false;
  }
  return starcraft::lang::unit_collision_rects_overlap(
      collision_body_for(mover, mover_x, mover_y),
      collision_body_for(obstacle, obstacle.x, obstacle.y));
}

const ScenarioUnitPreview* find_live_unit_collision(
    const BootstrapStatus& status,
    const ScenarioUnitPreview& mover,
    const int proposed_x,
    const int proposed_y) noexcept {
  for (const ScenarioUnitPreview& obstacle : status.units) {
    if (&obstacle != &mover && obstacle.alive &&
        unit_rectangles_overlap_at(mover, proposed_x, proposed_y, obstacle)) {
      return &obstacle;
    }
  }
  return nullptr;
}

const ScenarioUnitPreview* find_live_unit_footprint_collision(
    const BootstrapStatus& status,
    const ScenarioUnitPreview& mover,
    const int proposed_x,
    const int proposed_y) noexcept {
  for (const ScenarioUnitPreview& obstacle : status.units) {
    if (&obstacle != &mover && obstacle.alive &&
        unit_footprints_overlap_at(mover, proposed_x, proposed_y, obstacle)) {
      return &obstacle;
    }
  }
  return nullptr;
}

bool creation_position_passable(
    const BootstrapStatus& status,
    const ScenarioUnitPreview& unit,
    const int x,
    const int y) noexcept {
  static const std::vector<starcraft::lang::PathObstacle> no_obstacles;
  return x >= 0 && y >= 0 && x <= UINT16_MAX && y <= UINT16_MAX &&
         starcraft::lang::path_position_passable(
             status.pathing_map,
             x,
             y,
             unit.selection_width,
             unit.selection_height,
             no_obstacles) &&
         find_live_unit_footprint_collision(status, unit, x, y) == nullptr;
}

struct CreatedUnitPlacementContext {
  const BootstrapStatus* status{};
  const ScenarioUnitPreview* unit{};
};

bool creation_position_passable_callback(
    const int x,
    const int y,
    void* const context) noexcept {
  const auto* const placement =
      static_cast<const CreatedUnitPlacementContext*>(context);
  return placement != nullptr && placement->status != nullptr &&
         placement->unit != nullptr &&
         creation_position_passable(
             *placement->status, *placement->unit, x, y);
}

bool settle_created_unit(
    BootstrapStatus& status,
    ScenarioUnitPreview& unit,
    const std::uint16_t requested_x,
    const std::uint16_t requested_y) noexcept {
  // StarCraft.exe CUnitInit.cpp::sub_42EE60 passes the hidden trainee's
  // current sprite center to place_unit.cpp::sub_47FBF0.  For building
  // production that center is the producer center written by
  // CUnitBuild.cpp::sub_421EF0; the placement search, not a fabricated exit
  // offset, chooses the first clear eight-pixel-aligned perimeter point.
  const auto extents_for = [](const ScenarioUnitPreview& value) {
    const bool has_dat_extents = value.collision_left != 0U ||
                                 value.collision_top != 0U ||
                                 value.collision_right != 0U ||
                                 value.collision_bottom != 0U;
    return starcraft::lang::CollisionExtents{
        has_dat_extents ? value.collision_left : value.selection_width / 2,
        has_dat_extents ? value.collision_top : value.selection_height / 2,
        has_dat_extents
            ? value.collision_right
            : value.selection_width - value.selection_width / 2,
        has_dat_extents
            ? value.collision_bottom
            : value.selection_height - value.selection_height / 2,
    };
  };
  const ScenarioUnitPreview* const collision =
      find_live_unit_footprint_collision(status, unit, requested_x, requested_y);
  const starcraft::lang::CollisionExtents mover_extents = extents_for(unit);
  const starcraft::lang::CollisionExtents obstacle_extents =
      collision == nullptr ? starcraft::lang::CollisionExtents{}
                           : extents_for(*collision);
  CreatedUnitPlacementContext context{&status, &unit};
  starcraft::lang::PlacementPoint placement{};
  if (!starcraft::lang::find_created_unit_position(
          requested_x,
          requested_y,
          status.pathing_map.pixel_width(),
          status.pathing_map.pixel_height(),
          mover_extents,
          collision == nullptr ? nullptr : &obstacle_extents,
          creation_position_passable_callback,
          &context,
          placement)) {
    return false;
  }
  unit.x = static_cast<std::uint16_t>(placement.x);
  unit.y = static_cast<std::uint16_t>(placement.y);
  unit.x_fixed = placement.x << 8;
  unit.y_fixed = placement.y << 8;
  return true;
}

bool settle_melee_starting_workers(BootstrapStatus& status) noexcept {
  if (!status.pathing_map.valid() || !status.scenario.valid()) {
    return false;
  }
  bool settled = true;
  for (std::size_t player = 0; player < 8U; ++player) {
    if (!status.active_players[player]) {
      continue;
    }
    const std::uint8_t race = status.scenario.players()[player].race;
    const starcraft::game::ScenarioStartLocation& start =
        status.scenario.start_locations()[player];
    starcraft::lang::MeleeUnitTypes unit_types{};
    if (!starcraft::lang::melee_unit_types(race, unit_types) || !start.present) {
      settled = false;
      continue;
    }
    const std::uint16_t worker_type = unit_types.worker;
    for (ScenarioUnitPreview& worker : status.units) {
      if (!worker.alive || worker.owner != player ||
          worker.unit_type != worker_type || worker.x != start.x ||
          worker.y != start.y) {
        continue;
      }
      settled = settle_created_unit(status, worker, start.x, start.y) && settled;
    }
  }
  return settled;
}

bool plan_scv_path(
    BootstrapStatus& status,
    ScenarioUnitPreview& unit,
    const std::uint16_t target_x,
    const std::uint16_t target_y) noexcept {
  std::vector<starcraft::lang::PathObstacle> obstacles;
  std::vector<starcraft::lang::PathPoint> path;
  if (!collect_building_obstacles(status, &unit, obstacles) ||
      !starcraft::lang::find_unit_path(
          status.pathing_map,
          unit.x,
          unit.y,
          target_x,
          target_y,
          unit.selection_width,
          unit.selection_height,
          obstacles,
          path) ||
      path.empty()) {
    return false;
  }
  unit.movement_path = std::move(path);
  unit.movement_path_index = 0;
  unit.movement_final_x = target_x;
  unit.movement_final_y = target_y;
  unit.movement_target_x = unit.movement_path.front().x;
  unit.movement_target_y = unit.movement_path.front().y;
  unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
  unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
  return true;
}

void stop_unit_movement(
    const BootstrapStatus& status,
    ScenarioUnitPreview& unit) noexcept {
  unit.moving = false;
  unit.movement_speed = 0;
  unit.movement_path.clear();
  unit.movement_path_index = 0;
  unit.avoidance_ticks = 0;
  unit.collision_wait_ticks = 0;
  unit.avoidance_turn = 0;
  // CFlingy.cpp::sub_406670 at 0x00406670 dispatches image animation 12
  // when movement stops. Fall back to the recovered init animation only if
  // this image does not publish that action.
  if (!restart_unit_animation(status, unit, 12) &&
      unit.asset_index < status.unit_assets.size()) {
    const UnitRenderAsset& asset = status.unit_assets[unit.asset_index];
    unit.iscript_state = asset.initial_iscript_state;
    unit.current_sprite_frame = asset.initial_iscript_state.frame;
    unit.iscript_ready = asset.iscript_ready;
  }
}

void cancel_unit_order(
    BootstrapStatus& status,
    ScenarioUnitPreview& unit) noexcept {
  if (unit.active_order == ActiveUnitOrder::construct && unit.order_target_id != 0) {
    for (ScenarioUnitPreview& target : status.units) {
      if (target.unit_id == unit.order_target_id &&
          target.construction_builder_id == unit.unit_id) {
        target.construction_builder_id = 0;
        break;
      }
    }
  }
  if (unit.harvest_source_id != 0U) {
    for (ScenarioUnitPreview& source : status.units) {
      if (source.unit_id == unit.harvest_source_id) {
        (void)source.harvest_queue.release(unit.unit_id);
        break;
      }
    }
  }
  stop_unit_movement(status, unit);
  unit.active_order = ActiveUnitOrder::none;
  unit.order_target_id = 0;
  unit.harvest_source_id = 0;
  unit.action_timer = 0;
  unit.action_phase = 0;
}

std::size_t issue_scv_move_order(
    BootstrapStatus& status,
    const std::uint16_t target_x,
    const std::uint16_t target_y) noexcept {
  std::size_t issued{};
  for (ScenarioUnitPreview& unit : status.units) {
    if (!unit.selected || !unit.alive || unit.owner != 0 || unit.is_building ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    if (!plan_scv_path(status, unit, target_x, target_y)) {
      cancel_unit_order(status, unit);
      continue;
    }
    if (!unit.moving) {
      unit.movement_speed = 0;
      // The same 0x00406670 state transition dispatches animation 11 when a
      // stationary CFlingy begins moving.
      (void)restart_unit_animation(status, unit, 11);
    }
    unit.moving = true;
    unit.active_order = ActiveUnitOrder::move;
    unit.order_target_id = 0;
    unit.action_timer = 0;
    ++issued;
  }
  return issued;
}

std::uint32_t effective_unit_top_speed(
    const ScenarioUnitPreview& unit) noexcept {
  // CImage.cpp::sub_415210 case 0x2C writes the walking animation's current
  // velocity to CFlingy+0x48; case 0x3F can replace the top speed at +0x40.
  // Iscript-controlled walkers commonly retain DAT's value 2 until action 11.
  return unit.iscript_state.flingy_speed_event_count != 0U &&
                 unit.iscript_state.flingy_speed != 0U
             ? unit.iscript_state.flingy_speed
             : (unit.iscript_state.flingy_velocity_event_count != 0U &&
                        unit.iscript_state.flingy_velocity != 0U
                    ? unit.iscript_state.flingy_velocity
                    : unit.movement_top_speed);
}

ScenarioUnitPreview* find_unit_by_id(
    BootstrapStatus& status,
    const std::uint32_t unit_id) noexcept {
  const auto unit = std::find_if(
      status.units.begin(),
      status.units.end(),
      [unit_id](const ScenarioUnitPreview& value) {
        return value.unit_id == unit_id && value.alive;
      });
  return unit == status.units.end() ? nullptr : &*unit;
}

const ScenarioUnitPreview* find_unit_by_id(
    const BootstrapStatus& status,
    const std::uint32_t unit_id) noexcept {
  const auto unit = std::find_if(
      status.units.begin(),
      status.units.end(),
      [unit_id](const ScenarioUnitPreview& value) {
        return value.unit_id == unit_id && value.alive;
      });
  return unit == status.units.end() ? nullptr : &*unit;
}

int unit_edge_distance(
    const ScenarioUnitPreview& left,
    const ScenarioUnitPreview& right) noexcept {
  const int dx = (std::max)(
      0,
      std::abs(static_cast<int>(left.x) - right.x) -
          (static_cast<int>(left.selection_width) + right.selection_width) / 2);
  const int dy = (std::max)(
      0,
      std::abs(static_cast<int>(left.y) - right.y) -
          (static_cast<int>(left.selection_height) + right.selection_height) / 2);
  return static_cast<int>(std::lround(std::sqrt(
      static_cast<double>(dx) * dx + static_cast<double>(dy) * dy)));
}

bool plan_scv_interaction_path(
    BootstrapStatus& status,
    ScenarioUnitPreview& worker,
    const ScenarioUnitPreview& target) noexcept {
  const int clearance_x =
      (static_cast<int>(worker.selection_width) + target.selection_width) / 2 + 4;
  const int clearance_y =
      (static_cast<int>(worker.selection_height) + target.selection_height) / 2 + 4;
  std::array<starcraft::lang::PathPoint, 4> points{{
      {static_cast<std::uint16_t>((std::max)(0, static_cast<int>(target.x) - clearance_x)),
       target.y},
      {static_cast<std::uint16_t>((std::min)(
           static_cast<int>(status.pathing_map.pixel_width()) - 1,
           static_cast<int>(target.x) + clearance_x)),
       target.y},
      {target.x,
       static_cast<std::uint16_t>((std::max)(0, static_cast<int>(target.y) - clearance_y))},
      {target.x,
       static_cast<std::uint16_t>((std::min)(
           static_cast<int>(status.pathing_map.pixel_height()) - 1,
           static_cast<int>(target.y) + clearance_y))},
  }};
  std::sort(
      points.begin(),
      points.end(),
      [&worker](const starcraft::lang::PathPoint& a,
                const starcraft::lang::PathPoint& b) {
        const std::int64_t a_dx = static_cast<int>(a.x) - worker.x;
        const std::int64_t a_dy = static_cast<int>(a.y) - worker.y;
        const std::int64_t b_dx = static_cast<int>(b.x) - worker.x;
        const std::int64_t b_dy = static_cast<int>(b.y) - worker.y;
        return a_dx * a_dx + a_dy * a_dy < b_dx * b_dx + b_dy * b_dy;
      });
  for (const starcraft::lang::PathPoint& point : points) {
    if (plan_scv_path(status, worker, point.x, point.y)) {
      return true;
    }
  }
  return false;
}

bool begin_scv_interaction(
    BootstrapStatus& status,
    ScenarioUnitPreview& worker,
    const ScenarioUnitPreview& target,
    const ActiveUnitOrder order) noexcept {
  const bool worker_only_order =
      order == ActiveUnitOrder::repair || order == ActiveUnitOrder::construct ||
      order == ActiveUnitOrder::gather || order == ActiveUnitOrder::return_cargo;
  if (!worker.alive || worker.is_building || worker.movement_top_speed == 0U ||
      (worker_only_order && (worker.dat_flags & 0x08U) == 0U) || !target.alive ||
      !plan_scv_interaction_path(status, worker, target)) {
    cancel_unit_order(status, worker);
    return false;
  }
  if (!worker.moving) {
    worker.movement_speed = 0;
    (void)restart_unit_animation(status, worker, 11);
  }
  worker.moving = true;
  worker.active_order = order;
  worker.order_target_id = target.unit_id;
  worker.action_timer = 0;
  worker.action_phase = 0;
  if (order == ActiveUnitOrder::gather) {
    worker.harvest_source_id = target.unit_id;
  }
  return true;
}

void cancel_command_target(BootstrapStatus& status) noexcept {
  status.command_target_active = false;
  status.target_unit_order = 0;
  status.target_terrain_order = 0;
}

void begin_command_target(
    BootstrapStatus& status,
    const std::uint8_t unit_order,
    const std::uint8_t terrain_order) noexcept {
  // target.cpp::sub_4B0120 at 0x004B0120 stores two distinct order IDs:
  // usel.cpp::sub_4B4730 at 0x004B4730 selects the first for a clicked unit
  // and the second for empty terrain.
  status.active_command_card = 0;
  status.placement_active = false;
  status.placement_valid = false;
  status.placement_unit_type = 0xFFFFU;
  status.command_target_active = true;
  status.target_unit_order = unit_order;
  status.target_terrain_order = terrain_order;
}

std::size_t issue_active_scv_target(
    BootstrapStatus& status,
    const std::uint16_t world_x,
    const std::uint16_t world_y) noexcept {
  if (!status.command_target_active) {
    return 0;
  }
  const ScenarioUnitPreview* hit{};
  std::uint64_t best_distance = UINT64_MAX;
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive || unit.selected) {
      continue;
    }
    const int half_width = static_cast<int>(unit.selection_width) / 2;
    const int half_height = static_cast<int>(unit.selection_height) / 2;
    if (static_cast<int>(world_x) < static_cast<int>(unit.x) - half_width ||
        static_cast<int>(world_x) > static_cast<int>(unit.x) + half_width ||
        static_cast<int>(world_y) < static_cast<int>(unit.y) - half_height ||
        static_cast<int>(world_y) > static_cast<int>(unit.y) + half_height) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(world_x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(world_y) - unit.y;
    const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < best_distance) {
      best_distance = distance;
      hit = &unit;
    }
  }
  const std::uint8_t order = hit == nullptr ? status.target_terrain_order
                                             : status.target_unit_order;
  const std::uint16_t target_x = hit == nullptr ? world_x : hit->x;
  const std::uint16_t target_y = hit == nullptr ? world_y : hit->y;
  status.last_issued_order = order;
  cancel_command_target(status);

  if (hit == nullptr || order == 7 || order == 15 || order == 0x32) {
    return issue_scv_move_order(status, target_x, target_y);
  }

  ActiveUnitOrder active_order{ActiveUnitOrder::none};
  if (order == 9 && hit->owner != 0) {
    active_order = ActiveUnitOrder::attack;
  } else if (order == 0x24 && hit->owner == 0 && hit->is_building &&
             hit->hit_points < hit->max_hit_points) {
    active_order = hit->construction_complete ? ActiveUnitOrder::repair
                                               : ActiveUnitOrder::construct;
  } else if (order == 0x4E && (hit->dat_flags & 0x2000U) != 0 &&
             hit->resource_amount != 0) {
    active_order = ActiveUnitOrder::gather;
  }
  if (active_order == ActiveUnitOrder::none) {
    return 0;
  }
  std::size_t issued{};
  for (ScenarioUnitPreview& worker : status.units) {
    const bool worker_only_order = active_order != ActiveUnitOrder::attack;
    if (!worker.selected || !worker.alive || worker.owner != 0 || worker.is_building ||
        worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (active_order == ActiveUnitOrder::attack && !worker.has_ground_weapon)) {
      continue;
    }
    if (begin_scv_interaction(status, worker, *hit, active_order)) {
      if (active_order == ActiveUnitOrder::construct) {
        ScenarioUnitPreview* const building =
            find_unit_by_id(status, worker.order_target_id);
        if (building != nullptr) {
          building->construction_builder_id = worker.unit_id;
        }
      }
      ++issued;
    }
  }
  return issued;
}

std::size_t issue_scv_return_cargo(BootstrapStatus& status) noexcept;

std::size_t issue_scv_smart_order(
    BootstrapStatus& status,
    const std::uint16_t world_x,
    const std::uint16_t world_y) noexcept {
  const ScenarioUnitPreview* hit{};
  std::uint64_t best_distance = UINT64_MAX;
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive || unit.selected) {
      continue;
    }
    const int half_width = static_cast<int>(unit.selection_width) / 2;
    const int half_height = static_cast<int>(unit.selection_height) / 2;
    if (static_cast<int>(world_x) < static_cast<int>(unit.x) - half_width ||
        static_cast<int>(world_x) > static_cast<int>(unit.x) + half_width ||
        static_cast<int>(world_y) < static_cast<int>(unit.y) - half_height ||
        static_cast<int>(world_y) > static_cast<int>(unit.y) + half_height) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(world_x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(world_y) - unit.y;
    const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < best_distance) {
      best_distance = distance;
      hit = &unit;
    }
  }
  if (hit == nullptr) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, world_x, world_y);
  }

  ActiveUnitOrder order{ActiveUnitOrder::none};
  if ((hit->dat_flags & 0x2000U) != 0U && hit->resource_amount != 0U) {
    // rclick.cpp's worker classification selects the harvest order for a
    // resource-bearing target before testing hostile ownership.
    order = ActiveUnitOrder::gather;
    status.last_issued_order = 0x4EU;
  } else if (hit->owner != 0U && hit->owner != 11U) {
    order = ActiveUnitOrder::attack;
    status.last_issued_order = 9U;
  } else if (hit->owner == 0U && hit->is_building &&
             (!hit->construction_complete || hit->hit_points < hit->max_hit_points)) {
    order = hit->construction_complete ? ActiveUnitOrder::repair
                                       : ActiveUnitOrder::construct;
    status.last_issued_order = 0x24U;
  } else if (hit->owner == 0U && (hit->dat_flags & 0x1000U) != 0U) {
    status.last_command_opcode = 34U;
    return issue_scv_return_cargo(status);
  }
  if (order == ActiveUnitOrder::none) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, hit->x, hit->y);
  }

  std::size_t issued{};
  for (ScenarioUnitPreview& worker : status.units) {
    const bool worker_only_order = order != ActiveUnitOrder::attack;
    if (!worker.alive || !worker.selected || worker.owner != 0U ||
        worker.is_building || worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (order == ActiveUnitOrder::attack && !worker.has_ground_weapon)) {
      continue;
    }
    if (begin_scv_interaction(status, worker, *hit, order)) {
      if (order == ActiveUnitOrder::construct) {
        ScenarioUnitPreview* const building =
            find_unit_by_id(status, worker.order_target_id);
        if (building != nullptr) {
          building->construction_builder_id = worker.unit_id;
        }
      }
      ++issued;
    }
  }
  // rclick.cpp classifies every selected CUnit. If the target-specific order
  // did not apply to any selected unit (for example Marines on a mineral
  // patch), the mobile selection still receives the ordinary terrain move.
  if (issued == 0U) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, hit->x, hit->y);
  }
  return issued;
}

std::size_t issue_scv_return_cargo(BootstrapStatus& status) noexcept {
  const ScenarioUnitPreview* worker = first_selected_unit(status);
  if (worker == nullptr || (worker->dat_flags & 0x08U) == 0U) {
    return 0;
  }
  const ScenarioUnitPreview* nearest{};
  std::uint64_t nearest_distance = UINT64_MAX;
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive || (unit.dat_flags & 0x1000U) == 0 ||
        unit.owner != worker->owner) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(worker->x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(worker->y) - unit.y;
    const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < nearest_distance) {
      nearest_distance = distance;
      nearest = &unit;
    }
  }
  if (nearest == nullptr) {
    return 0;
  }
  status.last_command_opcode = 34;
  std::size_t issued{};
  for (ScenarioUnitPreview& selected : status.units) {
    if (!selected.selected || !selected.alive ||
        (selected.dat_flags & 0x08U) == 0U ||
        selected.owner != nearest->owner) {
      continue;
    }
    issued += begin_scv_interaction(
                  status, selected, *nearest, ActiveUnitOrder::return_cargo)
                  ? 1U
                  : 0U;
  }
  return issued;
}

bool advance_unit_movement(BootstrapStatus& status) noexcept {
  bool changed{};
  constexpr double pi = 3.14159265358979323846;
  for (ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive || !unit.moving || unit.is_building ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    const std::uint32_t top_speed = effective_unit_top_speed(unit);
    const std::uint32_t acceleration = unit.movement_acceleration;
    const std::int32_t delta_x = static_cast<std::int32_t>(unit.movement_target_x) - unit.x;
    const std::int32_t delta_y = static_cast<std::int32_t>(unit.movement_target_y) - unit.y;
    const double distance = std::sqrt(
        static_cast<double>(delta_x) * delta_x +
        static_cast<double>(delta_y) * delta_y);
    if (distance <= static_cast<double>(unit.movement_speed) / 256.0 + 1.0 &&
        find_live_unit_collision(
            status, unit, unit.movement_target_x, unit.movement_target_y) == nullptr) {
      unit.x = unit.movement_target_x;
      unit.y = unit.movement_target_y;
      unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
      unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
      if (unit.movement_path_index + 1U < unit.movement_path.size()) {
        ++unit.movement_path_index;
        unit.movement_target_x = unit.movement_path[unit.movement_path_index].x;
        unit.movement_target_y = unit.movement_path[unit.movement_path_index].y;
      } else {
        stop_unit_movement(status, unit);
        if (unit.active_order == ActiveUnitOrder::move) {
          unit.active_order = ActiveUnitOrder::none;
        }
      }
      changed = true;
      continue;
    }

    const std::uint8_t desired = starcraft::lang::direction_from_points(
        unit.x, unit.y, unit.movement_target_x, unit.movement_target_y);
    const std::int8_t turn = starcraft::lang::clamp_turn_delta(
        unit.direction, desired, unit.movement_turn_speed);
    unit.direction = static_cast<std::uint8_t>(unit.direction + turn);
    unit.movement_speed = (std::min)(top_speed, unit.movement_speed + acceleration);
    const std::int32_t map_width = static_cast<std::int32_t>(status.scenario_width) * 32;
    const std::int32_t map_height = static_cast<std::int32_t>(status.scenario_height) * 32;
    const auto movement_candidate = [&](const std::uint8_t direction) {
      const double angle = static_cast<double>(direction) * (2.0 * pi / 256.0);
      std::int32_t x_fixed = unit.x_fixed + static_cast<std::int32_t>(
          std::lround(std::sin(angle) * unit.movement_speed));
      std::int32_t y_fixed = unit.y_fixed - static_cast<std::int32_t>(
          std::lround(std::cos(angle) * unit.movement_speed));
      x_fixed =
          (std::clamp)(x_fixed, 0, (std::max)(0, (map_width - 1) << 8U));
      y_fixed =
          (std::clamp)(y_fixed, 0, (std::max)(0, (map_height - 1) << 8U));
      return std::array<std::int32_t, 2>{{x_fixed, y_fixed}};
    };
    std::array<std::int32_t, 2> proposed = movement_candidate(unit.direction);
    std::uint16_t proposed_x = static_cast<std::uint16_t>(proposed[0] >> 8U);
    std::uint16_t proposed_y = static_cast<std::uint16_t>(proposed[1] >> 8U);
    std::vector<starcraft::lang::PathObstacle> obstacles;
    if (!collect_building_obstacles(status, &unit, obstacles) ||
        !starcraft::lang::path_position_passable(
            status.pathing_map,
            proposed_x,
            proposed_y,
            unit.selection_width,
            unit.selection_height,
            obstacles)) {
      // CUnitPathCollide.cpp::sub_439030 re-evaluates live collision state as
      // a unit advances. Re-plan here so a building placed after the order was
      // issued cannot be crossed by a stale path.
      const std::uint16_t final_x = unit.movement_final_x;
      const std::uint16_t final_y = unit.movement_final_y;
      if (!plan_scv_path(status, unit, final_x, final_y)) {
        stop_unit_movement(status, unit);
      }
      changed = true;
      continue;
    }

    const ScenarioUnitPreview* collision =
        find_live_unit_collision(status, unit, proposed_x, proposed_y);
    if (collision != nullptr) {
      // CUnitPathCollide.cpp::sub_4393D0 filters the live units intersecting
      // the next footprint. sub_43A070 classifies the contacting rectangle
      // side, sub_439B90 selects a cardinal escape, and sub_43AC00 validates
      // the alternative against both terrain and units. The old bootstrap
      // used ID parity and six guessed angle offsets here. Use the recovered
      // contact-side/cardinal ordering instead.
      const starcraft::lang::UnitCollisionBody moving_body =
          collision_body_for(unit, proposed_x, proposed_y);
      const starcraft::lang::UnitCollisionBody fixed_body =
          collision_body_for(*collision, collision->x, collision->y);
      const starcraft::lang::CollisionEscapePoints escape =
          starcraft::lang::collision_escape_points(
              moving_body, fixed_body, unit.direction);
      bool avoided{};
      for (std::size_t escape_index = 0; escape_index < escape.count; ++escape_index) {
        const starcraft::lang::CollisionEscapePoint& escape_point =
            escape.points[escape_index];
        if (escape_point.x < 0 || escape_point.y < 0 ||
            escape_point.x > UINT16_MAX || escape_point.y > UINT16_MAX ||
            !starcraft::lang::path_position_passable(
                status.pathing_map,
                escape_point.x,
                escape_point.y,
                unit.selection_width,
                unit.selection_height,
                obstacles) ||
            find_live_unit_collision(
                status, unit, escape_point.x, escape_point.y) != nullptr) {
          continue;
        }
        const std::array<std::int32_t, 2> candidate =
            movement_candidate(escape_point.direction);
        const std::uint16_t candidate_x =
            static_cast<std::uint16_t>(candidate[0] >> 8U);
        const std::uint16_t candidate_y =
            static_cast<std::uint16_t>(candidate[1] >> 8U);
        if ((candidate_x == unit.x && candidate_y == unit.y) ||
            !starcraft::lang::path_position_passable(
                status.pathing_map,
                candidate_x,
                candidate_y,
                unit.selection_width,
                unit.selection_height,
                obstacles) ||
            find_live_unit_collision(status, unit, candidate_x, candidate_y) != nullptr) {
          continue;
        }
        const starcraft::lang::PathPoint waypoint{
            static_cast<std::uint16_t>(escape_point.x),
            static_cast<std::uint16_t>(escape_point.y),
        };
        if (unit.movement_target_x != waypoint.x ||
            unit.movement_target_y != waypoint.y) {
          const std::size_t insertion =
              (std::min)(unit.movement_path_index, unit.movement_path.size());
          unit.movement_path.insert(
              unit.movement_path.begin() + static_cast<std::ptrdiff_t>(insertion),
              waypoint);
          unit.movement_target_x = waypoint.x;
          unit.movement_target_y = waypoint.y;
        }
        proposed = candidate;
        proposed_x = candidate_x;
        proposed_y = candidate_y;
        unit.direction = escape_point.direction;
        unit.avoidance_turn = 0;
        unit.avoidance_ticks = 1;
        unit.collision_wait_ticks = 0;
        avoided = true;
        break;
      }
      if (!avoided) {
        unit.collision_wait_ticks = static_cast<std::uint8_t>((std::min)(
            255U, static_cast<unsigned>(unit.collision_wait_ticks) + 1U));
        unit.movement_speed = unit.movement_speed > acceleration
                                  ? unit.movement_speed - acceleration
                                  : 0U;
        changed = true;
        continue;
      }
    } else {
      unit.collision_wait_ticks = 0;
      if (unit.avoidance_ticks != 0U) {
        --unit.avoidance_ticks;
      } else {
        unit.avoidance_turn = 0;
      }
    }
    unit.x_fixed = proposed[0];
    unit.y_fixed = proposed[1];
    unit.x = static_cast<std::uint16_t>(unit.x_fixed >> 8U);
    unit.y = static_cast<std::uint16_t>(unit.y_fixed >> 8U);
    changed = true;
  }
  return changed;
}

bool advance_unit_actions(BootstrapStatus& status) noexcept {
  bool changed{};
  for (std::size_t worker_index = 0; worker_index < status.units.size();
       ++worker_index) {
    ScenarioUnitPreview& worker = status.units[worker_index];
    if (!worker.alive || worker.active_order == ActiveUnitOrder::none) {
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::move) {
      if (!worker.moving) {
        worker.active_order = ActiveUnitOrder::none;
      }
      continue;
    }
    if (worker.moving) {
      continue;
    }
    ScenarioUnitPreview* target = find_unit_by_id(status, worker.order_target_id);
    if (target == nullptr || target == &worker) {
      cancel_unit_order(status, worker);
      changed = true;
      continue;
    }
    const int interaction_range =
        worker.active_order == ActiveUnitOrder::attack
            ? static_cast<int>(worker.weapon_range)
            : 8;
    if (unit_edge_distance(worker, *target) > interaction_range) {
      const ActiveUnitOrder order = worker.active_order;
      const std::uint32_t source_id = worker.harvest_source_id;
      if (!begin_scv_interaction(status, worker, *target, order)) {
        cancel_unit_order(status, worker);
      }
      worker.harvest_source_id = source_id;
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::attack && worker.action_timer != 0) {
      --worker.action_timer;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::attack) {
      if (target->owner == worker.owner || !worker.has_ground_weapon ||
          worker.weapon_damage == 0) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      std::uint32_t damage = static_cast<std::uint32_t>(worker.weapon_damage) << 8U;
      if (worker.weapon_damage_class >= 1U && worker.weapon_damage_class <= 3U &&
          target->armor_class >= 1U && target->armor_class <= 3U) {
        damage = starcraft::lang::scale_damage(
            damage,
            static_cast<starcraft::lang::DamageClass>(worker.weapon_damage_class),
            static_cast<starcraft::lang::ArmorClass>(target->armor_class));
      }
      const std::uint32_t armor = static_cast<std::uint32_t>(target->armor) << 8U;
      damage = damage > armor ? damage - armor : 128U;
      damage = (std::max)(damage, 128U);
      target->hit_points = damage >= target->hit_points
                               ? 0U
                               : target->hit_points - damage;
      (void)restart_unit_animation(status, worker, 6);
      worker.action_timer = static_cast<std::uint16_t>((std::max)(
          1U, static_cast<unsigned>(worker.weapon_cooldown) >> 1U));
      if (target->hit_points == 0) {
        if (target->addon_parent_id != 0U) {
          ScenarioUnitPreview* const parent =
              find_unit_by_id(status, target->addon_parent_id);
          if (parent != nullptr && parent->attached_addon_id == target->unit_id) {
            parent->attached_addon_id = 0;
          }
        }
        if (target->attached_addon_id != 0U) {
          ScenarioUnitPreview* const addon =
              find_unit_by_id(status, target->attached_addon_id);
          if (addon != nullptr && addon->addon_parent_id == target->unit_id) {
            addon->addon_parent_id = 0;
          }
        }
        target->alive = false;
        target->selected = false;
        target->active_order = ActiveUnitOrder::none;
        target->moving = false;
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::construct) {
      if (!target->is_building || target->owner != worker.owner ||
          target->construction_complete || target->construction_ticks_total == 0U) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      target->construction_builder_id = worker.unit_id;
      if (worker.action_phase == 0U) {
        // CUnitBuild.cpp::sub_422160 runs the worker's construction animation
        // while applying the target's +168 fixed-point build increment.
        (void)restart_unit_animation(status, worker, 15);
        worker.action_phase = 1;
      }
      if (target->construction_ticks_remaining != 0U) {
        --target->construction_ticks_remaining;
      }
      const std::uint32_t initial_life = (std::max)(1U, target->max_hit_points / 10U);
      const std::uint32_t elapsed =
          target->construction_ticks_total - target->construction_ticks_remaining;
      target->hit_points = initial_life + static_cast<std::uint32_t>(
          static_cast<std::uint64_t>(target->max_hit_points - initial_life) * elapsed /
          target->construction_ticks_total);
      (void)advance_building_construction_animation(status, *target);
      if (target->construction_ticks_remaining == 0U) {
        target->hit_points = target->max_hit_points;
        target->construction_complete = true;
        target->construction_builder_id = 0;
        // CUnitInit.cpp::sub_42EBB0 dispatches animation 16 at completion.
        (void)restart_unit_animation(status, *target, 16U);
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::repair) {
      if (!target->is_building || target->owner != worker.owner ||
          target->hit_points >= target->max_hit_points) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      const std::uint32_t repair = (std::min)(
          static_cast<std::uint32_t>((std::max)(1U, static_cast<unsigned>(target->repair_step))),
          target->max_hit_points - target->hit_points);
      const std::uint64_t threshold =
          static_cast<std::uint64_t>((std::max)(1U, target->max_hit_points)) * 3U;
      const std::uint64_t mineral_accumulator =
          worker.repair_mineral_accumulator +
          static_cast<std::uint64_t>(repair) * target->mineral_cost;
      const std::uint64_t gas_accumulator =
          worker.repair_gas_accumulator +
          static_cast<std::uint64_t>(repair) * target->gas_cost;
      const std::uint32_t mineral_charge =
          static_cast<std::uint32_t>(mineral_accumulator / threshold);
      const std::uint32_t gas_charge =
          static_cast<std::uint32_t>(gas_accumulator / threshold);
      if (mineral_charge > status.player_minerals || gas_charge > status.player_gas) {
        post_resource_error(
            status, mineral_charge <= status.player_minerals && gas_charge != 0U);
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      status.player_minerals -= mineral_charge;
      status.player_gas -= gas_charge;
      worker.repair_mineral_accumulator =
          static_cast<std::uint32_t>(mineral_accumulator % threshold);
      worker.repair_gas_accumulator =
          static_cast<std::uint32_t>(gas_accumulator % threshold);
      target->hit_points += repair;
      (void)restart_unit_animation(status, worker, 15);
      if (target->hit_points >= target->max_hit_points) {
        target->hit_points = target->max_hit_points;
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::gather) {
      if ((target->dat_flags & 0x2000U) == 0 || target->resource_amount == 0 ||
          worker.cargo_minerals != 0 || worker.cargo_gas != 0) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // CUnitHarvest.cpp::sub_42D3C0 uses an order timer of 125 while the
      // worker performs the harvest animation; sub_42D9C0 then consumes ten
      // resource units from the source.
      if (worker.action_phase == 0) {
        const starcraft::lang::HarvestAdmission admission =
            target->harvest_queue.request(worker.unit_id);
        if (admission == starcraft::lang::HarvestAdmission::queued ||
            (admission == starcraft::lang::HarvestAdmission::already_present &&
             !target->harvest_queue.is_active(worker.unit_id))) {
          // sub_42C210 links a busy worker into the resource queue. It stays
          // visible and idle until sub_42C610 promotes it from the tail.
          worker.action_phase = 2;
          continue;
        }
      }
      if (worker.action_phase == 2) {
        if (!target->harvest_queue.is_active(worker.unit_id)) {
          continue;
        }
        worker.action_phase = 0;
      }
      if (worker.action_phase == 0) {
        worker.action_timer = 125;
        worker.action_phase = 1;
        (void)restart_unit_animation(status, worker, 15);
        continue;
      }
      if (worker.action_timer != 0) {
        --worker.action_timer;
        continue;
      }
      worker.action_phase = 0;
      target->resource_amount = target->resource_amount > 10
                                    ? static_cast<std::uint16_t>(target->resource_amount - 10)
                                    : 0;
      if (target->unit_type >= 176U && target->unit_type <= 178U) {
        worker.cargo_minerals = 8;
      } else {
        worker.cargo_gas = 8;
      }
      (void)target->harvest_queue.release(worker.unit_id);
      const ScenarioUnitPreview* nearest{};
      std::uint64_t nearest_distance = UINT64_MAX;
      for (const ScenarioUnitPreview& candidate : status.units) {
        if (!candidate.alive || candidate.owner != worker.owner ||
            (candidate.dat_flags & 0x1000U) == 0) {
          continue;
        }
        const std::int64_t dx = static_cast<int>(candidate.x) - worker.x;
        const std::int64_t dy = static_cast<int>(candidate.y) - worker.y;
        const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
        if (distance < nearest_distance) {
          nearest_distance = distance;
          nearest = &candidate;
        }
      }
      if (nearest == nullptr ||
          !begin_scv_interaction(
              status, worker, *nearest, ActiveUnitOrder::return_cargo)) {
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::return_cargo) {
      if ((target->dat_flags & 0x1000U) == 0 || target->owner != worker.owner) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      status.player_minerals += worker.cargo_minerals;
      status.player_gas += worker.cargo_gas;
      worker.cargo_minerals = 0;
      worker.cargo_gas = 0;
      ScenarioUnitPreview* const source =
          find_unit_by_id(status, worker.harvest_source_id);
      if (source != nullptr && source->resource_amount != 0 &&
          (source->dat_flags & 0x2000U) != 0) {
        (void)begin_scv_interaction(
            status, worker, *source, ActiveUnitOrder::gather);
      } else {
        cancel_unit_order(status, worker);
      }
      changed = true;
    }
  }
  return changed;
}

bool rebuild_creep_tiles(BootstrapStatus& status) noexcept {
  try {
    const std::size_t tile_count =
        static_cast<std::size_t>(status.scenario_width) * status.scenario_height;
    std::vector<starcraft::lang::CreepSource> sources;
    sources.reserve(status.units.size());
    for (const ScenarioUnitPreview& source : status.units) {
      if (!starcraft::lang::is_creep_source(source.unit_type)) {
        continue;
      }
      sources.push_back({
          source.unit_type,
          source.x,
          source.y,
          source.selection_width,
          source.selection_height,
          source.alive,
          source.construction_complete,
      });
    }
    starcraft::lang::CreepTileState previous{
        status.creep_tiles,
        status.creep_visual_tiles,
    };
    starcraft::lang::CreepTileState rebuilt{};
    if (!starcraft::lang::rebuild_creep_tile_state(
            status.scenario_width,
            status.scenario_height,
            sources.data(),
            sources.size(),
            previous,
            rebuilt)) {
      return false;
    }
    status.creep_tiles = std::move(rebuilt.occupied);
    status.creep_visual_tiles = std::move(rebuilt.visual_tile);
    if (status.scenario.valid() && status.terrain_tileset.valid()) {
      SpritePreviewFrame terrain;
      if (!build_terrain_preview(
              status.terrain_tileset,
              status.scenario,
              status.camera_x,
              status.camera_y,
              terrain,
              &status.creep_tiles,
              &status.creep_visual_tiles)) {
        return false;
      }
      status.terrain = std::move(terrain);
      status.terrain_ready = true;
      SpritePreviewFrame minimap;
      if (!build_minimap_preview(
              status.terrain_tileset,
              status.scenario,
              minimap,
              status.minimap_content_x,
              status.minimap_content_y,
              status.minimap_content_width,
              status.minimap_content_height,
              &status.creep_tiles,
              &status.creep_visual_tiles)) {
        return false;
      }
      status.minimap = std::move(minimap);
      status.minimap_ready = true;
    }
    return status.creep_tiles.size() == tile_count &&
           status.creep_visual_tiles.size() == tile_count;
  } catch (...) {
    status.creep_tiles.clear();
    status.creep_visual_tiles.clear();
    return false;
  }
}

bool tile_has_creep(
    const BootstrapStatus& status,
    const int tile_x,
    const int tile_y) noexcept {
  return tile_x >= 0 && tile_y >= 0 && tile_x < status.scenario_width &&
         tile_y < status.scenario_height &&
         status.creep_tiles.size() ==
             static_cast<std::size_t>(status.scenario_width) * status.scenario_height &&
         status.creep_tiles[static_cast<std::size_t>(tile_y) *
                                status.scenario_width +
                            tile_x] != 0U;
}

bool advance_addon_construction(BootstrapStatus& status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview& addon : status.units) {
    if (!addon.alive || addon.construction_complete || addon.addon_parent_id == 0U ||
        addon.construction_ticks_total == 0U) {
      continue;
    }
    const ScenarioUnitPreview* const parent =
        find_unit_by_id(status, addon.addon_parent_id);
    if (parent == nullptr || parent->attached_addon_id != addon.unit_id) {
      continue;
    }
    if (addon.construction_ticks_remaining != 0U) {
      --addon.construction_ticks_remaining;
    }
    const std::uint32_t initial_life =
        (std::max)(1U, addon.max_hit_points / 10U);
    const std::uint32_t elapsed =
        addon.construction_ticks_total - addon.construction_ticks_remaining;
    addon.hit_points = initial_life + static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(addon.max_hit_points - initial_life) * elapsed /
        addon.construction_ticks_total);
    (void)advance_building_construction_animation(status, addon);
    if (addon.construction_ticks_remaining == 0U) {
      addon.hit_points = addon.max_hit_points;
      addon.construction_complete = true;
      (void)restart_unit_animation(status, addon, 16U);
    }
    changed = true;
  }
  return changed;
}

bool addon_center_for_parent(
    const BuildableUnitVisual& addon,
    const ScenarioUnitPreview& parent,
    std::uint16_t& center_x,
    std::uint16_t& center_y) noexcept {
  if ((addon.simulation.dat_flags & 2U) == 0U ||
      addon.addon_parent_type != parent.unit_type) {
    return false;
  }
  starcraft::lang::TerranAddonPlacement placement{};
  if (!starcraft::lang::terran_addon_center(
          parent.unit_type,
          addon.unit_type,
          parent.x,
          parent.y,
          parent.selection_width,
          parent.selection_height,
          addon.placement_width,
          addon.placement_height,
          addon.addon_x,
          addon.addon_y,
          placement)) {
    return false;
  }
  center_x = placement.center_x;
  center_y = placement.center_y;
  return true;
}

bool placement_is_valid(
    const BootstrapStatus& status,
    const BuildableUnitVisual& buildable,
    const std::uint16_t center_x,
    const std::uint16_t center_y) noexcept {
  // placebox.cpp::sub_481410 at 0x00481410 reads dword_8DFFB0's width and
  // height, divides them by 32, and colors the per-tile placement bitmap.
  // Validate that footprint against map bounds, the CV5 0x0800 terrain-block
  // bit recovered through collide.cpp::sub_419260, and live unit occupancy.
  const int left = static_cast<int>(center_x) - buildable.placement_width / 2;
  const int top = static_cast<int>(center_y) - buildable.placement_height / 2;
  const int right = left + buildable.placement_width;
  const int bottom = top + buildable.placement_height;
  const int map_right = static_cast<int>(status.scenario_width) * 32;
  const int map_bottom = static_cast<int>(status.scenario_height) * 32;
  if (left < 0 || top < 0 || right > map_right || bottom > map_bottom) {
    return false;
  }
  if ((buildable.simulation.dat_flags & 2U) != 0U) {
    bool attached_to_parent{};
    for (const ScenarioUnitPreview& parent : status.units) {
      std::uint16_t addon_x{};
      std::uint16_t addon_y{};
      if (!parent.alive || !parent.construction_complete || parent.owner != 0 ||
          parent.attached_addon_id != 0U ||
          !addon_center_for_parent(buildable, parent, addon_x, addon_y)) {
        continue;
      }
      if (addon_x == center_x && addon_y == center_y) {
        attached_to_parent = true;
        break;
      }
    }
    if (!attached_to_parent) {
      return false;
    }
  }
  const int first_tile_x = left / 32;
  const int first_tile_y = top / 32;
  const int last_tile_x = (right - 1) / 32;
  const int last_tile_y = (bottom - 1) / 32;
  const bool requires_creep = (buildable.simulation.dat_flags & 0x20000U) != 0U;
  for (int tile_y = first_tile_y; tile_y <= last_tile_y; ++tile_y) {
    for (int tile_x = first_tile_x; tile_x <= last_tile_x; ++tile_x) {
      std::uint16_t map_tile{};
      if (!status.scenario.tile(
              static_cast<std::uint16_t>(tile_x),
              static_cast<std::uint16_t>(tile_y),
              map_tile) ||
          !status.terrain_tileset.buildable(map_tile)) {
        return false;
      }
      if (requires_creep && !tile_has_creep(status, tile_x, tile_y)) {
        return false;
      }
    }
  }
  const bool refinery = buildable.unit_type == 110U;
  std::uint32_t geyser_id{};
  if (refinery) {
    for (const ScenarioUnitPreview& unit : status.units) {
      if (unit.alive && unit.unit_type == 188U &&
          std::abs(static_cast<int>(unit.x) - center_x) <= 16 &&
          std::abs(static_cast<int>(unit.y) - center_y) <= 16) {
        geyser_id = unit.unit_id;
        break;
      }
    }
    if (geyser_id == 0) {
      return false;
    }
  }
  for (const ScenarioUnitPreview& unit : status.units) {
    if (!unit.alive) {
      continue;
    }
    const int unit_left = static_cast<int>(unit.x) - unit.selection_width / 2;
    const int unit_top = static_cast<int>(unit.y) - unit.selection_height / 2;
    const int unit_right = unit_left + unit.selection_width;
    const int unit_bottom = unit_top + unit.selection_height;
    if (left < unit_right && right > unit_left && top < unit_bottom && bottom > unit_top &&
        unit.unit_id != geyser_id) {
      return false;
    }
  }
  return true;
}

bool update_building_placement(
    BootstrapStatus& status,
    const int game_x,
    const int game_y) noexcept {
  if (!status.placement_active) {
    return false;
  }
  const BuildableUnitVisual* const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr || buildable->asset_index >= status.unit_assets.size()) {
    status.placement_valid = false;
    return false;
  }
  // placebox.cpp::sub_480F80 at 0x00480F80 and sub_481590 at 0x00481590
  // convert the 640x400 mouse point to map tiles with >> 5, then recover the
  // 32-pixel aligned world origin before drawing the footprint.
  const int world_x = game_x + status.camera_x;
  const int world_y = game_y + status.camera_y;
  const int half_width = buildable->placement_width / 2;
  const int half_height = buildable->placement_height / 2;
  const int snapped_left = ((std::max)(0, world_x - half_width + 16) / 32) * 32;
  const int snapped_top = ((std::max)(0, world_y - half_height + 16) / 32) * 32;
  status.placement_x = static_cast<std::uint16_t>(snapped_left + half_width);
  status.placement_y = static_cast<std::uint16_t>(snapped_top + half_height);
  status.placement_valid = placement_is_valid(
      status, *buildable, status.placement_x, status.placement_y);
  return true;
}

bool place_current_building(BootstrapStatus& status) noexcept {
  if (!status.placement_active || !status.placement_valid) {
    return false;
  }
  const BuildableUnitVisual* const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr || buildable->asset_index >= status.unit_assets.size()) {
    status.placement_valid = false;
    return false;
  }
  if (!resource_cost_available(
          status,
          buildable->simulation.mineral_cost,
          buildable->simulation.gas_cost) ||
      !placement_is_valid(status, *buildable, status.placement_x, status.placement_y)) {
    status.placement_valid = false;
    return false;
  }
  const ScenarioUnitPreview* const selected_source = first_selected_unit(status);
  const bool addon = (buildable->simulation.dat_flags & 2U) != 0U;
  std::uint32_t worker_id{};
  std::uint32_t parent_id{};
  if (selected_source == nullptr || selected_source->owner != 0) {
    return false;
  }
  if (addon) {
    std::uint16_t addon_x{};
    std::uint16_t addon_y{};
    if (!selected_source->construction_complete ||
        selected_source->unit_type != buildable->addon_parent_type ||
        selected_source->attached_addon_id != 0U ||
        !addon_center_for_parent(*buildable, *selected_source, addon_x, addon_y) ||
        addon_x != status.placement_x || addon_y != status.placement_y) {
      return false;
    }
    parent_id = selected_source->unit_id;
  } else {
    if (selected_source->unit_type != 7) {
      return false;
    }
    worker_id = selected_source->unit_id;
  }
  try {
    std::size_t geyser_index = SIZE_MAX;
    if (buildable->unit_type == 110U) {
      for (std::size_t index = 0; index < status.units.size(); ++index) {
        const ScenarioUnitPreview& unit = status.units[index];
        if (unit.alive && unit.unit_type == 188U &&
            std::abs(static_cast<int>(unit.x) - status.placement_x) <= 16 &&
            std::abs(static_cast<int>(unit.y) - status.placement_y) <= 16) {
          geyser_index = index;
          break;
        }
      }
      if (geyser_index == SIZE_MAX) {
        return false;
      }
    }
    const UnitRenderAsset& asset = status.unit_assets[buildable->asset_index];
    ScenarioUnitPreview building{};
    building.unit_id = status.next_unit_id++;
    building.x = status.placement_x;
    building.y = status.placement_y;
    building.x_fixed = static_cast<std::int32_t>(building.x) << 8U;
    building.y_fixed = static_cast<std::int32_t>(building.y) << 8U;
    building.unit_type = buildable->unit_type;
    building.owner = 0;
    building.asset_index = buildable->asset_index;
    building.selection_width = buildable->placement_width;
    building.selection_height = buildable->placement_height;
    building.iscript_state = asset.initial_iscript_state;
    building.overlay_iscript_state = asset.initial_overlay_iscript_state;
    building.current_sprite_frame = asset.initial_iscript_state.frame;
    building.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
    building.iscript_ready = asset.iscript_ready;
    building.overlay_ready = asset.overlay_ready;
    building.is_building = true;
    apply_simulation_traits(building, buildable->simulation);
    // CUnitInit.cpp::sub_42E6B0 and CUnitBuild.cpp::sub_422D20 initialize an
    // unfinished building at exactly one tenth of maximum fixed-point life.
    // The +172 field is units.dat build time / 2 and is decremented only while
    // a builder is actively applying the +168 construction increment.
    building.construction_complete = false;
    building.construction_ticks_total = static_cast<std::uint16_t>((std::max)(
        1U, static_cast<unsigned>(buildable->simulation.build_time) >> 1U));
    building.construction_ticks_remaining = building.construction_ticks_total;
    building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
    building.construction_builder_id = worker_id;
    building.addon_parent_id = parent_id;
    if (geyser_index != SIZE_MAX) {
      building.resource_amount = status.units[geyser_index].resource_amount;
    }
    const std::uint32_t building_id = building.unit_id;
    status.units.push_back(std::move(building));
    if (parent_id != 0U) {
      ScenarioUnitPreview* const parent = find_unit_by_id(status, parent_id);
      if (parent != nullptr) {
        parent->attached_addon_id = building_id;
      }
    }
    if (geyser_index != SIZE_MAX) {
      status.units[geyser_index].alive = false;
      status.units[geyser_index].selected = false;
    }
    status.player_minerals -= buildable->simulation.mineral_cost;
    status.player_gas -= buildable->simulation.gas_cost;
    status.placement_active = false;
    status.placement_valid = false;
    status.placement_unit_type = 0xFFFFU;
    status.active_command_card = 0;
    ScenarioUnitPreview* const worker = find_unit_by_id(status, worker_id);
    ScenarioUnitPreview* const created = find_unit_by_id(status, building_id);
    if (!addon && worker != nullptr && created != nullptr &&
        !begin_scv_interaction(
            status, *worker, *created, ActiveUnitOrder::construct)) {
      created->construction_builder_id = 0;
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool configure_preview_type(
    BootstrapStatus& status,
    ScenarioUnitPreview& unit,
    const std::uint16_t unit_type) noexcept {
  if (unit_type >= status.runtime_unit_types.size()) {
    return false;
  }
  const RuntimeUnitType& runtime = status.runtime_unit_types[unit_type];
  if (!runtime.ready || runtime.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const starcraft::lang::UnitInitializationData& initialization =
      runtime.initialization;
  const UnitRenderAsset& asset = status.unit_assets[runtime.asset_index];
  unit.unit_type = unit_type;
  unit.asset_index = runtime.asset_index;
  unit.selection_width = initialization.placement_width;
  unit.selection_height = initialization.placement_height;
  unit.collision_left = static_cast<std::uint16_t>(initialization.collision.left);
  unit.collision_top = static_cast<std::uint16_t>(initialization.collision.top);
  unit.collision_right = static_cast<std::uint16_t>(initialization.collision.right);
  unit.collision_bottom = static_cast<std::uint16_t>(initialization.collision.bottom);
  unit.iscript_state = asset.initial_iscript_state;
  unit.overlay_iscript_state = asset.initial_overlay_iscript_state;
  unit.current_sprite_frame = asset.initial_iscript_state.frame;
  unit.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  unit.iscript_ready = asset.iscript_ready;
  unit.overlay_ready = asset.overlay_ready;
  unit.is_building = initialization.is_building;
  unit.construction_complete = true;
  apply_initialization_traits(unit, initialization);
  return true;
}

void activate_command_button(
    BootstrapStatus& status,
    const std::uint16_t position) noexcept {
  const CommandCardView card = command_card_for(status);
  for (std::size_t button_index = 0; button_index < card.count; ++button_index) {
    const CommandButtonVisual& button = card.buttons[button_index];
    if (button.position == position) {
      status.last_command_position = position;
      if (button.action == CommandAction::train_unit) {
        ScenarioUnitPreview* producer{};
        for (ScenarioUnitPreview& unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == 0U &&
              unit.construction_complete &&
              starcraft::lang::producer_builds_unit(
                  unit.unit_type, button.argument)) {
            producer = &unit;
            break;
          }
        }
        if (producer == nullptr || button.argument >= status.runtime_unit_types.size()) {
          return;
        }
        const RuntimeUnitType& product = status.runtime_unit_types[button.argument];
        const starcraft::data::UnitSimulationTraits& simulation =
            product.initialization.simulation;
        if (!product.ready || producer->production_queue.full() ||
            !resource_cost_available(
                status, simulation.mineral_cost, simulation.gas_cost)) {
          return;
        }
        const bool starting_queue = producer->production_queue.empty();
        if (!producer->production_queue.enqueue(button.argument)) {
          return;
        }
        if (starting_queue) {
          producer->production_active = true;
          producer->production_started = GetTickCount();
          producer->production_kind =
              producer->unit_type == starcraft::lang::zerg_larva_type
                  ? starcraft::lang::UnitProductionKind::zerg_larva_morph
                  : starcraft::lang::UnitProductionKind::train;
          if (producer->production_kind ==
              starcraft::lang::UnitProductionKind::zerg_larva_morph) {
            // CUnitZBuild.cpp::sub_447820 changes larva 35 into egg 36 while
            // retaining the queued target at CUnit+0x98.
            (void)configure_preview_type(
                status, *producer, starcraft::lang::zerg_egg_type);
          } else {
            // CUnitBuild.cpp::sub_423020 dispatches Working (19) when the
            // first queue entry materializes.
            (void)restart_unit_animation(status, *producer, 19U);
          }
        }
        status.player_minerals -= simulation.mineral_cost;
        status.player_gas -= simulation.gas_cost;
      } else if (button.action == CommandAction::begin_move_target) {
        // statbtn.cpp action 0x0047EA20 calls target.cpp::sub_4B0120 with
        // unit order 0x32 and empty-terrain order 7.
        begin_command_target(status, 0x32, 7);
      } else if (button.action == CommandAction::stop) {
        // statbtn.cpp action 0x0047EA50 forwards command byte 30 through
        // sub_474790. Apply it locally while network dispatch is paused.
        status.last_command_opcode = 30;
        cancel_command_target(status);
        for (ScenarioUnitPreview& unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == 0 && !unit.is_building) {
            cancel_unit_order(status, unit);
          }
        }
      } else if (button.action == CommandAction::begin_attack_target) {
        // Action 0x0047EA80 supplies unit order 9 and terrain order 15.
        begin_command_target(status, 9, 15);
      } else if (button.action == CommandAction::begin_repair_target) {
        // Action 0x0047F660 supplies unit order 0x24 and terrain order 7.
        begin_command_target(status, 0x24, 7);
      } else if (button.action == CommandAction::begin_gather_target) {
        // Action 0x0047F700 supplies unit order 0x4E and terrain order 7.
        begin_command_target(status, 0x4E, 7);
      } else if (button.action == CommandAction::return_cargo) {
        // Action 0x0047F730 forwards command byte 34 through sub_474D40.
        cancel_command_target(status);
        (void)issue_scv_return_cargo(status);
      } else if (button.action == CommandAction::cancel_target) {
        cancel_command_target(status);
        status.active_command_card = 0;
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
      } else if (button.action == CommandAction::open_card) {
        cancel_command_target(status);
        status.active_command_card = button.argument;
      } else if (button.action == CommandAction::begin_building_placement) {
        const BuildableUnitVisual* const buildable =
            find_buildable_unit(status, button.argument);
        if (buildable != nullptr && buildable->asset_index < status.unit_assets.size() &&
            resource_cost_available(
                status,
                buildable->simulation.mineral_cost,
                buildable->simulation.gas_cost)) {
          cancel_command_target(status);
          status.placement_unit_type = button.argument;
          status.placement_active = true;
          status.placement_valid = false;
        }
      } else if (button.action == CommandAction::build_addon) {
        const BuildableUnitVisual* const addon =
            find_buildable_unit(status, button.argument);
        const ScenarioUnitPreview* const parent = first_selected_unit(status);
        std::uint16_t center_x{};
        std::uint16_t center_y{};
        if (addon != nullptr && parent != nullptr && parent->owner == 0 &&
            parent->construction_complete && parent->attached_addon_id == 0U &&
            resource_cost_available(
                status,
                addon->simulation.mineral_cost,
                addon->simulation.gas_cost) &&
            addon_center_for_parent(*addon, *parent, center_x, center_y)) {
          // placebox.cpp::sub_482050 validates the add-on footprint first,
          // subtracts dword_8DF730[addon] to recover the owner's footprint,
          // then CUnitTBuild.cpp::sub_440C90 links owner+192/addon+132.
          cancel_command_target(status);
          status.placement_unit_type = addon->unit_type;
          status.placement_x = center_x;
          status.placement_y = center_y;
          status.placement_active = true;
          status.placement_valid =
              placement_is_valid(status, *addon, center_x, center_y);
          if (status.placement_valid) {
            (void)place_current_building(status);
          }
        }
      } else if (button.action == CommandAction::close_card) {
        cancel_command_target(status);
        status.active_command_card = 0;
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
      }
      return;
    }
  }
}

bool advance_unit_production(
    BootstrapStatus& status,
    const std::uint32_t now) noexcept {
  bool changed{};
  try {
    const std::size_t producer_count = status.units.size();
    for (std::size_t source_index = 0; source_index < producer_count; ++source_index) {
      ScenarioUnitPreview& source = status.units[source_index];
      const std::uint16_t product_type = source.production_queue.front();
      if (!source.alive || !source.production_active ||
          product_type == starcraft::lang::cunit_production_empty ||
          product_type >= status.runtime_unit_types.size()) {
        continue;
      }
      const RuntimeUnitType& product = status.runtime_unit_types[product_type];
      const std::uint16_t total_ticks = starcraft::lang::production_total_ticks(
          product.initialization.simulation.build_time);
      const std::uint32_t duration =
          static_cast<std::uint32_t>(total_ticks) * kSimulationTickMilliseconds;
      if (!product.ready || total_ticks == 0U || now - source.production_started < duration) {
        continue;
      }

      if (source.production_kind ==
          starcraft::lang::UnitProductionKind::zerg_larva_morph) {
        source.production_queue.advance();
        source.production_active = false;
        source.production_started = 0;
        (void)configure_preview_type(status, source, product_type);
        // sub_447820 completes the egg's queued morph by changing its CUnit
        // type and dispatching the completion/idle transition.
        (void)restart_unit_animation(status, source, 13U);
        changed = true;
        continue;
      }

      const std::uint16_t source_x = source.x;
      const std::uint16_t source_y = source.y;
      const std::uint8_t owner = source.owner;
      ScenarioUnitPreview produced{};
      produced.unit_id = status.next_unit_id;
      produced.owner = owner;
      if (!configure_preview_type(status, produced, product_type)) {
        continue;
      }
      // sub_421EF0 creates the incomplete CUnit at the producer sprite's
      // exact x/y.  sub_423580 -> sub_42EFC0 -> sub_42EE60 only completes and
      // unhides it after sub_47FBF0 finds a legal position around that point.
      produced.x = source_x;
      produced.y = source_y;
      produced.x_fixed = static_cast<std::int32_t>(produced.x) << 8U;
      produced.y_fixed = static_cast<std::int32_t>(produced.y) << 8U;
      if (!settle_created_unit(status, produced, source_x, source_y)) {
        // The original completion finalizer returns zero here.  The producer
        // keeps its queue entry and retries instead of exposing the trainee
        // at the building center or silently losing the completed unit.
        continue;
      }
      ++status.next_unit_id;

      source.production_queue.advance();
      if (source.production_queue.empty()) {
        // Empty sentinel 228 makes sub_423020 dispatch animation 20.
        (void)restart_unit_animation(status, source, 20U);
        source.production_active = false;
        source.production_started = 0;
      } else {
        source.production_started = now;
      }
      status.units.push_back(std::move(produced));
      changed = true;
    }
    return changed;
  } catch (...) {
    return false;
  }
}

bool advance_zerg_larvae(BootstrapStatus& status) noexcept {
  bool changed{};
  try {
    const std::size_t initial_count = status.units.size();
    for (std::size_t base_index = 0; base_index < initial_count; ++base_index) {
      ScenarioUnitPreview& base = status.units[base_index];
      if (!base.alive || !base.construction_complete ||
          !starcraft::lang::is_zerg_town_hall(base.unit_type)) {
        continue;
      }
      if (base.larva_spawn_timer != 0U) {
        --base.larva_spawn_timer;
        continue;
      }
      base.larva_spawn_timer = starcraft::lang::zerg_larva_spawn_period;
      const std::size_t larvae = static_cast<std::size_t>(std::count_if(
          status.units.begin(), status.units.end(), [&base](const ScenarioUnitPreview& unit) {
            return unit.alive && unit.owner == base.owner &&
                   unit.unit_type == starcraft::lang::zerg_larva_type &&
                   unit.larva_parent_id == base.unit_id;
          }));
      if (larvae >= starcraft::lang::zerg_larva_limit) {
        continue;
      }

      const int left = static_cast<int>(base.x) - base.collision_left;
      const int top = static_cast<int>(base.y) - base.collision_top;
      const int right = static_cast<int>(base.x) + base.collision_right;
      const int bottom = static_cast<int>(base.y) + base.collision_bottom;
      const std::array<starcraft::lang::PathPoint, 4> candidates{{
          {base.x, static_cast<std::uint16_t>((std::max)(0, bottom + 10))},
          {static_cast<std::uint16_t>((std::max)(0, left - 10)), base.y},
          {static_cast<std::uint16_t>((std::max)(0, right + 10)), base.y},
          {base.x, static_cast<std::uint16_t>((std::max)(0, top - 10))},
      }};
      ScenarioUnitPreview larva{};
      larva.unit_id = status.next_unit_id++;
      larva.owner = base.owner;
      larva.larva_parent_id = base.unit_id;
      if (!configure_preview_type(status, larva, starcraft::lang::zerg_larva_type)) {
        continue;
      }
      bool placed{};
      for (const starcraft::lang::PathPoint& candidate : candidates) {
        larva.x = candidate.x;
        larva.y = candidate.y;
        larva.x_fixed = static_cast<std::int32_t>(larva.x) << 8U;
        larva.y_fixed = static_cast<std::int32_t>(larva.y) << 8U;
        if (settle_created_unit(status, larva, larva.x, larva.y)) {
          placed = true;
          break;
        }
      }
      if (placed) {
        status.units.push_back(std::move(larva));
        changed = true;
      }
    }
    return changed;
  } catch (...) {
    return false;
  }
}

void clear_selection(BootstrapStatus& status) noexcept {
  for (ScenarioUnitPreview& unit : status.units) {
    unit.selected = false;
  }
}

std::size_t selection_count(const BootstrapStatus& status) noexcept {
  std::size_t count{};
  for (const ScenarioUnitPreview& unit : status.units) {
    count += unit.selected ? 1U : 0U;
  }
  return count;
}

void complete_selection_drag(RecoveryWindowState& state) noexcept {
  BootstrapStatus* const status = state.status;
  if (status == nullptr) {
    return;
  }
  status->active_command_card = 0;
  status->placement_active = false;
  status->placement_valid = false;
  status->placement_unit_type = 0xFFFFU;
  cancel_command_target(*status);
  const bool additive =
      (GetKeyState(VK_SHIFT) & 0x8000) != 0 || (GetKeyState(VK_CONTROL) & 0x8000) != 0;
  const int delta_x = std::abs(state.selection_current_x - state.selection_start_x);
  const int delta_y = std::abs(state.selection_current_y - state.selection_start_y);

  if (delta_x <= 4 && delta_y <= 4) {
    // usel.cpp::sub_4B51B0 at 0x004B51B0 defines a click as a drag no
    // larger than four pixels. sub_4B51F0 uses Shift or Control to toggle.
    std::size_t hit = status->units.size();
    std::uint64_t best_distance = UINT64_MAX;
    for (std::size_t index = 0; index < status->units.size(); ++index) {
      const ScenarioUnitPreview& unit = status->units[index];
      if (!unit.alive) {
        continue;
      }
      const int center_x = unit.x - status->camera_x;
      const int center_y = unit.y - status->camera_y;
      const int half_width = static_cast<int>(unit.selection_width) / 2;
      const int half_height = static_cast<int>(unit.selection_height) / 2;
      if (state.selection_current_x < center_x - half_width ||
          state.selection_current_x > center_x + half_width ||
          state.selection_current_y < center_y - half_height ||
          state.selection_current_y > center_y + half_height) {
        continue;
      }
      const std::int64_t dx = state.selection_current_x - center_x;
      const std::int64_t dy = state.selection_current_y - center_y;
      const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
      if (distance < best_distance) {
        best_distance = distance;
        hit = index;
      }
    }
    if (!additive) {
      clear_selection(*status);
    }
    if (hit < status->units.size()) {
      status->units[hit].selected = additive ? !status->units[hit].selected : true;
      if (status->units[hit].selected &&
          queue_unit_response(*status, status->units[hit], false)) {
        (void)play_pending_game_sound(state);
      }
    }
    return;
  }

  if (!additive) {
    clear_selection(*status);
  }
  const int left = (std::min)(state.selection_start_x, state.selection_current_x);
  const int right = (std::max)(state.selection_start_x, state.selection_current_x);
  const int top = (std::min)(state.selection_start_y, state.selection_current_y);
  const int bottom = (std::max)(state.selection_start_y, state.selection_current_y);
  std::size_t selected = selection_count(*status);
  for (ScenarioUnitPreview& unit : status->units) {
    if (!unit.alive) {
      continue;
    }
    const int center_x = unit.x - status->camera_x;
    const int center_y = unit.y - status->camera_y;
    if (!unit.selected && unit.owner == 0 && center_x >= left && center_x <= right &&
        center_y >= top && center_y <= bottom && selected < 12) {
      unit.selected = true;
      ++selected;
    }
  }
  const ScenarioUnitPreview* const primary = first_selected_unit(*status);
  if (primary != nullptr && queue_unit_response(*status, *primary, false)) {
    (void)play_pending_game_sound(state);
  }
}

LRESULT CALLBACK recovery_window_proc(
    const HWND window,
    const UINT message,
    const WPARAM wparam,
    const LPARAM lparam) {
  switch (message) {
    case WM_NCCREATE: {
      const auto* const create = reinterpret_cast<const CREATESTRUCTA*>(lparam);
      SetWindowLongPtrA(
          window,
          GWLP_USERDATA,
          reinterpret_cast<LONG_PTR>(create->lpCreateParams));
      return TRUE;
    }
    case WM_CREATE: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state == nullptr || !initialize_opengl(window, *state)) {
        return -1;
      }
      // Audio failure does not make the render window unusable; the dedicated
      // audio regression probe still requires a live OpenAL device.
      (void)initialize_audio(*state);
      return 0;
    }
    case WM_ERASEBKGND:
      return 1;
    case WM_SIZE:
      InvalidateRect(window, nullptr, FALSE);
      return 0;
    case WM_PAINT: {
      PAINTSTRUCT paint{};
      BeginPaint(window, &paint);
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr) {
        (void)render_opengl(window, *state);
      }
      EndPaint(window, &paint);
      return 0;
    }
    case WM_LBUTTONDOWN: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      int game_x{};
      int game_y{};
      const bool game_position =
          state != nullptr && state->status != nullptr &&
          client_to_game(window, lparam, game_x, game_y);
      if (game_position && !state->status->placement_active &&
          !state->status->command_target_active &&
          center_camera_from_minimap(*state->status, game_x, game_y)) {
        state->minimap_dragging = true;
        SetCapture(window);
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (game_position && !state->status->placement_active &&
          !state->status->command_target_active) {
        const std::size_t selected_unit =
            status_selection_unit_at(*state->status, game_x, game_y);
        if (selected_unit < state->status->units.size()) {
          // statdata.cpp::sub_4A56B0 at 0x004A56B0 binds controls 33..44 to
          // the selected CUnit array. Plain click makes that slot primary;
          // Shift/Control removes it while retaining the other selected slots.
          const bool modifier = (GetKeyState(VK_SHIFT) & 0x8000) != 0 ||
                                (GetKeyState(VK_CONTROL) & 0x8000) != 0;
          if (modifier) {
            state->status->units[selected_unit].selected = false;
          } else {
            clear_selection(*state->status);
            state->status->units[selected_unit].selected = true;
            if (queue_unit_response(
                    *state->status,
                    state->status->units[selected_unit],
                    false)) {
              (void)play_pending_game_sound(*state);
            }
          }
          state->status->active_command_card = 0;
          InvalidateRect(window, nullptr, FALSE);
          return 0;
        }
      }
      if (game_position && state->status->placement_active &&
          !hud_pixel_opaque(*state->status, game_x, game_y)) {
        (void)update_building_placement(*state->status, game_x, game_y);
        (void)place_current_building(*state->status);
        (void)play_pending_resource_error(*state);
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (game_position) {
        const std::uint16_t command_position =
            command_position_at(*state->status, game_x, game_y);
        if (command_position != 0) {
          state->pressed_command_position = command_position;
          SetCapture(window);
          InvalidateRect(window, nullptr, FALSE);
          return 0;
        }
      }
      if (game_position && state->status->command_target_active &&
          !hud_pixel_opaque(*state->status, game_x, game_y)) {
        const int map_right = static_cast<int>(state->status->scenario_width) * 32 - 1;
        const int map_bottom = static_cast<int>(state->status->scenario_height) * 32 - 1;
        const int world_x = game_x + state->status->camera_x;
        const int world_y = game_y + state->status->camera_y;
        const std::size_t issued = issue_active_scv_target(
            *state->status,
            static_cast<std::uint16_t>((std::clamp)(world_x, 0, map_right)),
            static_cast<std::uint16_t>((std::clamp)(world_y, 0, map_bottom)));
        const ScenarioUnitPreview* const speaker = first_selected_unit(*state->status);
        if (issued != 0U && speaker != nullptr &&
            queue_unit_response(*state->status, *speaker, true)) {
          (void)play_pending_game_sound(*state);
        }
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (game_position &&
          !hud_pixel_opaque(*state->status, game_x, game_y)) {
        state->selection_dragging = true;
        state->selection_start_x = game_x;
        state->selection_start_y = game_y;
        state->selection_current_x = game_x;
        state->selection_current_y = game_y;
        SetCapture(window);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    }
    case WM_MOUSEMOVE: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      int game_x{};
      int game_y{};
      if (state != nullptr && state->status != nullptr &&
          client_to_game(window, lparam, game_x, game_y)) {
        state->mouse_game_x = game_x;
        state->mouse_game_y = game_y;
        if (!state->mouse_in_client) {
          state->mouse_in_client = true;
          TRACKMOUSEEVENT tracking{};
          tracking.cbSize = sizeof(tracking);
          tracking.dwFlags = TME_LEAVE;
          tracking.hwndTrack = window;
          TrackMouseEvent(&tracking);
        }
        if (state->minimap_dragging) {
          (void)center_camera_from_minimap(*state->status, game_x, game_y);
          InvalidateRect(window, nullptr, FALSE);
        } else if (state->status->placement_active) {
          (void)update_building_placement(*state->status, game_x, game_y);
          InvalidateRect(window, nullptr, FALSE);
        } else if (state->status->command_target_active) {
          InvalidateRect(window, nullptr, FALSE);
        } else if (state->selection_dragging) {
          state->selection_current_x = game_x;
          state->selection_current_y = game_y;
          InvalidateRect(window, nullptr, FALSE);
        }
      }
      return 0;
    }
    case WM_MOUSELEAVE: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr) {
        state->mouse_in_client = false;
        state->camera_scroll_ramp = 0;
      }
      return 0;
    }
    case WM_RBUTTONDOWN: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      int game_x{};
      int game_y{};
      if (state != nullptr && state->status != nullptr) {
        BootstrapStatus& status = *state->status;
        if (status.placement_active || status.command_target_active ||
            status.active_command_card != 0) {
          status.placement_active = false;
          status.placement_valid = false;
          status.placement_unit_type = 0xFFFFU;
          status.active_command_card = 0;
          cancel_command_target(status);
          InvalidateRect(window, nullptr, FALSE);
        } else if (client_to_game(window, lparam, game_x, game_y) &&
                   !hud_pixel_opaque(status, game_x, game_y)) {
          // rclick.cpp classifies the clicked CUnit before falling back to
          // sub_482E10's empty-terrain order 7. Preserve that ordering so a
          // mineral node gathers, a hostile unit attacks, and a damaged own
          // structure repairs instead of every click becoming a move.
          const int world_x = game_x + status.camera_x;
          const int world_y = game_y + status.camera_y;
          const int map_right = static_cast<int>(status.scenario_width) * 32 - 1;
          const int map_bottom = static_cast<int>(status.scenario_height) * 32 - 1;
          const std::size_t issued = issue_scv_smart_order(
              status,
              static_cast<std::uint16_t>((std::clamp)(world_x, 0, map_right)),
              static_cast<std::uint16_t>((std::clamp)(world_y, 0, map_bottom)));
          const ScenarioUnitPreview* const speaker = first_selected_unit(status);
          if (issued != 0U && speaker != nullptr &&
              queue_unit_response(status, *speaker, true)) {
            (void)play_pending_game_sound(*state);
          }
          InvalidateRect(window, nullptr, FALSE);
        }
      }
      return 0;
    }
    case WM_KEYDOWN: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (wparam == VK_ESCAPE && state != nullptr && state->status != nullptr) {
        state->status->placement_active = false;
        state->status->placement_valid = false;
        state->status->placement_unit_type = 0xFFFFU;
        state->status->active_command_card = 0;
        cancel_command_target(*state->status);
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (state != nullptr && camera_input_key(wparam) &&
          wparam < state->keys_down.size()) {
        state->keys_down[static_cast<std::size_t>(wparam)] = true;
        return 0;
      }
      return DefWindowProcA(window, message, wparam, lparam);
    }
    case WM_KEYUP: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr && camera_input_key(wparam) &&
          wparam < state->keys_down.size()) {
        state->keys_down[static_cast<std::size_t>(wparam)] = false;
        return 0;
      }
      return DefWindowProcA(window, message, wparam, lparam);
    }
    case WM_KILLFOCUS: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr) {
        state->keys_down.fill(false);
        state->camera_scroll_ramp = 0;
      }
      return 0;
    }
    case WM_LBUTTONUP: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      int game_x{};
      int game_y{};
      if (state != nullptr && state->minimap_dragging) {
        if (state->status != nullptr && client_to_game(window, lparam, game_x, game_y)) {
          (void)center_camera_from_minimap(*state->status, game_x, game_y);
        }
        state->minimap_dragging = false;
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (state != nullptr && state->pressed_command_position != 0) {
        const std::uint16_t pressed = state->pressed_command_position;
        state->pressed_command_position = 0;
        if (state->status != nullptr && client_to_game(window, lparam, game_x, game_y) &&
            command_position_at(*state->status, game_x, game_y) == pressed) {
          activate_command_button(*state->status, pressed);
          (void)play_pending_resource_error(*state);
          const ScenarioUnitPreview* const speaker =
              first_selected_unit(*state->status);
          if ((pressed == 2U || pressed == 6U) && speaker != nullptr &&
              speaker->unit_type == 7U &&
              queue_unit_response(*state->status, *speaker, true)) {
            (void)play_pending_game_sound(*state);
          }
        }
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        InvalidateRect(window, nullptr, FALSE);
        return 0;
      }
      if (state != nullptr && state->selection_dragging) {
        if (client_to_game(window, lparam, game_x, game_y)) {
          state->selection_current_x = game_x;
          state->selection_current_y = game_y;
        }
        complete_selection_drag(*state);
        state->selection_dragging = false;
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    }
    case WM_CAPTURECHANGED: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr && reinterpret_cast<HWND>(lparam) != window) {
        state->selection_dragging = false;
        state->minimap_dragging = false;
        state->pressed_command_position = 0;
      }
      return 0;
    }
    case WM_TIMER: {
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      BootstrapStatus* const status = state == nullptr ? nullptr : state->status;
      if (status != nullptr && !status->unit_assets.empty() && !status->units.empty()) {
        const starcraft::lang::IScriptProgramView program{
            status->iscript_bytes.data(), status->iscript_bytes.size()};
        const std::uint32_t clock = GetTickCount();
        (void)advance_camera_scroll(*state);
        (void)advance_zerg_larvae(*status);
        (void)advance_unit_production(*status, clock);
        advance_resource_display(*status);
        (void)advance_unit_movement(*status);
        const bool unit_actions_changed = advance_unit_actions(*status);
        const bool addon_construction_changed =
            advance_addon_construction(*status);
        (void)play_pending_resource_error(*state);
        (void)play_pending_game_sound(*state);
        if (unit_actions_changed || addon_construction_changed) {
          (void)rebuild_creep_tiles(*status);
        }
        (void)advance_selected_portrait(*status, clock);
        for (std::size_t index = 0; index < status->units.size(); ++index) {
          ScenarioUnitPreview& unit = status->units[index];
          if (!unit.alive || !unit.iscript_ready ||
              unit.asset_index >= status->unit_assets.size()) {
            continue;
          }
          const UnitRenderAsset& asset = status->unit_assets[unit.asset_index];
          const std::uint32_t previous_weapon_events =
              unit.iscript_state.weapon_event_count;
          const std::uint32_t previous_velocity_events =
              unit.iscript_state.flingy_velocity_event_count;
          const auto result = program.tick(
              unit.iscript_state,
              clock ^ static_cast<std::uint32_t>(index * 0x9E3779B9U),
              256,
              nullptr,
              status->scenario.tileset_id());
          if (result == starcraft::lang::IScriptTickResult::malformed_program ||
              result == starcraft::lang::IScriptTickResult::unsupported_opcode ||
              result == starcraft::lang::IScriptTickResult::instruction_limit ||
              unit.iscript_state.frame >= asset.sprite_frames.size()) {
            unit.iscript_ready = false;
          } else {
            unit.current_sprite_frame = unit.iscript_state.frame;
            if (unit.iscript_state.flingy_velocity_event_count !=
                previous_velocity_events) {
              unit.movement_speed = unit.iscript_state.flingy_velocity;
            }
            if (unit.iscript_state.weapon_event_count != previous_weapon_events &&
                unit.iscript_state.weapon_event == 8U) {
              // The SCV harvest/repair animation's opcode 0x2B launches
              // weapon 8. Its image-498 script selects SFX 23..27 before the
              // impact event; queue the same archived cutter range here.
              status->pending_game_sound =
                  choose_unit_sound(*status, 23U, 27U);
            }
          }
          if (unit.overlay_ready && !asset.overlay_frames.empty()) {
            const auto overlay_result = program.tick(
                unit.overlay_iscript_state,
                clock ^ static_cast<std::uint32_t>(index * 0x85EBCA6BU) ^ 0x5A5A5A5AU,
                256,
                &unit.iscript_state,
                status->scenario.tileset_id());
            if (overlay_result == starcraft::lang::IScriptTickResult::malformed_program ||
                overlay_result == starcraft::lang::IScriptTickResult::unsupported_opcode ||
                overlay_result == starcraft::lang::IScriptTickResult::instruction_limit ||
                overlay_result == starcraft::lang::IScriptTickResult::ended ||
                unit.overlay_iscript_state.frame >= asset.overlay_frames.size()) {
              unit.overlay_ready = false;
            } else {
              unit.current_overlay_frame = unit.overlay_iscript_state.frame;
            }
          }
          if (unit.dynamic_overlay_ready &&
              unit.dynamic_overlay_asset_index < status->unit_assets.size()) {
            const UnitRenderAsset& dynamic_asset =
                status->unit_assets[unit.dynamic_overlay_asset_index];
            const auto dynamic_result = program.tick(
                unit.dynamic_overlay_iscript_state,
                clock ^ static_cast<std::uint32_t>(index * 0xC2B2AE35U) ^
                    0x3C3C3C3CU,
                256,
                &unit.iscript_state,
                status->scenario.tileset_id());
            if (dynamic_result ==
                    starcraft::lang::IScriptTickResult::malformed_program ||
                dynamic_result ==
                    starcraft::lang::IScriptTickResult::unsupported_opcode ||
                dynamic_result ==
                    starcraft::lang::IScriptTickResult::instruction_limit ||
                dynamic_result == starcraft::lang::IScriptTickResult::ended ||
                unit.dynamic_overlay_iscript_state.frame >=
                    dynamic_asset.sprite_frames.size()) {
              unit.dynamic_overlay_ready = false;
            } else {
              unit.current_dynamic_overlay_frame =
                  unit.dynamic_overlay_iscript_state.frame;
            }
          }
        }
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    }
    case WM_DESTROY: {
      KillTimer(window, 1);
      auto* const state = reinterpret_cast<RecoveryWindowState*>(
          GetWindowLongPtrA(window, GWLP_USERDATA));
      if (state != nullptr) {
        shutdown_audio(*state);
        shutdown_opengl(window, *state);
      }
      PostQuitMessage(0);
      return 0;
    }
    default:
      return DefWindowProcA(window, message, wparam, lparam);
  }
}

}  // namespace

int WINAPI WinMain(
    const HINSTANCE instance,
    HINSTANCE,
    LPSTR command_line,
    const int show_command) {
  BootstrapStatus status = probe_assets();
  if (command_line != nullptr && std::strstr(command_line, "--probe-assets") != nullptr) {
    return status.assets_ready
               ? 0
               : (status.failed_runtime_unit_type == 0xFFFFU
                      ? 10
                      : 1000 + status.failed_runtime_unit_type);
  }
  const bool opengl_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-opengl") != nullptr;
  const bool selection_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-selection") != nullptr;
  const bool command_panel_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-command-panel") != nullptr;
  const bool status_panel_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-status-panel") != nullptr;
  const bool multi_status_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-multi-status") != nullptr;
  const bool production_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-production") != nullptr;
  const bool all_production_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-all-production") != nullptr;
  const bool harvest_queue_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-harvest-queue") != nullptr;
  const bool geyser_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-geysers") != nullptr;
  const bool building_working_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-building-working") != nullptr;
  const bool resource_feedback_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-resource-feedback") != nullptr;
  const bool resource_strip_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-resource-strip") != nullptr;
  const bool unit_audio_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-unit-audio") != nullptr;
  const bool music_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-music") != nullptr;
  const bool movement_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-movement") != nullptr;
  const bool pathfinding_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-pathfinding") != nullptr;
  const bool unit_avoidance_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-unit-avoidance") != nullptr;
  const bool portrait_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-portrait") != nullptr;
  const bool worker_actions_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-worker-actions") != nullptr;
  const bool harvest_visual_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-harvest-visual") != nullptr;
  const bool smart_orders_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-smart-orders") != nullptr;
  const bool refinery_placement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-refinery-placement") != nullptr;
  const bool minimap_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-minimap") != nullptr;
  const bool command_target_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-command-targets") != nullptr;
  const bool building_placement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-building-placement") != nullptr;
  const bool construction_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-construction") != nullptr;
  const bool construction_status_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-status-construction") != nullptr;
  const bool addon_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-addon") != nullptr;
  const bool creep_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-creep") != nullptr;
  const bool melee_start_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-melee-start") != nullptr;
  const bool camera_probe =
      command_line != nullptr && std::strstr(command_line, "--probe-camera") != nullptr;
  constexpr char capture_option[] = "--capture-opengl=";
  const char* const capture_argument =
      command_line == nullptr ? nullptr : std::strstr(command_line, capture_option);
  const char* const capture_path =
      capture_argument == nullptr ? nullptr : capture_argument + sizeof(capture_option) - 1U;

  RecoveryWindowState window_state{&status};
  WNDCLASSA window_class{};
  window_class.style = CS_DBLCLKS | CS_OWNDC;
  window_class.lpfnWndProc = recovery_window_proc;
  window_class.hInstance = instance;
  window_class.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
  window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
  window_class.hbrBackground = nullptr;
  window_class.lpszClassName = kWindowClass;

  if (RegisterClassA(&window_class) == 0) {
    return 1;
  }

  std::string window_title = kWindowTitle;
  if (!status.detail.empty()) {
    window_title += " | ";
    window_title += status.detail;
  }
  RECT requested_window{0, 0, 960, 600};
  AdjustWindowRect(&requested_window, WS_OVERLAPPEDWINDOW, FALSE);
  const HWND window = CreateWindowExA(
      0,
      kWindowClass,
      window_title.c_str(),
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      requested_window.right - requested_window.left,
      requested_window.bottom - requested_window.top,
      nullptr,
      nullptr,
      instance,
      &window_state);
  if (window == nullptr) {
    return 2;
  }

  if (opengl_probe || selection_probe || command_panel_probe || production_probe ||
      all_production_probe || harvest_queue_probe || geyser_probe ||
      building_working_probe ||
      resource_feedback_probe || resource_strip_probe || unit_audio_probe || music_probe ||
      movement_probe || pathfinding_probe || portrait_probe || command_target_probe ||
      worker_actions_probe || harvest_visual_probe || smart_orders_probe ||
      refinery_placement_probe ||
      building_placement_probe ||
      construction_probe || addon_probe || creep_probe || melee_start_probe ||
      unit_avoidance_probe ||
      minimap_probe || camera_probe || status_panel_probe || multi_status_probe ||
      construction_status_probe ||
      capture_path != nullptr) {
    bool selection_verified = true;
    if (selection_probe) {
      selection_verified = false;
      const auto visible = std::find_if(
          status.units.begin(),
          status.units.end(),
          [&status](const ScenarioUnitPreview& unit) {
            const int game_x = unit.x - status.camera_x;
            const int game_y = unit.y - status.camera_y;
            return game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
                   game_y < kMapViewportHeight;
          });
      if (visible != status.units.end()) {
        RECT client{};
        const ScenarioUnitPreview& unit = *visible;
        const int game_x = unit.x - status.camera_x;
        const int game_y = unit.y - status.camera_y;
        if (GetClientRect(window, &client) && client.right > 0 && client.bottom > 0 &&
            game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
            game_y < kMapViewportHeight) {
          const int client_x = game_x * client.right / kMapViewportWidth;
          const int client_y = game_y * client.bottom / kMapViewportHeight;
          const LPARAM click = MAKELPARAM(client_x, client_y);
          SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
          SendMessageA(window, WM_LBUTTONUP, 0, click);
          selection_verified = selection_count(status) == 1 && visible->selected;
        }
      }
    }
    bool camera_verified = true;
    if (camera_probe) {
      const std::uint16_t initial_x = status.camera_x;
      const std::uint16_t initial_y = status.camera_y;
      const int maximum_x =
          static_cast<int>(status.scenario_width) * 32 - kMapViewportWidth;
      const WPARAM key = initial_x < maximum_x ? VK_RIGHT : VK_LEFT;
      SendMessageA(window, WM_KEYDOWN, key, 0);
      for (int tick = 0; tick < 7; ++tick) {
        SendMessageA(window, WM_TIMER, 1, 0);
      }
      SendMessageA(window, WM_KEYUP, key, 0);
      camera_verified =
          status.terrain_ready && status.camera_y == initial_y &&
          status.camera_x != initial_x && (status.camera_x & 7U) == 0;
    }
    RECT probe_client{};
    const bool probe_client_ready =
        GetClientRect(window, &probe_client) && probe_client.right > 0 &&
        probe_client.bottom > 0;
    const auto client_point = [&](const int game_x, const int game_y) -> LPARAM {
      return MAKELPARAM(
          game_x * probe_client.right / kMapViewportWidth,
          game_y * probe_client.bottom / kMapViewportHeight);
    };
    const auto click_command = [&](const std::uint16_t position) -> bool {
      if (!probe_client_ready || position == 0 ||
          position > status.command_controls.size()) {
        return false;
      }
      const CommandControl& control = status.command_controls[position - 1U];
      const int game_x = (control.left + control.right) / 2;
      const int game_y = ((control.top + control.bottom) / 2) * 5 / 6;
      const LPARAM click = client_point(game_x, game_y);
      SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
      SendMessageA(window, WM_LBUTTONUP, 0, click);
      return true;
    };

    bool all_production_verified = true;
    int all_production_probe_stage{};
    if (all_production_probe) {
      constexpr std::array<std::pair<std::uint16_t, std::size_t>, 11>
          expected_cards{{
              {35, 9}, {106, 1}, {108, 1}, {111, 3}, {113, 3}, {114, 4},
              {130, 1}, {154, 1}, {155, 3}, {160, 3}, {167, 3},
          }};
      all_production_verified = status.assets_ready;
      all_production_probe_stage = all_production_verified ? 1 : 0;
      const starcraft::lang::UnitProductionProducerView producers =
          starcraft::lang::production_producer_types();
      all_production_verified =
          all_production_verified && producers.count == expected_cards.size();
      all_production_probe_stage = all_production_verified ? 2 : all_production_probe_stage;
      for (std::size_t producer_index = 0;
           all_production_verified && producer_index < expected_cards.size();
           ++producer_index) {
        const std::uint16_t producer_type = expected_cards[producer_index].first;
        const RuntimeUnitType& producer = status.runtime_unit_types[producer_type];
        const starcraft::lang::UnitProductionButtonView buttons =
            starcraft::lang::production_buttons_for(producer_type);
        all_production_verified =
            producers.unit_types[producer_index] == producer_type && producer.ready &&
            producer.asset_index < status.unit_assets.size() &&
            buttons.count == expected_cards[producer_index].second;
        starcraft::lang::UnitProductionQueue queue{};
        for (std::size_t button_index = 0;
             all_production_verified && button_index < buttons.count;
             ++button_index) {
          const starcraft::lang::UnitProductionButton& button =
              buttons.buttons[button_index];
          const RuntimeUnitType& product =
              status.runtime_unit_types[button.product_type];
          ScenarioUnitPreview preview{};
          all_production_verified =
              button.producer_type == producer_type &&
              button.position == button_index + 1U &&
              button.icon == button.product_type && product.ready &&
              product.asset_index < status.unit_assets.size() &&
              starcraft::lang::production_total_ticks(
                  product.initialization.simulation.build_time) != 0U &&
              queue.enqueue(button.product_type) &&
              queue.front() == button.product_type &&
              configure_preview_type(status, preview, button.product_type) &&
              preview.unit_type == button.product_type &&
              preview.asset_index == product.asset_index &&
              preview.selection_width != 0U && preview.selection_height != 0U &&
              ((producer_type == starcraft::lang::zerg_larva_type) ==
               (button.kind ==
                starcraft::lang::UnitProductionKind::zerg_larva_morph));
          queue.advance();
          if (all_production_verified) {
            ScenarioUnitPreview source{};
            source.unit_id = status.next_unit_id++;
            source.owner = 0U;
            all_production_verified =
                configure_preview_type(status, source, producer_type);
            bool synthetic_center_found{};
            for (int y = 160;
                 all_production_verified &&
                 y + 160 < status.pathing_map.pixel_height() &&
                 !synthetic_center_found;
                 y += 32) {
              for (int x = 160;
                   x + 160 < status.pathing_map.pixel_width(); x += 32) {
                if (!creation_position_passable(status, source, x, y)) {
                  continue;
                }
                source.x = static_cast<std::uint16_t>(x);
                source.y = static_cast<std::uint16_t>(y);
                source.x_fixed = x << 8U;
                source.y_fixed = y << 8U;
                // Verify that this producer center has a legal completion
                // perimeter for the product before using it in the card
                // probe.  The temporary source is the initial collider just
                // as it is in sub_47FBF0 during real production.
                status.units.push_back(source);
                ScenarioUnitPreview trial{};
                trial.unit_id = status.next_unit_id;
                trial.owner = source.owner;
                synthetic_center_found =
                    configure_preview_type(status, trial, button.product_type) &&
                    settle_created_unit(status, trial, source.x, source.y);
                status.units.pop_back();
                if (synthetic_center_found) {
                  break;
                }
              }
            }
            all_production_verified =
                all_production_verified && synthetic_center_found &&
                source.production_queue.enqueue(button.product_type);
            source.production_kind = button.kind;
            source.production_active = true;
            source.production_started = 0U;
            if (button.kind ==
                starcraft::lang::UnitProductionKind::zerg_larva_morph) {
              all_production_verified =
                  configure_preview_type(
                      status, source, starcraft::lang::zerg_egg_type) &&
                  all_production_verified;
            }
            const std::uint32_t source_id = source.unit_id;
            const std::size_t first_result_index = status.units.size();
            status.units.push_back(std::move(source));
            all_production_verified =
                all_production_verified &&
                advance_unit_production(status, UINT32_MAX);
            const auto completed = std::find_if(
                status.units.begin() +
                    static_cast<std::ptrdiff_t>(first_result_index),
                status.units.end(),
                [button, source_id](const ScenarioUnitPreview& unit) {
                  return unit.alive && unit.unit_type == button.product_type &&
                         (button.kind ==
                                  starcraft::lang::UnitProductionKind::zerg_larva_morph
                              ? unit.unit_id == source_id
                              : unit.unit_id != source_id);
                });
            bool completed_clear = completed != status.units.end();
            if (completed_clear &&
                button.kind == starcraft::lang::UnitProductionKind::train) {
              const ScenarioUnitPreview* const completed_pointer = &*completed;
              completed_clear = std::none_of(
                  status.units.begin(), status.units.end(),
                  [completed_pointer](const ScenarioUnitPreview& other) {
                    return other.alive && &other != completed_pointer &&
                           unit_footprints_overlap_at(
                               *completed_pointer,
                               completed_pointer->x,
                               completed_pointer->y,
                               other);
                  });
            }
            all_production_verified =
                all_production_verified && completed != status.units.end() &&
                completed_clear && completed->asset_index == product.asset_index &&
                completed->iscript_ready && !completed->iscript_state.hidden &&
                completed->current_sprite_frame <
                    status.unit_assets[completed->asset_index].sprite_frames.size() &&
                completed->max_hit_points ==
                    product.initialization.simulation.max_hit_points;
            // Every iteration uses a synthetic producer.  Remove it and its
            // result so later cards test placement rather than accumulating
            // unrelated blockers at the same coordinate.
            status.units.erase(
                status.units.begin() +
                    static_cast<std::ptrdiff_t>(first_result_index),
                status.units.end());
          }
        }
      }
      all_production_probe_stage =
          all_production_verified ? 4 : all_production_probe_stage;
      const starcraft::lang::ZergLarvaSourceView larva_sources =
          starcraft::lang::zerg_larva_source_types();
      for (std::size_t source_index = 0;
           source_index < larva_sources.count;
           ++source_index) {
        const std::uint16_t town_hall_type =
            larva_sources.unit_types[source_index];
        const RuntimeUnitType& town_hall = status.runtime_unit_types[town_hall_type];
        all_production_verified =
            all_production_verified &&
            starcraft::lang::is_zerg_town_hall(town_hall_type) && town_hall.ready &&
            town_hall.asset_index < status.unit_assets.size();
      }
      all_production_probe_stage =
          all_production_verified ? 5 : all_production_probe_stage;
    }

    bool geysers_verified = true;
    int geyser_probe_stage{};
    if (geyser_probe) {
      constexpr std::array<std::pair<std::uint16_t, std::uint16_t>, 8>
          expected_geysers{{
              {2432, 2976}, {672, 2976}, {2944, 1856}, {128, 1824},
              {128, 1216}, {2944, 1216}, {640, 96}, {2432, 96},
          }};
      const RuntimeUnitType& runtime = status.runtime_unit_types[188U];
      geysers_verified = status.geyser_asset_index < status.unit_assets.size() &&
                         runtime.ready &&
                         runtime.asset_index == status.geyser_asset_index;
      geyser_probe_stage = geysers_verified ? 1 : 0;
      std::size_t geyser_count{};
      for (const ScenarioUnitPreview& unit : status.units) {
        if (!unit.alive || unit.unit_type != 188U) {
          continue;
        }
        ++geyser_count;
        const bool expected_position =
            std::find(
                expected_geysers.begin(),
                expected_geysers.end(),
                std::pair<std::uint16_t, std::uint16_t>{unit.x, unit.y}) !=
            expected_geysers.end();
        geysers_verified = geysers_verified && unit.owner == 11U &&
                           unit.resource_amount == 5000U && expected_position &&
                           unit.asset_index == status.geyser_asset_index &&
                           unit.selection_width != 0U && unit.selection_height != 0U;
      }
      geysers_verified = geysers_verified && geyser_count == expected_geysers.size();
      geyser_probe_stage = geysers_verified ? 2 : geyser_probe_stage;
      // Put the primary executable's upper-left CHK geyser in the middle of
      // the 640x400 world viewport. The old probe only counted records and
      // could pass while no geyser reached the renderer.
      (void)set_camera_position(status, 640 - kMapViewportWidth / 2, 0);
      const auto visible_geyser = std::find_if(
          status.units.begin(),
          status.units.end(),
          [&status](const ScenarioUnitPreview& unit) {
            if (!unit.alive || unit.unit_type != 188U || !unit.iscript_ready ||
                unit.iscript_state.hidden ||
                unit.asset_index >= status.unit_assets.size()) {
              return false;
            }
            const UnitRenderAsset& asset = status.unit_assets[unit.asset_index];
            const int screen_x = static_cast<int>(unit.x) - status.camera_x;
            const int screen_y = static_cast<int>(unit.y) - status.camera_y;
            return screen_x >= 0 && screen_x < kMapViewportWidth &&
                   screen_y >= 0 && screen_y < kMapViewportHeight &&
                   unit.current_sprite_frame < asset.sprite_frames.size() &&
                   !asset.sprite_frames[unit.current_sprite_frame].bgra.empty();
          });
      geysers_verified = geysers_verified && status.terrain_ready &&
                         status.camera_x == 320U && status.camera_y == 0U &&
                         visible_geyser != status.units.end();
      geyser_probe_stage = geysers_verified ? 3 : geyser_probe_stage;
    }

    bool minimap_verified = true;
    if (minimap_probe) {
      minimap_verified = false;
      const std::uint16_t initial_camera_x = status.camera_x;
      const std::uint16_t initial_camera_y = status.camera_y;
      const std::size_t non_background_pixels = static_cast<std::size_t>(std::count_if(
          status.minimap.bgra.begin(),
          status.minimap.bgra.end(),
          [](const std::uint32_t pixel) { return pixel != 0xFF020202U; }));
      const std::size_t varied_pixels = status.minimap.bgra.empty()
                                            ? 0U
                                            : static_cast<std::size_t>(std::count_if(
                                                  status.minimap.bgra.begin(),
                                                  status.minimap.bgra.end(),
                                                  [&status](const std::uint32_t pixel) {
                                                    return pixel != status.minimap.bgra.front();
                                                  }));
      if (probe_client_ready && status.minimap_ready &&
          status.minimap.width == 128U && status.minimap.height == 128U &&
          status.minimap_content_width != 0U &&
          status.minimap_content_height != 0U && non_background_pixels != 0U &&
          varied_pixels != 0U) {
        constexpr int vertical_numerator = 5;
        constexpr int vertical_denominator = 6;
        const int control_width = status.minimap_control.right -
                                  status.minimap_control.left + 1;
        const int control_height = status.minimap_control.bottom -
                                   status.minimap_control.top + 1;
        const int content_left = status.minimap_control.left +
                                 status.minimap_content_x * control_width / 128;
        const int content_right = status.minimap_control.left +
                                  (status.minimap_content_x +
                                   status.minimap_content_width) *
                                      control_width / 128 -
                                  1;
        const int content_top =
            status.minimap_control.top * vertical_numerator / vertical_denominator +
            status.minimap_content_y * control_height * vertical_numerator /
                (128 * vertical_denominator);
        const int content_bottom =
            status.minimap_control.top * vertical_numerator / vertical_denominator +
            (status.minimap_content_y + status.minimap_content_height) *
                control_height * vertical_numerator /
                (128 * vertical_denominator) -
            1;
        const int maximum_camera_x = (std::max)(
            0, static_cast<int>(status.scenario_width) * 32 - kMapViewportWidth);
        const int maximum_camera_y = (std::max)(
            0, static_cast<int>(status.scenario_height) * 32 - kMapViewportHeight);
        const bool choose_far_right = initial_camera_x < maximum_camera_x / 2;
        const bool choose_far_bottom = initial_camera_y < maximum_camera_y / 2;
        const int minimap_x = choose_far_right ? content_right - 1 : content_left + 1;
        const int minimap_y = choose_far_bottom ? content_bottom - 1 : content_top + 1;
        const LPARAM minimap_click = client_point(minimap_x, minimap_y);
        SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, minimap_click);
        SendMessageA(window, WM_MOUSEMOVE, MK_LBUTTON, minimap_click);
        SendMessageA(window, WM_LBUTTONUP, 0, minimap_click);
        minimap_verified =
            status.terrain_ready && !window_state.minimap_dragging &&
            (status.camera_x != initial_camera_x ||
             status.camera_y != initial_camera_y) &&
            (status.camera_x & 7U) == 0U && (status.camera_y & 7U) == 0U &&
            status.camera_x <= maximum_camera_x && status.camera_y <= maximum_camera_y;
      }
    }

    const bool needs_scv = production_probe || movement_probe || pathfinding_probe ||
                           worker_actions_probe || harvest_visual_probe ||
                           smart_orders_probe || command_target_probe ||
                           building_placement_probe || refinery_placement_probe ||
                           construction_probe || unit_avoidance_probe ||
                           multi_status_probe || construction_status_probe ||
                           resource_feedback_probe;
    bool command_panel_verified = true;
    const std::size_t scvs_before_production = static_cast<std::size_t>(std::count_if(
        status.units.begin(), status.units.end(), [](const ScenarioUnitPreview& unit) {
          return unit.alive && unit.owner == 0U && unit.unit_type == 7U;
        }));
    if (command_panel_probe || status_panel_probe || building_working_probe || needs_scv) {
      command_panel_verified = false;
      if (status_panel_probe) {
        status.player_minerals = 1000;
      }
      const auto command_center = std::find_if(
          status.units.begin(),
          status.units.end(),
          [](const ScenarioUnitPreview& unit) {
            return unit.unit_type == 106 && unit.owner == 0;
          });
      if (command_center != status.units.end()) {
        clear_selection(status);
        command_center->selected = true;
        command_panel_verified = click_command(1) && status.last_command_position == 1;
      }
    }
    const auto active_local_producer = [&status]() -> ScenarioUnitPreview* {
      const auto producer = std::find_if(
          status.units.begin(), status.units.end(), [](ScenarioUnitPreview& unit) {
            return unit.alive && unit.owner == 0U && unit.production_active &&
                   !unit.production_queue.empty();
          });
      return producer == status.units.end() ? nullptr : &*producer;
    };
    const auto last_local_scv = [&status]() -> ScenarioUnitPreview* {
      const auto worker = std::find_if(
          status.units.rbegin(), status.units.rend(), [](ScenarioUnitPreview& unit) {
            return unit.alive && unit.owner == 0U && unit.unit_type == 7U;
          });
      return worker == status.units.rend() ? nullptr : &*worker;
    };
    bool harvest_queue_verified = true;
    int harvest_queue_probe_stage{};
    if (harvest_queue_probe) {
      std::array<std::uint32_t, 2> worker_ids{};
      std::size_t worker_count{};
      for (const ScenarioUnitPreview& unit : status.units) {
        if (unit.alive && unit.owner == 0U && unit.unit_type == 7U &&
            worker_count < worker_ids.size()) {
          worker_ids[worker_count++] = unit.unit_id;
        }
      }
      const auto mineral = std::find_if(
          status.units.begin(), status.units.end(), [](const ScenarioUnitPreview& unit) {
            return unit.alive && unit.unit_type >= 176U && unit.unit_type <= 178U &&
                   unit.resource_amount >= 20U;
          });
      harvest_queue_verified = worker_count == worker_ids.size() &&
                               mineral != status.units.end();
      harvest_queue_probe_stage = harvest_queue_verified ? 1 : 0;
      const std::uint32_t mineral_id =
          mineral == status.units.end() ? 0U : mineral->unit_id;
      const std::uint16_t resource_before =
          mineral == status.units.end() ? 0U : mineral->resource_amount;
      if (harvest_queue_verified) {
        mineral->harvest_queue.clear();
        for (const std::uint32_t worker_id : worker_ids) {
          ScenarioUnitPreview* const worker = find_unit_by_id(status, worker_id);
          ScenarioUnitPreview* const source = find_unit_by_id(status, mineral_id);
          harvest_queue_verified =
              harvest_queue_verified && worker != nullptr && source != nullptr;
          if (worker != nullptr && source != nullptr) {
            cancel_unit_order(status, *worker);
            harvest_queue_verified =
                begin_scv_interaction(
                    status, *worker, *source, ActiveUnitOrder::gather) &&
                harvest_queue_verified;
          }
        }
      }
      harvest_queue_probe_stage =
          harvest_queue_verified ? 2 : harvest_queue_probe_stage;
      bool observed_waiter{};
      bool observed_promotion{};
      bool workers_remained_distinct{};
      std::uint32_t first_active{};
      for (int tick = 0; harvest_queue_verified && tick < 2048; ++tick) {
        SendMessageA(window, WM_TIMER, 1, 0);
        ScenarioUnitPreview* const source = find_unit_by_id(status, mineral_id);
        ScenarioUnitPreview* const first = find_unit_by_id(status, worker_ids[0]);
        ScenarioUnitPreview* const second = find_unit_by_id(status, worker_ids[1]);
        harvest_queue_verified = source != nullptr && first != nullptr &&
                                 second != nullptr && first->alive && second->alive &&
                                 first->asset_index < status.unit_assets.size() &&
                                 second->asset_index < status.unit_assets.size();
        if (!harvest_queue_verified) {
          break;
        }
        const std::uint32_t active = source->harvest_queue.active_worker();
        if (!observed_waiter && active != 0U &&
            source->harvest_queue.queued_count() == 1U) {
          observed_waiter = true;
          first_active = active;
          workers_remained_distinct =
              !unit_footprints_overlap_at(*first, first->x, first->y, *second);
        }
        if (observed_waiter && active != 0U && active != first_active &&
            source->resource_amount < resource_before) {
          observed_promotion = true;
          break;
        }
      }
      harvest_queue_verified = harvest_queue_verified && observed_waiter &&
                               observed_promotion && workers_remained_distinct;
      harvest_queue_probe_stage =
          harvest_queue_verified ? 3 : harvest_queue_probe_stage;
      for (const std::uint32_t worker_id : worker_ids) {
        ScenarioUnitPreview* const worker = find_unit_by_id(status, worker_id);
        if (worker != nullptr) {
          cancel_unit_order(status, *worker);
        }
      }
      ScenarioUnitPreview* const source = find_unit_by_id(status, mineral_id);
      if (source != nullptr) {
        source->harvest_queue.clear();
      }
    }
    bool status_panel_verified = true;
    if (status_panel_probe) {
      const bool queued_twice = command_panel_verified && click_command(1);
      ScenarioUnitPreview* const producer = active_local_producer();
      const std::uint16_t queued_type =
          producer == nullptr ? starcraft::lang::cunit_production_empty
                              : producer->production_queue.front();
      const std::uint32_t duration =
          queued_type < status.runtime_unit_types.size()
              ? static_cast<std::uint32_t>(starcraft::lang::production_total_ticks(
                    status.runtime_unit_types[queued_type]
                        .initialization.simulation.build_time)) *
                    kSimulationTickMilliseconds
              : 0U;
      if (queued_twice && duration != 0U && producer != nullptr) {
        producer->production_started = GetTickCount() - duration / 2U;
      }
      status_panel_verified =
          queued_twice && status.status_panel_ready &&
          status.status_progress_art_ready && status.group_wireframe_ready &&
          status.status_wireframe_control.left == 168 &&
          status.status_wireframe_control.top == 388 &&
          status.status_wireframe_control.right == 231 &&
          status.status_wireframe_control.bottom == 451 &&
          status.status_queue_controls.front().left == 242 &&
          status.status_queue_controls.front().top == 402 &&
          status.status_selection_controls.front().left == 168 &&
          status.status_selection_controls.front().top == 396 &&
          !status_text(status, 107).empty() && producer != nullptr &&
          producer->production_active && producer->production_queue.count() == 2U;
    }
    bool resource_feedback_verified = true;
    if (resource_feedback_probe) {
      ScenarioUnitPreview* const producer = active_local_producer();
      const std::size_t queued_before =
          producer == nullptr ? 0U : producer->production_queue.count();
      const std::uint32_t mineral_before = status.player_minerals;
      const std::uint32_t errors_before = status.resource_error_count;
      const std::uint32_t sounds_before = window_state.audio_play_count;
      const bool rejected_click = click_command(1);
      resource_feedback_verified =
          rejected_click && mineral_before < status.scv_simulation.mineral_cost &&
          status.player_minerals == mineral_before &&
          producer != nullptr && producer->production_queue.count() == queued_before &&
          status.resource_error_count == errors_before + 1U &&
          status.system_message == printable_status_text(status_text(status, 781U)) &&
          static_cast<std::int32_t>(
              status.system_message_until - GetTickCount()) > 0 &&
          status.pending_resource_error_sound == 0xFFU && window_state.audio_ready &&
          window_state.audio_play_count == sounds_before + 1U;
    }
    bool resource_strip_verified = true;
    int resource_strip_probe_stage{};
    if (resource_strip_probe) {
      const std::array<std::uint32_t, 2> supply = local_supply(status);
      for (int tick = 0; tick < 32; ++tick) {
        advance_resource_display(status);
      }
      resource_strip_probe_stage =
          status.resource_panel_ready && status.unit_traits_ready ? 1 : 0;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 1 &&
          status.resource_supply_control.left == 436 &&
                  status.resource_supply_control.top == 0 &&
                  status.resource_gas_control.left == 368 &&
                  status.resource_gas_control.top == 0 &&
                  status.resource_mineral_control.left == 300 &&
                  status.resource_mineral_control.top == 0
              ? 2
              : resource_strip_probe_stage;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 2 && status.displayed_minerals != 0U &&
                  status.displayed_minerals <= status.player_minerals &&
                  status.displayed_gas == status.player_gas
              ? 3
              : resource_strip_probe_stage;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 3 && supply[1] != 0U
              ? 4
              : resource_strip_probe_stage;
      resource_strip_verified = resource_strip_probe_stage == 4;
    }
    bool music_verified = true;
    if (music_probe) {
      ALint source_state{};
      if (window_state.audio_context != nullptr) {
        (void)alcMakeContextCurrent(window_state.audio_context);
        alGetSourcei(window_state.music_source, AL_SOURCE_STATE, &source_state);
      }
      music_verified =
          status.music_available && status.music_path == R"(music\terran1.wav)" &&
          !status.music_wave.empty() && window_state.audio_ready &&
          window_state.music_playing && window_state.music_buffer != 0U &&
          window_state.music_source != 0U && source_state == AL_PLAYING;
    }
    bool unit_audio_verified = true;
    if (unit_audio_probe) {
      unit_audio_verified = false;
      clear_selection(status);
      const auto audible = std::find_if(
          status.units.begin(),
          status.units.end(),
          [&status](const ScenarioUnitPreview& unit) {
            const int game_x = static_cast<int>(unit.x) - status.camera_x;
            const int game_y = static_cast<int>(unit.y) - status.camera_y;
            return unit.alive && unit.owner == 0U &&
                   unit.unit_type < status.unit_sound_ranges.size() &&
                   status.unit_sound_ranges[unit.unit_type].what_first != 0U &&
                   game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
                   game_y < kMapViewportHeight &&
                   !hud_pixel_opaque(status, game_x, game_y);
          });
      if (audible != status.units.end() && probe_client_ready) {
        const std::uint32_t sounds_before = window_state.audio_play_count;
        const int game_x = static_cast<int>(audible->x) - status.camera_x;
        const int game_y = static_cast<int>(audible->y) - status.camera_y;
        const LPARAM click = client_point(game_x, game_y);
        SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
        SendMessageA(window, WM_LBUTTONUP, 0, click);
        const UnitSoundRanges& ranges = status.unit_sound_ranges[audible->unit_type];
        ALint music_state{};
        alGetSourcei(window_state.music_source, AL_SOURCE_STATE, &music_state);
        unit_audio_verified =
            audible->selected && selection_count(status) == 1U &&
            status.pending_game_sound == 0xFFFFU &&
            status.last_game_sound >= ranges.what_first &&
            status.last_game_sound <= ranges.what_last &&
            window_state.audio_play_count == sounds_before + 1U &&
            music_state == AL_PLAYING;
      }
    }
    bool building_working_verified = true;
    if (building_working_probe) {
      building_working_verified = false;
      ScenarioUnitPreview* const producer_pointer = active_local_producer();
      if (producer_pointer != nullptr) {
        ScenarioUnitPreview& producer = *producer_pointer;
        const std::uint16_t initial_overlay_pc =
            producer.dynamic_overlay_iscript_state.program_counter;
        for (int tick = 0; tick < 10; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        if (producer.dynamic_overlay_asset_index < status.unit_assets.size()) {
          const UnitRenderAsset& working_asset =
              status.unit_assets[producer.dynamic_overlay_asset_index];
          building_working_verified =
              producer.production_active && producer.unit_type == 106U &&
              producer.last_animation == 19U && producer.dynamic_overlay_ready &&
              producer.dynamic_overlay_asset_index ==
                  status.command_center_working_asset_index &&
              working_asset.image_id == 247U &&
              producer.dynamic_overlay_iscript_state.program_counter !=
                  initial_overlay_pc;
        }
      }
    }
    bool production_verified = true;
    if (needs_scv) {
      std::uint32_t producer_id{};
      ScenarioUnitPreview* producer_pointer = active_local_producer();
      if (producer_pointer != nullptr) {
        producer_id = producer_pointer->unit_id;
      }
      production_verified =
          producer_pointer != nullptr && producer_pointer->last_animation == 19U &&
          producer_pointer->dynamic_overlay_ready;
      if (production_verified) {
        const std::uint16_t queued_type = producer_pointer->production_queue.front();
        producer_pointer->production_started =
            GetTickCount() - static_cast<std::uint32_t>(
                                 starcraft::lang::production_total_ticks(
                                     status.runtime_unit_types[queued_type]
                                         .initialization.simulation.build_time)) *
                                 kSimulationTickMilliseconds;
        SendMessageA(window, WM_TIMER, 1, 0);
        const ScenarioUnitPreview* const producer =
            find_unit_by_id(status, producer_id);
        ScenarioUnitPreview* const produced_worker = last_local_scv();
        const std::size_t scvs_after = static_cast<std::size_t>(std::count_if(
            status.units.begin(), status.units.end(), [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.owner == 0U && unit.unit_type == 7U;
            }));
        bool spawn_clear = produced_worker != nullptr && producer != nullptr;
        if (spawn_clear) {
          spawn_clear =
              (produced_worker->x != producer->x ||
               produced_worker->y != producer->y) &&
              std::abs(static_cast<int>(produced_worker->x) - producer->x) <= 128 &&
              std::abs(static_cast<int>(produced_worker->y) - producer->y) <= 128;
          for (const ScenarioUnitPreview& other : status.units) {
            if (!spawn_clear) {
              break;
            }
            if (other.alive && &other != produced_worker) {
              spawn_clear = !unit_footprints_overlap_at(
                  *produced_worker,
                  produced_worker->x,
                  produced_worker->y,
                  other);
            }
          }
          static const std::vector<starcraft::lang::PathObstacle> no_obstacles;
          spawn_clear =
              spawn_clear && starcraft::lang::path_position_passable(
                                 status.pathing_map,
                                 produced_worker->x,
                                 produced_worker->y,
                                 produced_worker->selection_width,
                                 produced_worker->selection_height,
                                 no_obstacles);
        }
        production_verified = producer != nullptr && !producer->production_active &&
                              scvs_after == scvs_before_production + 1U &&
                              produced_worker != nullptr && spawn_clear &&
                              produced_worker->iscript_ready &&
                              !produced_worker->iscript_state.hidden &&
                              produced_worker->asset_index <
                                  status.unit_assets.size() &&
                              produced_worker->current_sprite_frame <
                                  status.unit_assets[produced_worker->asset_index]
                                      .sprite_frames.size() &&
                              producer->last_animation == 20U &&
                              !producer->dynamic_overlay_ready;
      }
    }
    bool smart_orders_verified = true;
    if (smart_orders_probe) {
      smart_orders_verified = false;
      ScenarioUnitPreview* const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview& worker = *worker_pointer;
        worker.selected = worker.alive && worker.owner == 0U && worker.unit_type == 7U;
        const auto mineral = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.resource_amount != 0U &&
                     (unit.dat_flags & 0x2000U) != 0U;
            });
        const auto enemy = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.owner != 0U && unit.owner != 11U &&
                     unit.max_hit_points != 0U;
            });
        bool gathered{};
        if (worker.selected && mineral != status.units.end()) {
          gathered = issue_scv_smart_order(status, mineral->x, mineral->y) == 1U &&
                     worker.active_order == ActiveUnitOrder::gather &&
                     worker.order_target_id == mineral->unit_id &&
                     status.last_issued_order == 0x4EU;
          cancel_unit_order(status, worker);
        }
        bool attacked{};
        if (gathered && enemy != status.units.end()) {
          attacked = issue_scv_smart_order(status, enemy->x, enemy->y) == 1U &&
                     worker.active_order == ActiveUnitOrder::attack &&
                     worker.order_target_id == enemy->unit_id &&
                     status.last_issued_order == 9U;
          cancel_unit_order(status, worker);
        }
        smart_orders_verified = gathered && attacked;
      }
    }
    bool harvest_visual_verified = true;
    if (harvest_visual_probe) {
      harvest_visual_verified = false;
      ScenarioUnitPreview* const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview& worker = *worker_pointer;
        worker.selected = worker.alive && worker.owner == 0U && worker.unit_type == 7U;
        auto mineral = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.unit_type >= 176U && unit.unit_type <= 178U &&
                     unit.resource_amount >= 10U;
            });
        if (worker.selected && mineral != status.units.end() &&
            begin_scv_interaction(
                status, worker, *mineral, ActiveUnitOrder::gather)) {
          worker.x = worker.movement_final_x;
          worker.y = worker.movement_final_y;
          worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
          worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
          stop_unit_movement(status, worker);
          bool animated{};
          bool cutter_event{};
          const std::uint32_t sounds_before = window_state.audio_play_count;
          for (int tick = 0;
               tick < 140 && worker.active_order != ActiveUnitOrder::return_cargo;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
            animated = animated ||
                       (worker.iscript_ready && worker.last_animation == 15U &&
                        (worker.current_sprite_frame == 51U ||
                         worker.current_sprite_frame == 102U));
            cutter_event = cutter_event ||
                           (worker.iscript_state.weapon_event_count != 0U &&
                            worker.iscript_state.weapon_event == 8U);
          }
          const UnitRenderAsset* const cargo_asset =
              status.mineral_cargo_asset_index < status.unit_assets.size()
                  ? &status.unit_assets[status.mineral_cargo_asset_index]
                  : nullptr;
          const std::uint16_t harvest_x = worker.x;
          const std::uint16_t harvest_y = worker.y;
          for (int tick = 0;
               tick < 48 && worker.cargo_minerals != 0U && worker.moving &&
               std::abs(static_cast<int>(worker.x) - harvest_x) +
                       std::abs(static_cast<int>(worker.y) - harvest_y) <
                   64;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
          }
          (void)set_camera_position(
              status,
              static_cast<int>(worker.x) - kMapViewportWidth / 2,
              static_cast<int>(worker.y) - kMapViewportHeight / 2);
          harvest_visual_verified =
              worker.active_order == ActiveUnitOrder::return_cargo &&
              worker.cargo_minerals == 8U && animated && cutter_event &&
              window_state.audio_play_count > sounds_before && cargo_asset != nullptr &&
              cargo_asset->image_id == 357U && cargo_asset->overlay_ready &&
              cargo_asset->overlay_image_id == 365U &&
              render_opengl(window, window_state);
        }
      }
    }
    bool multi_status_verified = true;
    if (multi_status_probe) {
      multi_status_verified = false;
      if (production_verified && !status.units.empty()) {
        ScenarioUnitPreview* command_center{};
        ScenarioUnitPreview* scv{};
        for (ScenarioUnitPreview& unit : status.units) {
          if (unit.alive && unit.owner == 0 && unit.unit_type == 106U) {
            command_center = &unit;
          } else if (unit.alive && unit.owner == 0 && unit.unit_type == 7U) {
            scv = &unit;
          }
        }
        if (command_center != nullptr && scv != nullptr) {
          clear_selection(status);
          command_center->selected = true;
          scv->selected = true;
          multi_status_verified =
              selection_count(status) == 2U && status.group_wireframe_ready &&
              command_center->unit_type < status.group_wireframe_frames.size() &&
              scv->unit_type < status.group_wireframe_frames.size();
        }
      }
    }
    bool movement_verified = true;
    int movement_probe_stage{};
    if (movement_probe) {
      movement_verified = false;
      ScenarioUnitPreview* const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        movement_probe_stage = 1;
        clear_selection(status);
        ScenarioUnitPreview& scv = *scv_pointer;
        scv.selected = true;
        const std::uint16_t initial_x = scv.x;
        const std::uint16_t initial_y = scv.y;
        const int game_x = scv.x - status.camera_x;
        const int game_y = scv.y - status.camera_y;
        const int target_game_x = game_x > 320 ? game_x - 96 : game_x + 96;
        const int target_game_y = (std::clamp)(game_y, 32, 240);
        SendMessageA(
            window,
            WM_RBUTTONDOWN,
            MK_RBUTTON,
            client_point(target_game_x, target_game_y));
        const bool order_started = scv.moving && command_card_for(status).count == 9;
        for (int tick = 0; tick < 16; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        const bool scv_moved =
            order_started && (scv.x != initial_x || scv.y != initial_y);
        movement_probe_stage = scv_moved ? 2 : movement_probe_stage;

        // Walking-control units enter action 11 with flingy.dat's sentinel
        // speed and receive their actual fixed-point speed through iscript
        // opcode 0x3F. Exercise a Marine through the same right-click path;
        // this specifically guards against the old "only SCVs move" result.
        cancel_unit_order(status, scv);
        const bool marine_ready = configure_preview_type(status, scv, 0U);
        movement_probe_stage = marine_ready ? 3 : movement_probe_stage;
        starcraft::lang::PathPoint marine_start{};
        starcraft::lang::PathPoint marine_target{};
        bool marine_path_ready{};
        if (marine_ready) {
          std::vector<starcraft::lang::PathObstacle> obstacles;
          marine_path_ready = collect_building_obstacles(status, &scv, obstacles);
          for (int y = 96;
               marine_path_ready && y < status.pathing_map.pixel_height() - 96;
               y += 32) {
            for (int x = 96; x < status.pathing_map.pixel_width() - 224; x += 32) {
              std::vector<starcraft::lang::PathPoint> path;
              if (!creation_position_passable(status, scv, x, y) ||
                  !creation_position_passable(status, scv, x + 128, y) ||
                  !starcraft::lang::find_unit_path(
                      status.pathing_map,
                      x,
                      y,
                      x + 128,
                      y,
                      scv.selection_width,
                      scv.selection_height,
                      obstacles,
                      path) ||
                  path.empty()) {
                continue;
              }
              marine_start = {static_cast<std::uint16_t>(x),
                              static_cast<std::uint16_t>(y)};
              marine_target = {static_cast<std::uint16_t>(x + 128),
                               static_cast<std::uint16_t>(y)};
              marine_path_ready = true;
              break;
            }
            if (marine_target.x != 0U) {
              break;
            }
          }
          marine_path_ready = marine_target.x != 0U;
        }
        movement_probe_stage = marine_path_ready ? 4 : movement_probe_stage;
        if (marine_path_ready) {
          scv.x = marine_start.x;
          scv.y = marine_start.y;
          scv.x_fixed = static_cast<std::int32_t>(scv.x) << 8U;
          scv.y_fixed = static_cast<std::int32_t>(scv.y) << 8U;
          // The selected probe path points east (direction 64). Start north
          // so the probe covers CFlingy facing rotation as well as translation.
          scv.direction = 0U;
          (void)set_camera_position(
              status,
              static_cast<int>(scv.x) - kMapViewportWidth / 2,
              static_cast<int>(scv.y) - 160);
          const int marine_target_game_x = marine_target.x - status.camera_x;
          const int marine_target_game_y = marine_target.y - status.camera_y;
          SendMessageA(
              window,
              WM_RBUTTONDOWN,
              MK_RBUTTON,
              client_point(marine_target_game_x, marine_target_game_y));
        }
        const std::uint16_t marine_initial_x = scv.x;
        const std::uint16_t marine_initial_y = scv.y;
        const std::uint8_t marine_initial_direction = scv.direction;
        movement_probe_stage = scv.moving ? 5 : movement_probe_stage;
        movement_probe_stage = scv.last_animation == 11U ? 6 : movement_probe_stage;
        const bool marine_iscript_speed =
            scv.iscript_state.flingy_velocity_event_count != 0U ||
            scv.iscript_state.flingy_speed_event_count != 0U;
        movement_probe_stage = marine_iscript_speed
                                   ? 7
                                   : movement_probe_stage;
        movement_probe_stage = effective_unit_top_speed(scv) > scv.movement_top_speed
                                   ? 8
                                   : movement_probe_stage;
        const bool marine_order_started =
            marine_ready && scv.moving && scv.last_animation == 11U &&
            marine_iscript_speed &&
            effective_unit_top_speed(scv) > scv.movement_top_speed;
        for (int tick = 0; tick < 16; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        const int marine_distance =
            std::abs(static_cast<int>(scv.x) - marine_initial_x) +
            std::abs(static_cast<int>(scv.y) - marine_initial_y);
        const bool marine_turning_image =
            scv.asset_index < status.unit_assets.size() &&
            status.unit_assets[scv.asset_index].graphics_turns;
        movement_probe_stage = marine_turning_image ? 9 : movement_probe_stage;
        const starcraft::game::ImageFacingFrame initial_facing =
            starcraft::game::image_facing_frame(marine_initial_direction);
        const starcraft::game::ImageFacingFrame current_facing =
            starcraft::game::image_facing_frame(scv.direction);
        const bool marine_turned =
            scv.direction != marine_initial_direction &&
            (current_facing.frame != initial_facing.frame ||
             current_facing.mirrored != initial_facing.mirrored);
        movement_probe_stage = marine_turned ? 10 : movement_probe_stage;
        movement_verified =
            scv_moved && marine_order_started && marine_distance >= 8 &&
            marine_turning_image && marine_turned;
        movement_probe_stage = movement_verified ? 11 : movement_probe_stage;
      }
    }
    bool pathfinding_verified = true;
    if (pathfinding_probe) {
      pathfinding_verified = false;
      ScenarioUnitPreview* const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview& scv = *scv_pointer;
        scv.selected = true;
        std::vector<starcraft::lang::PathObstacle> obstacles;
        if (collect_building_obstacles(status, &scv, obstacles)) {
          for (const ScenarioUnitPreview& building : status.units) {
            if (!building.is_building || pathfinding_verified) {
              continue;
            }
            const int half_worker_width = scv.selection_width / 2;
            const int half_worker_height = scv.selection_height / 2;
            const int half_building_width = building.selection_width / 2;
            const int half_building_height = building.selection_height / 2;
            const int clearance_x = half_building_width + half_worker_width + 24;
            const int clearance_y = half_building_height + half_worker_height + 24;
            const std::array<std::array<int, 4>, 2> candidates{{
                {{static_cast<int>(building.x) - clearance_x,
                  building.y,
                  static_cast<int>(building.x) + clearance_x,
                  building.y}},
                {{building.x,
                  static_cast<int>(building.y) - clearance_y,
                  building.x,
                  static_cast<int>(building.y) + clearance_y}},
            }};
            for (const auto& candidate : candidates) {
              if (candidate[0] < 0 || candidate[1] < 0 || candidate[2] < 0 ||
                  candidate[3] < 0 || candidate[0] >= status.pathing_map.pixel_width() ||
                  candidate[2] >= status.pathing_map.pixel_width() ||
                  candidate[1] >= status.pathing_map.pixel_height() ||
                  candidate[3] >= status.pathing_map.pixel_height() ||
                  !starcraft::lang::path_position_passable(
                      status.pathing_map,
                      candidate[0],
                      candidate[1],
                      scv.selection_width,
                      scv.selection_height,
                      obstacles) ||
                  !starcraft::lang::path_position_passable(
                      status.pathing_map,
                      candidate[2],
                      candidate[3],
                      scv.selection_width,
                      scv.selection_height,
                      obstacles)) {
                continue;
              }
              scv.x = static_cast<std::uint16_t>(candidate[0]);
              scv.y = static_cast<std::uint16_t>(candidate[1]);
              scv.x_fixed = static_cast<std::int32_t>(scv.x) << 8U;
              scv.y_fixed = static_cast<std::int32_t>(scv.y) << 8U;
              scv.direction = starcraft::lang::direction_from_points(
                  scv.x,
                  scv.y,
                  static_cast<std::uint16_t>(candidate[2]),
                  static_cast<std::uint16_t>(candidate[3]));
              const bool order_started = issue_scv_move_order(
                                             status,
                                             static_cast<std::uint16_t>(candidate[2]),
                                             static_cast<std::uint16_t>(candidate[3])) == 1 &&
                                         scv.movement_path.size() >= 2;
              bool remained_passable = order_started;
              for (int tick = 0; tick < 768 && scv.moving && remained_passable; ++tick) {
                SendMessageA(window, WM_TIMER, 1, 0);
                remained_passable = starcraft::lang::path_position_passable(
                    status.pathing_map,
                    scv.x,
                    scv.y,
                    scv.selection_width,
                    scv.selection_height,
                    obstacles);
              }
              pathfinding_verified =
                  order_started && remained_passable && !scv.moving &&
                  scv.x == candidate[2] && scv.y == candidate[3];
              if (pathfinding_verified) {
                break;
              }
            }
          }
        }
      }
    }
    bool unit_avoidance_verified = true;
    int unit_avoidance_probe_stage{};
    if (unit_avoidance_probe) {
      unit_avoidance_verified = false;
      ScenarioUnitPreview* const produced_pointer = last_local_scv();
      if (production_verified && produced_pointer != nullptr) {
        unit_avoidance_probe_stage = 1;
        ScenarioUnitPreview& produced = *produced_pointer;
        std::vector<starcraft::lang::PathObstacle> obstacles;
        // units.dat flag 0x08 makes workers pass mobile CUnits in the original
        // collision predicate; CUnitHarvest serializes their resource access.
        // Exercise CUnitPathCollide's mobile/mobile branch with Vulture type 2
        // (the Factory card's first product) instead of falsifying the SCV's
        // recovered worker property. Its CFlingy uses the ordinary speed path;
        // infantry type 0 uses movement-control 2's walking state machine.
        if (configure_preview_type(status, produced, 2U) &&
            collect_building_obstacles(status, &produced, obstacles)) {
          std::array<starcraft::lang::PathPoint, 4> crossing{};
          bool crossing_found{};
          const auto clear_of_existing_units = [&](const int x, const int y) {
            return std::none_of(
                status.units.begin(),
                status.units.end(),
                [&produced, x, y](const ScenarioUnitPreview& other) {
                  return other.alive && other.unit_id != produced.unit_id &&
                         unit_footprints_overlap_at(produced, x, y, other);
                });
          };
          for (int y = 96; y + 64 < status.pathing_map.pixel_height() &&
                              !crossing_found;
               y += 16) {
            for (int x = 96; x + 64 < status.pathing_map.pixel_width(); x += 16) {
              crossing = {{
                  {static_cast<std::uint16_t>(x - 64), static_cast<std::uint16_t>(y)},
                  {static_cast<std::uint16_t>(x + 64), static_cast<std::uint16_t>(y)},
                  {static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y - 64)},
                  {static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y + 64)},
              }};
              bool points_clear = clear_of_existing_units(x, y);
              for (const starcraft::lang::PathPoint& point : crossing) {
                points_clear =
                    points_clear && clear_of_existing_units(point.x, point.y) &&
                    starcraft::lang::path_position_passable(
                        status.pathing_map,
                        point.x,
                        point.y,
                        produced.selection_width,
                        produced.selection_height,
                        obstacles);
              }
              std::vector<starcraft::lang::PathPoint> horizontal;
              std::vector<starcraft::lang::PathPoint> vertical;
              crossing_found =
                  points_clear &&
                  starcraft::lang::find_unit_path(
                      status.pathing_map,
                      crossing[0].x,
                      crossing[0].y,
                      crossing[1].x,
                      crossing[1].y,
                      produced.selection_width,
                      produced.selection_height,
                      obstacles,
                      horizontal) &&
                  horizontal.size() == 1U &&
                  starcraft::lang::find_unit_path(
                      status.pathing_map,
                      crossing[2].x,
                      crossing[2].y,
                      crossing[3].x,
                      crossing[3].y,
                      produced.selection_width,
                      produced.selection_height,
                      obstacles,
                      vertical) &&
                  vertical.size() == 1U;
              if (crossing_found) {
                break;
              }
            }
          }
          if (crossing_found) {
            unit_avoidance_probe_stage = 2;
            const std::uint32_t first_id = produced.unit_id;
            produced.x = crossing[0].x;
            produced.y = crossing[0].y;
            produced.x_fixed = static_cast<std::int32_t>(produced.x) << 8U;
            produced.y_fixed = static_cast<std::int32_t>(produced.y) << 8U;
            produced.selected = false;
            cancel_unit_order(status, produced);
            ScenarioUnitPreview second = produced;
            second.unit_id = status.next_unit_id++;
            second.x = crossing[2].x;
            second.y = crossing[2].y;
            second.x_fixed = static_cast<std::int32_t>(second.x) << 8U;
            second.y_fixed = static_cast<std::int32_t>(second.y) << 8U;
            const std::uint32_t second_id = second.unit_id;
            status.units.push_back(std::move(second));
            ScenarioUnitPreview* first = find_unit_by_id(status, first_id);
            ScenarioUnitPreview* other = find_unit_by_id(status, second_id);
            if (first != nullptr && other != nullptr &&
                plan_scv_path(status, *first, crossing[1].x, crossing[1].y) &&
                plan_scv_path(status, *other, crossing[3].x, crossing[3].y)) {
              unit_avoidance_probe_stage = 3;
              first->moving = true;
              first->active_order = ActiveUnitOrder::move;
              other->moving = true;
              other->active_order = ActiveUnitOrder::move;
              bool separated = true;
              bool avoidance_observed{};
              for (int tick = 0;
                   tick < 1536 && separated && (first->moving || other->moving);
                   ++tick) {
                SendMessageA(window, WM_TIMER, 1, 0);
                first = find_unit_by_id(status, first_id);
                other = find_unit_by_id(status, second_id);
                if (first == nullptr || other == nullptr) {
                  separated = false;
                  break;
                }
                separated = !unit_footprints_overlap_at(
                    *first, first->x, first->y, *other);
                avoidance_observed =
                    avoidance_observed || first->avoidance_ticks != 0U ||
                    other->avoidance_ticks != 0U ||
                    first->collision_wait_ticks != 0U ||
                    other->collision_wait_ticks != 0U;
              }
              unit_avoidance_verified =
                  separated && avoidance_observed && !first->moving && !other->moving &&
                  first->x == crossing[1].x && first->y == crossing[1].y &&
                  other->x == crossing[3].x && other->y == crossing[3].y;
              unit_avoidance_probe_stage = avoidance_observed ? 4 : 3;
              if (avoidance_observed && !separated) {
                unit_avoidance_probe_stage = 12;
              } else if (avoidance_observed && (first->moving || other->moving)) {
                unit_avoidance_probe_stage = 11;
              } else if (avoidance_observed &&
                         (first->x != crossing[1].x || first->y != crossing[1].y ||
                          other->x != crossing[3].x || other->y != crossing[3].y)) {
                unit_avoidance_probe_stage = 10;
              }
              unit_avoidance_probe_stage = unit_avoidance_verified ? 5 : unit_avoidance_probe_stage;
            }
          }
        }
      }
    }
    bool portrait_verified = true;
    if (portrait_probe) {
      portrait_verified = false;
      clear_selection(status);
      for (ScenarioUnitPreview& unit : status.units) {
        unit.selected = true;
        UnitPortraitAsset* const portrait = selected_portrait(status);
        if (portrait != nullptr && portrait->portrait.ready &&
            portrait->portrait.frame.width != 0 && portrait->portrait.frame.height != 0 &&
            portrait->portrait.frame.bgra.size() ==
                static_cast<std::size_t>(portrait->portrait.frame.width) *
                    portrait->portrait.frame.height) {
          const std::uint32_t next_tick = portrait->portrait.last_frame_tick +
                                          portrait->portrait.frame_delay_ms;
          portrait_verified = advance_selected_portrait(status, next_tick) &&
                              portrait->portrait.ready &&
                              !portrait->portrait.frame.bgra.empty();
          break;
        }
        unit.selected = false;
      }
    }
    bool worker_actions_verified = true;
    if (worker_actions_probe) {
      worker_actions_verified = false;
      ScenarioUnitPreview* const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview& worker = *worker_pointer;
        worker.selected = true;
        const auto move_to_interaction_point = [&](ScenarioUnitPreview& target,
                                                   const ActiveUnitOrder order) {
          if (!begin_scv_interaction(status, worker, target, order)) {
            return false;
          }
          worker.x = worker.movement_final_x;
          worker.y = worker.movement_final_y;
          worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
          worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
          stop_unit_movement(status, worker);
          return unit_edge_distance(worker, target) <=
                 (order == ActiveUnitOrder::attack
                      ? static_cast<int>(worker.weapon_range)
                      : 8);
        };

        auto enemy = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.owner != 0 && unit.owner != 11 &&
                     unit.max_hit_points != 0;
            });
        bool attack_verified{};
        if (enemy != status.units.end()) {
          enemy->hit_points = 128;
          attack_verified =
              move_to_interaction_point(*enemy, ActiveUnitOrder::attack);
          if (attack_verified) {
            SendMessageA(window, WM_TIMER, 1, 0);
            attack_verified = !enemy->alive && enemy->hit_points == 0 &&
                              worker.active_order == ActiveUnitOrder::none;
          }
        }

        auto command_center = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.owner == 0 && unit.unit_type == 106;
            });
        bool repair_verified{};
        if (attack_verified && command_center != status.units.end()) {
          const std::uint32_t damage = (std::max)(
              256U, static_cast<std::uint32_t>(command_center->repair_step) * 2U);
          command_center->hit_points = command_center->max_hit_points - damage;
          status.player_minerals = 10000;
          status.player_gas = 10000;
          repair_verified =
              move_to_interaction_point(*command_center, ActiveUnitOrder::repair);
          for (int tick = 0; tick < 8 && repair_verified &&
                             command_center->hit_points < command_center->max_hit_points;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
          }
          repair_verified =
              repair_verified &&
              command_center->hit_points == command_center->max_hit_points &&
              worker.active_order == ActiveUnitOrder::none;
        }

        auto mineral = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.unit_type >= 176U && unit.unit_type <= 178U &&
                     unit.resource_amount >= 10U;
            });
        bool gather_verified{};
        if (repair_verified && mineral != status.units.end()) {
          const std::uint16_t resource_before = mineral->resource_amount;
          const std::uint32_t minerals_before = status.player_minerals;
          const std::uint32_t sounds_before = window_state.audio_play_count;
          bool mining_animation_observed{};
          bool mining_weapon_observed{};
          gather_verified = move_to_interaction_point(*mineral, ActiveUnitOrder::gather);
          for (int tick = 0; tick < 140 && gather_verified &&
                             worker.active_order != ActiveUnitOrder::return_cargo;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
            mining_animation_observed =
                mining_animation_observed ||
                (worker.iscript_ready && worker.last_animation == 15U &&
                 (worker.current_sprite_frame == 51U ||
                  worker.current_sprite_frame == 102U));
            mining_weapon_observed =
                mining_weapon_observed ||
                (worker.iscript_state.weapon_event_count != 0U &&
                 worker.iscript_state.weapon_event == 8U);
          }
          const bool cargo_assets_valid =
              status.mineral_cargo_asset_index < status.unit_assets.size() &&
              status.terran_gas_cargo_asset_index < status.unit_assets.size() &&
              status.unit_assets[status.mineral_cargo_asset_index].image_id == 357U &&
              status.unit_assets[status.terran_gas_cargo_asset_index].image_id == 359U;
          gather_verified =
              gather_verified && worker.active_order == ActiveUnitOrder::return_cargo &&
              worker.cargo_minerals == 8 &&
              mineral->resource_amount + 10U == resource_before &&
              mining_animation_observed && mining_weapon_observed &&
              window_state.audio_play_count > sounds_before && cargo_assets_valid &&
              render_opengl(window, window_state);
          if (gather_verified) {
            ScenarioUnitPreview* const depot =
                find_unit_by_id(status, worker.order_target_id);
            gather_verified = depot != nullptr;
            if (depot != nullptr) {
              worker.x = worker.movement_final_x;
              worker.y = worker.movement_final_y;
              worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
              worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
              stop_unit_movement(status, worker);
              SendMessageA(window, WM_TIMER, 1, 0);
              gather_verified = status.player_minerals == minerals_before + 8U &&
                                worker.cargo_minerals == 0 &&
                                (worker.active_order == ActiveUnitOrder::gather ||
                                 worker.active_order == ActiveUnitOrder::none);
            }
          }
        }
        worker_actions_verified =
            attack_verified && repair_verified && gather_verified;
      }
    }
    bool refinery_placement_verified = true;
    int refinery_probe_stage{};
    if (refinery_placement_probe) {
      refinery_placement_verified = false;
      const BuildableUnitVisual* const refinery = find_buildable_unit(status, 110U);
      auto geyser = std::find_if(
          status.units.begin(),
          status.units.end(),
          [](const ScenarioUnitPreview& unit) {
            return unit.alive && unit.unit_type == 188U && unit.resource_amount != 0;
          });
      if (refinery != nullptr && geyser == status.units.end() &&
          refinery->asset_index < status.unit_assets.size()) {
        const UnitRenderAsset& asset = status.unit_assets[refinery->asset_index];
        ScenarioUnitPreview synthetic{};
        synthetic.unit_id = status.next_unit_id++;
        synthetic.unit_type = 188;
        synthetic.owner = 11;
        synthetic.asset_index = refinery->asset_index;
        synthetic.selection_width = refinery->placement_width;
        synthetic.selection_height = refinery->placement_height;
        synthetic.iscript_state = asset.initial_iscript_state;
        synthetic.overlay_iscript_state = asset.initial_overlay_iscript_state;
        synthetic.current_sprite_frame = asset.initial_iscript_state.frame;
        synthetic.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
        synthetic.iscript_ready = asset.iscript_ready;
        synthetic.overlay_ready = asset.overlay_ready;
        synthetic.is_building = true;
        apply_simulation_traits(synthetic, refinery->simulation);
        synthetic.dat_flags |= 0x2000U;
        synthetic.resource_amount = 5000;
        status.units.push_back(std::move(synthetic));
        ScenarioUnitPreview& inserted = status.units.back();
        const int half_width = refinery->placement_width / 2;
        const int half_height = refinery->placement_height / 2;
        bool site_found{};
        for (int y = half_height; y < status.pathing_map.pixel_height() - half_height &&
                                   !site_found;
             y += 32) {
          for (int x = half_width; x < status.pathing_map.pixel_width() - half_width;
               x += 32) {
            inserted.x = static_cast<std::uint16_t>(x);
            inserted.y = static_cast<std::uint16_t>(y);
            inserted.x_fixed = x << 8U;
            inserted.y_fixed = y << 8U;
            if (placement_is_valid(status, *refinery, inserted.x, inserted.y)) {
              site_found = true;
              break;
            }
          }
        }
        if (!site_found) {
          inserted.alive = false;
        }
        geyser = std::find_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.unit_type == 188U &&
                     unit.resource_amount != 0;
            });
      }
      refinery_probe_stage = refinery != nullptr ? 1 : 0;
      refinery_probe_stage = geyser != status.units.end() ? 2 : refinery_probe_stage;
      if (refinery != nullptr && geyser != status.units.end()) {
        refinery_probe_stage = 3;
        clear_selection(status);
        const auto worker = std::find_if(
            status.units.rbegin(),
            status.units.rend(),
            [](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.owner == 0 && unit.unit_type == 7;
            });
        if (worker != status.units.rend()) {
          worker->selected = true;
        }
        status.player_minerals = 10000;
        status.player_gas = 10000;
        status.placement_active = true;
        status.placement_unit_type = 110U;
        status.placement_x = geyser->x;
        status.placement_y = geyser->y;
        status.placement_valid =
            placement_is_valid(status, *refinery, status.placement_x, status.placement_y);
        refinery_probe_stage = status.placement_valid ? 4 : refinery_probe_stage;
        const std::uint32_t geyser_id = geyser->unit_id;
        const std::uint16_t resource_amount = geyser->resource_amount;
        const bool placed = status.placement_valid && place_current_building(status);
        refinery_probe_stage = placed ? 5 : refinery_probe_stage;
        const ScenarioUnitPreview* const consumed = find_unit_by_id(status, geyser_id);
        const auto created = std::find_if(
            status.units.rbegin(),
            status.units.rend(),
            [resource_amount](const ScenarioUnitPreview& unit) {
              return unit.alive && unit.unit_type == 110U &&
                     unit.resource_amount == resource_amount;
            });
        refinery_placement_verified =
            placed && consumed == nullptr && created != status.units.rend();
        refinery_probe_stage = consumed == nullptr ? 6 : refinery_probe_stage;
        refinery_probe_stage = created != status.units.rend() ? 7 : refinery_probe_stage;
      }
    }
    bool command_target_verified = true;
    int command_target_probe_stage{};
    if (command_target_probe) {
      command_target_verified = false;
      ScenarioUnitPreview* const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        command_target_probe_stage = 1;
        clear_selection(status);
        ScenarioUnitPreview& scv = *scv_pointer;
        scv.selected = true;
        status.active_command_card = 0;
        cancel_command_target(status);
        const int scv_game_x = scv.x - status.camera_x;
        const int scv_game_y = scv.y - status.camera_y;
        int target_game_x{-1};
        int target_game_y{-1};
        for (int y = 32; y <= 240 && target_game_x < 0; y += 16) {
          for (int x = 32; x <= 608; x += 16) {
            if (std::abs(x - scv_game_x) < 64 && std::abs(y - scv_game_y) < 64) {
              continue;
            }
            const int world_x = x + status.camera_x;
            const int world_y = y + status.camera_y;
            const bool occupied = std::any_of(
                status.units.begin(),
                status.units.end(),
                [world_x, world_y](const ScenarioUnitPreview& unit) {
                  return world_x >= unit.x - unit.selection_width / 2 &&
                         world_x <= unit.x + unit.selection_width / 2 &&
                         world_y >= unit.y - unit.selection_height / 2 &&
                         world_y <= unit.y + unit.selection_height / 2;
                });
            if (!occupied) {
              target_game_x = x;
              target_game_y = y;
              break;
            }
          }
        }
        const bool move_target_started =
            target_game_x >= 0 && click_command(1) && status.command_target_active &&
            status.target_unit_order == 0x32 && status.target_terrain_order == 7 &&
            command_card_for(status).count == 1;
        command_target_probe_stage = move_target_started ? 2 : command_target_probe_stage;
        const LPARAM target = client_point(target_game_x, target_game_y);
        if (move_target_started) {
          SendMessageA(window, WM_MOUSEMOVE, 0, target);
          SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, target);
          SendMessageA(window, WM_LBUTTONUP, 0, target);
        }
        const bool move_target_issued =
            move_target_started && !status.command_target_active &&
            status.last_issued_order == 7 && scv.moving;
        command_target_probe_stage = move_target_issued ? 3 : command_target_probe_stage;
        const bool stopped =
            move_target_issued && click_command(2) &&
            status.last_command_opcode == 30 && !scv.moving;
        command_target_probe_stage = stopped ? 4 : command_target_probe_stage;
        const bool attack_started =
            stopped && click_command(3) && status.command_target_active &&
            status.target_unit_order == 9 && status.target_terrain_order == 15;
        command_target_probe_stage = attack_started ? 5 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool repair_started =
            attack_started && !status.command_target_active && click_command(4) &&
            status.command_target_active && status.target_unit_order == 0x24 &&
            status.target_terrain_order == 7;
        command_target_probe_stage = repair_started ? 6 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool gather_started =
            repair_started && !status.command_target_active && click_command(5) &&
            status.command_target_active && status.target_unit_order == 0x4E &&
            status.target_terrain_order == 7;
        command_target_probe_stage = gather_started ? 7 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool cargo_returned =
            gather_started && !status.command_target_active && click_command(6) &&
            status.last_command_opcode == 34 && scv.moving;
        command_target_probe_stage = cargo_returned ? 8 : command_target_probe_stage;
        command_target_verified = cargo_returned;
      }
    }
    bool building_placement_verified = true;
    if (building_placement_probe) {
      building_placement_verified = false;
      ScenarioUnitPreview* const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        clear_selection(status);
        scv_pointer->selected = true;
        status.active_command_card = 0;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        const std::size_t command_centers_before = static_cast<std::size_t>(std::count_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) { return unit.unit_type == 106; }));
        const bool basic_card_opened =
            click_command(7) && status.active_command_card == 237;
        const bool placement_started =
            basic_card_opened && click_command(1) && status.placement_active &&
            status.placement_unit_type == 106;
        int placement_game_x{};
        int placement_game_y{};
        bool valid_target{};
        if (placement_started) {
          for (int y = 48; y < 250 && !valid_target; y += 16) {
            for (int x = 48; x < 592; x += 16) {
              if (update_building_placement(status, x, y) && status.placement_valid) {
                placement_game_x = x;
                placement_game_y = y;
                valid_target = true;
                break;
              }
            }
          }
        }
        if (valid_target) {
          SendMessageA(
              window,
              WM_LBUTTONDOWN,
              MK_LBUTTON,
              client_point(placement_game_x, placement_game_y));
        }
        const std::size_t command_centers_after = static_cast<std::size_t>(std::count_if(
            status.units.begin(),
            status.units.end(),
            [](const ScenarioUnitPreview& unit) { return unit.unit_type == 106; }));
        building_placement_verified =
            valid_target && !status.placement_active &&
            command_centers_after == command_centers_before + 1U;
      }
    }
    bool construction_verified = true;
    bool construction_status_verified = true;
    bool construction_animation_verified = true;
    bool building_idle_animation_verified = true;
    int construction_probe_stage{};
    if (construction_probe || construction_status_probe) {
      construction_verified = !construction_probe;
      construction_status_verified = !construction_status_probe;
      construction_animation_verified = !construction_probe;
      building_idle_animation_verified = !construction_probe;
      const BuildableUnitVisual* const command_center =
          find_buildable_unit(status, 106U);
      construction_probe_stage = command_center != nullptr ? 1 : 0;
      auto worker = std::find_if(
          status.units.rbegin(),
          status.units.rend(),
          [](const ScenarioUnitPreview& unit) {
            return unit.alive && unit.owner == 0 && unit.unit_type == 7;
          });
      construction_probe_stage =
          worker != status.units.rend() ? 2 : construction_probe_stage;
      if (production_verified && command_center != nullptr &&
          worker != status.units.rend()) {
        construction_probe_stage = 3;
        clear_selection(status);
        worker->selected = true;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        status.placement_active = true;
        status.placement_unit_type = 106U;
        bool site_found{};
        const int half_width = command_center->placement_width / 2;
        const int half_height = command_center->placement_height / 2;
        for (int y = half_height; y + half_height <= status.pathing_map.pixel_height() &&
                                  !site_found;
             y += 32) {
          for (int x = half_width; x + half_width <= status.pathing_map.pixel_width();
               x += 32) {
            status.placement_x = static_cast<std::uint16_t>(x);
            status.placement_y = static_cast<std::uint16_t>(y);
            site_found = placement_is_valid(
                status, *command_center, status.placement_x, status.placement_y);
            if (site_found) {
              break;
            }
          }
        }
        construction_probe_stage = site_found ? 4 : construction_probe_stage;
        status.placement_valid = site_found;
        const bool placed = site_found && place_current_building(status);
        construction_probe_stage = placed ? 5 : construction_probe_stage;
        ScenarioUnitPreview* const created =
            placed && !status.units.empty() ? &status.units.back() : nullptr;
        ScenarioUnitPreview* const builder =
            created == nullptr ? nullptr
                               : find_unit_by_id(status, created->construction_builder_id);
        const bool started =
            created != nullptr && created->unit_type == 106U &&
            !created->construction_complete &&
            created->construction_ticks_total != 0U &&
            created->construction_ticks_remaining == created->construction_ticks_total &&
            created->hit_points == (std::max)(1U, created->max_hit_points / 10U) &&
            builder != nullptr && builder->active_order == ActiveUnitOrder::construct &&
            builder->order_target_id == created->unit_id;
        construction_probe_stage = created != nullptr ? 6 : construction_probe_stage;
        construction_probe_stage = builder != nullptr ? 7 : construction_probe_stage;
        construction_probe_stage = started ? 8 : construction_probe_stage;
        bool advanced{};
        bool incomplete_card_gated{};
        if (started) {
          clear_selection(status);
          created->selected = true;
          incomplete_card_gated = command_card_for(status).count == 0U;
          created->selected = false;
          builder->selected = true;
          builder->x = builder->movement_final_x;
          builder->y = builder->movement_final_y;
          builder->x_fixed = static_cast<std::int32_t>(builder->x) << 8U;
          builder->y_fixed = static_cast<std::int32_t>(builder->y) << 8U;
          stop_unit_movement(status, *builder);
          const std::uint16_t remaining_before =
              created->construction_ticks_remaining;
          const std::uint32_t life_before = created->hit_points;
          SendMessageA(window, WM_TIMER, 1, 0);
          advanced = created->construction_ticks_remaining + 1U == remaining_before &&
                     created->hit_points > life_before &&
                     !created->construction_complete;
          construction_probe_stage = advanced ? 9 : construction_probe_stage;
          if (construction_status_probe) {
            created->construction_ticks_remaining = static_cast<std::uint16_t>((std::max)(
                1U,
                static_cast<unsigned>(created->construction_ticks_total) / 2U));
            const std::uint32_t initial_life =
                (std::max)(1U, created->max_hit_points / 10U);
            const std::uint32_t completed_ticks =
                created->construction_ticks_total -
                created->construction_ticks_remaining;
            created->hit_points = (std::min)(
                created->max_hit_points,
                initial_life + completed_ticks * created->repair_step);
            clear_selection(status);
            created->selected = true;
            construction_status_verified =
                incomplete_card_gated && advanced && !created->construction_complete &&
                created->construction_ticks_remaining != 0U &&
                created->construction_ticks_remaining <
                    created->construction_ticks_total &&
                created->hit_points > initial_life &&
                status.status_construction_label_control.left == 240 &&
                status.status_construction_progress_control.left == 263;
          }
          if (construction_probe) {
            created->construction_animation_phase = 0U;
            created->hit_points = created->max_hit_points / 5U + 1U;
            const bool crossed_one =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 1U;
            const bool played_one =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 2U;
            created->hit_points = 2U * created->max_hit_points / 5U + 1U;
            const bool crossed_two =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 3U;
            const bool played_two =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 4U;
            created->hit_points = 3U * created->max_hit_points / 5U + 1U;
            const bool crossed_three =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 5U;
            const bool played_three =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 6U;
            created->hit_points = 4U * created->max_hit_points / 5U + 1U;
            const bool crossed_four =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 7U;
            construction_animation_verified =
                crossed_one && played_one && crossed_two && played_two &&
                crossed_three && played_three && crossed_four &&
                created->iscript_ready;
            created->construction_ticks_remaining = 1;
            SendMessageA(window, WM_TIMER, 1, 0);
            const std::uint16_t idle_program_before =
                created->iscript_state.program_counter;
            const std::uint8_t idle_sleep_before = created->iscript_state.sleep_ticks;
            SendMessageA(window, WM_TIMER, 1, 0);
            building_idle_animation_verified =
                created->iscript_ready &&
                (created->iscript_state.program_counter != idle_program_before ||
                 created->iscript_state.sleep_ticks != idle_sleep_before);
            clear_selection(status);
            created->selected = true;
            construction_probe_stage =
                created->construction_complete ? 10 : construction_probe_stage;
            construction_probe_stage =
                created->construction_ticks_remaining == 0U
                    ? 11
                    : construction_probe_stage;
            construction_probe_stage =
                created->hit_points == created->max_hit_points
                    ? 12
                    : construction_probe_stage;
            construction_probe_stage =
                created->construction_builder_id == 0U
                    ? 13
                    : construction_probe_stage;
            construction_probe_stage =
                builder->active_order == ActiveUnitOrder::none
                    ? 14
                    : construction_probe_stage;
            construction_probe_stage =
                command_card_for(status).count == kCommandCenterButtons.size()
                    ? 15
                    : construction_probe_stage;
            construction_verified =
                incomplete_card_gated && advanced && construction_animation_verified &&
                building_idle_animation_verified && created->construction_complete &&
                created->construction_ticks_remaining == 0U &&
                created->hit_points == created->max_hit_points &&
                created->construction_builder_id == 0U &&
                builder->active_order == ActiveUnitOrder::none &&
                command_card_for(status).count == kCommandCenterButtons.size();
            construction_probe_stage =
                construction_verified ? 16 : construction_probe_stage;
          }
        }
      }
    }
    bool addon_verified = true;
    if (addon_probe) {
      addon_verified = false;
      ScenarioUnitPreview* parent{};
      for (ScenarioUnitPreview& unit : status.units) {
        if (unit.alive && unit.owner == 0 && unit.unit_type == 106U &&
            unit.construction_complete && unit.attached_addon_id == 0U) {
          parent = &unit;
          break;
        }
      }
      const BuildableUnitVisual* const addon = find_buildable_unit(status, 107U);
      std::uint16_t expected_x{};
      std::uint16_t expected_y{};
      if (parent != nullptr && addon != nullptr && probe_client_ready &&
          addon_center_for_parent(*addon, *parent, expected_x, expected_y)) {
        clear_selection(status);
        parent->selected = true;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        const bool exact_site =
            placement_is_valid(status, *addon, expected_x, expected_y);
        const bool wrong_site_rejected =
            !placement_is_valid(
                status,
                *addon,
                static_cast<std::uint16_t>(expected_x + 32U),
                expected_y);
        const std::size_t units_before = status.units.size();
        const bool command_accepted = exact_site && wrong_site_rejected && click_command(7);
        ScenarioUnitPreview* const created =
            command_accepted && status.units.size() == units_before + 1U
                ? &status.units.back()
                : nullptr;
        const bool linked =
            created != nullptr && created->unit_type == 107U &&
            created->addon_parent_id == parent->unit_id &&
            parent->attached_addon_id == created->unit_id &&
            created->x == expected_x && created->y == expected_y &&
            !created->construction_complete &&
            created->hit_points == (std::max)(1U, created->max_hit_points / 10U);
        if (linked) {
          created->construction_ticks_remaining = 1;
          SendMessageA(window, WM_TIMER, 1, 0);
          const std::size_t units_after = status.units.size();
          (void)click_command(8);
          addon_verified =
              created->construction_complete &&
              created->hit_points == created->max_hit_points &&
              parent->attached_addon_id == created->unit_id &&
              status.units.size() == units_after;
        }
      }
    }
    bool creep_verified = true;
    int creep_probe_stage{};
    if (creep_probe) {
      creep_verified = false;
      BuildableUnitVisual requires_creep{};
      requires_creep.unit_type = 143U;
      requires_creep.placement_width = 32;
      requires_creep.placement_height = 32;
      requires_creep.simulation.dat_flags = 0x20001U;
      std::uint16_t target_x{};
      std::uint16_t target_y{};
      std::uint16_t source_x{};
      std::uint16_t source_y{};
      bool site_found{};
      const auto unoccupied = [&](const int x, const int y) {
        ScenarioUnitPreview footprint{};
        footprint.selection_width = 32;
        footprint.selection_height = 32;
        return std::none_of(
            status.units.begin(),
            status.units.end(),
            [&footprint, x, y](const ScenarioUnitPreview& unit) {
              return unit.alive && unit_footprints_overlap_at(footprint, x, y, unit);
            });
      };
      for (int y = 16; y < status.pathing_map.pixel_height() - 16 && !site_found;
           y += 32) {
        for (int x = 16; x + 112 < status.pathing_map.pixel_width(); x += 32) {
          std::uint16_t target_tile{};
          std::uint16_t source_tile{};
          if (status.scenario.tile(
                  static_cast<std::uint16_t>(x / 32),
                  static_cast<std::uint16_t>(y / 32),
                  target_tile) &&
              status.scenario.tile(
                  static_cast<std::uint16_t>((x + 96) / 32),
                  static_cast<std::uint16_t>(y / 32),
                  source_tile) &&
              status.terrain_tileset.buildable(target_tile) &&
              status.terrain_tileset.buildable(source_tile) && unoccupied(x, y) &&
              unoccupied(x + 96, y)) {
            target_x = static_cast<std::uint16_t>(x);
            target_y = static_cast<std::uint16_t>(y);
            source_x = static_cast<std::uint16_t>(x + 96);
            source_y = target_y;
            site_found = true;
            break;
          }
        }
      }
      if (site_found) {
        creep_probe_stage = 1;
        (void)rebuild_creep_tiles(status);
        const std::vector<std::uint8_t> creep_before = status.creep_tiles;
        const bool rejected_without_creep =
            !placement_is_valid(status, requires_creep, target_x, target_y);
        creep_probe_stage = rejected_without_creep ? 2 : creep_probe_stage;
        ScenarioUnitPreview source{};
        source.unit_id = status.next_unit_id++;
        source.x = source_x;
        source.y = source_y;
        source.x_fixed = static_cast<std::int32_t>(source.x) << 8U;
        source.y_fixed = static_cast<std::int32_t>(source.y) << 8U;
        source.unit_type = 131U;
        source.owner = 1;
        source.selection_width = 32;
        source.selection_height = 32;
        source.is_building = true;
        const std::uint32_t source_id = source.unit_id;
        status.units.push_back(std::move(source));
        const bool spread = rebuild_creep_tiles(status) &&
                            placement_is_valid(
                                status, requires_creep, target_x, target_y);
        creep_probe_stage = spread ? 3 : creep_probe_stage;
        bool exact_ellipse = spread &&
                             creep_before.size() == status.creep_tiles.size() &&
                             status.creep_visual_tiles.size() ==
                                 status.creep_tiles.size();
        for (int tile_y = 0;
             exact_ellipse && tile_y < status.scenario_height;
             ++tile_y) {
          const int dy = 32 * tile_y - static_cast<int>(source_y) + 16;
          for (int tile_x = 0; tile_x < status.scenario_width; ++tile_x) {
            const int dx = 32 * tile_x - static_cast<int>(source_x) + 16;
            const std::size_t tile =
                static_cast<std::size_t>(tile_y) * status.scenario_width + tile_x;
            const bool in_recovered_ellipse =
                256LL * dy * dy + 100LL * dx * dx <= 10240000LL;
            const bool expected = creep_before[tile] != 0U || in_recovered_ellipse;
            if ((status.creep_tiles[tile] != 0U) != expected ||
                (expected && (status.creep_visual_tiles[tile] < 16U ||
                              status.creep_visual_tiles[tile] > 24U))) {
              exact_ellipse = false;
              break;
            }
          }
        }
        creep_probe_stage = exact_ellipse ? 4 : creep_probe_stage;
        ScenarioUnitPreview* const inserted = find_unit_by_id(status, source_id);
        if (inserted != nullptr) {
          inserted->alive = false;
        }
        const bool receded = rebuild_creep_tiles(status) &&
                             !placement_is_valid(
                                 status, requires_creep, target_x, target_y) &&
                             status.creep_tiles == creep_before;
        creep_probe_stage = receded ? 5 : creep_probe_stage;
        creep_verified = rejected_without_creep && spread && exact_ellipse &&
                         receded && status.terrain_ready && status.minimap_ready;
        creep_probe_stage = creep_verified ? 6 : creep_probe_stage;
      }
    }
    bool melee_start_verified = true;
    if (melee_start_probe) {
      melee_start_verified = status.local_race < 3U;
      for (std::size_t player = 0;
           melee_start_verified && player < 8U;
           ++player) {
        if (!status.active_players[player]) {
          continue;
        }
        const std::uint8_t race = status.scenario.players()[player].race;
        const starcraft::game::ScenarioStartLocation& start =
            status.scenario.start_locations()[player];
        if (race >= 3U || !start.present) {
          melee_start_verified = false;
          break;
        }
        starcraft::lang::MeleeUnitTypes unit_types{};
        if (!starcraft::lang::melee_unit_types(race, unit_types)) {
          melee_start_verified = false;
          break;
        }
        const std::uint16_t worker_type = unit_types.worker;
        const std::uint16_t base_type = unit_types.base;
        std::size_t workers{};
        const ScenarioUnitPreview* base{};
        for (const ScenarioUnitPreview& unit : status.units) {
          if (!unit.alive || unit.owner != player) {
            continue;
          }
          if (unit.unit_type == worker_type) {
            ++workers;
            melee_start_verified =
                melee_start_verified &&
                std::abs(static_cast<int>(unit.x) - start.x) <= 128 &&
                std::abs(static_cast<int>(unit.y) - start.y) <= 128 &&
                !(unit.x == start.x && unit.y == start.y);
          } else if (unit.unit_type == base_type && base == nullptr) {
            base = &unit;
          }
        }
        if (base == nullptr) {
          melee_start_verified = false;
          break;
        }
        const starcraft::lang::PlacementPoint expected_base =
            starcraft::lang::align_melee_base(
                start.x,
                start.y,
                base->selection_width,
                base->selection_height);
        melee_start_verified =
            workers == starcraft::lang::retail_melee_starting_worker_count &&
            base->x == expected_base.x && base->y == expected_base.y;
        for (std::size_t left = 0;
             melee_start_verified && left < status.units.size();
             ++left) {
          const ScenarioUnitPreview& worker = status.units[left];
          if (!worker.alive || worker.owner != player ||
              worker.unit_type != worker_type) {
            continue;
          }
          for (std::size_t right = 0; right < status.units.size(); ++right) {
            const ScenarioUnitPreview& other = status.units[right];
            if (left == right || !other.alive || other.owner != player ||
                (other.unit_type != worker_type && other.unit_type != base_type)) {
              continue;
            }
            if (unit_footprints_overlap_at(worker, worker.x, worker.y, other)) {
              melee_start_verified = false;
              break;
            }
          }
        }
      }
      const std::array<std::uint32_t, 2> supply = local_supply(status);
      melee_start_verified = melee_start_verified && supply[0] == 8U &&
                             supply[1] ==
                                 starcraft::lang::retail_melee_base_supply_internal;
    }
    const bool rendered = status.assets_ready && render_opengl(window, window_state);
    const bool captured = capture_path == nullptr ||
                          (rendered && capture_opengl_bmp(window, window_state, capture_path));
    DestroyWindow(window);
    if (command_target_probe && !command_target_verified) {
      return 20 + command_target_probe_stage;
    }
    if (creep_probe && !creep_verified) {
      return 120 + creep_probe_stage;
    }
    if (refinery_placement_probe && !refinery_placement_verified) {
      return 40 + refinery_probe_stage;
    }
    if (construction_probe && !construction_verified) {
      return 60 + construction_probe_stage;
    }
    if (resource_strip_probe && !resource_strip_verified) {
      return 80 + resource_strip_probe_stage;
    }
    if (unit_avoidance_probe && !unit_avoidance_verified) {
      return 160 + unit_avoidance_probe_stage;
    }
    if (harvest_queue_probe && !harvest_queue_verified) {
      return 180 + harvest_queue_probe_stage;
    }
    if (geyser_probe && !geysers_verified) {
      return 190 + geyser_probe_stage;
    }
    if (all_production_probe && !all_production_verified) {
      return 200 + all_production_probe_stage;
    }
    if (movement_probe && !movement_verified) {
      return 220 + movement_probe_stage;
    }
    return rendered && captured && selection_verified && command_panel_verified &&
                   production_verified && movement_verified && pathfinding_verified &&
                   portrait_verified && worker_actions_verified &&
                   harvest_visual_verified && building_working_verified &&
                   smart_orders_verified &&
                   refinery_placement_verified &&
                   command_target_verified && building_placement_verified &&
                   construction_verified && addon_verified && creep_verified &&
                   melee_start_verified &&
                   construction_animation_verified && building_idle_animation_verified &&
                   unit_avoidance_verified && minimap_verified && camera_verified
                   && status_panel_verified && multi_status_verified
                   && construction_status_verified && resource_feedback_verified &&
                   resource_strip_verified && unit_audio_verified && music_verified &&
                   harvest_queue_verified && geysers_verified &&
                   all_production_verified
               ? 0
               : 11;
  }

  ShowWindow(window, show_command);
  UpdateWindow(window);
  SetTimer(window, 1, kSimulationTickMilliseconds, nullptr);

  MSG message{};
  while (GetMessageA(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageA(&message);
  }
  return static_cast<int>(message.wParam);
}
