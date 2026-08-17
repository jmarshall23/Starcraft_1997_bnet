#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/flingy.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {
namespace {

bool technology_ready(const BootstrapStatus &status,
                      const ScenarioUnitPreview &caster,
                      const std::uint8_t technology) noexcept {
  return technology < status.technology_traits.size() &&
         caster.owner < status.player_researched_technologies.size() &&
         status.player_researched_technologies[caster.owner][technology];
}

bool protoss_caster_supports(const ScenarioUnitPreview &caster,
                             const std::uint8_t technology) noexcept {
  return ((caster.unit_type == 67U || caster.unit_type == 79U) &&
          (technology == 22U || technology == 23U)) ||
         (caster.unit_type == 71U &&
          (technology == 24U || technology == 25U));
}

bool spawn_point_effect(BootstrapStatus &status, const std::size_t asset_index,
                        const std::uint16_t x, const std::uint16_t y,
                        const std::uint8_t owner) noexcept {
  if (asset_index >= status.unit_assets.size()) {
    return false;
  }
  try {
    const UnitRenderAsset &asset = status.unit_assets[asset_index];
    ScenarioUnitPreview effect{};
    effect.unit_id = status.next_unit_id++;
    effect.x = x;
    effect.y = y;
    effect.x_fixed = static_cast<std::int32_t>(x) << 8U;
    effect.y_fixed = static_cast<std::int32_t>(y) << 8U;
    effect.owner = owner;
    effect.asset_index = asset_index;
    effect.iscript_state = asset.initial_iscript_state;
    effect.overlay_iscript_state = asset.initial_overlay_iscript_state;
    effect.current_sprite_frame = effect.iscript_state.frame;
    effect.current_overlay_frame = effect.overlay_iscript_state.frame;
    effect.iscript_ready = asset.iscript_ready;
    effect.overlay_ready = asset.overlay_ready;
    effect.sprite_elevation = 18U;
    status.transient_images.push_back(std::move(effect));
    return true;
  } catch (...) {
    return false;
  }
}

void finish_cast(BootstrapStatus &status, ScenarioUnitPreview &caster,
                 const std::uint8_t technology, const std::uint16_t target_x,
                 const std::uint16_t target_y) noexcept {
  const std::uint8_t order = technology_target_order(technology);
  if (order < status.order_animations.size() &&
      status.order_animations[order] < 27U) {
    (void)restart_unit_animation(status, caster,
                                 status.order_animations[order]);
  }
  caster.direction = starcraft::lang::direction_from_points(
      caster.x, caster.y, target_x, target_y);
  caster.active_order = ActiveUnitOrder::none;
  caster.order_target_id = 0U;
  caster.pending_technology = 28U;
  caster.pending_technology_x = 0U;
  caster.pending_technology_y = 0U;
  stop_unit_movement(status, caster);
}

bool create_hallucination(BootstrapStatus &status,
                          const ScenarioUnitPreview &caster,
                          const ScenarioUnitPreview &target) noexcept {
  if (target.is_building || target.in_transport || target.sprite_hidden) {
    return false;
  }
  try {
    const std::uint16_t target_type = target.unit_type;
    const std::uint16_t target_x = target.x;
    const std::uint16_t target_y = target.y;
    const std::uint8_t target_direction = target.direction;
    const std::uint8_t owner = caster.owner;
    ScenarioUnitPreview illusion{};
    illusion.unit_id = status.next_unit_id;
    illusion.owner = owner;
    if (!configure_preview_type(status, illusion, target_type)) {
      return false;
    }
    illusion.x = target_x;
    illusion.y = target_y;
    illusion.x_fixed = static_cast<std::int32_t>(target_x) << 8U;
    illusion.y_fixed = static_cast<std::int32_t>(target_y) << 8U;
    illusion.direction = target_direction;
    illusion.hallucination = true;
    // sub_43DE50 creates one CUnit and clamps CUnit+258 to 4500. It also
    // duplicates a target's attached subunits through sub_43DB80; ordinary
    // units still produce exactly one hallucination in this executable.
    illusion.hallucination_ticks = 4500U;
    if (!settle_created_unit(status, illusion, target_x, target_y)) {
      return false;
    }
    ++status.next_unit_id;
    status.units.push_back(std::move(illusion));
    if (status.hallucination_asset_index < status.unit_assets.size()) {
      (void)spawn_point_effect(status, status.hallucination_asset_index,
                               status.units.back().x, status.units.back().y,
                               owner);
    }
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace

bool cast_protoss_technology(BootstrapStatus &status,
                             ScenarioUnitPreview &caster,
                             ScenarioUnitPreview *const target,
                             const std::uint16_t target_x,
                             const std::uint16_t target_y,
                             const std::uint8_t technology) noexcept {
  if (!caster.alive || caster.stasis_ticks != 0U ||
      !protoss_caster_supports(caster, technology) ||
      !technology_ready(status, caster, technology)) {
    return false;
  }
  if (technology == 23U &&
      (target == nullptr || !target->alive || target == &caster)) {
    return false;
  }

  std::uint32_t range{};
  if (technology == 23U) {
    // sub_43DE50 passes the literal Hallucination range 192 to sub_4405C0.
    range = 192U;
  } else if (technology == 22U || technology == 25U) {
    const std::uint8_t order = technology_target_order(technology);
    const std::uint8_t weapon = status.order_weapons[order];
    if (weapon >= status.weapon_traits.size()) {
      return false;
    }
    range = status.weapon_traits[weapon].maximum_range;
  }
  const std::int64_t dx = static_cast<std::int64_t>(target_x) - caster.x;
  const std::int64_t dy = static_cast<std::int64_t>(target_y) - caster.y;
  if (range != 0U && dx * dx + dy * dy >
                         static_cast<std::int64_t>(range) * range) {
    caster.pending_technology = technology;
    caster.pending_technology_x = target_x;
    caster.pending_technology_y = target_y;
    if (target != nullptr) {
      return begin_scv_interaction(status, caster, *target,
                                   ActiveUnitOrder::cast_technology);
    }
    if (!plan_scv_path(status, caster, target_x, target_y)) {
      caster.pending_technology = 28U;
      return false;
    }
    caster.active_order = ActiveUnitOrder::cast_technology_position;
    return true;
  }

  const std::uint32_t cost =
      static_cast<std::uint32_t>(status.technology_traits[technology].energy_cost)
      << 8U;
  if (caster.energy < cost) {
    return false;
  }

  if (technology == 23U) {
    // sub_43DE50 only consumes the technology after the new CUnit has a legal
    // placement. Snapshot the IDs because insertion can grow status.units.
    const std::uint32_t caster_id = caster.unit_id;
    if (!create_hallucination(status, caster, *target)) {
      return false;
    }
    ScenarioUnitPreview *const current_caster =
        find_unit_by_id(status, caster_id);
    if (current_caster == nullptr || current_caster->energy < cost) {
      return false;
    }
    current_caster->energy =
        static_cast<std::uint16_t>(current_caster->energy - cost);
    finish_cast(status, *current_caster, technology, target_x, target_y);
    return true;
  }
  if (technology == 22U) {
    // order 137 uses weapon 56: radius 48, damage 8. The weapon script emits
    // fourteen damage events over the storm's life.
    const auto &weapon = status.weapon_traits[56U];
    try {
      status.protoss_spell_effects.push_back(
          {ProtossSpellEffectKind::psionic_storm, caster.unit_id, target_x,
           target_y, 28U, weapon.inner_splash_radius, weapon.damage,
           caster.owner, 0U, 14U});
    } catch (...) {
      return false;
    }
    caster.energy = static_cast<std::uint16_t>(caster.energy - cost);
    (void)spawn_point_effect(status, status.psionic_storm_asset_index, target_x,
                             target_y, caster.owner);
  } else if (technology == 24U) {
    // sub_43CBE0 waits 50 turns, then enumerates a radius of 64 at the source
    // point and relocates eligible allied CUnits around the Arbiter.
    try {
      status.protoss_spell_effects.push_back(
          {ProtossSpellEffectKind::recall, caster.unit_id, target_x, target_y,
           50U, 64U, 0U, caster.owner, 0U, 0U});
    } catch (...) {
      return false;
    }
    caster.energy = static_cast<std::uint16_t>(caster.energy - cost);
    (void)spawn_point_effect(status, status.recall_asset_index, target_x,
                             target_y, caster.owner);
  } else if (technology == 25U) {
    const auto &weapon = status.weapon_traits[55U];
    caster.energy = static_cast<std::uint16_t>(caster.energy - cost);
    // CBullet explosion 11 reaches sub_43D680, whose sub_450610 call uses the
    // literal 48-pixel enumeration radius.  Weapon 55's DAT splash fields are
    // zero, so using those fields here would make Stasis affect nothing.
    constexpr int stasis_radius = 48;
    for (ScenarioUnitPreview &affected : status.units) {
      if (!affected.alive || &affected == &caster || affected.is_building ||
          affected.in_transport || affected.stasis_ticks != 0U ||
          std::abs(static_cast<int>(affected.x) - target_x) >
              stasis_radius ||
          std::abs(static_cast<int>(affected.y) - target_y) >
              stasis_radius) {
        continue;
      }
      // CBullet explosion 11 calls sub_43D680/sub_43D6F0, which stores the
      // current weapon value in CUnit+262 (weapon 55 supplies 1750).
      affected.stasis_ticks =
          (std::max)(affected.stasis_ticks, weapon.damage);
      cancel_unit_order(status, affected);
    }
    (void)spawn_point_effect(status, status.stasis_field_asset_index, target_x,
                             target_y, caster.owner);
  }
  finish_cast(status, caster, technology, target_x, target_y);
  return true;
}

bool cast_unit_technology_at(BootstrapStatus &status,
                             ScenarioUnitPreview &caster,
                             const std::uint16_t target_x,
                             const std::uint16_t target_y,
                             const std::uint8_t technology) noexcept {
  return cast_protoss_technology(status, caster, nullptr, target_x, target_y,
                                 technology);
}

bool advance_protoss_spell_effects(BootstrapStatus &status) noexcept {
  bool changed{};
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive) {
      continue;
    }
    if (unit.stasis_ticks != 0U) {
      --unit.stasis_ticks;
      changed = true;
    }
    if (unit.hallucination) {
      if (unit.hallucination_ticks != 0U) {
        --unit.hallucination_ticks;
      }
      if (unit.hallucination_ticks == 0U ||
          unit.hit_points < unit.max_hit_points ||
          unit.shield_points < unit.max_shield_points ||
          unit.stasis_ticks != 0U || unit.irradiate_ticks != 0U) {
        unit.alive = false;
      }
      changed = true;
    }
  }

  for (ProtossSpellEffect &effect : status.protoss_spell_effects) {
    if (effect.ticks_remaining != 0U) {
      --effect.ticks_remaining;
    }
    if (effect.kind == ProtossSpellEffectKind::psionic_storm &&
        effect.pulses_remaining != 0U) {
      if (effect.pulse_timer != 0U) {
        --effect.pulse_timer;
      } else {
        effect.pulse_timer = 1U;
        --effect.pulses_remaining;
        for (ScenarioUnitPreview &target : status.units) {
          if (!target.alive || target.unit_id == effect.caster_id ||
              target.stasis_ticks != 0U ||
              std::abs(static_cast<int>(target.x) - effect.x) > effect.radius ||
              std::abs(static_cast<int>(target.y) - effect.y) > effect.radius) {
            continue;
          }
          apply_fixed_unit_damage(
              target, static_cast<std::uint32_t>(effect.damage) << 8U);
          if (target.hit_points == 0U) {
            destroy_unit(status, target, effect.owner);
          }
        }
      }
    } else if (effect.kind == ProtossSpellEffectKind::recall &&
               effect.ticks_remaining == 0U) {
      ScenarioUnitPreview *const caster =
          find_unit_by_id(status, effect.caster_id);
      if (caster != nullptr && caster->alive) {
        for (ScenarioUnitPreview &unit : status.units) {
          if (!unit.alive || &unit == caster || unit.owner != effect.owner ||
              unit.is_building || unit.in_transport || unit.sprite_hidden ||
              unit.stasis_ticks != 0U || unit.unit_type == 30U ||
              unit.unit_type == 35U || unit.unit_type == 36U ||
              std::abs(static_cast<int>(unit.x) - effect.x) > effect.radius ||
              std::abs(static_cast<int>(unit.y) - effect.y) > effect.radius) {
            continue;
          }
          (void)settle_created_unit(status, unit, caster->x, caster->y);
        }
      }
    }
    changed = true;
  }
  const auto finished = std::remove_if(
      status.protoss_spell_effects.begin(),
      status.protoss_spell_effects.end(), [](const ProtossSpellEffect &effect) {
        return effect.ticks_remaining == 0U &&
               (effect.kind == ProtossSpellEffectKind::recall ||
                effect.pulses_remaining == 0U);
      });
  status.protoss_spell_effects.erase(finished,
                                     status.protoss_spell_effects.end());
  return changed;
}

} // namespace starcraft::recovery
