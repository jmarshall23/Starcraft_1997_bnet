#include "terrain/terrain_brush.hpp"

#include <cstddef>

namespace staredit::terrain {
namespace {

constexpr std::uint8_t kMaximumBrushDimension = 15U;

}  // namespace

bool valid_brush_dimensions(const TerrainBrush& brush) noexcept {
  return brush.width != 0U && brush.height != 0U &&
         brush.width <= kMaximumBrushDimension &&
         brush.height <= kMaximumBrushDimension &&
         (brush.width & 1U) != 0U && (brush.height & 1U) != 0U;
}

bool make_rectangular_stamp(const TerrainBrush& brush,
                            const std::uint16_t anchor_x,
                            const std::uint16_t anchor_y,
                            const std::uint16_t map_width,
                            const std::uint16_t map_height,
                            std::vector<TerrainStampCell>& output) noexcept {
  output.clear();
  if (!valid_brush_dimensions(brush) || map_width == 0U || map_height == 0U ||
      anchor_x >= map_width || anchor_y >= map_height) {
    return false;
  }
  try {
    output.reserve(static_cast<std::size_t>(brush.width) * brush.height);
    const int first_x = static_cast<int>(anchor_x) - brush.width / 2;
    const int first_y = static_cast<int>(anchor_y) - brush.height / 2;
    for (int row = 0; row < brush.height; ++row) {
      const int y = first_y + row;
      if (y < 0 || y >= map_height) {
        continue;
      }
      for (int column = 0; column < brush.width; ++column) {
        const int x = first_x + column;
        if (x >= 0 && x < map_width) {
          output.push_back({static_cast<std::uint16_t>(x),
                            static_cast<std::uint16_t>(y), brush.tile_id});
        }
      }
    }
    return !output.empty();
  } catch (...) {
    output.clear();
    return false;
  }
}

}  // namespace staredit::terrain
