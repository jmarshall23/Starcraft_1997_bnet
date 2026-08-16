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
