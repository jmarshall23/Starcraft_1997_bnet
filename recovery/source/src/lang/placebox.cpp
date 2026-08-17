#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {
namespace {

bool power_frame_contains(const UnitRenderAsset &asset,
                          const SpritePreviewFrame &frame,
                          const ScenarioUnitPreview &pylon,
                          const int world_x, const int world_y) noexcept {
  // CUnitProtoss.cpp::sub_43C070 samples one quadrant of sprite 198 at
  // (16 + 32*x, 16 + 32*y), mirrors those samples across both axes, then
  // sub_43C200 indexes the resulting 32-pixel grid around each live Pylon.
  const int quadrant_columns =
      (static_cast<int>(asset.sprite_canvas_width) + 15) >> 5;
  const int quadrant_rows =
      (static_cast<int>(asset.sprite_canvas_height) + 15) >> 5;
  const int half_width = 32 * quadrant_columns;
  const int half_height = 32 * quadrant_rows;
  const int dx = world_x - static_cast<int>(pylon.x);
  const int dy = world_y - static_cast<int>(pylon.y);
  if (quadrant_columns == 0 || quadrant_rows == 0 ||
      std::abs(dx) >= half_width || std::abs(dy) >= half_height) {
    return false;
  }
  const int grid_x = (half_width + dx) / 32;
  const int grid_y = (half_height + dy) / 32;
  const int sample_column = grid_x < quadrant_columns
                                ? quadrant_columns - grid_x - 1
                                : grid_x - quadrant_columns;
  const int sample_row = grid_y < quadrant_rows
                             ? grid_y
                             : 2 * quadrant_rows - grid_y - 1;
  const int sample_x = sample_column * 32 + 16 - frame.x_offset;
  const int sample_y = sample_row * 32 + 16 - frame.y_offset;
  if (sample_x < 0 || sample_y < 0 || sample_x >= frame.width ||
      sample_y >= frame.height) {
    return false;
  }
  return (frame.bgra[static_cast<std::size_t>(sample_y) * frame.width +
                     sample_x] >>
          24U) != 0U;
}

bool protoss_position_powered(const BootstrapStatus &status,
                              const std::uint16_t unit_type,
                              const std::uint8_t owner,
                              const std::uint16_t center_x,
                              const std::uint16_t center_y) noexcept {
  // CUnitProtoss.cpp::sub_43C200 exempts Nexus and Pylon. Assimilator uses
  // CUnitPBuild's separate geyser order and is likewise not placement-gated.
  if (unit_type == 154U || unit_type == 156U || unit_type == 157U) {
    return true;
  }
  if (!starcraft::lang::is_protoss_buildable_unit_type(unit_type) ||
      status.pylon_power_asset_index >= status.unit_assets.size()) {
    return !starcraft::lang::is_protoss_buildable_unit_type(unit_type);
  }
  const UnitRenderAsset &power =
      status.unit_assets[status.pylon_power_asset_index];
  for (const ScenarioUnitPreview &pylon : status.units) {
    if (!pylon.alive || !pylon.construction_complete || pylon.owner != owner ||
        pylon.unit_type != 156U) {
      continue;
    }
    if (power.initial_iscript_state.frame < power.sprite_frames.size() &&
        power_frame_contains(
            power, power.sprite_frames[power.initial_iscript_state.frame],
            pylon, center_x, center_y)) {
      return true;
    }
    if (power.overlay_ready &&
        power.initial_overlay_iscript_state.frame < power.overlay_frames.size() &&
        power_frame_contains(
            power,
            power.overlay_frames[power.initial_overlay_iscript_state.frame],
            pylon, center_x, center_y)) {
      return true;
    }
  }
  return false;
}

}  // namespace

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
                        const std::uint16_t center_y,
                        const std::uint8_t owner) noexcept {
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
  if (!protoss_position_powered(status, buildable.unit_type, owner, center_x,
                                center_y)) {
    return false;
  }
  if ((buildable.simulation.dat_flags & 2U) != 0U) {
    bool attached_to_parent{};
    for (const ScenarioUnitPreview &parent : status.units) {
      std::uint16_t addon_x{};
      std::uint16_t addon_y{};
      if (!parent.alive || !parent.construction_complete ||
          parent.owner != owner ||
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
  // The three race refineries share the original geyser-only placement path:
  // Terran Refinery 110, Zerg Extractor 149, Protoss Assimilator 157.
  const bool refinery = buildable.unit_type == 110U ||
                        buildable.unit_type == 149U ||
                        buildable.unit_type == 157U;
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

bool begin_protoss_build_order(BootstrapStatus &status,
                               ScenarioUnitPreview &probe,
                               const BuildableUnitVisual &buildable,
                               const std::uint16_t center_x,
                               const std::uint16_t center_y,
                               const bool charge_resources) noexcept {
  if (!probe.alive || probe.is_building ||
      !starcraft::lang::is_protoss_probe(probe.unit_type) ||
      !placement_is_valid(status, buildable, center_x, center_y, probe.owner)) {
    return false;
  }
  std::uint32_t &minerals = probe.owner == 0U
                                ? status.player_minerals
                                : status.player_mineral_stock[probe.owner];
  std::uint32_t &gas = probe.owner == 0U
                           ? status.player_gas
                           : status.player_gas_stock[probe.owner];
  if (charge_resources &&
      (minerals < buildable.simulation.mineral_cost ||
       gas < buildable.simulation.gas_cost)) {
    return false;
  }

  const int half_width = buildable.placement_width / 2;
  const int half_height = buildable.placement_height / 2;
  std::array<starcraft::lang::PathPoint, 4> approaches{{
      {static_cast<std::uint16_t>((std::max)(0, static_cast<int>(center_x) -
                                                    half_width -
                                                    probe.collision_right - 2)),
       center_y},
      {static_cast<std::uint16_t>((std::min)(
           static_cast<int>(UINT16_MAX), static_cast<int>(center_x) +
                                             half_width +
                                             probe.collision_left + 2)),
       center_y},
      {center_x,
       static_cast<std::uint16_t>((std::max)(0, static_cast<int>(center_y) -
                                                    half_height -
                                                    probe.collision_bottom - 2))},
      {center_x,
       static_cast<std::uint16_t>((std::min)(
           static_cast<int>(UINT16_MAX), static_cast<int>(center_y) +
                                             half_height +
                                             probe.collision_top + 2))},
  }};
  std::sort(approaches.begin(), approaches.end(), [&probe](const auto &left,
                                                           const auto &right) {
    const auto distance = [&probe](const auto &point) {
      const std::int64_t dx = static_cast<int>(point.x) - probe.x;
      const std::int64_t dy = static_cast<int>(point.y) - probe.y;
      return dx * dx + dy * dy;
    };
    return distance(left) < distance(right);
  });
  cancel_unit_order(status, probe);
  bool path_ready{};
  for (const auto &approach : approaches) {
    if (plan_scv_path(status, probe, approach.x, approach.y)) {
      path_ready = true;
      break;
    }
  }
  if (!path_ready) {
    return false;
  }
  if (charge_resources) {
    minerals -= buildable.simulation.mineral_cost;
    gas -= buildable.simulation.gas_cost;
    if (probe.owner == 0U) {
      status.player_mineral_stock[0] = minerals;
      status.player_gas_stock[0] = gas;
    }
  }
  probe.active_order = ActiveUnitOrder::protoss_build;
  probe.construction_target_type = buildable.unit_type;
  probe.build_target_x = center_x;
  probe.build_target_y = center_y;
  probe.action_phase = 0U;
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
  const bool nydus_exit = status.nydus_parent_id != 0U &&
                          buildable->unit_type == 134U;
  if ((!nydus_exit &&
       !resource_cost_available(status, buildable->simulation.mineral_cost,
                                buildable->simulation.gas_cost)) ||
      !placement_is_valid(status, *buildable, status.placement_x,
                          status.placement_y)) {
    status.placement_valid = false;
    return false;
  }
  const ScenarioUnitPreview *const selected_source = first_selected_unit(status);
  const bool addon = (buildable->simulation.dat_flags & 2U) != 0U;
  std::uint32_t worker_id{};
  std::uint32_t parent_id{};
  bool terran_worker{};
  bool zerg_worker{};
  bool protoss_worker{};
  if (selected_source == nullptr || selected_source->owner != 0) {
    return false;
  }
  if (nydus_exit) {
    if (selected_source->unit_id != status.nydus_parent_id ||
        selected_source->unit_type != 134U ||
        !selected_source->construction_complete ||
        selected_source->attached_addon_id != 0U) {
      return false;
    }
    parent_id = selected_source->unit_id;
  } else if (addon) {
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
    terran_worker =
        starcraft::lang::is_terran_scv(selected_source->unit_type);
    zerg_worker = starcraft::lang::is_zerg_drone(selected_source->unit_type);
    protoss_worker =
        starcraft::lang::is_protoss_probe(selected_source->unit_type);
    if (!terran_worker && !zerg_worker && !protoss_worker) {
      return false;
    }
    worker_id = selected_source->unit_id;
  }
  if (protoss_worker) {
    ScenarioUnitPreview *const probe = find_unit_by_id(status, worker_id);
    if (probe == nullptr ||
        !begin_protoss_build_order(status, *probe, *buildable,
                                   status.placement_x, status.placement_y,
                                   true)) {
      return false;
    }
    status.placement_active = false;
    status.placement_valid = false;
    status.placement_unit_type = 0xFFFFU;
    status.active_command_card = 0U;
    return true;
  }
  try {
    std::size_t geyser_index = SIZE_MAX;
    const bool refinery = buildable->unit_type == 110U ||
                          buildable->unit_type == 149U ||
                          buildable->unit_type == 157U;
    if (refinery) {
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

    const auto initialize_construction =
        [&](ScenarioUnitPreview &building) -> bool {
      if (!configure_preview_type(status, building, buildable->unit_type)) {
        return false;
      }
      building.x = status.placement_x;
      building.y = status.placement_y;
      building.x_fixed = static_cast<std::int32_t>(building.x) << 8U;
      building.y_fixed = static_cast<std::int32_t>(building.y) << 8U;
      building.owner = 0U;
      building.is_building = true;
      // CUnitInit.cpp::sub_42E6B0 and CUnitBuild.cpp::sub_422D20 initialize
      // unfinished life at exactly one tenth maximum. CUnit+172 is the
      // units.dat build time divided by two.
      building.construction_complete = false;
      building.construction_ticks_total = static_cast<std::uint16_t>((
          std::max)(1U,
                    static_cast<unsigned>(buildable->simulation.build_time) >>
                        1U));
      building.construction_ticks_remaining =
          building.construction_ticks_total;
      building.hit_points =
          (std::max)(1U, building.max_hit_points / 10U);
      building.construction_builder_id = terran_worker ? worker_id : 0U;
      building.addon_parent_id = parent_id;
      building.construction_animation_phase = 0U;
      return true;
    };

    // CUnitZBuild.cpp::sub_4475E0 changes the Drone CUnit itself to the
    // selected structure for every entry except Extractor 149. Preserve that
    // identity and selection instead of creating a second unit.
    if (zerg_worker && buildable->unit_type != 149U) {
      ScenarioUnitPreview *const drone = find_unit_by_id(status, worker_id);
      if (drone == nullptr) {
        return false;
      }
      cancel_unit_order(status, *drone);
      if (!initialize_construction(*drone)) {
        return false;
      }
      status.player_minerals -= buildable->simulation.mineral_cost;
      status.player_gas -= buildable->simulation.gas_cost;
      status.placement_active = false;
      status.placement_valid = false;
      status.placement_unit_type = 0xFFFFU;
      status.active_command_card = 0U;
      return true;
    }

    ScenarioUnitPreview building{};
    building.unit_id = status.next_unit_id++;
    if (!initialize_construction(building)) {
      return false;
    }
    // Extractor is the sole Zerg branch that creates a separate building on
    // the geyser. Transfer selection before sub_4301A0's Drone cleanup.
    building.selected = zerg_worker && buildable->unit_type == 149U;
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
    if (zerg_worker && buildable->unit_type == 149U) {
      ScenarioUnitPreview *const drone = find_unit_by_id(status, worker_id);
      if (drone != nullptr) {
        cancel_unit_order(status, *drone);
        drone->alive = false;
        drone->selected = false;
      }
    }
    if (!nydus_exit) {
      status.player_minerals -= buildable->simulation.mineral_cost;
      status.player_gas -= buildable->simulation.gas_cost;
    }
    status.placement_active = false;
    status.placement_valid = false;
    status.placement_unit_type = 0xFFFFU;
    status.nydus_parent_id = 0U;
    status.active_command_card = 0;
    ScenarioUnitPreview *const worker = find_unit_by_id(status, worker_id);
    ScenarioUnitPreview *const created = find_unit_by_id(status, building_id);
    if (!addon && terran_worker && worker != nullptr && created != nullptr &&
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
