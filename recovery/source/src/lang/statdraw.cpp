#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace starcraft::recovery {

void draw_status_text_gl(const RecoveryWindowState &state,
                         const CommandControl &control,
                         const std::string_view text, const std::uint8_t red,
                         const std::uint8_t green, const std::uint8_t blue) {
  if (state.font_display_lists == 0 || text.empty()) {
    return;
  }
  const std::size_t maximum_characters = static_cast<std::size_t>(
      (std::max)(1, (control.right - control.left + 1) / 6));
  std::string printable = printable_status_text(text);
  if (printable.size() > maximum_characters) {
    printable.resize(maximum_characters);
  }
  draw_game_text_gl(state, printable, static_cast<float>(control.left),
                    static_cast<float>(control.top) * hud_vertical_scale() +
                        9.0F,
                    red, green, blue, false);
}

void draw_system_message_gl(const RecoveryWindowState &state,
                            const std::uint32_t now) {
  const BootstrapStatus *const status = state.status;
  if (status == nullptr || status->system_message.empty() ||
      static_cast<std::int32_t>(status->system_message_until - now) <= 0) {
    return;
  }
  // mapmsg.cpp::sub_46C5C0 invalidates the 630-pixel-wide system-message row
  // at logical y=291 and expires it after seven seconds.
  CommandControl shadow{};
  shadow.left = 13;
  shadow.top = 292;
  shadow.right = 629;
  shadow.bottom = 306;
  draw_status_text_gl(state, shadow, status->system_message, 0, 0, 0);
  --shadow.left;
  --shadow.top;
  --shadow.right;
  --shadow.bottom;
  draw_status_text_gl(state, shadow, status->system_message, 255, 72, 48);
}

} // namespace starcraft::recovery
