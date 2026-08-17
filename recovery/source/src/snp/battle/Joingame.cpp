#include "Battle.hpp"

namespace starcraft::recovery::battle {

bool UpdateGameList(BattleRuntime &runtime) noexcept {
  runtime.status = "Refreshing advertised games...";
  return SrvGetGameList(runtime);
}

bool DoJoinGame(BattleRuntime &runtime) noexcept {
  runtime.screen = BattleScreen::join_game;
  runtime.edit_control = EditControl::game_name;
  runtime.hovered_control = -1;
  runtime.pressed_control = -1;
  return UpdateGameList(runtime);
}

bool DoJoin(BattleRuntime &runtime) noexcept {
  if (runtime.selected_game >= runtime.games.size()) {
    runtime.status = "Select a game first.";
    return false;
  }
  runtime.status = "Joining advertised game...";
  return SrvNotifyJoin(runtime,
                       runtime.games[runtime.selected_game].identifier);
}

} // namespace starcraft::recovery::battle
