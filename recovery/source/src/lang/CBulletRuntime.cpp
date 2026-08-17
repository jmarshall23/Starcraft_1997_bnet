#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/iscript.hpp"

#include <algorithm>
#include <cstdint>

namespace starcraft::recovery {

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
  for (std::size_t index = 0; index < status.transient_images.size(); ++index) {
    ScenarioUnitPreview &effect = status.transient_images[index];
    if (!effect.alive || !effect.iscript_ready ||
        effect.asset_index >= status.unit_assets.size()) {
      effect.alive = false;
      continue;
    }
    const UnitRenderAsset &asset = status.unit_assets[effect.asset_index];
    const std::uint32_t prior_sound = effect.iscript_state.sound_event_count;
    const auto result = program.tick(
        effect.iscript_state,
        clock ^ static_cast<std::uint32_t>(index * 0x9E3779B9U) ^
            effect.unit_id,
        256U, nullptr, status.scenario.tileset_id());
    if (effect.iscript_state.sound_event_count != prior_sound) {
      status.pending_game_sound = effect.iscript_state.sound_event;
    }
    if (result == starcraft::lang::IScriptTickResult::ended ||
        result == starcraft::lang::IScriptTickResult::malformed_program ||
        result == starcraft::lang::IScriptTickResult::unsupported_opcode ||
        result == starcraft::lang::IScriptTickResult::instruction_limit ||
        effect.iscript_state.frame >= asset.sprite_frames.size()) {
      effect.alive = false;
    } else {
      effect.current_sprite_frame = effect.iscript_state.frame;
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
    changed = true;
  }
  const auto dead = std::remove_if(
      status.transient_images.begin(), status.transient_images.end(),
      [](const ScenarioUnitPreview &effect) { return !effect.alive; });
  if (dead != status.transient_images.end()) {
    status.transient_images.erase(dead, status.transient_images.end());
    changed = true;
  }
  return changed;
}

} // namespace starcraft::recovery
