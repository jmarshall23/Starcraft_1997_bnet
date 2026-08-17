#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

namespace {

constexpr int kMaximumCatchUpFrames = 3;
constexpr std::int32_t kClockResyncThresholdMilliseconds = 200;

void drain_pending_game_sounds(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr) {
    return;
  }
  for (std::size_t slot = 0U; slot < kDigitalSoundSourceCount &&
                              state.status->pending_game_sound.sound_id !=
                                  0xFFFFU;
       ++slot) {
    (void)play_pending_game_sound(state);
  }
}

[[nodiscard]] std::int32_t tick_delta(const std::uint32_t left,
                                      const std::uint32_t right) noexcept {
  return static_cast<std::int32_t>(left - right);
}

[[nodiscard]] bool apply_timed_glue_action(
    const HWND window, RecoveryWindowState &state,
    const GlueAction action) noexcept {
  switch (action) {
  case GlueAction::redraw:
    return true;
  case GlueAction::start_game:
    (void)start_selected_glue_map(state);
    return true;
  case GlueAction::quit:
    PostMessageA(window, WM_CLOSE, 0, 0);
    return false;
  case GlueAction::none:
  default:
    return false;
  }
}

[[nodiscard]] bool advance_timed_game_dialog(
    const HWND window, RecoveryWindowState &state,
    const std::uint32_t clock) noexcept {
  if (!state.game_dialog.paused) {
    return false;
  }
  if (state.game_dialog.screen == GameDialogScreen::defeat &&
      clock - state.game_dialog.result_started_tick >= 20000U) {
    build_match_scores(state);
    state.game_dialog.score_started_tick = clock;
    state.game_dialog.screen = GameDialogScreen::score;
    state.game_dialog.hovered_control = -1;
    state.game_dialog.pressed_control = -1;
    return true;
  }
  if (state.game_dialog.screen == GameDialogScreen::score &&
      clock - state.game_dialog.score_started_tick >= 120000U) {
    state.game_dialog.screen = GameDialogScreen::none;
    state.game_dialog.match_active = false;
    state.game_dialog.paused = false;
    state.game_dialog.observer_mode = false;
    glues_enter_screen(state.glue, GlueScreen::main_menu, clock);
    if (state.music_playing) {
      alSourceStop(state.music_source);
      state.music_playing = false;
    }
    (void)play_title_music(state);
    return true;
  }
  (void)window;
  return false;
}

} // namespace

bool advance_game_loop_frame(const HWND window, RecoveryWindowState &state,
                             const std::uint32_t clock) noexcept {
  if (glue_active(state.glue)) {
    return apply_timed_glue_action(window, state,
                                   advance_glue(state.glue, clock));
  }
  if (state.game_dialog.paused) {
    return advance_timed_game_dialog(window, state, clock);
  }

  BootstrapStatus *const status = state.status;
  if (status == nullptr || status->unit_assets.empty() ||
      status->units.empty()) {
    return false;
  }

  const starcraft::lang::IScriptProgramView program{
      status->iscript_bytes.data(), status->iscript_bytes.size()};
  (void)advance_camera_scroll(state);
  (void)advance_zerg_larvae(*status);
  (void)advance_ai_players(*status, clock);
  (void)advance_unit_production(*status, clock);
  (void)advance_technology_research(*status);
  advance_resource_display(*status);
  (void)advance_unit_movement(*status);
  (void)advance_unit_actions(*status);
  (void)advance_unit_energy(*status);
  (void)advance_protoss_spell_effects(*status);
  (void)advance_addon_construction(*status);
  (void)advance_protoss_building_construction(*status);
  (void)advance_zerg_building_construction(*status);
  (void)play_pending_resource_error(state);
  evaluate_melee_outcome(state);
  // This is an O(creep-source-count) state check on ordinary turns. Terrain
  // and minimap reconstruction occurs only when a source appears, completes,
  // moves, changes footprint, or dies.
  (void)rebuild_creep_tiles(*status);
  (void)advance_selected_portrait(*status, clock);

  for (std::size_t index = 0; index < status->units.size(); ++index) {
    ScenarioUnitPreview &unit = status->units[index];
    if (!unit.alive || !unit.iscript_ready ||
        unit.asset_index >= status->unit_assets.size()) {
      continue;
    }
    if (unit.iscript_state.waiting_for_attack_target) {
      const ScenarioUnitPreview *const target =
          find_unit_by_id(*status, unit.order_target_id);
      if (unit.active_order != ActiveUnitOrder::attack || target == nullptr ||
          !target->alive) {
        // Opcode 0x35 calls CUnitCombat.cpp::sub_425670 once CUnit+100 is
        // null; CSprite.cpp::sub_41C0B0 then maps ground attack init/repeat
        // (2/5) to action 8. Do that handoff before the held script can retry.
        unit.iscript_state.waiting_for_attack_target = false;
        (void)restart_unit_animation(*status, unit, 8U);
      }
    }
    const UnitRenderAsset &asset = status->unit_assets[unit.asset_index];
    const std::uint32_t previous_weapon_events =
        unit.iscript_state.weapon_event_count;
    const std::uint32_t previous_sound_events =
        unit.iscript_state.sound_event_count;
    const std::uint32_t previous_overlay_events =
        unit.iscript_state.overlay_event_count;
    const std::uint32_t previous_velocity_events =
        unit.iscript_state.flingy_velocity_event_count;
    const std::uint32_t previous_resource_overlay_events =
        unit.iscript_state.resource_overlay_event_count;
    const std::uint32_t previous_sprite_events =
        unit.iscript_state.sprite_event_count;
    const auto result = program.tick(
        unit.iscript_state,
        clock ^ static_cast<std::uint32_t>(index * 0x9E3779B9U), 256,
        nullptr, status->scenario.tileset_id());
    if (unit.iscript_state.sound_event_count != previous_sound_events) {
      (void)queue_positional_game_sound(
          *status, unit.iscript_state.sound_event, unit.x, unit.y);
    }
    if (unit.iscript_state.overlay_event_count != previous_overlay_events) {
      (void)materialize_unit_overlay(*status, unit, unit.iscript_state);
    }
    if (unit.iscript_state.sprite_event_count != previous_sprite_events) {
      (void)spawn_iscript_sprite_effect(*status, unit, unit.iscript_state);
    }
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
          unit.active_order == ActiveUnitOrder::gather &&
          (unit.iscript_state.weapon_event == 8U ||
           unit.iscript_state.weapon_event == 42U)) {
        (void)spawn_worker_mining_effect(*status, unit,
                                         unit.iscript_state.weapon_event);
      }
      if (unit.iscript_state.resource_overlay_event_count !=
          previous_resource_overlay_events) {
        (void)spawn_resource_overlay_effect(
            *status, unit, unit.iscript_state.resource_overlay_point);
      }
    }

    if (unit.overlay_ready && !asset.overlay_frames.empty()) {
      const std::uint32_t previous_overlay_sound_events =
          unit.overlay_iscript_state.sound_event_count;
      const auto overlay_result = program.tick(
          unit.overlay_iscript_state,
          clock ^ static_cast<std::uint32_t>(index * 0x85EBCA6BU) ^
              0x5A5A5A5AU,
          256, &unit.iscript_state, status->scenario.tileset_id());
      if (unit.overlay_iscript_state.sound_event_count !=
          previous_overlay_sound_events) {
        (void)queue_positional_game_sound(
            *status, unit.overlay_iscript_state.sound_event, unit.x, unit.y);
      }
      unit.iscript_state.image_target_flags |=
          unit.overlay_iscript_state.image_target_flags;
      unit.overlay_iscript_state.image_target_flags = 0U;
      if (overlay_result ==
              starcraft::lang::IScriptTickResult::malformed_program ||
          overlay_result ==
              starcraft::lang::IScriptTickResult::unsupported_opcode ||
          overlay_result ==
              starcraft::lang::IScriptTickResult::instruction_limit ||
          overlay_result == starcraft::lang::IScriptTickResult::ended ||
          unit.overlay_iscript_state.frame >= asset.overlay_frames.size()) {
        unit.overlay_ready = false;
      } else {
        unit.current_overlay_frame = unit.overlay_iscript_state.frame;
      }
    }

    if (unit.dynamic_overlay_ready &&
        unit.dynamic_overlay_asset_index < status->unit_assets.size()) {
      const UnitRenderAsset &dynamic_asset =
          status->unit_assets[unit.dynamic_overlay_asset_index];
      const std::uint32_t previous_dynamic_sound_events =
          unit.dynamic_overlay_iscript_state.sound_event_count;
      const auto dynamic_result = program.tick(
          unit.dynamic_overlay_iscript_state,
          clock ^ static_cast<std::uint32_t>(index * 0xC2B2AE35U) ^
              0x3C3C3C3CU,
          256, &unit.iscript_state, status->scenario.tileset_id());
      if (unit.dynamic_overlay_iscript_state.sound_event_count !=
          previous_dynamic_sound_events) {
        (void)queue_positional_game_sound(
            *status, unit.dynamic_overlay_iscript_state.sound_event, unit.x,
            unit.y);
      }
      unit.iscript_state.image_target_flags |=
          unit.dynamic_overlay_iscript_state.image_target_flags;
      unit.dynamic_overlay_iscript_state.image_target_flags = 0U;
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
        if (unit.dying) {
          unit.alive = false;
          unit.dying = false;
        }
      } else {
        unit.current_dynamic_overlay_frame =
            unit.dynamic_overlay_iscript_state.frame;
      }
    }
    if (unit.dying && !unit.dynamic_overlay_ready &&
        (result == starcraft::lang::IScriptTickResult::ended ||
         !unit.iscript_ready)) {
      unit.alive = false;
      unit.dying = false;
    }
  }
  (void)advance_transient_images(*status, clock);
  drain_pending_game_sounds(state);
  return true;
}

int run_game_loop(const HWND window, RecoveryWindowState &state) noexcept {
  // gameloop.cpp::sub_453B60 keeps simulation outside the window procedure.
  // sub_44C750 drains the Win32 queue with PeekMessage, the outer loop runs at
  // the selected game-speed interval, performs at most three catch-up turns,
  // and only then presents a frame.  Preserve that ordering here.
  std::uint32_t next_simulation_tick = GetTickCount();
  bool redraw = true;
  MSG message{};
  for (;;) {
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != FALSE) {
      if (message.message == WM_QUIT) {
        return static_cast<int>(message.wParam);
      }
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }

    std::uint32_t now = GetTickCount();
    const std::int32_t schedule_error = tick_delta(now, next_simulation_tick);
    if (schedule_error > kClockResyncThresholdMilliseconds ||
        schedule_error < -kClockResyncThresholdMilliseconds) {
      next_simulation_tick = now;
    }

    int frames_advanced = 0;
    while (tick_delta(now, next_simulation_tick) >= 0 &&
           frames_advanced < kMaximumCatchUpFrames) {
      redraw = advance_game_loop_frame(window, state, next_simulation_tick) ||
               redraw;
      next_simulation_tick += kSimulationTickMilliseconds;
      ++frames_advanced;
      now = GetTickCount();
    }

    if (redraw && IsWindowVisible(window) != FALSE &&
        IsIconic(window) == FALSE) {
      (void)render_opengl(window, state);
      redraw = false;
    }

    now = GetTickCount();
    const std::int32_t milliseconds_until_tick =
        tick_delta(next_simulation_tick, now);
    const DWORD wait_milliseconds =
        milliseconds_until_tick > 0
            ? static_cast<DWORD>((std::min)(milliseconds_until_tick,
                                           static_cast<std::int32_t>(50)))
            : 0U;
    (void)MsgWaitForMultipleObjectsEx(0, nullptr, wait_milliseconds,
                                      QS_ALLINPUT, MWMO_INPUTAVAILABLE);
  }
}

} // namespace starcraft::recovery
