#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/flingy.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace starcraft::recovery {
namespace {

std::uint16_t maximum_energy_for(const BootstrapStatus &status,
                                 const ScenarioUnitPreview &unit) noexcept {
  // CUnitSpells.cpp::sub_4409B0 returns 200*256 normally and 250*256 for
  // these exact unit/upgrade pairs.
  std::uint8_t upgrade{46U};
  switch (unit.unit_type) {
  case 1U:
  case 16U: upgrade = 21U; break;
  case 8U: upgrade = 22U; break;
  case 9U: upgrade = 19U; break;
  case 12U: upgrade = 23U; break;
  case 45U: upgrade = 31U; break;
  case 46U: upgrade = 32U; break;
  case 67U:
  case 79U: upgrade = 40U; break;
  case 71U: upgrade = 44U; break;
  default: break;
  }
  const bool upgraded =
      upgrade < 46U && unit.owner < status.player_upgrade_levels.size() &&
      status.player_upgrade_levels[unit.owner][upgrade] != 0U;
  return static_cast<std::uint16_t>(upgraded ? 64000U : 51200U);
}

bool technology_is_researched(const BootstrapStatus &status,
                              const ScenarioUnitPreview &unit,
                              const std::uint8_t technology) noexcept {
  return technology < 28U &&
         unit.owner < status.player_researched_technologies.size() &&
         status.player_researched_technologies[unit.owner][technology];
}

bool caster_supports(const ScenarioUnitPreview &caster,
                     const std::uint8_t technology) noexcept {
  return (caster.unit_type == 9U &&
          (technology == 2U || technology == 6U || technology == 7U)) ||
         (caster.unit_type == 12U && technology == 8U);
}

const starcraft::data::WeaponSimulationTraits *spell_weapon(
    const BootstrapStatus &status, const std::uint8_t technology) noexcept {
  const std::uint8_t order = technology_target_order(technology);
  if (order >= status.order_weapons.size()) {
    return nullptr;
  }
  const std::uint8_t weapon = status.order_weapons[order];
  return weapon < status.weapon_traits.size() ? &status.weapon_traits[weapon]
                                               : nullptr;
}

} // namespace

void initialize_unit_energy(const BootstrapStatus &status,
                            ScenarioUnitPreview &unit) noexcept {
  // CUnitUpdate.cpp::sub_445890 guards energy updates with units.dat flag
  // 0x200000. CUnitInit gives an energy-bearing unit one quarter of max.
  if ((unit.dat_flags & 0x200000U) == 0U) {
    unit.energy = 0U;
    unit.max_energy = 0U;
    return;
  }
  unit.max_energy = maximum_energy_for(status, unit);
  unit.energy = static_cast<std::uint16_t>(unit.max_energy / 4U);
}

std::uint8_t technology_target_order(const std::uint8_t technology) noexcept {
  // order.cpp::sub_47EE60's exact targeted-technology switch.
  switch (technology) {
  case 1U: return 110U;
  case 2U: return 117U;
  case 3U: return 128U;
  case 4U: return 134U;
  case 6U: return 136U;
  case 7U: return 138U;
  case 8U: return 109U;
  case 12U: return 30U;
  case 14U: return 116U;
  case 15U: return 114U;
  case 16U: return 139U;
  case 17U: return 140U;
  case 18U: return 141U;
  case 19U: return 115U;
  case 22U: return 137U;
  case 23U: return 143U;
  case 24U: return 132U;
  case 25U: return 142U;
  default: return 0U;
  }
}

bool toggle_unit_cloak(BootstrapStatus &status, ScenarioUnitPreview &unit,
                       const bool cloak) noexcept {
  if (!unit.alive || unit.unit_type != 8U ||
      !technology_is_researched(status, unit, 9U)) {
    return false;
  }
  if (!cloak) {
    unit.cloaked = false;
    const std::uint8_t animation = status.order_animations[25U];
    if (animation < 27U) {
      (void)restart_unit_animation(status, unit, animation);
    }
    return true;
  }
  const std::uint32_t cost =
      static_cast<std::uint32_t>(status.technology_traits[9U].energy_cost)
      << 8U;
  if (unit.cloaked || unit.energy < cost) {
    return unit.cloaked;
  }
  unit.energy = static_cast<std::uint16_t>(unit.energy - cost);
  unit.cloaked = true;
  const std::uint8_t animation = status.order_animations[105U];
  if (animation < 27U) {
    (void)restart_unit_animation(status, unit, animation);
  }
  return true;
}

bool cast_unit_technology(BootstrapStatus &status, ScenarioUnitPreview &caster,
                          ScenarioUnitPreview &target,
                          const std::uint8_t technology) noexcept {
  if ((caster.unit_type == 67U || caster.unit_type == 79U ||
       caster.unit_type == 71U) &&
      technology >= 22U && technology <= 25U) {
    return cast_protoss_technology(status, caster, &target, target.x, target.y,
                                   technology);
  }
  if (!caster.alive || !target.alive || caster.owner == 11U ||
      !caster_supports(caster, technology) ||
      !technology_is_researched(status, caster, technology)) {
    return false;
  }
  // sub_43F890 rejects buildings for EMP/Irradiate. Defensive Matrix and
  // Yamato use their order weapon's ordinary target validation.
  if ((technology == 6U || technology == 7U) && target.is_building) {
    return false;
  }
  const auto *const weapon = spell_weapon(status, technology);
  if (weapon == nullptr) {
    return false;
  }
  if (unit_edge_distance(caster, target) >
      static_cast<int>(weapon->maximum_range)) {
    caster.pending_technology = technology;
    return begin_scv_interaction(status, caster, target,
                                 ActiveUnitOrder::cast_technology);
  }
  const std::uint32_t cost =
      static_cast<std::uint32_t>(status.technology_traits[technology].energy_cost)
      << 8U;
  if (caster.energy < cost) {
    return false;
  }
  caster.energy = static_cast<std::uint16_t>(caster.energy - cost);
  stop_unit_movement(status, caster);
  caster.direction = starcraft::lang::direction_from_points(
      caster.x, caster.y, target.x, target.y);
  const std::uint8_t order = technology_target_order(technology);
  const std::uint8_t animation = status.order_animations[order];
  if (animation < 27U) {
    (void)restart_unit_animation(status, caster, animation);
  }

  if (technology == 2U) {
    // CUnitTransport-adjacent spell handler sub_443BB0 writes these exact
    // CUnit+248/+250 values: 100 fixed-point HP for 2250 turns.
    target.defensive_matrix_points = 25600U;
    target.defensive_matrix_ticks = 2250U;
  } else if (technology == 6U) {
    // CBullet impact type 8 calls sub_4408B0, which clears energy and shields
    // for every CUnit in weapon field 11's bounding radius except the caster.
    const int radius = weapon->inner_splash_radius;
    for (ScenarioUnitPreview &affected : status.units) {
      if (!affected.alive || &affected == &caster ||
          std::abs(static_cast<int>(affected.x) - target.x) > radius ||
          std::abs(static_cast<int>(affected.y) - target.y) > radius) {
        continue;
      }
      affected.energy = 0U;
      affected.shield_points = 0U;
    }
  } else if (technology == 7U) {
    // sub_444170 writes 1125 to CUnit+260 and the caster at +264.
    target.irradiate_ticks = 1125U;
    target.irradiate_owner = caster.owner;
  } else if (technology == 8U) {
    apply_fixed_unit_damage(
        target, static_cast<std::uint32_t>(weapon->damage) << 8U);
    if (target.hit_points == 0U) {
      destroy_unit(status, target, caster.owner);
    }
  }
  caster.active_order = ActiveUnitOrder::none;
  caster.order_target_id = 0U;
  caster.pending_technology = 28U;
  return true;
}

bool advance_unit_energy(BootstrapStatus &status) noexcept {
  bool changed{};
  const auto &irradiate_weapon = status.weapon_traits[20U];
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive) {
      continue;
    }
    if (unit.max_energy != 0U) {
      unit.max_energy = maximum_energy_for(status, unit);
      if (unit.cloaked) {
        // sub_440860 returns 13 fixed-point energy per update for a Wraith.
        if (unit.energy < 13U) {
          (void)toggle_unit_cloak(status, unit, false);
        } else {
          unit.energy = static_cast<std::uint16_t>(unit.energy - 13U);
        }
      } else if (unit.energy < unit.max_energy) {
        // sub_445890 regenerates exactly eight fixed-point units per update.
        unit.energy = static_cast<std::uint16_t>((std::min)(
            static_cast<unsigned>(unit.max_energy),
            static_cast<unsigned>(unit.energy) + 8U));
      }
      changed = true;
    }
    if (unit.defensive_matrix_ticks != 0U) {
      --unit.defensive_matrix_ticks;
      if (unit.defensive_matrix_ticks == 0U) {
        unit.defensive_matrix_points = 0U;
      }
      changed = true;
    }
    if (unit.irradiate_ticks == 0U) {
      continue;
    }
    --unit.irradiate_ticks;
    changed = true;
    if ((unit.irradiate_ticks & 7U) != 0U) {
      continue;
    }
    const std::uint32_t pulse =
        (static_cast<std::uint32_t>(irradiate_weapon.damage) << 8U) / 8U;
    for (ScenarioUnitPreview &affected : status.units) {
      // CUnitTSpells.cpp::sub_444400 tests units.dat flag 0x10000, rejects
      // buildings/eggs, and applies the weapon-20 pulse inside range 0x20.
      if (!affected.alive || affected.is_building ||
          affected.unit_type == 35U || affected.unit_type == 36U ||
          (affected.dat_flags & 0x10000U) == 0U ||
          unit_edge_distance(unit, affected) > 0x20) {
        continue;
      }
      apply_fixed_unit_damage(affected, pulse);
      if (affected.hit_points == 0U) {
        destroy_unit(status, affected, unit.irradiate_owner);
      }
    }
    if (unit.irradiate_ticks == 0U) {
      unit.irradiate_owner = 0xFFU;
    }
  }
  return changed;
}

} // namespace starcraft::recovery
