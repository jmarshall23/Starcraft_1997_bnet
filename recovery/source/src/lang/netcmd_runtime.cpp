#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::uint8_t kSmartOrderCommand = 1U;
constexpr std::uint8_t kCommandButtonCommand = 2U;
constexpr std::uint8_t kTargetOrderCommand = 3U;
constexpr std::uint8_t kBuildingPlacementCommand = 4U;
constexpr std::uint32_t kInputDelayTurns = 3U;
constexpr std::size_t kMaximumSelectedUnits = 12U;
constexpr std::size_t kMaximumTurnPayload = 2048U;

void append_u16(std::vector<std::uint8_t> &output,
                const std::uint16_t value) {
  output.push_back(static_cast<std::uint8_t>(value));
  output.push_back(static_cast<std::uint8_t>(value >> 8U));
}

void append_u32(std::vector<std::uint8_t> &output,
                const std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>(value));
  output.push_back(static_cast<std::uint8_t>(value >> 8U));
  output.push_back(static_cast<std::uint8_t>(value >> 16U));
  output.push_back(static_cast<std::uint8_t>(value >> 24U));
}

bool read_u16(const std::vector<std::uint8_t> &input, std::size_t &offset,
              std::uint16_t &value) noexcept {
  if (offset + 2U > input.size()) return false;
  value = static_cast<std::uint16_t>(input[offset]) |
          static_cast<std::uint16_t>(input[offset + 1U] << 8U);
  offset += 2U;
  return true;
}

bool read_u32(const std::vector<std::uint8_t> &input, std::size_t &offset,
              std::uint32_t &value) noexcept {
  if (offset + 4U > input.size()) return false;
  value = static_cast<std::uint32_t>(input[offset]) |
          (static_cast<std::uint32_t>(input[offset + 1U]) << 8U) |
          (static_cast<std::uint32_t>(input[offset + 2U]) << 16U) |
          (static_cast<std::uint32_t>(input[offset + 3U]) << 24U);
  offset += 4U;
  return true;
}

bool collect_local_selection(const BootstrapStatus &status,
                             std::vector<std::uint32_t> &selected) {
  selected.clear();
  selected.reserve(kMaximumSelectedUnits);
  for (const ScenarioUnitPreview &unit : status.units) {
    if (unit.selected && unit.alive && unit.owner == status.local_player &&
        selected.size() < kMaximumSelectedUnits) {
      selected.push_back(unit.unit_id);
    }
  }
  return !selected.empty();
}

bool append_pending_command(battle::BattleRuntime &runtime,
                            const std::vector<std::uint8_t> &command) {
  const std::uint32_t turn =
      (std::max)(runtime.simulation_turn + kInputDelayTurns,
                 runtime.next_turn_to_submit);
  auto pending = std::find_if(
      runtime.outgoing_turns.begin(), runtime.outgoing_turns.end(),
      [turn](const battle::PendingOutgoingTurn &candidate) {
        return candidate.turn == turn;
      });
  if (pending == runtime.outgoing_turns.end()) {
    runtime.outgoing_turns.push_back({turn, {}});
    pending = std::prev(runtime.outgoing_turns.end());
  }
  if (pending->payload.size() + command.size() > kMaximumTurnPayload) {
    return false;
  }
  pending->payload.insert(pending->payload.end(), command.begin(),
                          command.end());
  return true;
}

bool local_interface_action(const CommandButtonVisual::Action action) {
  using Action = CommandButtonVisual::Action;
  switch (action) {
    case Action::none:
    case Action::begin_move_target:
    case Action::begin_attack_target:
    case Action::begin_repair_target:
    case Action::begin_gather_target:
    case Action::cancel_target:
    case Action::open_card:
    case Action::begin_building_placement:
    case Action::place_nydus_exit:
    case Action::begin_patrol_target:
    case Action::begin_load_target:
    case Action::begin_technology_target:
    case Action::close_card:
      return true;
    default:
      return false;
  }
}

} // namespace

bool queue_network_smart_order(RecoveryWindowState &state,
                               const std::uint16_t world_x,
                               const std::uint16_t world_y) noexcept {
  if (state.status == nullptr || !state.glue.online_lobby ||
      !state.glue.battle_net.game_started) {
    return false;
  }
  BootstrapStatus &status = *state.status;
  try {
    std::vector<std::uint32_t> selected;
    if (!collect_local_selection(status, selected)) {
      return false;
    }
    std::vector<std::uint8_t> command;
    command.reserve(10U + selected.size() * sizeof(std::uint32_t));
    command.push_back(kSmartOrderCommand);
    append_u16(command, world_x);
    append_u16(command, world_y);
    append_u32(command, smart_order_target_at(status, world_x, world_y));
    command.push_back(static_cast<std::uint8_t>(selected.size()));
    for (const std::uint32_t unit_id : selected) {
      append_u32(command, unit_id);
    }
    return append_pending_command(state.glue.battle_net, command);
  } catch (...) {
    return false;
  }
}

bool queue_network_command_button(RecoveryWindowState &state,
                                  const std::uint16_t position) noexcept {
  if (state.status == nullptr || !state.glue.online_lobby ||
      !state.glue.battle_net.game_started) {
    return false;
  }
  BootstrapStatus &status = *state.status;
  const ScenarioUnitPreview *const selected_unit = first_selected_unit(status);
  if (selected_unit == nullptr || selected_unit->owner != status.local_player) {
    return false;
  }
  const CommandCardView card = command_card_for(status);
  const CommandButtonVisual *button{};
  for (std::size_t index = 0U; index < card.count; ++index) {
    if (card.buttons[index].position == position) {
      button = &card.buttons[index];
      break;
    }
  }
  if (button == nullptr) {
    return false;
  }
  if (local_interface_action(button->action)) {
    activate_command_button(status, position);
    return true;
  }
  if (!command_button_enabled(status, *selected_unit, *button)) {
    return false;
  }

  try {
    std::vector<std::uint32_t> selected;
    if (!collect_local_selection(status, selected)) {
      return false;
    }
    std::vector<std::uint8_t> command;
    command.reserve(6U + selected.size() * sizeof(std::uint32_t));
    command.push_back(kCommandButtonCommand);
    append_u16(command, position);
    append_u16(command, status.active_command_card);
    command.push_back(static_cast<std::uint8_t>(selected.size()));
    for (const std::uint32_t unit_id : selected) {
      append_u32(command, unit_id);
    }
    return append_pending_command(state.glue.battle_net, command);
  } catch (...) {
    return false;
  }
}

bool queue_network_target_order(RecoveryWindowState &state,
                                const std::uint16_t world_x,
                                const std::uint16_t world_y) noexcept {
  if (state.status == nullptr || !state.glue.online_lobby ||
      !state.glue.battle_net.game_started ||
      !state.status->command_target_active) {
    return false;
  }
  BootstrapStatus &status = *state.status;
  try {
    std::vector<std::uint32_t> selected;
    if (!collect_local_selection(status, selected)) {
      return false;
    }
    std::vector<std::uint8_t> command;
    command.reserve(12U + selected.size() * sizeof(std::uint32_t));
    command.push_back(kTargetOrderCommand);
    command.push_back(status.target_unit_order);
    command.push_back(status.target_terrain_order);
    append_u16(command, world_x);
    append_u16(command, world_y);
    append_u32(command, smart_order_target_at(status, world_x, world_y));
    command.push_back(static_cast<std::uint8_t>(selected.size()));
    for (const std::uint32_t unit_id : selected) {
      append_u32(command, unit_id);
    }
    if (!append_pending_command(state.glue.battle_net, command)) {
      return false;
    }
    cancel_command_target(status);
    return true;
  } catch (...) {
    return false;
  }
}

bool queue_network_building_placement(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr || !state.glue.online_lobby ||
      !state.glue.battle_net.game_started ||
      !state.status->placement_active || !state.status->placement_valid) {
    return false;
  }
  BootstrapStatus &status = *state.status;
  try {
    std::vector<std::uint32_t> selected;
    if (!collect_local_selection(status, selected)) {
      return false;
    }
    std::vector<std::uint8_t> command;
    command.reserve(12U + selected.size() * sizeof(std::uint32_t));
    command.push_back(kBuildingPlacementCommand);
    append_u16(command, status.placement_unit_type);
    append_u32(command, status.nydus_parent_id);
    append_u16(command, status.placement_x);
    append_u16(command, status.placement_y);
    command.push_back(static_cast<std::uint8_t>(selected.size()));
    for (const std::uint32_t unit_id : selected) {
      append_u32(command, unit_id);
    }
    if (!append_pending_command(state.glue.battle_net, command)) {
      return false;
    }
    status.placement_active = false;
    status.placement_valid = false;
    status.placement_unit_type = 0xFFFFU;
    status.nydus_parent_id = 0U;
    status.active_command_card = 0U;
    return true;
  } catch (...) {
    return false;
  }
}

bool take_network_outgoing_payload(battle::BattleRuntime &runtime,
                                   const std::uint32_t turn,
                                   std::vector<std::uint8_t> &payload) noexcept {
  payload.clear();
  const auto found = std::find_if(
      runtime.outgoing_turns.begin(), runtime.outgoing_turns.end(),
      [turn](const battle::PendingOutgoingTurn &candidate) {
        return candidate.turn == turn;
      });
  if (found == runtime.outgoing_turns.end()) {
    return true;
  }
  payload = std::move(found->payload);
  runtime.outgoing_turns.erase(found);
  return true;
}

bool apply_network_committed_turn(
    BootstrapStatus &status, const battle::CommittedTurn &commit) noexcept {
  std::vector<std::uint8_t> saved_selection;
  try {
    saved_selection.reserve(status.units.size());
    for (const ScenarioUnitPreview &unit : status.units) {
      saved_selection.push_back(unit.selected ? 1U : 0U);
    }
  } catch (...) {
    return false;
  }

  const std::uint8_t previous_command_player = status.command_player;
  const std::uint32_t previous_command_clock = status.command_execution_clock;
  const bool previous_synchronized_execution =
      status.synchronized_command_execution;
  const std::uint16_t previous_active_card = status.active_command_card;
  const std::uint16_t previous_placement_type = status.placement_unit_type;
  const std::uint32_t previous_nydus_parent = status.nydus_parent_id;
  const std::uint16_t previous_placement_x = status.placement_x;
  const std::uint16_t previous_placement_y = status.placement_y;
  const bool previous_placement_active = status.placement_active;
  const bool previous_placement_valid = status.placement_valid;
  const bool previous_target_active = status.command_target_active;
  const std::uint8_t previous_unit_order = status.target_unit_order;
  const std::uint8_t previous_terrain_order = status.target_terrain_order;
  const auto restore_interface = [&]() noexcept {
    for (std::size_t index = 0U; index < status.units.size() &&
                                index < saved_selection.size();
         ++index) {
      status.units[index].selected = saved_selection[index] != 0U;
    }
    status.command_player = previous_command_player;
    status.command_execution_clock = previous_command_clock;
    status.synchronized_command_execution = previous_synchronized_execution;
    status.active_command_card = previous_active_card;
    status.placement_unit_type = previous_placement_type;
    status.nydus_parent_id = previous_nydus_parent;
    status.placement_x = previous_placement_x;
    status.placement_y = previous_placement_y;
    status.placement_active = previous_placement_active;
    status.placement_valid = previous_placement_valid;
    status.command_target_active = previous_target_active;
    status.target_unit_order = previous_unit_order;
    status.target_terrain_order = previous_terrain_order;
  };
  status.command_execution_clock = commit.turn * kSimulationTickMilliseconds;
  status.synchronized_command_execution = true;
  for (const battle::PlayerTurnPayload &player : commit.players) {
    if (player.slot >= status.player_mineral_stock.size()) {
      restore_interface();
      return false;
    }
    std::size_t offset{};
    while (offset < player.payload.size()) {
      const std::uint8_t command = player.payload[offset++];
      status.command_player = player.slot;
      const auto select_command_units = [&](const std::size_t count) noexcept {
        if (count > kMaximumSelectedUnits ||
            offset + count * sizeof(std::uint32_t) > player.payload.size()) {
          return false;
        }
        for (ScenarioUnitPreview &unit : status.units) unit.selected = false;
        for (std::size_t index = 0U; index < count; ++index) {
          std::uint32_t unit_id{};
          if (!read_u32(player.payload, offset, unit_id)) {
            return false;
          }
          ScenarioUnitPreview *const unit = find_unit_by_id(status, unit_id);
          if (unit != nullptr && unit->alive && unit->owner == player.slot) {
            unit->selected = true;
          }
        }
        return true;
      };

      if (command == kSmartOrderCommand) {
        std::uint16_t world_x{};
        std::uint16_t world_y{};
        std::uint32_t target_unit_id{};
        if (!read_u16(player.payload, offset, world_x) ||
            !read_u16(player.payload, offset, world_y) ||
            !read_u32(player.payload, offset, target_unit_id) ||
            offset >= player.payload.size()) {
          restore_interface();
          return false;
        }
        const std::size_t count = player.payload[offset++];
        if (!select_command_units(count)) {
          restore_interface();
          return false;
        }
        (void)issue_scv_smart_order_target(status, world_x, world_y,
                                           target_unit_id);
      } else if (command == kCommandButtonCommand) {
        std::uint16_t position{};
        std::uint16_t command_card{};
        if (!read_u16(player.payload, offset, position) ||
            !read_u16(player.payload, offset, command_card) ||
            offset >= player.payload.size()) {
          restore_interface();
          return false;
        }
        const std::size_t count = player.payload[offset++];
        if (!select_command_units(count)) {
          restore_interface();
          return false;
        }
        status.active_command_card = command_card;
        status.command_target_active = false;
        status.placement_active = false;
        activate_command_button(status, position);
      } else if (command == kTargetOrderCommand) {
        if (offset + 2U > player.payload.size()) {
          restore_interface();
          return false;
        }
        const std::uint8_t unit_order = player.payload[offset++];
        const std::uint8_t terrain_order = player.payload[offset++];
        std::uint16_t world_x{};
        std::uint16_t world_y{};
        std::uint32_t target_unit_id{};
        if (!read_u16(player.payload, offset, world_x) ||
            !read_u16(player.payload, offset, world_y) ||
            !read_u32(player.payload, offset, target_unit_id) ||
            offset >= player.payload.size()) {
          restore_interface();
          return false;
        }
        const std::size_t count = player.payload[offset++];
        if (!select_command_units(count)) {
          restore_interface();
          return false;
        }
        begin_command_target(status, unit_order, terrain_order);
        (void)issue_active_scv_target(status, world_x, world_y,
                                      target_unit_id);
      } else if (command == kBuildingPlacementCommand) {
        std::uint16_t unit_type{};
        std::uint32_t nydus_parent{};
        std::uint16_t world_x{};
        std::uint16_t world_y{};
        if (!read_u16(player.payload, offset, unit_type) ||
            !read_u32(player.payload, offset, nydus_parent) ||
            !read_u16(player.payload, offset, world_x) ||
            !read_u16(player.payload, offset, world_y) ||
            offset >= player.payload.size()) {
          restore_interface();
          return false;
        }
        const std::size_t count = player.payload[offset++];
        if (!select_command_units(count)) {
          restore_interface();
          return false;
        }
        status.placement_unit_type = unit_type;
        status.nydus_parent_id = nydus_parent;
        status.placement_x = world_x;
        status.placement_y = world_y;
        status.placement_active = true;
        const BuildableUnitVisual *const buildable =
            find_buildable_unit(status, unit_type);
        status.placement_valid =
            buildable != nullptr &&
            placement_is_valid(status, *buildable, world_x, world_y,
                               player.slot);
        (void)place_current_building(status);
      } else {
        restore_interface();
        return false;
      }
    }
  }
  restore_interface();
  return true;
}

} // namespace starcraft::recovery
