#include "../../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/tile2.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::recovery {

namespace {

[[nodiscard]] CreepSourceRuntimeState creep_source_state(
    const ScenarioUnitPreview &source) noexcept {
  return {
      source.unit_type,
      source.x,
      source.y,
      source.selection_width,
      source.selection_height,
      source.alive,
      source.construction_complete,
  };
}

[[nodiscard]] bool same_creep_source(
    const CreepSourceRuntimeState &left,
    const CreepSourceRuntimeState &right) noexcept {
  return left.unit_type == right.unit_type && left.x == right.x &&
         left.y == right.y && left.placement_width == right.placement_width &&
         left.placement_height == right.placement_height &&
         left.alive == right.alive && left.complete == right.complete;
}

[[nodiscard]] bool creep_source_state_is_current(
    const BootstrapStatus &status, const std::size_t tile_count) noexcept {
  if (!status.creep_source_state_ready ||
      status.creep_tiles.size() != tile_count ||
      status.creep_visual_tiles.size() != tile_count ||
      status.creep_edge_frames.size() != tile_count ||
      (status.scenario.valid() && status.terrain_tileset.valid() &&
       (!status.terrain_ready || !status.minimap_ready))) {
    return false;
  }
  std::size_t state_index{};
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!starcraft::lang::is_creep_source(unit.unit_type)) {
      continue;
    }
    if (state_index >= status.creep_source_states.size() ||
        !same_creep_source(status.creep_source_states[state_index],
                           creep_source_state(unit))) {
      return false;
    }
    ++state_index;
  }
  return state_index == status.creep_source_states.size();
}

} // namespace

bool rebuild_creep_tiles(BootstrapStatus &status) noexcept {
  try {
    const std::size_t tile_count =
        static_cast<std::size_t>(status.scenario_width) *
        status.scenario_height;
    // CUnitUpdate.cpp::sub_4446F0 calls tile2.cpp::sub_4B2A10 only for a
    // CUnit carrying the creep-update state. The former runtime instead
    // regenerated every map tile after ordinary AI/harvest/combat changes.
    // An exact snapshot of sub_4B2A10's inputs preserves that scheduling in
    // the flattened recovery model.
    if (creep_source_state_is_current(status, tile_count)) {
      return true;
    }
    std::vector<starcraft::lang::CreepSource> sources;
    std::vector<CreepSourceRuntimeState> source_states;
    sources.reserve(status.units.size());
    source_states.reserve(status.units.size());
    for (const ScenarioUnitPreview &source : status.units) {
      if (!starcraft::lang::is_creep_source(source.unit_type)) {
        continue;
      }
      source_states.push_back(creep_source_state(source));
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
    const bool complete = status.creep_tiles.size() == tile_count &&
                          status.creep_visual_tiles.size() == tile_count &&
                          status.creep_edge_frames.size() == tile_count;
    if (complete) {
      status.creep_source_states = std::move(source_states);
      status.creep_source_state_ready = true;
      ++status.creep_rebuild_count;
    }
    return complete;
  } catch (...) {
    status.creep_tiles.clear();
    status.creep_visual_tiles.clear();
    status.creep_edge_frames.clear();
    status.creep_source_state_ready = false;
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
