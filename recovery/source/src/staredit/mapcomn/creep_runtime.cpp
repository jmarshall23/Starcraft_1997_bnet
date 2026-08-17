#include "../../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/tile2.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool rebuild_creep_tiles(BootstrapStatus &status) noexcept {
  try {
    const std::size_t tile_count =
        static_cast<std::size_t>(status.scenario_width) *
        status.scenario_height;
    std::vector<starcraft::lang::CreepSource> sources;
    sources.reserve(status.units.size());
    for (const ScenarioUnitPreview &source : status.units) {
      if (!starcraft::lang::is_creep_source(source.unit_type)) {
        continue;
      }
      sources.push_back({
          source.unit_type,
          source.x,
          source.y,
          source.selection_width,
          source.selection_height,
          source.alive,
          source.construction_complete,
      });
    }
    starcraft::lang::CreepTileState previous{
        status.creep_tiles,
        status.creep_visual_tiles,
        status.creep_edge_frames,
    };
    starcraft::lang::CreepTileState rebuilt{};
    if (!starcraft::lang::rebuild_creep_tile_state(
            status.scenario_width, status.scenario_height, sources.data(),
            sources.size(), previous, rebuilt)) {
      return false;
    }
    status.creep_tiles = std::move(rebuilt.occupied);
    status.creep_visual_tiles = std::move(rebuilt.visual_tile);
    status.creep_edge_frames = std::move(rebuilt.edge_frame);
    if (status.scenario.valid() && status.terrain_tileset.valid()) {
      SpritePreviewFrame terrain;
      if (!build_terrain_preview(status.terrain_tileset, status.scenario,
                                 status.camera_x, status.camera_y, terrain,
                                 &status.creep_tiles,
                                 &status.creep_visual_tiles,
                                 &status.creep_edge_frames)) {
        return false;
      }
      status.terrain = std::move(terrain);
      status.terrain_ready = true;
      SpritePreviewFrame minimap;
      if (!build_minimap_preview(
              status.terrain_tileset, status.scenario, minimap,
              status.minimap_content_x, status.minimap_content_y,
              status.minimap_content_width, status.minimap_content_height,
              &status.creep_tiles, &status.creep_visual_tiles)) {
        return false;
      }
      status.minimap = std::move(minimap);
      status.minimap_ready = true;
    }
    return status.creep_tiles.size() == tile_count &&
           status.creep_visual_tiles.size() == tile_count &&
           status.creep_edge_frames.size() == tile_count;
  } catch (...) {
    status.creep_tiles.clear();
    status.creep_visual_tiles.clear();
    status.creep_edge_frames.clear();
    return false;
  }
}

bool tile_has_creep(const BootstrapStatus &status, const int tile_x,
                    const int tile_y) noexcept {
  return tile_x >= 0 && tile_y >= 0 && tile_x < status.scenario_width &&
         tile_y < status.scenario_height &&
         status.creep_tiles.size() ==
             static_cast<std::size_t>(status.scenario_width) *
                 status.scenario_height &&
         status.creep_tiles[static_cast<std::size_t>(tile_y) *
                                status.scenario_width +
                            tile_x] != 0U;
}

} // namespace starcraft::recovery
