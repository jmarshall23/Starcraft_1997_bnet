#include "Battle.hpp"

#include <algorithm>
#include <cctype>

namespace starcraft::recovery::battle {
namespace {

bool valid_account_name(const std::string_view name) noexcept {
  if (name.size() < 2U || name.size() > 15U) {
    return false;
  }
  return std::all_of(name.begin(), name.end(), [](const unsigned char value) {
    return std::isalnum(value) != 0 || value == '_' || value == '-' ||
           value == '[' || value == ']';
  });
}

bool credentials_ready(BattleRuntime &runtime) noexcept {
  if (!valid_account_name(runtime.account_name)) {
    runtime.status =
        "Account names must be 2-15 letters, numbers, or _-[] characters.";
    return false;
  }
  if (runtime.password.size() < 4U || runtime.password.size() > 64U) {
    runtime.status = "Passwords must contain 4-64 characters.";
    return false;
  }
  return true;
}

} // namespace

bool UiLogon(BattleRuntime &runtime) noexcept {
  runtime.screen = BattleScreen::logon;
  runtime.edit_control = EditControl::account_name;
  runtime.hovered_control = -1;
  runtime.pressed_control = -1;
  return true;
}

bool LogonAttempt(BattleRuntime &runtime) noexcept {
  if (!credentials_ready(runtime)) {
    return false;
  }
  runtime.status = "Logging on...";
  return SrvLogon(runtime, runtime.account_name, runtime.password);
}

} // namespace starcraft::recovery::battle
