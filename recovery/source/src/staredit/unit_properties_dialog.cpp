#include "unit_properties_dialog.hpp"

#include "document.hpp"
#include "resource.h"

#include <array>
#include <cerrno>
#include <cstdint>
#include <cwchar>
#include <limits>
#include <string>
#include <string_view>

namespace staredit {
namespace {

struct UnitDialogState {
  EditorDocument* document{};
  UnitAttributes attributes{};
  bool accepted{};
};

void set_decimal(const HWND dialog,
                 const int control,
                 const std::uint32_t value) noexcept {
  SetDlgItemTextW(dialog, control, std::to_wstring(value).c_str());
}

void set_hex(const HWND dialog,
             const int control,
             const std::uint16_t value) noexcept {
  std::array<wchar_t, 16> text{};
  (void)swprintf_s(text.data(), text.size(), L"0x%04X", value);
  SetDlgItemTextW(dialog, control, text.data());
}

bool read_number(const HWND dialog,
                 const int control,
                 const int base,
                 const std::uint32_t maximum,
                 std::uint32_t& value) noexcept {
  std::array<wchar_t, 64> text{};
  if (GetDlgItemTextW(dialog, control, text.data(),
                      static_cast<int>(text.size())) == 0) {
    return false;
  }
  wchar_t* end{};
  errno = 0;
  const unsigned long parsed = std::wcstoul(text.data(), &end, base);
  while (end != nullptr && *end == L' ') {
    ++end;
  }
  if (errno == ERANGE || end == text.data() || end == nullptr || *end != L'\0' ||
      parsed > maximum) {
    return false;
  }
  value = static_cast<std::uint32_t>(parsed);
  return true;
}

void enable_retail_fields(const HWND dialog, const bool enabled) noexcept {
  constexpr std::array<int, 8> controls{{
      IDC_UNIT_HP, IDC_UNIT_SHIELDS, IDC_UNIT_ENERGY, IDC_UNIT_RESOURCES,
      IDC_UNIT_HANGAR, IDC_UNIT_STATE_FLAGS, IDC_UNIT_VALID_STATE,
      IDC_UNIT_VALID_DATA,
  }};
  for (const int control : controls) {
    EnableWindow(GetDlgItem(dialog, control), enabled ? TRUE : FALSE);
  }
  ShowWindow(GetDlgItem(dialog, IDC_UNIT_BETA_NOTE),
             enabled ? SW_HIDE : SW_SHOW);
}

bool initialize_dialog(const HWND dialog, UnitDialogState& state) noexcept {
  if (state.document == nullptr ||
      !state.document->unit_attributes(state.attributes.index,
                                       state.attributes)) {
    return false;
  }
  const std::string_view name = state.document->object_brush_name(
      EditorLayer::units, state.attributes.type);
  const std::wstring wide_name = name.empty()
                                     ? L"Unit " +
                                           std::to_wstring(state.attributes.type)
                                     : std::wstring{name.begin(), name.end()};
  SetDlgItemTextW(dialog, IDC_UNIT_NAME, wide_name.c_str());
  set_decimal(dialog, IDC_UNIT_TYPE, state.attributes.type);
  set_decimal(dialog, IDC_UNIT_X, state.attributes.x);
  set_decimal(dialog, IDC_UNIT_Y, state.attributes.y);
  const HWND owner = GetDlgItem(dialog, IDC_UNIT_OWNER);
  for (std::size_t player = 0U;
       player < starcraft::data::chk_player_slot_count; ++player) {
    const std::wstring label = L"Player " + std::to_wstring(player + 1U);
    const LRESULT item = SendMessageW(owner, CB_ADDSTRING, 0,
                                     reinterpret_cast<LPARAM>(label.c_str()));
    if (item != CB_ERR && item != CB_ERRSPACE) {
      SendMessageW(owner, CB_SETITEMDATA, static_cast<WPARAM>(item), player);
    }
  }
  SendMessageW(owner, CB_SETCURSEL, state.attributes.owner, 0);
  set_decimal(dialog, IDC_UNIT_HP, state.attributes.hit_points);
  set_decimal(dialog, IDC_UNIT_SHIELDS, state.attributes.shield_points);
  set_decimal(dialog, IDC_UNIT_ENERGY, state.attributes.energy);
  set_decimal(dialog, IDC_UNIT_RESOURCES, state.attributes.resource_amount);
  set_decimal(dialog, IDC_UNIT_HANGAR, state.attributes.hangar_count);
  set_hex(dialog, IDC_UNIT_STATE_FLAGS, state.attributes.state_flags);
  set_hex(dialog, IDC_UNIT_VALID_STATE, state.attributes.valid_state_flags);
  set_hex(dialog, IDC_UNIT_VALID_DATA, state.attributes.valid_data_flags);
  enable_retail_fields(dialog, state.attributes.has_retail_attributes);
  return true;
}

bool read_dialog(const HWND dialog, UnitDialogState& state) noexcept {
  std::uint32_t type{};
  std::uint32_t x{};
  std::uint32_t y{};
  if (!read_number(dialog, IDC_UNIT_TYPE, 10, 227U, type) ||
      !read_number(dialog, IDC_UNIT_X, 10,
                   static_cast<std::uint32_t>(state.document->width()) *
                           EditorDocument::tile_width -
                       1U,
                   x) ||
      !read_number(dialog, IDC_UNIT_Y, 10,
                   static_cast<std::uint32_t>(state.document->height()) *
                           EditorDocument::tile_height -
                       1U,
                   y)) {
    return false;
  }
  const LRESULT selected =
      SendDlgItemMessageW(dialog, IDC_UNIT_OWNER, CB_GETCURSEL, 0, 0);
  if (selected == CB_ERR) {
    return false;
  }
  state.attributes.type = static_cast<std::uint16_t>(type);
  state.attributes.x = static_cast<std::uint16_t>(x);
  state.attributes.y = static_cast<std::uint16_t>(y);
  state.attributes.owner = static_cast<std::uint8_t>(SendDlgItemMessageW(
      dialog, IDC_UNIT_OWNER, CB_GETITEMDATA, static_cast<WPARAM>(selected), 0));
  if (!state.attributes.has_retail_attributes) {
    return true;
  }
  std::uint32_t hp{};
  std::uint32_t shields{};
  std::uint32_t energy{};
  std::uint32_t resources{};
  std::uint32_t hangar{};
  std::uint32_t state_flags{};
  std::uint32_t valid_state{};
  std::uint32_t valid_data{};
  if (!read_number(dialog, IDC_UNIT_HP, 10, 100U, hp) ||
      !read_number(dialog, IDC_UNIT_SHIELDS, 10, 100U, shields) ||
      !read_number(dialog, IDC_UNIT_ENERGY, 10, 100U, energy) ||
      !read_number(dialog, IDC_UNIT_RESOURCES, 10,
                   (std::numeric_limits<std::uint32_t>::max)(), resources) ||
      !read_number(dialog, IDC_UNIT_HANGAR, 10, 65535U, hangar) ||
      !read_number(dialog, IDC_UNIT_STATE_FLAGS, 0, 65535U, state_flags) ||
      !read_number(dialog, IDC_UNIT_VALID_STATE, 0, 65535U, valid_state) ||
      !read_number(dialog, IDC_UNIT_VALID_DATA, 0, 65535U, valid_data)) {
    return false;
  }
  state.attributes.hit_points = static_cast<std::uint8_t>(hp);
  state.attributes.shield_points = static_cast<std::uint8_t>(shields);
  state.attributes.energy = static_cast<std::uint8_t>(energy);
  state.attributes.resource_amount = resources;
  state.attributes.hangar_count = static_cast<std::uint16_t>(hangar);
  state.attributes.state_flags = static_cast<std::uint16_t>(state_flags);
  state.attributes.valid_state_flags = static_cast<std::uint16_t>(valid_state);
  state.attributes.valid_data_flags = static_cast<std::uint16_t>(valid_data);
  return true;
}

INT_PTR CALLBACK unit_dialog_proc(const HWND dialog,
                                  const UINT message,
                                  const WPARAM wparam,
                                  const LPARAM lparam) noexcept {
  auto* state = reinterpret_cast<UnitDialogState*>(
      GetWindowLongPtrW(dialog, DWLP_USER));
  if (message == WM_INITDIALOG) {
    state = reinterpret_cast<UnitDialogState*>(lparam);
    if (state == nullptr) {
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    SetWindowLongPtrW(dialog, DWLP_USER, reinterpret_cast<LONG_PTR>(state));
    if (!initialize_dialog(dialog, *state)) {
      EndDialog(dialog, IDCANCEL);
      return TRUE;
    }
    return TRUE;
  }
  if (message != WM_COMMAND || state == nullptr) {
    return FALSE;
  }
  if (LOWORD(wparam) == IDOK) {
    if (!read_dialog(dialog, *state)) {
      MessageBoxW(dialog,
                  L"Enter valid unit type, player, position, and attribute values.",
                  L"Unit Properties", MB_OK | MB_ICONWARNING);
      return TRUE;
    }
    if (!state->document->update_unit(state->attributes.index,
                                      state->attributes)) {
      MessageBoxW(dialog, L"The unit record could not be updated.",
                  L"Unit Properties", MB_OK | MB_ICONERROR);
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

bool show_unit_properties_dialog(const HWND parent,
                                 const HINSTANCE instance,
                                 EditorDocument& document,
                                 const std::size_t unit_index) noexcept {
  UnitDialogState state{};
  state.document = &document;
  state.attributes.index = unit_index;
  const INT_PTR result = DialogBoxParamW(
      instance, MAKEINTRESOURCEW(IDD_UNIT_PROPERTIES), parent,
      unit_dialog_proc, reinterpret_cast<LPARAM>(&state));
  return result == IDOK && state.accepted;
}

}  // namespace staredit
