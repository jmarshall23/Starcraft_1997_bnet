#include "Battle.hpp"

#include <algorithm>

namespace starcraft::recovery::battle {

void ChatAddUser(BattleRuntime &runtime, const std::string_view user) noexcept {
  if (user.empty() || std::find(runtime.users.begin(), runtime.users.end(), user) !=
                          runtime.users.end()) {
    return;
  }
  try {
    runtime.users.emplace_back(user);
    std::sort(runtime.users.begin(), runtime.users.end());
  } catch (...) {
  }
}

void ChatDeleteUser(BattleRuntime &runtime,
                    const std::string_view user) noexcept {
  const auto found = std::find(runtime.users.begin(), runtime.users.end(), user);
  if (found != runtime.users.end()) {
    runtime.users.erase(found);
  }
}

void ChatReceiveMsg(BattleRuntime &runtime, const std::string_view user,
                    const std::string_view message) noexcept {
  try {
    std::string line;
    line.reserve(user.size() + message.size() + 3U);
    line.push_back('<');
    line.append(user);
    line += "> ";
    line.append(message);
    runtime.chat_lines.push_back(std::move(line));
    if (runtime.chat_lines.size() > 100U) {
      runtime.chat_lines.erase(runtime.chat_lines.begin(),
                               runtime.chat_lines.begin() +
                                   static_cast<std::ptrdiff_t>(
                                       runtime.chat_lines.size() - 100U));
    }
  } catch (...) {
  }
}

bool SendChatMsg(BattleRuntime &runtime) noexcept {
  if (runtime.chat_input.empty()) {
    return false;
  }
  const std::string message = runtime.chat_input;
  if (!SrvSendChatString(runtime, message)) {
    return false;
  }
  runtime.chat_input.clear();
  return true;
}

bool DoCreateGame(BattleRuntime &runtime,
                  const std::string_view map_name) noexcept {
  runtime.screen = BattleScreen::create_game;
  runtime.edit_control = EditControl::game_name;
  runtime.hovered_control = -1;
  runtime.pressed_control = -1;
  runtime.selected_map_name.assign(map_name);
  for (std::size_t index = 0U; index < runtime.available_maps.size(); ++index) {
    if (runtime.available_maps[index].name == map_name) {
      runtime.selected_map = index;
      break;
    }
  }
  runtime.status.clear();
  return true;
}

bool CreateGameAttempt(BattleRuntime &runtime) noexcept {
  if (runtime.game_name.empty()) {
    runtime.status = "Enter a game name.";
    return false;
  }
  if (runtime.selected_map_name.empty()) {
    runtime.status = "Select a multiplayer map.";
    return false;
  }
  const std::uint32_t maximum_players =
      runtime.selected_map < runtime.available_maps.size()
          ? (std::max)(1U,
                       runtime.available_maps[runtime.selected_map].players)
          : 8U;
  runtime.status = "Creating advertised game...";
  return SrvStartAdvertisingGame(runtime, runtime.game_name,
                                 runtime.selected_map_name, maximum_players);
}

} // namespace starcraft::recovery::battle
