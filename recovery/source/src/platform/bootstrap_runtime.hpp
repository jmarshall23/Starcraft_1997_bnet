#pragma once

#include "starcraft/data/dat.hpp"
#include "starcraft/game/scenario.hpp"
#include "starcraft/gds/tileset.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_init.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/lang/pathfinding.hpp"
#include "starcraft/runtime/storm.hpp"

#include "smacker.h"

#include <AL/al.h>
#include <AL/alc.h>
// Windows' OpenGL header depends on Win32 declarations from windows.h.
// clang-format off
#include <windows.h>
#include <gl/GL.h>
// clang-format on

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace starcraft::recovery {

inline constexpr std::uint16_t kMapViewportWidth = 640;
inline constexpr std::uint16_t kMapViewportHeight = 400;
inline constexpr std::uint32_t kSimulationTickMilliseconds = 42;
inline constexpr GLenum kGlBgra = 0x80E1;

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
  SmackerPortrait(const SmackerPortrait &) = delete;
  SmackerPortrait &operator=(const SmackerPortrait &) = delete;
  SmackerPortrait(SmackerPortrait &&other) noexcept
      : decoder(std::exchange(other.decoder, nullptr)),
        frame(std::move(other.frame)), path(std::move(other.path)),
        frame_count(other.frame_count), last_frame_tick(other.last_frame_tick),
        frame_delay_ms(other.frame_delay_ms), ready(other.ready) {
    other.ready = false;
  }
  SmackerPortrait &operator=(SmackerPortrait &&other) noexcept {
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
  const CommandButtonVisual *buttons{};
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

struct PcmWaveView {
  const std::uint8_t *samples{};
  ALsizei sample_bytes{};
  ALsizei sample_rate{};
  ALenum format{};
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
  std::array<UnitSoundRanges, starcraft::lang::kUnitTypeCount>
      unit_sound_ranges{};
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
  std::array<RuntimeUnitType, starcraft::lang::kUnitTypeCount>
      runtime_unit_types{};
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
  BootstrapStatus *status{};
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
  ALCdevice *audio_device{};
  ALCcontext *audio_context{};
  ALuint audio_source{};
  std::array<ALuint, 2> resource_error_buffers{};
  std::vector<ALuint> archived_sound_buffers{};
  ALuint music_source{};
  ALuint music_buffer{};
  std::uint32_t audio_play_count{};
  bool audio_ready{};
  bool music_playing{};
};

// Shared recovered runtime services. Implementations live in source files
// named after the matching Microsoft compilation units; bootstrap_win32.cpp
// only coordinates them.
[[nodiscard]] std::uint16_t read_u16(const std::vector<std::uint8_t> &bytes,
                                     std::size_t offset) noexcept;
[[nodiscard]] std::uint32_t read_u32(const std::vector<std::uint8_t> &bytes,
                                     std::size_t offset) noexcept;
[[nodiscard]] bool parse_pcm_wave(const std::vector<std::uint8_t> &bytes,
                                  PcmWaveView &wave) noexcept;
[[nodiscard]] bool
parse_command_controls(const std::vector<std::uint8_t> &layout,
                       std::array<CommandControl, 9> &controls) noexcept;
[[nodiscard]] bool parse_dialog_control(const std::vector<std::uint8_t> &layout,
                                        std::int16_t identifier,
                                        CommandControl &output) noexcept;
[[nodiscard]] bool
parse_status_panel_controls(const std::vector<std::uint8_t> &layout,
                            BootstrapStatus &status) noexcept;
[[nodiscard]] bool
parse_resource_panel_controls(const std::vector<std::uint8_t> &layout,
                              BootstrapStatus &status) noexcept;
[[nodiscard]] bool
decode_preview_frames(const std::vector<std::uint8_t> &group,
                      const std::vector<std::uint8_t> &palette,
                      std::vector<SpritePreviewFrame> &frames,
                      std::uint16_t &canvas_width, std::uint16_t &canvas_height,
                      const std::vector<std::uint8_t> *index_remap = nullptr);
[[nodiscard]] bool decode_pcx_frame(const starcraft::runtime::DecodedPcx &image,
                                    bool transparent_zero,
                                    SpritePreviewFrame &frame);
[[nodiscard]] bool decode_smacker_frame(SmackerPortrait &portrait) noexcept;
[[nodiscard]] bool load_unit_portrait(starcraft::runtime::StormModule &storm,
                                      const starcraft::data::CoreDataSet &data,
                                      std::uint16_t unit_type,
                                      std::uint8_t owner,
                                      SmackerPortrait &output) noexcept;
[[nodiscard]] bool
apply_preview_draw_function(std::uint8_t draw_function,
                            std::vector<SpritePreviewFrame> &frames);
[[nodiscard]] bool load_image_preview(starcraft::runtime::StormModule &storm,
                                      const starcraft::data::CoreDataSet &data,
                                      std::uint16_t image_id,
                                      const std::vector<std::uint8_t> &palette,
                                      std::string &path,
                                      std::vector<SpritePreviewFrame> &frames,
                                      std::uint16_t &canvas_width,
                                      std::uint16_t &canvas_height);
[[nodiscard]] bool load_unit_render_asset(
    starcraft::runtime::StormModule &storm,
    const starcraft::data::CoreDataSet &data,
    const std::vector<std::uint8_t> &palette,
    const std::vector<std::uint8_t> &iscript_bytes, std::uint16_t image_id,
    std::uint16_t tileset_frame_offset, UnitRenderAsset &output);
void draw_preview_frame_gl(const SpritePreviewFrame &frame, float x, float y,
                           float width, float height,
                           const std::uint32_t *override_bgra = nullptr,
                           bool mirrored = false);
void draw_preview_frame_fraction_gl(const SpritePreviewFrame &frame, float x,
                                    float y, float width, float height,
                                    float fraction);
void draw_team_colored_frame_gl(const BootstrapStatus &status,
                                const SpritePreviewFrame &frame,
                                std::uint8_t owner, float x, float y,
                                float width, float height,
                                bool mirrored = false);
void draw_scenario_unit_gl(const BootstrapStatus &status,
                           const ScenarioUnitPreview &unit);
void draw_building_placement_gl(const BootstrapStatus &status);

[[nodiscard]] constexpr float hud_vertical_scale() noexcept {
  return 400.0F / 480.0F;
}
[[nodiscard]] std::string_view status_text(const BootstrapStatus &status,
                                           std::uint16_t one_based_id) noexcept;
[[nodiscard]] std::string printable_status_text(std::string_view source);
void draw_status_text_gl(const RecoveryWindowState &state,
                         const CommandControl &control, std::string_view text,
                         std::uint8_t red = 220U, std::uint8_t green = 220U,
                         std::uint8_t blue = 220U);
void draw_system_message_gl(const RecoveryWindowState &state,
                            std::uint32_t now);
void advance_resource_display(BootstrapStatus &status) noexcept;
[[nodiscard]] std::array<std::uint32_t, 2>
local_supply(const BootstrapStatus &status) noexcept;
void draw_resource_strip_gl(const RecoveryWindowState &state);
[[nodiscard]] std::vector<std::uint32_t>
translated_wireframe(const BootstrapStatus &status,
                     const SpritePreviewFrame &frame,
                     const ScenarioUnitPreview &unit);
void draw_wireframe_in_control_gl(const BootstrapStatus &status,
                                  const ScenarioUnitPreview &unit,
                                  const SpritePreviewFrame &frame,
                                  const CommandControl &control);
void draw_selected_status_panel_gl(const RecoveryWindowState &state,
                                   std::uint32_t now);
[[nodiscard]] const UnitPortraitAsset *
selected_portrait(const BootstrapStatus &status) noexcept;
[[nodiscard]] UnitPortraitAsset *
selected_portrait(BootstrapStatus &status) noexcept;
void draw_selected_portrait_gl(const BootstrapStatus &status);
[[nodiscard]] bool advance_selected_portrait(BootstrapStatus &status,
                                             std::uint32_t now) noexcept;
void draw_minimap_gl(const BootstrapStatus &status);
void draw_hud_control_frame_gl(const SpritePreviewFrame &frame,
                               const CommandControl &control);
void draw_selected_command_panel_gl(const RecoveryWindowState &state);
void apply_simulation_traits(
    ScenarioUnitPreview &unit,
    const starcraft::data::UnitSimulationTraits &traits) noexcept;
void apply_initialization_traits(
    ScenarioUnitPreview &unit,
    const starcraft::lang::UnitInitializationData &initialization) noexcept;
[[nodiscard]] const ScenarioUnitPreview *
first_selected_unit(const BootstrapStatus &status) noexcept;
[[nodiscard]] CommandCardView
command_card_for(const BootstrapStatus &status) noexcept;
[[nodiscard]] const BuildableUnitVisual *
find_buildable_unit(const BootstrapStatus &status,
                    std::uint16_t unit_type) noexcept;
[[nodiscard]] bool build_terrain_preview(
    const starcraft::gds::TilesetData &tileset,
    const starcraft::game::MultiplayerScenario &scenario,
    std::uint16_t camera_x, std::uint16_t camera_y, SpritePreviewFrame &output,
    const std::vector<std::uint8_t> *creep_tiles = nullptr,
    const std::vector<std::uint8_t> *creep_visual_tiles = nullptr);
[[nodiscard]] bool build_minimap_preview(
    const starcraft::gds::TilesetData &tileset,
    const starcraft::game::MultiplayerScenario &scenario,
    SpritePreviewFrame &output, std::uint16_t &content_x,
    std::uint16_t &content_y, std::uint16_t &content_width,
    std::uint16_t &content_height,
    const std::vector<std::uint8_t> *creep_tiles = nullptr,
    const std::vector<std::uint8_t> *creep_visual_tiles = nullptr) noexcept;
[[nodiscard]] bool
start_image_script(const std::vector<std::uint8_t> &bytes,
                   std::uint16_t script_id,
                   starcraft::lang::IScriptState &state,
                   const starcraft::lang::IScriptState *parent = nullptr,
                   std::uint16_t tileset_frame_offset = 0U);
void post_resource_error(BootstrapStatus &status, bool gas) noexcept;
[[nodiscard]] bool resource_cost_available(BootstrapStatus &status,
                                           std::uint32_t mineral_cost,
                                           std::uint32_t gas_cost) noexcept;

[[nodiscard]] bool restart_unit_animation(const BootstrapStatus &status,
                                          ScenarioUnitPreview &unit,
                                          std::uint8_t animation) noexcept;
[[nodiscard]] bool
advance_building_construction_animation(const BootstrapStatus &status,
                                        ScenarioUnitPreview &building) noexcept;
[[nodiscard]] bool collect_building_obstacles(
    const BootstrapStatus &status, const ScenarioUnitPreview *ignored_unit,
    std::vector<starcraft::lang::PathObstacle> &output) noexcept;
[[nodiscard]] bool
unit_footprints_overlap_at(const ScenarioUnitPreview &mover, int mover_x,
                           int mover_y,
                           const ScenarioUnitPreview &obstacle) noexcept;
[[nodiscard]] bool
unit_rectangles_overlap_at(const ScenarioUnitPreview &mover, int mover_x,
                           int mover_y,
                           const ScenarioUnitPreview &obstacle) noexcept;
[[nodiscard]] const ScenarioUnitPreview *
find_live_unit_collision(const BootstrapStatus &status,
                         const ScenarioUnitPreview &mover, int proposed_x,
                         int proposed_y) noexcept;
[[nodiscard]] const ScenarioUnitPreview *
find_live_unit_footprint_collision(const BootstrapStatus &status,
                                   const ScenarioUnitPreview &mover,
                                   int proposed_x, int proposed_y) noexcept;
[[nodiscard]] bool creation_position_passable(const BootstrapStatus &status,
                                              const ScenarioUnitPreview &unit,
                                              int x, int y) noexcept;
[[nodiscard]] bool settle_created_unit(BootstrapStatus &status,
                                       ScenarioUnitPreview &unit,
                                       std::uint16_t requested_x,
                                       std::uint16_t requested_y) noexcept;
[[nodiscard]] bool
settle_melee_starting_workers(BootstrapStatus &status) noexcept;
[[nodiscard]] bool plan_scv_path(BootstrapStatus &status,
                                 ScenarioUnitPreview &unit,
                                 std::uint16_t target_x,
                                 std::uint16_t target_y) noexcept;
void stop_unit_movement(const BootstrapStatus &status,
                        ScenarioUnitPreview &unit) noexcept;
void cancel_unit_order(BootstrapStatus &status,
                       ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] std::size_t issue_scv_move_order(BootstrapStatus &status,
                                               std::uint16_t target_x,
                                               std::uint16_t target_y) noexcept;
[[nodiscard]] std::uint32_t
effective_unit_top_speed(const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] ScenarioUnitPreview *
find_unit_by_id(BootstrapStatus &status, std::uint32_t unit_id) noexcept;
[[nodiscard]] const ScenarioUnitPreview *
find_unit_by_id(const BootstrapStatus &status, std::uint32_t unit_id) noexcept;
[[nodiscard]] int unit_edge_distance(const ScenarioUnitPreview &left,
                                     const ScenarioUnitPreview &right) noexcept;
[[nodiscard]] bool
plan_scv_interaction_path(BootstrapStatus &status, ScenarioUnitPreview &worker,
                          const ScenarioUnitPreview &target) noexcept;
[[nodiscard]] bool begin_scv_interaction(BootstrapStatus &status,
                                         ScenarioUnitPreview &worker,
                                         const ScenarioUnitPreview &target,
                                         ActiveUnitOrder order) noexcept;
void cancel_command_target(BootstrapStatus &status) noexcept;
void begin_command_target(BootstrapStatus &status, std::uint8_t unit_order,
                          std::uint8_t terrain_order) noexcept;
[[nodiscard]] std::size_t
issue_active_scv_target(BootstrapStatus &status, std::uint16_t world_x,
                        std::uint16_t world_y) noexcept;
[[nodiscard]] std::size_t issue_scv_smart_order(BootstrapStatus &status,
                                                std::uint16_t world_x,
                                                std::uint16_t world_y) noexcept;
[[nodiscard]] std::size_t
issue_scv_return_cargo(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_unit_movement(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_unit_actions(BootstrapStatus &status) noexcept;
[[nodiscard]] bool rebuild_creep_tiles(BootstrapStatus &status) noexcept;
[[nodiscard]] bool tile_has_creep(const BootstrapStatus &status, int tile_x,
                                  int tile_y) noexcept;
[[nodiscard]] bool advance_addon_construction(BootstrapStatus &status) noexcept;
[[nodiscard]] bool addon_center_for_parent(const BuildableUnitVisual &addon,
                                           const ScenarioUnitPreview &parent,
                                           std::uint16_t &center_x,
                                           std::uint16_t &center_y) noexcept;
[[nodiscard]] bool placement_is_valid(const BootstrapStatus &status,
                                      const BuildableUnitVisual &buildable,
                                      std::uint16_t center_x,
                                      std::uint16_t center_y) noexcept;
[[nodiscard]] bool update_building_placement(BootstrapStatus &status,
                                             int game_x, int game_y) noexcept;
[[nodiscard]] bool place_current_building(BootstrapStatus &status) noexcept;
[[nodiscard]] bool configure_preview_type(BootstrapStatus &status,
                                          ScenarioUnitPreview &unit,
                                          std::uint16_t unit_type) noexcept;
void activate_command_button(BootstrapStatus &status,
                             std::uint16_t position) noexcept;
[[nodiscard]] bool advance_unit_production(BootstrapStatus &status,
                                           std::uint32_t now) noexcept;
[[nodiscard]] bool advance_zerg_larvae(BootstrapStatus &status) noexcept;
void shutdown_audio(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool initialize_audio(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool
play_pending_resource_error(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool play_pending_game_sound(RecoveryWindowState &state) noexcept;
[[nodiscard]] std::uint16_t choose_unit_sound(BootstrapStatus &status,
                                              std::uint16_t first,
                                              std::uint16_t last) noexcept;
[[nodiscard]] bool queue_unit_response(BootstrapStatus &status,
                                       const ScenarioUnitPreview &unit,
                                       bool order_acknowledgement) noexcept;
void clear_selection(BootstrapStatus &status) noexcept;
[[nodiscard]] std::size_t
selection_count(const BootstrapStatus &status) noexcept;
void complete_selection_drag(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool set_camera_position(BootstrapStatus &status, int requested_x,
                                       int requested_y) noexcept;
[[nodiscard]] bool camera_input_key(WPARAM key) noexcept;
[[nodiscard]] bool advance_camera_scroll(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool center_camera_from_minimap(BootstrapStatus &status,
                                              int game_x, int game_y) noexcept;
[[nodiscard]] std::uint16_t command_position_at(const BootstrapStatus &status,
                                                int game_x,
                                                int game_y) noexcept;
[[nodiscard]] std::size_t
status_selection_unit_at(const BootstrapStatus &status, int game_x,
                         int game_y) noexcept;
[[nodiscard]] std::filesystem::path executable_directory();
[[nodiscard]] std::filesystem::path locate_input_root();
[[nodiscard]] bool read_loose_file(const std::filesystem::path &path,
                                   std::vector<std::uint8_t> &output) noexcept;
[[nodiscard]] bool
extract_unit_sound_ranges(const starcraft::data::CoreDataSet &data,
                          BootstrapStatus &status) noexcept;
[[nodiscard]] bool cache_unit_sound_assets(
    starcraft::runtime::StormModule &storm,
    const std::vector<std::uint8_t> &sfx_data,
    const std::vector<std::uint8_t> &sfx_table,
    const std::array<bool, starcraft::lang::kUnitTypeCount> &wanted_types,
    BootstrapStatus &status) noexcept;
[[nodiscard]] BootstrapStatus probe_assets();
[[nodiscard]] bool initialize_opengl(HWND window,
                                     RecoveryWindowState &state) noexcept;
void shutdown_opengl(HWND window, RecoveryWindowState &state) noexcept;
void draw_selection_drag_gl(const RecoveryWindowState &state) noexcept;
void draw_command_target_gl(const RecoveryWindowState &state) noexcept;
[[nodiscard]] bool render_opengl(HWND window,
                                 RecoveryWindowState &state) noexcept;
[[nodiscard]] bool capture_opengl_bmp(HWND window, RecoveryWindowState &state,
                                      const char *output_path) noexcept;
[[nodiscard]] bool client_to_game(HWND window, LPARAM lparam, int &game_x,
                                  int &game_y) noexcept;
[[nodiscard]] bool hud_pixel_opaque(const BootstrapStatus &status, int game_x,
                                    int game_y) noexcept;
LRESULT CALLBACK recovery_window_proc(HWND window, UINT message, WPARAM wparam,
                                      LPARAM lparam);
[[nodiscard]] int run_bootstrap_probes(const char *command_line, HWND window,
                                       RecoveryWindowState &window_state,
                                       BootstrapStatus &status, bool &handled);

} // namespace starcraft::recovery
