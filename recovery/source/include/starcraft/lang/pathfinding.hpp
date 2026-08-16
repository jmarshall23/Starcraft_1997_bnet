#pragma once

#include <cstdint>
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

 private:
  std::uint16_t width_{};
  std::uint16_t height_{};
  std::vector<std::uint8_t> walkable_{};
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

}  // namespace starcraft::lang
