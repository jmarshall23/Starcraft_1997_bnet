#include "starcraft/lang/pathfinding.hpp"

#include "starcraft/game/scenario.hpp"
#include "starcraft/gds/tileset.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace starcraft::lang {
namespace {

constexpr int kMinitilePixels = 8;
constexpr int kStraightCost = 10;
constexpr int kDiagonalCost = 14;

int floor_divide(const int value, const int divisor) noexcept {
  return value >= 0 ? value / divisor : -((-value + divisor - 1) / divisor);
}

int octile_distance(
    const int x,
    const int y,
    const int target_x,
    const int target_y) noexcept {
  const int dx = std::abs(target_x - x);
  const int dy = std::abs(target_y - y);
  return kStraightCost * (std::max)(dx, dy) -
         (2 * kStraightCost - kDiagonalCost) * (std::min)(dx, dy);
}

bool segment_passable(
    const PathingMap& map,
    const int start_x,
    const int start_y,
    const int end_x,
    const int end_y,
    const std::uint16_t mover_width,
    const std::uint16_t mover_height,
    const std::vector<PathObstacle>& obstacles) noexcept {
  const int dx = end_x - start_x;
  const int dy = end_y - start_y;
  const int steps = (std::max)(std::abs(dx), std::abs(dy)) / 4 + 1;
  for (int step = 1; step <= steps; ++step) {
    const int x = start_x + static_cast<int>(
                                static_cast<std::int64_t>(dx) * step / steps);
    const int y = start_y + static_cast<int>(
                                static_cast<std::int64_t>(dy) * step / steps);
    if (!path_position_passable(
            map, x, y, mover_width, mover_height, obstacles)) {
      return false;
    }
  }
  return true;
}

struct OpenNode {
  int total_cost{};
  int heuristic{};
  int index{};
};

struct OpenNodeLater {
  bool operator()(const OpenNode& left, const OpenNode& right) const noexcept {
    if (left.total_cost != right.total_cost) {
      return left.total_cost > right.total_cost;
    }
    if (left.heuristic != right.heuristic) {
      return left.heuristic > right.heuristic;
    }
    return left.index > right.index;
  }
};

}  // namespace

bool PathingMap::build(
    const gds::TilesetData& tileset,
    const game::MultiplayerScenario& scenario) noexcept {
  if (!tileset.valid() || !scenario.valid() || scenario.width() > UINT16_MAX / 4U ||
      scenario.height() > UINT16_MAX / 4U) {
    return false;
  }
  try {
    const std::uint16_t width = static_cast<std::uint16_t>(scenario.width() * 4U);
    const std::uint16_t height = static_cast<std::uint16_t>(scenario.height() * 4U);
    std::vector<std::uint8_t> cells(static_cast<std::size_t>(width) * height);
    for (std::uint16_t tile_y = 0; tile_y < scenario.height(); ++tile_y) {
      for (std::uint16_t tile_x = 0; tile_x < scenario.width(); ++tile_x) {
        std::uint16_t map_tile{};
        if (!scenario.tile(tile_x, tile_y, map_tile)) {
          return false;
        }
        for (std::uint8_t mini_y = 0; mini_y < 4U; ++mini_y) {
          for (std::uint8_t mini_x = 0; mini_x < 4U; ++mini_x) {
            const std::size_t x = static_cast<std::size_t>(tile_x) * 4U + mini_x;
            const std::size_t y = static_cast<std::size_t>(tile_y) * 4U + mini_y;
            cells[y * width + x] = tileset.walkable(map_tile, mini_x, mini_y) ? 1U : 0U;
          }
        }
      }
    }
    return assign(width, height, std::move(cells));
  } catch (...) {
    return false;
  }
}

bool PathingMap::assign(
    const std::uint16_t minitile_width,
    const std::uint16_t minitile_height,
    std::vector<std::uint8_t> walkable) noexcept {
  if (minitile_width == 0 || minitile_height == 0 ||
      minitile_width > UINT16_MAX / kMinitilePixels ||
      minitile_height > UINT16_MAX / kMinitilePixels ||
      walkable.size() !=
          static_cast<std::size_t>(minitile_width) * minitile_height) {
    return false;
  }
  for (std::uint8_t& cell : walkable) {
    cell = cell == 0 ? 0U : 1U;
  }
  try {
    const std::size_t prefix_width =
        static_cast<std::size_t>(minitile_width) + 1U;
    std::vector<std::uint32_t> blocked_prefix(
        prefix_width * (static_cast<std::size_t>(minitile_height) + 1U));
    for (std::size_t y = 0; y < minitile_height; ++y) {
      std::uint32_t row_blocked{};
      for (std::size_t x = 0; x < minitile_width; ++x) {
        row_blocked += walkable[y * minitile_width + x] == 0U ? 1U : 0U;
        blocked_prefix[(y + 1U) * prefix_width + x + 1U] =
            blocked_prefix[y * prefix_width + x + 1U] + row_blocked;
      }
    }
    width_ = minitile_width;
    height_ = minitile_height;
    walkable_ = std::move(walkable);
    blocked_prefix_ = std::move(blocked_prefix);
    return true;
  } catch (...) {
    width_ = 0U;
    height_ = 0U;
    walkable_.clear();
    blocked_prefix_.clear();
    return false;
  }
}

bool PathingMap::valid() const noexcept {
  return width_ != 0 && height_ != 0 &&
         walkable_.size() == static_cast<std::size_t>(width_) * height_ &&
         blocked_prefix_.size() ==
             (static_cast<std::size_t>(width_) + 1U) *
                 (static_cast<std::size_t>(height_) + 1U);
}

std::uint16_t PathingMap::minitile_width() const noexcept { return width_; }
std::uint16_t PathingMap::minitile_height() const noexcept { return height_; }
std::uint16_t PathingMap::pixel_width() const noexcept {
  return static_cast<std::uint16_t>(width_ * kMinitilePixels);
}
std::uint16_t PathingMap::pixel_height() const noexcept {
  return static_cast<std::uint16_t>(height_ * kMinitilePixels);
}

bool PathingMap::walkable(const std::uint16_t x, const std::uint16_t y) const noexcept {
  return valid() && x < width_ && y < height_ &&
         walkable_[static_cast<std::size_t>(y) * width_ + x] != 0;
}

bool PathingMap::walkable_rectangle(const int first_x, const int first_y,
                                    const int last_x,
                                    const int last_y) const noexcept {
  if (!valid() || first_x < 0 || first_y < 0 || last_x < first_x ||
      last_y < first_y || last_x >= width_ || last_y >= height_) {
    return false;
  }
  const std::size_t prefix_width = static_cast<std::size_t>(width_) + 1U;
  const std::size_t left = static_cast<std::size_t>(first_x);
  const std::size_t top = static_cast<std::size_t>(first_y);
  const std::size_t right = static_cast<std::size_t>(last_x) + 1U;
  const std::size_t bottom = static_cast<std::size_t>(last_y) + 1U;
  const std::uint32_t blocked =
      blocked_prefix_[bottom * prefix_width + right] -
      blocked_prefix_[top * prefix_width + right] -
      blocked_prefix_[bottom * prefix_width + left] +
      blocked_prefix_[top * prefix_width + left];
  return blocked == 0U;
}

bool path_position_passable(
    const PathingMap& map,
    const int center_x,
    const int center_y,
    const std::uint16_t mover_width,
    const std::uint16_t mover_height,
    const std::vector<PathObstacle>& obstacles) noexcept {
  if (!map.valid() || mover_width == 0 || mover_height == 0) {
    return false;
  }
  const int left = center_x - static_cast<int>(mover_width) / 2;
  const int top = center_y - static_cast<int>(mover_height) / 2;
  const int right = left + mover_width;
  const int bottom = top + mover_height;
  if (left < 0 || top < 0 || right > map.pixel_width() ||
      bottom > map.pixel_height()) {
    return false;
  }
  const int first_x = floor_divide(left, kMinitilePixels);
  const int first_y = floor_divide(top, kMinitilePixels);
  const int last_x = floor_divide(right - 1, kMinitilePixels);
  const int last_y = floor_divide(bottom - 1, kMinitilePixels);
  if (!map.walkable_rectangle(first_x, first_y, last_x, last_y)) {
    return false;
  }
  for (const PathObstacle& obstacle : obstacles) {
    if (left < obstacle.right && right > obstacle.left && top < obstacle.bottom &&
        bottom > obstacle.top) {
      return false;
    }
  }
  return true;
}

bool find_unit_path(
    const PathingMap& map,
    const std::uint16_t start_x,
    const std::uint16_t start_y,
    const std::uint16_t target_x,
    const std::uint16_t target_y,
    const std::uint16_t mover_width,
    const std::uint16_t mover_height,
    const std::vector<PathObstacle>& obstacles,
    std::vector<PathPoint>& output) noexcept {
  output.clear();
  if (!map.valid() || start_x >= map.pixel_width() || start_y >= map.pixel_height() ||
      target_x >= map.pixel_width() || target_y >= map.pixel_height()) {
    return false;
  }
  try {
    const bool exact_target_passable = path_position_passable(
        map, target_x, target_y, mover_width, mover_height, obstacles);
    if (exact_target_passable && segment_passable(
                                     map,
                                     start_x,
                                     start_y,
                                     target_x,
                                     target_y,
                                     mover_width,
                                     mover_height,
                                     obstacles)) {
      output.push_back({target_x, target_y});
      return true;
    }

    const int grid_width = map.minitile_width();
    const int grid_height = map.minitile_height();
    const int node_count = grid_width * grid_height;
    std::vector<std::int8_t> passability_cache(
        static_cast<std::size_t>(node_count), -1);
    auto node_x = [grid_width](const int index) { return index % grid_width; };
    auto node_y = [grid_width](const int index) { return index / grid_width; };
    auto center_x = [](const int x) { return x * kMinitilePixels + kMinitilePixels / 2; };
    auto center_y = [](const int y) { return y * kMinitilePixels + kMinitilePixels / 2; };
    const auto cell_passable = [&](const int x, const int y) {
      if (x < 0 || y < 0 || x >= grid_width || y >= grid_height) {
        return false;
      }
      std::int8_t &cached = passability_cache[static_cast<std::size_t>(
          y * grid_width + x)];
      if (cached < 0) {
        cached = path_position_passable(map, center_x(x), center_y(y),
                                        mover_width, mover_height, obstacles)
                     ? 1
                     : 0;
      }
      return cached != 0;
    };

    int start_cell_x = start_x / kMinitilePixels;
    int start_cell_y = start_y / kMinitilePixels;
    if (!cell_passable(start_cell_x, start_cell_y)) {
      bool found{};
      for (int radius = 1; radius <= 8 && !found; ++radius) {
        for (int y = start_cell_y - radius; y <= start_cell_y + radius && !found; ++y) {
          for (int x = start_cell_x - radius; x <= start_cell_x + radius; ++x) {
            if ((std::max)(std::abs(x - start_cell_x), std::abs(y - start_cell_y)) ==
                    radius &&
                cell_passable(x, y)) {
              start_cell_x = x;
              start_cell_y = y;
              found = true;
              break;
            }
          }
        }
      }
      if (!found) {
        return false;
      }
    }

    const int target_cell_x = target_x / kMinitilePixels;
    const int target_cell_y = target_y / kMinitilePixels;
    std::vector<std::uint8_t> goal(static_cast<std::size_t>(node_count));
    bool goal_found{};
    for (int radius = 0; radius < (std::max)(grid_width, grid_height) && !goal_found;
         ++radius) {
      for (int y = target_cell_y - radius; y <= target_cell_y + radius; ++y) {
        for (int x = target_cell_x - radius; x <= target_cell_x + radius; ++x) {
          if ((std::max)(std::abs(x - target_cell_x), std::abs(y - target_cell_y)) !=
                  radius ||
              !cell_passable(x, y)) {
            continue;
          }
          goal[static_cast<std::size_t>(y * grid_width + x)] = 1;
          goal_found = true;
        }
      }
    }
    if (!goal_found) {
      return false;
    }

    const int start_index = start_cell_y * grid_width + start_cell_x;
    constexpr int infinity = (std::numeric_limits<int>::max)() / 4;
    std::vector<int> cost(static_cast<std::size_t>(node_count), infinity);
    std::vector<int> parent(static_cast<std::size_t>(node_count), -1);
    std::vector<std::uint8_t> closed(static_cast<std::size_t>(node_count));
    std::priority_queue<OpenNode, std::vector<OpenNode>, OpenNodeLater> open;
    cost[start_index] = 0;
    const int initial_heuristic = octile_distance(
        start_cell_x, start_cell_y, target_cell_x, target_cell_y);
    open.push({initial_heuristic, initial_heuristic, start_index});

    // CUnitPath's area queue at 0x00495A70 expands the same eight compass
    // neighbors and conditionally suppresses corner transitions. Keep a fixed
    // clockwise order so equal-cost paths are deterministic in lockstep.
    constexpr std::array<std::array<int, 2>, 8> directions{{
        {{0, -1}}, {{1, -1}}, {{1, 0}}, {{1, 1}},
        {{0, 1}},  {{-1, 1}}, {{-1, 0}}, {{-1, -1}},
    }};
    int reached{-1};
    while (!open.empty()) {
      const OpenNode current = open.top();
      open.pop();
      if (closed[current.index] != 0) {
        continue;
      }
      closed[current.index] = 1;
      if (goal[current.index] != 0) {
        reached = current.index;
        break;
      }
      const int current_x = node_x(current.index);
      const int current_y = node_y(current.index);
      for (const auto& direction : directions) {
        const int next_x = current_x + direction[0];
        const int next_y = current_y + direction[1];
        if (!cell_passable(next_x, next_y)) {
          continue;
        }
        const bool diagonal = direction[0] != 0 && direction[1] != 0;
        if (diagonal &&
            (!cell_passable(current_x + direction[0], current_y) ||
             !cell_passable(current_x, current_y + direction[1]))) {
          continue;
        }
        const int next_index = next_y * grid_width + next_x;
        const int next_cost = cost[current.index] +
                              (diagonal ? kDiagonalCost : kStraightCost);
        if (next_cost >= cost[next_index]) {
          continue;
        }
        cost[next_index] = next_cost;
        parent[next_index] = current.index;
        const int heuristic = octile_distance(
            next_x, next_y, target_cell_x, target_cell_y);
        open.push({next_cost + heuristic, heuristic, next_index});
      }
    }
    if (reached < 0) {
      return false;
    }

    std::vector<PathPoint> reversed;
    for (int node = reached; node != start_index && node >= 0; node = parent[node]) {
      reversed.push_back({
          static_cast<std::uint16_t>(center_x(node_x(node))),
          static_cast<std::uint16_t>(center_y(node_y(node))),
      });
    }
    if (reversed.empty()) {
      if (exact_target_passable) {
        output.push_back({target_x, target_y});
      }
      return !output.empty();
    }
    std::reverse(reversed.begin(), reversed.end());
    if (exact_target_passable) {
      reversed.back() = {target_x, target_y};
    }

    int cursor_x = start_x;
    int cursor_y = start_y;
    std::size_t begin{};
    while (begin < reversed.size()) {
      std::size_t furthest = begin;
      for (std::size_t candidate = reversed.size(); candidate > begin; --candidate) {
        const PathPoint& point = reversed[candidate - 1U];
        if (segment_passable(
                map,
                cursor_x,
                cursor_y,
                point.x,
                point.y,
                mover_width,
                mover_height,
                obstacles)) {
          furthest = candidate - 1U;
          break;
        }
      }
      output.push_back(reversed[furthest]);
      cursor_x = reversed[furthest].x;
      cursor_y = reversed[furthest].y;
      begin = furthest + 1U;
    }
    return !output.empty();
  } catch (...) {
    output.clear();
    return false;
  }
}

}  // namespace starcraft::lang
