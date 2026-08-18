#pragma once

#include "formats/isom_section.hpp"
#include "starcraft/gds/tileset.hpp"
#include "terrain/isom_catalog.hpp"
#include "terrain/isom_topology.hpp"

#include <cstdint>
#include <vector>

namespace staredit::terrain {

class IsomTerrainEditor final {
 public:
  [[nodiscard]] static bool paint(
      formats::IsomSection& isom,
      std::vector<std::uint16_t>& editor_tiles,
      std::vector<std::uint16_t>& game_tiles,
      std::uint16_t map_width,
      std::uint16_t map_height,
      std::uint16_t tile_x,
      std::uint16_t tile_y,
      std::uint16_t terrain_type,
      std::uint16_t brush_extent,
      const IsomCatalog& catalog,
      const IsomTopology& topology,
      const starcraft::gds::TilesetData& tileset) noexcept;
};

}  // namespace staredit::terrain
