#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
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
  const GlueControl *const control = control_with_id(
      state.glue.connection_controls, image.control_identifier);
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

std::int16_t connection_control_at(const GlueRuntime &glue, const int x,
                                   const int y) noexcept {
  for (const GlueControl &control : glue.connection_controls) {
    if ((control.type == 14U || control.type == 12U) &&
        point_in_control(control, x, y)) {
      return control.identifier;
    }
  }
  return -1;
}

GlueAction activate_connection_control(GlueRuntime &glue,
                                       const std::int16_t identifier,
                                       const int, const int y,
                                       const std::uint32_t now) noexcept {
  if (identifier == 10) { // Cancel
    return glues_leave_screen(glue, GlueScreen::main_menu,
                              GlueAction::none, now);
  }
  if (identifier == 5) { // Provider list
    const GlueControl *const list =
        control_with_id(glue.connection_controls, 5);
    if (list != nullptr) {
      constexpr int row_height = 28;
      const int row = (y - list->top - 8) / row_height;
      if (row >= 0 &&
          static_cast<std::size_t>(row) < glue.providers.size()) {
        glue.selected_provider = static_cast<std::size_t>(row);
      }
    }
    return GlueAction::redraw;
  }
  if (identifier != 9) { // Okay
    return GlueAction::none;
  }
  if (glue.selected_provider == 0U) {
    return glues_leave_screen(glue, GlueScreen::map_selection,
                              GlueAction::none, now);
  }
  if (glue.selected_provider == 1U) {
    set_message(glue, "The new LAN session transport is the next net task.",
                now);
  } else {
    if (battle::UiBeginConnect(glue.battle_net)) {
      glue.online_lobby = false;
      return glues_leave_screen(glue, GlueScreen::battle_net,
                                GlueAction::none, now);
    }
    set_message(glue, glue.battle_net.status.c_str(), now);
  }
  return GlueAction::redraw;
}

void draw_connection_gl(const RecoveryWindowState &state) noexcept {
  draw_preview_frame_gl(state.glue.connection_background, 0.0F, 0.0F,
                        static_cast<float>(kMapViewportWidth),
                        static_cast<float>(kMapViewportHeight));
  for (const GlueImage &image : state.glue.connection_images) {
    draw_control_image(state, image);
  }

  const GlueControl *const heading =
      control_with_id(state.glue.connection_controls, -1);
  if (heading != nullptr && !heading->text.empty()) {
    draw_glue_centered_text_gl(state, heading->text, *heading, 230U, 230U,
                               230U, false);
  }

  const GlueControl *const list =
      control_with_id(state.glue.connection_controls, 5);
  if (list != nullptr) {
    std::int16_t list_left{};
    std::int16_t list_top{};
    std::int16_t list_right{};
    std::int16_t list_bottom{};
    glues_control_rect(state.glue, *list, list_left, list_top, list_right,
                       list_bottom);
    constexpr float row_height = 28.0F;
    const float selected_top =
        static_cast<float>(list_top + 8) +
        row_height * static_cast<float>(state.glue.selected_provider);
    glDisable(GL_TEXTURE_2D);
    glColor4ub(48U, 92U, 144U, 150U);
    glBegin(GL_QUADS);
    glVertex2f(static_cast<float>(list_left + 5),
               selected_top * hud_vertical_scale());
    glVertex2f(static_cast<float>(list_right - 5),
               selected_top * hud_vertical_scale());
    glVertex2f(static_cast<float>(list_right - 5),
               (selected_top + 22.0F) * hud_vertical_scale());
    glVertex2f(static_cast<float>(list_left + 5),
               (selected_top + 22.0F) * hud_vertical_scale());
    glEnd();
    glColor4ub(255U, 255U, 255U, 255U);
    glEnable(GL_TEXTURE_2D);
    for (std::size_t index = 0; index < state.glue.providers.size(); ++index) {
      const bool selected = index == state.glue.selected_provider;
      draw_glue_text_gl(
          state, state.glue.providers[index], static_cast<float>(list_left + 12),
          static_cast<float>(list_top + 24) +
              row_height * static_cast<float>(index),
          selected ? 255U : 205U, selected ? 230U : 205U,
          selected ? 128U : 205U, false);
    }
  }

  const GlueControl *const provider_name =
      control_with_id(state.glue.connection_controls, 6);
  if (provider_name != nullptr) {
    draw_glue_centered_text_gl(
        state, state.glue.providers[state.glue.selected_provider],
        *provider_name, 255U, 220U, 96U, true);
  }
  const GlueControl *const description =
      control_with_id(state.glue.connection_controls, 8);
  if (description != nullptr) {
    std::int16_t description_left{};
    std::int16_t description_top{};
    std::int16_t description_right{};
    std::int16_t description_bottom{};
    glues_control_rect(state.glue, *description, description_left,
                       description_top, description_right,
                       description_bottom);
    draw_glue_text_gl(
        state,
        state.glue.provider_descriptions[state.glue.selected_provider],
        static_cast<float>(description_left + 4),
        static_cast<float>(description_top + 18), 220U, 220U, 220U, false);
  }
  for (const std::int16_t identifier : {std::int16_t{9}, std::int16_t{10}}) {
    const GlueControl *const button =
        control_with_id(state.glue.connection_controls, identifier);
    if (button == nullptr) {
      continue;
    }
    const bool hovered = state.glue.hovered_control == identifier;
    const bool pressed = state.glue.pressed_control == identifier;
    draw_glue_centered_text_gl(
        state, button->text, *button, pressed ? 255U : hovered ? 255U : 220U,
        pressed ? 128U : hovered ? 224U : 220U,
        pressed ? 48U : hovered ? 96U : 220U, false);
  }
}

} // namespace starcraft::recovery
