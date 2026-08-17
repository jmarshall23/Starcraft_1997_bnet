#include "starcraft/lang/los.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace starcraft::lang {
namespace {

// The seven-dword headers at original VA 0x004F88D0 contain these exact
// odd dimensions. All twelve masks use the same 3x3 inner rectangle.
constexpr std::array<std::array<std::uint8_t, 2>, 12> kSightDimensions{{
    {{3U, 3U}}, {{3U, 3U}}, {{5U, 5U}}, {{7U, 5U}},
    {{9U, 7U}}, {{11U, 9U}}, {{13U, 11U}}, {{15U, 13U}},
    {{17U, 13U}}, {{19U, 15U}}, {{21U, 17U}}, {{23U, 17U}},
}};

SightMask build_mask(const std::uint8_t type) {
  SightMask result{};
  result.width = kSightDimensions[type][0];
  result.height = kSightDimensions[type][1];
  const int half_width = result.width / 2;
  const int half_height = result.height / 2;
  std::vector<std::array<int, 2>> positions;
  positions.reserve(static_cast<std::size_t>(result.width) * result.height);

  // sub_46AC00 starts with width-height+1 cells, expands each mirrored row
  // by two, and caps at the full width. This is the original rounded sight
  // footprint, rather than an inferred Euclidean circle.
  int span = static_cast<int>(result.width) - result.height + 1;
  std::vector<bool> occupied(static_cast<std::size_t>(result.width) *
                             result.height);
  for (int row = 0; row <= half_height; ++row) {
    if (span < result.width) {
      span += 2;
    }
    const int first = (static_cast<int>(result.width) - span) / 2;
    const int last = first + span;
    for (int column = first; column < last; ++column) {
      occupied[static_cast<std::size_t>(row) * result.width + column] = true;
      occupied[static_cast<std::size_t>(result.height - 1 - row) *
                   result.width +
               column] = true;
    }
  }
  for (int y = -half_height; y <= half_height; ++y) {
    for (int x = -half_width; x <= half_width; ++x) {
      if (occupied[static_cast<std::size_t>(y + half_height) * result.width +
                   x + half_width]) {
        positions.push_back({x, y});
      }
    }
  }
  std::stable_sort(positions.begin(), positions.end(),
                   [](const auto &left, const auto &right) {
    const int left_ring = (std::max)(std::abs(left[0]), std::abs(left[1]));
    const int right_ring =
        (std::max)(std::abs(right[0]), std::abs(right[1]));
    if (left_ring != right_ring) {
      return left_ring < right_ring;
    }
    const int left_radius = left[0] * left[0] + left[1] * left[1];
    const int right_radius = right[0] * right[0] + right[1] * right[1];
    if (left_radius != right_radius) {
      return left_radius < right_radius;
    }
    return left[1] != right[1] ? left[1] < right[1]
                               : left[0] < right[0];
  });

  const auto position_index = [&positions](const int x,
                                            const int y) -> std::uint16_t {
    const auto found = std::find_if(
        positions.begin(), positions.end(), [x, y](const auto &position) {
          return position[0] == x && position[1] == y;
        });
    return found == positions.end()
               ? std::numeric_limits<std::uint16_t>::max()
               : static_cast<std::uint16_t>(found - positions.begin());
  };

  result.cells.reserve(positions.size());
  for (std::size_t index = 0; index < positions.size(); ++index) {
    const int x = positions[index][0];
    const int y = positions[index][1];
    SightMaskCell cell{};
    cell.x = static_cast<std::int8_t>(x);
    cell.y = static_cast<std::int8_t>(y);
    cell.inner = std::abs(x) <= 1 && std::abs(y) <= 1;
    if (cell.inner) {
      ++result.inner_count;
      cell.dependencies = {
          static_cast<std::uint16_t>(index),
          static_cast<std::uint16_t>(index),
      };
    } else {
      // los.cpp::sub_46AEF0 first steps both coordinates one cell toward
      // the origin. On a diagonal or axis that single cell is both
      // dependencies. Otherwise it restores the coordinate on the longer
      // axis, producing the second adjacent predecessor. This is the exact
      // integer dependency rule used by the retail LOS nodes; no angular or
      // Euclidean approximation is involved.
      const int step_x = (x > 0) - (x < 0);
      const int step_y = (y > 0) - (y < 0);
      const int diagonal_x = x - step_x;
      const int diagonal_y = y - step_y;
      int second_x = diagonal_x;
      int second_y = diagonal_y;
      if (x != 0 && y != 0 && std::abs(x) != std::abs(y)) {
        if (std::abs(x) <= std::abs(y)) {
          second_x = x;
        } else {
          second_y = y;
        }
      }
      cell.dependencies = {
          position_index(diagonal_x, diagonal_y),
          position_index(second_x, second_y),
      };
      if (cell.dependencies[0] >= index ||
          cell.dependencies[1] >= index) {
        // Every retail node depends on an already-emitted inner ring. Keep
        // the table valid even for a malformed replacement header.
        cell.dependencies = {0U, 0U};
      }
    }
    result.cells.push_back(cell);
  }
  return result;
}

const std::array<SightMask, 12> &all_masks() {
  static const std::array<SightMask, 12> masks = [] {
    std::array<SightMask, 12> value{};
    for (std::uint8_t type = 0; type < value.size(); ++type) {
      value[type] = build_mask(type);
    }
    return value;
  }();
  return masks;
}

}  // namespace

const SightMask &sight_mask(const std::uint8_t type) noexcept {
  return all_masks()[(std::min)(static_cast<std::size_t>(type),
                                all_masks().size() - 1U)];
}

}  // namespace starcraft::lang
