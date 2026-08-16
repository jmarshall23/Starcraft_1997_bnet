#include "../platform/bootstrap_runtime.hpp"

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
  if (!play_result_music(state, outcome) && state.music_playing) {
    alSourceStop(state.music_source);
    state.music_playing = false;
  }
}

} // namespace starcraft::recovery
