#include "../platform/bootstrap_runtime.hpp"

namespace starcraft::recovery {

void draw_command_target_gl(const RecoveryWindowState &state) noexcept {
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

} // namespace starcraft::recovery
