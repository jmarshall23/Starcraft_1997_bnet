#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_protoss.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace starcraft::recovery {
namespace {

bool restart_dynamic_construction_image(
    const BootstrapStatus &status, ScenarioUnitPreview &building,
    const std::uint8_t animation) noexcept {
  if (building.dynamic_overlay_asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset =
      status.unit_assets[building.dynamic_overlay_asset_index];
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  starcraft::lang::IScriptState state{};
  if (!program.valid() || !program.start(asset.iscript_id, animation, state)) {
    return false;
  }
  const auto result = program.tick(state, 0U, 256U, &building.iscript_state,
                                   status.scenario.tileset_id());
  if ((result != starcraft::lang::IScriptTickResult::yielded &&
       result != starcraft::lang::IScriptTickResult::sleeping) ||
      state.frame >= asset.sprite_frames.size()) {
    return false;
  }
  building.dynamic_overlay_iscript_state = state;
  building.current_dynamic_overlay_frame = state.frame;
  building.dynamic_overlay_ready = true;
  building.iscript_state.image_target_flags |= state.image_target_flags;
  building.dynamic_overlay_iscript_state.image_target_flags = 0U;
  return true;
}

}  // namespace

bool complete_protoss_build_order(BootstrapStatus &status,
                                  ScenarioUnitPreview &probe) noexcept {
  if (!probe.alive || probe.active_order != ActiveUnitOrder::protoss_build ||
      probe.construction_target_type == 0xFFFFU) {
    return false;
  }
  const BuildableUnitVisual *const buildable =
      find_buildable_unit(status, probe.construction_target_type);
  const auto finish_probe_order = [&] {
    probe.construction_target_type = 0xFFFFU;
    probe.build_target_x = 0U;
    probe.build_target_y = 0U;
    cancel_unit_order(status, probe);
    (void)restart_unit_animation(status, probe, 12U);
  };
  const auto refund_probe_order = [&] {
    if (buildable == nullptr) {
      return;
    }
    std::uint32_t &minerals =
        probe.owner == 0U ? status.player_minerals
                          : status.player_mineral_stock[probe.owner];
    std::uint32_t &gas = probe.owner == 0U
                             ? status.player_gas
                             : status.player_gas_stock[probe.owner];
    minerals += buildable->simulation.mineral_cost;
    gas += buildable->simulation.gas_cost;
    if (probe.owner == 0U) {
      status.player_mineral_stock[0] = minerals;
      status.player_gas_stock[0] = gas;
    }
  };
  // A stopped path is not by itself proof that the Probe reached the build
  // site: collision/path failure can also stop a unit. sub_43BBF0 reaches its
  // creation branch only after the move order has completed at the requested
  // approach point. Never permit a remote warp when a replan stopped early.
  if (probe.x != probe.movement_final_x || probe.y != probe.movement_final_y) {
    refund_probe_order();
    finish_probe_order();
    return false;
  }
  if (buildable == nullptr ||
      !placement_is_valid(status, *buildable, probe.build_target_x,
                          probe.build_target_y, probe.owner, probe.unit_id)) {
    refund_probe_order();
    finish_probe_order();
    return false;
  }

  try {
    std::size_t geyser_index = SIZE_MAX;
    if (buildable->unit_type == 157U) {
      for (std::size_t index = 0; index < status.units.size(); ++index) {
        const ScenarioUnitPreview &unit = status.units[index];
        if (unit.alive && unit.unit_type == 188U &&
            std::abs(static_cast<int>(unit.x) - probe.build_target_x) <= 16 &&
            std::abs(static_cast<int>(unit.y) - probe.build_target_y) <= 16) {
          geyser_index = index;
          break;
        }
      }
      if (geyser_index == SIZE_MAX) {
        finish_probe_order();
        return false;
      }
    }

    ScenarioUnitPreview building{};
    building.unit_id = status.next_unit_id++;
    building.owner = probe.owner;
    if (!configure_preview_type(status, building, buildable->unit_type)) {
      finish_probe_order();
      return false;
    }
    building.x = probe.build_target_x;
    building.y = probe.build_target_y;
    building.x_fixed = static_cast<std::int32_t>(building.x) << 8U;
    building.y_fixed = static_cast<std::int32_t>(building.y) << 8U;
    building.is_building = true;
    building.construction_complete = false;
    building.construction_visible = false;
    building.construction_ticks_total = static_cast<std::uint16_t>((
        std::max)(1U,
                  static_cast<unsigned>(buildable->simulation.build_time) >>
                      1U));
    building.construction_ticks_remaining = building.construction_ticks_total;
    building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
    building.construction_animation_phase = 0U;
    if (geyser_index != SIZE_MAX) {
      building.resource_amount = status.units[geyser_index].resource_amount;
      status.units[geyser_index].alive = false;
      status.units[geyser_index].selected = false;
    }

    if (status.protoss_warp_asset_index < status.unit_assets.size()) {
      const UnitRenderAsset &warp =
          status.unit_assets[status.protoss_warp_asset_index];
      building.dynamic_overlay_asset_index = status.protoss_warp_asset_index;
      building.dynamic_overlay_iscript_state = warp.initial_iscript_state;
      building.current_dynamic_overlay_frame =
          warp.initial_iscript_state.frame;
      building.action_timer = warp.initial_iscript_state.program_counter;
      building.dynamic_overlay_above = true;
      building.dynamic_overlay_ready = warp.iscript_ready;
    }
    status.units.push_back(std::move(building));
    // gamesnd.cpp ID 245 is the exact sub_43BBF0 materialization sound.
    (void)queue_positional_game_sound(status, 245U, building.x, building.y);
    finish_probe_order();
    return true;
  } catch (...) {
    finish_probe_order();
    return false;
  }
}

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
        // sub_43BDF0 phase zero dispatches action 13 to the current sprite
        // image list. At this point sub_43BBF0 has replaced that list with
        // image 189, so keep the real building hidden and let pb1Glow's
        // script deliver the first opcode-0x27 bit-1 handoff.
        (void)restart_dynamic_construction_image(status, building, 13U);
        building.construction_animation_phase = 1U;
      }
    } else if (building.construction_animation_phase == 1U &&
               (building.iscript_state.image_target_flags & 1U) != 0U) {
      building.iscript_state.image_target_flags &=
          static_cast<std::uint8_t>(~1U);
      // Phase one replaces image 189 with the building's real primary image,
      // attaches image 188, and plays sound 246. Only this handoff makes the
      // building visible.
      building.construction_visible = true;
      building.dynamic_overlay_ready = false;
      (void)restart_unit_animation(status, building, 13U);
      if (status.protoss_materialize_asset_index <
          status.unit_assets.size()) {
        const UnitRenderAsset &materialize =
            status.unit_assets[status.protoss_materialize_asset_index];
        building.dynamic_overlay_asset_index =
            status.protoss_materialize_asset_index;
        building.dynamic_overlay_iscript_state =
            materialize.initial_iscript_state;
        building.iscript_state.image_target_flags |=
            building.dynamic_overlay_iscript_state.image_target_flags;
        building.dynamic_overlay_iscript_state.image_target_flags = 0U;
        building.current_dynamic_overlay_frame =
            materialize.initial_iscript_state.frame;
        building.dynamic_overlay_above = true;
        building.dynamic_overlay_ready = materialize.iscript_ready;
      }
      (void)queue_positional_game_sound(status, 246U, building.x, building.y);
      building.construction_animation_phase = 2U;
    } else if (building.construction_animation_phase == 2U &&
               (building.iscript_state.image_target_flags & 1U) != 0U) {
      // Image 188's own target signal removes that texture image, restores
      // the normal primary list, and starts the building's action 21.
      building.iscript_state.image_target_flags &=
          static_cast<std::uint8_t>(~1U);
      building.dynamic_overlay_ready = false;
      (void)restart_unit_animation(status, building, 21U);
      building.construction_animation_phase = 3U;
    } else if (building.construction_animation_phase == 3U &&
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
