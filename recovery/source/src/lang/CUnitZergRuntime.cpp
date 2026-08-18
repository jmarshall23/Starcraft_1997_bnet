#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/flingy.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

namespace {

struct SeparationDirection {
  int x{};
  int y{};
};

constexpr std::array<SeparationDirection, 8> kSeparationDirections{{
    {0, -1}, {1, -1}, {1, 0}, {1, 1},
    {0, 1},  {-1, 1}, {-1, 0}, {-1, -1},
}};

std::size_t direction_slot(const std::uint8_t direction) noexcept {
  return (static_cast<std::size_t>(direction) + 16U) / 32U %
         kSeparationDirections.size();
}

bool begin_separation_move(BootstrapStatus &status,
                           ScenarioUnitPreview &unit, const int origin_x,
                           const int origin_y, const int distance_x,
                           const int distance_y,
                           const std::size_t preferred_slot) noexcept {
  if (!unit.alive || unit.sprite_hidden || unit.is_building ||
      unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
    return false;
  }

  // CUnitInit.cpp::sub_42F380 removes the old CUnitBounds entry through
  // sub_4200C0, changes the unit type, then reinserts the new rectangle with
  // sub_41FF70.  CUnitPathCollide resolves units caught by that expanded
  // rectangle in cardinal order.  Keep that recovered ordering and use the
  // place_unit.cpp::sub_47FBF0 perimeter search for the legal endpoint.
  constexpr std::array<int, 8> kSearchTurns{{0, 1, -1, 2, -2, 3, -3, 4}};
  const int maximum_x = (std::max)(
      0, static_cast<int>(status.pathing_map.pixel_width()) - 1);
  const int maximum_y = (std::max)(
      0, static_cast<int>(status.pathing_map.pixel_height()) - 1);
  for (const int turn : kSearchTurns) {
    const int slot_value = static_cast<int>(preferred_slot) + turn;
    const std::size_t slot = static_cast<std::size_t>(
        (slot_value + static_cast<int>(kSeparationDirections.size())) %
        static_cast<int>(kSeparationDirections.size()));
    const SeparationDirection direction = kSeparationDirections[slot];
    const int requested_x = (std::clamp)(
        origin_x + direction.x * distance_x, 0, maximum_x);
    const int requested_y = (std::clamp)(
        origin_y + direction.y * distance_y, 0, maximum_y);
    ScenarioUnitPreview endpoint = unit;
    if (!settle_created_unit(status, endpoint,
                             static_cast<std::uint16_t>(requested_x),
                             static_cast<std::uint16_t>(requested_y)) ||
        (endpoint.x == unit.x && endpoint.y == unit.y) ||
        !plan_scv_path(status, unit, endpoint.x, endpoint.y)) {
      continue;
    }
    unit.movement_speed = 0U;
    unit.moving = true;
    if (unit.active_order == ActiveUnitOrder::none) {
      unit.active_order = ActiveUnitOrder::move;
      unit.order_target_id = 0U;
    }
    (void)restart_unit_animation(status, unit, 11U);
    return true;
  }
  return false;
}

bool begin_hatched_unit_release(BootstrapStatus &status,
                                ScenarioUnitPreview &unit) noexcept {
  const ScenarioUnitPreview *const parent =
      find_unit_by_id(status, unit.larva_parent_id);
  const std::uint8_t radial_direction =
      parent != nullptr && (parent->x != unit.x || parent->y != unit.y)
          ? starcraft::lang::direction_from_points(parent->x, parent->y,
                                                   unit.x, unit.y)
          : static_cast<std::uint8_t>(unit.unit_id * 37U);
  const int distance_x =
      (std::max)(12, static_cast<int>(unit.selection_width) / 2 + 8);
  const int distance_y =
      (std::max)(12, static_cast<int>(unit.selection_height) / 2 + 8);
  return begin_separation_move(status, unit, unit.x, unit.y, distance_x,
                               distance_y,
                               direction_slot(radial_direction));
}

} // namespace

std::size_t displace_units_for_zerg_egg(BootstrapStatus &status,
                                        ScenarioUnitPreview &egg) noexcept {
  std::size_t displaced{};
  try {
    for (ScenarioUnitPreview &unit : status.units) {
      if (unit.unit_id == egg.unit_id || !unit.alive || unit.sprite_hidden ||
          unit.is_building ||
          is_airborne(unit) != is_airborne(egg) ||
          !unit_footprints_overlap_at(unit, unit.x, unit.y, egg)) {
        continue;
      }
      const std::uint8_t radial_direction =
          unit.x != egg.x || unit.y != egg.y
              ? starcraft::lang::direction_from_points(egg.x, egg.y, unit.x,
                                                       unit.y)
              : static_cast<std::uint8_t>(unit.unit_id * 37U);
      const int distance_x =
          (std::max)(static_cast<int>(egg.collision_left),
                     static_cast<int>(egg.collision_right)) +
          (std::max)(static_cast<int>(unit.collision_left),
                     static_cast<int>(unit.collision_right)) +
          6;
      const int distance_y =
          (std::max)(static_cast<int>(egg.collision_top),
                     static_cast<int>(egg.collision_bottom)) +
          (std::max)(static_cast<int>(unit.collision_top),
                     static_cast<int>(unit.collision_bottom)) +
          6;
      displaced += begin_separation_move(
                       status, unit, egg.x, egg.y, (std::max)(12, distance_x),
                       (std::max)(12, distance_y),
                       direction_slot(radial_direction))
                       ? 1U
                       : 0U;
    }
  } catch (...) {
    return displaced;
  }
  return displaced;
}

bool advance_zerg_larvae(BootstrapStatus &status) noexcept {
  bool changed{};
  try {
    const std::size_t initial_count = status.units.size();
    // CUnitZerg.cpp::sub_449270 is order 0x4C. A larva outside range ten of
    // its hatchery tether moves back to that CUnit; an idle larva already in
    // range selects a passable diagonal ten-pixel step. The original order
    // dispatcher revisits this low-priority wander every eight turns.
    for (std::size_t larva_index = 0U; larva_index < initial_count;
         ++larva_index) {
      ScenarioUnitPreview &larva = status.units[larva_index];
      if (larva.alive && larva.zerg_hatch_release_pending) {
        // CUnitZBuild.cpp::sub_447820 waits for target flag four from the
        // special-state image before exposing the final unit order.  Only
        // then may collision release visibly walk the unit away from its egg.
        if ((larva.iscript_state.image_target_flags & 4U) != 0U ||
            !larva.iscript_state.active) {
          larva.iscript_state.image_target_flags &=
              static_cast<std::uint8_t>(~4U);
          larva.zerg_hatch_release_pending = false;
          // sub_42F380 has already reinserted the final CUnit bounds. Repeat
          // the overlap release at the IScript birth boundary because a unit
          // walking clear of the smaller Egg can still intersect the larger
          // hatched footprint by the time flag four is published.
          changed = displace_units_for_zerg_egg(status, larva) != 0U ||
                    changed;
          changed = begin_hatched_unit_release(status, larva) || changed;
        }
        continue;
      }
      if (!larva.alive || larva.unit_type != starcraft::lang::zerg_larva_type ||
          larva.production_active || larva.active_order != ActiveUnitOrder::none ||
          larva.moving) {
        continue;
      }
      if (larva.action_timer != 0U) {
        --larva.action_timer;
        continue;
      }
      larva.action_timer = 8U;
      const ScenarioUnitPreview *const parent =
          find_unit_by_id(status, larva.larva_parent_id);
      if (parent == nullptr) {
        continue;
      }

      std::uint16_t target_x = parent->x;
      std::uint16_t target_y = parent->y;
      if (unit_edge_distance(larva, *parent) <= 10) {
        const std::uint8_t choice = static_cast<std::uint8_t>(
            larva.unit_id + larva.action_phase++);
        const int candidate_x = static_cast<int>(larva.x) +
                                ((choice & 1U) != 0U ? 10 : -10);
        const int candidate_y = static_cast<int>(larva.y) +
                                ((choice & 2U) != 0U ? 10 : -10);
        if (candidate_x < 0 || candidate_y < 0 || candidate_x > UINT16_MAX ||
            candidate_y > UINT16_MAX) {
          continue;
        }
        target_x = static_cast<std::uint16_t>(candidate_x);
        target_y = static_cast<std::uint16_t>(candidate_y);
      }
      if (plan_scv_path(status, larva, target_x, target_y)) {
        larva.moving = true;
        larva.movement_speed = 0U;
        (void)restart_unit_animation(status, larva, 11U);
        changed = true;
      }
    }

    for (std::size_t base_index = 0; base_index < initial_count; ++base_index) {
      ScenarioUnitPreview &base = status.units[base_index];
      if (!base.alive || !base.construction_complete ||
          !starcraft::lang::is_zerg_town_hall(base.unit_type)) {
        continue;
      }
      if (base.larva_spawn_timer != 0U) {
        --base.larva_spawn_timer;
        continue;
      }
      base.larva_spawn_timer = starcraft::lang::zerg_larva_spawn_period;
      const std::size_t larvae = static_cast<std::size_t>(std::count_if(
          status.units.begin(), status.units.end(),
          [&base](const ScenarioUnitPreview &unit) {
            return unit.alive && unit.owner == base.owner &&
                   unit.unit_type == starcraft::lang::zerg_larva_type &&
                   unit.larva_parent_id == base.unit_id;
          }));
      if (larvae >= starcraft::lang::zerg_larva_limit) {
        continue;
      }

      const int left = static_cast<int>(base.x) - base.collision_left;
      const int top = static_cast<int>(base.y) - base.collision_top;
      const int right = static_cast<int>(base.x) + base.collision_right;
      const int bottom = static_cast<int>(base.y) + base.collision_bottom;
      const std::array<starcraft::lang::PathPoint, 4> candidates{{
          {base.x, static_cast<std::uint16_t>((std::max)(0, bottom + 10))},
          {static_cast<std::uint16_t>((std::max)(0, left - 10)), base.y},
          {static_cast<std::uint16_t>((std::max)(0, right + 10)), base.y},
          {base.x, static_cast<std::uint16_t>((std::max)(0, top - 10))},
      }};
      ScenarioUnitPreview larva{};
      larva.unit_id = status.next_unit_id++;
      larva.owner = base.owner;
      larva.larva_parent_id = base.unit_id;
      if (!configure_preview_type(status, larva,
                                  starcraft::lang::zerg_larva_type)) {
        continue;
      }
      bool placed{};
      for (const starcraft::lang::PathPoint &candidate : candidates) {
        larva.x = candidate.x;
        larva.y = candidate.y;
        larva.x_fixed = static_cast<std::int32_t>(larva.x) << 8U;
        larva.y_fixed = static_cast<std::int32_t>(larva.y) << 8U;
        if (settle_created_unit(status, larva, larva.x, larva.y)) {
          placed = true;
          break;
        }
      }
      if (placed) {
        larva.action_timer = 8U;
        status.units.push_back(std::move(larva));
        changed = true;
      }
    }
    return changed;
  } catch (...) {
    return false;
  }
}

} // namespace starcraft::recovery
