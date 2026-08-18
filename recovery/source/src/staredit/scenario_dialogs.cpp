#include "scenario_dialogs.hpp"

#include "document.hpp"
#include "resource.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <new>
#include <string>

namespace staredit {
namespace {

struct PlayerSettingsState {
  EditorDocument* document{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> races{};
  int selected{};
  bool accepted{};
};

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

}  // namespace staredit
