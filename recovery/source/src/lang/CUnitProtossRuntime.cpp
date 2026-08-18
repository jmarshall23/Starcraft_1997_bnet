#include "../platform/bootstrap_runtime.hpp"

#include <cstdint>

namespace starcraft::recovery {

bool pylon_power_display_active(const BootstrapStatus &status) noexcept {
  // CUnitGUI.cpp::sub_42B340 exposes the field when a completed local Pylon
  // is selected. placebox.cpp::sub_4817F0 also exposes it for the lifetime of
  // building placement; sub_42B450/sub_42B4E0 and sub_481E30/sub_481EC0 hide
  // it again on deselection or placement teardown.
  if (status.placement_active) {
    return true;
  }
  for (const ScenarioUnitPreview &unit : status.units) {
    if (unit.alive && unit.owner == status.local_player &&
        unit.unit_type == 156U &&
        unit.construction_complete && unit.selected) {
      return true;
    }
  }
  return false;
}

void draw_pylon_power_fields_gl(const BootstrapStatus &status) {
  if (!pylon_power_display_active(status) ||
      status.pylon_power_asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset &asset =
      status.unit_assets[status.pylon_power_asset_index];
  for (const ScenarioUnitPreview &pylon : status.units) {
    if (!pylon.alive || pylon.owner != status.local_player ||
        pylon.unit_type != 156U ||
        !pylon.construction_complete) {
      continue;
    }
    // CUnitProtoss.cpp::sub_43CF60 creates sprite 198 at the Pylon's exact
    // world coordinates, forces its elevation to zero, and shows it only
    // while the global power-field state is active.
    ScenarioUnitPreview field{};
    field.x = pylon.x;
    field.y = pylon.y;
    field.owner = pylon.owner;
    field.asset_index = status.pylon_power_asset_index;
    field.sprite_elevation = 0U;
    field.iscript_state = asset.initial_iscript_state;
    field.overlay_iscript_state = asset.initial_overlay_iscript_state;
    field.current_sprite_frame = asset.initial_iscript_state.frame;
    field.current_overlay_frame = asset.initial_overlay_iscript_state.frame;
    field.iscript_ready = asset.iscript_ready;
    field.overlay_ready = asset.overlay_ready;
    draw_scenario_unit_gl(status, field);
  }
}

}  // namespace starcraft::recovery
