#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_init.hpp"

#include <cstdint>

namespace starcraft::recovery {

bool replace_preview_primary_image(BootstrapStatus &status,
                                   ScenarioUnitPreview &unit,
                                   const std::size_t asset_index) noexcept {
  if (asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[asset_index];
  unit.asset_index = asset_index;
  unit.iscript_state = asset.initial_iscript_state;
  unit.overlay_iscript_state = asset.initial_overlay_iscript_state;
  unit.current_sprite_frame = asset.initial_iscript_state.frame;
  unit.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  unit.iscript_ready = asset.iscript_ready;
  unit.overlay_ready = asset.overlay_ready;
  unit.dynamic_overlay_asset_index = SIZE_MAX;
  unit.dynamic_overlay_iscript_state = {};
  unit.current_dynamic_overlay_frame = 0U;
  unit.dynamic_overlay_x_offset = 0;
  unit.dynamic_overlay_y_offset = 0;
  unit.dynamic_overlay_above = false;
  unit.dynamic_overlay_ready = false;
  unit.damage_overlay_stage = 0xFFU;
  return true;
}

bool configure_preview_type(BootstrapStatus &status, ScenarioUnitPreview &unit,
                            const std::uint16_t unit_type) noexcept {
  if (unit_type >= status.runtime_unit_types.size()) {
    return false;
  }
  const RuntimeUnitType &runtime = status.runtime_unit_types[unit_type];
  if (!runtime.ready || runtime.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const starcraft::lang::UnitInitializationData &initialization =
      runtime.initialization;
  const UnitRenderAsset &asset = status.unit_assets[runtime.asset_index];
  unit.unit_type = unit_type;
  unit.asset_index = runtime.asset_index;
  unit.selection_circle_asset_index = runtime.selection_circle_asset_index;
  unit.selection_circle_y_offset = initialization.selection_circle_y_offset;
  unit.selection_width = initialization.placement_width;
  unit.selection_height = initialization.placement_height;
  unit.collision_left =
      static_cast<std::uint16_t>(initialization.collision.left);
  unit.collision_top = static_cast<std::uint16_t>(initialization.collision.top);
  unit.collision_right =
      static_cast<std::uint16_t>(initialization.collision.right);
  unit.collision_bottom =
      static_cast<std::uint16_t>(initialization.collision.bottom);
  unit.iscript_state = asset.initial_iscript_state;
  unit.overlay_iscript_state = asset.initial_overlay_iscript_state;
  unit.current_sprite_frame = asset.initial_iscript_state.frame;
  unit.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
  unit.iscript_ready = asset.iscript_ready;
  unit.overlay_ready = asset.overlay_ready;
  unit.is_building = initialization.is_building;
  unit.construction_complete = true;
  unit.attack_fire_pending = false;
  unit.attack_fire_timeout = 0U;
  unit.damage_overlay_stage = 0xFFU;
  apply_initialization_traits(unit, initialization);
  initialize_unit_energy(status, unit);
  return true;
}

void apply_simulation_traits(
    ScenarioUnitPreview &unit,
    const starcraft::data::UnitSimulationTraits &traits) noexcept {
  unit.max_hit_points = traits.max_hit_points;
  unit.hit_points = traits.max_hit_points;
  unit.max_shield_points = traits.max_shield_points;
  unit.shield_points = traits.max_shield_points;
  unit.dat_flags = traits.dat_flags;
  unit.armor = traits.armor;
  unit.armor_class = traits.armor_class;
  unit.armor_upgrade = traits.armor_upgrade;
  unit.weapon_range = traits.ground_weapon_range;
  unit.ground_weapon = traits.ground_weapon;
  unit.weapon_damage = traits.ground_weapon_damage;
  unit.weapon_damage_factor = traits.ground_weapon_damage_factor;
  unit.weapon_damage_class = traits.ground_weapon_damage_class;
  unit.weapon_cooldown = traits.ground_weapon_cooldown;
  unit.weapon_upgrade = traits.ground_weapon_upgrade;
  unit.air_weapon_range = traits.air_weapon_range;
  unit.air_weapon = traits.air_weapon;
  unit.air_weapon_damage = traits.air_weapon_damage;
  unit.air_weapon_damage_factor = traits.air_weapon_damage_factor;
  unit.air_weapon_damage_class = traits.air_weapon_damage_class;
  unit.air_weapon_cooldown = traits.air_weapon_cooldown;
  unit.air_weapon_upgrade = traits.air_weapon_upgrade;
  unit.seek_range = traits.seek_range;
  unit.sight_range = traits.sight_range;
  unit.cargo_space_required = traits.cargo_space_required;
  unit.cargo_space_provided = traits.cargo_space_provided;
  unit.has_ground_weapon = traits.has_ground_weapon;
  unit.has_air_weapon = traits.has_air_weapon;
  unit.mineral_cost = traits.mineral_cost;
  unit.gas_cost = traits.gas_cost;
  const std::uint32_t build_ticks =
      (std::max)(1U, static_cast<std::uint32_t>(traits.build_time) >> 1U);
  unit.repair_step = static_cast<std::uint16_t>(
      (std::min)(static_cast<std::uint32_t>(UINT16_MAX),
                 (traits.max_hit_points + build_ticks - 1U) / build_ticks));
  unit.resource_amount =
      starcraft::lang::initial_resource_amount(unit.unit_type);
  // CUnitInit.cpp::sub_42E400 installs the permanent-cloak state while it
  // constructs an Observer. Temporary Ghost/Wraith cloak reaches the same
  // CUnitColor draw state later through the cloak orders.
  unit.permanently_cloaked = unit.unit_type == 84U;
  unit.cloaked = unit.permanently_cloaked;
}

void apply_initialization_traits(
    ScenarioUnitPreview &unit,
    const starcraft::lang::UnitInitializationData &initialization) noexcept {
  apply_simulation_traits(unit, initialization.simulation);
  unit.sprite_elevation = initialization.sprite_elevation;
  unit.movement_top_speed =
      initialization.has_movement ? initialization.movement.top_speed : 0U;
  unit.movement_acceleration =
      initialization.has_movement ? initialization.movement.acceleration : 0U;
  unit.movement_turn_speed =
      initialization.has_movement ? initialization.movement.turn_speed : 0U;
  unit.movement_control = initialization.has_movement
                              ? initialization.movement.movement_control
                              : 0U;
}

} // namespace starcraft::recovery
