#pragma once

#include <cstdint>
#include <vector>

namespace staredit::terrain {

struct TerrainBrush {
  std::uint16_t tile_id{};
  std::uint8_t width{1U};
  std::uint8_t height{1U};
};

struct TerrainStampCell {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t tile_id{};
};

[[nodiscard]] bool valid_brush_dimensions(
    const TerrainBrush& brush) noexcept;

// Builds a clipped, row-major rectangular stamp centered on the anchor tile.
// Odd dimensions make the anchor stable while dragging and match the recovered
// editor's tile-centered mouse model.
[[nodiscard]] bool make_rectangular_stamp(
    const TerrainBrush& brush,
    std::uint16_t anchor_x,
    std::uint16_t anchor_y,
    std::uint16_t map_width,
    std::uint16_t map_height,
    std::vector<TerrainStampCell>& output) noexcept;

}  // namespace staredit::terrain
