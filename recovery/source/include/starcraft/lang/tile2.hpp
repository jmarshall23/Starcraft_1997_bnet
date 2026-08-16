#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::lang {

struct CreepSource {
  std::uint16_t unit_type{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t placement_width{};
  std::uint16_t placement_height{};
  bool alive{};
  bool complete{};
};

struct CreepTileState {
  std::vector<std::uint8_t> occupied{};
  std::vector<std::uint8_t> visual_tile{};
};

[[nodiscard]] bool is_creep_source(std::uint16_t unit_type) noexcept;

// Recovered live source-mask behavior from U:\SWAR\lang\tile2.cpp,
// principally sub_4B2A10, using mapcomn's exact source bounds/ellipse.
[[nodiscard]] bool rebuild_creep_tile_state(
    std::uint16_t map_tile_width,
    std::uint16_t map_tile_height,
    const CreepSource* sources,
    std::size_t source_count,
    const CreepTileState& previous,
    CreepTileState& output) noexcept;

}  // namespace starcraft::lang
