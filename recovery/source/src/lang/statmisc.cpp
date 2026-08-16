#include "../platform/bootstrap_runtime.hpp"

namespace starcraft::recovery {
namespace {

const GlueControl *menu_control(const RecoveryWindowState &state) noexcept {
  for (const GlueControl &control : state.game_dialog.hud_menu_controls) {
    if (control.identifier == 1) {
      return &control;
    }
  }
  return nullptr;
}

} // namespace

bool hud_menu_button_at(const RecoveryWindowState &state, const int x,
                        const int game_y) noexcept {
  if (!state.game_dialog.assets_ready ||
      (!state.game_dialog.match_active && !state.game_dialog.observer_mode) ||
      state.game_dialog.screen == GameDialogScreen::victory ||
      state.game_dialog.screen == GameDialogScreen::defeat ||
      state.game_dialog.screen == GameDialogScreen::score) {
    return false;
  }
  const GlueControl *const control = menu_control(state);
  if (control == nullptr) {
    return false;
  }
  const int y = static_cast<int>(static_cast<float>(game_y) /
                                 hud_vertical_scale());
  return x >= control->left && x <= control->right && y >= control->top &&
         y <= control->bottom;
}

void draw_hud_menu_button_gl(const RecoveryWindowState &state) noexcept {
  const GlueControl *const control = menu_control(state);
  if (control == nullptr ||
      (!state.game_dialog.match_active && !state.game_dialog.observer_mode)) {
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(15U, 25U, 30U, 238U);
  glBegin(GL_QUADS);
  glVertex2f(static_cast<float>(control->left),
             static_cast<float>(control->top) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->right),
             static_cast<float>(control->top) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->right),
             static_cast<float>(control->bottom) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->left),
             static_cast<float>(control->bottom) * hud_vertical_scale());
  glEnd();
  glBegin(GL_LINE_LOOP);
  glColor4ub(72U, 128U, 144U, 255U);
  glVertex2f(static_cast<float>(control->left),
             static_cast<float>(control->top) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->right),
             static_cast<float>(control->top) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->right),
             static_cast<float>(control->bottom) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control->left),
             static_cast<float>(control->bottom) * hud_vertical_scale());
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
  draw_glue_centered_text_gl(state, "MENU", *control, 232U, 216U, 144U,
                             false);
}

} // namespace starcraft::recovery
