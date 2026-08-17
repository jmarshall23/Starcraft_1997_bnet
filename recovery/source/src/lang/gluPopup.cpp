#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::uint16_t kGlueWidth = 640U;
constexpr std::uint16_t kGlueHeight = 480U;

const GlueControl *control_with_id(const GlueRuntime &glue,
                                   const std::int16_t identifier) noexcept {
  for (const GlueControl &control : glue.ok_popup_controls) {
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

std::vector<std::string> wrap_popup_message(
    const starcraft::gds::BitmapFont &font, const std::string_view message,
    const float maximum_width) {
  std::vector<std::string> lines;
  std::string line;
  std::size_t cursor{};
  while (cursor < message.size()) {
    while (cursor < message.size() && message[cursor] == ' ') {
      ++cursor;
    }
    const std::size_t word_begin = cursor;
    while (cursor < message.size() && message[cursor] != ' ') {
      ++cursor;
    }
    if (word_begin == cursor) {
      break;
    }
    const std::string_view word = message.substr(word_begin, cursor - word_begin);
    std::string candidate = line;
    if (!candidate.empty()) {
      candidate.push_back(' ');
    }
    candidate.append(word);
    if (!line.empty() && font.text_width(candidate) > maximum_width) {
      lines.push_back(std::move(line));
      line.assign(word);
    } else {
      line = std::move(candidate);
    }
  }
  if (!line.empty()) {
    lines.push_back(std::move(line));
  }
  return lines;
}

void draw_popup_message(const RecoveryWindowState &state,
                        const GlueControl &control) noexcept {
  try {
    const starcraft::gds::BitmapFont &font = state.glue.small_font;
    const float width = static_cast<float>(control.right - control.left + 1);
    const std::vector<std::string> lines =
        wrap_popup_message(font, state.glue.modal_message, width);
    if (lines.empty()) {
      return;
    }
    const float line_height = static_cast<float>(font.maximum_height + 1U);
    const float block_height = line_height * static_cast<float>(lines.size());
    float baseline = static_cast<float>(control.top) +
                     (static_cast<float>(control.bottom - control.top + 1) -
                      block_height) /
                         2.0F +
                     static_cast<float>(font.maximum_height);
    for (const std::string &line : lines) {
      const float line_width = font.text_width(line);
      const float x = (static_cast<float>(control.left + control.right + 1) -
                       line_width) /
                      2.0F;
      draw_glue_styled_text_gl(state, line, x, baseline,
                               glue_control_font_style(control), false);
      baseline += line_height;
    }
  } catch (...) {
  }
}

} // namespace

void show_glue_ok_popup(GlueRuntime &glue,
                        const std::string_view message) noexcept {
  if (glue.modal_popup_visible) {
    return;
  }
  try {
    glue.modal_message.assign(message);
    glue.modal_popup_visible = true;
    glue.hovered_control = -1;
    glue.pressed_control = -1;
  } catch (...) {
    glue.modal_message.clear();
    glue.modal_popup_visible = false;
  }
}

void dismiss_glue_ok_popup(GlueRuntime &glue) noexcept {
  glue.modal_message.clear();
  glue.modal_popup_visible = false;
  glue.hovered_control = -1;
  glue.pressed_control = -1;
}

std::int16_t glue_ok_popup_control_at(const GlueRuntime &glue, const int x,
                                      const int y) noexcept {
  if (!glue.modal_popup_visible) {
    return -1;
  }
  for (const GlueControl &control : glue.ok_popup_controls) {
    if (control.type == 14U && point_in_control(control, x, y)) {
      return control.identifier;
    }
  }
  return -1;
}

void draw_glue_ok_popup_gl(const RecoveryWindowState &state) noexcept {
  if (!state.glue.modal_popup_visible ||
      !state.glue.ok_popup_background.width ||
      !state.glue.ok_popup_background.height) {
    return;
  }
  const float left = static_cast<float>(
      (kGlueWidth - state.glue.ok_popup_background.width) / 2U);
  const float top = static_cast<float>(
      (kGlueHeight - state.glue.ok_popup_background.height) / 2U);
  draw_preview_frame_gl(
      state.glue.ok_popup_background, left, top * hud_vertical_scale(),
      static_cast<float>(state.glue.ok_popup_background.width),
      static_cast<float>(state.glue.ok_popup_background.height) *
          hud_vertical_scale());

  const GlueControl *const message = control_with_id(state.glue, 2);
  if (message != nullptr) {
    draw_popup_message(state, *message);
  }
  const GlueControl *const ok = control_with_id(state.glue, 1);
  if (ok != nullptr) {
    const bool highlighted = state.glue.hovered_control == 1 ||
                             state.glue.pressed_control == 1;
    draw_glue_centered_styled_text_gl(
        state, ok->text, *ok, glue_control_font_style(*ok, highlighted),
        false);
  }
}

} // namespace starcraft::recovery
