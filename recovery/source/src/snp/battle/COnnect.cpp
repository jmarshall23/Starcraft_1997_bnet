#include "Battle.hpp"

namespace starcraft::recovery::battle {

bool ConnectDialogProc(BattleRuntime &runtime) noexcept {
  // COnnect.cpp::ConnectDialogProc receives its first service notification
  // only after DIALOG_CONNECT_BG and DIALOG_CONNECT_CANCEL have painted.
  // Keep socket discovery on that side of the recovered UI boundary.
  runtime.status = "Searching for the fastest Battle.net server...";
  return SrvConnectToServer(runtime);
}

} // namespace starcraft::recovery::battle
