#include "starcraft/lang/tile2.hpp"

#include "starcraft/staredit/mapcomn/creep.hpp"

#include <cstddef>
#include <cstdint>
#include <array>
#include <vector>

namespace starcraft::lang {
namespace {

// Exact dword_4E6AD8/dword_4E6ADC order consumed by
// mapcomn/creep.cpp::sub_4D0D1E. Bit eight is the center tile.
constexpr std::array<std::array<int, 2>, 9> kCreepNeighbors{{
    {{1, 1}}, {{0, 1}}, {{-1, 1}}, {{1, 0}}, {{-1, 0}},
    {{1, -1}}, {{0, -1}}, {{-1, -1}}, {{0, 0}},
}};

std::uint8_t canonical_edge_code(const std::uint16_t mask) noexcept {
  // Literal recovery of sub_4D091A's 512-entry mask normalization table.
  if ((mask & 0x100U) != 0U) {
    return 127U;
  }
  std::uint8_t code{};
  if ((mask & 0x10U) != 0U) code |= 0x09U;
  if ((mask & 0x08U) != 0U) code |= 0x24U;
  if ((mask & 0x40U) != 0U) code |= 0x02U;
  if ((mask & 0x02U) != 0U) code |= 0x10U;
  if ((mask & 0xC0U) == 0xC0U) code |= 0x01U;
  if ((mask & 0x60U) == 0x60U) code |= 0x04U;
  if ((mask & 0x06U) == 0x06U) code |= 0x08U;
  if ((mask & 0x03U) == 0x03U) code |= 0x20U;
  if ((code & 0x21U) == 0x21U || (code & 0x0CU) == 0x0CU) {
    code |= 0x40U;
  }
  return code;
}

const std::array<std::uint8_t, 128>& compact_edge_frames() noexcept {
  // sub_4D0A79 walks canonical codes in ascending order, scans all 512 masks
  // for each code, and assigns a dense frame index when that code exists.
  // The ordering matters: walking masks first selects the right shapes but
  // assigns the wrong GRP frame to every non-trivial edge.
  static const std::array<std::uint8_t, 128> frames = [] {
    std::array<std::uint8_t, 128> result{};
    std::array<bool, 128> generated{};
    for (std::uint16_t mask = 0; mask < 512U; ++mask) {
      generated[canonical_edge_code(mask)] = true;
    }
    std::uint8_t next{};
    for (std::size_t code = 0; code < generated.size(); ++code) {
      if (generated[code]) {
        result[code] = next++;
      }
    }
    return result;
  }();
  return frames;
}

}  // namespace

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

std::uint8_t
creep_edge_frame_for_neighbor_mask(const std::uint16_t mask) noexcept {
  return compact_edge_frames()[canonical_edge_code(mask & 0x1FFU)];
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
    output.edge_frame.assign(tile_count, 0U);
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
    for (int tile_y = 0; tile_y < map_tile_height; ++tile_y) {
      for (int tile_x = 0; tile_x < map_tile_width; ++tile_x) {
        const std::size_t tile =
            static_cast<std::size_t>(tile_y) * map_tile_width + tile_x;
        if (output.occupied[tile] != 0U) {
          continue;
        }
        std::uint16_t neighbor_mask{};
        for (std::size_t neighbor = 0; neighbor < kCreepNeighbors.size();
             ++neighbor) {
          const int x = tile_x + kCreepNeighbors[neighbor][0];
          const int y = tile_y + kCreepNeighbors[neighbor][1];
          if (x >= 0 && y >= 0 && x < map_tile_width &&
              y < map_tile_height &&
              output.occupied[static_cast<std::size_t>(y) * map_tile_width +
                              x] != 0U) {
            neighbor_mask |= static_cast<std::uint16_t>(1U << neighbor);
          }
        }
        output.edge_frame[tile] =
            creep_edge_frame_for_neighbor_mask(neighbor_mask);
      }
    }
    return true;
  } catch (...) {
    output = {};
    return false;
  }
}

}  // namespace starcraft::lang
