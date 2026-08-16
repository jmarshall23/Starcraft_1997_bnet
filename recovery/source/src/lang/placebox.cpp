#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

bool advance_addon_construction(BootstrapStatus &status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview &addon : status.units) {
    if (!addon.alive || addon.construction_complete ||
        addon.addon_parent_id == 0U || addon.construction_ticks_total == 0U) {
      continue;
    }
    const ScenarioUnitPreview *const parent =
        find_unit_by_id(status, addon.addon_parent_id);
    if (parent == nullptr || parent->attached_addon_id != addon.unit_id) {
      continue;
    }
    if (addon.construction_ticks_remaining != 0U) {
      --addon.construction_ticks_remaining;
    }
    const std::uint32_t initial_life =
        (std::max)(1U, addon.max_hit_points / 10U);
    const std::uint32_t elapsed =
        addon.construction_ticks_total - addon.construction_ticks_remaining;
    addon.hit_points =
        initial_life +
        static_cast<std::uint32_t>(
            static_cast<std::uint64_t>(addon.max_hit_points - initial_life) *
            elapsed / addon.construction_ticks_total);
    (void)advance_building_construction_animation(status, addon);
    if (addon.construction_ticks_remaining == 0U) {
      addon.hit_points = addon.max_hit_points;
      addon.construction_complete = true;
      (void)restart_unit_animation(status, addon, 16U);
    }
    changed = true;
  }
  return changed;
}

bool addon_center_for_parent(const BuildableUnitVisual &addon,
                             const ScenarioUnitPreview &parent,
                             std::uint16_t &center_x,
                             std::uint16_t &center_y) noexcept {
  if ((addon.simulation.dat_flags & 2U) == 0U ||
      addon.addon_parent_type != parent.unit_type) {
    return false;
  }
  starcraft::lang::TerranAddonPlacement placement{};
  if (!starcraft::lang::terran_addon_center(
          parent.unit_type, addon.unit_type, parent.x, parent.y,
          parent.selection_width, parent.selection_height,
          addon.placement_width, addon.placement_height, addon.addon_x,
          addon.addon_y, placement)) {
    return false;
  }
  center_x = placement.center_x;
  center_y = placement.center_y;
  return true;
}

bool placement_is_valid(const BootstrapStatus &status,
                        const BuildableUnitVisual &buildable,
                        const std::uint16_t center_x,
                        const std::uint16_t center_y) noexcept {
  // placebox.cpp::sub_481410 at 0x00481410 reads dword_8DFFB0's width and
  // height, divides them by 32, and colors the per-tile placement bitmap.
  // Validate that footprint against map bounds, the CV5 0x0800 terrain-block
  // bit recovered through collide.cpp::sub_419260, and live unit occupancy.
  const int left = static_cast<int>(center_x) - buildable.placement_width / 2;
  const int top = static_cast<int>(center_y) - buildable.placement_height / 2;
  const int right = left + buildable.placement_width;
  const int bottom = top + buildable.placement_height;
  const int map_right = static_cast<int>(status.scenario_width) * 32;
  const int map_bottom = static_cast<int>(status.scenario_height) * 32;
  if (left < 0 || top < 0 || right > map_right || bottom > map_bottom) {
    return false;
  }
  if ((buildable.simulation.dat_flags & 2U) != 0U) {
    bool attached_to_parent{};
    for (const ScenarioUnitPreview &parent : status.units) {
      std::uint16_t addon_x{};
      std::uint16_t addon_y{};
      if (!parent.alive || !parent.construction_complete || parent.owner != 0 ||
          parent.attached_addon_id != 0U ||
          !addon_center_for_parent(buildable, parent, addon_x, addon_y)) {
        continue;
      }
      if (addon_x == center_x && addon_y == center_y) {
        attached_to_parent = true;
        break;
      }
    }
    if (!attached_to_parent) {
      return false;
    }
  }
  const int first_tile_x = left / 32;
  const int first_tile_y = top / 32;
  const int last_tile_x = (right - 1) / 32;
  const int last_tile_y = (bottom - 1) / 32;
  const bool requires_creep = (buildable.simulation.dat_flags & 0x20000U) != 0U;
  for (int tile_y = first_tile_y; tile_y <= last_tile_y; ++tile_y) {
    for (int tile_x = first_tile_x; tile_x <= last_tile_x; ++tile_x) {
      std::uint16_t map_tile{};
      if (!status.scenario.tile(static_cast<std::uint16_t>(tile_x),
                                static_cast<std::uint16_t>(tile_y), map_tile) ||
          !status.terrain_tileset.buildable(map_tile)) {
        return false;
      }
      if (requires_creep && !tile_has_creep(status, tile_x, tile_y)) {
        return false;
      }
    }
  }
  const bool refinery = buildable.unit_type == 110U;
  std::uint32_t geyser_id{};
  if (refinery) {
    for (const ScenarioUnitPreview &unit : status.units) {
      if (unit.alive && unit.unit_type == 188U &&
          std::abs(static_cast<int>(unit.x) - center_x) <= 16 &&
          std::abs(static_cast<int>(unit.y) - center_y) <= 16) {
        geyser_id = unit.unit_id;
        break;
      }
    }
    if (geyser_id == 0) {
      return false;
    }
  }
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive) {
      continue;
    }
    const int unit_left = static_cast<int>(unit.x) - unit.selection_width / 2;
    const int unit_top = static_cast<int>(unit.y) - unit.selection_height / 2;
    const int unit_right = unit_left + unit.selection_width;
    const int unit_bottom = unit_top + unit.selection_height;
    if (left < unit_right && right > unit_left && top < unit_bottom &&
        bottom > unit_top && unit.unit_id != geyser_id) {
      return false;
    }
  }
  return true;
}

bool update_building_placement(BootstrapStatus &status, const int game_x,
                               const int game_y) noexcept {
  if (!status.placement_active) {
    return false;
  }
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr ||
      buildable->asset_index >= status.unit_assets.size()) {
    status.placement_valid = false;
    return false;
  }
  // placebox.cpp::sub_480F80 at 0x00480F80 and sub_481590 at 0x00481590
  // convert the 640x400 mouse point to map tiles with >> 5, then recover the
  // 32-pixel aligned world origin before drawing the footprint.
  const int world_x = game_x + status.camera_x;
  const int world_y = game_y + status.camera_y;
  const int half_width = buildable->placement_width / 2;
  const int half_height = buildable->placement_height / 2;
  const int snapped_left = ((std::max)(0, world_x - half_width + 16) / 32) * 32;
  const int snapped_top = ((std::max)(0, world_y - half_height + 16) / 32) * 32;
  status.placement_x = static_cast<std::uint16_t>(snapped_left + half_width);
  status.placement_y = static_cast<std::uint16_t>(snapped_top + half_height);
  status.placement_valid = placement_is_valid(
      status, *buildable, status.placement_x, status.placement_y);
  return true;
}

bool place_current_building(BootstrapStatus &status) noexcept {
  if (!status.placement_active || !status.placement_valid) {
    return false;
  }
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr ||
      buildable->asset_index >= status.unit_assets.size()) {
    status.placement_valid = false;
    return false;
  }
  if (!resource_cost_available(status, buildable->simulation.mineral_cost,
                               buildable->simulation.gas_cost) ||
      !placement_is_valid(status, *buildable, status.placement_x,
                          status.placement_y)) {
    status.placement_valid = false;
    return false;
  }
  const ScenarioUnitPreview *const selected_source =
      first_selected_unit(status);
  const bool addon = (buildable->simulation.dat_flags & 2U) != 0U;
  std::uint32_t worker_id{};
  std::uint32_t parent_id{};
  if (selected_source == nullptr || selected_source->owner != 0) {
    return false;
  }
  if (addon) {
    std::uint16_t addon_x{};
    std::uint16_t addon_y{};
    if (!selected_source->construction_complete ||
        selected_source->unit_type != buildable->addon_parent_type ||
        selected_source->attached_addon_id != 0U ||
        !addon_center_for_parent(*buildable, *selected_source, addon_x,
                                 addon_y) ||
        addon_x != status.placement_x || addon_y != status.placement_y) {
      return false;
    }
    parent_id = selected_source->unit_id;
  } else {
    if (selected_source->unit_type != 7) {
      return false;
    }
    worker_id = selected_source->unit_id;
  }
  try {
    std::size_t geyser_index = SIZE_MAX;
    if (buildable->unit_type == 110U) {
      for (std::size_t index = 0; index < status.units.size(); ++index) {
        const ScenarioUnitPreview &unit = status.units[index];
        if (unit.alive && unit.unit_type == 188U &&
            std::abs(static_cast<int>(unit.x) - status.placement_x) <= 16 &&
            std::abs(static_cast<int>(unit.y) - status.placement_y) <= 16) {
          geyser_index = index;
          break;
        }
      }
      if (geyser_index == SIZE_MAX) {
        return false;
      }
    }
    const UnitRenderAsset &asset = status.unit_assets[buildable->asset_index];
    ScenarioUnitPreview building{};
    building.unit_id = status.next_unit_id++;
    building.x = status.placement_x;
    building.y = status.placement_y;
    building.x_fixed = static_cast<std::int32_t>(building.x) << 8U;
    building.y_fixed = static_cast<std::int32_t>(building.y) << 8U;
    building.unit_type = buildable->unit_type;
    building.owner = 0;
    building.asset_index = buildable->asset_index;
    building.selection_width = buildable->placement_width;
    building.selection_height = buildable->placement_height;
    building.iscript_state = asset.initial_iscript_state;
    building.overlay_iscript_state = asset.initial_overlay_iscript_state;
    building.current_sprite_frame = asset.initial_iscript_state.frame;
    building.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
    building.iscript_ready = asset.iscript_ready;
    building.overlay_ready = asset.overlay_ready;
    building.is_building = true;
    apply_simulation_traits(building, buildable->simulation);
    // CUnitInit.cpp::sub_42E6B0 and CUnitBuild.cpp::sub_422D20 initialize an
    // unfinished building at exactly one tenth of maximum fixed-point life.
    // The +172 field is units.dat build time / 2 and is decremented only while
    // a builder is actively applying the +168 construction increment.
    building.construction_complete = false;
    building.construction_ticks_total = static_cast<std::uint16_t>((
        std::max)(1U, static_cast<unsigned>(buildable->simulation.build_time) >>
                          1U));
    building.construction_ticks_remaining = building.construction_ticks_total;
    building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
    building.construction_builder_id = worker_id;
    building.addon_parent_id = parent_id;
    if (geyser_index != SIZE_MAX) {
      building.resource_amount = status.units[geyser_index].resource_amount;
    }
    const std::uint32_t building_id = building.unit_id;
    status.units.push_back(std::move(building));
    if (parent_id != 0U) {
      ScenarioUnitPreview *const parent = find_unit_by_id(status, parent_id);
      if (parent != nullptr) {
        parent->attached_addon_id = building_id;
      }
    }
    if (geyser_index != SIZE_MAX) {
      status.units[geyser_index].alive = false;
      status.units[geyser_index].selected = false;
    }
    status.player_minerals -= buildable->simulation.mineral_cost;
    status.player_gas -= buildable->simulation.gas_cost;
    status.placement_active = false;
    status.placement_valid = false;
    status.placement_unit_type = 0xFFFFU;
    status.active_command_card = 0;
    ScenarioUnitPreview *const worker = find_unit_by_id(status, worker_id);
    ScenarioUnitPreview *const created = find_unit_by_id(status, building_id);
    if (!addon && worker != nullptr && created != nullptr &&
        !begin_scv_interaction(status, *worker, *created,
                               ActiveUnitOrder::construct)) {
      created->construction_builder_id = 0;
    }
    return true;
  } catch (...) {
    return false;
  }
}

void draw_building_placement_gl(const BootstrapStatus &status) {
  if (!status.placement_active) {
    return;
  }
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, status.placement_unit_type);
  if (buildable == nullptr ||
      buildable->asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset &asset = status.unit_assets[buildable->asset_index];
  ScenarioUnitPreview ghost{};
  ghost.x = status.placement_x;
  ghost.y = status.placement_y;
  ghost.unit_type = buildable->unit_type;
  ghost.owner = 0;
  ghost.asset_index = buildable->asset_index;
  ghost.selection_width = buildable->placement_width;
  ghost.selection_height = buildable->placement_height;
  ghost.iscript_state = asset.initial_iscript_state;
  ghost.overlay_iscript_state = asset.initial_overlay_iscript_state;
  ghost.current_sprite_frame = asset.initial_iscript_state.frame;
  ghost.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  ghost.iscript_ready = asset.iscript_ready;
  ghost.overlay_ready = asset.overlay_ready;
  draw_scenario_unit_gl(status, ghost);

  const int left = static_cast<int>(status.placement_x) -
                   static_cast<int>(buildable->placement_width) / 2 -
                   status.camera_x;
  const int top = static_cast<int>(status.placement_y) -
                  static_cast<int>(buildable->placement_height) / 2 -
                  status.camera_y;
  const int right = left + buildable->placement_width;
  const int bottom = top + buildable->placement_height;
  glDisable(GL_TEXTURE_2D);
  if (status.placement_valid) {
    glColor4ub(32, 255, 32, 72);
  } else {
    glColor4ub(255, 48, 32, 96);
  }
  glBegin(GL_QUADS);
  glVertex2i(left, top);
  glVertex2i(right, top);
  glVertex2i(right, bottom);
  glVertex2i(left, bottom);
  glEnd();
  glColor4ub(status.placement_valid ? 32 : 255,
             status.placement_valid ? 255 : 48, 32, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(left, top);
  glVertex2i(right, top);
  glVertex2i(right, bottom);
  glVertex2i(left, bottom);
  glEnd();
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

} // namespace starcraft::recovery
