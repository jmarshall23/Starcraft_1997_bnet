#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_zerg.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

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
