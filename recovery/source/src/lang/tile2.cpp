#include "starcraft/lang/tile2.hpp"

#include "starcraft/staredit/mapcomn/creep.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::lang {

bool is_creep_source(const std::uint16_t unit_type) noexcept {
  switch (unit_type) {
    case 131:
    case 132:
    case 133:
    case 143:
    case 144:
    case 146:
      return true;
    default:
      return false;
  }
}

bool rebuild_creep_tile_state(
    const std::uint16_t map_tile_width,
    const std::uint16_t map_tile_height,
    const CreepSource* const sources,
    const std::size_t source_count,
    const CreepTileState& previous,
    CreepTileState& output) noexcept {
  output = {};
  if ((sources == nullptr && source_count != 0U) || map_tile_width == 0U ||
      map_tile_height == 0U) {
    return false;
  }
  try {
    const std::size_t tile_count =
        static_cast<std::size_t>(map_tile_width) * map_tile_height;
    output.occupied.assign(tile_count, 0U);
    output.visual_tile.assign(tile_count, 0U);
    for (std::size_t source_index = 0; source_index < source_count; ++source_index) {
      const CreepSource& source = sources[source_index];
      if (!source.alive || !source.complete || !is_creep_source(source.unit_type)) {
        continue;
      }
      staredit::mapcomn::CreepBounds bounds{};
      if (!staredit::mapcomn::creep_bounds(
              source.unit_type,
              source.x,
              source.y,
              source.placement_width,
              source.placement_height,
              true,
              map_tile_width,
              map_tile_height,
              bounds)) {
        continue;
      }
      for (int tile_y = bounds.top; tile_y <= bounds.bottom; ++tile_y) {
        for (int tile_x = bounds.left; tile_x <= bounds.right; ++tile_x) {
          if (bounds.elliptical &&
              !staredit::mapcomn::creep_ellipse_contains_tile(
                  source.x, source.y, tile_x, tile_y)) {
            continue;
          }
          output.occupied[static_cast<std::size_t>(tile_y) * map_tile_width +
                          tile_x] = 1U;
        }
      }
    }
    for (std::size_t tile = 0; tile < tile_count; ++tile) {
      if (output.occupied[tile] == 0U) {
        continue;
      }
      if (previous.occupied.size() == tile_count &&
          previous.visual_tile.size() == tile_count &&
          previous.occupied[tile] != 0U && previous.visual_tile[tile] >= 16U &&
          previous.visual_tile[tile] <= 24U) {
        output.visual_tile[tile] = previous.visual_tile[tile];
      } else {
        output.visual_tile[tile] =
            staredit::mapcomn::choose_creep_visual_tile();
      }
    }
    return true;
  } catch (...) {
    output = {};
    return false;
  }
}

}  // namespace starcraft::lang
