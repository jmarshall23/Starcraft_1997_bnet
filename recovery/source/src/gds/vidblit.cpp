#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

bool render_opengl(const HWND window, RecoveryWindowState &state) noexcept {
  if (state.device_context == nullptr || state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    return false;
  }
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 ||
      client.bottom <= 0) {
    return false;
  }
  while (glGetError() != GL_NO_ERROR) {
  }
  const PresentationViewport viewport =
      presentation_viewport(client.right, client.bottom);
  if (viewport.width <= 0 || viewport.height <= 0) {
    return false;
  }
  glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, kMapViewportWidth, kMapViewportHeight, 0.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  const BootstrapStatus *const status = state.status;
  if (glue_active(state.glue)) {
    (void)render_glue(state);
  } else if (status != nullptr && status->terrain_ready) {
    draw_preview_frame_gl(status->terrain, 0.0F, 0.0F,
                          static_cast<float>(kMapViewportWidth),
                          static_cast<float>(kMapViewportHeight));
    for (const ScenarioUnitPreview &unit : status->units) {
      draw_scenario_unit_gl(*status, unit);
    }
    draw_building_placement_gl(*status);
    draw_selection_drag_gl(state);
    if (status->hud_ready) {
      draw_preview_frame_gl(status->hud, 0.0F, 0.0F,
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
    draw_hud_menu_button_gl(state);
    draw_game_dialog_gl(state, GetTickCount());
  }
  glFlush();
  std::array<std::array<GLint, 2>, 4> sample_points{{
      {{(std::clamp)(viewport.x + viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        viewport.height / 4}},
      {{(std::clamp)(viewport.x + 3 * viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        viewport.height / 4}},
      {{(std::clamp)(viewport.x + viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        3 * viewport.height / 4}},
      {{(std::clamp)(viewport.x + 3 * viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        3 * viewport.height / 4}},
  }};
  bool has_colored_pixel{};
  glReadBuffer(GL_BACK);
  for (const auto &point : sample_points) {
    std::array<GLubyte, 4> pixel{};
    glReadPixels(point[0], point[1], 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                 pixel.data());
    has_colored_pixel =
        has_colored_pixel || pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0;
  }
  const bool commands_valid = glGetError() == GL_NO_ERROR;
  const bool decoded_score_surface =
      state.game_dialog.screen == GameDialogScreen::score &&
      state.status != nullptr && state.status->local_race < 3U &&
      !state.game_dialog
           .score_backgrounds[2U * state.status->local_race +
                              (state.game_dialog.outcome ==
                                       MatchOutcome::victory
                                   ? 1U
                                   : 0U)]
           .bgra.empty();
  return SwapBuffers(state.device_context) != FALSE && commands_valid &&
         (has_colored_pixel || decoded_score_surface);
}

} // namespace starcraft::recovery
