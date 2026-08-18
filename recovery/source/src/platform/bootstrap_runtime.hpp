#pragma once

#include "starcraft/data/dat.hpp"
#include "starcraft/game/scenario.hpp"
#include "starcraft/gds/font.hpp"
#include "starcraft/gds/tileset.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/CAI.h"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_init.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/lang/pathfinding.hpp"
#include "starcraft/runtime/storm.hpp"

#include "../snp/battle/Battle.hpp"

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
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace starcraft::recovery {

inline constexpr std::uint16_t kMapViewportWidth = 640;
inline constexpr std::uint16_t kMapViewportHeight = 400;
inline constexpr std::uint32_t kSimulationTickMilliseconds = 42;
inline constexpr std::size_t kDigitalSoundSourceCount = 128U;
inline constexpr GLenum kGlBgra = 0x80E1;

struct PresentationViewport {
  int x{};
  int y{};
  int width{};
  int height{};
};

struct SpritePreviewFrame {
  std::uint8_t x_offset{};
  std::uint8_t y_offset{};
  std::uint16_t width{};
  std::uint16_t height{};
  std::vector<std::uint32_t> bgra{};
  std::vector<std::uint8_t> palette_indices{};
  std::vector<std::uint8_t> opacity{};
};

// gluBN.cpp maps Battle.snp's artwork identifiers to these archive PCX files.
// Keep the decoded ownership with GlueRuntime so all Battle.net dialogs share
// the one retail palette/art set exactly as BattleNetDialogProc did.
struct BattleArtwork {
  SpritePreviewFrame connect_background{};
  SpritePreviewFrame logon_background{};
  SpritePreviewFrame shell_background{};
  SpritePreviewFrame channel_background{};
  SpritePreviewFrame join_background{};
  SpritePreviewFrame chat_background{};
  SpritePreviewFrame ladder_background{};
  SpritePreviewFrame create_background{};
  SpritePreviewFrame browse_background{};
  SpritePreviewFrame profile_background{};
  SpritePreviewFrame welcome_ad{};
  SpritePreviewFrame small_popup{};
  SpritePreviewFrame large_popup{};
  std::array<SpritePreviewFrame, 5> battle_buttons{};
  std::array<SpritePreviewFrame, 5> extra_small_buttons{};
  std::array<SpritePreviewFrame, 5> small_buttons{};
  std::array<SpritePreviewFrame, 5> medium_buttons{};
  std::array<SpritePreviewFrame, 5> browse_buttons{};
  bool ready{};
};

// playflic.cpp owns the decoder lifetime used by both glue-control movies and
// the unit portrait surface.  The retail BIN files point at SMK descriptors;
// keeping the decoder with the decoded frame mirrors that ownership without
// leaving video state in the Win32 bootstrapper.
struct SmackerAnimation {
  smk decoder{};
  std::vector<std::uint8_t> encoded{};
  SpritePreviewFrame frame{};
  std::string path{};
  std::uint32_t frame_count{};
  std::uint32_t last_frame_tick{};
  std::uint32_t frame_delay_ms{1};
  bool transparent_zero{};
  bool ready{};

  SmackerAnimation() = default;
  ~SmackerAnimation() {
    if (decoder != nullptr) {
      smk_close(decoder);
    }
  }
  SmackerAnimation(const SmackerAnimation &) = delete;
  SmackerAnimation &operator=(const SmackerAnimation &) = delete;
  SmackerAnimation(SmackerAnimation &&other) noexcept
      : decoder(std::exchange(other.decoder, nullptr)),
        encoded(std::move(other.encoded)), frame(std::move(other.frame)),
        path(std::move(other.path)),
        frame_count(other.frame_count), last_frame_tick(other.last_frame_tick),
        frame_delay_ms(other.frame_delay_ms),
        transparent_zero(other.transparent_zero), ready(other.ready) {
    other.ready = false;
  }
  SmackerAnimation &operator=(SmackerAnimation &&other) noexcept {
    if (this != &other) {
      if (decoder != nullptr) {
        smk_close(decoder);
      }
      decoder = std::exchange(other.decoder, nullptr);
      encoded = std::move(other.encoded);
      frame = std::move(other.frame);
      path = std::move(other.path);
      frame_count = other.frame_count;
      last_frame_tick = other.last_frame_tick;
      frame_delay_ms = other.frame_delay_ms;
      transparent_zero = other.transparent_zero;
      ready = other.ready;
      other.ready = false;
    }
    return *this;
  }
};

enum class GlueScreen : std::uint8_t {
  title,
  main_menu,
  connection,
  map_selection,
  lobby,
  ready,
  battle_net,
  gameplay,
};

enum class GlueAction : std::uint8_t {
  none,
  redraw,
  start_game,
  quit,
};

struct GlueControl {
  std::int16_t identifier{};
  std::uint16_t type{};
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
  std::uint32_t flags{};
  std::uint32_t visual_offset{};
  std::string text{};
};

struct GlueVideo {
  std::int16_t control_identifier{};
  std::uint32_t descriptor_flags{};
  std::int16_t x_offset{};
  std::int16_t y_offset{};
  SmackerAnimation animation{};
};

struct GlueTransformControl {
  std::int16_t identifier{};
  std::int16_t direction{};
};

enum class GlueTransformPhase : std::uint8_t { none, entering, leaving };

struct GlueImage {
  std::int16_t control_identifier{};
  SpritePreviewFrame frame{};
};

struct GlueMapEntry {
  std::string path{};
  std::string name{};
  std::uint16_t width{};
  std::uint16_t height{};
  std::uint16_t tileset{};
  std::size_t player_count{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> races{};
};

struct GlueLobbySlot {
  std::string name{};
  std::uint8_t ownership{};
  std::uint8_t race{};
  bool local{};
  bool open{};
  bool network_player{};
  bool network_configurable{};
};

enum class GlueFontStyle : std::uint8_t {
  gold = 0,
  green = 1,
  bright_green = 2,
  disabled = 3,
  normal = 4,
  error = 5,
};

struct GlueRuntime {
  GlueScreen screen{GlueScreen::gameplay};
  SpritePreviewFrame title_background{};
  SpritePreviewFrame main_background{};
  SpritePreviewFrame connection_background{};
  std::vector<GlueControl> title_controls{};
  std::vector<GlueControl> main_controls{};
  std::vector<GlueControl> ok_popup_controls{};
  std::vector<GlueControl> connection_controls{};
  std::vector<GlueControl> lobby_controls{};
  std::vector<GlueImage> main_images{};
  std::vector<GlueImage> connection_images{};
  std::vector<GlueImage> lobby_images{};
  std::vector<GlueVideo> main_videos{};
  SpritePreviewFrame ok_popup_background{};
  std::vector<SpritePreviewFrame> main_dialog_frames{};
  std::vector<SpritePreviewFrame> network_dialog_frames{};
  starcraft::gds::BitmapFont small_font{};
  starcraft::gds::BitmapFont large_font{};
  std::array<std::array<std::uint32_t, 8>, 6> title_font_colors{};
  std::array<std::array<std::uint32_t, 8>, 6> main_font_colors{};
  std::array<std::array<std::uint32_t, 8>, 6> network_font_colors{};
  BattleArtwork battle_artwork{};
  std::vector<GlueMapEntry> maps{};
  std::array<GlueLobbySlot, starcraft::data::chk_player_slot_count>
      lobby_slots{};
  std::array<std::string, 3> providers{{
      "Battle.net",
      "Local Area Network",
      "Offline Skirmish",
  }};
  std::array<std::string, 3> provider_descriptions{{
      "Online login, chat, and matchmaking will use the new service.",
      "Local-network transport will use the new session protocol.",
      "Create a local game with computer-controlled player slots.",
  }};
  std::string message{};
  std::string modal_message{};
  std::string failure{};
  std::uint32_t screen_entered_tick{};
  std::uint32_t selected_map_changed_tick{};
  std::uint32_t message_until{};
  std::uint32_t ready_deadline{};
  std::uint32_t clock_tick{};
  std::uint32_t transform_started_tick{};
  std::int16_t hovered_control{-1};
  std::int16_t pressed_control{-1};
  std::int16_t popup_control{-1};
  std::int16_t popup_row{-1};
  std::size_t selected_provider{};
  std::size_t selected_map{};
  GlueTransformPhase transform_phase{GlueTransformPhase::none};
  GlueScreen transform_target{GlueScreen::gameplay};
  GlueAction transform_action{GlueAction::none};
  std::vector<GlueTransformControl> transform_controls{};
  battle::BattleRuntime battle_net{};
  bool online_lobby{};
  bool modal_popup_visible{};
  bool assets_ready{};
};

// opt_dlgs.cpp, VictDlg.cpp, victory.cpp, and gluScore.cpp form the recovered
// in-game dialog/result state machine.  The values deliberately mirror the
// separate retail BIN dialogs instead of treating the F10 menu as glue.
enum class GameDialogScreen : std::uint8_t {
  none,
  game_menu,
  options,
  sound_options,
  speed_options,
  voice_options,
  help_menu,
  keystroke_help,
  tips,
  objectives,
  abort_menu,
  confirm_restart,
  confirm_quit_menu,
  confirm_quit_program,
  victory,
  defeat,
  score,
  count,
};

enum class MatchOutcome : std::uint8_t { none, defeat, victory };

enum class ScoreCategory : std::uint8_t {
  overview,
  units,
  structures,
  resources,
};

enum class GameDialogAction : std::uint8_t {
  none,
  redraw,
  restart_match,
  return_to_menu,
  quit_program,
};

struct MatchScoreRow {
  std::uint8_t player{};
  std::uint8_t race{};
  bool victorious{};
  std::string name{};
  std::uint32_t produced{};
  std::uint32_t killed{};
  std::uint32_t lost{};
  std::uint32_t unit_score{};
  std::uint32_t built{};
  std::uint32_t razed{};
  std::uint32_t structures_lost{};
  std::uint32_t structure_score{};
  std::uint32_t minerals{};
  std::uint32_t gas{};
  std::uint32_t resources_spent{};
  std::uint32_t resource_score{};
  std::uint32_t total{};
};

struct GameDialogRuntime {
  std::array<std::vector<GlueControl>,
             static_cast<std::size_t>(GameDialogScreen::count)>
      layouts{};
  std::vector<GlueControl> hud_menu_controls{};
  std::vector<GlueControl> score_controls{};
  std::vector<SpritePreviewFrame> dialog_tile_frames{};
  std::array<SpritePreviewFrame, 6> score_backgrounds{};
  std::array<SpritePreviewFrame, 6> score_boxes{};
  std::vector<std::uint8_t> help_text_table{};
  std::vector<std::uint8_t> tips_table{};
  std::vector<std::uint8_t> glue_text_table{};
  std::vector<std::uint32_t> initial_unit_ids{};
  std::vector<MatchScoreRow> score_rows{};
  GameDialogScreen screen{GameDialogScreen::none};
  MatchOutcome outcome{MatchOutcome::none};
  ScoreCategory score_category{ScoreCategory::overview};
  std::int16_t hovered_control{-1};
  std::int16_t pressed_control{-1};
  std::uint32_t score_started_tick{};
  std::uint32_t result_started_tick{};
  std::uint32_t initial_minerals{};
  std::uint32_t initial_gas{};
  std::size_t tip_index{1U};
  std::size_t help_scroll{};
  std::uint8_t music_volume{75U};
  std::uint8_t digital_volume{75U};
  std::uint8_t game_speed{4U};
  std::uint8_t mouse_scroll{3U};
  std::uint8_t key_scroll{3U};
  std::uint8_t voice_mode{};
  bool unit_speech{true};
  bool acknowledgements{true};
  bool building_sounds{true};
  bool show_tips{true};
  bool assets_ready{};
  bool match_active{};
  bool paused{};
  bool observer_mode{};
};

using SmackerPortrait = SmackerAnimation;

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
  std::uint8_t image_remapping{};
  bool graphics_turns{};
  std::uint16_t iscript_id{};
  starcraft::lang::IScriptState initial_iscript_state{};
  bool iscript_ready{};
  std::vector<SpritePreviewFrame> sprite_frames{};
  std::string overlay_path{};
  std::uint16_t overlay_image_id{};
  std::uint8_t overlay_draw_function{};
  std::uint8_t overlay_remapping{};
  bool overlay_graphics_turns{};
  std::uint16_t overlay_iscript_id{};
  starcraft::lang::IScriptState initial_overlay_iscript_state{};
  bool overlay_ready{};
  bool overlay_above{};
  std::int8_t overlay_x_offset{};
  std::int8_t overlay_y_offset{};
  std::vector<SpritePreviewFrame> overlay_frames{};
  std::string special_overlay_path{};
  std::uint32_t special_overlay_frame_count{};
  std::uint32_t special_overlay_point_count{};
  // Signed x/y pairs, indexed as (frame * point_count + point) * 2.
  std::vector<std::int8_t> special_overlay_points{};
  std::string damage_overlay_path{};
  std::uint32_t damage_overlay_frame_count{};
  std::uint32_t damage_overlay_point_count{};
  // Signed x/y pairs from images.dat field 11's LO table.
  std::vector<std::int8_t> damage_overlay_points{};
};

enum class ActiveUnitOrder : std::uint8_t {
  none,
  move,
  attack_move,
  attack,
  repair,
  construct,
  gather,
  return_cargo,
  terran_build,
  terran_build_exit,
  protoss_build,
  zerg_build,
  enter_transport,
  pickup_transport,
  return_hangar,
  cast_technology,
  cast_technology_position,
  archon_warp,
};

struct PendingGameSound {
  std::uint16_t sound_id{0xFFFFU};
  std::uint16_t world_x{};
  std::uint16_t world_y{};
  bool positional{};
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
  std::uint8_t destroyed_by_owner{0xFFU};
  std::size_t asset_index{};
  std::size_t selection_circle_asset_index{SIZE_MAX};
  std::int8_t selection_circle_y_offset{};
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
  std::uint8_t sprite_elevation{};
  std::uint8_t movement_turn_speed{};
  std::uint8_t movement_control{};
  std::vector<starcraft::lang::PathPoint> movement_path{};
  std::size_t movement_path_index{};
  std::uint16_t movement_final_x{};
  std::uint16_t movement_final_y{};
  // CUnitCombat.cpp::sub_4288E0 keeps order 15's destination while it
  // temporarily switches to order 9 for an acquired enemy.
  std::uint16_t attack_move_target_x{};
  std::uint16_t attack_move_target_y{};
  std::uint32_t max_hit_points{};
  std::uint32_t hit_points{};
  std::uint32_t max_shield_points{};
  std::uint32_t shield_points{};
  std::uint32_t dat_flags{};
  std::uint32_t weapon_range{};
  std::uint32_t air_weapon_range{};
  std::uint32_t order_target_id{};
  std::uint32_t harvest_source_id{};
  std::uint32_t repair_mineral_accumulator{};
  std::uint32_t repair_gas_accumulator{};
  std::uint32_t construction_builder_id{};
  std::uint32_t addon_parent_id{};
  std::uint32_t attached_addon_id{};
  std::uint32_t larva_parent_id{};
  std::uint32_t transport_parent_id{};
  std::array<std::uint32_t, 8> cargo_unit_ids{};
  std::array<std::uint32_t, 10> hangar_unit_ids{};
  std::uint32_t hangar_parent_id{};
  std::uint16_t weapon_damage{};
  std::uint16_t weapon_damage_factor{};
  std::uint16_t air_weapon_damage{};
  std::uint16_t air_weapon_damage_factor{};
  std::uint32_t projectile_damage{};
  std::uint32_t projectile_source_id{};
  std::uint32_t projectile_target_id{};
  std::uint16_t energy{};
  std::uint16_t max_energy{};
  std::uint16_t defensive_matrix_points{};
  std::uint16_t defensive_matrix_ticks{};
  std::uint16_t irradiate_ticks{};
  std::uint16_t repair_step{};
  std::uint16_t resource_amount{};
  std::uint16_t mineral_cost{};
  std::uint16_t gas_cost{};
  std::uint16_t action_timer{};
  std::uint16_t construction_ticks_total{};
  std::uint16_t construction_ticks_remaining{};
  std::uint16_t construction_target_type{0xFFFFU};
  std::uint16_t build_target_x{};
  std::uint16_t build_target_y{};
  std::uint16_t technology_ticks_total{};
  std::uint16_t technology_ticks_remaining{};
  std::uint16_t stasis_ticks{};
  std::uint16_t hallucination_ticks{};
  std::uint16_t pending_technology_x{};
  std::uint16_t pending_technology_y{};
  std::uint8_t armor{};
  std::uint8_t armor_class{};
  std::uint8_t ground_weapon{66U};
  std::uint8_t air_weapon{66U};
  std::uint8_t projectile_weapon{66U};
  std::uint8_t weapon_damage_class{};
  std::uint8_t weapon_cooldown{};
  std::uint8_t weapon_upgrade{46U};
  std::uint8_t air_weapon_damage_class{};
  std::uint8_t air_weapon_cooldown{};
  std::uint8_t air_weapon_upgrade{46U};
  std::uint8_t seek_range{};
  std::uint8_t sight_range{};
  std::uint8_t armor_upgrade{46U};
  std::uint8_t cargo_space_provided{};
  std::uint8_t cargo_space_required{};
  std::uint8_t cargo_minerals{};
  std::uint8_t cargo_gas{};
  std::uint8_t action_phase{};
  std::uint8_t construction_animation_phase{};
  std::uint8_t active_technology{28U};
  std::uint8_t pending_technology{28U};
  std::uint8_t irradiate_owner{0xFFU};
  std::uint8_t active_upgrade{46U};
  std::uint8_t last_animation{};
  std::int8_t dynamic_overlay_x_offset{};
  std::int8_t dynamic_overlay_y_offset{};
  std::uint8_t avoidance_ticks{};
  std::uint8_t collision_wait_ticks{};
  // sai_path.cpp path object +17. sub_4926E0 counts this down before another
  // blocked-path search and the recovered transition reloads it with 25.
  std::uint8_t path_recheck_ticks{};
  // CUnitCombat.cpp::sub_427610 starts the attack image action; its IScript
  // weapon opcode is the actual firing boundary.
  std::uint8_t attack_fire_timeout{};
  // CUnit+167 in sub_42BBA0. 0xFF means the damage attachment state has not
  // yet been initialized for this primary image.
  std::uint8_t damage_overlay_stage{0xFFU};
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
  // CSprite+0x12 bit 0x10 in the original. Refinery harvesting keeps the
  // CUnit alive while removing its sprite from drawing and spatial queries.
  bool sprite_hidden{};
  bool selected{};
  bool moving{};
  bool is_building{};
  bool construction_complete{true};
  bool has_ground_weapon{};
  bool has_air_weapon{};
  bool cloaked{};
  bool permanently_cloaked{};
  bool hallucination{};
  bool alive{true};
  bool production_active{};
  bool dying{};
  bool in_transport{};
  bool is_projectile{};
  bool projectile_birth_pending{};
  bool hangar_launched{};
  bool archon_merging{};
  bool attack_move_resume{};
  bool attack_fire_pending{};
  bool is_damage_overlay{};
  bool damage_overlay_heavy{};
  std::uint8_t damage_overlay_point{};
  std::uint32_t damage_overlay_parent_id{};
  // CUnitZBuild.cpp::sub_447820 leaves the hatched unit in its special
  // birth image until opcode 0x27 publishes the completion flag.  The
  // collision release is deferred to that boundary so the unit does not
  // slide away while it is still visually inside the egg.
  bool zerg_hatch_release_pending{};
  // The original Protoss building CUnit occupies its footprint while image
  // 189 performs the warp-in entrance ahead of the primary building image.
  bool construction_visible{true};
};

// Only these CUnit fields can change the tile2 creep coverage recovered from
// sub_4B2A10. Keeping the last applied source state prevents unrelated unit
// activity from rebuilding the terrain and minimap.
struct CreepSourceRuntimeState {
  std::uint16_t unit_type{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t placement_width{};
  std::uint16_t placement_height{};
  bool alive{};
  bool complete{};
};

struct VisionSourceRuntimeState {
  std::uint32_t unit_id{};
  std::uint16_t tile_x{};
  std::uint16_t tile_y{};
  std::uint8_t owner{};
  std::uint8_t sight_range{};
  bool airborne{};
  bool active{};
};

enum class FogTileState : std::uint8_t {
  visible,
  explored,
  unexplored,
};

struct AiBuildRequest {
  std::uint16_t unit_type{0xFFFFU};
  std::uint8_t quantity{};
  std::uint8_t priority{};
  std::uint8_t kind{};  // 0 unit/building, 1 upgrade, 2 technology
};

enum class ProtossSpellEffectKind : std::uint8_t {
  psionic_storm,
  recall,
};

struct ProtossSpellEffect {
  ProtossSpellEffectKind kind{};
  std::uint32_t caster_id{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t ticks_remaining{};
  std::uint16_t radius{};
  std::uint16_t damage{};
  std::uint8_t owner{};
  std::uint8_t pulse_timer{};
  std::uint8_t pulses_remaining{};
};

struct AiPlayerRuntime {
  std::shared_ptr<CAI> controller{};
  std::array<AiBuildRequest, 64> build_requests{};
  std::uint32_t script_pc{};
  std::uint16_t sleep_ticks{};
  std::uint32_t update_counter{};
  std::uint8_t macro_update_ticks{};
  std::size_t build_request_count{};
  std::uint8_t owner{};
  std::uint8_t race{};
  bool enabled{};
  bool script_active{};
  bool attack_requested{};
  std::string script_error{};
};

struct CommandControl {
  std::uint16_t position{};
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
};

struct CargoStatusSlot {
  std::uint32_t unit_id{};
  std::uint16_t control_id{};
};

enum class StatusStatKind : std::uint8_t {
  weapon,
  armor,
  shields,
};

struct StatusStatVisual {
  std::uint16_t icon{};
  std::uint16_t source_id{};
  std::uint16_t base_value{};
  std::uint16_t bonus_value{};
  std::uint8_t level{};
  StatusStatKind kind{StatusStatKind::weapon};
};

struct DatDisplayVisual {
  std::uint16_t label_string_id{};
  std::uint16_t icon{};
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
    research_technology,
    upgrade_technology,
    morph_building,
    place_nydus_exit,
    cancel_research,
    cancel_upgrade,
    cancel_construction,
    begin_patrol_target,
    hold_position,
    toggle_cloak,
    begin_load_target,
    begin_technology_target,
    archon_warp,
    unload_all,
    close_card,
  } action{};
};

struct CommandCardView {
  const CommandButtonVisual *buttons{};
  std::size_t count{};
};

struct UnitRequirementResult {
  bool visible{true};
  bool allowed{true};
  std::array<std::uint16_t, 12> required_units{};
  std::array<std::uint16_t, 12> missing_units{};
  std::size_t required_count{};
  std::size_t missing_count{};
};

struct BuildableUnitVisual {
  std::uint16_t unit_type{};
  std::uint16_t placement_width{};
  std::uint16_t placement_height{};
  std::size_t asset_index{SIZE_MAX};
  std::size_t construction_asset_index{SIZE_MAX};
  starcraft::data::UnitSimulationTraits simulation{};
  std::uint16_t addon_parent_type{0xFFFFU};
  std::int16_t addon_x{};
  std::int16_t addon_y{};
};

struct RuntimeUnitType {
  starcraft::lang::UnitInitializationData initialization{};
  std::size_t asset_index{SIZE_MAX};
  std::size_t selection_circle_asset_index{SIZE_MAX};
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
  std::size_t scv_mining_effect_asset_index{SIZE_MAX};
  std::size_t probe_mining_effect_asset_index{SIZE_MAX};
  std::size_t command_center_working_asset_index{SIZE_MAX};
  std::size_t zerg_extractor_construction_asset_index{SIZE_MAX};
  std::size_t protoss_warp_asset_index{SIZE_MAX};
  std::size_t protoss_materialize_asset_index{SIZE_MAX};
  std::size_t pylon_power_asset_index{SIZE_MAX};
  std::size_t psionic_storm_asset_index{SIZE_MAX};
  std::size_t stasis_field_asset_index{SIZE_MAX};
  std::size_t recall_asset_index{SIZE_MAX};
  std::size_t hallucination_asset_index{SIZE_MAX};
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
  // statdata.bin controls 9..12 are populated by
  // statdraw.cpp::sub_4A6D20 with armor, shields, and distinct weapons.
  std::vector<CommandControl> status_stat_controls{};
  CommandControl status_action_label_control{};
  CommandControl status_progress_control{};
  CommandControl status_construction_label_control{};
  CommandControl status_construction_progress_control{};
  std::array<CommandControl, 5> status_queue_controls{};
  // statdata.bin controls 18..32 are the three layouts selected by
  // statdraw.cpp::sub_4A89B0/sub_4A9B00 for 4-, 2-, and 1-space cargo.
  std::array<CommandControl, 15> status_cargo_controls{};
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
  // statres.bin has one supply slot per CHK race. Indexed Zerg, Terran,
  // Protoss even though the controls are numbered 1, 3, 2 respectively.
  std::array<CommandControl, 3> resource_supply_controls{};
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
  PendingGameSound pending_game_sound{};
  std::array<PendingGameSound, kDigitalSoundSourceCount - 1U>
      pending_game_sound_backlog{};
  std::uint16_t pending_game_sound_count{};
  std::uint16_t last_game_sound{0xFFFFU};
  std::array<std::uint32_t, 944> game_sound_play_counts{};
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
  std::uint32_t nydus_parent_id{};
  std::uint16_t placement_x{};
  std::uint16_t placement_y{};
  bool placement_active{};
  bool placement_valid{};
  bool command_target_active{};
  std::uint8_t target_unit_order{};
  std::uint8_t target_terrain_order{};
  std::uint8_t last_issued_order{};
  std::uint8_t last_command_opcode{};
  std::vector<BuildableUnitVisual> buildable_units{};
  std::array<RuntimeUnitType, starcraft::lang::kUnitTypeCount>
      runtime_unit_types{};
  std::array<starcraft::data::TechnologyResearchTraits, 28>
      technology_traits{};
  std::array<starcraft::data::WeaponSimulationTraits, 66> weapon_traits{};
  std::vector<DatDisplayVisual> weapon_display_traits{};
  std::array<std::size_t, 66> weapon_asset_indices{};
  std::array<std::uint8_t, 156> order_weapons{};
  std::array<std::uint8_t, 156> order_technologies{};
  std::array<std::uint8_t, 156> order_animations{};
  std::array<starcraft::data::UpgradeResearchTraits, 46> upgrade_traits{};
  std::vector<DatDisplayVisual> upgrade_display_traits{};
  std::array<bool, 28> researched_technologies{};
  std::array<std::uint8_t, 46> upgrade_levels{};
  std::array<std::array<bool, 28>, starcraft::data::chk_player_slot_count>
      player_researched_technologies{};
  std::array<std::array<std::uint8_t, 46>,
             starcraft::data::chk_player_slot_count>
      player_upgrade_levels{};
  std::uint16_t failed_runtime_unit_type{0xFFFFU};
  std::uint32_t next_unit_id{1};
  // net_misc.cpp::sub_479790/sub_4797B0 owns the synchronized gameplay RNG.
  // Construction uses it for the SCV's working point and welding duration.
  std::uint32_t synchronized_random_state{1U};
  std::uint32_t player_minerals{50};
  std::uint32_t player_gas{};
  std::array<std::uint32_t, starcraft::data::chk_player_slot_count>
      player_mineral_stock{};
  std::array<std::uint32_t, starcraft::data::chk_player_slot_count>
      player_gas_stock{};
  std::array<std::uint32_t, starcraft::data::chk_player_slot_count>
      minerals_gathered{};
  std::array<std::uint32_t, starcraft::data::chk_player_slot_count>
      gas_gathered{};
  // UI reads local_player while deterministic command playback temporarily
  // assigns command_player to the slot that authored the committed command.
  std::uint8_t local_player{};
  std::uint8_t command_player{};
  // Battle.net commands use the committed simulation turn as their clock so
  // production starts at exactly the same instant on every peer.
  std::uint32_t command_execution_clock{};
  bool synchronized_command_execution{};
  std::uint8_t local_race{};
  bool team_colors_ready{};
  std::vector<std::uint8_t> game_palette{};
  std::array<std::array<std::uint8_t, 8>, 3> selection_color_indices{};
  bool selection_colors_ready{};
  // sprites.dat field 0 has 267 u16 image IDs in this executable revision.
  // CImage opcodes 0x0E..0x11 select through this table when they create a
  // separately owned CSprite rather than an attached CImage.
  std::array<std::uint16_t, 267> sprite_image_ids{};
  // CImage.cpp::sub_410F60 indexes the five 18-byte gColorShifts entries by
  // images.dat remapping. CImage.cpp::sub_409B00 loads shift.pcx as entry 0;
  // entries 1..4 are ofire, gfire, bfire, and trans50.
  std::array<std::vector<std::uint8_t>, 5> image_color_shifts{};
  // light.cpp::sub_46A670 loads Tileset\\<era>\\dark.pcx as 32 rows of
  // 256 palette translations. mask.cpp selects rows 0..31.
  std::vector<std::uint8_t> terrain_dark_levels{};
  std::array<std::array<std::uint8_t, 8>, 12> team_color_indices{};
  std::size_t terrain_group_count{};
  std::size_t terrain_megatile_count{};
  std::size_t terrain_minitile_count{};
  starcraft::game::MultiplayerScenario scenario{};
  starcraft::gds::TilesetData terrain_tileset{};
  starcraft::lang::PathingMap pathing_map{};
  // gamemap's 32-bit word per MTXM tile: bits 0..7 are the current mask,
  // bits 8..15 are the permanent black/unexplored mask, and bits 16..31 are
  // the recovered CV5 terrain flags used by line of sight.
  std::vector<std::uint32_t> fog_map_tiles{};
  SpritePreviewFrame fogged_terrain{};
  SpritePreviewFrame fogged_minimap{};
  std::vector<VisionSourceRuntimeState> vision_source_states{};
  bool vision_source_state_ready{};
  bool fog_render_surfaces_ready{};
  bool fog_of_war_enabled{true};
  std::uint32_t fog_rebuild_count{};
  std::uint16_t fog_refresh_ticks{};
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
  std::vector<std::uint8_t> creep_edge_frames{};
  std::vector<CreepSourceRuntimeState> creep_source_states{};
  bool creep_source_state_ready{};
  std::uint32_t creep_rebuild_count{};
  std::vector<std::uint8_t> ai_script_bytes{};
  AiDifficulty ai_difficulty{AiDifficulty::medium};
  std::array<AiPlayerRuntime, 8> ai_players{};
  std::vector<ScenarioUnitPreview> units{};
  std::vector<ScenarioUnitPreview> transient_images{};
  std::vector<ProtossSpellEffect> protoss_spell_effects{};
};

struct RecoveryWindowState {
  BootstrapStatus *status{};
  GlueRuntime glue{};
  GameDialogRuntime game_dialog{};
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
  // Expensive synchronous back-buffer sampling is reserved for the OpenGL
  // regression probe and never enabled during ordinary gameplay.
  bool validate_render_pixels{};
  std::uint16_t pressed_command_position{};
  std::uint16_t hovered_command_position{};
  GLuint font_display_lists{};
  GLuint glue_font_display_lists{};
  std::array<float, 96> font_advances{};
  std::array<float, 96> glue_font_advances{};
  float font_outline_scale{10.0F};
  float glue_font_outline_scale{20.0F};
  ALCdevice *audio_device{};
  ALCcontext *audio_context{};
  std::array<ALuint, kDigitalSoundSourceCount> audio_sources{};
  std::uint16_t audio_source_cursor{};
  std::array<ALuint, 2> resource_error_buffers{};
  std::vector<ALuint> archived_sound_buffers{};
  ALuint music_source{};
  ALuint music_buffer{};
  std::uint32_t audio_play_count{};
  bool audio_ready{};
  bool music_playing{};
  bool imgui_ready{};
  bool debug_console_open{};
  bool debug_console_focus{};
  std::array<char, 256> debug_console_input{};
  std::vector<std::string> debug_console_lines{};
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
[[nodiscard]] std::size_t cargo_status_slots(
    const BootstrapStatus &status, const ScenarioUnitPreview &transport,
    std::array<CargoStatusSlot, 8> &slots) noexcept;
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
[[nodiscard]] bool initialize_glue_assets(GlueRuntime &glue) noexcept;
[[nodiscard]] PresentationViewport presentation_viewport(
    int client_width, int client_height) noexcept;
[[nodiscard]] bool parse_glue_layout(
    const std::vector<std::uint8_t> &layout,
    std::vector<GlueControl> &controls) noexcept;
[[nodiscard]] bool enumerate_glue_maps(
    starcraft::runtime::StormModule &storm, const std::filesystem::path &root,
    GlueRuntime &glue) noexcept;
void configure_lobby_slots(GlueRuntime &glue) noexcept;
[[nodiscard]] bool glue_active(const GlueRuntime &glue) noexcept;
[[nodiscard]] bool render_glue(const RecoveryWindowState &state) noexcept;
[[nodiscard]] bool client_to_glue(HWND window, LPARAM lparam, int &glue_x,
                                  int &glue_y) noexcept;
[[nodiscard]] GlueAction glue_mouse_move(GlueRuntime &glue, int x,
                                         int y) noexcept;
[[nodiscard]] GlueAction glue_left_down(GlueRuntime &glue, int x,
                                        int y) noexcept;
[[nodiscard]] GlueAction glue_left_up(GlueRuntime &glue, int x, int y,
                                      std::uint32_t now) noexcept;
[[nodiscard]] GlueAction glue_key_down(GlueRuntime &glue, WPARAM key,
                                       std::uint32_t now) noexcept;
[[nodiscard]] GlueAction glue_character(GlueRuntime &glue,
                                        char character) noexcept;
[[nodiscard]] GlueAction advance_glue(GlueRuntime &glue,
                                      std::uint32_t now) noexcept;
[[nodiscard]] bool title_loading_visible(const GlueRuntime &glue,
                                         std::uint32_t now) noexcept;
[[nodiscard]] GlueFontStyle
glue_control_font_style(const GlueControl &control,
                        bool highlighted = false) noexcept;
void glues_enter_screen(GlueRuntime &glue, GlueScreen screen,
                        std::uint32_t now) noexcept;
[[nodiscard]] GlueAction glues_leave_screen(GlueRuntime &glue,
                                            GlueScreen target,
                                            GlueAction action,
                                            std::uint32_t now) noexcept;
void glues_control_rect(const GlueRuntime &glue, const GlueControl &control,
                        std::int16_t &left, std::int16_t &top,
                        std::int16_t &right, std::int16_t &bottom) noexcept;
void draw_glue_text_gl(const RecoveryWindowState &state, std::string_view text,
                       float x, float y, std::uint8_t red = 220U,
                       std::uint8_t green = 220U,
                       std::uint8_t blue = 220U,
                       bool large = false) noexcept;
void draw_glue_styled_text_gl(
    const RecoveryWindowState &state, std::string_view text, float x, float y,
    GlueFontStyle style = GlueFontStyle::normal, bool large = false,
    std::uint8_t alpha = 255U) noexcept;
void draw_game_text_gl(const RecoveryWindowState &state,
                       std::string_view text, float x, float y,
                       std::uint8_t red = 220U,
                       std::uint8_t green = 220U,
                       std::uint8_t blue = 220U,
                       bool large = false) noexcept;
void draw_glue_centered_text_gl(const RecoveryWindowState &state,
                                std::string_view text,
                                const GlueControl &control,
                                std::uint8_t red = 220U,
                                std::uint8_t green = 220U,
                                std::uint8_t blue = 220U,
                                bool large = true) noexcept;
void draw_glue_centered_styled_text_gl(
    const RecoveryWindowState &state, std::string_view text,
    const GlueControl &control,
    GlueFontStyle style = GlueFontStyle::normal, bool large = true,
    std::uint8_t alpha = 255U) noexcept;
void draw_title_gl(const RecoveryWindowState &state) noexcept;
void draw_main_menu_gl(const RecoveryWindowState &state) noexcept;
void draw_glue_ok_popup_gl(const RecoveryWindowState &state) noexcept;
void draw_connection_gl(const RecoveryWindowState &state) noexcept;
void draw_map_selection_gl(const RecoveryWindowState &state) noexcept;
void draw_lobby_gl(const RecoveryWindowState &state) noexcept;
void draw_lobby_slots_gl(const RecoveryWindowState &state) noexcept;
void draw_ready_gl(const RecoveryWindowState &state,
                   std::uint32_t now) noexcept;
[[nodiscard]] std::int16_t main_menu_control_at(const GlueRuntime &glue, int x,
                                                int y) noexcept;
[[nodiscard]] std::int16_t connection_control_at(const GlueRuntime &glue,
                                                 int x, int y) noexcept;
[[nodiscard]] GlueAction
activate_main_menu_control(GlueRuntime &glue, std::int16_t identifier,
                           std::uint32_t now) noexcept;
void show_glue_ok_popup(GlueRuntime &glue,
                        std::string_view message) noexcept;
void dismiss_glue_ok_popup(GlueRuntime &glue) noexcept;
[[nodiscard]] std::int16_t glue_ok_popup_control_at(
    const GlueRuntime &glue, int x, int y) noexcept;
[[nodiscard]] GlueAction
activate_connection_control(GlueRuntime &glue, std::int16_t identifier, int x,
                            int y, std::uint32_t now) noexcept;
[[nodiscard]] std::int16_t map_selection_control_at(const GlueRuntime &glue,
                                                    int x, int y) noexcept;
[[nodiscard]] GlueAction
activate_map_selection_control(GlueRuntime &glue, std::int16_t identifier,
                               int x, int y, std::uint32_t now) noexcept;
[[nodiscard]] std::int16_t lobby_control_at(const GlueRuntime &glue, int x,
                                            int y) noexcept;
[[nodiscard]] GlueAction activate_lobby_control(
    GlueRuntime &glue, std::int16_t identifier, int x, int y,
    std::uint32_t now) noexcept;
[[nodiscard]] bool start_selected_glue_map(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool initialize_game_dialog_assets(
    GameDialogRuntime &dialog, const BootstrapStatus &status) noexcept;
void begin_match_flow(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool game_dialog_active(const RecoveryWindowState &state) noexcept;
void open_game_menu(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool hud_menu_button_at(const RecoveryWindowState &state, int x,
                                      int y) noexcept;
[[nodiscard]] GameDialogAction game_dialog_mouse_move(
    RecoveryWindowState &state, int x, int y) noexcept;
[[nodiscard]] GameDialogAction game_dialog_left_down(
    RecoveryWindowState &state, int x, int y) noexcept;
[[nodiscard]] GameDialogAction game_dialog_left_up(
    RecoveryWindowState &state, int x, int y, std::uint32_t now) noexcept;
[[nodiscard]] GameDialogAction game_dialog_key_down(
    RecoveryWindowState &state, WPARAM key, std::uint32_t now) noexcept;
void draw_hud_menu_button_gl(const RecoveryWindowState &state) noexcept;
void draw_game_dialog_gl(const RecoveryWindowState &state,
                         std::uint32_t now) noexcept;
void evaluate_melee_outcome(RecoveryWindowState &state) noexcept;
void show_match_outcome(RecoveryWindowState &state,
                        MatchOutcome outcome) noexcept;
[[nodiscard]] bool play_result_music(RecoveryWindowState &state,
                                     MatchOutcome outcome) noexcept;
[[nodiscard]] bool play_title_music(RecoveryWindowState &state) noexcept;
void build_match_scores(RecoveryWindowState &state) noexcept;
void draw_score_screen_gl(const RecoveryWindowState &state,
                          std::uint32_t now) noexcept;
[[nodiscard]] bool decode_smacker_frame(SmackerPortrait &portrait) noexcept;
[[nodiscard]] bool load_smacker_animation(
    starcraft::runtime::StormModule &storm, const char *path,
    bool transparent_zero, SmackerAnimation &output) noexcept;
[[nodiscard]] bool advance_smacker_animation(SmackerAnimation &animation,
                                              std::uint32_t now) noexcept;
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
[[nodiscard]] bool sprite_draws_before(
    const ScenarioUnitPreview &left,
    const ScenarioUnitPreview &right) noexcept;
[[nodiscard]] bool sprite_intersects_world_viewport(
    const BootstrapStatus &status,
    const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] bool pylon_power_display_active(
    const BootstrapStatus &status) noexcept;
void draw_pylon_power_fields_gl(const BootstrapStatus &status);
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
[[nodiscard]] std::array<std::uint32_t, 2>
player_supply(const BootstrapStatus &status, std::uint8_t owner,
              std::uint8_t race) noexcept;
[[nodiscard]] std::size_t
resource_supply_icon_frame(std::uint8_t race) noexcept;
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
[[nodiscard]] bool initialize_fog_of_war(BootstrapStatus &status) noexcept;
[[nodiscard]] bool rebuild_fog_of_war(BootstrapStatus &status,
                                       bool force = false) noexcept;
[[nodiscard]] bool
rebuild_fog_render_surfaces(BootstrapStatus &status) noexcept;
[[nodiscard]] FogTileState fog_tile_state(
    const BootstrapStatus &status, int tile_x, int tile_y,
    std::uint8_t player) noexcept;
[[nodiscard]] bool fog_world_position_visible(
    const BootstrapStatus &status, std::uint16_t world_x,
    std::uint16_t world_y, std::uint8_t player) noexcept;
[[nodiscard]] bool fog_unit_visible(
    const BootstrapStatus &status,
    const ScenarioUnitPreview &unit,
    std::uint8_t player) noexcept;
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
[[nodiscard]] UnitRequirementResult unit_requirements_for(
    const BootstrapStatus &status, const ScenarioUnitPreview &producer,
    std::uint16_t product_type) noexcept;
[[nodiscard]] bool command_button_enabled(
    const BootstrapStatus &status, const ScenarioUnitPreview &producer,
    const CommandButtonVisual &button) noexcept;
[[nodiscard]] CommandCardView
recovered_building_card(std::uint16_t unit_type) noexcept;
[[nodiscard]] const BuildableUnitVisual *
find_buildable_unit(const BootstrapStatus &status,
                    std::uint16_t unit_type) noexcept;
[[nodiscard]] bool build_terrain_preview(
    const starcraft::gds::TilesetData &tileset,
    const starcraft::game::MultiplayerScenario &scenario,
    std::uint16_t camera_x, std::uint16_t camera_y, SpritePreviewFrame &output,
    const std::vector<std::uint8_t> *creep_tiles = nullptr,
    const std::vector<std::uint8_t> *creep_visual_tiles = nullptr,
    const std::vector<std::uint8_t> *creep_edge_frames = nullptr);
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
[[nodiscard]] std::uint32_t player_minerals_for(
    const BootstrapStatus &status, std::uint8_t player) noexcept;
[[nodiscard]] std::uint32_t player_gas_for(
    const BootstrapStatus &status, std::uint8_t player) noexcept;
void spend_player_resources(BootstrapStatus &status, std::uint8_t player,
                            std::uint32_t minerals,
                            std::uint32_t gas) noexcept;
void refund_player_resources(BootstrapStatus &status, std::uint8_t player,
                             std::uint32_t minerals,
                             std::uint32_t gas) noexcept;

[[nodiscard]] bool restart_unit_animation(BootstrapStatus &status,
                                           ScenarioUnitPreview &unit,
                                           std::uint8_t animation) noexcept;
[[nodiscard]] bool materialize_unit_overlay(
    const BootstrapStatus &status, ScenarioUnitPreview &unit,
    const starcraft::lang::IScriptState &source) noexcept;
[[nodiscard]] bool
advance_building_construction_animation(BootstrapStatus &status,
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
[[nodiscard]] bool is_airborne(const ScenarioUnitPreview &unit) noexcept;
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
void stop_unit_movement(BootstrapStatus &status,
                        ScenarioUnitPreview &unit) noexcept;
void cancel_unit_order(BootstrapStatus &status,
                       ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] std::size_t issue_scv_move_order(BootstrapStatus &status,
                                               std::uint16_t target_x,
                                               std::uint16_t target_y) noexcept;
[[nodiscard]] std::size_t issue_scv_attack_move_order(
    BootstrapStatus &status, std::uint16_t target_x,
    std::uint16_t target_y) noexcept;
[[nodiscard]] std::uint32_t
effective_unit_top_speed(const BootstrapStatus &status,
                         const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] std::uint32_t
effective_unit_weapon_range(const BootstrapStatus &status,
                            const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] std::uint32_t
effective_unit_weapon_cooldown(const BootstrapStatus &status,
                               const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] bool unit_has_weapon_against(
    const ScenarioUnitPreview &unit,
    const ScenarioUnitPreview &target) noexcept;
[[nodiscard]] ScenarioUnitPreview *
find_unit_by_id(BootstrapStatus &status, std::uint32_t unit_id) noexcept;
[[nodiscard]] const ScenarioUnitPreview *
find_unit_by_id(const BootstrapStatus &status, std::uint32_t unit_id) noexcept;
[[nodiscard]] int unit_edge_distance(const ScenarioUnitPreview &left,
                                     const ScenarioUnitPreview &right) noexcept;
[[nodiscard]] bool
plan_scv_interaction_path(BootstrapStatus &status, ScenarioUnitPreview &worker,
                          const ScenarioUnitPreview &target,
                          int interaction_range,
                          ActiveUnitOrder order) noexcept;
[[nodiscard]] bool begin_scv_interaction(BootstrapStatus &status,
                                         ScenarioUnitPreview &worker,
                                         const ScenarioUnitPreview &target,
                                         ActiveUnitOrder order) noexcept;
[[nodiscard]] bool transport_accepts_unit(
    const BootstrapStatus &status, const ScenarioUnitPreview &transport,
    const ScenarioUnitPreview &passenger) noexcept;
[[nodiscard]] std::size_t unload_transport_units(
    BootstrapStatus &status, ScenarioUnitPreview &transport) noexcept;
[[nodiscard]] bool toggle_unit_cloak(BootstrapStatus &status,
                                     ScenarioUnitPreview &unit,
                                     bool cloak) noexcept;
[[nodiscard]] std::uint8_t technology_target_order(
    std::uint8_t technology) noexcept;
[[nodiscard]] bool cast_unit_technology(BootstrapStatus &status,
                                        ScenarioUnitPreview &caster,
                                        ScenarioUnitPreview &target,
                                        std::uint8_t technology) noexcept;
[[nodiscard]] bool cast_unit_technology_at(BootstrapStatus &status,
                                           ScenarioUnitPreview &caster,
                                           std::uint16_t target_x,
                                           std::uint16_t target_y,
                                           std::uint8_t technology) noexcept;
[[nodiscard]] bool cast_protoss_technology(
    BootstrapStatus &status, ScenarioUnitPreview &caster,
    ScenarioUnitPreview *target, std::uint16_t target_x,
    std::uint16_t target_y, std::uint8_t technology) noexcept;
[[nodiscard]] bool advance_protoss_spell_effects(
    BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_unit_energy(BootstrapStatus &status) noexcept;
void initialize_unit_energy(const BootstrapStatus &status,
                            ScenarioUnitPreview &unit) noexcept;
void apply_fixed_unit_damage(ScenarioUnitPreview &target,
                             std::uint32_t damage) noexcept;
void apply_combat_unit_damage(BootstrapStatus &status,
                              ScenarioUnitPreview &target,
                              std::uint32_t damage,
                              std::uint32_t attacker_id,
                              std::uint8_t attacking_owner) noexcept;
void apply_weapon_unit_damage(BootstrapStatus &status,
                              ScenarioUnitPreview &target,
                              std::uint32_t raw_damage,
                              std::uint8_t weapon_type,
                              std::uint32_t attacker_id,
                              std::uint8_t attacking_owner) noexcept;
[[nodiscard]] bool fire_pending_unit_weapon(
    BootstrapStatus &status, ScenarioUnitPreview &attacker,
    std::uint8_t weapon_event) noexcept;
[[nodiscard]] bool
advance_building_damage_states(BootstrapStatus &status) noexcept;
void destroy_unit(BootstrapStatus &status, ScenarioUnitPreview &target,
                  std::uint8_t attacking_owner) noexcept;
[[nodiscard]] bool cancel_building_construction(
    BootstrapStatus &status, ScenarioUnitPreview &building) noexcept;
void cancel_command_target(BootstrapStatus &status) noexcept;
void begin_command_target(BootstrapStatus &status, std::uint8_t unit_order,
                          std::uint8_t terrain_order) noexcept;
[[nodiscard]] std::size_t
issue_active_scv_target(BootstrapStatus &status, std::uint16_t world_x,
                        std::uint16_t world_y,
                        std::uint32_t target_unit_id = UINT32_MAX) noexcept;
[[nodiscard]] std::size_t issue_scv_smart_order(BootstrapStatus &status,
                                                std::uint16_t world_x,
                                                std::uint16_t world_y) noexcept;
[[nodiscard]] std::uint32_t
smart_order_target_at(const BootstrapStatus &status, std::uint16_t world_x,
                      std::uint16_t world_y) noexcept;
[[nodiscard]] std::size_t issue_scv_smart_order_target(
    BootstrapStatus &status, std::uint16_t world_x, std::uint16_t world_y,
    std::uint32_t target_unit_id) noexcept;
[[nodiscard]] std::size_t
issue_scv_return_cargo(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_unit_movement(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_unit_actions(BootstrapStatus &status) noexcept;
[[nodiscard]] bool spawn_worker_mining_effect(
    BootstrapStatus &status, const ScenarioUnitPreview &worker,
    std::uint8_t weapon_type) noexcept;
[[nodiscard]] bool spawn_weapon_projectile(
    BootstrapStatus &status, const ScenarioUnitPreview &source,
    const ScenarioUnitPreview &target, std::uint8_t weapon_type,
    std::uint32_t damage) noexcept;
[[nodiscard]] bool spawn_resource_overlay_effect(
    BootstrapStatus &status, const ScenarioUnitPreview &source,
    std::uint8_t point) noexcept;
[[nodiscard]] bool spawn_iscript_sprite_effect(
    BootstrapStatus &status, const ScenarioUnitPreview &source,
    const starcraft::lang::IScriptState &event) noexcept;
[[nodiscard]] bool advance_transient_images(BootstrapStatus &status,
                                            std::uint32_t clock) noexcept;
[[nodiscard]] bool rebuild_creep_tiles(BootstrapStatus &status) noexcept;
[[nodiscard]] bool tile_has_creep(const BootstrapStatus &status, int tile_x,
                                  int tile_y) noexcept;
[[nodiscard]] bool advance_addon_construction(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_protoss_building_construction(
    BootstrapStatus &status) noexcept;
[[nodiscard]] bool complete_protoss_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &probe) noexcept;
[[nodiscard]] bool advance_zerg_building_construction(
    BootstrapStatus &status) noexcept;
[[nodiscard]] bool initialize_ai_players(BootstrapStatus &status) noexcept;
[[nodiscard]] bool advance_ai_players(BootstrapStatus &status,
                                      std::uint32_t now) noexcept;
[[nodiscard]] bool advance_technology_research(
    BootstrapStatus &status) noexcept;
[[nodiscard]] bool addon_center_for_parent(const BuildableUnitVisual &addon,
                                           const ScenarioUnitPreview &parent,
                                           std::uint16_t &center_x,
                                           std::uint16_t &center_y) noexcept;
[[nodiscard]] bool placement_is_valid(const BootstrapStatus &status,
                                      const BuildableUnitVisual &buildable,
                                      std::uint16_t center_x,
                                      std::uint16_t center_y,
                                      std::uint8_t owner = 0U,
                                      std::uint32_t ignored_unit_id = 0U) noexcept;
[[nodiscard]] bool update_building_placement(BootstrapStatus &status,
                                             int game_x, int game_y) noexcept;
[[nodiscard]] bool place_current_building(BootstrapStatus &status) noexcept;
[[nodiscard]] bool begin_terran_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &scv,
    const BuildableUnitVisual &buildable, std::uint16_t center_x,
    std::uint16_t center_y, bool charge_resources) noexcept;
[[nodiscard]] bool complete_terran_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &scv) noexcept;
[[nodiscard]] bool advance_terran_construction_order(
    BootstrapStatus &status, ScenarioUnitPreview &scv,
    ScenarioUnitPreview &building) noexcept;
[[nodiscard]] bool begin_protoss_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &probe,
    const BuildableUnitVisual &buildable, std::uint16_t center_x,
    std::uint16_t center_y, bool charge_resources) noexcept;
[[nodiscard]] bool begin_zerg_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &drone,
    const BuildableUnitVisual &buildable, std::uint16_t center_x,
    std::uint16_t center_y, bool charge_resources) noexcept;
[[nodiscard]] bool complete_zerg_build_order(
    BootstrapStatus &status, ScenarioUnitPreview &drone) noexcept;
[[nodiscard]] bool configure_preview_type(BootstrapStatus &status,
                                          ScenarioUnitPreview &unit,
                                          std::uint16_t unit_type) noexcept;
[[nodiscard]] bool replace_preview_primary_image(
    BootstrapStatus &status, ScenarioUnitPreview &unit,
    std::size_t asset_index) noexcept;
void activate_command_button(BootstrapStatus &status,
                             std::uint16_t position) noexcept;
[[nodiscard]] bool advance_unit_production(BootstrapStatus &status,
                                           std::uint32_t now) noexcept;
[[nodiscard]] bool advance_zerg_larvae(BootstrapStatus &status) noexcept;
[[nodiscard]] std::size_t displace_units_for_zerg_egg(
    BootstrapStatus &status, ScenarioUnitPreview &egg) noexcept;
void shutdown_audio(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool initialize_audio(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool
play_pending_resource_error(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool play_pending_game_sound(RecoveryWindowState &state) noexcept;
[[nodiscard]] bool play_digital_sound_buffer(RecoveryWindowState &state,
                                             ALuint buffer,
                                             const PendingGameSound &event = {}) noexcept;
[[nodiscard]] bool queue_game_sound(BootstrapStatus &status,
                                    std::uint16_t sound_id) noexcept;
[[nodiscard]] bool queue_positional_game_sound(
    BootstrapStatus &status, std::uint16_t sound_id, std::uint16_t world_x,
    std::uint16_t world_y) noexcept;
[[nodiscard]] bool queue_unit_ready_sound(
    BootstrapStatus &status, const ScenarioUnitPreview &unit) noexcept;
[[nodiscard]] bool initialize_debug_console(HWND window,
                                            RecoveryWindowState &state) noexcept;
void shutdown_debug_console(RecoveryWindowState &state) noexcept;
void draw_debug_console(RecoveryWindowState &state,
                        const PresentationViewport &viewport) noexcept;
[[nodiscard]] bool debug_console_wants_mouse() noexcept;
[[nodiscard]] bool debug_console_wants_keyboard() noexcept;
[[nodiscard]] bool execute_debug_console_command(BootstrapStatus &status,
                                                 std::string_view command,
                                                 std::string &result) noexcept;
[[nodiscard]] std::uint16_t choose_unit_sound(BootstrapStatus &status,
                                              std::uint16_t first,
                                              std::uint16_t last) noexcept;
[[nodiscard]] bool queue_unit_response(BootstrapStatus &status,
                                       const ScenarioUnitPreview &unit,
                                       bool order_acknowledgement) noexcept;
[[nodiscard]] bool queue_network_smart_order(
    RecoveryWindowState &state, std::uint16_t world_x,
    std::uint16_t world_y) noexcept;
[[nodiscard]] bool queue_network_command_button(
    RecoveryWindowState &state, std::uint16_t position) noexcept;
[[nodiscard]] bool queue_network_target_order(
    RecoveryWindowState &state, std::uint16_t world_x,
    std::uint16_t world_y) noexcept;
[[nodiscard]] bool queue_network_building_placement(
    RecoveryWindowState &state) noexcept;
[[nodiscard]] bool take_network_outgoing_payload(
    battle::BattleRuntime &runtime, std::uint32_t turn,
    std::vector<std::uint8_t> &payload) noexcept;
[[nodiscard]] bool apply_network_committed_turn(
    BootstrapStatus &status, const battle::CommittedTurn &commit) noexcept;
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
[[nodiscard]] std::size_t status_stat_visuals(
    const BootstrapStatus &status, const ScenarioUnitPreview &unit,
    std::array<StatusStatVisual, 4> &visuals) noexcept;
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
[[nodiscard]] BootstrapStatus probe_assets(
    const std::filesystem::path &selected_map = {},
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>
        *ownership_override = nullptr,
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>
        *race_override = nullptr,
    std::uint32_t melee_start_seed = 0U,
    std::uint8_t local_player = 0U);
[[nodiscard]] bool initialize_opengl(HWND window,
                                     RecoveryWindowState &state) noexcept;
void shutdown_opengl(HWND window, RecoveryWindowState &state) noexcept;
void draw_selection_drag_gl(const RecoveryWindowState &state) noexcept;
void draw_command_target_gl(const RecoveryWindowState &state) noexcept;
[[nodiscard]] bool render_opengl(HWND window,
                                 RecoveryWindowState &state) noexcept;
[[nodiscard]] bool capture_opengl_bmp(HWND window, RecoveryWindowState &state,
                                      const char *output_path) noexcept;
// gameloop.cpp owns the executable's non-blocking message pump and fixed-rate
// simulation scheduler. A frame can also be advanced directly by regression
// probes without manufacturing a window-timer message.
[[nodiscard]] bool advance_game_loop_frame(
    HWND window, RecoveryWindowState &state, std::uint32_t clock) noexcept;
[[nodiscard]] int run_game_loop(HWND window,
                                RecoveryWindowState &state) noexcept;
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
