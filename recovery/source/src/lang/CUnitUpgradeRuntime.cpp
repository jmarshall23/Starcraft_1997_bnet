#include "../platform/bootstrap_runtime.hpp"

#include <cstdint>

namespace starcraft::recovery {

bool advance_technology_research(BootstrapStatus &status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview &building : status.units) {
    if (!building.alive || !building.is_building ||
        building.technology_ticks_remaining == 0U) {
      continue;
    }
    --building.technology_ticks_remaining;
    changed = true;
    if (building.technology_ticks_remaining != 0U) {
      continue;
    }

    // CUnitUpgrade.cpp::sub_4467E0/sub_4469E0 store the active technology at
    // CUnit+200, the active upgrade at +201, and their shared remaining timer
    // at +198. Completion commits the corresponding player table entry.
    if (building.active_technology < status.researched_technologies.size()) {
      if (building.owner < status.player_researched_technologies.size()) {
        status.player_researched_technologies[building.owner]
                                               [building.active_technology] =
            true;
      }
      if (building.owner == 0U) {
        status.researched_technologies[building.active_technology] = true;
      }
      building.active_technology = 28U;
    } else if (building.active_upgrade < status.upgrade_levels.size()) {
      if (building.owner < status.player_upgrade_levels.size()) {
        ++status.player_upgrade_levels[building.owner]
                                      [building.active_upgrade];
      }
      if (building.owner == 0U) {
        ++status.upgrade_levels[building.active_upgrade];
      }
      building.active_upgrade = 46U;
    }
    building.technology_ticks_total = 0U;
    (void)restart_unit_animation(status, building, 20U);
  }
  return changed;
}

}  // namespace starcraft::recovery
