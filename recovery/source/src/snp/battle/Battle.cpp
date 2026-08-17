#include "Battle.hpp"

namespace starcraft::recovery::battle {

bool SnpQuery(const std::uint32_t index,
              SnpDescriptor &descriptor) noexcept {
  if (index != 0U) {
    return false;
  }
  // The original export returns the Battle.net provider descriptor at index
  // zero. This linked recovery module exposes the same discovery shape.
  descriptor.identifier = 0x54454E42U; // 'BNET' in the original x86 order.
  descriptor.description = "Battle.net (Recovery Service)";
  descriptor.requirements = "TCP/IP";
  descriptor.capabilities = 1U;
  return true;
}

bool SnpBind(const std::uint32_t index) noexcept {
  return index == 0U;
}

bool UiInitialize(BattleRuntime &runtime) noexcept {
  runtime.screen = BattleScreen::connecting;
  runtime.edit_control = EditControl::none;
  runtime.hovered_control = -1;
  runtime.pressed_control = -1;
  runtime.pending_game_lobby = false;
  runtime.connect_pending = false;
  runtime.connect_artwork_presented = false;
  return SrvInitialize(runtime);
}

void UiDestroy(BattleRuntime &runtime) noexcept {
  SrvDestroy(runtime);
}

} // namespace starcraft::recovery::battle
