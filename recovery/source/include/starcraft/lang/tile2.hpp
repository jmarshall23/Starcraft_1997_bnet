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
  // mapcomn/creep.cpp keeps this sg_pEdgeMap beside sg_pCreepMap. Zero is
  // no edge; non-zero values select a frame from the tileset .grp.
  std::vector<std::uint8_t> edge_frame{};
};

[[nodiscard]] bool is_creep_source(std::uint16_t unit_type) noexcept;

// Returns the one-based Creep.grp frame chosen by sub_4D0A79 for the nine-bit
// neighbor mask consumed by sub_4D0D1E. Zero means no edge image.
[[nodiscard]] std::uint8_t
creep_edge_frame_for_neighbor_mask(std::uint16_t mask) noexcept;

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
