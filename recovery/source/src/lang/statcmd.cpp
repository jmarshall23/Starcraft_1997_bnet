#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

using CommandAction = CommandButtonVisual::Action;

void activate_command_button(BootstrapStatus &status,
                             const std::uint16_t position) noexcept {
  const CommandCardView card = command_card_for(status);
  for (std::size_t button_index = 0; button_index < card.count;
       ++button_index) {
    const CommandButtonVisual &button = card.buttons[button_index];
    if (button.position == position) {
      status.last_command_position = position;
      if (button.action == CommandAction::train_unit) {
        ScenarioUnitPreview *producer{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.selected && unit.alive && unit.owner == 0U &&
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
          producer->production_started = GetTickCount();
          producer->production_kind =
              producer->unit_type == starcraft::lang::zerg_larva_type
                  ? starcraft::lang::UnitProductionKind::zerg_larva_morph
                  : starcraft::lang::UnitProductionKind::train;
          if (producer->production_kind ==
              starcraft::lang::UnitProductionKind::zerg_larva_morph) {
            // CUnitZBuild.cpp::sub_447820 changes larva 35 into egg 36 while
            // retaining the queued target at CUnit+0x98.
            (void)configure_preview_type(status, *producer,
                                         starcraft::lang::zerg_egg_type);
          } else {
            // CUnitBuild.cpp::sub_423020 dispatches Working (19) when the
            // first queue entry materializes.
            (void)restart_unit_animation(status, *producer, 19U);
          }
        }
        status.player_minerals -= simulation.mineral_cost;
        status.player_gas -= simulation.gas_cost;
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
          if (unit.selected && unit.alive && unit.owner == 0 &&
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
          status.placement_active = true;
          status.placement_valid = false;
        }
      } else if (button.action == CommandAction::build_addon) {
        const BuildableUnitVisual *const addon =
            find_buildable_unit(status, button.argument);
        const ScenarioUnitPreview *const parent = first_selected_unit(status);
        std::uint16_t center_x{};
        std::uint16_t center_y{};
        if (addon != nullptr && parent != nullptr && parent->owner == 0 &&
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
              placement_is_valid(status, *addon, center_x, center_y);
          if (status.placement_valid) {
            (void)place_current_building(status);
          }
        }
      } else if (button.action == CommandAction::close_card) {
        cancel_command_target(status);
        status.active_command_card = 0;
        status.placement_active = false;
        status.placement_valid = false;
        status.placement_unit_type = 0xFFFFU;
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

// The first three packed records are shared by the ordinary mobile-unit cards
// (Marine card 0, Ghost card 1, Vulture card 2, and the Zerg/Protoss cards):
// action 0x0047EA20 move, 0x0047EA50 stop, and 0x0047EA80 attack.
constexpr std::array<CommandButtonVisual, 3> kMobileUnitButtons{{
    {1, 228, 0, CommandAction::begin_move_target},
    {2, 229, 0, CommandAction::stop},
    {3, 230, 0, CommandAction::begin_attack_target},
}};

constexpr std::array<CommandButtonVisual, 1> kTargetCancelButtons{{
    {9, 236, 0, CommandAction::cancel_target},
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

CommandCardView command_card_for(const BootstrapStatus &status) noexcept {
  const ScenarioUnitPreview *const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return {};
  }
  if (status.command_target_active) {
    return {kTargetCancelButtons.data(), kTargetCancelButtons.size()};
  }
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
  if (selected->construction_complete) {
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
  }
  if (!selected->is_building && selected->movement_top_speed != 0U) {
    return {kMobileUnitButtons.data(), kMobileUnitButtons.size()};
  }
  if (selected->unit_type == 116 && selected->construction_complete) {
    return {kScienceFacilityButtons.data(), kScienceFacilityButtons.size()};
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
  }
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
