#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>

namespace starcraft::recovery {

void show_match_outcome(RecoveryWindowState &state,
                        const MatchOutcome outcome) noexcept {
  if (outcome == MatchOutcome::none || !state.game_dialog.assets_ready) {
    return;
  }
  GameDialogRuntime &dialog = state.game_dialog;
  dialog.outcome = outcome;
  dialog.screen = outcome == MatchOutcome::victory
                      ? GameDialogScreen::victory
                      : GameDialogScreen::defeat;
  dialog.hovered_control = -1;
  dialog.pressed_control = -1;
  dialog.paused = true;
  dialog.result_started_tick = GetTickCount();
  battle::BattleRuntime &network = state.glue.battle_net;
  if (state.glue.online_lobby && network.game_started &&
      !network.game_result_reported) {
    std::uint8_t winner = network.local_player_slot;
    bool winner_known = outcome == MatchOutcome::victory;
    if (outcome == MatchOutcome::defeat) {
      const auto opponent = std::find_if(
          network.lobby_players.begin(), network.lobby_players.end(),
          [&network](const battle::LobbyPlayer &player) {
            return player.slot != network.local_player_slot;
          });
      if (opponent != network.lobby_players.end()) {
        winner = opponent->slot;
        winner_known = true;
      }
    }
    if (winner_known) {
      (void)battle::SrvReportGameResult(network, winner);
    }
  }
  if (!play_result_music(state, outcome) && state.music_playing) {
    alSourceStop(state.music_source);
    state.music_playing = false;
  }
}

} // namespace starcraft::recovery
