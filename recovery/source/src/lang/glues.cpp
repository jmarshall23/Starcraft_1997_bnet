#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::uint16_t kGlueWidth = 640U;
constexpr std::uint16_t kGlueHeight = 480U;
constexpr std::uint32_t kTitleDurationMilliseconds = 1800U;

std::string clean_glue_text(const std::string_view encoded) {
  std::string text;
  const bool has_hotkey_markup =
      encoded.find(static_cast<char>(4)) != std::string_view::npos &&
      encoded.find(static_cast<char>(1)) != std::string_view::npos;
  const std::size_t begin = has_hotkey_markup && !encoded.empty() ? 1U : 0U;
  text.reserve(encoded.size() - begin);
  for (std::size_t index = begin; index < encoded.size(); ++index) {
    const unsigned char value = static_cast<unsigned char>(encoded[index]);
    if (value >= 32U && value < 127U) {
      text.push_back(static_cast<char>(value));
    }
  }
  return text;
}

bool parse_glue_layout_impl(const std::vector<std::uint8_t> &layout,
                            std::vector<GlueControl> &controls) {
  controls.clear();
  if (layout.size() < 70U || read_u16(layout, 34U) != 0U) {
    return false;
  }
  const std::int16_t root_x =
      static_cast<std::int16_t>(read_u16(layout, 4U));
  const std::int16_t root_y =
      static_cast<std::int16_t>(read_u16(layout, 6U));
  std::uint32_t offset = read_u32(layout, 66U);
  std::size_t visited{};
  try {
    while (offset != 0U && offset + 70U <= layout.size() &&
           visited++ < 256U) {
      GlueControl control{};
      control.identifier =
          static_cast<std::int16_t>(read_u16(layout, offset + 32U));
      control.type = read_u16(layout, offset + 34U);
      control.left = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 4U)));
      control.top = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 6U)));
      control.right = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 8U)));
      control.bottom = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 10U)));
      control.flags = read_u32(layout, offset + 24U);
      const std::uint32_t text_offset = read_u32(layout, offset + 20U);
      if (text_offset != 0U && text_offset < layout.size()) {
        const char *const first =
            reinterpret_cast<const char *>(layout.data() + text_offset);
        const std::size_t available = layout.size() - text_offset;
        const void *const terminator = std::memchr(first, 0, available);
        if (terminator == nullptr) {
          return false;
        }
        const auto *const last = static_cast<const char *>(terminator);
        control.text = clean_glue_text(
            std::string_view{first, static_cast<std::size_t>(last - first)});
      }
      if (control.right < control.left || control.bottom < control.top) {
        return false;
      }
      controls.push_back(std::move(control));
      const std::uint32_t next = read_u32(layout, offset);
      if (next == offset) {
        return false;
      }
      offset = next;
    }
  } catch (...) {
    controls.clear();
    return false;
  }
  return offset == 0U && !controls.empty();
}

bool load_glue_pcx(starcraft::runtime::StormModule &storm,
                   const char *const path, const bool transparent,
                   SpritePreviewFrame &frame) {
  starcraft::runtime::DecodedPcx image{};
  return storm.load_pcx(path, image) &&
         decode_pcx_frame(image, transparent, frame);
}

bool load_control_images(starcraft::runtime::StormModule &storm,
                         const std::vector<GlueControl> &controls,
                         std::vector<GlueImage> &images) {
  images.clear();
  try {
    for (const GlueControl &control : controls) {
      if (control.type != 5U || control.text.empty()) {
        continue;
      }
      GlueImage image{};
      image.control_identifier = control.identifier;
      if (!load_glue_pcx(storm, control.text.c_str(), true, image.frame)) {
        return false;
      }
      images.push_back(std::move(image));
    }
  } catch (...) {
    images.clear();
    return false;
  }
  return true;
}

void enter_screen(GlueRuntime &glue, const GlueScreen screen,
                  const std::uint32_t now) noexcept {
  glue.screen = screen;
  glue.screen_entered_tick = now;
  glue.hovered_control = -1;
  glue.pressed_control = -1;
  glue.message.clear();
  glue.message_until = 0U;
}

void draw_message(const RecoveryWindowState &state) noexcept {
  if (state.glue.message.empty()) {
    return;
  }
  draw_glue_text_gl(state, state.glue.message, 28.0F, 468.0F, 255U, 208U,
                    64U, false);
}

std::int16_t control_at(const GlueRuntime &glue, const int x,
                        const int y) noexcept {
  switch (glue.screen) {
  case GlueScreen::main_menu:
    return main_menu_control_at(glue, x, y);
  case GlueScreen::connection:
    return connection_control_at(glue, x, y);
  case GlueScreen::map_selection:
    return map_selection_control_at(glue, x, y);
  case GlueScreen::lobby:
    return lobby_control_at(glue, x, y);
  default:
    return -1;
  }
}

} // namespace

bool parse_glue_layout(const std::vector<std::uint8_t> &layout,
                       std::vector<GlueControl> &controls) noexcept {
  return parse_glue_layout_impl(layout, controls);
}

bool initialize_glue_assets(GlueRuntime &glue) noexcept {
  glue = {};
  const std::filesystem::path root = locate_input_root();
  if (root.empty()) {
    glue.failure = "The StarCraft input directory was not found.";
    return false;
  }
  starcraft::runtime::StormModule storm{root / L"storm.dll"};
  if (!storm.loaded()) {
    glue.failure = "The supplied storm.dll could not be loaded.";
    return false;
  }
  void *base{};
  void *patch{};
  if (!storm.open_archive(root / L"StarDat.mpq", &base, 1000U) ||
      !storm.open_archive(root / L"patch_rt.mpq", &patch, 2000U)) {
    if (patch != nullptr) {
      (void)storm.close_archive(patch);
    }
    if (base != nullptr) {
      (void)storm.close_archive(base);
    }
    glue.failure = "The StarCraft data archives could not be opened.";
    return false;
  }

  std::vector<std::uint8_t> main_layout;
  std::vector<std::uint8_t> connection_layout;
  std::vector<std::uint8_t> lobby_layout;
  const bool loaded =
      load_glue_pcx(storm, R"(glue\title\title-beta.pcx)", false,
                    glue.title_background) &&
      load_glue_pcx(storm, R"(glue\PalMm\BackGnd.pcx)", false,
                    glue.main_background) &&
      load_glue_pcx(storm, R"(glue\PalNl\BackGnd.pcx)", false,
                    glue.connection_background) &&
      storm.load_file(R"(rez\gluMain.bin)", main_layout) &&
      storm.load_file(R"(rez\gluConn.bin)", connection_layout) &&
      storm.load_file(R"(rez\gluChat.bin)", lobby_layout) &&
      parse_glue_layout(main_layout, glue.main_controls) &&
      parse_glue_layout(connection_layout, glue.connection_controls) &&
      parse_glue_layout(lobby_layout, glue.lobby_controls) &&
      load_control_images(storm, glue.main_controls, glue.main_images) &&
      load_control_images(storm, glue.connection_controls,
                          glue.connection_images) &&
      load_control_images(storm, glue.lobby_controls, glue.lobby_images) &&
      enumerate_glue_maps(storm, root, glue);
  const bool patch_closed = storm.close_archive(patch);
  const bool base_closed = storm.close_archive(base);
  glue.assets_ready = loaded && patch_closed && base_closed;
  if (!glue.assets_ready) {
    glue.failure = "A title, glue dialog, control image, or multiplayer map "
                   "failed to decode.";
    glue.screen = GlueScreen::gameplay;
    return false;
  }
  configure_lobby_slots(glue);
  enter_screen(glue, GlueScreen::title, GetTickCount());
  return true;
}

bool glue_active(const GlueRuntime &glue) noexcept {
  return glue.assets_ready && glue.screen != GlueScreen::gameplay;
}

bool client_to_glue(const HWND window, const LPARAM lparam, int &glue_x,
                    int &glue_y) noexcept {
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 ||
      client.bottom <= 0) {
    return false;
  }
  const int client_x = static_cast<short>(LOWORD(lparam));
  const int client_y = static_cast<short>(HIWORD(lparam));
  glue_x = client_x * kGlueWidth / client.right;
  glue_y = client_y * kGlueHeight / client.bottom;
  return glue_x >= 0 && glue_y >= 0 && glue_x < kGlueWidth &&
         glue_y < kGlueHeight;
}

GlueAction glue_mouse_move(GlueRuntime &glue, const int x,
                           const int y) noexcept {
  const std::int16_t previous = glue.hovered_control;
  switch (glue.screen) {
  case GlueScreen::main_menu:
    glue.hovered_control = main_menu_control_at(glue, x, y);
    break;
  case GlueScreen::connection:
    glue.hovered_control = connection_control_at(glue, x, y);
    break;
  case GlueScreen::map_selection:
    glue.hovered_control = map_selection_control_at(glue, x, y);
    break;
  case GlueScreen::lobby:
    glue.hovered_control = lobby_control_at(glue, x, y);
    break;
  default:
    glue.hovered_control = -1;
    break;
  }
  return previous == glue.hovered_control ? GlueAction::none
                                          : GlueAction::redraw;
}

GlueAction glue_left_down(GlueRuntime &glue, const int x,
                          const int y) noexcept {
  if (glue.screen == GlueScreen::title) {
    glue.pressed_control = 0;
    return GlueAction::redraw;
  }
  glue.pressed_control = control_at(glue, x, y);
  return glue.pressed_control == -1 ? GlueAction::none : GlueAction::redraw;
}

GlueAction glue_left_up(GlueRuntime &glue, const int x, const int y,
                        const std::uint32_t now) noexcept {
  if (glue.screen == GlueScreen::title) {
    enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  const std::int16_t released = control_at(glue, x, y);
  const std::int16_t pressed = glue.pressed_control;
  glue.pressed_control = -1;
  if (pressed == -1 || released != pressed) {
    return GlueAction::redraw;
  }
  switch (glue.screen) {
  case GlueScreen::main_menu:
    return activate_main_menu_control(glue, released, now);
  case GlueScreen::connection:
    return activate_connection_control(glue, released, x, y, now);
  case GlueScreen::map_selection:
    return activate_map_selection_control(glue, released, x, y, now);
  case GlueScreen::lobby:
    return activate_lobby_control(glue, released, x, y, now);
  default:
    return GlueAction::redraw;
  }
}

GlueAction glue_key_down(GlueRuntime &glue, const WPARAM key,
                         const std::uint32_t now) noexcept {
  if (glue.screen == GlueScreen::title) {
    enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  if (key == VK_ESCAPE) {
    if (glue.screen == GlueScreen::connection) {
      enter_screen(glue, GlueScreen::main_menu, now);
      return GlueAction::redraw;
    }
    if (glue.screen == GlueScreen::map_selection) {
      enter_screen(glue, GlueScreen::connection, now);
      return GlueAction::redraw;
    }
    if (glue.screen == GlueScreen::lobby) {
      enter_screen(glue, GlueScreen::map_selection, now);
      return GlueAction::redraw;
    }
    if (glue.screen == GlueScreen::ready) {
      enter_screen(glue, GlueScreen::lobby, now);
      glue.ready_deadline = 0U;
      return GlueAction::redraw;
    }
    return GlueAction::quit;
  }
  if (glue.screen == GlueScreen::main_menu) {
    if (key == 'M') {
      return activate_main_menu_control(glue, 4, now);
    }
    if (key == 'X') {
      return GlueAction::quit;
    }
  } else if (glue.screen == GlueScreen::connection) {
    if (key == VK_UP && glue.selected_provider > 0U) {
      --glue.selected_provider;
      return GlueAction::redraw;
    }
    if (key == VK_DOWN &&
        glue.selected_provider + 1U < glue.providers.size()) {
      ++glue.selected_provider;
      return GlueAction::redraw;
    }
    if (key == VK_RETURN) {
      return activate_connection_control(glue, 9, 0, 0, now);
    }
  } else if (glue.screen == GlueScreen::map_selection) {
    if (key == VK_UP && glue.selected_map > 0U) {
      --glue.selected_map;
      return GlueAction::redraw;
    }
    if (key == VK_DOWN && glue.selected_map + 1U < glue.maps.size()) {
      ++glue.selected_map;
      return GlueAction::redraw;
    }
    if (key == VK_RETURN) {
      return activate_map_selection_control(glue, 6, 0, 0, now);
    }
  } else if (glue.screen == GlueScreen::lobby && key == VK_RETURN) {
    return activate_lobby_control(glue, 6, 0, 0, now);
  }
  return GlueAction::none;
}

GlueAction advance_glue(GlueRuntime &glue, const std::uint32_t now) noexcept {
  if (glue.screen == GlueScreen::title &&
      now - glue.screen_entered_tick >= kTitleDurationMilliseconds) {
    enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  if (glue.screen == GlueScreen::ready && glue.ready_deadline != 0U &&
      static_cast<std::int32_t>(now - glue.ready_deadline) >= 0) {
    glue.ready_deadline = 0U;
    return GlueAction::start_game;
  }
  if (glue.screen == GlueScreen::ready) {
    return GlueAction::redraw;
  }
  if (!glue.message.empty() && glue.message_until != 0U &&
      static_cast<std::int32_t>(now - glue.message_until) >= 0) {
    glue.message.clear();
    glue.message_until = 0U;
    return GlueAction::redraw;
  }
  return GlueAction::none;
}

void draw_glue_text_gl(const RecoveryWindowState &state,
                       const std::string_view text, const float x,
                       const float y, const std::uint8_t red,
                       const std::uint8_t green, const std::uint8_t blue,
                       const bool large) noexcept {
  const GLuint lists = large ? state.glue_font_display_lists
                             : state.font_display_lists;
  if (lists == 0U || text.empty()) {
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(red, green, blue, 255U);
  glRasterPos2f(x, y * hud_vertical_scale());
  glListBase(lists - 32U);
  glCallLists(static_cast<GLsizei>(text.size()), GL_UNSIGNED_BYTE, text.data());
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
}

void draw_glue_centered_text_gl(const RecoveryWindowState &state,
                                const std::string_view text,
                                const GlueControl &control,
                                const std::uint8_t red,
                                const std::uint8_t green,
                                const std::uint8_t blue,
                                const bool large) noexcept {
  const float character_width = large ? 11.0F : 6.0F;
  const float x = (static_cast<float>(control.left + control.right) -
                   character_width * static_cast<float>(text.size())) /
                  2.0F;
  const float y =
      (static_cast<float>(control.top + control.bottom) +
       (large ? 18.0F : 9.0F)) /
      2.0F;
  draw_glue_text_gl(state, text, x, y, red, green, blue, large);
}

bool render_glue(const RecoveryWindowState &state) noexcept {
  switch (state.glue.screen) {
  case GlueScreen::title:
    draw_title_gl(state);
    break;
  case GlueScreen::main_menu:
    draw_main_menu_gl(state);
    draw_message(state);
    break;
  case GlueScreen::connection:
    draw_connection_gl(state);
    draw_message(state);
    break;
  case GlueScreen::map_selection:
    draw_map_selection_gl(state);
    draw_message(state);
    break;
  case GlueScreen::lobby:
    draw_lobby_gl(state);
    draw_message(state);
    break;
  case GlueScreen::ready:
    draw_ready_gl(state, GetTickCount());
    draw_message(state);
    break;
  case GlueScreen::gameplay:
    return false;
  }
  return true;
}

} // namespace starcraft::recovery
