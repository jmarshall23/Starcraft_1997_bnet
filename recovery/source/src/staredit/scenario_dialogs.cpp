#include "scenario_dialogs.hpp"

#include "document.hpp"
#include "resource.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <string>
#include <string_view>
#include <vector>

namespace staredit {
namespace {

struct PlayerSettingsState {
  EditorDocument* document{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> races{};
  int selected{};
  bool accepted{};
};

struct ScenarioPropertiesState {
  EditorDocument* document{};
  bool accepted{};
};

struct ForcesState {
  EditorDocument* document{};
  ScenarioForces forces{};
  int selected_player{};
  int selected_force{};
  bool accepted{};
};

std::wstring widen_chk_string(const std::string_view value) noexcept {
  if (value.empty()) {
    return {};
  }
  if (value.size() > static_cast<std::size_t>(INT_MAX)) {
    return {};
  }
  try {
    const int required = MultiByteToWideChar(
        CP_ACP, 0, value.data(), static_cast<int>(value.size()), nullptr, 0);
    if (required <= 0) {
      return {};
    }
    std::wstring result(static_cast<std::size_t>(required), L'\0');
    if (MultiByteToWideChar(CP_ACP, 0, value.data(),
                            static_cast<int>(value.size()), result.data(),
                            required) != required) {
      return {};
    }
    return result;
  } catch (...) {
    return {};
  }
}

bool read_chk_string(const HWND dialog,
                     const int control,
                     std::string& value) noexcept {
  value.clear();
  const HWND item = GetDlgItem(dialog, control);
  const int length = item == nullptr ? -1 : GetWindowTextLengthW(item);
  if (length < 0) {
    return false;
  }
  try {
    std::vector<wchar_t> wide(static_cast<std::size_t>(length) + 1U, L'\0');
    if (GetWindowTextW(item, wide.data(), length + 1) < 0) {
      return false;
    }
    if (length == 0) {
      return true;
    }
    const int required = WideCharToMultiByte(
        CP_ACP, 0, wide.data(), length, nullptr, 0, nullptr, nullptr);
    if (required <= 0) {
      return false;
    }
    value.resize(static_cast<std::size_t>(required));
    return WideCharToMultiByte(CP_ACP, 0, wide.data(), length, value.data(),
                               required, nullptr, nullptr) == required;
  } catch (...) {
    value.clear();
    return false;
  }
}

void add_combo_item(const HWND combo,
                    const wchar_t* const text,
                    const std::uint8_t value) noexcept {
  const LRESULT index = SendMessageW(combo, CB_ADDSTRING, 0,
                                    reinterpret_cast<LPARAM>(text));
  if (index != CB_ERR && index != CB_ERRSPACE) {
    SendMessageW(combo, CB_SETITEMDATA, static_cast<WPARAM>(index), value);
  }
}

int combo_index_for_value(const HWND combo, const std::uint8_t value) noexcept {
  const int count = static_cast<int>(SendMessageW(combo, CB_GETCOUNT, 0, 0));
  for (int index = 0; index < count; ++index) {
    if (static_cast<std::uint8_t>(SendMessageW(combo, CB_GETITEMDATA, index,
                                               0)) == value) {
      return index;
    }
  }
  return 0;
}

void load_selected_player(const HWND dialog,
                          const PlayerSettingsState& state) noexcept {
  const std::size_t player = static_cast<std::size_t>(state.selected);
  SendDlgItemMessageW(
      dialog, IDC_PLAYER_OWNERSHIP, CB_SETCURSEL,
      combo_index_for_value(GetDlgItem(dialog, IDC_PLAYER_OWNERSHIP),
                            state.ownership[player]),
      0);
  SendDlgItemMessageW(
      dialog, IDC_PLAYER_RACE, CB_SETCURSEL,
      combo_index_for_value(GetDlgItem(dialog, IDC_PLAYER_RACE),
                            state.races[player]),
      0);
}

void save_selected_player(const HWND dialog,
                          PlayerSettingsState& state) noexcept {
  if (state.selected < 0 ||
      state.selected >=
          static_cast<int>(starcraft::data::chk_player_slot_count)) {
    return;
  }
  const auto read = [dialog](const int control) {
    const LRESULT selected = SendDlgItemMessageW(dialog, control, CB_GETCURSEL,
                                                 0, 0);
    return selected == CB_ERR
               ? static_cast<std::uint8_t>(0U)
               : static_cast<std::uint8_t>(SendDlgItemMessageW(
                     dialog, control, CB_GETITEMDATA,
                     static_cast<WPARAM>(selected), 0));
  };
  const std::size_t player = static_cast<std::size_t>(state.selected);
  state.ownership[player] = read(IDC_PLAYER_OWNERSHIP);
  state.races[player] = read(IDC_PLAYER_RACE);
}

INT_PTR CALLBACK player_settings_proc(const HWND dialog,
                                      const UINT message,
                                      const WPARAM wparam,
                                      const LPARAM lparam) noexcept {
  auto* state = reinterpret_cast<PlayerSettingsState*>(
      GetWindowLongPtrW(dialog, DWLP_USER));
  if (message == WM_INITDIALOG) {
    state = reinterpret_cast<PlayerSettingsState*>(lparam);
    if (state == nullptr || state->document == nullptr ||
        !state->document->player_settings(state->ownership, state->races)) {
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    SetWindowLongPtrW(dialog, DWLP_USER, reinterpret_cast<LONG_PTR>(state));
    for (std::size_t player = 0U;
         player < starcraft::data::chk_player_slot_count; ++player) {
      const std::wstring label = L"Player " + std::to_wstring(player + 1U);
      SendDlgItemMessageW(dialog, IDC_PLAYER_LIST, LB_ADDSTRING, 0,
                          reinterpret_cast<LPARAM>(label.c_str()));
    }
    const HWND ownership = GetDlgItem(dialog, IDC_PLAYER_OWNERSHIP);
    add_combo_item(ownership, L"Inactive", 0U);
    add_combo_item(ownership, L"Computer (slot)", 1U);
    add_combo_item(ownership, L"Human (slot)", 2U);
    add_combo_item(ownership, L"Rescue Passive", 3U);
    add_combo_item(ownership, L"Unused", 4U);
    add_combo_item(ownership, L"Computer", 5U);
    add_combo_item(ownership, L"Human", 6U);
    add_combo_item(ownership, L"Neutral", 7U);
    add_combo_item(ownership, L"Closed", 8U);
    const HWND race = GetDlgItem(dialog, IDC_PLAYER_RACE);
    add_combo_item(race, L"Zerg", 0U);
    add_combo_item(race, L"Terran", 1U);
    add_combo_item(race, L"Protoss", 2U);
    add_combo_item(race, L"Independent", 3U);
    add_combo_item(race, L"Neutral", 4U);
    add_combo_item(race, L"User Selectable", 5U);
    add_combo_item(race, L"Inactive", 7U);
    state->selected = 0;
    SendDlgItemMessageW(dialog, IDC_PLAYER_LIST, LB_SETCURSEL, 0, 0);
    load_selected_player(dialog, *state);
    return TRUE;
  }
  if (message != WM_COMMAND || state == nullptr) {
    return FALSE;
  }
  if (LOWORD(wparam) == IDC_PLAYER_LIST && HIWORD(wparam) == LBN_SELCHANGE) {
    save_selected_player(dialog, *state);
    state->selected = static_cast<int>(SendDlgItemMessageW(
        dialog, IDC_PLAYER_LIST, LB_GETCURSEL, 0, 0));
    load_selected_player(dialog, *state);
    return TRUE;
  }
  if (LOWORD(wparam) == IDOK) {
    save_selected_player(dialog, *state);
    if (!state->document->set_player_settings(state->ownership, state->races)) {
      MessageBoxW(dialog, L"The player settings could not be written to CHK.",
                  L"Player Settings", MB_OK | MB_ICONERROR);
      return TRUE;
    }
    state->accepted = true;
    EndDialog(dialog, IDOK);
    return TRUE;
  }
  if (LOWORD(wparam) == IDCANCEL) {
    EndDialog(dialog, IDCANCEL);
    return TRUE;
  }
  return FALSE;
}

INT_PTR CALLBACK scenario_properties_proc(const HWND dialog,
                                          const UINT message,
                                          const WPARAM wparam,
                                          const LPARAM lparam) noexcept {
  auto* state = reinterpret_cast<ScenarioPropertiesState*>(
      GetWindowLongPtrW(dialog, DWLP_USER));
  if (message == WM_INITDIALOG) {
    state = reinterpret_cast<ScenarioPropertiesState*>(lparam);
    if (state == nullptr || state->document == nullptr) {
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    ScenarioProperties properties{};
    if (!state->document->scenario_properties(properties)) {
      MessageBoxW(dialog, L"The SPRP or STR section is not readable.",
                  L"Scenario Properties", MB_OK | MB_ICONERROR);
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    SetWindowLongPtrW(dialog, DWLP_USER, reinterpret_cast<LONG_PTR>(state));
    const std::wstring name = widen_chk_string(properties.name);
    const std::wstring description = widen_chk_string(properties.description);
    SetDlgItemTextW(dialog, IDC_SCENARIO_NAME, name.c_str());
    SetDlgItemTextW(dialog, IDC_SCENARIO_DESCRIPTION, description.c_str());
    SendDlgItemMessageW(dialog, IDC_SCENARIO_NAME, EM_SETLIMITTEXT, 255U, 0);
    SendDlgItemMessageW(dialog, IDC_SCENARIO_DESCRIPTION, EM_SETLIMITTEXT,
                        4095U, 0);
    return TRUE;
  }
  if (message != WM_COMMAND || state == nullptr) {
    return FALSE;
  }
  if (LOWORD(wparam) == IDOK) {
    ScenarioProperties properties{};
    if (!read_chk_string(dialog, IDC_SCENARIO_NAME, properties.name) ||
        !read_chk_string(dialog, IDC_SCENARIO_DESCRIPTION,
                         properties.description) ||
        !state->document->set_scenario_properties(properties)) {
      MessageBoxW(dialog,
                  L"The scenario name or description could not be written "
                  L"to SPRP/STR.",
                  L"Scenario Properties", MB_OK | MB_ICONERROR);
      return TRUE;
    }
    state->accepted = true;
    EndDialog(dialog, IDOK);
    return TRUE;
  }
  if (LOWORD(wparam) == IDCANCEL) {
    EndDialog(dialog, IDCANCEL);
    return TRUE;
  }
  return FALSE;
}

void load_selected_force_player(const HWND dialog,
                                const ForcesState& state) noexcept {
  if (state.selected_player < 0 ||
      state.selected_player >=
          static_cast<int>(formats::force_player_count)) {
    return;
  }
  SendDlgItemMessageW(
      dialog, IDC_FORCE_ASSIGNMENT, CB_SETCURSEL,
      state.forces.player_force[static_cast<std::size_t>(
          state.selected_player)],
      0);
}

void save_selected_force_player(const HWND dialog,
                                ForcesState& state) noexcept {
  if (state.selected_player < 0 ||
      state.selected_player >=
          static_cast<int>(formats::force_player_count)) {
    return;
  }
  const LRESULT selection = SendDlgItemMessageW(
      dialog, IDC_FORCE_ASSIGNMENT, CB_GETCURSEL, 0, 0);
  if (selection != CB_ERR) {
    state.forces.player_force[static_cast<std::size_t>(
        state.selected_player)] = static_cast<std::uint8_t>(selection);
  }
}

void load_selected_force(const HWND dialog,
                         const ForcesState& state) noexcept {
  if (state.selected_force < 0 ||
      state.selected_force >= static_cast<int>(formats::force_count)) {
    return;
  }
  const std::size_t force = static_cast<std::size_t>(state.selected_force);
  const std::wstring name = widen_chk_string(state.forces.names[force]);
  SetDlgItemTextW(dialog, IDC_FORCE_NAME, name.c_str());
  const auto check = [dialog, &state, force](const int control,
                                             const std::uint8_t flag) {
    CheckDlgButton(dialog, control,
                   (state.forces.flags[force] & flag) != 0U ? BST_CHECKED
                                                            : BST_UNCHECKED);
  };
  check(IDC_FORCE_RANDOM_START, formats::force_flag_random_start);
  check(IDC_FORCE_ALLIES, formats::force_flag_allies);
  check(IDC_FORCE_ALLIED_VICTORY, formats::force_flag_allied_victory);
  check(IDC_FORCE_SHARED_VISION, formats::force_flag_shared_vision);
}

bool save_selected_force(const HWND dialog, ForcesState& state) noexcept {
  if (state.selected_force < 0 ||
      state.selected_force >= static_cast<int>(formats::force_count)) {
    return true;
  }
  const std::size_t force = static_cast<std::size_t>(state.selected_force);
  if (!read_chk_string(dialog, IDC_FORCE_NAME, state.forces.names[force])) {
    return false;
  }
  if (!state.forces.supports_flags) {
    return true;
  }
  const auto checked = [dialog](const int control) {
    return IsDlgButtonChecked(dialog, control) == BST_CHECKED;
  };
  std::uint8_t flags = state.forces.flags[force];
  flags &= static_cast<std::uint8_t>(
      ~(formats::force_flag_random_start | formats::force_flag_allies |
        formats::force_flag_allied_victory |
        formats::force_flag_shared_vision));
  flags |= checked(IDC_FORCE_RANDOM_START)
               ? formats::force_flag_random_start
               : 0U;
  flags |= checked(IDC_FORCE_ALLIES) ? formats::force_flag_allies : 0U;
  flags |= checked(IDC_FORCE_ALLIED_VICTORY)
               ? formats::force_flag_allied_victory
               : 0U;
  flags |= checked(IDC_FORCE_SHARED_VISION)
               ? formats::force_flag_shared_vision
               : 0U;
  state.forces.flags[force] = flags;
  return true;
}

INT_PTR CALLBACK forces_proc(const HWND dialog,
                             const UINT message,
                             const WPARAM wparam,
                             const LPARAM lparam) noexcept {
  auto* state =
      reinterpret_cast<ForcesState*>(GetWindowLongPtrW(dialog, DWLP_USER));
  if (message == WM_INITDIALOG) {
    state = reinterpret_cast<ForcesState*>(lparam);
    if (state == nullptr || state->document == nullptr ||
        !state->document->scenario_forces(state->forces)) {
      MessageBoxW(dialog, L"The FORC or STR section is not readable.",
                  L"Forces", MB_OK | MB_ICONERROR);
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    SetWindowLongPtrW(dialog, DWLP_USER, reinterpret_cast<LONG_PTR>(state));
    for (std::size_t player = 0U; player < formats::force_player_count;
         ++player) {
      const std::wstring label = L"Player " + std::to_wstring(player + 1U);
      SendDlgItemMessageW(dialog, IDC_FORCE_PLAYER_LIST, LB_ADDSTRING, 0,
                          reinterpret_cast<LPARAM>(label.c_str()));
    }
    for (std::size_t force = 0U; force < formats::force_count; ++force) {
      const std::wstring label = L"Force " + std::to_wstring(force + 1U);
      SendDlgItemMessageW(dialog, IDC_FORCE_LIST, LB_ADDSTRING, 0,
                          reinterpret_cast<LPARAM>(label.c_str()));
      SendDlgItemMessageW(dialog, IDC_FORCE_ASSIGNMENT, CB_ADDSTRING, 0,
                          reinterpret_cast<LPARAM>(label.c_str()));
    }
    SendDlgItemMessageW(dialog, IDC_FORCE_NAME, EM_SETLIMITTEXT, 255U, 0);
    state->selected_player = 0;
    state->selected_force = 0;
    SendDlgItemMessageW(dialog, IDC_FORCE_PLAYER_LIST, LB_SETCURSEL, 0, 0);
    SendDlgItemMessageW(dialog, IDC_FORCE_LIST, LB_SETCURSEL, 0, 0);
    load_selected_force_player(dialog, *state);
    load_selected_force(dialog, *state);
    for (const int control : {IDC_FORCE_RANDOM_START, IDC_FORCE_ALLIES,
                              IDC_FORCE_ALLIED_VICTORY,
                              IDC_FORCE_SHARED_VISION}) {
      EnableWindow(GetDlgItem(dialog, control),
                   state->forces.supports_flags ? TRUE : FALSE);
    }
    ShowWindow(GetDlgItem(dialog, IDC_FORCE_BETA_NOTE),
               state->forces.supports_flags ? SW_HIDE : SW_SHOW);
    return TRUE;
  }
  if (message != WM_COMMAND || state == nullptr) {
    return FALSE;
  }
  if (LOWORD(wparam) == IDC_FORCE_PLAYER_LIST &&
      HIWORD(wparam) == LBN_SELCHANGE) {
    save_selected_force_player(dialog, *state);
    state->selected_player = static_cast<int>(SendDlgItemMessageW(
        dialog, IDC_FORCE_PLAYER_LIST, LB_GETCURSEL, 0, 0));
    load_selected_force_player(dialog, *state);
    return TRUE;
  }
  if (LOWORD(wparam) == IDC_FORCE_LIST && HIWORD(wparam) == LBN_SELCHANGE) {
    if (!save_selected_force(dialog, *state)) {
      return TRUE;
    }
    state->selected_force = static_cast<int>(SendDlgItemMessageW(
        dialog, IDC_FORCE_LIST, LB_GETCURSEL, 0, 0));
    load_selected_force(dialog, *state);
    return TRUE;
  }
  if (LOWORD(wparam) == IDOK) {
    save_selected_force_player(dialog, *state);
    if (!save_selected_force(dialog, *state) ||
        !state->document->set_scenario_forces(state->forces)) {
      MessageBoxW(dialog,
                  L"The force assignments, names, or flags could not be "
                  L"written to FORC/STR.",
                  L"Forces", MB_OK | MB_ICONERROR);
      return TRUE;
    }
    state->accepted = true;
    EndDialog(dialog, IDOK);
    return TRUE;
  }
  if (LOWORD(wparam) == IDCANCEL) {
    EndDialog(dialog, IDCANCEL);
    return TRUE;
  }
  return FALSE;
}

}  // namespace

bool show_player_settings_dialog(const HWND parent,
                                 const HINSTANCE instance,
                                 EditorDocument& document) noexcept {
  PlayerSettingsState state{&document};
  const INT_PTR result = DialogBoxParamW(
      instance, MAKEINTRESOURCEW(IDD_PLAYER_SETTINGS), parent,
      player_settings_proc, reinterpret_cast<LPARAM>(&state));
  return result == IDOK && state.accepted;
}

bool show_scenario_properties_dialog(const HWND parent,
                                     const HINSTANCE instance,
                                     EditorDocument& document) noexcept {
  ScenarioPropertiesState state{&document};
  const INT_PTR result = DialogBoxParamW(
      instance, MAKEINTRESOURCEW(IDD_SCENARIO_PROPERTIES), parent,
      scenario_properties_proc, reinterpret_cast<LPARAM>(&state));
  return result == IDOK && state.accepted;
}

bool show_forces_dialog(const HWND parent,
                        const HINSTANCE instance,
                        EditorDocument& document) noexcept {
  ForcesState state{&document};
  const INT_PTR result = DialogBoxParamW(
      instance, MAKEINTRESOURCEW(IDD_FORCES), parent, forces_proc,
      reinterpret_cast<LPARAM>(&state));
  return result == IDOK && state.accepted;
}

}  // namespace staredit
