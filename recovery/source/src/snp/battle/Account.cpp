#include "Battle.hpp"

#include <algorithm>
#include <cctype>

namespace starcraft::recovery::battle {
namespace {

bool valid_new_account(BattleRuntime &runtime) noexcept {
  const bool valid_name = runtime.account_name.size() >= 2U &&
                          runtime.account_name.size() <= 15U &&
                          std::all_of(runtime.account_name.begin(),
                                      runtime.account_name.end(),
                                      [](const unsigned char value) {
    return std::isalnum(value) != 0 || value == '_' || value == '-' ||
           value == '[' || value == ']';
  });
  if (!valid_name) {
    runtime.status =
        "Account names must be 2-15 letters, numbers, or _-[] characters.";
    return false;
  }
  if (runtime.password.size() < 4U || runtime.password.size() > 64U) {
    runtime.status = "Passwords must contain 4-64 characters.";
    return false;
  }
  if (runtime.password != runtime.confirm_password) {
    runtime.status = "The passwords do not match.";
    runtime.password.clear();
    runtime.confirm_password.clear();
    runtime.edit_control = EditControl::password;
    return false;
  }
  return true;
}

} // namespace

bool NewAccount(BattleRuntime &runtime) noexcept {
  if (runtime.screen != BattleScreen::new_account) {
    runtime.screen = BattleScreen::new_account;
    runtime.edit_control = EditControl::account_name;
    runtime.password.clear();
    runtime.confirm_password.clear();
    runtime.status.clear();
    return true;
  }
  if (!valid_new_account(runtime)) {
    return false;
  }
  runtime.status = "Creating account...";
  return SrvCreateAccount(runtime, runtime.account_name, runtime.password);
}

} // namespace starcraft::recovery::battle
