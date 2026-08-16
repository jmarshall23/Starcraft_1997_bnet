#include "starcraft/staredit/mapcomn/creep.hpp"

#include <algorithm>
#include <cstdlib>

namespace starcraft::staredit::mapcomn {

bool creep_uses_ellipse(
    const std::uint16_t unit_type,
    const bool completed_flag) noexcept {
  switch (unit_type) {
    case 131:  // Hatchery
    case 143:  // Creep Colony
      return completed_flag;
    case 132:  // Lair
    case 133:  // Hive
    case 144:  // Sunken Colony
    case 146:  // Spore Colony
      return true;
    default:
      return false;
  }
}

bool creep_bounds(
    const std::uint16_t unit_type,
    const int center_x,
    const int center_y,
    const std::uint16_t placement_width,
    const std::uint16_t placement_height,
    const bool completed_flag,
    const int map_tile_width,
    const int map_tile_height,
    CreepBounds& output) noexcept {
  if (map_tile_width <= 0 || map_tile_height <= 0) {
    return false;
  }
  output.elliptical = creep_uses_ellipse(unit_type, completed_flag);
  if (output.elliptical) {
    output.left = (center_x - 320) / 32;
    output.right = (center_x + 320) / 32;
    output.top = (center_y - 200) / 32;
    output.bottom = (center_y + 200) / 32;
  } else {
    const int width = placement_width / 32U;
    const int height = placement_height / 32U;
    if (width <= 0 || height <= 0) {
      return false;
    }
    output.left = center_x / 32 - width / 2;
    output.top = center_y / 32 - height / 2;
    output.right = output.left + width - 1;
    output.bottom = output.top + height - 1;
  }
  output.left = (std::max)(0, output.left);
  output.top = (std::max)(0, output.top);
  output.right = (std::min)(map_tile_width - 1, output.right);
  output.bottom = (std::min)(map_tile_height - 1, output.bottom);
  return output.left <= output.right && output.top <= output.bottom;
}

bool creep_ellipse_contains_tile(
    const int center_x,
    const int center_y,
    const int tile_x,
    const int tile_y) noexcept {
  const int dx = 32 * tile_x - center_x + 16;
  const int dy = 32 * tile_y - center_y + 16;
  return 256LL * dy * dy + 100LL * dx * dx <= 10240000LL;
}

std::uint8_t choose_creep_visual_tile() noexcept {
  // sub_4D0E07 writes rand()%9+16: CV5 group 1, members 0..8.
  return static_cast<std::uint8_t>(std::rand() % 9 + 16);
}

}  // namespace starcraft::staredit::mapcomn
