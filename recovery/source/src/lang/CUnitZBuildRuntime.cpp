#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_zerg.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace starcraft::recovery {
namespace {

void advance_zerg_build_life(ScenarioUnitPreview &building) noexcept {
  if (building.construction_ticks_total == 0U) {
    return;
  }
  if (building.construction_ticks_remaining != 0U) {
    --building.construction_ticks_remaining;
  }
  const std::uint32_t initial_life =
      (std::max)(1U, building.max_hit_points / 10U);
  const std::uint32_t elapsed = building.construction_ticks_total -
                                building.construction_ticks_remaining;
  building.hit_points =
      initial_life + static_cast<std::uint32_t>(
                         static_cast<std::uint64_t>(building.max_hit_points -
                                                    initial_life) *
                         elapsed / building.construction_ticks_total);
}

}  // namespace

bool complete_zerg_build_order(BootstrapStatus &status,
                               ScenarioUnitPreview &drone) noexcept {
  if (!drone.alive || drone.active_order != ActiveUnitOrder::zerg_build ||
      drone.construction_target_type == 0xFFFFU) {
    return false;
  }
  const std::uint16_t target_type = drone.construction_target_type;
  const std::uint16_t target_x = drone.build_target_x;
  const std::uint16_t target_y = drone.build_target_y;
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, target_type);
  const auto clear_order = [&] {
    drone.construction_target_type = 0xFFFFU;
    drone.build_target_x = 0U;
    drone.build_target_y = 0U;
    cancel_unit_order(status, drone);
  };
  const auto refund = [&] {
    if (buildable == nullptr) {
      return;
    }
    std::uint32_t &minerals =
        drone.owner == 0U ? status.player_minerals
                          : status.player_mineral_stock[drone.owner];
    std::uint32_t &gas = drone.owner == 0U
                             ? status.player_gas
                             : status.player_gas_stock[drone.owner];
    minerals += buildable->simulation.mineral_cost;
    gas += buildable->simulation.gas_cost;
    if (drone.owner == 0U) {
      status.player_mineral_stock[0] = minerals;
      status.player_gas_stock[0] = gas;
    }
  };

  if (drone.x != drone.movement_final_x ||
      drone.y != drone.movement_final_y || buildable == nullptr) {
    refund();
    clear_order();
    (void)restart_unit_animation(status, drone, 12U);
    return false;
  }

  if (drone.action_phase == 1U) {
    if (!placement_is_valid(status, *buildable, target_x, target_y,
                            drone.owner, drone.unit_id) ||
        !plan_scv_path(status, drone, target_x, target_y)) {
      refund();
      clear_order();
      (void)restart_unit_animation(status, drone, 12U);
      return false;
    }
    // sub_448E60 calls sub_445220 for the center and sub_41C380 for the
    // Drone's final ground-settling movement before entering state two.
    drone.movement_speed = 0U;
    (void)restart_unit_animation(status, drone, 11U);
    drone.moving = true;
    drone.active_order = ActiveUnitOrder::zerg_build;
    drone.action_phase = 2U;
    return true;
  }

  if (drone.action_phase != 2U ||
      !placement_is_valid(status, *buildable, target_x, target_y,
                          drone.owner, drone.unit_id)) {
    refund();
    clear_order();
    (void)restart_unit_animation(status, drone, 12U);
    return false;
  }

  std::size_t geyser_index = SIZE_MAX;
  if (target_type == 149U) {
    for (std::size_t index = 0U; index < status.units.size(); ++index) {
      const ScenarioUnitPreview &unit = status.units[index];
      if (unit.alive && unit.unit_type == 188U &&
          std::abs(static_cast<int>(unit.x) - target_x) <= 16 &&
          std::abs(static_cast<int>(unit.y) - target_y) <= 16) {
        geyser_index = index;
        break;
      }
    }
    if (geyser_index == SIZE_MAX) {
      refund();
      clear_order();
      return false;
    }
  }

  const auto initialize_construction =
      [&](ScenarioUnitPreview &building, const std::uint8_t owner) {
    if (!configure_preview_type(status, building, target_type)) {
      return false;
    }
    building.x = target_x;
    building.y = target_y;
    building.x_fixed = static_cast<std::int32_t>(target_x) << 8U;
    building.y_fixed = static_cast<std::int32_t>(target_y) << 8U;
    building.owner = owner;
    building.is_building = true;
    building.construction_complete = false;
    building.construction_visible = true;
    building.construction_ticks_total = static_cast<std::uint16_t>((
        std::max)(1U,
                  static_cast<unsigned>(buildable->simulation.build_time) >>
                      1U));
    building.construction_ticks_remaining =
        building.construction_ticks_total;
    building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
    building.construction_animation_phase = 0U;
    building.construction_target_type = 0xFFFFU;
    building.active_order = ActiveUnitOrder::none;
    building.moving = false;
    building.movement_path.clear();
    // sub_4475E0 calls sub_42BA80(this, 1) after changing the Drone's CUnit
    // type. That routine replaces the primary sprite image with units.dat
    // field 4; actions 13/14/15 animate this cocoon instead of exposing the
    // completed structure graphic during construction.
    return buildable->construction_asset_index == SIZE_MAX ||
           replace_preview_primary_image(status, building,
                                         buildable->construction_asset_index);
  };

  try {
    if (target_type != 149U) {
      const std::uint8_t owner = drone.owner;
      clear_order();
      if (!initialize_construction(drone, owner)) {
        refund();
        return false;
      }
      // sub_4475E0 transforms the Drone CUnit itself and dispatches order
      // 0x2e. The structure's own Iscript actions 13/14/15 render the Zerg
      // cocoon growth and the final blood-burst handoff.
      return true;
    }

    ScenarioUnitPreview extractor{};
    extractor.unit_id = status.next_unit_id++;
    const std::uint8_t owner = drone.owner;
    const bool selected = drone.selected;
    if (!initialize_construction(extractor, owner)) {
      refund();
      clear_order();
      return false;
    }
    extractor.selected = selected;
    extractor.resource_amount = status.units[geyser_index].resource_amount;
    if (status.zerg_extractor_construction_asset_index <
        status.unit_assets.size()) {
      const UnitRenderAsset &construction = status.unit_assets[
          status.zerg_extractor_construction_asset_index];
      extractor.dynamic_overlay_asset_index =
          status.zerg_extractor_construction_asset_index;
      extractor.dynamic_overlay_iscript_state =
          construction.initial_iscript_state;
      extractor.current_dynamic_overlay_frame =
          construction.initial_iscript_state.frame;
      extractor.dynamic_overlay_above = true;
      extractor.dynamic_overlay_ready = construction.iscript_ready;
    }
    status.units[geyser_index].alive = false;
    status.units[geyser_index].selected = false;
    clear_order();
    drone.alive = false;
    drone.selected = false;
    status.units.push_back(std::move(extractor));
    // The retail Extractor branch is the sole exception: it creates a new
    // CUnit on the geyser, removes the Drone, and attaches image 314. Its
    // normal structure actions still drive the recovered construction phase.
    return true;
  } catch (...) {
    return false;
  }
}

bool advance_zerg_building_construction(BootstrapStatus &status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview &building : status.units) {
    const bool building_morph =
        starcraft::lang::is_zerg_building_morph_target(
            building.construction_target_type);
    if (!building.alive || !building.is_building ||
        building.construction_complete ||
        (!starcraft::lang::is_zerg_buildable_unit_type(building.unit_type) &&
         !building_morph) ||
        building.construction_ticks_total == 0U) {
      continue;
    }

    // CUnitZBuild.cpp::sub_447D80 uses CUnit+90 as this state. Its exact
    // thresholds are remaining build time below 3/4, below 1/4, and zero;
    // actions 13, 14, and 15 are dispatched on the following states.
    switch (building.construction_animation_phase) {
      case 0U:
        if (building.construction_ticks_remaining <
            (3U * building.construction_ticks_total) / 4U) {
          building.construction_animation_phase = 1U;
        }
        break;
      case 1U:
        // The Extractor branch attaches image 314 when it consumes the
        // geyser. sub_447D80 removes that one-off layer at the first growth
        // threshold before dispatching action 13 to the construction image.
        if (building.unit_type == 149U &&
            building.dynamic_overlay_asset_index ==
                status.zerg_extractor_construction_asset_index) {
          building.dynamic_overlay_ready = false;
          building.dynamic_overlay_asset_index = SIZE_MAX;
        }
        (void)restart_unit_animation(status, building, 13U);
        building.construction_animation_phase = 2U;
        break;
      case 2U:
        if (building.construction_ticks_remaining <
            building.construction_ticks_total / 4U) {
          building.construction_animation_phase = 3U;
        }
        break;
      case 3U:
        (void)restart_unit_animation(status, building, 14U);
        building.construction_animation_phase = 4U;
        break;
      case 4U:
        if (building.construction_ticks_remaining == 0U) {
          building.construction_animation_phase = 5U;
        }
        break;
      case 5U:
        // Action 15 is the construction image's final blood-burst sequence.
        // Its opcode-0x27 signal owns the handoff to the completed building;
        // only synthesize that signal when malformed/missing data prevents
        // the licensed construction script from starting.
        if (!restart_unit_animation(status, building, 15U)) {
          building.iscript_state.image_target_flags |= 4U;
        }
        building.construction_animation_phase = 6U;
        break;
      case 6U:
        // sub_447D80 waits for IScript opcode 0x27 to OR bit 4 into CUnit+91.
        if ((building.iscript_state.image_target_flags & 4U) != 0U) {
          building.iscript_state.image_target_flags &=
              static_cast<std::uint8_t>(~4U);
          if (building_morph) {
            const std::uint16_t target = building.construction_target_type;
            if (!configure_preview_type(status, building, target)) {
              break;
            }
            building.construction_target_type = 0xFFFFU;
          } else {
            const BuildableUnitVisual *const completed =
                find_buildable_unit(status, building.unit_type);
            if (completed == nullptr ||
                !replace_preview_primary_image(status, building,
                                               completed->asset_index)) {
              break;
            }
          }
          building.hit_points = building.max_hit_points;
          building.construction_complete = true;
          building.construction_ticks_total = 0U;
          building.construction_ticks_remaining = 0U;
          (void)restart_unit_animation(status, building, 16U);
        }
        changed = true;
        continue;
      default:
        break;
    }

    advance_zerg_build_life(building);
    changed = true;
  }
  return changed;
}

}  // namespace starcraft::recovery
