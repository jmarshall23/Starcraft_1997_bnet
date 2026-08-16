#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_protoss.hpp"

#include <algorithm>
#include <cstdint>

namespace starcraft::recovery {

bool advance_protoss_building_construction(BootstrapStatus &status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview &building : status.units) {
    if (!building.alive || !building.is_building ||
        building.construction_complete ||
        !starcraft::lang::is_protoss_buildable_unit_type(building.unit_type) ||
        building.construction_ticks_total == 0U) {
      continue;
    }

    // CUnitPBuild.cpp::sub_43BDF0 advances life and shields from the building
    // itself; the Probe is not retained as its builder. At zero remaining
    // time it dispatches action 13, follows the image-target bit-1 handoff,
    // then dispatches action 21 before finalizing the CUnit.
    if (building.construction_animation_phase == 0U) {
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
      if (building.construction_ticks_remaining == 0U) {
        (void)restart_unit_animation(status, building, 13U);
        // The retail CUnit has the temporary Protoss construction image in
        // its sprite image list; that image's opcode 0x27 supplies this first
        // bit-1 handoff. The recovered renderer does not materialize that
        // transient image as a CImage, so preserve its target event here.
        building.iscript_state.image_target_flags |= 1U;
        building.construction_animation_phase = 1U;
      }
    } else if (building.construction_animation_phase == 1U &&
               (building.iscript_state.image_target_flags & 1U) != 0U) {
      building.iscript_state.image_target_flags &=
          static_cast<std::uint8_t>(~1U);
      // The retail state inserts image 246 between these two signals. The
      // recovered renderer has one dynamic-overlay slot, so action 21 on the
      // primary image preserves the same second signal/finalization boundary.
      (void)restart_unit_animation(status, building, 21U);
      building.iscript_state.image_target_flags |= 1U;
      building.construction_animation_phase = 2U;
    } else if (building.construction_animation_phase == 2U &&
               (building.iscript_state.image_target_flags & 1U) != 0U) {
      building.iscript_state.image_target_flags &=
          static_cast<std::uint8_t>(~1U);
      building.hit_points = building.max_hit_points;
      building.construction_complete = true;
      (void)restart_unit_animation(status, building, 16U);
    }
    changed = true;
  }
  return changed;
}

}  // namespace starcraft::recovery
