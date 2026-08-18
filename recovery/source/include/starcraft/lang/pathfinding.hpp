#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace starcraft::game {
class MultiplayerScenario;
}

namespace starcraft::gds {
class TilesetData;
}

namespace starcraft::lang {

struct PathPoint {
  std::uint16_t x{};
  std::uint16_t y{};
};

struct PathObstacle {
  int left{};
  int top{};
  int right{};
  int bottom{};
};

struct PathRequest {
  std::size_t request_id{};
  std::uint16_t start_x{};
  std::uint16_t start_y{};
  std::uint16_t target_x{};
  std::uint16_t target_y{};
  std::uint16_t mover_width{};
  std::uint16_t mover_height{};
  std::vector<PathObstacle> obstacles{};
};

struct PathResult {
  std::size_t request_id{};
  bool found{};
  std::vector<PathPoint> path{};
};

class PathingMap final {
 public:
  [[nodiscard]] bool build(
      const gds::TilesetData& tileset,
      const game::MultiplayerScenario& scenario) noexcept;
  [[nodiscard]] bool assign(
      std::uint16_t minitile_width,
      std::uint16_t minitile_height,
      std::vector<std::uint8_t> walkable) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::uint16_t minitile_width() const noexcept;
  [[nodiscard]] std::uint16_t minitile_height() const noexcept;
  [[nodiscard]] std::uint16_t pixel_width() const noexcept;
  [[nodiscard]] std::uint16_t pixel_height() const noexcept;
  [[nodiscard]] bool walkable(std::uint16_t x, std::uint16_t y) const noexcept;
  [[nodiscard]] bool walkable_rectangle(
      int first_x,
      int first_y,
      int last_x,
      int last_y) const noexcept;

 private:
  std::uint16_t width_{};
  std::uint16_t height_{};
  std::vector<std::uint8_t> walkable_{};
  // Summed-area count of blocked minitiles. CUnitPath tests rectangular
  // footprints repeatedly while expanding a path; this preserves the same
  // CV5 walkability result without rescanning every minitile in the body.
  std::vector<std::uint32_t> blocked_prefix_{};
};

[[nodiscard]] bool path_position_passable(
    const PathingMap& map,
    int center_x,
    int center_y,
    std::uint16_t mover_width,
    std::uint16_t mover_height,
    const std::vector<PathObstacle>& obstacles) noexcept;

[[nodiscard]] bool find_unit_path(
    const PathingMap& map,
    std::uint16_t start_x,
    std::uint16_t start_y,
    std::uint16_t target_x,
    std::uint16_t target_y,
    std::uint16_t mover_width,
    std::uint16_t mover_height,
    const std::vector<PathObstacle>& obstacles,
    std::vector<PathPoint>& output) noexcept;

// Persistent workers parallelize independent unit searches. A batch is a
// deterministic barrier: every result is written to its request slot and the
// caller applies results in its own stable order after find_paths returns.
class PathfindingExecutor final {
 public:
  explicit PathfindingExecutor(std::size_t worker_count = 0) noexcept;
  ~PathfindingExecutor();

  PathfindingExecutor(const PathfindingExecutor&) = delete;
  PathfindingExecutor& operator=(const PathfindingExecutor&) = delete;

  [[nodiscard]] bool find_paths(
      const PathingMap& map,
      const std::vector<PathRequest>& requests,
      std::vector<PathResult>& results) noexcept;
  [[nodiscard]] std::size_t worker_count() const noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_{};
};

}  // namespace starcraft::lang
