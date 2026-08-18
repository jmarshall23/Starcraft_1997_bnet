#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/flingy.hpp"
#include "starcraft/lang/iscript.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool spawn_weapon_projectile(BootstrapStatus &status,
                             const ScenarioUnitPreview &source,
                             const ScenarioUnitPreview &target,
                             const std::uint8_t weapon_type,
                             const std::uint32_t damage) noexcept {
  if (weapon_type >= status.weapon_traits.size() ||
      weapon_type >= status.weapon_asset_indices.size()) {
    return false;
  }
  const starcraft::data::WeaponSimulationTraits &weapon =
      status.weapon_traits[weapon_type];
  const std::size_t asset_index = status.weapon_asset_indices[weapon_type];
  if (!weapon.has_projectile_graphic ||
      asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[asset_index];
  ScenarioUnitPreview bullet{};
  bullet.unit_id = status.next_unit_id++;
  bullet.owner = source.owner;
  bullet.asset_index = asset_index;
  bullet.direction = source.direction;
  bullet.sprite_elevation = static_cast<std::uint8_t>((std::min)(
      255U, static_cast<unsigned>(target.sprite_elevation) + 1U));
  // CUnitCombat.cpp::sub_426360 rotates weapons.dat's forward offset by the
  // launch direction and subtracts its vertical offset before calling
  // CBullet::sub_4024F0.
  constexpr double pi = 3.14159265358979323846;
  const double angle = static_cast<double>(source.direction) *
                       (2.0 * pi / 256.0);
  const int launch_x = static_cast<int>(source.x) +
                       static_cast<int>(std::lround(
                           std::sin(angle) * weapon.forward_offset));
  const int launch_y = static_cast<int>(source.y) -
                       static_cast<int>(std::lround(
                           std::cos(angle) * weapon.forward_offset)) -
                       weapon.vertical_offset;
  bullet.x = static_cast<std::uint16_t>((std::clamp)(launch_x, 0, 65535));
  bullet.y = static_cast<std::uint16_t>((std::clamp)(launch_y, 0, 65535));
  bullet.x_fixed = static_cast<std::int32_t>(bullet.x) << 8U;
  bullet.y_fixed = static_cast<std::int32_t>(bullet.y) << 8U;
  bullet.movement_speed = weapon.projectile_top_speed;
  bullet.movement_top_speed = weapon.projectile_top_speed;
  bullet.projectile_source_id = source.unit_id;
  bullet.projectile_target_id = target.unit_id;
  bullet.projectile_weapon = weapon_type;
  bullet.projectile_damage = damage;
  bullet.action_timer = weapon.removal_timer == 0U ? 255U
                                                   : weapon.removal_timer;
  bullet.is_projectile = true;
  // The image weapon opcode appends the CBullet after the current global
  // object walk has passed its insertion point. It becomes eligible for
  // CFlingy::sub_406290 on the following simulation turn.
  bullet.projectile_birth_pending = true;
  bullet.iscript_state = asset.initial_iscript_state;
  bullet.overlay_iscript_state = asset.initial_overlay_iscript_state;
  bullet.current_sprite_frame = asset.initial_iscript_state.frame;
  bullet.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  bullet.iscript_ready = asset.iscript_ready;
  bullet.overlay_ready = asset.overlay_ready;
  try {
    status.transient_images.push_back(std::move(bullet));
    return true;
  } catch (...) {
    return false;
  }
}

bool spawn_worker_mining_effect(BootstrapStatus &status,
                                const ScenarioUnitPreview &worker,
                                const std::uint8_t weapon_type) noexcept {
  // CUnitCombat.cpp::sub_426650 handles IScript opcode 0x2B by passing the
  // weapon, CUnit order-target coordinates, owner, direction, and source
  // CUnit to CBullet.cpp::sub_4024F0. The worker Working scripts use weapon 8
  // (SCV cutter) and weapon 42 (Probe mineral beam).
  const std::size_t asset_index =
      weapon_type == 8U    ? status.scv_mining_effect_asset_index
      : weapon_type == 42U ? status.probe_mining_effect_asset_index
                           : SIZE_MAX;
  const ScenarioUnitPreview *const target =
      find_unit_by_id(status, worker.order_target_id);
  if (target == nullptr || asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[asset_index];
  ScenarioUnitPreview effect{};
  effect.unit_id = status.next_unit_id++;
  effect.x = target->x;
  effect.y = target->y;
  effect.owner = worker.owner;
  effect.direction = worker.direction;
  effect.sprite_elevation = static_cast<std::uint8_t>(
      (std::min)(255U, static_cast<unsigned>(target->sprite_elevation) + 1U));
  effect.asset_index = asset_index;
  effect.iscript_state = asset.initial_iscript_state;
  effect.overlay_iscript_state = asset.initial_overlay_iscript_state;
  effect.current_sprite_frame = asset.initial_iscript_state.frame;
  effect.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  effect.iscript_ready = asset.iscript_ready;
  effect.overlay_ready = asset.overlay_ready;
  try {
    status.transient_images.push_back(std::move(effect));
    return true;
  } catch (...) {
    return false;
  }
}

bool advance_transient_images(BootstrapStatus &status,
                              const std::uint32_t clock) noexcept {
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  bool changed{};
  std::vector<ScenarioUnitPreview> impacts;
  std::vector<std::pair<std::uint32_t, std::uint8_t>> destroyed_targets;
  try {
    impacts.reserve(8U);
    destroyed_targets.reserve(8U);
  } catch (...) {
  }
  // CBullet.cpp::sub_403720 advances the CFlingy first. Reaching its tracked
  // target (or exhausting weapons.dat's removal timer) dispatches action 1
  // on the bullet sprite and applies the weapon's impact behavior.
  const std::size_t projectile_pass_count = status.transient_images.size();
  for (std::size_t index = 0U; index < projectile_pass_count; ++index) {
    ScenarioUnitPreview &bullet = status.transient_images[index];
    if (!bullet.alive || !bullet.is_projectile ||
        bullet.projectile_weapon >= status.weapon_traits.size()) {
      continue;
    }
    if (bullet.projectile_birth_pending) {
      bullet.projectile_birth_pending = false;
      changed = true;
      continue;
    }
    ScenarioUnitPreview *const target =
        find_unit_by_id(status, bullet.projectile_target_id);
    if (target == nullptr) {
      bullet.alive = false;
      changed = true;
      continue;
    }
    const int dx = static_cast<int>(target->x) - bullet.x;
    const int dy = static_cast<int>(target->y) - bullet.y;
    const double distance = std::sqrt(static_cast<double>(dx) * dx +
                                      static_cast<double>(dy) * dy);
    const double step = (std::max)(1.0, bullet.movement_speed / 256.0);
    if (bullet.action_timer != 0U) {
      --bullet.action_timer;
    }
    if (distance > step + 1.0 && bullet.action_timer != 0U) {
      bullet.direction = starcraft::lang::direction_from_points(
          bullet.x, bullet.y, target->x, target->y);
      bullet.x_fixed += static_cast<std::int32_t>(
          std::lround(static_cast<double>(dx) / distance * step * 256.0));
      bullet.y_fixed += static_cast<std::int32_t>(
          std::lround(static_cast<double>(dy) / distance * step * 256.0));
      bullet.x = static_cast<std::uint16_t>((std::clamp)(
          bullet.x_fixed >> 8U, 0, static_cast<int>(UINT16_MAX)));
      bullet.y = static_cast<std::uint16_t>((std::clamp)(
          bullet.y_fixed >> 8U, 0, static_cast<int>(UINT16_MAX)));
      changed = true;
      continue;
    }

    bullet.x = target->x;
    bullet.y = target->y;
    bullet.alive = false;
    if (bullet.projectile_damage != 0U) {
      apply_weapon_unit_damage(status, *target, bullet.projectile_damage,
                               bullet.projectile_weapon,
                               bullet.projectile_source_id, bullet.owner);
      if (target->hit_points == 0U) {
        try {
          destroyed_targets.emplace_back(target->unit_id, bullet.owner);
        } catch (...) {
        }
      }
    }
    ScenarioUnitPreview impact = bullet;
    impact.unit_id = status.next_unit_id++;
    impact.alive = true;
    impact.is_projectile = false;
    impact.projectile_damage = 0U;
    impact.projectile_target_id = 0U;
    impact.construction_visible = true;
    const UnitRenderAsset &asset = status.unit_assets[impact.asset_index];
    starcraft::lang::IScriptState impact_state{};
    if (program.start(asset.iscript_id, 1U, impact_state)) {
      const auto result = program.tick(impact_state, clock, 256U, nullptr,
                                       status.scenario.tileset_id());
      if ((result == starcraft::lang::IScriptTickResult::yielded ||
           result == starcraft::lang::IScriptTickResult::sleeping) &&
          impact_state.frame < asset.sprite_frames.size()) {
        impact.iscript_state = impact_state;
        impact.current_sprite_frame = impact_state.frame;
        impact.iscript_ready = true;
        impact.overlay_ready = false;
        try {
          impacts.push_back(std::move(impact));
        } catch (...) {
        }
      }
    }
    changed = true;
  }
  for (std::size_t index = 0; index < status.transient_images.size(); ++index) {
    ScenarioUnitPreview &effect = status.transient_images[index];
    if (!effect.alive || effect.asset_index >= status.unit_assets.size()) {
      effect.alive = false;
      continue;
    }
    if (!effect.is_projectile && effect.action_timer == UINT16_MAX) {
      effect.action_timer = 0U;
      changed = true;
      continue;
    }
    const UnitRenderAsset &asset = status.unit_assets[effect.asset_index];
    bool spawn_sprite_event{};
    if (effect.iscript_ready) {
      const std::uint32_t prior_sound = effect.iscript_state.sound_event_count;
      const std::uint32_t prior_overlay =
          effect.iscript_state.overlay_event_count;
      const std::uint32_t prior_sprite = effect.iscript_state.sprite_event_count;
      const auto result = program.tick(
          effect.iscript_state,
          clock ^ static_cast<std::uint32_t>(index * 0x9E3779B9U) ^
              effect.unit_id,
          256U, nullptr, status.scenario.tileset_id());
      if (effect.iscript_state.sound_event_count != prior_sound) {
        (void)queue_positional_game_sound(status,
                                          effect.iscript_state.sound_event,
                                          effect.x, effect.y);
      }
      if (effect.iscript_state.overlay_event_count != prior_overlay) {
        (void)materialize_unit_overlay(status, effect, effect.iscript_state);
      }
      if (effect.iscript_state.sprite_event_count != prior_sprite) {
        spawn_sprite_event = true;
      }
      if (result == starcraft::lang::IScriptTickResult::ended ||
          result == starcraft::lang::IScriptTickResult::malformed_program ||
          result == starcraft::lang::IScriptTickResult::unsupported_opcode ||
          result == starcraft::lang::IScriptTickResult::instruction_limit ||
          effect.iscript_state.frame >= asset.sprite_frames.size()) {
        effect.iscript_ready = false;
        // CImage may end while its owning CBullet/CFlingy is still in flight
        // (Hydralisk spines do this). sub_403720 owns bullet lifetime, so an
        // ended image must not delete the projectile object before impact.
        // Non-projectile detached death sprites retain the older behavior.
        if (!effect.is_projectile) {
          effect.construction_visible = false;
        }
      } else {
        effect.current_sprite_frame = effect.iscript_state.frame;
      }
    }
    if (effect.overlay_ready && !asset.overlay_frames.empty()) {
      const auto overlay_result = program.tick(
          effect.overlay_iscript_state,
          clock ^ static_cast<std::uint32_t>(index * 0x85EBCA6BU) ^
              effect.unit_id,
          256U, &effect.iscript_state, status.scenario.tileset_id());
      if (overlay_result == starcraft::lang::IScriptTickResult::ended ||
          overlay_result ==
              starcraft::lang::IScriptTickResult::malformed_program ||
          overlay_result ==
              starcraft::lang::IScriptTickResult::unsupported_opcode ||
          overlay_result ==
              starcraft::lang::IScriptTickResult::instruction_limit ||
          effect.overlay_iscript_state.frame >= asset.overlay_frames.size()) {
        effect.overlay_ready = false;
      } else {
        effect.current_overlay_frame = effect.overlay_iscript_state.frame;
      }
    }
    if (effect.dynamic_overlay_ready &&
        effect.dynamic_overlay_asset_index < status.unit_assets.size()) {
      const UnitRenderAsset &dynamic_asset =
          status.unit_assets[effect.dynamic_overlay_asset_index];
      const std::uint32_t prior_dynamic_sound =
          effect.dynamic_overlay_iscript_state.sound_event_count;
      const auto dynamic_result = program.tick(
          effect.dynamic_overlay_iscript_state,
          clock ^ static_cast<std::uint32_t>(index * 0xC2B2AE35U) ^
              effect.unit_id,
          256U, &effect.iscript_state, status.scenario.tileset_id());
      if (effect.dynamic_overlay_iscript_state.sound_event_count !=
          prior_dynamic_sound) {
        (void)queue_positional_game_sound(
            status, effect.dynamic_overlay_iscript_state.sound_event,
            effect.x, effect.y);
      }
      if (dynamic_result == starcraft::lang::IScriptTickResult::ended ||
          dynamic_result ==
              starcraft::lang::IScriptTickResult::malformed_program ||
          dynamic_result ==
              starcraft::lang::IScriptTickResult::unsupported_opcode ||
          dynamic_result ==
              starcraft::lang::IScriptTickResult::instruction_limit ||
          effect.dynamic_overlay_iscript_state.frame >=
              dynamic_asset.sprite_frames.size()) {
        effect.dynamic_overlay_ready = false;
      } else {
        effect.current_dynamic_overlay_frame =
            effect.dynamic_overlay_iscript_state.frame;
      }
    }
    if (!effect.is_projectile && !effect.iscript_ready && !effect.overlay_ready &&
        !effect.dynamic_overlay_ready) {
      effect.alive = false;
    }
    changed = true;
    if (spawn_sprite_event) {
      // This appends to transient_images and may reallocate it. It must be the
      // final operation using `effect` in this iteration.
      (void)spawn_iscript_sprite_effect(status, effect, effect.iscript_state);
    }
  }
  const auto dead = std::remove_if(
      status.transient_images.begin(), status.transient_images.end(),
      [](const ScenarioUnitPreview &effect) { return !effect.alive; });
  if (dead != status.transient_images.end()) {
    status.transient_images.erase(dead, status.transient_images.end());
    changed = true;
  }
  for (ScenarioUnitPreview &impact : impacts) {
    try {
      status.transient_images.push_back(std::move(impact));
    } catch (...) {
    }
  }
  std::sort(destroyed_targets.begin(), destroyed_targets.end());
  destroyed_targets.erase(
      std::unique(destroyed_targets.begin(), destroyed_targets.end()),
      destroyed_targets.end());
  for (const auto [target_id, attacking_owner] : destroyed_targets) {
    ScenarioUnitPreview *const target = find_unit_by_id(status, target_id);
    if (target != nullptr && target->hit_points == 0U) {
      destroy_unit(status, *target, attacking_owner);
    }
  }
  return changed;
}

} // namespace starcraft::recovery
