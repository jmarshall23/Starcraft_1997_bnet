#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::uint16_t kGlueWidth = 640U;
constexpr std::uint16_t kGlueHeight = 480U;
constexpr std::uint32_t kTitleDurationMilliseconds = 1800U;
constexpr std::uint32_t kTransformTickMilliseconds = 20U;
constexpr std::uint32_t kTransformEnterMilliseconds = 480U;
constexpr std::uint32_t kTransformLeaveMilliseconds = 240U;

constexpr std::array<GlueTransformControl, 4> kConnectionTransforms{{
    {1, 0}, {2, 2}, {3, 3}, {4, 2},
}};
constexpr std::array<GlueTransformControl, 5> kLobbyTransforms{{
    {1, 3}, {2, 2}, {3, 0}, {4, 3}, {5, 2},
}};
// glues.cpp::sub_460100 consumes these exact (control, direction) pairs.
// sub_460290 performs the quarter-distance entrance step; sub_460400 and
// sub_4604B0 perform the doubling exit step. The matching PDB is absent, so
// retain address evidence instead of assigning unsupported original names.

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
      // BIN control records end with an unaligned pointer at +66. For type-14
      // controls it owns the linked SMK descriptor chain (path, flags, and
      // per-layer origin) used by the original dialog renderer.
      control.visual_offset = read_u32(layout, offset + 66U);
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
                   SpritePreviewFrame &frame,
                   std::vector<std::uint8_t> *const palette = nullptr) {
  starcraft::runtime::DecodedPcx image{};
  if (!storm.load_pcx(path, image) ||
      !decode_pcx_frame(image, transparent, frame)) {
    return false;
  }
  if (palette != nullptr) {
    *palette = image.palette;
  }
  return true;
}

bool load_glue_font_colors(
    starcraft::runtime::StormModule &storm, const char *const path,
    const std::vector<std::uint8_t> &palette,
    std::array<std::array<std::uint32_t, 8>, 6> &colors) {
  starcraft::runtime::DecodedPcx table{};
  if (!storm.load_pcx(path, table) || table.width != 48U ||
      table.height != 1U || table.pixels.size() != 48U ||
      palette.size() != 1024U) {
    return false;
  }
  for (std::size_t style = 0; style < colors.size(); ++style) {
    for (std::size_t shade = 0; shade < colors[style].size(); ++shade) {
      const std::size_t palette_index = table.pixels[style * 8U + shade];
      const std::size_t color = palette_index * 4U;
      colors[style][shade] =
          (static_cast<std::uint32_t>(palette[color]) << 16U) |
          (static_cast<std::uint32_t>(palette[color + 1U]) << 8U) |
          palette[color + 2U];
    }
  }
  return true;
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

bool load_main_control_videos(starcraft::runtime::StormModule &storm,
                              const std::vector<std::uint8_t> &layout,
                              const std::vector<GlueControl> &controls,
                              std::vector<GlueVideo> &videos) {
  videos.clear();
  try {
    for (const GlueControl &control : controls) {
      std::uint32_t descriptor = control.visual_offset;
      std::size_t visited{};
      while (descriptor != 0U && descriptor + 30U <= layout.size() &&
             visited++ < 16U) {
        const std::uint32_t next = read_u32(layout, descriptor);
        const std::uint32_t flags = read_u32(layout, descriptor + 4U);
        const std::uint32_t path_offset =
            read_u32(layout, descriptor + 10U);
        if (path_offset == 0U || path_offset >= layout.size()) {
          return false;
        }
        const char *const path =
            reinterpret_cast<const char *>(layout.data() + path_offset);
        const std::size_t available = layout.size() - path_offset;
        if (std::memchr(path, 0, available) == nullptr) {
          return false;
        }
        GlueVideo video{};
        video.control_identifier = control.identifier;
        video.descriptor_flags = flags;
        video.x_offset =
            static_cast<std::int16_t>(read_u16(layout, descriptor + 18U));
        video.y_offset =
            static_cast<std::int16_t>(read_u16(layout, descriptor + 20U));
        // The menu SMKs are composited over BackGnd.pcx. Palette index zero is
        // the transparent key for both the base (flag 4) and highlighted
        // (flag 12) descriptors; bit 8 selects the highlighted layer.
        if (!load_smacker_animation(storm, path, true, video.animation)) {
          return false;
        }
        videos.push_back(std::move(video));
        if (next == descriptor) {
          return false;
        }
        descriptor = next;
      }
      if (descriptor != 0U) {
        return false;
      }
    }
  } catch (...) {
    videos.clear();
    return false;
  }
  return !videos.empty();
}

void reset_screen_state(GlueRuntime &glue, const GlueScreen screen,
                        const std::uint32_t now) noexcept {
  glue.screen = screen;
  glue.screen_entered_tick = now;
  if (screen == GlueScreen::map_selection) {
    glue.selected_map_changed_tick = now;
  }
  glue.hovered_control = -1;
  glue.pressed_control = -1;
  glue.message.clear();
  glue.message_until = 0U;
  glue.popup_control = -1;
  glue.popup_row = -1;
  glue.transform_started_tick = now;
  glue.clock_tick = now;
  glue.transform_target = screen;
  glue.transform_action = GlueAction::none;
  glue.transform_controls.clear();
  if (screen == GlueScreen::connection) {
    glue.transform_controls.assign(kConnectionTransforms.begin(),
                                   kConnectionTransforms.end());
  } else if (screen == GlueScreen::map_selection ||
             screen == GlueScreen::lobby) {
    glue.transform_controls.assign(kLobbyTransforms.begin(),
                                   kLobbyTransforms.end());
  }
  glue.transform_phase = glue.transform_controls.empty()
                             ? GlueTransformPhase::none
                             : GlueTransformPhase::entering;
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
  case GlueScreen::battle_net:
    return battle::BattleNetControlAt(glue.battle_net, x, y);
  default:
    return -1;
  }
}

GlueAction apply_battle_ui_action(GlueRuntime &glue,
                                  const battle::BattleUiAction action,
                                  const std::uint32_t now) noexcept {
  switch (action) {
  case battle::BattleUiAction::redraw:
    return GlueAction::redraw;
  case battle::BattleUiAction::leave_battle_net:
    glue.online_lobby = false;
    glues_enter_screen(glue, GlueScreen::connection, now);
    return GlueAction::redraw;
  case battle::BattleUiAction::enter_game_lobby:
    glue.online_lobby = true;
    configure_lobby_slots(glue);
    glues_enter_screen(glue, GlueScreen::lobby, now);
    return GlueAction::redraw;
  case battle::BattleUiAction::none:
  default:
    return GlueAction::none;
  }
}

std::string_view selected_map_name(const GlueRuntime &glue) noexcept {
  return glue.selected_map < glue.maps.size()
             ? std::string_view{glue.maps[glue.selected_map].name}
             : std::string_view{"Unknown Map"};
}

} // namespace

void glues_enter_screen(GlueRuntime &glue, const GlueScreen screen,
                        const std::uint32_t now) noexcept {
  reset_screen_state(glue, screen, now);
}

GlueAction glues_leave_screen(GlueRuntime &glue, const GlueScreen target,
                              const GlueAction action,
                              const std::uint32_t now) noexcept {
  if (glue.transform_controls.empty()) {
    if (target != glue.screen) {
      glues_enter_screen(glue, target, now);
    }
    return action == GlueAction::none ? GlueAction::redraw : action;
  }
  glue.transform_phase = GlueTransformPhase::leaving;
  glue.transform_started_tick = now;
  glue.transform_target = target;
  glue.transform_action = action;
  glue.hovered_control = -1;
  glue.pressed_control = -1;
  glue.popup_control = -1;
  glue.popup_row = -1;
  return GlueAction::redraw;
}

void glues_control_rect(const GlueRuntime &glue, const GlueControl &control,
                        std::int16_t &left, std::int16_t &top,
                        std::int16_t &right,
                        std::int16_t &bottom) noexcept {
  left = control.left;
  top = control.top;
  right = control.right;
  bottom = control.bottom;
  if (glue.transform_phase == GlueTransformPhase::none) {
    return;
  }
  auto found = std::find_if(
      glue.transform_controls.begin(), glue.transform_controls.end(),
      [&control](const GlueTransformControl &transform) {
        return transform.identifier == control.identifier;
      });
  if (found == glue.transform_controls.end()) {
    const std::vector<GlueControl> &layout =
        glue.screen == GlueScreen::connection ? glue.connection_controls
                                              : glue.lobby_controls;
    const int center_x = (control.left + control.right) / 2;
    const int center_y = (control.top + control.bottom) / 2;
    for (auto candidate = glue.transform_controls.begin();
         candidate != glue.transform_controls.end(); ++candidate) {
      const auto panel = std::find_if(
          layout.begin(), layout.end(), [&candidate](const GlueControl &value) {
            return value.identifier == candidate->identifier &&
                   value.type == 5U;
          });
      if (panel != layout.end() && center_x >= panel->left &&
          center_x <= panel->right && center_y >= panel->top &&
          center_y <= panel->bottom) {
        found = candidate;
        break;
      }
    }
    if (found == glue.transform_controls.end()) {
      return;
    }
  }
  const std::uint32_t elapsed =
      glue.clock_tick - glue.transform_started_tick;
  const std::uint32_t ticks = elapsed / kTransformTickMilliseconds;
  int offset{};
  if (glue.transform_phase == GlueTransformPhase::entering) {
    switch (found->direction) {
    case 0:
      offset = -(static_cast<int>(control.right) + 1);
      break;
    case 1:
      offset = -(static_cast<int>(control.bottom) + 1);
      break;
    case 2:
      offset = static_cast<int>(kGlueWidth) - control.left;
      break;
    default:
      offset = static_cast<int>(kGlueHeight) - control.top;
      break;
    }
    for (std::uint32_t tick = 0U; tick < ticks && offset != 0; ++tick) {
      const int distance = (std::max)(1, std::abs(offset) >> 2);
      offset += offset < 0 ? distance : -distance;
      if (std::abs(offset) < distance) {
        offset = 0;
      }
    }
  } else {
    const std::uint32_t limited_ticks = (std::min)(ticks, 30U);
    const int distance =
        limited_ticks == 0U ? 0 : (1 << limited_ticks) - 1;
    offset = (found->direction == 0 || found->direction == 1) ? -distance
                                                              : distance;
  }
  if (found->direction == 0 || found->direction == 2) {
    left = static_cast<std::int16_t>(left + offset);
    right = static_cast<std::int16_t>(right + offset);
  } else {
    top = static_cast<std::int16_t>(top + offset);
    bottom = static_cast<std::int16_t>(bottom + offset);
  }
}

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
  std::vector<std::uint8_t> main_palette;
  std::vector<std::uint8_t> network_palette;
  std::vector<std::uint8_t> main_dialog_group;
  std::vector<std::uint8_t> network_dialog_group;
  std::uint16_t dialog_width{};
  std::uint16_t dialog_height{};
  const bool loaded =
      load_glue_pcx(storm, R"(glue\title\title-beta.pcx)", false,
                    glue.title_background) &&
      load_glue_pcx(storm, R"(glue\PalMm\BackGnd.pcx)", false,
                    glue.main_background, &main_palette) &&
      load_glue_pcx(storm, R"(glue\PalNl\BackGnd.pcx)", false,
                    glue.connection_background, &network_palette) &&
      starcraft::gds::load_starcraft_font(storm, R"(font\font10.fnt)",
                                          glue.small_font) &&
      starcraft::gds::load_starcraft_font(storm, R"(font\font16.fnt)",
                                          glue.large_font) &&
      load_glue_font_colors(storm, R"(glue\PalMm\TFont.pcx)", main_palette,
                            glue.main_font_colors) &&
      load_glue_font_colors(storm, R"(glue\PalNl\TFont.pcx)",
                            network_palette, glue.network_font_colors) &&
      storm.load_file(R"(rez\gluMain.bin)", main_layout) &&
      storm.load_file(R"(rez\gluConn.bin)", connection_layout) &&
      storm.load_file(R"(rez\gluChat.bin)", lobby_layout) &&
      parse_glue_layout(main_layout, glue.main_controls) &&
      parse_glue_layout(connection_layout, glue.connection_controls) &&
      parse_glue_layout(lobby_layout, glue.lobby_controls) &&
      load_main_control_videos(storm, main_layout, glue.main_controls,
                               glue.main_videos) &&
      load_control_images(storm, glue.main_controls, glue.main_images) &&
      load_control_images(storm, glue.connection_controls,
                          glue.connection_images) &&
      load_control_images(storm, glue.lobby_controls, glue.lobby_images) &&
      storm.load_file(R"(glue\PalMm\Dlg.grp)", main_dialog_group) &&
      decode_preview_frames(main_dialog_group, main_palette,
                            glue.main_dialog_frames, dialog_width,
                            dialog_height) &&
      storm.load_file(R"(glue\PalNl\Dlg.grp)", network_dialog_group) &&
      decode_preview_frames(network_dialog_group, network_palette,
                            glue.network_dialog_frames, dialog_width,
                            dialog_height) &&
      battle::UiLoadArtwork(storm, glue.battle_artwork) &&
      enumerate_glue_maps(storm, root, glue);
  const bool patch_closed = storm.close_archive(patch);
  const bool base_closed = storm.close_archive(base);
  glue.assets_ready = loaded && patch_closed && base_closed;
  if (!glue.assets_ready) {
    glue.failure = "A title, StarCraft font, glue dialog, Battle.net artwork, "
                   "control image, or multiplayer map failed to decode.";
    glue.screen = GlueScreen::gameplay;
    return false;
  }
  try {
    glue.battle_net.available_maps.clear();
    glue.battle_net.available_maps.reserve(glue.maps.size());
    for (const GlueMapEntry &map : glue.maps) {
      battle::BattleMapEntry entry{};
      entry.path = map.path;
      entry.name = map.name;
      entry.width = map.width;
      entry.height = map.height;
      entry.players = static_cast<std::uint32_t>(map.player_count);
      glue.battle_net.available_maps.push_back(std::move(entry));
    }
  } catch (...) {
    glue.assets_ready = false;
    glue.failure = "The multiplayer map list could not be prepared for "
                   "Battle.net.";
    glue.screen = GlueScreen::gameplay;
    return false;
  }
  configure_lobby_slots(glue);
  glues_enter_screen(glue, GlueScreen::title, GetTickCount());
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
  const PresentationViewport viewport =
      presentation_viewport(client.right, client.bottom);
  if (viewport.width <= 0 || viewport.height <= 0 ||
      client_x < viewport.x || client_x >= viewport.x + viewport.width ||
      client_y < viewport.y || client_y >= viewport.y + viewport.height) {
    return false;
  }
  glue_x = (client_x - viewport.x) * kGlueWidth / viewport.width;
  glue_y = (client_y - viewport.y) * kGlueHeight / viewport.height;
  return glue_x >= 0 && glue_y >= 0 && glue_x < kGlueWidth &&
         glue_y < kGlueHeight;
}

GlueAction glue_mouse_move(GlueRuntime &glue, const int x,
                           const int y) noexcept {
  if (glue.transform_phase != GlueTransformPhase::none) {
    const bool changed = glue.hovered_control != -1;
    glue.hovered_control = -1;
    return changed ? GlueAction::redraw : GlueAction::none;
  }
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
  case GlueScreen::battle_net:
    return apply_battle_ui_action(
        glue, battle::BattleNetMouseMove(glue.battle_net, x, y),
        glue.clock_tick);
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
  if (glue.transform_phase != GlueTransformPhase::none) {
    return GlueAction::none;
  }
  if (glue.screen == GlueScreen::battle_net) {
    return apply_battle_ui_action(
        glue, battle::BattleNetLeftDown(glue.battle_net, x, y),
        glue.clock_tick);
  }
  glue.pressed_control = control_at(glue, x, y);
  if (glue.screen == GlueScreen::lobby && glue.popup_control != -1 &&
      glue.pressed_control == -1) {
    glue.popup_control = -1;
    glue.popup_row = -1;
    return GlueAction::redraw;
  }
  return glue.pressed_control == -1 ? GlueAction::none : GlueAction::redraw;
}

GlueAction glue_left_up(GlueRuntime &glue, const int x, const int y,
                        const std::uint32_t now) noexcept {
  if (glue.screen == GlueScreen::title) {
    glues_enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  if (glue.transform_phase != GlueTransformPhase::none) {
    glue.pressed_control = -1;
    return GlueAction::none;
  }
  if (glue.screen == GlueScreen::battle_net) {
    return apply_battle_ui_action(
        glue,
        battle::BattleNetLeftUp(glue.battle_net, x, y,
                                selected_map_name(glue)),
        now);
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
    glues_enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  if (glue.transform_phase != GlueTransformPhase::none) {
    return GlueAction::none;
  }
  if (key == VK_ESCAPE && glue.popup_control != -1) {
    glue.popup_control = -1;
    glue.popup_row = -1;
    return GlueAction::redraw;
  }
  if (key == VK_ESCAPE) {
    if (glue.screen == GlueScreen::battle_net) {
      return apply_battle_ui_action(
          glue,
          battle::BattleNetKeyDown(glue.battle_net,
                                   static_cast<std::uintptr_t>(key),
                                   selected_map_name(glue)),
          now);
    }
    if (glue.screen == GlueScreen::connection) {
      return glues_leave_screen(glue, GlueScreen::main_menu,
                                GlueAction::none, now);
    }
    if (glue.screen == GlueScreen::map_selection) {
      return glues_leave_screen(glue, GlueScreen::connection,
                                GlueAction::none, now);
    }
    if (glue.screen == GlueScreen::lobby) {
      if (glue.online_lobby) {
        glue.online_lobby = false;
        glues_enter_screen(glue, GlueScreen::battle_net, now);
        return GlueAction::redraw;
      }
      return glues_leave_screen(glue, GlueScreen::map_selection,
                                GlueAction::none, now);
    }
    if (glue.screen == GlueScreen::ready) {
      glues_enter_screen(glue, GlueScreen::lobby, now);
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
      glue.selected_map_changed_tick = now;
      return GlueAction::redraw;
    }
    if (key == VK_DOWN && glue.selected_map + 1U < glue.maps.size()) {
      ++glue.selected_map;
      glue.selected_map_changed_tick = now;
      return GlueAction::redraw;
    }
    if (key == VK_RETURN) {
      return activate_map_selection_control(glue, 6, 0, 0, now);
    }
  } else if (glue.screen == GlueScreen::lobby && key == VK_RETURN) {
    return activate_lobby_control(glue, 6, 0, 0, now);
  } else if (glue.screen == GlueScreen::battle_net) {
    return apply_battle_ui_action(
        glue,
        battle::BattleNetKeyDown(glue.battle_net,
                                 static_cast<std::uintptr_t>(key),
                                 selected_map_name(glue)),
        now);
  }
  return GlueAction::none;
}

GlueAction glue_character(GlueRuntime &glue, const char character) noexcept {
  if (glue.screen != GlueScreen::battle_net) {
    return GlueAction::none;
  }
  return apply_battle_ui_action(
      glue, battle::BattleNetCharacter(glue.battle_net, character),
      glue.clock_tick);
}

GlueAction advance_glue(GlueRuntime &glue, const std::uint32_t now) noexcept {
  glue.clock_tick = now;
  if (glue.screen == GlueScreen::title &&
      now - glue.screen_entered_tick >= kTitleDurationMilliseconds) {
    glues_enter_screen(glue, GlueScreen::main_menu, now);
    return GlueAction::redraw;
  }
  if (glue.transform_phase == GlueTransformPhase::entering) {
    if (now - glue.transform_started_tick >= kTransformEnterMilliseconds) {
      glue.transform_phase = GlueTransformPhase::none;
    }
    return GlueAction::redraw;
  }
  if (glue.transform_phase == GlueTransformPhase::leaving) {
    if (now - glue.transform_started_tick < kTransformLeaveMilliseconds) {
      return GlueAction::redraw;
    }
    const GlueScreen target = glue.transform_target;
    const GlueAction action = glue.transform_action;
    glues_enter_screen(glue, target, now);
    return action == GlueAction::none ? GlueAction::redraw : action;
  }
  if (glue.screen == GlueScreen::ready && glue.ready_deadline != 0U &&
      static_cast<std::int32_t>(now - glue.ready_deadline) >= 0) {
    glue.ready_deadline = 0U;
    return GlueAction::start_game;
  }
  if (glue.screen == GlueScreen::ready) {
    return GlueAction::redraw;
  }
  if (glue.screen == GlueScreen::battle_net) {
    return apply_battle_ui_action(glue,
                                  battle::UiNotification(glue.battle_net),
                                  now);
  }
  if (glue.screen == GlueScreen::main_menu) {
    bool advanced{};
    for (GlueVideo &video : glue.main_videos) {
      advanced = advance_smacker_animation(video.animation, now) || advanced;
    }
    if (advanced) {
      return GlueAction::redraw;
    }
  }
  if (!glue.message.empty() && glue.message_until != 0U &&
      static_cast<std::int32_t>(now - glue.message_until) >= 0) {
    glue.message.clear();
    glue.message_until = 0U;
    return GlueAction::redraw;
  }
  if (glue.screen == GlueScreen::map_selection &&
      now - glue.selected_map_changed_tick < 240U) {
    // gluMap's selected-map details are palette-faded after a row change.
    // Continue presenting frames until that recovered UI transition settles.
    return GlueAction::redraw;
  }
  return GlueAction::none;
}

void draw_glue_text_gl(const RecoveryWindowState &state,
                       const std::string_view text, const float x,
                       const float y, const std::uint8_t red,
                       const std::uint8_t green, const std::uint8_t blue,
                       const bool large) noexcept {
  GlueFontStyle style = GlueFontStyle::normal;
  if (red >= 224U && green < 176U) {
    style = GlueFontStyle::error;
  } else if (green > red + 12U) {
    style = GlueFontStyle::bright_green;
  } else if (red >= 240U && green >= 190U && blue < 176U) {
    style = GlueFontStyle::gold;
  } else if (red < 176U && green < 176U && blue < 176U) {
    style = GlueFontStyle::disabled;
  }
  draw_glue_styled_text_gl(state, text, x, y, style, large);
}

void draw_glue_styled_text_gl(const RecoveryWindowState &state,
                              const std::string_view text, const float x,
                              const float y, const GlueFontStyle style,
                              const bool large,
                              const std::uint8_t alpha) noexcept {
  const starcraft::gds::BitmapFont &font =
      large ? state.glue.large_font : state.glue.small_font;
  if (text.empty()) {
    return;
  }
  if (font.glyphs.empty()) {
    draw_game_text_gl(state, text, x, y * hud_vertical_scale(), 220U, 220U,
                      220U, large);
    return;
  }
  const auto &color_tables =
      state.glue.screen == GlueScreen::main_menu
          ? state.glue.main_font_colors
          : state.glue.network_font_colors;
  const std::size_t style_index = static_cast<std::size_t>(style);
  if (style_index >= color_tables.size()) {
    return;
  }
  const float vertical_scale = hud_vertical_scale();
  float cursor_x = x;
  float baseline_y = y;
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  for (const unsigned char character : text) {
    if (character == '\n') {
      cursor_x = x;
      baseline_y += static_cast<float>(font.maximum_height + 1U);
      continue;
    }
    if (character == ' ') {
      cursor_x += static_cast<float>(font.maximum_width / 2U);
      continue;
    }
    const starcraft::gds::FontGlyph *glyph = font.glyph(character);
    if (glyph == nullptr) {
      glyph = font.glyph('?');
    }
    if (glyph == nullptr) {
      continue;
    }
    const float glyph_left = cursor_x + glyph->x_offset;
    const float glyph_top = baseline_y - font.maximum_height + glyph->y_offset;
    for (std::size_t row = 0; row < glyph->height; ++row) {
      for (std::size_t column = 0; column < glyph->width; ++column) {
        const std::uint8_t shade =
            glyph->shades[row * glyph->width + column];
        if (shade >= color_tables[style_index].size()) {
          continue;
        }
        const std::uint32_t color = color_tables[style_index][shade];
        glColor4ub(static_cast<std::uint8_t>(color >> 16U),
                   static_cast<std::uint8_t>(color >> 8U),
                   static_cast<std::uint8_t>(color), alpha);
        const float left = glyph_left + static_cast<float>(column);
        const float top =
            (glyph_top + static_cast<float>(row)) * vertical_scale;
        glVertex2f(left, top);
        glVertex2f(left + 1.0F, top);
        glVertex2f(left + 1.0F, top + vertical_scale);
        glVertex2f(left, top + vertical_scale);
      }
    }
    cursor_x += static_cast<float>(glyph->width + glyph->x_offset + 1U);
  }
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
}

void draw_game_text_gl(const RecoveryWindowState &state,
                       const std::string_view text, const float x,
                       const float y, const std::uint8_t red,
                       const std::uint8_t green, const std::uint8_t blue,
                       const bool large) noexcept {
  const GLuint lists = large ? state.glue_font_display_lists
                             : state.font_display_lists;
  if (lists == 0U || text.empty()) {
    return;
  }
  std::string printable;
  try {
    printable.reserve(text.size());
    for (const unsigned char character : text) {
      printable.push_back(character >= 32U && character < 128U
                              ? static_cast<char>(character)
                              : '?');
    }
  } catch (...) {
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(red, green, blue, 255U);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(x, y, 0.0F);
  const float scale = large ? state.glue_font_outline_scale
                            : state.font_outline_scale;
  glScalef(scale, -scale, 1.0F);
  glListBase(lists - 32U);
  glCallLists(static_cast<GLsizei>(printable.size()), GL_UNSIGNED_BYTE,
              printable.data());
  glPopMatrix();
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
  GlueFontStyle style = GlueFontStyle::normal;
  if (red >= 224U && green < 176U) {
    style = GlueFontStyle::error;
  } else if (green > red + 12U) {
    style = GlueFontStyle::bright_green;
  } else if (red >= 240U && green >= 190U && blue < 176U) {
    style = GlueFontStyle::gold;
  } else if (red < 176U && green < 176U && blue < 176U) {
    style = GlueFontStyle::disabled;
  }
  draw_glue_centered_styled_text_gl(state, text, control, style, large);
}

void draw_glue_centered_styled_text_gl(
    const RecoveryWindowState &state, const std::string_view text,
    const GlueControl &control, const GlueFontStyle style, const bool large,
    const std::uint8_t alpha) noexcept {
  const starcraft::gds::BitmapFont &font =
      large ? state.glue.large_font : state.glue.small_font;
  const float text_width = font.glyphs.empty()
                               ? 0.0F
                               : font.text_width(text);
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
  glues_control_rect(state.glue, control, left, top, right, bottom);
  const float x =
      (static_cast<float>(left + right + 1) - text_width) / 2.0F;
  const float y =
      (static_cast<float>(top + bottom) +
       static_cast<float>(font.maximum_height)) /
      2.0F;
  draw_glue_styled_text_gl(state, text, x, y, style, large, alpha);
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
  case GlueScreen::battle_net:
    battle::DrawBattleNet(state);
    break;
  case GlueScreen::gameplay:
    return false;
  }
  return true;
}

} // namespace starcraft::recovery
