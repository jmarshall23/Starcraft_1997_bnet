#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::int16_t kMapRowBase = 300;
constexpr int kMapListLeft = 28;
constexpr int kMapListTop = 48;
constexpr int kMapListRight = 363;
constexpr int kMapRowHeight = 32;

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

void enter_screen(GlueRuntime &glue, const GlueScreen screen,
                  const std::uint32_t now) noexcept {
  glue.screen = screen;
  glue.screen_entered_tick = now;
  glue.hovered_control = -1;
  glue.pressed_control = -1;
  glue.message.clear();
  glue.message_until = 0U;
}

void draw_lobby_image(const RecoveryWindowState &state,
                      const GlueImage &image) noexcept {
  const GlueControl *const control =
      control_with_id(state.glue.lobby_controls, image.control_identifier);
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

std::string map_display_name(const std::filesystem::path &path) {
  return path.stem().string();
}

} // namespace

bool enumerate_glue_maps(starcraft::runtime::StormModule &storm,
                         const std::filesystem::path &root,
                         GlueRuntime &glue) noexcept {
  glue.maps.clear();
  try {
    std::vector<std::filesystem::path> paths;
    const std::filesystem::path maps_directory = root / L"maps";
    for (const std::filesystem::directory_entry &entry :
         std::filesystem::directory_iterator{maps_directory}) {
      const std::wstring extension = entry.path().extension().wstring();
      const DWORD attributes = GetFileAttributesW(entry.path().c_str());
      // gluMap.cpp::sub_461750 at 0x00461750 uses FindFirst/FindNext,
      // excludes hidden files, and compares the configured map extension
      // without regard to case before handing the entry to sub_4621A0.
      if (entry.is_regular_file() && attributes != INVALID_FILE_ATTRIBUTES &&
          (attributes & FILE_ATTRIBUTE_HIDDEN) == 0U &&
          _wcsicmp(extension.c_str(), L".scm") == 0) {
        paths.push_back(entry.path());
      }
    }
    std::sort(paths.begin(), paths.end());
    for (const std::filesystem::path &path : paths) {
      void *map_archive{};
      if (!storm.open_archive(path, &map_archive, 3000U)) {
        continue;
      }
      std::vector<std::uint8_t> bytes;
      const bool loaded = storm.load_file_from_archive(
          map_archive, R"(staredit\scenario.chk)", bytes);
      const bool closed = storm.close_archive(map_archive);
      if (!loaded || !closed) {
        continue;
      }
      const starcraft::data::ChkView chk{bytes.data(), bytes.size()};
      starcraft::game::MultiplayerScenario scenario{};
      if (!scenario.load(chk)) {
        continue;
      }
      GlueMapEntry map{};
      map.path = std::filesystem::relative(path, root).generic_string();
      std::replace(map.path.begin(), map.path.end(), '/', '\\');
      map.name = map_display_name(path);
      map.width = scenario.width();
      map.height = scenario.height();
      map.tileset = scenario.tileset_id();
      map.player_count = scenario.active_player_count();
      for (std::size_t player = 0; player < map.ownership.size(); ++player) {
        map.ownership[player] = scenario.players()[player].ownership;
        map.races[player] = scenario.players()[player].race;
      }
      glue.maps.push_back(std::move(map));
    }
    if (glue.maps.empty()) {
      return false;
    }
    const auto preferred = std::find_if(
        glue.maps.begin(), glue.maps.end(), [](const GlueMapEntry &entry) {
          return entry.path == R"(maps\96x96_space4.scm)";
        });
    glue.selected_map = preferred == glue.maps.end()
                            ? 0U
                            : static_cast<std::size_t>(preferred -
                                                       glue.maps.begin());
    return true;
  } catch (...) {
    glue.maps.clear();
    glue.selected_map = 0U;
    return false;
  }
}

void configure_lobby_slots(GlueRuntime &glue) noexcept {
  glue.lobby_slots = {};
  if (glue.selected_map >= glue.maps.size()) {
    return;
  }
  const GlueMapEntry &map = glue.maps[glue.selected_map];
  bool local_assigned{};
  for (std::size_t player = 0; player < glue.lobby_slots.size(); ++player) {
    if (map.ownership[player] == 0U || map.races[player] >= 3U) {
      continue;
    }
    GlueLobbySlot &slot = glue.lobby_slots[player];
    slot.local = !local_assigned;
    slot.name = slot.local ? "Player" : "Computer";
    // gluChatSlot.cpp::sub_45CE70 at 0x0045CE70 walks all twelve 44-byte
    // network slot records and updates their name/race/status controls.
    // Preserve its 12-slot topology here. The beta lobby slot records use 6
    // for the local human and 5 for an AI player.
    slot.ownership = slot.local ? 6U : 5U;
    slot.race = map.races[player];
    local_assigned = true;
  }
}

std::int16_t map_selection_control_at(const GlueRuntime &glue, const int x,
                                      const int y) noexcept {
  for (const std::int16_t identifier : {std::int16_t{6}, std::int16_t{7}}) {
    const GlueControl *const control =
        control_with_id(glue.lobby_controls, identifier);
    if (control != nullptr && point_in_control(*control, x, y)) {
      return identifier;
    }
  }
  if (x >= kMapListLeft && x <= kMapListRight && y >= kMapListTop) {
    const int row = (y - kMapListTop) / kMapRowHeight;
    if (row >= 0 && static_cast<std::size_t>(row) < glue.maps.size() &&
        row < 6) {
      return static_cast<std::int16_t>(kMapRowBase + row);
    }
  }
  return -1;
}

GlueAction activate_map_selection_control(GlueRuntime &glue,
                                          const std::int16_t identifier,
                                          const int, const int,
                                          const std::uint32_t now) noexcept {
  if (identifier == 7) {
    enter_screen(glue, GlueScreen::connection, now);
    return GlueAction::redraw;
  }
  if (identifier >= kMapRowBase && identifier < kMapRowBase + 6) {
    const std::size_t map = static_cast<std::size_t>(identifier - kMapRowBase);
    if (map < glue.maps.size()) {
      glue.selected_map = map;
    }
    return GlueAction::redraw;
  }
  if (identifier == 6 && glue.selected_map < glue.maps.size()) {
    configure_lobby_slots(glue);
    enter_screen(glue, GlueScreen::lobby, now);
    return GlueAction::redraw;
  }
  return GlueAction::none;
}

bool start_selected_glue_map(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr || state.glue.selected_map >= state.glue.maps.size()) {
    return false;
  }
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> races{};
  for (std::size_t player = 0; player < state.glue.lobby_slots.size();
       ++player) {
    ownership[player] = state.glue.lobby_slots[player].ownership;
    races[player] = state.glue.lobby_slots[player].race;
  }
  BootstrapStatus replacement =
      probe_assets(std::filesystem::path{state.glue.maps[state.glue.selected_map]
                                             .path},
                   &ownership, &races);
  if (!replacement.assets_ready) {
    state.glue.screen = GlueScreen::lobby;
    state.glue.message = replacement.detail.empty()
                             ? "The selected map could not be started."
                             : replacement.detail;
    state.glue.message_until = GetTickCount() + 6000U;
    return false;
  }
  shutdown_audio(state);
  *state.status = std::move(replacement);
  (void)initialize_audio(state);
  state.glue.screen = GlueScreen::gameplay;
  state.glue.message.clear();
  state.glue.message_until = 0U;
  begin_match_flow(state);
  return true;
}

void draw_map_selection_gl(const RecoveryWindowState &state) noexcept {
  draw_preview_frame_gl(state.glue.connection_background, 0.0F, 0.0F,
                        static_cast<float>(kMapViewportWidth),
                        static_cast<float>(kMapViewportHeight));
  for (const GlueImage &image : state.glue.lobby_images) {
    if (image.control_identifier == 3 || image.control_identifier == 4 ||
        image.control_identifier == 5) {
      draw_lobby_image(state, image);
    }
  }
  draw_glue_text_gl(state, "Select Map", 116.0F, 26.0F, 255U, 220U, 96U,
                    true);
  for (std::size_t index = 0; index < state.glue.maps.size() && index < 6U;
       ++index) {
    const float top = static_cast<float>(kMapListTop +
                                         static_cast<int>(index) *
                                             kMapRowHeight);
    if (index == state.glue.selected_map) {
      glDisable(GL_TEXTURE_2D);
      glColor4ub(48U, 92U, 144U, 150U);
      glBegin(GL_QUADS);
      glVertex2f(31.0F, top * hud_vertical_scale());
      glVertex2f(360.0F, top * hud_vertical_scale());
      glVertex2f(360.0F, (top + 26.0F) * hud_vertical_scale());
      glVertex2f(31.0F, (top + 26.0F) * hud_vertical_scale());
      glEnd();
      glColor4ub(255U, 255U, 255U, 255U);
      glEnable(GL_TEXTURE_2D);
    }
    const GlueMapEntry &map = state.glue.maps[index];
    const std::string detail = map.name + "  " + std::to_string(map.width) +
                               "x" + std::to_string(map.height) + "  " +
                               std::to_string(map.player_count) + " Players";
    draw_glue_text_gl(state, detail, 39.0F, top + 18.0F,
                      index == state.glue.selected_map ? 255U : 210U,
                      index == state.glue.selected_map ? 230U : 210U,
                      index == state.glue.selected_map ? 128U : 210U, false);
  }
  const GlueMapEntry &selected = state.glue.maps[state.glue.selected_map];
  draw_glue_text_gl(state, "Game Type:", 404.0F, 137.0F, 210U, 210U, 210U);
  draw_glue_text_gl(state, "Melee", 412.0F, 167.0F, 255U, 220U, 96U);
  draw_glue_text_gl(state, "Map Size:", 404.0F, 207.0F, 210U, 210U, 210U);
  draw_glue_text_gl(state,
                    std::to_string(selected.width) + "x" +
                        std::to_string(selected.height),
                    412.0F, 237.0F, 255U, 220U, 96U);
  for (const std::int16_t identifier : {std::int16_t{6}, std::int16_t{7}}) {
    const GlueControl *const button =
        control_with_id(state.glue.lobby_controls, identifier);
    if (button == nullptr) {
      continue;
    }
    const bool hovered = state.glue.hovered_control == identifier;
    const bool pressed = state.glue.pressed_control == identifier;
    draw_glue_centered_text_gl(
        state, button->text, *button, pressed || hovered ? 255U : 220U,
        pressed ? 128U : hovered ? 224U : 220U,
        pressed ? 48U : hovered ? 96U : 220U, false);
  }
}

} // namespace starcraft::recovery
