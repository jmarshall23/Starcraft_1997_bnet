#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/los.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::uint32_t kCurrentMask = 0x000000FFU;
constexpr std::uint32_t kMiddleElevation = 0x01000000U;
constexpr std::uint32_t kHighElevation = 0x02000000U;
constexpr std::uint8_t kFullLight = 31U;
constexpr std::uint8_t kFogLight = kFullLight >> 1U;

[[nodiscard]] int floor_divide(const int value, const int divisor) noexcept {
  return value >= 0 ? value / divisor
                    : -((-value + divisor - 1) / divisor);
}

[[nodiscard]] std::size_t tile_index(const BootstrapStatus &status,
                                     const int x, const int y) noexcept {
  return static_cast<std::size_t>(y) * status.scenario_width + x;
}

[[nodiscard]] FogTileState raw_fog_tile_state(
    const BootstrapStatus &status, const int tile_x, const int tile_y,
    const std::uint8_t player) noexcept {
  if (player >= 8U || tile_x < 0 || tile_y < 0 ||
      tile_x >= status.scenario_width || tile_y >= status.scenario_height ||
      status.fog_map_tiles.size() !=
          static_cast<std::size_t>(status.scenario_width) *
              status.scenario_height) {
    return FogTileState::unexplored;
  }
  const std::uint32_t player_mask = 1U << player;
  const std::uint32_t tile =
      status.fog_map_tiles[tile_index(status, tile_x, tile_y)];
  if ((tile & player_mask) == 0U) {
    return FogTileState::visible;
  }
  return (tile & (player_mask << 8U)) == 0U ? FogTileState::explored
                                            : FogTileState::unexplored;
}

[[nodiscard]] std::uint8_t effective_sight_range(
    const BootstrapStatus &status,
    const ScenarioUnitPreview &unit) noexcept {
  // CUnitGUI.cpp::sub_42B710 reads units.dat sight range and adds two mask
  // levels for the four retail sight upgrades. The executable addresses map
  // to upgrade IDs 20, 25, 41, and 38 in this data revision.
  std::uint8_t upgrade = 46U;
  switch (unit.unit_type) {
  case 1U:  // Terran Ghost
  case 16U: // Hero Sarah Kerrigan
    upgrade = 20U;
    break;
  case 42U: // Zerg Overlord
    upgrade = 25U;
    break;
  case 70U: // Protoss Scout
    upgrade = 41U;
    break;
  case 84U: // Protoss Observer
    upgrade = 38U;
    break;
  default:
    break;
  }
  const bool upgraded = unit.owner < status.player_upgrade_levels.size() &&
                        upgrade < status.player_upgrade_levels[unit.owner].size() &&
                        status.player_upgrade_levels[unit.owner][upgrade] != 0U;
  return static_cast<std::uint8_t>((std::min)(
      11U, static_cast<unsigned>(unit.sight_range) + (upgraded ? 2U : 0U)));
}

[[nodiscard]] VisionSourceRuntimeState vision_source_state(
    const BootstrapStatus &status,
    const ScenarioUnitPreview &unit) noexcept {
  const bool active = unit.alive && !unit.dying && !unit.sprite_hidden &&
                      !unit.in_transport && !unit.is_projectile &&
                      unit.owner < 8U;
  return {
      unit.unit_id,
      static_cast<std::uint16_t>(unit.x / 32U),
      static_cast<std::uint16_t>(unit.y / 32U),
      unit.owner,
      effective_sight_range(status, unit),
      is_airborne(unit),
      active,
  };
}

[[nodiscard]] bool same_vision_source(
    const VisionSourceRuntimeState &left,
    const VisionSourceRuntimeState &right) noexcept {
  return left.unit_id == right.unit_id && left.tile_x == right.tile_x &&
         left.tile_y == right.tile_y && left.owner == right.owner &&
         left.sight_range == right.sight_range &&
         left.airborne == right.airborne && left.active == right.active;
}

[[nodiscard]] bool vision_sources_are_current(
    const BootstrapStatus &status) noexcept {
  if (!status.vision_source_state_ready) {
    return false;
  }
  std::size_t state_index{};
  for (const ScenarioUnitPreview &unit : status.units) {
    const VisionSourceRuntimeState source = vision_source_state(status, unit);
    if (!source.active) {
      continue;
    }
    if (state_index >= status.vision_source_states.size() ||
        !same_vision_source(status.vision_source_states[state_index],
                            source)) {
      return false;
    }
    ++state_index;
  }
  return state_index == status.vision_source_states.size();
}

[[nodiscard]] std::uint8_t tile_light(const BootstrapStatus &status,
                                      const int tile_x,
                                      const int tile_y) noexcept {
  switch (raw_fog_tile_state(status, tile_x, tile_y, status.local_player)) {
  case FogTileState::visible:
    return kFullLight;
  case FogTileState::explored:
    return kFogLight;
  case FogTileState::unexplored:
  default:
    return 0U;
  }
}

[[nodiscard]] std::uint32_t translated_dark_pixel(
    const BootstrapStatus &status, const std::uint8_t source,
    const std::uint8_t level, const std::uint32_t fallback) noexcept {
  if (level >= kFullLight) {
    return fallback;
  }
  if (level == 0U) {
    return 0xFF000000U;
  }
  if (status.terrain_dark_levels.size() == 32U * 256U &&
      status.game_palette.size() == 1024U) {
    const std::uint8_t mapped = status.terrain_dark_levels[
        static_cast<std::size_t>(level) * 256U + source];
    const std::size_t color = static_cast<std::size_t>(mapped) * 4U;
    return 0xFF000000U | status.game_palette[color + 2U] |
           (static_cast<std::uint32_t>(status.game_palette[color + 1U])
            << 8U) |
           (static_cast<std::uint32_t>(status.game_palette[color]) << 16U);
  }
  const std::uint32_t blue = (fallback & 0xFFU) * level / kFullLight;
  const std::uint32_t green = ((fallback >> 8U) & 0xFFU) * level / kFullLight;
  const std::uint32_t red = ((fallback >> 16U) & 0xFFU) * level / kFullLight;
  return 0xFF000000U | blue | (green << 8U) | (red << 16U);
}

void reveal_sight_mask(BootstrapStatus &status,
                       const VisionSourceRuntimeState &source) {
  const starcraft::lang::SightMask &mask =
      starcraft::lang::sight_mask(source.sight_range);
  std::vector<std::uint8_t> visible(mask.cells.size());
  const std::uint32_t owner_mask = 1U << source.owner;
  const auto in_map = [&status](const int x, const int y) {
    return x >= 0 && y >= 0 && x < status.scenario_width &&
           y < status.scenario_height;
  };
  const std::size_t source_index =
      tile_index(status, source.tile_x, source.tile_y);
  const bool unobstructed =
      source.airborne ||
      (status.fog_map_tiles[source_index] & kHighElevation) != 0U;

  for (std::size_t index = 0; index < mask.cells.size(); ++index) {
    const starcraft::lang::SightMaskCell &cell = mask.cells[index];
    const int x = static_cast<int>(source.tile_x) + cell.x;
    const int y = static_cast<int>(source.tile_y) + cell.y;
    if (!in_map(x, y)) {
      continue;
    }
    bool reveal = cell.inner;
    if (!reveal) {
      for (const std::uint16_t dependency : cell.dependencies) {
        if (dependency >= index || visible[dependency] == 0U) {
          continue;
        }
        if (unobstructed) {
          reveal = true;
          break;
        }
        const starcraft::lang::SightMaskCell &parent =
            mask.cells[dependency];
        const int parent_x = static_cast<int>(source.tile_x) + parent.x;
        const int parent_y = static_cast<int>(source.tile_y) + parent.y;
        if (!in_map(parent_x, parent_y)) {
          continue;
        }
        const std::uint32_t parent_flags =
            status.fog_map_tiles[tile_index(status, parent_x, parent_y)];
        const std::uint32_t target_flags =
            status.fog_map_tiles[tile_index(status, x, y)];
        // mask.cpp::sub_46CF60 accepts a dependency only when its player bit
        // and 0x01000000 terrain bit are clear, and either the destination is
        // high terrain or the dependency is not. This is the original
        // low-ground cliff occlusion test.
        if ((parent_flags & kMiddleElevation) == 0U &&
            ((target_flags & kHighElevation) != 0U ||
             (parent_flags & kHighElevation) == 0U)) {
          reveal = true;
          break;
        }
      }
    }
    if (!reveal) {
      continue;
    }
    visible[index] = 1U;
    std::uint32_t &tile = status.fog_map_tiles[tile_index(status, x, y)];
    // sub_46D140 applies ~playerMask & ~(playerMask << 8): current sight and
    // permanent exploration are cleared together, while terrain flags stay.
    tile &= ~owner_mask;
    tile &= ~(owner_mask << 8U);
  }
}

} // namespace

FogTileState fog_tile_state(const BootstrapStatus &status, const int tile_x,
                             const int tile_y,
                             const std::uint8_t player) noexcept {
  if (!status.fog_of_war_enabled) {
    return FogTileState::visible;
  }
  return raw_fog_tile_state(status, tile_x, tile_y, player);
}

bool fog_world_position_visible(const BootstrapStatus &status,
                                const std::uint16_t world_x,
                                const std::uint16_t world_y,
                                const std::uint8_t player) noexcept {
  if (status.fog_map_tiles.size() !=
      static_cast<std::size_t>(status.scenario_width) *
          status.scenario_height) {
    return true;
  }
  return fog_tile_state(status, world_x / 32U, world_y / 32U, player) ==
         FogTileState::visible;
}

bool fog_unit_visible(const BootstrapStatus &status,
                      const ScenarioUnitPreview &unit,
                      const std::uint8_t player) noexcept {
  if (!status.fog_of_war_enabled) {
    return true;
  }
  if (status.fog_map_tiles.size() !=
      static_cast<std::size_t>(status.scenario_width) *
          status.scenario_height) {
    return true;
  }
  const int half_width = (std::max)(1, static_cast<int>(unit.selection_width)) /
                         2;
  const int half_height =
      (std::max)(1, static_cast<int>(unit.selection_height)) / 2;
  const int left = (std::max)(0, static_cast<int>(unit.x) - half_width) / 32;
  const int top = (std::max)(0, static_cast<int>(unit.y) - half_height) / 32;
  const int right =
      (std::max)(0, static_cast<int>(unit.x) + half_width) / 32;
  const int bottom =
      (std::max)(0, static_cast<int>(unit.y) + half_height) / 32;
  for (int y = top; y <= bottom; ++y) {
    for (int x = left; x <= right; ++x) {
      if (fog_tile_state(status, x, y, player) == FogTileState::visible) {
        return true;
      }
    }
  }
  return false;
}

bool rebuild_fog_render_surfaces(BootstrapStatus &status) noexcept {
  status.fog_render_surfaces_ready = false;
  if (!status.terrain_ready || status.terrain.width != kMapViewportWidth ||
      status.terrain.height != kMapViewportHeight ||
      status.terrain.bgra.size() != status.terrain.palette_indices.size()) {
    return false;
  }
  try {
    status.fogged_terrain = status.terrain;
    constexpr int grid_width = 26;
    constexpr int grid_height = 20;
    const int grid_left = floor_divide(static_cast<int>(status.camera_x) - 64,
                                       32);
    const int grid_top = floor_divide(static_cast<int>(status.camera_y) - 64,
                                      32);
    std::array<std::uint8_t, grid_width * grid_height> raw{};
    std::array<std::uint8_t, grid_width * grid_height> smooth{};
    for (int y = 0; y < grid_height; ++y) {
      for (int x = 0; x < grid_width; ++x) {
        raw[static_cast<std::size_t>(y) * grid_width + x] =
            tile_light(status, grid_left + x, grid_top + y);
      }
    }
    for (int y = 0; y < grid_height; ++y) {
      for (int x = 0; x < grid_width; ++x) {
        const auto sample = [&](const int sample_x, const int sample_y) {
          const int clamped_x = (std::clamp)(sample_x, 0, grid_width - 1);
          const int clamped_y = (std::clamp)(sample_y, 0, grid_height - 1);
          return static_cast<unsigned>(
              raw[static_cast<std::size_t>(clamped_y) * grid_width +
                  clamped_x]);
        };
        // sub_46D780's exact 3x3 kernel: diagonals + 2*orthogonals +
        // 4*center, divided by sixteen.
        const unsigned value =
            sample(x - 1, y - 1) + sample(x + 1, y - 1) +
            sample(x - 1, y + 1) + sample(x + 1, y + 1) +
            2U * (sample(x, y - 1) + sample(x - 1, y) +
                  sample(x + 1, y) + sample(x, y + 1) +
                  2U * sample(x, y));
        smooth[static_cast<std::size_t>(y) * grid_width + x] =
            static_cast<std::uint8_t>(value >> 4U);
      }
    }
    for (int screen_y = 0; screen_y < kMapViewportHeight; ++screen_y) {
      const int world_y = status.camera_y + screen_y;
      const int cell_y = floor_divide(world_y - 16, 32) - grid_top;
      const int fraction_y = (world_y - 16) -
                             floor_divide(world_y - 16, 32) * 32;
      for (int screen_x = 0; screen_x < kMapViewportWidth; ++screen_x) {
        const int world_x = status.camera_x + screen_x;
        const int cell_x = floor_divide(world_x - 16, 32) - grid_left;
        const int fraction_x = (world_x - 16) -
                               floor_divide(world_x - 16, 32) * 32;
        const int x0 = (std::clamp)(cell_x, 0, grid_width - 1);
        const int x1 = (std::clamp)(cell_x + 1, 0, grid_width - 1);
        const int y0 = (std::clamp)(cell_y, 0, grid_height - 1);
        const int y1 = (std::clamp)(cell_y + 1, 0, grid_height - 1);
        const unsigned top =
            smooth[static_cast<std::size_t>(y0) * grid_width + x0] *
                (32 - fraction_x) +
            smooth[static_cast<std::size_t>(y0) * grid_width + x1] *
                fraction_x;
        const unsigned bottom =
            smooth[static_cast<std::size_t>(y1) * grid_width + x0] *
                (32 - fraction_x) +
            smooth[static_cast<std::size_t>(y1) * grid_width + x1] *
                fraction_x;
        const std::uint8_t level = static_cast<std::uint8_t>(
            (top * (32 - fraction_y) + bottom * fraction_y + 512U) >> 10U);
        const std::size_t pixel =
            static_cast<std::size_t>(screen_y) * kMapViewportWidth + screen_x;
        status.fogged_terrain.bgra[pixel] = translated_dark_pixel(
            status, status.terrain.palette_indices[pixel], level,
            status.terrain.bgra[pixel]);
      }
    }

    status.fogged_minimap = status.minimap;
    if (status.minimap_ready && status.minimap.width == 128U &&
        status.minimap.height == 128U &&
        status.minimap.palette_indices.size() == status.minimap.bgra.size()) {
      for (std::uint16_t y = 0; y < status.minimap_content_height; ++y) {
        const int tile_y = static_cast<int>(
            static_cast<std::uint32_t>(y) * status.scenario_height /
            (std::max)(1U,
                       static_cast<unsigned>(status.minimap_content_height)));
        for (std::uint16_t x = 0; x < status.minimap_content_width; ++x) {
          const int tile_x = static_cast<int>(
              static_cast<std::uint32_t>(x) * status.scenario_width /
              (std::max)(1U,
                         static_cast<unsigned>(status.minimap_content_width)));
          const std::size_t pixel =
              static_cast<std::size_t>(status.minimap_content_y + y) * 128U +
              status.minimap_content_x + x;
          const FogTileState state =
              raw_fog_tile_state(status, tile_x, tile_y,
                                 status.local_player);
          if (state == FogTileState::unexplored) {
            status.fogged_minimap.bgra[pixel] = 0xFF000000U;
          } else if (state == FogTileState::explored) {
            status.fogged_minimap.bgra[pixel] = translated_dark_pixel(
                status, status.minimap.palette_indices[pixel], kFogLight,
                status.minimap.bgra[pixel]);
          }
        }
      }
    }
    status.fog_render_surfaces_ready = true;
    return true;
  } catch (...) {
    status.fogged_terrain = {};
    status.fogged_minimap = {};
    return false;
  }
}

bool rebuild_fog_of_war(BootstrapStatus &status, const bool force) noexcept {
  const std::size_t count = static_cast<std::size_t>(status.scenario_width) *
                            status.scenario_height;
  if (!status.scenario.valid() || status.fog_map_tiles.size() != count ||
      status.scenario.fog_mask().size() != count) {
    return false;
  }
  const bool refresh_due = status.fog_refresh_ticks == 0U;
  if (!force && !refresh_due && vision_sources_are_current(status)) {
    --status.fog_refresh_ticks;
    return true;
  }
  try {
    std::vector<VisionSourceRuntimeState> states;
    states.reserve(status.units.size());
    for (const ScenarioUnitPreview &unit : status.units) {
      const VisionSourceRuntimeState source = vision_source_state(status, unit);
      if (!source.active || source.tile_x >= status.scenario_width ||
          source.tile_y >= status.scenario_height) {
        continue;
      }
      states.push_back(source);
    }

    const bool full_refresh = force || refresh_due;
    if (full_refresh) {
      for (std::size_t index = 0; index < count; ++index) {
        // mask.cpp::sub_46DA10 restores every current-visibility byte to
        // 0xFF before the live CUnits reveal their masks again. The scenario
        // MASK byte is copied into the next byte only once during map load;
        // that byte is the persistent initially-explored/history state.
        status.fog_map_tiles[index] |= kCurrentMask;
      }
    }

    bool visibility_changed = full_refresh;
    for (const VisionSourceRuntimeState &source : states) {
      bool reveal = full_refresh;
      if (!reveal) {
        const auto previous = std::find_if(
            status.vision_source_states.begin(),
            status.vision_source_states.end(),
            [&source](const VisionSourceRuntimeState &candidate) {
              return candidate.unit_id == source.unit_id;
            });
        reveal = previous == status.vision_source_states.end() ||
                 !same_vision_source(*previous, source);
      }
      if (reveal) {
        reveal_sight_mask(status, source);
        visibility_changed = true;
      }
    }
    status.vision_source_states = std::move(states);
    status.vision_source_state_ready = true;
    ++status.fog_rebuild_count;
    status.fog_refresh_ticks =
        full_refresh ? 100U
                     : static_cast<std::uint16_t>(
                           status.fog_refresh_ticks == 0U
                               ? 0U
                               : status.fog_refresh_ticks - 1U);
    return !visibility_changed || rebuild_fog_render_surfaces(status);
  } catch (...) {
    status.vision_source_states.clear();
    status.vision_source_state_ready = false;
    return false;
  }
}

bool initialize_fog_of_war(BootstrapStatus &status) noexcept {
  const std::size_t count = static_cast<std::size_t>(status.scenario_width) *
                            status.scenario_height;
  if (!status.scenario.valid() || !status.terrain_tileset.valid() ||
      status.scenario.fog_mask().size() != count) {
    return false;
  }
  try {
    status.fog_map_tiles.assign(count, 0U);
    for (std::uint16_t y = 0; y < status.scenario_height; ++y) {
      for (std::uint16_t x = 0; x < status.scenario_width; ++x) {
        std::uint16_t map_tile{};
        std::uint16_t group_flags{};
        std::array<std::uint16_t, 16> minitile_flags{};
        if (!status.scenario.tile(x, y, map_tile) ||
            !status.terrain_tileset.terrain_flags(
                map_tile, group_flags, minitile_flags)) {
          return false;
        }
        const std::size_t index = tile_index(status, x, y);
        const std::uint32_t mask = status.scenario.fog_mask()[index];
        status.fog_map_tiles[index] =
            (static_cast<std::uint32_t>(group_flags & 0xFFF8U) << 16U) |
            mask | (mask << 8U);
      }
    }
    status.vision_source_state_ready = false;
    return rebuild_fog_of_war(status, true);
  } catch (...) {
    status.fog_map_tiles.clear();
    status.vision_source_states.clear();
    status.vision_source_state_ready = false;
    return false;
  }
}

} // namespace starcraft::recovery
