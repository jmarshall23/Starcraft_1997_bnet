#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

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
      if (!product.ready || total_ticks == 0U ||
          now - source.production_started < duration) {
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
