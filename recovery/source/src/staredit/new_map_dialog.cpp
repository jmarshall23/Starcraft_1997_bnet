#include "new_map_dialog.hpp"

#include "resource.h"

#include <algorithm>
#include <array>
#include <string>

namespace staredit {
namespace {

constexpr std::array<NewMapTileset, 5> kTilesets{{
    {0U,
     L"Badlands",
     {"Dirt", "Mud", "High Dirt", "Water", "Grass", "High Grass", "",
      "", "Structure", "Asphalt", "Rocky Ground", "", ""},
     0U},
    {1U,
     L"Space Platform",
     {"Space", "Low Platform", "Rusty Pit", "Platform", "Dark Platform",
      "Plating", "Solar Array", "High Platform", "High Plating",
      "Elevated Catwalk", "", "", ""},
     3U},
    {2U,
     L"Installation",
     {"Substructure", "Floor", "Roof", "Substructure Plating", "Plating",
      "Substructure Panels", "Bottomless Pit", "", "", "", "", "", ""},
     1U},
    {3U,
     L"Ash World",
     {"Magma", "Dirt", "Lava", "Shale", "Broken Rock", "High Dirt",
      "High Lava", "High Shale", "", "", "", "", ""},
     1U},
    {4U,
     L"Jungle World",
     {"Water", "Dirt", "Mud", "Jungle", "Rocky Ground", "Ruins",
      "Raised Jungle", "Temple", "High Dirt", "High Jungle", "High Ruins",
      "High Raised Jungle", "High Temple"},
     3U},
}};

std::wstring widen_ascii(const std::string_view value) {
  return std::wstring{value.begin(), value.end()};
}

bool is_dimension_control(const HWND control) noexcept {
  const int id = GetDlgCtrlID(control);
  return id == IDC_NEW_WIDTH || id == IDC_NEW_HEIGHT;
}

int selected_item(const HWND control) noexcept {
  return static_cast<int>(SendMessageW(
      control, is_dimension_control(control) ? CB_GETCURSEL : LB_GETCURSEL,
      0, 0));
}

std::uint16_t selected_data(const HWND control,
                            const std::uint16_t fallback) noexcept {
  const int selection = selected_item(control);
  if (selection == LB_ERR) {
    return fallback;
  }
  const LRESULT data = SendMessageW(control,
                                    is_dimension_control(control)
                                        ? CB_GETITEMDATA
                                        : LB_GETITEMDATA,
                                    static_cast<WPARAM>(selection), 0);
  return data == CB_ERR ? fallback : static_cast<std::uint16_t>(data);
}

void select_data(const HWND control, const std::uint16_t wanted) noexcept {
  const bool combo = is_dimension_control(control);
  const int count = static_cast<int>(SendMessageW(
      control, combo ? CB_GETCOUNT : LB_GETCOUNT, 0, 0));
  for (int index = 0; index < count; ++index) {
    if (static_cast<std::uint16_t>(SendMessageW(
            control, combo ? CB_GETITEMDATA : LB_GETITEMDATA,
            static_cast<WPARAM>(index), 0)) == wanted) {
      (void)SendMessageW(control, combo ? CB_SETCURSEL : LB_SETCURSEL,
                         static_cast<WPARAM>(index), 0);
      return;
    }
  }
  if (count > 0) {
    (void)SendMessageW(control, combo ? CB_SETCURSEL : LB_SETCURSEL, 0, 0);
  }
}

void rebuild_terrain_list(const HWND dialog,
                          const std::uint16_t requested) noexcept {
  const HWND tileset_control = GetDlgItem(dialog, IDC_NEW_TILESET);
  const HWND terrain_control = GetDlgItem(dialog, IDC_NEW_TERRAIN);
  const std::uint16_t tileset_id = selected_data(tileset_control, 0U);
  const NewMapTileset* const tileset = new_map_tileset(tileset_id);
  if (terrain_control == nullptr || tileset == nullptr) {
    return;
  }
  (void)SendMessageW(terrain_control, WM_SETREDRAW, FALSE, 0);
  (void)SendMessageW(terrain_control, LB_RESETCONTENT, 0, 0);
  for (std::size_t index = 0U; index < tileset->terrain.size(); ++index) {
    if (tileset->terrain[index].empty()) {
      continue;
    }
    const std::wstring label = widen_ascii(tileset->terrain[index]);
    const LRESULT item = SendMessageW(terrain_control, LB_ADDSTRING, 0,
                                      reinterpret_cast<LPARAM>(label.c_str()));
    if (item != LB_ERR && item != LB_ERRSPACE) {
      (void)SendMessageW(terrain_control, LB_SETITEMDATA,
                         static_cast<WPARAM>(item),
                         static_cast<LPARAM>(index));
    }
  }
  select_data(terrain_control, requested);
  (void)SendMessageW(terrain_control, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(terrain_control, nullptr, TRUE);
}

void synchronize_square_map(const HWND dialog) noexcept {
  const bool square =
      IsDlgButtonChecked(dialog, IDC_NEW_SQUARE) == BST_CHECKED;
  const HWND width = GetDlgItem(dialog, IDC_NEW_WIDTH);
  const HWND height = GetDlgItem(dialog, IDC_NEW_HEIGHT);
  EnableWindow(height, square ? FALSE : TRUE);
  if (square && width != nullptr && height != nullptr) {
    const int selection = selected_item(width);
    if (selection != CB_ERR) {
      (void)SendMessageW(height, CB_SETCURSEL,
                         static_cast<WPARAM>(selection), 0);
    }
  }
}

INT_PTR CALLBACK new_map_dialog_proc(const HWND dialog,
                                     const UINT message,
                                     const WPARAM wparam,
                                     const LPARAM lparam) noexcept {
  auto* settings = reinterpret_cast<NewMapSettings*>(
      GetWindowLongPtrW(dialog, DWLP_USER));
  switch (message) {
    case WM_INITDIALOG: {
      settings = reinterpret_cast<NewMapSettings*>(lparam);
      SetWindowLongPtrW(dialog, DWLP_USER,
                        reinterpret_cast<LONG_PTR>(settings));
      if (settings == nullptr) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
      }
      const HWND width = GetDlgItem(dialog, IDC_NEW_WIDTH);
      const HWND height = GetDlgItem(dialog, IDC_NEW_HEIGHT);
      for (const std::uint16_t dimension : new_map_dimensions) {
        const std::wstring label = std::to_wstring(dimension);
        const LRESULT width_item = SendMessageW(
            width, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        const LRESULT height_item = SendMessageW(
            height, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        if (width_item >= 0) {
          (void)SendMessageW(width, CB_SETITEMDATA,
                             static_cast<WPARAM>(width_item), dimension);
        }
        if (height_item >= 0) {
          (void)SendMessageW(height, CB_SETITEMDATA,
                             static_cast<WPARAM>(height_item), dimension);
        }
      }
      select_data(width, settings->width);
      select_data(height, settings->height);
      CheckDlgButton(dialog, IDC_NEW_SQUARE,
                     settings->width == settings->height ? BST_CHECKED
                                                        : BST_UNCHECKED);

      const HWND tilesets = GetDlgItem(dialog, IDC_NEW_TILESET);
      for (const NewMapTileset& tileset : kTilesets) {
        const LRESULT item = SendMessageW(
            tilesets, LB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(tileset.name.data()));
        if (item >= 0) {
          (void)SendMessageW(tilesets, LB_SETITEMDATA,
                             static_cast<WPARAM>(item), tileset.id);
        }
      }
      select_data(tilesets, settings->tileset_id);
      rebuild_terrain_list(dialog, settings->terrain_index);
      synchronize_square_map(dialog);
      return TRUE;
    }
    case WM_COMMAND: {
      const UINT id = LOWORD(wparam);
      const UINT notification = HIWORD(wparam);
      if (id == IDCANCEL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
      }
      if (id == IDOK) {
        if (settings == nullptr) {
          EndDialog(dialog, IDCANCEL);
          return TRUE;
        }
        settings->width = selected_data(GetDlgItem(dialog, IDC_NEW_WIDTH), 128U);
        settings->height = selected_data(GetDlgItem(dialog, IDC_NEW_HEIGHT), 128U);
        settings->tileset_id =
            selected_data(GetDlgItem(dialog, IDC_NEW_TILESET), 0U);
        const NewMapTileset* const tileset =
            new_map_tileset(settings->tileset_id);
        settings->terrain_index = selected_data(
            GetDlgItem(dialog, IDC_NEW_TERRAIN),
            tileset == nullptr ? 0U : tileset->default_terrain);
        if (!valid_new_map_settings(*settings)) {
          MessageBoxW(dialog, L"Please select a valid map size and terrain.",
                      L"New Scenario", MB_OK | MB_ICONWARNING);
          return TRUE;
        }
        EndDialog(dialog, IDOK);
        return TRUE;
      }
      if ((id == IDC_NEW_WIDTH && notification == CBN_SELCHANGE) ||
          id == IDC_NEW_SQUARE) {
        synchronize_square_map(dialog);
        return TRUE;
      }
      if (id == IDC_NEW_TILESET && notification == LBN_SELCHANGE) {
        const std::uint16_t tileset_id = selected_data(
            GetDlgItem(dialog, IDC_NEW_TILESET), 0U);
        const NewMapTileset* const tileset = new_map_tileset(tileset_id);
        rebuild_terrain_list(dialog,
                             tileset == nullptr ? 0U
                                                : tileset->default_terrain);
        return TRUE;
      }
      if (id == ID_HELP) {
        MessageBoxW(dialog,
                    L"Choose a map width and height, then choose the tileset "
                    L"and logical terrain used to fill the new scenario.",
                    L"New Scenario Help", MB_OK | MB_ICONINFORMATION);
        return TRUE;
      }
      break;
    }
    default:
      break;
  }
  return FALSE;
}

}  // namespace

const std::array<NewMapTileset, 5>& new_map_tilesets() noexcept {
  return kTilesets;
}

const NewMapTileset* new_map_tileset(const std::uint16_t id) noexcept {
  const auto found = std::find_if(
      kTilesets.begin(), kTilesets.end(),
      [id](const NewMapTileset& tileset) { return tileset.id == id; });
  return found == kTilesets.end() ? nullptr : &*found;
}

std::string_view new_map_terrain_name(
    const NewMapSettings& settings) noexcept {
  const NewMapTileset* const tileset = new_map_tileset(settings.tileset_id);
  return tileset != nullptr && settings.terrain_index < tileset->terrain.size()
             ? tileset->terrain[settings.terrain_index]
             : std::string_view{};
}

bool valid_new_map_settings(const NewMapSettings& settings) noexcept {
  const bool valid_width =
      std::find(new_map_dimensions.begin(), new_map_dimensions.end(),
                settings.width) != new_map_dimensions.end();
  const bool valid_height =
      std::find(new_map_dimensions.begin(), new_map_dimensions.end(),
                settings.height) != new_map_dimensions.end();
  return valid_width && valid_height && !new_map_terrain_name(settings).empty();
}

bool show_new_map_dialog(const HWND owner,
                         const HINSTANCE instance,
                         NewMapSettings& settings) noexcept {
  NewMapSettings candidate = settings;
  const INT_PTR result = DialogBoxParamW(
      instance, MAKEINTRESOURCEW(IDD_NEW_SCENARIO), owner,
      new_map_dialog_proc, reinterpret_cast<LPARAM>(&candidate));
  if (result != IDOK) {
    return false;
  }
  settings = candidate;
  return true;
}

}  // namespace staredit
