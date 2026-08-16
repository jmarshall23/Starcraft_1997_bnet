#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_zerg.hpp"

#include <algorithm>
#include <cstdint>

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
        (void)restart_unit_animation(status, building, 15U);
        // sub_447D80 receives bit 4 from the temporary Zerg construction
        // image targeted by this action. That CImage is not materialized in
        // the recovered single-image renderer, so retain its opcode-0x27
        // handoff explicitly for the following state.
        building.iscript_state.image_target_flags |= 4U;
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
