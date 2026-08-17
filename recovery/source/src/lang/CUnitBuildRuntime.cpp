#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_path_collide.hpp"
#include "starcraft/lang/flingy.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace starcraft::recovery {
namespace {

std::uint16_t synchronized_game_random(BootstrapStatus &status) noexcept {
  // net_misc.cpp::sub_4797B0 at 0x004797B0. The call-site tag only updates
  // diagnostics in the original; the synchronized value comes from this LCG.
  status.synchronized_random_state =
      22695477U * status.synchronized_random_state + 1U;
  return static_cast<std::uint16_t>(
      (status.synchronized_random_state >> 16U) & 0x7FFFU);
}

void clear_pending_terran_build(ScenarioUnitPreview &scv) noexcept {
  scv.construction_target_type = 0xFFFFU;
  scv.build_target_x = 0U;
  scv.build_target_y = 0U;
}

void refund_build_cost(BootstrapStatus &status,
                       const ScenarioUnitPreview &worker,
                       const BuildableUnitVisual &buildable) noexcept {
  std::uint32_t &minerals =
      worker.owner == 0U ? status.player_minerals
                         : status.player_mineral_stock[worker.owner];
  std::uint32_t &gas = worker.owner == 0U
                           ? status.player_gas
                           : status.player_gas_stock[worker.owner];
  minerals += buildable.simulation.mineral_cost;
  gas += buildable.simulation.gas_cost;
  if (worker.owner == 0U) {
    status.player_mineral_stock[0] = minerals;
    status.player_gas_stock[0] = gas;
  }
}

bool begin_build_site_approach(BootstrapStatus &status,
                               ScenarioUnitPreview &worker,
                               const BuildableUnitVisual &buildable,
                               const std::uint16_t center_x,
                               const std::uint16_t center_y) noexcept {
  const int half_width = buildable.placement_width / 2;
  const int half_height = buildable.placement_height / 2;
  std::array<starcraft::lang::PathPoint, 4> approaches{{
      {static_cast<std::uint16_t>((std::max)(
           0, static_cast<int>(center_x) - half_width -
                  static_cast<int>(worker.collision_right) - 2)),
       center_y},
      {static_cast<std::uint16_t>((std::min)(
           static_cast<int>(UINT16_MAX),
           static_cast<int>(center_x) + half_width +
               static_cast<int>(worker.collision_left) + 2)),
       center_y},
      {center_x,
       static_cast<std::uint16_t>((std::max)(
           0, static_cast<int>(center_y) - half_height -
                  static_cast<int>(worker.collision_bottom) - 2))},
      {center_x,
       static_cast<std::uint16_t>((std::min)(
           static_cast<int>(UINT16_MAX),
           static_cast<int>(center_y) + half_height +
               static_cast<int>(worker.collision_top) + 2))},
  }};
  std::sort(approaches.begin(), approaches.end(),
            [&worker](const auto &left, const auto &right) {
              const auto distance = [&worker](const auto &point) {
                const std::int64_t dx =
                    static_cast<int>(point.x) - worker.x;
                const std::int64_t dy =
                    static_cast<int>(point.y) - worker.y;
                return dx * dx + dy * dy;
              };
              return distance(left) < distance(right);
            });
  for (const auto &approach : approaches) {
    if (plan_scv_path(status, worker, approach.x, approach.y)) {
      return true;
    }
  }
  return false;
}

starcraft::lang::CollisionExtents collision_extents_for(
    const ScenarioUnitPreview &unit) noexcept {
  const bool from_dat = unit.collision_left != 0U ||
                        unit.collision_top != 0U ||
                        unit.collision_right != 0U ||
                        unit.collision_bottom != 0U;
  return {
      from_dat ? unit.collision_left : unit.selection_width / 2,
      from_dat ? unit.collision_top : unit.selection_height / 2,
      from_dat ? unit.collision_right
               : unit.selection_width - unit.selection_width / 2,
      from_dat ? unit.collision_bottom
               : unit.selection_height - unit.selection_height / 2,
  };
}

bool begin_completed_build_escape(BootstrapStatus &status,
                                  ScenarioUnitPreview &scv,
                                  const ScenarioUnitPreview &building) noexcept {
  if (!unit_rectangles_overlap_at(scv, scv.x, scv.y, building)) {
    return false;
  }
  const starcraft::lang::CollisionExtents worker_extents =
      collision_extents_for(scv);
  const starcraft::lang::CollisionExtents building_extents =
      collision_extents_for(building);
  const starcraft::lang::UnitCollisionBody worker_body{
      scv.unit_id, scv.unit_type, scv.x, scv.y, worker_extents,
      starcraft::lang::cunit_status_collision_enabled, scv.dat_flags,
      scv.sprite_elevation, false};
  const starcraft::lang::UnitCollisionBody building_body{
      building.unit_id, building.unit_type, building.x, building.y,
      building_extents,
      starcraft::lang::cunit_status_collision_enabled |
          starcraft::lang::cunit_status_building,
      building.dat_flags, building.sprite_elevation, false};
  const auto begin_escape_to = [&](const int x, const int y,
                                   const std::uint8_t direction) {
    if (x < 0 || y < 0 || x > UINT16_MAX || y > UINT16_MAX ||
        !plan_scv_path(status, scv, static_cast<std::uint16_t>(x),
                       static_cast<std::uint16_t>(y))) {
      return false;
    }
    ScenarioUnitPreview terminal = scv;
    terminal.x = scv.movement_final_x;
    terminal.y = scv.movement_final_y;
    if (unit_rectangles_overlap_at(terminal, terminal.x, terminal.y,
                                   building)) {
      return false;
    }
    scv.direction = direction;
    scv.movement_speed = 0U;
    (void)restart_unit_animation(status, scv, 11U);
    scv.moving = true;
    scv.active_order = ActiveUnitOrder::terran_build_exit;
    scv.order_target_id = building.unit_id;
    scv.action_phase = 0U;
    return true;
  };

  // CUnitPathCollide.cpp::sub_43A070/sub_439B90 classifies the contacting
  // rectangle side and produces up to two cardinal points one pixel beyond
  // the obstacle. This is the normal collision-path handoff that resumes
  // after CUnitBuild.cpp::sub_422540 releases the completed structure.
  const starcraft::lang::CollisionEscapePoints escapes =
      starcraft::lang::collision_escape_points(worker_body, building_body,
                                                scv.direction);
  for (std::size_t index = 0U; index < escapes.count; ++index) {
    const auto &escape = escapes.points[index];
    if (begin_escape_to(escape.x, escape.y, escape.direction)) {
      return true;
    }
  }

  // If another live unit blocks both cardinal sides, use the retail
  // place_unit.cpp::sub_47FBF0 expanding eight-pixel perimeter search to
  // obtain a legal endpoint, then visibly move there through the same exit
  // order rather than leaving the SCV embedded in the completed footprint.
  ScenarioUnitPreview settled = scv;
  if (!settle_created_unit(status, settled, scv.x, scv.y)) {
    return false;
  }
  return begin_escape_to(settled.x, settled.y,
                         starcraft::lang::direction_from_points(
                             scv.x, scv.y, settled.x, settled.y));
}

void advance_construction_life(BootstrapStatus &status,
                               ScenarioUnitPreview &building) noexcept {
  if (building.construction_ticks_remaining != 0U) {
    --building.construction_ticks_remaining;
  }
  const std::uint32_t initial_life =
      (std::max)(1U, building.max_hit_points / 10U);
  const std::uint32_t elapsed = building.construction_ticks_total -
                                building.construction_ticks_remaining;
  building.hit_points =
      initial_life + static_cast<std::uint32_t>(
                         static_cast<std::uint64_t>(
                             building.max_hit_points - initial_life) *
                         elapsed / building.construction_ticks_total);
  (void)advance_building_construction_animation(status, building);
}

}  // namespace

bool begin_terran_build_order(BootstrapStatus &status,
                              ScenarioUnitPreview &scv,
                              const BuildableUnitVisual &buildable,
                              const std::uint16_t center_x,
                              const std::uint16_t center_y,
                              const bool charge_resources) noexcept {
  if (!scv.alive || scv.is_building || scv.unit_type != 7U ||
      !placement_is_valid(status, buildable, center_x, center_y, scv.owner,
                          scv.unit_id)) {
    return false;
  }
  std::uint32_t &minerals =
      scv.owner == 0U ? status.player_minerals
                      : status.player_mineral_stock[scv.owner];
  std::uint32_t &gas = scv.owner == 0U
                           ? status.player_gas
                           : status.player_gas_stock[scv.owner];
  if (charge_resources &&
      (minerals < buildable.simulation.mineral_cost ||
       gas < buildable.simulation.gas_cost)) {
    return false;
  }

  // CUnitBuild.cpp::sub_422DF0 state zero issues the movement order first.
  // The incomplete CUnit is not allocated until state one confirms that the
  // SCV reached the requested footprint and placement is still legal.
  cancel_unit_order(status, scv);
  if (!begin_build_site_approach(status, scv, buildable, center_x, center_y)) {
    return false;
  }
  if (charge_resources) {
    minerals -= buildable.simulation.mineral_cost;
    gas -= buildable.simulation.gas_cost;
    if (scv.owner == 0U) {
      status.player_mineral_stock[0] = minerals;
      status.player_gas_stock[0] = gas;
    }
  }
  scv.movement_speed = 0U;
  (void)restart_unit_animation(status, scv, 11U);
  scv.moving = true;
  scv.active_order = ActiveUnitOrder::terran_build;
  scv.construction_target_type = buildable.unit_type;
  scv.build_target_x = center_x;
  scv.build_target_y = center_y;
  scv.action_phase = 1U;
  return true;
}

bool complete_terran_build_order(BootstrapStatus &status,
                                 ScenarioUnitPreview &scv) noexcept {
  if (!scv.alive || scv.active_order != ActiveUnitOrder::terran_build ||
      scv.construction_target_type == 0xFFFFU) {
    return false;
  }
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, scv.construction_target_type);
  const auto finish_failed_order = [&](const bool refund) {
    if (refund && buildable != nullptr) {
      refund_build_cost(status, scv, *buildable);
    }
    clear_pending_terran_build(scv);
    cancel_unit_order(status, scv);
    (void)restart_unit_animation(status, scv, 12U);
  };
  if (buildable == nullptr || scv.x != scv.movement_final_x ||
      scv.y != scv.movement_final_y ||
      !placement_is_valid(status, *buildable, scv.build_target_x,
                          scv.build_target_y, scv.owner, scv.unit_id)) {
    finish_failed_order(true);
    return false;
  }

  std::uint32_t geyser_id{};
  if (buildable->unit_type == 110U) {
    for (const ScenarioUnitPreview &unit : status.units) {
      if (unit.alive && unit.unit_type == 188U &&
          std::abs(static_cast<int>(unit.x) - scv.build_target_x) <= 16 &&
          std::abs(static_cast<int>(unit.y) - scv.build_target_y) <= 16) {
        geyser_id = unit.unit_id;
        break;
      }
    }
    if (geyser_id == 0U) {
      finish_failed_order(true);
      return false;
    }
  }

  const std::uint32_t worker_id = scv.unit_id;
  const std::uint8_t owner = scv.owner;
  const std::uint16_t target_x = scv.build_target_x;
  const std::uint16_t target_y = scv.build_target_y;
  try {
    ScenarioUnitPreview building{};
    building.unit_id = status.next_unit_id++;
    building.owner = owner;
    if (!configure_preview_type(status, building, buildable->unit_type)) {
      --status.next_unit_id;
      finish_failed_order(true);
      return false;
    }
    building.x = target_x;
    building.y = target_y;
    building.x_fixed = static_cast<std::int32_t>(target_x) << 8U;
    building.y_fixed = static_cast<std::int32_t>(target_y) << 8U;
    building.is_building = true;
    building.construction_complete = false;
    building.construction_ticks_total = static_cast<std::uint16_t>((
        std::max)(1U,
                  static_cast<unsigned>(buildable->simulation.build_time) >>
                      1U));
    building.construction_ticks_remaining =
        building.construction_ticks_total;
    building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
    building.construction_builder_id = worker_id;
    building.construction_animation_phase = 0U;
    if (geyser_id != 0U) {
      const ScenarioUnitPreview *const geyser =
          find_unit_by_id(status, geyser_id);
      if (geyser != nullptr) {
        building.resource_amount = geyser->resource_amount;
      }
    }
    // CUnitInit.cpp::sub_42E6B0 calls CUnitGUI.cpp::sub_42BA80(this, 1)
    // for every incomplete building. That selects units.dat field 4 before
    // the first visible frame; the completed structure image must not flash.
    if (buildable->construction_asset_index != SIZE_MAX &&
        !replace_preview_primary_image(status, building,
                                       buildable->construction_asset_index)) {
      --status.next_unit_id;
      finish_failed_order(true);
      return false;
    }
    const std::uint32_t building_id = building.unit_id;
    status.units.push_back(std::move(building));
    if (geyser_id != 0U) {
      ScenarioUnitPreview *const geyser = find_unit_by_id(status, geyser_id);
      if (geyser != nullptr) {
        geyser->alive = false;
        geyser->selected = false;
      }
    }
    ScenarioUnitPreview *const builder = find_unit_by_id(status, worker_id);
    if (builder == nullptr) {
      return false;
    }
    clear_pending_terran_build(*builder);
    builder->active_order = ActiveUnitOrder::construct;
    builder->order_target_id = building_id;
    builder->action_timer = 0U;
    // sub_422DF0 installs order 35 with CUnit+90 equal to three. The next
    // sub_422540 update transitions into the randomized working-point loop.
    builder->action_phase = 3U;
    builder->moving = false;
    builder->movement_speed = 0U;
    builder->movement_path.clear();
    builder->movement_path_index = 0U;
    return true;
  } catch (...) {
    finish_failed_order(true);
    return false;
  }
}

bool advance_terran_construction_order(BootstrapStatus &status,
                                       ScenarioUnitPreview &scv,
                                       ScenarioUnitPreview &building) noexcept {
  if (!scv.alive || scv.active_order != ActiveUnitOrder::construct ||
      !building.alive || !building.is_building ||
      building.owner != scv.owner || building.construction_complete ||
      building.construction_ticks_total == 0U) {
    cancel_unit_order(status, scv);
    return false;
  }

  building.construction_builder_id = scv.unit_id;
  // CUnitBuild.cpp::sub_422540 at 0x00422540. States 4..8 repeatedly choose
  // a point across the current construction sprite, move the SCV within 20
  // pixels, face the structure, weld for 30..93 turns, and repeat. Build life
  // advances throughout that movement/weld loop.
  switch (scv.action_phase) {
    case 0U:
    case 2U:
      scv.action_phase = 3U;
      return true;
    case 3U:
      scv.action_phase = 4U;
      return true;
    case 4U: {
      const std::uint16_t random = synchronized_game_random(status);
      int sprite_width = static_cast<int>(building.selection_width);
      int sprite_height = static_cast<int>(building.selection_height);
      if (building.asset_index < status.unit_assets.size()) {
        const UnitRenderAsset &asset = status.unit_assets[building.asset_index];
        sprite_width = (std::max)(1, (std::min)(255,
            static_cast<int>(asset.sprite_canvas_width)));
        sprite_height = (std::max)(1, (std::min)(255,
            static_cast<int>(asset.sprite_canvas_height)));
      }
      const int left_extent = scv.collision_left != 0U
                                  ? scv.collision_left
                                  : scv.selection_width / 2;
      const int top_extent = scv.collision_top != 0U
                                 ? scv.collision_top
                                 : scv.selection_height / 2;
      const int right_extent = scv.collision_right != 0U
                                   ? scv.collision_right
                                   : scv.selection_width -
                                         scv.selection_width / 2;
      const int bottom_extent = scv.collision_bottom != 0U
                                    ? scv.collision_bottom
                                    : scv.selection_height -
                                          scv.selection_height / 2;
      const int map_width = static_cast<int>(status.scenario_width) * 32;
      const int map_height = static_cast<int>(status.scenario_height) * 32;
      const int x = (std::clamp)(
          static_cast<int>(building.x) +
              static_cast<int>((random >> 8U) & 0xFFU) % sprite_width -
              sprite_width / 2,
          left_extent, (std::max)(left_extent, map_width - right_extent - 1));
      const int y = (std::clamp)(
          static_cast<int>(building.y) +
              static_cast<int>(random & 0xFFU) % sprite_height -
              sprite_height / 2,
          top_extent, (std::max)(top_extent, map_height - bottom_extent - 1));
      scv.build_target_x = static_cast<std::uint16_t>(x);
      scv.build_target_y = static_cast<std::uint16_t>(y);
      if (plan_scv_path(status, scv, scv.build_target_x,
                        scv.build_target_y)) {
        scv.movement_speed = 0U;
        (void)restart_unit_animation(status, scv, 11U);
        scv.moving = true;
        scv.action_phase = 5U;
      } else {
        scv.action_phase = 6U;
      }
      break;
    }
    case 5U:
      scv.action_phase = 6U;
      break;
    case 6U: {
      const std::int64_t dx = static_cast<int>(scv.build_target_x) - scv.x;
      const std::int64_t dy = static_cast<int>(scv.build_target_y) - scv.y;
      if (dx * dx + dy * dy <= 20 * 20) {
        stop_unit_movement(status, scv);
        scv.action_phase = 7U;
      }
      break;
    }
    case 7U: {
      const std::uint8_t desired = starcraft::lang::direction_from_points(
          scv.x, scv.y, building.x, building.y);
      const std::int8_t turn = starcraft::lang::clamp_turn_delta(
          scv.direction, desired, (std::max)(1U,
              static_cast<unsigned>(scv.movement_turn_speed)));
      scv.direction = static_cast<std::uint8_t>(scv.direction + turn);
      const std::uint8_t clockwise =
          static_cast<std::uint8_t>(scv.direction - desired);
      const unsigned difference =
          clockwise > 128U ? 256U - clockwise : clockwise;
      if (difference <= (std::max)(1U,
                                  static_cast<unsigned>(scv.movement_turn_speed))) {
        scv.action_timer = static_cast<std::uint16_t>(
            (synchronized_game_random(status) & 0x3FU) + 30U);
        (void)restart_unit_animation(status, scv, 15U);
        scv.action_phase = 8U;
      }
      break;
    }
    case 8U:
      if (scv.action_timer != 0U) {
        --scv.action_timer;
      }
      if (scv.action_timer == 0U) {
        (void)restart_unit_animation(status, scv, 8U);
        scv.action_phase = 4U;
      }
      break;
    default:
      scv.action_phase = 4U;
      break;
  }

  advance_construction_life(status, building);
  if (building.construction_ticks_remaining != 0U) {
    return true;
  }

  building.hit_points = building.max_hit_points;
  building.construction_complete = true;
  building.construction_builder_id = 0U;
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, building.unit_type);
  if (buildable != nullptr && buildable->asset_index != SIZE_MAX &&
      building.asset_index != buildable->asset_index) {
    (void)replace_preview_primary_image(status, building,
                                        buildable->asset_index);
  }
  // CUnitInit.cpp::sub_42EBB0 first restores the normal units.dat image and
  // then dispatches action 16 to enter the completed building's idle script.
  (void)restart_unit_animation(status, building, 16U);
  if (building.unit_type == 110U) {
    scv.active_order = ActiveUnitOrder::none;
    scv.order_target_id = 0U;
    scv.action_phase = 0U;
    if (!begin_scv_interaction(status, scv, building,
                               ActiveUnitOrder::gather)) {
      cancel_unit_order(status, scv);
    }
  } else {
    if (!begin_completed_build_escape(status, scv, building)) {
      // The worker already ended outside the completed collision rectangle.
      // sub_422540 therefore falls directly through its normal idle order.
      cancel_unit_order(status, scv);
    }
  }
  return true;
}

bool advance_unit_production(BootstrapStatus &status,
                             const std::uint32_t now) noexcept {
  bool changed{};
  try {
    const std::size_t producer_count = status.units.size();
    for (std::size_t source_index = 0; source_index < producer_count;
         ++source_index) {
      ScenarioUnitPreview &source = status.units[source_index];
      const std::uint16_t product_type = source.production_queue.front();
      if (!source.alive || !source.production_active ||
          product_type == starcraft::lang::cunit_production_empty ||
          product_type >= status.runtime_unit_types.size()) {
        continue;
      }
      const RuntimeUnitType &product = status.runtime_unit_types[product_type];
      const std::uint16_t total_ticks = starcraft::lang::production_total_ticks(
          product.initialization.simulation.build_time);
      const std::uint32_t duration =
          static_cast<std::uint32_t>(total_ticks) * kSimulationTickMilliseconds;
      const std::int32_t elapsed =
          static_cast<std::int32_t>(now - source.production_started);
      if (!product.ready || total_ticks == 0U || elapsed < 0 ||
          static_cast<std::uint32_t>(elapsed) < duration) {
        continue;
      }

      if (source.production_kind ==
          starcraft::lang::UnitProductionKind::zerg_larva_morph) {
        source.production_queue.advance();
        source.production_active = false;
        source.production_started = 0;
        (void)configure_preview_type(status, source, product_type);
        // sub_447820 completes the egg's queued morph by changing its CUnit
        // type and dispatching the completion/idle transition.
        (void)restart_unit_animation(status, source, 13U);
        (void)queue_unit_ready_sound(status, source);
        changed = true;
        continue;
      }

      if (source.production_kind ==
          starcraft::lang::UnitProductionKind::carrier_hangar) {
        auto free_slot = std::find(source.hangar_unit_ids.begin(),
                                   source.hangar_unit_ids.end(), 0U);
        const std::size_t maximum =
            source.owner < status.player_upgrade_levels.size() &&
                    ((source.unit_type == 83U &&
                      status.player_upgrade_levels[source.owner][36U] != 0U) ||
                     ((source.unit_type == 72U || source.unit_type == 82U) &&
                      status.player_upgrade_levels[source.owner][43U] != 0U))
                ? 10U
                : 5U;
        const std::size_t current = static_cast<std::size_t>(std::count_if(
            source.hangar_unit_ids.begin(), source.hangar_unit_ids.end(),
            [](const std::uint32_t id) { return id != 0U; }));
        if (free_slot == source.hangar_unit_ids.end() || current >= maximum) {
          source.production_queue.advance();
          source.production_active = !source.production_queue.empty();
          source.production_started = source.production_active ? now : 0U;
          changed = true;
          continue;
        }
        ScenarioUnitPreview fighter{};
        fighter.unit_id = status.next_unit_id++;
        fighter.owner = source.owner;
        if (!configure_preview_type(status, fighter, product_type)) {
          --status.next_unit_id;
          continue;
        }
        fighter.x = source.x;
        fighter.y = source.y;
        fighter.x_fixed = static_cast<std::int32_t>(fighter.x) << 8U;
        fighter.y_fixed = static_cast<std::int32_t>(fighter.y) << 8U;
        fighter.hangar_parent_id = source.unit_id;
        fighter.sprite_hidden = true;
        *free_slot = fighter.unit_id;
        source.production_queue.advance();
        source.production_active = !source.production_queue.empty();
        source.production_started = source.production_active ? now : 0U;
        status.units.push_back(std::move(fighter));
        changed = true;
        continue;
      }

      const std::uint16_t source_x = source.x;
      const std::uint16_t source_y = source.y;
      const std::uint8_t owner = source.owner;
      ScenarioUnitPreview produced{};
      produced.unit_id = status.next_unit_id;
      produced.owner = owner;
      if (!configure_preview_type(status, produced, product_type)) {
        continue;
      }
      // sub_421EF0 creates the incomplete CUnit at the producer sprite's
      // exact x/y.  sub_423580 -> sub_42EFC0 -> sub_42EE60 only completes and
      // unhides it after sub_47FBF0 finds a legal position around that point.
      produced.x = source_x;
      produced.y = source_y;
      produced.x_fixed = static_cast<std::int32_t>(produced.x) << 8U;
      produced.y_fixed = static_cast<std::int32_t>(produced.y) << 8U;
      if (!settle_created_unit(status, produced, source_x, source_y)) {
        // The original completion finalizer returns zero here.  The producer
        // keeps its queue entry and retries instead of exposing the trainee
        // at the building center or silently losing the completed unit.
        continue;
      }
      ++status.next_unit_id;

      source.production_queue.advance();
      if (source.production_queue.empty()) {
        // Empty sentinel 228 makes sub_423020 dispatch animation 20.
        (void)restart_unit_animation(status, source, 20U);
        source.production_active = false;
        source.production_started = 0;
      } else {
        source.production_started = now;
      }
      status.units.push_back(std::move(produced));
      (void)queue_unit_ready_sound(status, status.units.back());
      changed = true;
    }
    return changed;
  } catch (...) {
    return false;
  }
}

const BuildableUnitVisual *
find_buildable_unit(const BootstrapStatus &status,
                    const std::uint16_t unit_type) noexcept {
  const auto entry =
      std::find_if(status.buildable_units.begin(), status.buildable_units.end(),
                   [unit_type](const BuildableUnitVisual &value) {
                     return value.unit_type == unit_type;
                   });
  return entry == status.buildable_units.end() ? nullptr : &*entry;
}

} // namespace starcraft::recovery
