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
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
  glues_control_rect(state.glue, *control, left, top, right, bottom);
  draw_preview_frame_gl(
      image.frame, static_cast<float>(left),
      static_cast<float>(top) * hud_vertical_scale(),
      static_cast<float>(right - left + 1),
      static_cast<float>(bottom - top + 1) * hud_vertical_scale());
}

void draw_control_videos(const RecoveryWindowState &state,
                         const GlueControl &control) noexcept {
  const bool highlighted =
      state.glue.hovered_control == control.identifier ||
      state.glue.pressed_control == control.identifier;
  for (const GlueVideo &video : state.glue.main_videos) {
    if (video.control_identifier != control.identifier ||
        !video.animation.ready ||
        ((video.descriptor_flags & 8U) != 0U && !highlighted)) {
      continue;
    }
    std::int16_t left{};
    std::int16_t top{};
    std::int16_t right{};
    std::int16_t bottom{};
    glues_control_rect(state.glue, control, left, top, right, bottom);
    draw_preview_frame_gl(
        video.animation.frame,
        static_cast<float>(left + video.x_offset),
        static_cast<float>(top + video.y_offset) * hud_vertical_scale(),
        static_cast<float>(video.animation.frame.width),
        static_cast<float>(video.animation.frame.height) *
            hud_vertical_scale());
  }
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
    glues_enter_screen(glue, GlueScreen::connection, now);
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
  // gluMain.bin interleaves PCX and SMK controls. Preserve that order: eTail
  // sits below Exit.smk while the intro/credits panels sit above the large
  // menu movies.
  for (const GlueControl &control : state.glue.main_controls) {
    if (control.type == 5U) {
      for (const GlueImage &image : state.glue.main_images) {
        if (image.control_identifier == control.identifier) {
          draw_control_image(state, image);
          break;
        }
      }
    } else if (control.type == 14U) {
      draw_control_videos(state, control);
    }
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
