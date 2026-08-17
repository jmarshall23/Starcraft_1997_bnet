#include "Battle.hpp"

namespace starcraft::recovery::battle {

bool ChatSelectChannel(BattleRuntime &runtime,
                       const std::string_view channel) noexcept {
  if (channel.empty()) {
    runtime.status = "Select a channel first.";
    return false;
  }
  runtime.status = "Joining channel...";
  return SrvJoinChannel(runtime, channel);
}

void ChatChannelJoined(BattleRuntime &runtime,
                       const std::string_view channel) noexcept {
  runtime.current_channel.assign(channel);
  runtime.screen = BattleScreen::chat_room;
  runtime.edit_control = EditControl::chat_input;
  runtime.chat_lines.clear();
  runtime.status = "Joined " + runtime.current_channel + ".";
}

} // namespace starcraft::recovery::battle
