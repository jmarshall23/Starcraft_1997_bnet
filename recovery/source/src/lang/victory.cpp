#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/count.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace starcraft::recovery {
namespace {

bool has_alive_building(const BootstrapStatus &status,
                        const std::uint8_t player) noexcept {
  return std::any_of(status.units.begin(), status.units.end(),
                     [player](const ScenarioUnitPreview &unit) {
                       return unit.alive && unit.owner == player &&
                              unit.is_building;
                     });
}

bool was_present_at_start(const GameDialogRuntime &dialog,
                          const std::uint32_t unit_id) noexcept {
  return std::find(dialog.initial_unit_ids.begin(),
                   dialog.initial_unit_ids.end(),
                   unit_id) != dialog.initial_unit_ids.end();
}

std::string player_name(const RecoveryWindowState &state,
                        const std::size_t player) {
  if (player < state.glue.lobby_slots.size() &&
      !state.glue.lobby_slots[player].name.empty()) {
    return state.glue.lobby_slots[player].name;
  }
  return player == 0U ? "Player" : "Computer " + std::to_string(player);
}

} // namespace

void begin_match_flow(RecoveryWindowState &state) noexcept {
  GameDialogRuntime &dialog = state.game_dialog;
  dialog.screen = GameDialogScreen::none;
  dialog.outcome = MatchOutcome::none;
  dialog.score_category = ScoreCategory::overview;
  dialog.hovered_control = -1;
  dialog.pressed_control = -1;
  dialog.score_started_tick = 0U;
  dialog.result_started_tick = 0U;
  dialog.initial_minerals = state.status == nullptr
                                ? 0U
                                : state.status->player_minerals;
  dialog.initial_gas =
      state.status == nullptr ? 0U : state.status->player_gas;
  dialog.score_rows.clear();
  dialog.initial_unit_ids.clear();
  dialog.paused = false;
  dialog.observer_mode = false;
  dialog.match_active = dialog.assets_ready && state.status != nullptr &&
                        state.status->assets_ready;
  if (state.status != nullptr) {
    try {
      dialog.initial_unit_ids.reserve(state.status->units.size());
      for (const ScenarioUnitPreview &unit : state.status->units) {
        dialog.initial_unit_ids.push_back(unit.unit_id);
      }
    } catch (...) {
      dialog.initial_unit_ids.clear();
    }
  }
}

void evaluate_melee_outcome(RecoveryWindowState &state) noexcept {
  GameDialogRuntime &dialog = state.game_dialog;
  BootstrapStatus *const status = state.status;
  if (!dialog.match_active || dialog.outcome != MatchOutcome::none ||
      dialog.observer_mode || status == nullptr) {
    return;
  }

  // triggers\Melee.trg evaluates the local Command condition first: zero
  // buildings dispatches Defeat.  Its following Opponents condition dispatches
  // Victory after no configured opponent retains a building.
  if (!has_alive_building(*status, 0U)) {
    show_match_outcome(state, MatchOutcome::defeat);
    return;
  }
  bool opponent_remains{};
  const auto &players = status->scenario.players();
  for (std::size_t player = 1U;
       player < 8U && player < players.size(); ++player) {
    if (players[player].ownership != 0U &&
        has_alive_building(*status, static_cast<std::uint8_t>(player))) {
      opponent_remains = true;
      break;
    }
  }
  if (!opponent_remains) {
    show_match_outcome(state, MatchOutcome::victory);
  }
}

void build_match_scores(RecoveryWindowState &state) noexcept {
  GameDialogRuntime &dialog = state.game_dialog;
  const BootstrapStatus *const status = state.status;
  dialog.score_rows.clear();
  if (status == nullptr) {
    return;
  }

  std::array<int, 8> row_for_player{};
  row_for_player.fill(-1);
  try {
    const auto &players = status->scenario.players();
    for (std::size_t player = 0; player < 8U; ++player) {
      const bool configured = player < players.size() &&
                              players[player].ownership != 0U;
      const bool has_units = std::any_of(
          status->units.begin(), status->units.end(),
          [player](const ScenarioUnitPreview &unit) {
            return unit.owner == player && unit.owner < 8U;
          });
      if (!configured && !has_units) {
        continue;
      }
      MatchScoreRow row{};
      row.player = static_cast<std::uint8_t>(player);
      row.race = player < players.size() ? players[player].race : 0U;
      if (row.race >= 3U) {
        row.race = 0U;
      }
      row.victorious = has_alive_building(
          *status, static_cast<std::uint8_t>(player));
      row.name = player_name(state, player);
      row_for_player[player] = static_cast<int>(dialog.score_rows.size());
      dialog.score_rows.push_back(std::move(row));
    }

    for (const ScenarioUnitPreview &unit : status->units) {
      if (unit.owner >= row_for_player.size() ||
          row_for_player[unit.owner] < 0 ||
          unit.unit_type >= status->unit_traits.size()) {
        continue;
      }
      const std::uint32_t value = status->unit_traits[unit.unit_type].score_value;
      MatchScoreRow &owner =
          dialog.score_rows[static_cast<std::size_t>(row_for_player[unit.owner])];
      if (!was_present_at_start(dialog, unit.unit_id) &&
          unit.construction_complete) {
        if (unit.is_building) {
          ++owner.built;
          owner.structure_score += value;
        } else {
          ++owner.produced;
          owner.unit_score += value;
        }
      }
      if (!unit.alive && unit.destroyed_by_owner < row_for_player.size() &&
          unit.destroyed_by_owner != unit.owner &&
          row_for_player[unit.destroyed_by_owner] >= 0) {
        MatchScoreRow &killer = dialog.score_rows[static_cast<std::size_t>(
            row_for_player[unit.destroyed_by_owner])];
        if (unit.is_building) {
          ++owner.structures_lost;
          ++killer.razed;
          killer.structure_score += value;
        } else {
          ++owner.lost;
          ++killer.killed;
          killer.unit_score += value;
        }
      }
    }

    for (MatchScoreRow &row : dialog.score_rows) {
      if (row.player < status->minerals_gathered.size()) {
        row.minerals = status->minerals_gathered[row.player];
        row.gas = status->gas_gathered[row.player];
      }
      // Melee initializes each active player with the same local starting
      // resources.  The recovered simulation only spends player zero's pool;
      // preserve the other rows at their starting totals until CPU economy is
      // recovered.
      row.minerals += dialog.initial_minerals;
      row.gas += dialog.initial_gas;
      row.resource_score = row.minerals + row.gas;
      if (row.player == 0U) {
        const std::uint32_t remaining =
            status->player_minerals + status->player_gas;
        row.resources_spent = row.resource_score > remaining
                                  ? row.resource_score - remaining
                                  : 0U;
      }
      row.total = row.unit_score + row.structure_score + row.resource_score;
    }
    std::stable_sort(dialog.score_rows.begin(), dialog.score_rows.end(),
                     [](const MatchScoreRow &left,
                        const MatchScoreRow &right) {
                       return left.total > right.total;
                     });
  } catch (...) {
    dialog.score_rows.clear();
  }
}

} // namespace starcraft::recovery
