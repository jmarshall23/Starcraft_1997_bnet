#include "../platform/bootstrap_runtime.hpp"

#include <cstdint>

namespace starcraft::recovery {
namespace {

const GlueControl *control_with_id(const std::vector<GlueControl> &controls,
                                   const std::int16_t identifier) noexcept {
  for (const GlueControl &control : controls) {
    if (control.identifier == identifier) {
      return &control;
    }
  }
  return nullptr;
}

bool point_in_control(const GlueControl &control, const int x,
                      const int y) noexcept {
  return x >= control.left && x <= control.right && y >= control.top &&
         y <= control.bottom;
}

void draw_control_image(const RecoveryWindowState &state,
                        const GlueImage &image) noexcept {
  const GlueControl *const control =
      control_with_id(state.glue.main_controls, image.control_identifier);
  if (control == nullptr) {
    return;
  }
  draw_preview_frame_gl(
      image.frame, static_cast<float>(control->left),
      static_cast<float>(control->top) * hud_vertical_scale(),
      static_cast<float>(control->right - control->left + 1),
      static_cast<float>(control->bottom - control->top + 1) *
          hud_vertical_scale());
}

void set_message(GlueRuntime &glue, const char *const message,
                 const std::uint32_t now) noexcept {
  try {
    glue.message = message;
    glue.message_until = now + 4500U;
  } catch (...) {
    glue.message.clear();
    glue.message_until = 0U;
  }
}

} // namespace

std::int16_t main_menu_control_at(const GlueRuntime &glue, const int x,
                                  const int y) noexcept {
  for (const GlueControl &control : glue.main_controls) {
    if (control.type == 14U && point_in_control(control, x, y)) {
      return control.identifier;
    }
  }
  return -1;
}

GlueAction activate_main_menu_control(GlueRuntime &glue,
                                      const std::int16_t identifier,
                                      const std::uint32_t now) noexcept {
  switch (identifier) {
  case 2: // Exit
    return GlueAction::quit;
  case 3: // Single Player
    set_message(glue,
                "This feature has been disabled for the Battle.net beta.",
                now);
    return GlueAction::redraw;
  case 4: // Multiplayer
    glue.screen = GlueScreen::connection;
    glue.screen_entered_tick = now;
    glue.hovered_control = -1;
    glue.pressed_control = -1;
    glue.message.clear();
    glue.message_until = 0U;
    return GlueAction::redraw;
  case 5: // Campaign Editor
    set_message(glue,
                "This feature has been disabled for the Battle.net beta.",
                now);
    return GlueAction::redraw;
  case 8: // View Intro
    set_message(glue, "This feature has been disabled for the Battle.net beta.", now);
    return GlueAction::redraw;
  case 9: // Show Credits
    set_message(glue, "This feature has been disabled for the Battle.net beta.", now);
    return GlueAction::redraw;
  default:
    return GlueAction::none;
  }
}

void draw_main_menu_gl(const RecoveryWindowState &state) noexcept {
  draw_preview_frame_gl(state.glue.main_background, 0.0F, 0.0F,
                        static_cast<float>(kMapViewportWidth),
                        static_cast<float>(kMapViewportHeight));
  for (const GlueImage &image : state.glue.main_images) {
    draw_control_image(state, image);
  }
  for (const GlueControl &control : state.glue.main_controls) {
    if (control.type != 14U || control.text.empty()) {
      continue;
    }
    const bool hovered = state.glue.hovered_control == control.identifier;
    const bool pressed = state.glue.pressed_control == control.identifier;
    const std::uint8_t red = pressed ? 255U : hovered ? 255U : 208U;
    const std::uint8_t green = pressed ? 128U : hovered ? 224U : 208U;
    const std::uint8_t blue = pressed ? 48U : hovered ? 96U : 208U;
    draw_glue_centered_text_gl(state, control.text, control, red, green, blue,
                               control.identifier >= 2 &&
                                   control.identifier <= 5);
  }
}

} // namespace starcraft::recovery
