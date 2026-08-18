#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace starcraft::recovery {

using CommandAction = CommandButtonVisual::Action;

void activate_command_button(BootstrapStatus &status,
                             const std::uint16_t position) noexcept {
  const std::uint8_t command_owner = status.command_player;
  const CommandCardView card = command_card_for(status);
  for (std::size_t button_index = 0; button_index < card.count;
       ++button_index) {
    const CommandButtonVisual &button = card.buttons[button_index];
    if (button.position == position) {
      status.last_command_position = position;
      const ScenarioUnitPreview *const selected = first_selected_unit(status);
      if (selected != nullptr &&
          (button.action == CommandAction::train_unit ||
           button.action == CommandAction::begin_building_placement ||
           button.action == CommandAction::build_addon ||
           button.action == CommandAction::morph_building)) {
        const UnitRequirementResult requirements =
            unit_requirements_for(status, *selected, button.argument);
        if (!requirements.visible || !requirements.allowed) {
          return;
        }
      }
      if (button.action == CommandAction::train_unit) {
        ScenarioUnitPreview *producer{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.construction_complete &&
              starcraft::lang::producer_builds_unit(unit.unit_type,
                                                    button.argument)) {
            producer = &unit;
            break;
          }
        }
        if (producer == nullptr ||
            button.argument >= status.runtime_unit_types.size()) {
          return;
        }
        if (producer->unit_type == 72U || producer->unit_type == 82U ||
            producer->unit_type == 83U) {
          const std::size_t hangar_count = static_cast<std::size_t>(
              std::count_if(producer->hangar_unit_ids.begin(),
                            producer->hangar_unit_ids.end(),
                            [](const std::uint32_t id) { return id != 0U; }));
          const bool expanded =
              producer->owner < status.player_upgrade_levels.size() &&
              ((producer->unit_type == 83U &&
                status.player_upgrade_levels[producer->owner][36U] != 0U) ||
               (producer->unit_type != 83U &&
                status.player_upgrade_levels[producer->owner][43U] != 0U));
          if (hangar_count + producer->production_queue.count() >=
              (expanded ? 10U : 5U)) {
            return;
          }
        }
        const RuntimeUnitType &product =
            status.runtime_unit_types[button.argument];
        const starcraft::data::UnitSimulationTraits &simulation =
            product.initialization.simulation;
        if (!product.ready || producer->production_queue.full() ||
            !resource_cost_available(status, simulation.mineral_cost,
                                     simulation.gas_cost)) {
          return;
        }
        const bool starting_queue = producer->production_queue.empty();
        if (!producer->production_queue.enqueue(button.argument)) {
          return;
        }
        if (starting_queue) {
          producer->production_active = true;
          producer->production_started =
              status.synchronized_command_execution
                  ? status.command_execution_clock
                  : GetTickCount();
          producer->production_kind =
              producer->unit_type == starcraft::lang::zerg_larva_type
                  ? starcraft::lang::UnitProductionKind::zerg_larva_morph
              : producer->unit_type == 72U || producer->unit_type == 82U ||
                        producer->unit_type == 83U
                  ? starcraft::lang::UnitProductionKind::carrier_hangar
                  : starcraft::lang::UnitProductionKind::train;
          if (producer->production_kind ==
              starcraft::lang::UnitProductionKind::zerg_larva_morph) {
            // CUnitZBuild.cpp::sub_447820 changes larva 35 into egg 36 while
            // retaining the queued target at CUnit+0x98.
            (void)configure_preview_type(status, *producer,
                                         starcraft::lang::zerg_egg_type);
            (void)displace_units_for_zerg_egg(status, *producer);
          } else {
            // CUnitBuild.cpp::sub_423020 dispatches Working (19) when the
            // first queue entry materializes.
            (void)restart_unit_animation(status, *producer, 19U);
          }
        }
        spend_player_resources(status, command_owner,
                               simulation.mineral_cost,
                               simulation.gas_cost);
      } else if (button.action == CommandAction::begin_move_target) {
        // statbtn.cpp action 0x0047EA20 calls target.cpp::sub_4B0120 with
        // unit order 0x32 and empty-terrain order 7.
        begin_command_target(status, 0x32, 7);
      } else if (button.action == CommandAction::stop) {
        // statbtn.cpp action 0x0047EA50 forwards command byte 30 through
        // sub_474790. Apply it locally while network dispatch is paused.
        status.last_command_opcode = 30;
        cancel_command_target(status);
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              !unit.is_building) {
            cancel_unit_order(status, unit);
          }
        }
      } else if (button.action == CommandAction::begin_attack_target) {
        // Action 0x0047EA80 supplies unit order 9 and terrain order 15.
        begin_command_target(status, 9, 15);
      } else if (button.action == CommandAction::begin_repair_target) {
        // Action 0x0047F660 supplies unit order 0x24 and terrain order 7.
        begin_command_target(status, 0x24, 7);
      } else if (button.action == CommandAction::begin_gather_target) {
        // Action 0x0047F700 supplies unit order 0x4E and terrain order 7.
        begin_command_target(status, 0x4E, 7);
      } else if (button.action == CommandAction::return_cargo) {
        // Action 0x0047F730 forwards command byte 34 through sub_474D40.
        cancel_command_target(status);
        (void)issue_scv_return_cargo(status);
      } else if (button.action == CommandAction::cancel_target) {
        cancel_command_target(status);
        status.active_command_card = 0;
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
        status.nydus_parent_id = 0U;
      } else if (button.action == CommandAction::cancel_construction) {
        cancel_command_target(status);
        status.active_command_card = 0U;
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.is_building && !unit.construction_complete) {
            (void)cancel_building_construction(status, unit);
            break;
          }
        }
      } else if (button.action == CommandAction::open_card) {
        cancel_command_target(status);
        status.active_command_card = button.argument;
      } else if (button.action == CommandAction::begin_building_placement) {
        const BuildableUnitVisual *const buildable =
            find_buildable_unit(status, button.argument);
        if (buildable != nullptr &&
            buildable->asset_index < status.unit_assets.size() &&
            resource_cost_available(status, buildable->simulation.mineral_cost,
                                    buildable->simulation.gas_cost)) {
          cancel_command_target(status);
          status.placement_unit_type = button.argument;
          status.nydus_parent_id = 0U;
          status.placement_active = true;
          status.placement_valid = false;
        }
      } else if (button.action == CommandAction::build_addon) {
        const BuildableUnitVisual *const addon =
            find_buildable_unit(status, button.argument);
        const ScenarioUnitPreview *const parent = first_selected_unit(status);
        std::uint16_t center_x{};
        std::uint16_t center_y{};
        if (addon != nullptr && parent != nullptr &&
            parent->owner == command_owner &&
            parent->construction_complete && parent->attached_addon_id == 0U &&
            resource_cost_available(status, addon->simulation.mineral_cost,
                                    addon->simulation.gas_cost) &&
            addon_center_for_parent(*addon, *parent, center_x, center_y)) {
          // placebox.cpp::sub_482050 validates the add-on footprint first,
          // subtracts dword_8DF730[addon] to recover the owner's footprint,
          // then CUnitTBuild.cpp::sub_440C90 links owner+192/addon+132.
          cancel_command_target(status);
          status.placement_unit_type = addon->unit_type;
          status.placement_x = center_x;
          status.placement_y = center_y;
          status.placement_active = true;
          status.placement_valid =
              placement_is_valid(status, *addon, center_x, center_y,
                                 command_owner);
          if (status.placement_valid) {
            (void)place_current_building(status);
          }
        }
      } else if (button.action == CommandAction::research_technology) {
        ScenarioUnitPreview *building{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.is_building && unit.construction_complete) {
            building = &unit;
            break;
          }
        }
        if (building == nullptr ||
            button.argument >= status.technology_traits.size() ||
            building->owner >=
                status.player_researched_technologies.size() ||
            status.player_researched_technologies[building->owner]
                                                  [button.argument] ||
            building->active_technology != 28U ||
            building->active_upgrade != 46U) {
          return;
        }
        const starcraft::data::TechnologyResearchTraits &traits =
            status.technology_traits[button.argument];
        if (!resource_cost_available(status, traits.mineral_cost,
                                     traits.gas_cost)) {
          return;
        }
        spend_player_resources(status, building->owner,
                               traits.mineral_cost, traits.gas_cost);
        building->active_technology =
            static_cast<std::uint8_t>(button.argument);
        building->technology_ticks_total = static_cast<std::uint16_t>(
            (std::max)(1U, static_cast<unsigned>(traits.research_time) >> 1U));
        building->technology_ticks_remaining =
            building->technology_ticks_total;
        (void)restart_unit_animation(status, *building, 19U);
      } else if (button.action == CommandAction::upgrade_technology) {
        ScenarioUnitPreview *building{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.is_building && unit.construction_complete) {
            building = &unit;
            break;
          }
        }
        if (building == nullptr || button.argument >= status.upgrade_traits.size() ||
            building->active_technology != 28U ||
            building->active_upgrade != 46U) {
          return;
        }
        const std::size_t upgrade = button.argument;
        const starcraft::data::UpgradeResearchTraits &traits =
            status.upgrade_traits[upgrade];
        const std::uint32_t level =
            building->owner < status.player_upgrade_levels.size()
                ? status.player_upgrade_levels[building->owner][upgrade]
                : status.upgrade_levels[upgrade];
        if (level >= traits.maximum_level) {
          return;
        }
        const std::uint32_t minerals =
            traits.mineral_cost + level * traits.mineral_factor;
        const std::uint32_t gas = traits.gas_cost + level * traits.gas_factor;
        const std::uint32_t raw_time =
            traits.research_time + level * traits.time_factor;
        if (!resource_cost_available(status, minerals, gas)) {
          return;
        }
        spend_player_resources(status, building->owner, minerals, gas);
        building->active_upgrade = static_cast<std::uint8_t>(upgrade);
        building->technology_ticks_total = static_cast<std::uint16_t>(
            (std::max)(1U, raw_time >> 1U));
        building->technology_ticks_remaining =
            building->technology_ticks_total;
        (void)restart_unit_animation(status, *building, 19U);
      } else if (button.action == CommandAction::morph_building) {
        ScenarioUnitPreview *building{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.is_building && unit.construction_complete) {
            building = &unit;
            break;
          }
        }
        if (building == nullptr ||
            !starcraft::lang::is_zerg_building_morph_target(button.argument) ||
            button.argument >= status.runtime_unit_types.size()) {
          return;
        }
        const RuntimeUnitType &target = status.runtime_unit_types[button.argument];
        const starcraft::data::UnitSimulationTraits &simulation =
            target.initialization.simulation;
        if (!target.ready ||
            target.initialization.placement_width != building->selection_width ||
            target.initialization.placement_height !=
                building->selection_height ||
            !resource_cost_available(status, simulation.mineral_cost,
                                     simulation.gas_cost)) {
          return;
        }
        spend_player_resources(status, building->owner,
                               simulation.mineral_cost,
                               simulation.gas_cost);
        building->construction_complete = false;
        building->construction_target_type = button.argument;
        building->construction_ticks_total = static_cast<std::uint16_t>(
            (std::max)(1U,
                       static_cast<unsigned>(simulation.build_time) >> 1U));
        building->construction_ticks_remaining =
            building->construction_ticks_total;
        building->construction_animation_phase = 2U;
        status.active_command_card = 0U;
        // CUnitZBuild.cpp::sub_447B40 calls sub_42BA80(this, 1) before
        // action 13. For building morphs that lookup uses the current
        // Hatchery/Spire/Colony type, so its units.dat construction image
        // remains primary until the target type is committed.
        const BuildableUnitVisual *const current =
            find_buildable_unit(status, building->unit_type);
        if (current != nullptr &&
            current->construction_asset_index != SIZE_MAX) {
          (void)replace_preview_primary_image(
              status, *building, current->construction_asset_index);
        }
        (void)restart_unit_animation(status, *building, 13U);
      } else if (button.action == CommandAction::place_nydus_exit) {
        const ScenarioUnitPreview *const canal = first_selected_unit(status);
        const BuildableUnitVisual *const exit = find_buildable_unit(status, 134U);
        if (canal == nullptr || canal->owner != command_owner ||
            canal->unit_type != 134U || !canal->construction_complete ||
            canal->attached_addon_id != 0U || exit == nullptr) {
          return;
        }
        cancel_command_target(status);
        status.nydus_parent_id = canal->unit_id;
        status.placement_unit_type = 134U;
        status.placement_active = true;
        status.placement_valid = false;
      } else if (button.action == CommandAction::cancel_research) {
        for (ScenarioUnitPreview &unit : status.units) {
          if (!unit.selected || unit.active_technology >=
                                    status.technology_traits.size()) {
            continue;
          }
          const starcraft::data::TechnologyResearchTraits &traits =
              status.technology_traits[unit.active_technology];
          refund_player_resources(status, unit.owner,
                                  3U * traits.mineral_cost / 4U,
                                  3U * traits.gas_cost / 4U);
          unit.active_technology = 28U;
          unit.technology_ticks_total = 0U;
          unit.technology_ticks_remaining = 0U;
          (void)restart_unit_animation(status, unit, 20U);
          break;
        }
      } else if (button.action == CommandAction::cancel_upgrade) {
        for (ScenarioUnitPreview &unit : status.units) {
          if (!unit.selected ||
              unit.active_upgrade >= status.upgrade_traits.size()) {
            continue;
          }
          const std::size_t upgrade = unit.active_upgrade;
          const starcraft::data::UpgradeResearchTraits &traits =
              status.upgrade_traits[upgrade];
          const std::uint32_t level =
              unit.owner < status.player_upgrade_levels.size()
                  ? status.player_upgrade_levels[unit.owner][upgrade]
                  : status.upgrade_levels[upgrade];
          refund_player_resources(
              status, unit.owner,
              3U * (traits.mineral_cost + level * traits.mineral_factor) / 4U,
              3U * (traits.gas_cost + level * traits.gas_factor) / 4U);
          unit.active_upgrade = 46U;
          unit.technology_ticks_total = 0U;
          unit.technology_ticks_remaining = 0U;
          (void)restart_unit_animation(status, unit, 20U);
          break;
        }
      } else if (button.action == CommandAction::begin_patrol_target) {
        // statbtn.cpp::sub_47EC70 passes order 0x92 for both unit and terrain.
        begin_command_target(status, 0x92U, 0x92U);
      } else if (button.action == CommandAction::hold_position) {
        // statbtn.cpp::sub_47ECC0 dispatches the local hold command through
        // sub_4748E0. The recovered simulation keeps the unit stationary.
        cancel_command_target(status);
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              !unit.is_building) {
            cancel_unit_order(status, unit);
          }
        }
      } else if (button.action == CommandAction::toggle_cloak) {
        cancel_command_target(status);
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner) {
            (void)toggle_unit_cloak(status, unit, button.argument != 0U);
          }
        }
      } else if (button.action == CommandAction::begin_load_target) {
        // sub_47F900 opens the target cursor with unit order 0x5B and the
        // ordinary terrain move order 7.
        begin_command_target(status, 0x5BU, 7U);
      } else if (button.action == CommandAction::begin_technology_target) {
        const std::uint8_t order = technology_target_order(
            static_cast<std::uint8_t>(button.argument));
        if (order != 0U) {
          // Target-tech's recovered jump table passes the same spell order in
          // CL and DL to target.cpp::sub_4B0120. Empty terrain remains a spell
          // target rather than silently becoming move order 7.
          begin_command_target(status, order, order);
        }
      } else if (button.action == CommandAction::archon_warp) {
        // netcmd.cpp::sub_4762B0 consumes selected CUnits in adjacent pairs.
        // Each valid (67,67) pair receives reciprocal order 0x66; the order
        // handler moves the pair together before one CUnit becomes type 68.
        std::array<std::uint32_t, 12> templars{};
        std::size_t count{};
        for (const ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == command_owner &&
              unit.unit_type == 67U &&
              count < templars.size()) {
            templars[count++] = unit.unit_id;
          }
        }
        for (std::size_t index = 0U; index + 1U < count; index += 2U) {
          ScenarioUnitPreview *const first =
              find_unit_by_id(status, templars[index]);
          ScenarioUnitPreview *const second =
              find_unit_by_id(status, templars[index + 1U]);
          if (first == nullptr || second == nullptr) {
            continue;
          }
          const bool first_started = begin_scv_interaction(
              status, *first, *second, ActiveUnitOrder::archon_warp);
          const bool second_started = begin_scv_interaction(
              status, *second, *first, ActiveUnitOrder::archon_warp);
          if (!first_started || !second_started) {
            cancel_unit_order(status, *first);
            cancel_unit_order(status, *second);
          }
        }
      } else if (button.action == CommandAction::unload_all) {
        // sub_47F950 issues order 0x6C; CUnitTransport.cpp::sub_443850 walks
        // the same eight compact cargo slots for Dropships, Shuttles,
        // Overlords, and Bunkers.
        for (ScenarioUnitPreview &transport : status.units) {
          if (transport.selected && transport.alive &&
              transport.owner == command_owner &&
              transport.construction_complete) {
            (void)unload_transport_units(status, transport);
          }
        }
      } else if (button.action == CommandAction::close_card) {
        cancel_command_target(status);
        status.active_command_card = 0;
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
        status.nydus_parent_id = 0U;
      }
      return;
    }
  }
}

bool parse_command_controls(const std::vector<std::uint8_t> &layout,
                            std::array<CommandControl, 9> &controls) noexcept {
  // DLG/dlg.cpp::sub_4D9CC0 at 0x004D9CC0 relocates the linked controls.
  // statcmd.cpp::sub_4A4150 reads each command position at control+32.
  if (layout.size() < 70 || read_u16(layout, 34) != 0) {
    return false;
  }
  const std::int16_t root_x = static_cast<std::int16_t>(read_u16(layout, 4));
  const std::int16_t root_y = static_cast<std::int16_t>(read_u16(layout, 6));
  std::uint32_t offset = read_u32(layout, 66);
  std::array<bool, 9> found{};
  std::size_t count{};
  while (offset != 0 && offset + 70U <= layout.size() &&
         count < controls.size()) {
    if (read_u16(layout, offset + 34U) != 2) {
      return false;
    }
    const std::uint16_t position = read_u16(layout, offset + 32U);
    if (position == 0 || position > controls.size() || found[position - 1U]) {
      return false;
    }
    CommandControl control{};
    control.position = position;
    control.left = static_cast<std::int16_t>(
        root_x + static_cast<std::int16_t>(read_u16(layout, offset + 4U)));
    control.top = static_cast<std::int16_t>(
        root_y + static_cast<std::int16_t>(read_u16(layout, offset + 6U)));
    control.right = static_cast<std::int16_t>(
        root_x + static_cast<std::int16_t>(read_u16(layout, offset + 8U)));
    control.bottom = static_cast<std::int16_t>(
        root_y + static_cast<std::int16_t>(read_u16(layout, offset + 10U)));
    controls[position - 1U] = control;
    found[position - 1U] = true;
    ++count;
    offset = read_u32(layout, offset);
  }
  return count == controls.size() &&
         std::all_of(found.begin(), found.end(),
                     [](const bool value) { return value; });
}

// The packed 19-byte records referenced by the original command-card table at
// 0x00500188 are preserved here after applying their initial-state conditions.
// Type 7 points to 0x004FF3D0, type 106 to 0x004FFA00, card 237 to 0x004FF480,
// and card 240 to 0x004FF530.
constexpr std::array<CommandButtonVisual, 9> kScvButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 232, 0, CommandAction::begin_repair_target},
    {5, 231, 0, CommandAction::begin_gather_target},
    {6, 233, 0, CommandAction::return_cargo},
    {7, 234, 237, CommandAction::open_card},
    {8, 235, 240, CommandAction::open_card},
    {9, 236, 0, CommandAction::cancel_target},
}};

// Unit 41 points to 0x004FDF28. The two position-9 records at the end of the
// original nine-entry card are the mutually exclusive Burrow/Unburrow state;
// they remain omitted until that researched order is represented. The seven
// unconditional records, including both exact build-subcard links, are here.
constexpr std::array<CommandButtonVisual, 7> kDroneButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {5, 231, 0, CommandAction::begin_gather_target},
    {6, 233, 0, CommandAction::return_cargo},
    {7, 257, 236, CommandAction::open_card},
    {8, 258, 239, CommandAction::open_card},
}};

// Unit 64 points to 0x004FE980. These are all seven packed Probe records.
constexpr std::array<CommandButtonVisual, 7> kProbeButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {5, 231, 0, CommandAction::begin_gather_target},
    {6, 233, 0, CommandAction::return_cargo},
    {7, 272, 238, CommandAction::open_card},
    {8, 273, 241, CommandAction::open_card},
}};

// The first three packed records are shared by the ordinary mobile-unit cards
// (Marine card 0, Ghost card 1, Vulture card 2, and the Zerg/Protoss cards):
// action 0x0047EA20 move, 0x0047EA50 stop, and 0x0047EA80 attack.
constexpr std::array<CommandButtonVisual, 3> kMobileUnitButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
}};

// Aircraft cards transcribed from the 0x00500188 command-card table. The
// shared positions 4/5 are the exact patrol (0x0047EC70) and hold
// (0x0047ECC0) records omitted by the old three-button fallback.
constexpr std::array<CommandButtonVisual, 5> kAircraftButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
}};

// Exact Protoss unit records at cards 67, 68, 71, 72/82, and 83. The old
// generic ground/air fallbacks returned before these unit-specific records.
constexpr std::array<CommandButtonVisual, 8> kHighTemplarButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {7, 275, 22, CommandAction::begin_technology_target},
    {8, 277, 23, CommandAction::begin_technology_target},
    {9, 68, 27, CommandAction::archon_warp},
}};

// Tassadar's type-79 card is a distinct seven-record table at 0x004FEE38.
// Its two technology-looking records both dispatch the generic command-30
// callback rather than Target-tech, and it has no Archon Warp record.
constexpr std::array<CommandButtonVisual, 7> kHeroTemplarButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {7, 277, 22, CommandAction::none},
    {8, 277, 23, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 6> kArchonButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    // Technology 21 (Mind Control) has no order in orders.dat and the shipped
    // callback only sends command 0x1E/21. Keep its recovered visual record;
    // it remains inert until its missing command receiver is recovered.
    {7, 274, 21, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 8> kArbiterButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {7, 280, 24, CommandAction::begin_technology_target},
    {8, 278, 25, CommandAction::begin_technology_target},
    // Essence Flare is present in this beta card/techdata but has no order in
    // orders.dat and reaches Target-tech's fatal default in StarCraft.exe.
    {9, 279, 26, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 7> kCarrierButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {7, 73, 73, CommandAction::train_unit},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 7> kReaverButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {7, 85, 85, CommandAction::train_unit},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 7> kTransportButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
    {4, 254, 0, CommandAction::begin_patrol_target},
    {5, 255, 0, CommandAction::hold_position},
    {8, 307, 0, CommandAction::begin_load_target},
    {9, 310, 0, CommandAction::unload_all},
}};

constexpr std::array<CommandButtonVisual, 3> kScienceVesselAbilities{{
    {7, 247, 6, CommandAction::begin_technology_target},
    {8, 241, 2, CommandAction::begin_technology_target},
    {9, 242, 7, CommandAction::begin_technology_target},
}};

constexpr std::array<CommandButtonVisual, 1> kBattlecruiserAbilities{{
    {7, 251, 8, CommandAction::begin_technology_target},
}};

constexpr std::array<CommandButtonVisual, 1> kTargetCancelButtons{{
    {9, 236, 0, CommandAction::cancel_target},
}};

// Cards 231/232/234 each resolve their conditional position-nine record to
// statbtn.cpp::sub_47EA00. That routine sends net command 0x1c (Order Kill)
// for the one selected incomplete Terran/Zerg/Protoss structure.
constexpr std::array<CommandButtonVisual, 1> kCancelConstructionButtons{{
    {9, 236, 0, CommandAction::cancel_construction},
}};

constexpr std::array<CommandButtonVisual, 4> kCommandCenterButtons{{
    {1, 7, 7, CommandAction::train_unit},
    {7, 107, 107, CommandAction::build_addon},
    {8, 108, 108, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 2> kFactoryButtons{{
    {2, 117, 117, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 2> kStarportButtons{{
    {2, 118, 118, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

constexpr std::array<CommandButtonVisual, 3> kScienceFacilityButtons{{
    {7, 117, 117, CommandAction::build_addon},
    {8, 118, 118, CommandAction::build_addon},
    {9, 236, 0, CommandAction::none},
}};

// Exact idle-building records decoded from the executable's command-card
// table at 0x00500188. Only mutually exclusive lift/land/cancel records are
// omitted; the active research/upgrade cancel card is selected above.
constexpr std::array<CommandButtonVisual, 2> kAcademyButtons{{
    {1, 237, 0, CommandAction::research_technology},
    {4, 238, 16, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 1> kMachineShopButtons{{
    {1, 239, 18, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 6> kTerranScienceFacilityButtons{{
    {1, 247, 6, CommandAction::research_technology},
    {2, 241, 2, CommandAction::research_technology},
    {3, 242, 7, CommandAction::research_technology},
    {4, 248, 19, CommandAction::upgrade_technology},
    {7, 117, 117, CommandAction::build_addon},
    {8, 118, 118, CommandAction::build_addon},
}};
constexpr std::array<CommandButtonVisual, 4> kControlTowerButtons{{
    {1, 240, 1, CommandAction::research_technology},
    {2, 252, 10, CommandAction::research_technology},
    {4, 249, 20, CommandAction::upgrade_technology},
    {5, 256, 21, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 4> kCovertOpsButtons{{
    {1, 252, 9, CommandAction::research_technology},
    {2, 251, 8, CommandAction::research_technology},
    {4, 284, 22, CommandAction::upgrade_technology},
    {5, 285, 23, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kPhysicsLabButtons{{
    {1, 243, 3, CommandAction::research_technology},
    {2, 245, 5, CommandAction::research_technology},
    {4, 286, 17, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 2> kEngineeringBayButtons{{
    {1, 287, 7, CommandAction::upgrade_technology},
    {2, 291, 0, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 4> kArmoryButtons{{
    {1, 288, 8, CommandAction::upgrade_technology},
    {2, 289, 9, CommandAction::upgrade_technology},
    {4, 292, 1, CommandAction::upgrade_technology},
    {5, 290, 2, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 1> kBunkerButtons{{
    {9, 310, 0, CommandAction::unload_all},
}};

constexpr std::array<CommandButtonVisual, 9> kTerranBasicBuildButtons{{
    {1, 106, 106, CommandAction::begin_building_placement},
    {2, 109, 109, CommandAction::begin_building_placement},
    {3, 110, 110, CommandAction::begin_building_placement},
    {4, 111, 111, CommandAction::begin_building_placement},
    {5, 122, 122, CommandAction::begin_building_placement},
    {6, 124, 124, CommandAction::begin_building_placement},
    {7, 112, 112, CommandAction::begin_building_placement},
    {8, 125, 125, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

constexpr std::array<CommandButtonVisual, 5> kTerranAdvancedBuildButtons{{
    {1, 113, 113, CommandAction::begin_building_placement},
    {2, 114, 114, CommandAction::begin_building_placement},
    {3, 116, 116, CommandAction::begin_building_placement},
    {4, 123, 123, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

// Card 236 at 0x004FDFD8 and card 239 at 0x004FE060. The executable routes
// these records through CUnitZBuild.cpp's Drone-morph action 0x0047F490.
constexpr std::array<CommandButtonVisual, 7> kZergBasicBuildButtons{{
    {1, 131, 131, CommandAction::begin_building_placement},
    {2, 143, 143, CommandAction::begin_building_placement},
    {3, 149, 149, CommandAction::begin_building_placement},
    {4, 142, 142, CommandAction::begin_building_placement},
    {5, 139, 139, CommandAction::begin_building_placement},
    {7, 135, 135, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

constexpr std::array<CommandButtonVisual, 6> kZergAdvancedBuildButtons{{
    {1, 134, 134, CommandAction::begin_building_placement},
    {2, 141, 141, CommandAction::begin_building_placement},
    {3, 138, 138, CommandAction::begin_building_placement},
    {4, 140, 140, CommandAction::begin_building_placement},
    {5, 136, 136, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

// Card 238 at 0x004FEA08 and card 241 at 0x004FEAB8. Their original action
// 0x0047F2B0 enters the Protoss placement order implemented below the UI.
constexpr std::array<CommandButtonVisual, 9> kProtossBasicBuildButtons{{
    {1, 154, 154, CommandAction::begin_building_placement},
    {2, 156, 156, CommandAction::begin_building_placement},
    {3, 157, 157, CommandAction::begin_building_placement},
    {4, 160, 160, CommandAction::begin_building_placement},
    {5, 166, 166, CommandAction::begin_building_placement},
    {6, 162, 162, CommandAction::begin_building_placement},
    {7, 164, 164, CommandAction::begin_building_placement},
    {8, 172, 172, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

constexpr std::array<CommandButtonVisual, 9> kProtossAdvancedBuildButtons{{
    {1, 155, 155, CommandAction::begin_building_placement},
    {2, 167, 167, CommandAction::begin_building_placement},
    {3, 163, 163, CommandAction::begin_building_placement},
    {4, 159, 159, CommandAction::begin_building_placement},
    {5, 169, 169, CommandAction::begin_building_placement},
    {6, 165, 165, CommandAction::begin_building_placement},
    {7, 171, 171, CommandAction::begin_building_placement},
    {8, 170, 170, CommandAction::begin_building_placement},
    {9, 236, 0, CommandAction::close_card},
}};

// The following idle building cards are direct transcriptions of the packed
// 19-byte command records reached from the executable table at 0x00500188.
// Conditions 0x004A29F0 and 0x004A2AF0 gate research and upgrades; their
// mutually exclusive in-progress cancel records are supplied dynamically.
constexpr std::array<CommandButtonVisual, 2> kHatcheryButtons{{
    {1, 259, 11, CommandAction::research_technology},
    {7, 132, 132, CommandAction::morph_building},
}};
constexpr std::array<CommandButtonVisual, 5> kLairButtons{{
    {1, 259, 11, CommandAction::research_technology},
    {4, 261, 24, CommandAction::upgrade_technology},
    {5, 262, 25, CommandAction::upgrade_technology},
    {6, 295, 26, CommandAction::upgrade_technology},
    {7, 133, 133, CommandAction::morph_building},
}};
constexpr std::array<CommandButtonVisual, 4> kHiveButtons{{
    {1, 259, 11, CommandAction::research_technology},
    {4, 261, 24, CommandAction::upgrade_technology},
    {5, 262, 25, CommandAction::upgrade_technology},
    {6, 295, 26, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 1> kNydusCanalButtons{{
    {1, 134, 134, CommandAction::place_nydus_exit},
}};
constexpr std::array<CommandButtonVisual, 2> kHydraliskDenButtons{{
    {1, 266, 29, CommandAction::upgrade_technology},
    {2, 268, 30, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 4> kDefilerMoundButtons{{
    {1, 265, 16, CommandAction::research_technology},
    {2, 270, 15, CommandAction::research_technology},
    {3, 301, 17, CommandAction::research_technology},
    {4, 294, 32, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kGreaterSpireButtons{{
    {1, 300, 12, CommandAction::upgrade_technology},
    {2, 297, 4, CommandAction::upgrade_technology},
    {4, 44, 13, CommandAction::research_technology},
}};
constexpr std::array<CommandButtonVisual, 5> kQueensNestButtons{{
    {1, 271, 19, CommandAction::research_technology},
    {2, 40, 14, CommandAction::research_technology},
    {3, 267, 18, CommandAction::research_technology},
    {4, 293, 31, CommandAction::upgrade_technology},
    {5, 311, 12, CommandAction::research_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kEvolutionChamberButtons{{
    {1, 298, 10, CommandAction::upgrade_technology},
    {2, 299, 11, CommandAction::upgrade_technology},
    {4, 296, 3, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 1> kUltraliskCavernButtons{{
    {1, 269, 20, CommandAction::research_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kSpireButtons{{
    {1, 300, 12, CommandAction::upgrade_technology},
    {2, 297, 4, CommandAction::upgrade_technology},
    {7, 137, 137, CommandAction::morph_building},
}};
constexpr std::array<CommandButtonVisual, 2> kSpawningPoolButtons{{
    {1, 263, 27, CommandAction::upgrade_technology},
    {2, 264, 28, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 2> kCreepColonyButtons{{
    {7, 144, 144, CommandAction::morph_building},
    {8, 146, 146, CommandAction::morph_building},
}};
constexpr std::array<CommandButtonVisual, 2> kDefenseBuildingButtons{{
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
}};

constexpr std::array<CommandButtonVisual, 1> kCancelResearchButtons{{
    {9, 236, 0, CommandAction::cancel_research},
}};
constexpr std::array<CommandButtonVisual, 1> kCancelUpgradeButtons{{
    {9, 236, 0, CommandAction::cancel_upgrade},
}};

constexpr std::array<CommandButtonVisual, 2> kObservatoryButtons{{
    {1, 276, 39, CommandAction::upgrade_technology},
    {2, 315, 38, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 1> kCitadelButtons{{
    {4, 306, 34, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kCyberneticsCoreButtons{{
    {1, 305, 14, CommandAction::upgrade_technology},
    {2, 303, 6, CommandAction::upgrade_technology},
    {4, 281, 33, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 5> kTemplarArchivesButtons{{
    {1, 275, 22, CommandAction::research_technology},
    {2, 277, 23, CommandAction::research_technology},
    {3, 68, 27, CommandAction::research_technology},
    {4, 316, 40, CommandAction::upgrade_technology},
    {6, 274, 21, CommandAction::research_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kForgeButtons{{
    {1, 304, 13, CommandAction::upgrade_technology},
    {4, 302, 5, CommandAction::upgrade_technology},
    {5, 308, 15, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kFleetBeaconButtons{{
    {1, 317, 41, CommandAction::upgrade_technology},
    {2, 318, 42, CommandAction::upgrade_technology},
    {3, 319, 43, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 4> kArbiterTribunalButtons{{
    {1, 280, 24, CommandAction::research_technology},
    {2, 278, 25, CommandAction::research_technology},
    {3, 279, 26, CommandAction::research_technology},
    {4, 320, 44, CommandAction::upgrade_technology},
}};
constexpr std::array<CommandButtonVisual, 3> kRoboticsSupportBayButtons{{
    {1, 312, 35, CommandAction::upgrade_technology},
    {2, 313, 36, CommandAction::upgrade_technology},
    {3, 314, 37, CommandAction::upgrade_technology},
}};

template <std::size_t Size>
constexpr CommandCardView card_view(
    const std::array<CommandButtonVisual, Size> &buttons) noexcept {
  return {buttons.data(), buttons.size()};
}

CommandCardView recovered_building_card(const std::uint16_t unit_type) noexcept {
  switch (unit_type) {
    case 112U: return card_view(kAcademyButtons);
    case 120U: return card_view(kMachineShopButtons);
    case 116U: return card_view(kTerranScienceFacilityButtons);
    case 115U: return card_view(kControlTowerButtons);
    case 117U: return card_view(kCovertOpsButtons);
    case 118U: return card_view(kPhysicsLabButtons);
    case 122U: return card_view(kEngineeringBayButtons);
    case 123U: return card_view(kArmoryButtons);
    case 125U: return card_view(kBunkerButtons);
    case 131U: return card_view(kHatcheryButtons);
    case 132U: return card_view(kLairButtons);
    case 133U: return card_view(kHiveButtons);
    case 134U: return card_view(kNydusCanalButtons);
    case 135U: return card_view(kHydraliskDenButtons);
    case 136U: return card_view(kDefilerMoundButtons);
    case 137U: return card_view(kGreaterSpireButtons);
    case 138U: return card_view(kQueensNestButtons);
    case 139U: return card_view(kEvolutionChamberButtons);
    case 140U: return card_view(kUltraliskCavernButtons);
    case 141U: return card_view(kSpireButtons);
    case 142U: return card_view(kSpawningPoolButtons);
    case 143U: return card_view(kCreepColonyButtons);
    case 144U:
    case 146U:
    case 162U: return card_view(kDefenseBuildingButtons);
    case 159U: return card_view(kObservatoryButtons);
    case 163U: return card_view(kCitadelButtons);
    case 164U: return card_view(kCyberneticsCoreButtons);
    case 165U: return card_view(kTemplarArchivesButtons);
    case 166U: return card_view(kForgeButtons);
    case 169U: return card_view(kFleetBeaconButtons);
    case 170U: return card_view(kArbiterTribunalButtons);
    case 171U: return card_view(kRoboticsSupportBayButtons);
    default: return {};
  }
}

CommandCardView command_card_for(const BootstrapStatus &status) noexcept {
  const ScenarioUnitPreview *const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return {};
  }
  // statcmd.cpp's owner-condition callbacks suppress every actionable record
  // when the first selected CUnit is not controlled by the local player. The
  // stat/portrait panels remain independent of this command-card decision.
  if (selected->owner != status.command_player) {
    return {};
  }
  if (status.command_target_active) {
    return {kTargetCancelButtons.data(), kTargetCancelButtons.size()};
  }
  if (selected->is_building && !selected->construction_complete) {
    return {kCancelConstructionButtons.data(),
            kCancelConstructionButtons.size()};
  }
  const auto technology_ready = [&](const std::uint8_t technology) noexcept {
    return technology < status.technology_traits.size() &&
           selected->owner < status.player_researched_technologies.size() &&
           status.player_researched_technologies[selected->owner][technology];
  };
  if (starcraft::lang::is_terran_scv(selected->unit_type) &&
      status.active_command_card == 237) {
    return {kTerranBasicBuildButtons.data(), kTerranBasicBuildButtons.size()};
  }
  if (starcraft::lang::is_terran_scv(selected->unit_type) &&
      status.active_command_card == 240) {
    return {kTerranAdvancedBuildButtons.data(),
            kTerranAdvancedBuildButtons.size()};
  }
  if (starcraft::lang::is_terran_scv(selected->unit_type)) {
    return {kScvButtons.data(), kScvButtons.size()};
  }
  if (starcraft::lang::is_zerg_drone(selected->unit_type) &&
      status.active_command_card == 236U) {
    return {kZergBasicBuildButtons.data(), kZergBasicBuildButtons.size()};
  }
  if (starcraft::lang::is_zerg_drone(selected->unit_type) &&
      status.active_command_card == 239U) {
    return {kZergAdvancedBuildButtons.data(),
            kZergAdvancedBuildButtons.size()};
  }
  if (starcraft::lang::is_zerg_drone(selected->unit_type)) {
    return {kDroneButtons.data(), kDroneButtons.size()};
  }
  if (starcraft::lang::is_protoss_probe(selected->unit_type) &&
      status.active_command_card == 238U) {
    return {kProtossBasicBuildButtons.data(),
            kProtossBasicBuildButtons.size()};
  }
  if (starcraft::lang::is_protoss_probe(selected->unit_type) &&
      status.active_command_card == 241U) {
    return {kProtossAdvancedBuildButtons.data(),
            kProtossAdvancedBuildButtons.size()};
  }
  if (starcraft::lang::is_protoss_probe(selected->unit_type)) {
    return {kProbeButtons.data(), kProbeButtons.size()};
  }
  if (selected->construction_complete && selected->unit_type == 67U) {
    static thread_local std::array<CommandButtonVisual, 8> available{};
    std::size_t count{};
    for (const CommandButtonVisual &button : kHighTemplarButtons) {
      if (button.action == CommandAction::begin_technology_target &&
          !technology_ready(static_cast<std::uint8_t>(button.argument))) {
        continue;
      }
      available[count++] = button;
    }
    const std::size_t selected_templars = static_cast<std::size_t>(
        std::count_if(status.units.begin(), status.units.end(),
                      [&status](const ScenarioUnitPreview &unit) {
                        return unit.selected && unit.alive &&
                               unit.owner == status.command_player &&
                               unit.unit_type == 67U;
                      }));
    // Conditions 0x004A3160/0x004A3180 choose two records at the same
    // position.  The first is the unavailable placeholder; the second
    // dispatches sub_47F9C0 only when a Templar pair is selected.
    if (count != 0U && available[count - 1U].position == 9U &&
        selected_templars < 2U) {
      available[count - 1U].action = CommandAction::none;
    }
    return {available.data(), count};
  }
  if (selected->construction_complete && selected->unit_type == 79U) {
    static thread_local std::array<CommandButtonVisual, 7> available{};
    std::copy(kHeroTemplarButtons.begin(),
              kHeroTemplarButtons.begin() + 5U, available.begin());
    std::size_t count = 5U;
    for (std::size_t index = 5U; index < kHeroTemplarButtons.size(); ++index) {
      if (technology_ready(static_cast<std::uint8_t>(
              kHeroTemplarButtons[index].argument))) {
        available[count++] = kHeroTemplarButtons[index];
      }
    }
    return {available.data(), count};
  }
  if (selected->construction_complete && selected->unit_type == 68U) {
    static thread_local std::array<CommandButtonVisual, 6> available{};
    std::copy(kArchonButtons.begin(), kArchonButtons.begin() + 5U,
              available.begin());
    std::size_t count = 5U;
    if (technology_ready(21U)) {
      available[count++] = kArchonButtons.back();
    }
    return {available.data(), count};
  }
  if (selected->construction_complete && selected->unit_type == 71U) {
    static thread_local std::array<CommandButtonVisual, 8> available{};
    std::size_t count{};
    for (const CommandButtonVisual &button : kArbiterButtons) {
      if ((button.action == CommandAction::begin_technology_target ||
           button.action == CommandAction::none) &&
          button.argument >= 24U && button.argument <= 26U &&
          !technology_ready(static_cast<std::uint8_t>(button.argument))) {
        continue;
      }
      available[count++] = button;
    }
    return {available.data(), count};
  }
  if (selected->construction_complete &&
      (selected->unit_type == 72U || selected->unit_type == 82U)) {
    return card_view(kCarrierButtons);
  }
  if (selected->construction_complete && selected->unit_type == 83U) {
    return card_view(kReaverButtons);
  }
  if (selected->construction_complete && is_airborne(*selected)) {
    if (selected->unit_type == 11U || selected->unit_type == 69U ||
        (selected->unit_type == 42U && selected->owner <
                                           status.player_upgrade_levels.size() &&
         status.player_upgrade_levels[selected->owner][24U] != 0U)) {
      return card_view(kTransportButtons);
    }
    static thread_local std::array<CommandButtonVisual, 9> aircraft{};
    std::copy(kAircraftButtons.begin(), kAircraftButtons.end(),
              aircraft.begin());
    std::size_t count = kAircraftButtons.size();
    if (selected->unit_type == 8U && technology_ready(9U)) {
      aircraft[count++] = selected->cloaked
                              ? CommandButtonVisual{
                                    7, 253, 0, CommandAction::toggle_cloak}
                              : CommandButtonVisual{
                                    7, 252, 1, CommandAction::toggle_cloak};
    } else if (selected->unit_type == 9U) {
      for (const CommandButtonVisual &ability : kScienceVesselAbilities) {
        if (technology_ready(static_cast<std::uint8_t>(ability.argument))) {
          aircraft[count++] = ability;
        }
      }
    } else if (selected->unit_type == 12U && technology_ready(8U)) {
      aircraft[count++] = kBattlecruiserAbilities.front();
    }
    return {aircraft.data(), count};
  }
  if (selected->construction_complete) {
    if (selected->active_technology < status.technology_traits.size()) {
      return card_view(kCancelResearchButtons);
    }
    if (selected->active_upgrade < status.upgrade_traits.size()) {
      return card_view(kCancelUpgradeButtons);
    }
    const starcraft::lang::UnitProductionButtonView production =
        starcraft::lang::production_buttons_for(selected->unit_type);
    if (production.count != 0U) {
      // The production identities live in CUnitBuild.cpp. Convert them to the
      // renderer's transitional visual records, then append the add-on buttons
      // that coexist on these exact primary-executable cards.
      static thread_local std::array<CommandButtonVisual, 9> recovered{};
      std::size_t count{};
      for (std::size_t index = 0;
           index < production.count && count < recovered.size(); ++index) {
        const starcraft::lang::UnitProductionButton &button =
            production.buttons[index];
        recovered[count++] = {
            button.position,
            button.icon,
            button.product_type,
            CommandAction::train_unit,
        };
      }
      const auto append_addon = [&](const std::uint16_t position,
                                    const std::uint16_t type) {
        if (count < recovered.size()) {
          recovered[count++] = {position, type, type,
                                CommandAction::build_addon};
        }
      };
      if (selected->unit_type == 106U) {
        append_addon(7U, 107U);
        append_addon(8U, 108U);
      } else if (selected->unit_type == 113U) {
        append_addon(7U, 120U);
      } else if (selected->unit_type == 114U) {
        append_addon(7U, 115U);
      }
      if (selected->unit_type != starcraft::lang::zerg_larva_type &&
          count < recovered.size()) {
        // Every non-Larva production card in the recovered table has its
        // condition-selected position-9 cancel record with icon 236.
        recovered[count++] = {9U, 236U, 0U, CommandAction::none};
      }
      return {recovered.data(), count};
    }
    const CommandCardView building = recovered_building_card(selected->unit_type);
    if (building.count != 0U) {
      static thread_local std::array<CommandButtonVisual, 9> available{};
      std::size_t count{};
      for (std::size_t index = 0; index < building.count; ++index) {
        const CommandButtonVisual &button = building.buttons[index];
        if (button.action == CommandAction::research_technology &&
            (button.argument >= status.technology_traits.size() ||
             selected->owner >=
                 status.player_researched_technologies.size() ||
             status.player_researched_technologies[selected->owner]
                                                   [button.argument])) {
          continue;
        }
        available[count++] = button;
      }
      return {available.data(), count};
    }
  }
  if (!selected->is_building && selected->movement_top_speed != 0U) {
    return {kMobileUnitButtons.data(), kMobileUnitButtons.size()};
  }
  return {};
}

void draw_hud_control_frame_gl(const SpritePreviewFrame &frame,
                               const CommandControl &control) {
  constexpr float vertical_scale = 400.0F / 480.0F;
  draw_preview_frame_gl(
      frame, static_cast<float>(control.left + frame.x_offset),
      static_cast<float>(control.top + frame.y_offset) * vertical_scale,
      static_cast<float>(frame.width),
      static_cast<float>(frame.height) * vertical_scale);
}

void draw_selected_command_panel_gl(const RecoveryWindowState &state) {
  const BootstrapStatus *const status = state.status;
  if (status == nullptr || !status->command_panel_ready ||
      status->command_panel_frames.empty()) {
    return;
  }
  const CommandCardView card = command_card_for(*status);
  const ScenarioUnitPreview *const selected = first_selected_unit(*status);
  for (std::size_t index = 0; index < card.count; ++index) {
    const CommandButtonVisual &button = card.buttons[index];
    if (button.position == 0 ||
        button.position > status->command_controls.size() ||
        button.icon >= status->command_icon_frames.size()) {
      continue;
    }
    const CommandControl &control =
        status->command_controls[button.position - 1U];
    const std::size_t panel_frame =
        state.pressed_command_position == button.position &&
                status->command_panel_frames.size() > 1
            ? 1U
            : 0U;
    draw_hud_control_frame_gl(status->command_panel_frames[panel_frame],
                              control);
    draw_hud_control_frame_gl(status->command_icon_frames[button.icon],
                              control);
    if (selected != nullptr &&
        !command_button_enabled(*status, *selected, button)) {
      const float left = static_cast<float>(control.left);
      const float top = static_cast<float>(control.top) * hud_vertical_scale();
      const float right = static_cast<float>(control.right + 1);
      const float bottom =
          static_cast<float>(control.bottom + 1) * hud_vertical_scale();
      glColor4ub(8U, 8U, 8U, 152U);
      glBegin(GL_QUADS);
      glVertex2f(left, top);
      glVertex2f(right, top);
      glVertex2f(right, bottom);
      glVertex2f(left, bottom);
      glEnd();
      glColor4ub(255U, 255U, 255U, 255U);
    }
  }

  if (selected == nullptr || state.hovered_command_position == 0U) {
    return;
  }
  const CommandButtonVisual *hovered{};
  for (std::size_t index = 0; index < card.count; ++index) {
    if (card.buttons[index].position == state.hovered_command_position) {
      hovered = &card.buttons[index];
      break;
    }
  }
  if (hovered == nullptr ||
      (hovered->action != CommandAction::train_unit &&
       hovered->action != CommandAction::begin_building_placement &&
       hovered->action != CommandAction::build_addon &&
       hovered->action != CommandAction::morph_building &&
       hovered->action != CommandAction::upgrade_technology)) {
    return;
  }

  std::uint32_t mineral_cost{};
  std::uint32_t gas_cost{};
  std::string name;
  std::string prerequisite_text;
  UnitRequirementResult requirements{};
  if (hovered->action == CommandAction::upgrade_technology &&
      hovered->argument < status->upgrade_traits.size()) {
    const auto &traits = status->upgrade_traits[hovered->argument];
    const std::uint32_t level =
        selected->owner < status->player_upgrade_levels.size()
            ? status->player_upgrade_levels[selected->owner]
                                           [hovered->argument]
            : status->upgrade_levels[hovered->argument];
    mineral_cost = traits.mineral_cost + level * traits.mineral_factor;
    gas_cost = traits.gas_cost + level * traits.gas_factor;
    const std::uint16_t label_id =
        hovered->argument < status->upgrade_display_traits.size()
            ? status->upgrade_display_traits[hovered->argument].label_string_id
            : 0U;
    name = printable_status_text(status_text(*status, label_id));
    if (name.empty()) {
      name = "Upgrade " + std::to_string(hovered->argument);
    }
    prerequisite_text =
        level >= traits.maximum_level
            ? "Maximum level reached"
            : "Upgrade to level " + std::to_string(level + 1U) + " of " +
                  std::to_string(traits.maximum_level);
  } else if (hovered->argument < status->runtime_unit_types.size() &&
      status->runtime_unit_types[hovered->argument].ready) {
    const auto &simulation = status->runtime_unit_types[hovered->argument]
                                 .initialization.simulation;
    mineral_cost = simulation.mineral_cost;
    gas_cost = simulation.gas_cost;
  } else if (const BuildableUnitVisual *const buildable =
                 find_buildable_unit(*status, hovered->argument)) {
    mineral_cost = buildable->simulation.mineral_cost;
    gas_cost = buildable->simulation.gas_cost;
  } else {
    return;
  }

  if (hovered->action != CommandAction::upgrade_technology) {
    requirements = unit_requirements_for(*status, *selected, hovered->argument);
    name = printable_status_text(status_text(
        *status, static_cast<std::uint16_t>(hovered->argument + 1U)));
    if (name.empty()) {
      name = "Unit " + std::to_string(hovered->argument);
    }
  }
  const std::string cost = "Minerals: " + std::to_string(mineral_cost) +
                           "   Gas: " + std::to_string(gas_cost);
  if (hovered->action != CommandAction::upgrade_technology) {
    prerequisite_text = "Requires: ";
    if (requirements.required_count == 0U) {
      prerequisite_text += "None";
    } else {
      for (std::size_t index = 0; index < requirements.required_count;
           ++index) {
        if (index != 0U) {
          prerequisite_text += ", ";
        }
        std::string prerequisite = printable_status_text(status_text(
            *status, static_cast<std::uint16_t>(
                         requirements.required_units[index] + 1U)));
        prerequisite_text += prerequisite.empty()
                                 ? "Unit " + std::to_string(
                                                requirements.required_units[index])
                                 : prerequisite;
      }
    }
  }

  const CommandControl &control =
      status->command_controls[hovered->position - 1U];
  const float width = 330.0F;
  const float left = (std::min)(305.0F, (std::max)(
                                          2.0F,
                                          static_cast<float>(control.left) -
                                              width + 52.0F));
  const float top = static_cast<float>(control.top) * hud_vertical_scale() -
                    50.0F;
  glColor4ub(5U, 10U, 14U, 238U);
  glBegin(GL_QUADS);
  glVertex2f(left, top);
  glVertex2f(left + width, top);
  glVertex2f(left + width, top + 46.0F);
  glVertex2f(left, top + 46.0F);
  glEnd();
  glColor4ub(55U, 120U, 150U, 255U);
  glBegin(GL_LINE_LOOP);
  glVertex2f(left, top);
  glVertex2f(left + width, top);
  glVertex2f(left + width, top + 46.0F);
  glVertex2f(left, top + 46.0F);
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  const bool cost_missing =
      player_minerals_for(*status, selected->owner) < mineral_cost ||
      player_gas_for(*status, selected->owner) < gas_cost;
  draw_game_text_gl(state, name, left + 5.0F, top + 4.0F, 235U, 235U,
                    235U);
  draw_game_text_gl(state, cost, left + 5.0F, top + 17.0F,
                    cost_missing ? 255U : 210U,
                    cost_missing ? 70U : 210U,
                    cost_missing ? 60U : 210U);
  const bool unavailable = !command_button_enabled(*status, *selected, *hovered);
  draw_game_text_gl(state, prerequisite_text, left + 5.0F, top + 30.0F,
                    unavailable ? 255U : 210U,
                    unavailable ? 75U : 210U,
                    unavailable ? 60U : 210U);
}

std::uint16_t command_position_at(const BootstrapStatus &status,
                                  const int game_x, const int game_y) noexcept {
  const CommandCardView card = command_card_for(status);
  if (!status.command_panel_ready || card.count == 0) {
    return 0;
  }
  constexpr int vertical_numerator = 5;
  constexpr int vertical_denominator = 6;
  for (std::size_t index = 0; index < card.count; ++index) {
    const CommandButtonVisual &button = card.buttons[index];
    const CommandControl &control =
        status.command_controls[button.position - 1U];
    const int top = control.top * vertical_numerator / vertical_denominator;
    const int bottom =
        (control.bottom + 1) * vertical_numerator / vertical_denominator - 1;
    if (game_x >= control.left && game_x <= control.right && game_y >= top &&
        game_y <= bottom) {
      return button.position;
    }
  }
  return 0;
}

} // namespace starcraft::recovery
