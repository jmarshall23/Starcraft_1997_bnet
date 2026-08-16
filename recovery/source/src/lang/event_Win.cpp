#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>

namespace starcraft::recovery {

namespace {

void apply_glue_action(const HWND window, RecoveryWindowState *const state,
                       const GlueAction action) noexcept {
  if (action == GlueAction::redraw) {
    InvalidateRect(window, nullptr, FALSE);
  } else if (action == GlueAction::start_game && state != nullptr) {
    (void)start_selected_glue_map(*state);
    InvalidateRect(window, nullptr, FALSE);
  } else if (action == GlueAction::quit) {
    PostMessageA(window, WM_CLOSE, 0, 0);
  }
}

void apply_game_dialog_action(const HWND window,
                              RecoveryWindowState *const state,
                              const GameDialogAction action) noexcept {
  if (state == nullptr || action == GameDialogAction::none) {
    return;
  }
  if (action == GameDialogAction::redraw) {
    InvalidateRect(window, nullptr, FALSE);
  } else if (action == GameDialogAction::restart_match) {
    (void)start_selected_glue_map(*state);
    InvalidateRect(window, nullptr, FALSE);
  } else if (action == GameDialogAction::return_to_menu) {
    state->game_dialog.screen = GameDialogScreen::none;
    state->game_dialog.match_active = false;
    state->game_dialog.paused = false;
    state->game_dialog.observer_mode = false;
    state->glue.screen = GlueScreen::main_menu;
    state->glue.screen_entered_tick = GetTickCount();
    state->glue.hovered_control = -1;
    state->glue.pressed_control = -1;
    if (state->music_playing) {
      alSourceStop(state->music_source);
      state->music_playing = false;
    }
    (void)play_title_music(*state);
    InvalidateRect(window, nullptr, FALSE);
  } else if (action == GameDialogAction::quit_program) {
    PostMessageA(window, WM_CLOSE, 0, 0);
  }
}

} // namespace

bool client_to_game(const HWND window, const LPARAM lparam, int &game_x,
                    int &game_y) noexcept {
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 ||
      client.bottom <= 0) {
    return false;
  }
  const int client_x = static_cast<short>(LOWORD(lparam));
  const int client_y = static_cast<short>(HIWORD(lparam));
  game_x = client_x * kMapViewportWidth / client.right;
  game_y = client_y * kMapViewportHeight / client.bottom;
  return game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
         game_y < kMapViewportHeight;
}

bool hud_pixel_opaque(const BootstrapStatus &status, const int game_x,
                      const int game_y) noexcept {
  if (!status.hud_ready || game_x < 0 || game_y < 0 ||
      game_x >= kMapViewportWidth || game_y >= kMapViewportHeight) {
    return false;
  }
  const std::size_t source_x =
      static_cast<std::size_t>(game_x) * status.hud.width / kMapViewportWidth;
  const std::size_t source_y =
      static_cast<std::size_t>(game_y) * status.hud.height / kMapViewportHeight;
  const std::size_t pixel = source_y * status.hud.width + source_x;
  return pixel < status.hud.bgra.size() &&
         (status.hud.bgra[pixel] & 0xFF000000U) != 0;
}

LRESULT CALLBACK recovery_window_proc(const HWND window, const UINT message,
                                      const WPARAM wparam,
                                      const LPARAM lparam) {
  switch (message) {
  case WM_NCCREATE: {
    const auto *const create = reinterpret_cast<const CREATESTRUCTA *>(lparam);
    SetWindowLongPtrA(window, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    return TRUE;
  }
  case WM_CREATE: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state == nullptr || !initialize_opengl(window, *state)) {
      return -1;
    }
    // Audio failure does not make the render window unusable; the dedicated
    // audio regression probe still requires a live OpenAL device.
    (void)initialize_audio(*state);
    if (glue_active(state->glue)) {
      (void)play_title_music(*state);
    }
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr) {
      (void)render_opengl(window, *state);
    }
    EndPaint(window, &paint);
    return 0;
  }
  case WM_LBUTTONDOWN: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && glue_active(state->glue)) {
      int glue_x{};
      int glue_y{};
      if (client_to_glue(window, lparam, glue_x, glue_y)) {
        const GlueAction action =
            glue_left_down(state->glue, glue_x, glue_y);
        if (action == GlueAction::redraw) {
          SetCapture(window);
        }
        apply_glue_action(window, state, action);
      }
      return 0;
    }
    int game_x{};
    int game_y{};
    const bool game_position = state != nullptr && state->status != nullptr &&
                               client_to_game(window, lparam, game_x, game_y);
    if (game_position && game_dialog_active(*state)) {
      const GameDialogAction action =
          game_dialog_left_down(*state, game_x, game_y);
      if (action == GameDialogAction::redraw) {
        SetCapture(window);
      }
      apply_game_dialog_action(window, state, action);
      return 0;
    }
    if (game_position && hud_menu_button_at(*state, game_x, game_y)) {
      open_game_menu(*state);
      InvalidateRect(window, nullptr, FALSE);
      return 0;
    }
    if (state != nullptr &&
        (state->game_dialog.paused || state->game_dialog.observer_mode)) {
      return 0;
    }
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
          if (queue_unit_response(*state->status,
                                  state->status->units[selected_unit], false)) {
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
      const int map_right =
          static_cast<int>(state->status->scenario_width) * 32 - 1;
      const int map_bottom =
          static_cast<int>(state->status->scenario_height) * 32 - 1;
      const int world_x = game_x + state->status->camera_x;
      const int world_y = game_y + state->status->camera_y;
      const std::size_t issued = issue_active_scv_target(
          *state->status,
          static_cast<std::uint16_t>((std::clamp)(world_x, 0, map_right)),
          static_cast<std::uint16_t>((std::clamp)(world_y, 0, map_bottom)));
      const ScenarioUnitPreview *const speaker =
          first_selected_unit(*state->status);
      if (issued != 0U && speaker != nullptr &&
          queue_unit_response(*state->status, *speaker, true)) {
        (void)play_pending_game_sound(*state);
      }
      InvalidateRect(window, nullptr, FALSE);
      return 0;
    }
    if (game_position && !hud_pixel_opaque(*state->status, game_x, game_y)) {
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && glue_active(state->glue)) {
      int glue_x{};
      int glue_y{};
      if (client_to_glue(window, lparam, glue_x, glue_y)) {
        if (!state->mouse_in_client) {
          state->mouse_in_client = true;
          TRACKMOUSEEVENT tracking{};
          tracking.cbSize = sizeof(tracking);
          tracking.dwFlags = TME_LEAVE;
          tracking.hwndTrack = window;
          TrackMouseEvent(&tracking);
        }
        apply_glue_action(window, state,
                          glue_mouse_move(state->glue, glue_x, glue_y));
      }
      return 0;
    }
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
      if (game_dialog_active(*state)) {
        apply_game_dialog_action(
            window, state, game_dialog_mouse_move(*state, game_x, game_y));
        return 0;
      }
      if (state->game_dialog.paused || state->game_dialog.observer_mode) {
        return 0;
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr) {
      if (glue_active(state->glue)) {
        apply_glue_action(window, state,
                          glue_mouse_move(state->glue, -1, -1));
      } else if (game_dialog_active(*state)) {
        apply_game_dialog_action(
            window, state, game_dialog_mouse_move(*state, -1, -1));
      }
      state->mouse_in_client = false;
      state->camera_scroll_ramp = 0;
    }
    return 0;
  }
  case WM_RBUTTONDOWN: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    int game_x{};
    int game_y{};
    if (state != nullptr && state->status != nullptr) {
      if (game_dialog_active(*state) || state->game_dialog.paused ||
          state->game_dialog.observer_mode) {
        return 0;
      }
      BootstrapStatus &status = *state->status;
      if (status.placement_active || status.command_target_active ||
          status.active_command_card != 0) {
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
        status.nydus_parent_id = 0U;
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
        const int map_bottom =
            static_cast<int>(status.scenario_height) * 32 - 1;
        const std::size_t issued = issue_scv_smart_order(
            status,
            static_cast<std::uint16_t>((std::clamp)(world_x, 0, map_right)),
            static_cast<std::uint16_t>((std::clamp)(world_y, 0, map_bottom)));
        const ScenarioUnitPreview *const speaker = first_selected_unit(status);
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && glue_active(state->glue)) {
      apply_glue_action(window, state,
                        glue_key_down(state->glue, wparam, GetTickCount()));
      return 0;
    }
    if (state != nullptr &&
        (game_dialog_active(*state) || wparam == VK_F10)) {
      apply_game_dialog_action(
          window, state,
          game_dialog_key_down(*state, wparam, GetTickCount()));
      return 0;
    }
    if (wparam == VK_ESCAPE && state != nullptr && state->status != nullptr) {
      const bool cancelled = state->status->placement_active ||
                             state->status->command_target_active ||
                             state->status->active_command_card != 0;
      state->status->placement_active = false;
      state->status->placement_valid = false;
      state->status->placement_unit_type = 0xFFFFU;
      state->status->nydus_parent_id = 0U;
      state->status->active_command_card = 0;
      cancel_command_target(*state->status);
      if (!cancelled) {
        open_game_menu(*state);
      }
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && camera_input_key(wparam) &&
        wparam < state->keys_down.size()) {
      state->keys_down[static_cast<std::size_t>(wparam)] = false;
      return 0;
    }
    return DefWindowProcA(window, message, wparam, lparam);
  }
  case WM_KILLFOCUS: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr) {
      state->keys_down.fill(false);
      state->camera_scroll_ramp = 0;
    }
    return 0;
  }
  case WM_LBUTTONUP: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && glue_active(state->glue)) {
      int glue_x{-1};
      int glue_y{-1};
      (void)client_to_glue(window, lparam, glue_x, glue_y);
      const GlueAction action =
          glue_left_up(state->glue, glue_x, glue_y, GetTickCount());
      if (GetCapture() == window) {
        ReleaseCapture();
      }
      apply_glue_action(window, state, action);
      return 0;
    }
    int game_x{};
    int game_y{};
    if (state != nullptr && game_dialog_active(*state)) {
      (void)client_to_game(window, lparam, game_x, game_y);
      const GameDialogAction action = game_dialog_left_up(
          *state, game_x, game_y, GetTickCount());
      if (GetCapture() == window) {
        ReleaseCapture();
      }
      apply_game_dialog_action(window, state, action);
      return 0;
    }
    if (state != nullptr && state->minimap_dragging) {
      if (state->status != nullptr &&
          client_to_game(window, lparam, game_x, game_y)) {
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
      if (state->status != nullptr &&
          client_to_game(window, lparam, game_x, game_y) &&
          command_position_at(*state->status, game_x, game_y) == pressed) {
        activate_command_button(*state->status, pressed);
        (void)play_pending_resource_error(*state);
        const ScenarioUnitPreview *const speaker =
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && reinterpret_cast<HWND>(lparam) != window) {
      state->glue.pressed_control = -1;
      state->game_dialog.pressed_control = -1;
      state->selection_dragging = false;
      state->minimap_dragging = false;
      state->pressed_command_position = 0;
    }
    return 0;
  }
  case WM_TIMER: {
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (state != nullptr && glue_active(state->glue)) {
      apply_glue_action(window, state,
                        advance_glue(state->glue, GetTickCount()));
      return 0;
    }
    if (state != nullptr && state->game_dialog.paused) {
      const std::uint32_t now = GetTickCount();
      if (state->game_dialog.screen == GameDialogScreen::defeat &&
          now - state->game_dialog.result_started_tick >= 20000U) {
        build_match_scores(*state);
        state->game_dialog.score_started_tick = now;
        state->game_dialog.screen = GameDialogScreen::score;
        state->game_dialog.hovered_control = -1;
        state->game_dialog.pressed_control = -1;
      } else if (state->game_dialog.screen == GameDialogScreen::score &&
                 now - state->game_dialog.score_started_tick >= 120000U) {
        apply_game_dialog_action(window, state,
                                 GameDialogAction::return_to_menu);
        return 0;
      }
      InvalidateRect(window, nullptr, FALSE);
      return 0;
    }
    BootstrapStatus *const status = state == nullptr ? nullptr : state->status;
    if (status != nullptr && !status->unit_assets.empty() &&
        !status->units.empty()) {
      const starcraft::lang::IScriptProgramView program{
          status->iscript_bytes.data(), status->iscript_bytes.size()};
      const std::uint32_t clock = GetTickCount();
      (void)advance_camera_scroll(*state);
      (void)advance_zerg_larvae(*status);
      (void)advance_unit_production(*status, clock);
      (void)advance_technology_research(*status);
      advance_resource_display(*status);
      (void)advance_unit_movement(*status);
      const bool unit_actions_changed = advance_unit_actions(*status);
      const bool addon_construction_changed =
          advance_addon_construction(*status);
      const bool protoss_construction_changed =
          advance_protoss_building_construction(*status);
      const bool zerg_construction_changed =
          advance_zerg_building_construction(*status);
      (void)play_pending_resource_error(*state);
      (void)play_pending_game_sound(*state);
      evaluate_melee_outcome(*state);
      if (unit_actions_changed || addon_construction_changed ||
          protoss_construction_changed || zerg_construction_changed) {
        (void)rebuild_creep_tiles(*status);
      }
      (void)advance_selected_portrait(*status, clock);
      for (std::size_t index = 0; index < status->units.size(); ++index) {
        ScenarioUnitPreview &unit = status->units[index];
        if (!unit.alive || !unit.iscript_ready ||
            unit.asset_index >= status->unit_assets.size()) {
          continue;
        }
        const UnitRenderAsset &asset = status->unit_assets[unit.asset_index];
        const std::uint32_t previous_weapon_events =
            unit.iscript_state.weapon_event_count;
        const std::uint32_t previous_velocity_events =
            unit.iscript_state.flingy_velocity_event_count;
        const auto result = program.tick(
            unit.iscript_state,
            clock ^ static_cast<std::uint32_t>(index * 0x9E3779B9U), 256,
            nullptr, status->scenario.tileset_id());
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
            status->pending_game_sound = choose_unit_sound(*status, 23U, 27U);
          }
        }
        if (unit.overlay_ready && !asset.overlay_frames.empty()) {
          const auto overlay_result = program.tick(
              unit.overlay_iscript_state,
              clock ^ static_cast<std::uint32_t>(index * 0x85EBCA6BU) ^
                  0x5A5A5A5AU,
              256, &unit.iscript_state, status->scenario.tileset_id());
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
          const auto dynamic_result = program.tick(
              unit.dynamic_overlay_iscript_state,
              clock ^ static_cast<std::uint32_t>(index * 0xC2B2AE35U) ^
                  0x3C3C3C3CU,
              256, &unit.iscript_state, status->scenario.tileset_id());
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
    auto *const state = reinterpret_cast<RecoveryWindowState *>(
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

} // namespace starcraft::recovery
