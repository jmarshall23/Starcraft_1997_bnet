#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace starcraft::recovery {

// cellscrl input routine sub_49E140 indexes this byte table at original VA
// 0x004FD6B8 as [scroll-speed option][acceleration step 0..6].
constexpr std::array<std::array<std::uint8_t, 7>, 7> kCameraScrollSpeeds{{
    {{0, 0, 8, 8, 16, 16, 16}},
    {{0, 0, 8, 8, 16, 16, 24}},
    {{0, 8, 8, 16, 16, 24, 24}},
    {{0, 8, 8, 16, 24, 24, 32}},
    {{0, 8, 16, 16, 24, 32, 40}},
    {{0, 8, 16, 24, 32, 40, 48}},
    {{0, 8, 24, 32, 40, 56, 64}},
}};
constexpr std::size_t kDefaultCameraScrollSpeed = 3;

bool set_camera_position(BootstrapStatus &status, const int requested_x,
                         const int requested_y) noexcept {
  if (!status.scenario.valid() || !status.terrain_tileset.valid()) {
    return false;
  }
  const int maximum_x =
      (std::max)(0, static_cast<int>(status.scenario_width) * 32 -
                        kMapViewportWidth);
  const int maximum_y =
      (std::max)(0, static_cast<int>(status.scenario_height) * 32 -
                        kMapViewportHeight);
  // The common camera setter at 0x00405BB0 clamps both axes and clears the
  // low three bits. Keep the original eight-pixel camera granularity.
  const int camera_x = (std::clamp)(requested_x, 0, maximum_x) & ~7;
  const int camera_y = (std::clamp)(requested_y, 0, maximum_y) & ~7;
  if (camera_x == status.camera_x && camera_y == status.camera_y) {
    return false;
  }
  SpritePreviewFrame terrain{};
  if (!build_terrain_preview(status.terrain_tileset, status.scenario,
                             static_cast<std::uint16_t>(camera_x),
                             static_cast<std::uint16_t>(camera_y), terrain,
                             &status.creep_tiles, &status.creep_visual_tiles)) {
    return false;
  }
  status.camera_x = static_cast<std::uint16_t>(camera_x);
  status.camera_y = static_cast<std::uint16_t>(camera_y);
  status.camera_tile_x = static_cast<std::uint16_t>(camera_x / 32);
  status.camera_tile_y = static_cast<std::uint16_t>(camera_y / 32);
  status.terrain = std::move(terrain);
  status.terrain_ready = true;
  return true;
}

bool camera_key_down(const RecoveryWindowState &state, const int key) noexcept {
  return key >= 0 && key < static_cast<int>(state.keys_down.size()) &&
         state.keys_down[static_cast<std::size_t>(key)];
}

bool camera_input_key(const WPARAM key) noexcept {
  switch (key) {
  case VK_LEFT:
  case VK_RIGHT:
  case VK_UP:
  case VK_DOWN:
  case VK_HOME:
  case VK_END:
  case VK_PRIOR:
  case VK_NEXT:
  case VK_NUMPAD1:
  case VK_NUMPAD2:
  case VK_NUMPAD3:
  case VK_NUMPAD4:
  case VK_NUMPAD6:
  case VK_NUMPAD7:
  case VK_NUMPAD8:
  case VK_NUMPAD9:
    return true;
  default:
    return false;
  }
}

bool advance_camera_scroll(RecoveryWindowState &state) noexcept {
  BootstrapStatus *const status = state.status;
  if (status == nullptr) {
    return false;
  }
  int vertical{};
  int horizontal{};
  const bool down =
      camera_key_down(state, VK_DOWN) || camera_key_down(state, VK_END) ||
      camera_key_down(state, VK_NEXT) || camera_key_down(state, VK_NUMPAD1) ||
      camera_key_down(state, VK_NUMPAD2) || camera_key_down(state, VK_NUMPAD3);
  const bool up =
      camera_key_down(state, VK_UP) || camera_key_down(state, VK_HOME) ||
      camera_key_down(state, VK_PRIOR) || camera_key_down(state, VK_NUMPAD7) ||
      camera_key_down(state, VK_NUMPAD8) || camera_key_down(state, VK_NUMPAD9);
  const bool right =
      camera_key_down(state, VK_RIGHT) || camera_key_down(state, VK_PRIOR) ||
      camera_key_down(state, VK_NEXT) || camera_key_down(state, VK_NUMPAD3) ||
      camera_key_down(state, VK_NUMPAD6) || camera_key_down(state, VK_NUMPAD9);
  const bool left =
      camera_key_down(state, VK_LEFT) || camera_key_down(state, VK_HOME) ||
      camera_key_down(state, VK_END) || camera_key_down(state, VK_NUMPAD1) ||
      camera_key_down(state, VK_NUMPAD4) || camera_key_down(state, VK_NUMPAD7);
  vertical = down ? 1 : (up ? -1 : 0);
  horizontal = right ? 1 : (left ? -1 : 0);

  // cursor.cpp::sub_44AA10 at 0x0044AA10 uses a two-pixel edge band.
  // Its 478 bottom edge belonged to the original 480-line surface; this
  // stretched recovery surface has the corresponding logical edge at 398.
  if (horizontal == 0 && vertical == 0 && state.mouse_in_client) {
    horizontal = state.mouse_game_x <= 1
                     ? -1
                     : (state.mouse_game_x >= kMapViewportWidth - 2 ? 1 : 0);
    vertical = state.mouse_game_y <= 1
                   ? -1
                   : (state.mouse_game_y >= kMapViewportHeight - 2 ? 1 : 0);
  }
  if (horizontal == 0 && vertical == 0) {
    state.camera_scroll_ramp = 0;
    return false;
  }
  if (state.camera_scroll_ramp < 6U) {
    ++state.camera_scroll_ramp;
  }
  const int distance =
      kCameraScrollSpeeds[kDefaultCameraScrollSpeed][state.camera_scroll_ramp];
  const bool moved = set_camera_position(
      *status, static_cast<int>(status->camera_x) + horizontal * distance,
      static_cast<int>(status->camera_y) + vertical * distance);
  if (moved && status->placement_active && state.mouse_in_client) {
    (void)update_building_placement(*status, state.mouse_game_x,
                                    state.mouse_game_y);
  }
  return moved;
}

} // namespace starcraft::recovery
