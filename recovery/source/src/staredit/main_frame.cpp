#include "main_frame.hpp"

#include "brush_palette.hpp"
#include "document.hpp"
#include "map_view.hpp"
#include "minimap.hpp"
#include "new_map_dialog.hpp"
#include "resource.h"
#include "scenario_dialogs.hpp"

#include <algorithm>
#include <array>
#include <commctrl.h>
#include <commdlg.h>
#include <cstdint>
#include <memory>
#include <new>
#include <string>
#include <utility>

namespace staredit {
namespace {

constexpr wchar_t kFrameClass[] = L"StarEditFrame";
constexpr wchar_t kApplicationTitle[] = L"Starcraft Campaign Editor";
constexpr int kBrushPaletteWidth = 228;
constexpr int kMinimapHeight = 196;

void append_item(const HMENU menu,
                 const UINT flags,
                 const UINT_PTR id,
                 const wchar_t* const label) noexcept {
  (void)AppendMenuW(menu, flags, id, label);
}

HMENU create_application_menu(HMENU& window_menu) noexcept {
  const HMENU menu = CreateMenu();
  const HMENU file = CreatePopupMenu();
  const HMENU edit = CreatePopupMenu();
  const HMENU view = CreatePopupMenu();
  const HMENU layer = CreatePopupMenu();
  const HMENU brush = CreatePopupMenu();
  const HMENU scenario = CreatePopupMenu();
  window_menu = CreatePopupMenu();
  const HMENU help = CreatePopupMenu();
  if (menu == nullptr || file == nullptr || edit == nullptr || view == nullptr ||
      layer == nullptr || brush == nullptr || scenario == nullptr ||
      window_menu == nullptr || help == nullptr) {
    if (menu != nullptr) {
      DestroyMenu(menu);
    }
    return nullptr;
  }

  append_item(file, MF_STRING, ID_FILE_NEW, L"&New\tCtrl+N");
  append_item(file, MF_STRING, ID_FILE_OPEN, L"&Open...\tCtrl+O");
  append_item(file, MF_STRING, ID_FILE_CLOSE, L"&Close");
  append_item(file, MF_STRING | MF_GRAYED, ID_FILE_EXPORT_CHK,
              L"Export scenario.&chk...");
  append_item(file, MF_SEPARATOR, 0, nullptr);
  append_item(file, MF_STRING, ID_FILE_EXIT, L"E&xit");

  append_item(edit, MF_STRING | MF_GRAYED, ID_EDIT_UNDO, L"&Undo\tCtrl+Z");
  append_item(edit, MF_STRING | MF_GRAYED, ID_EDIT_REDO, L"&Redo\tCtrl+Y");

  append_item(view, MF_STRING, ID_VIEW_GRID, L"Tile &Grid");
  append_item(view, MF_STRING | MF_CHECKED, ID_VIEW_UNITS,
              L"&Unit Markers");
  append_item(view, MF_STRING | MF_CHECKED, ID_VIEW_BRUSH_PALETTE,
              L"&Brush Palette");

  append_item(layer, MF_STRING | MF_CHECKED, ID_LAYER_TERRAIN, L"&Terrain");
  append_item(layer, MF_STRING, ID_LAYER_DOODADS, L"&Doodads");
  append_item(layer, MF_STRING, ID_LAYER_UNITS, L"&Units");
  append_item(layer, MF_STRING, ID_LAYER_LOCATIONS, L"&Locations");
  append_item(layer, MF_STRING, ID_LAYER_SPRITES, L"&Sprites");
  append_item(layer, MF_STRING, ID_LAYER_FOG, L"&Fog of War");

  append_item(brush, MF_STRING | MF_CHECKED, ID_BRUSH_SIZE_1, L"&1 x 1");
  append_item(brush, MF_STRING, ID_BRUSH_SIZE_3, L"&3 x 3");
  append_item(brush, MF_STRING, ID_BRUSH_SIZE_5, L"&5 x 5");

  append_item(scenario, MF_STRING, ID_SCENARIO_SUMMARY, L"&Summary...");
  append_item(scenario, MF_SEPARATOR, 0, nullptr);
  append_item(scenario, MF_STRING, ID_SCENARIO_PLAYERS,
              L"&Player Settings...");
  append_item(scenario, MF_STRING | MF_GRAYED, ID_SCENARIO_FORCES,
              L"&Forces...");
  append_item(scenario, MF_STRING | MF_GRAYED, ID_SCENARIO_SOUNDS,
              L"&Sounds...");
  append_item(scenario, MF_STRING | MF_GRAYED, ID_SCENARIO_TRIGGERS,
              L"&Triggers...");
  append_item(scenario, MF_STRING | MF_GRAYED, ID_SCENARIO_BRIEFING,
              L"Mission &Briefing...");

  append_item(window_menu, MF_STRING, ID_WINDOW_CASCADE, L"&Cascade");
  append_item(window_menu, MF_STRING, ID_WINDOW_TILE_HORIZONTAL,
              L"Tile &Horizontally");
  append_item(window_menu, MF_STRING, ID_WINDOW_TILE_VERTICAL,
              L"Tile &Vertically");
  append_item(window_menu, MF_STRING, ID_WINDOW_ARRANGE_ICONS,
              L"&Arrange Icons");

  append_item(help, MF_STRING, ID_HELP_ABOUT, L"&About StarEdit...");

  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(file), L"&File");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(edit), L"&Edit");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(view), L"&View");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(layer), L"&Layer");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(brush), L"&Brush");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(scenario), L"&Scenario");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(window_menu), L"&Window");
  append_item(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(help), L"&Help");
  return menu;
}

std::wstring widen_ascii(const std::string& value) {
  return std::wstring{value.begin(), value.end()};
}

}  // namespace

MainFrame::MainFrame(std::filesystem::path data_root) noexcept
    : data_root_(std::move(data_root)) {}

bool MainFrame::register_class(const HINSTANCE instance) noexcept {
  WNDCLASSEXW window_class{};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = window_proc;
  window_class.hInstance = instance;
  window_class.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_STAREDIT_ICON));
  window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  window_class.hbrBackground =
      reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_APPWORKSPACE + 1));
  window_class.lpszClassName = kFrameClass;
  window_class.hIconSm = window_class.hIcon;
  return RegisterClassExW(&window_class) != 0;
}

bool MainFrame::create(const HINSTANCE instance, const int show_command) noexcept {
  instance_ = instance;
  window_ = CreateWindowExW(0, kFrameClass, kApplicationTitle,
                            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                            CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800, nullptr,
                            nullptr, instance, this);
  if (window_ == nullptr) {
    return false;
  }
  ShowWindow(window_, show_command);
  UpdateWindow(window_);
  return true;
}

bool MainFrame::open_document(const std::filesystem::path& path) noexcept {
  try {
    auto document = std::make_unique<EditorDocument>();
    std::wstring error{};
    if (!document->load(path, data_root_, error)) {
      MessageBoxW(window_, error.c_str(), L"Unable to open scenario",
                  MB_OK | MB_ICONERROR);
      return false;
    }
    return attach_document(std::move(document));
  } catch (...) {
    MessageBoxW(window_, L"There was not enough memory to open this scenario.",
                L"Unable to open scenario", MB_OK | MB_ICONERROR);
    return false;
  }
}

bool MainFrame::new_document() noexcept {
  NewMapSettings settings = new_map_defaults_;
  if (!show_new_map_dialog(window_, instance_, settings)) {
    return false;
  }
  new_map_defaults_ = settings;
  return new_document(settings);
}

bool MainFrame::new_default_document() noexcept {
  return new_document({96U, 96U, 4U, 1U});
}

bool MainFrame::new_document(const NewMapSettings& settings) noexcept {
  if (!valid_new_map_settings(settings)) {
    return false;
  }
  try {
    auto document = std::make_unique<EditorDocument>();
    std::wstring error{};
    std::filesystem::path scenario_template{};
    switch (settings.tileset_id) {
      case 1U:
        scenario_template = data_root_ / L"maps" / L"96x96_space4.scm";
        break;
      case 3U:
        scenario_template = data_root_ / L"maps" / L"96x96_ash4.scm";
        break;
      default:
        scenario_template =
            data_root_ / L"maps" / L"96x96_wasteland4.scm";
        break;
    }
    if (!document->create_blank(
            scenario_template, data_root_, settings.width, settings.height,
            settings.tileset_id, new_map_terrain_name(settings), error)) {
      MessageBoxW(window_, error.c_str(), L"Unable to create map",
                  MB_OK | MB_ICONERROR);
      return false;
    }
    return attach_document(std::move(document));
  } catch (...) {
    MessageBoxW(window_, L"There was not enough memory to create a new map.",
                L"Unable to create map", MB_OK | MB_ICONERROR);
    return false;
  }
}

bool MainFrame::attach_document(
    std::unique_ptr<EditorDocument> document) noexcept {
  const HWND child = create_map_view(mdi_client_, instance_, document);
  if (child == nullptr) {
    MessageBoxW(window_, L"The map view window could not be created.",
                L"Unable to open scenario", MB_OK | MB_ICONERROR);
    return false;
  }
  set_active_grid_visible(mdi_client_, grid_visible_);
  set_active_units_visible(mdi_client_, units_visible_);
  set_active_brush_size(mdi_client_, brush_size_);
  set_active_layer(mdi_client_, active_layer_);
  set_brush_palette_layer(brush_palette_, active_layer_);
  refresh_brush_palette(brush_palette_);
  refresh_minimap(minimap_);
  update_status();
  return true;
}

HWND MainFrame::window() const noexcept { return window_; }
HWND MainFrame::mdi_client() const noexcept { return mdi_client_; }
HWND MainFrame::brush_palette() const noexcept { return brush_palette_; }
HWND MainFrame::minimap() const noexcept { return minimap_; }
HACCEL MainFrame::accelerators() const noexcept { return accelerators_; }

bool MainFrame::on_create() noexcept {
  menu_ = create_application_menu(window_menu_);
  if (menu_ == nullptr || SetMenu(window_, menu_) == FALSE) {
    return false;
  }

  CLIENTCREATESTRUCT client_create{};
  client_create.hWindowMenu = window_menu_;
  client_create.idFirstChild = ID_MDI_FIRST_CHILD;
  mdi_client_ = CreateWindowExW(
      WS_EX_CLIENTEDGE, L"MDICLIENT", nullptr,
      WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, window_, nullptr, instance_, &client_create);
  brush_palette_ = create_brush_palette(window_, instance_, mdi_client_);
  minimap_ = create_minimap(window_, instance_, mdi_client_);
  status_bar_ = CreateWindowExW(0, STATUSCLASSNAMEW, L"Ready",
                                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0,
                                0, 0, window_, nullptr, instance_, nullptr);
  std::array<ACCEL, 5> accelerator_entries{{
      {static_cast<BYTE>(FVIRTKEY | FCONTROL), static_cast<WORD>('N'),
       ID_FILE_NEW},
      {static_cast<BYTE>(FVIRTKEY | FCONTROL), static_cast<WORD>('O'),
       ID_FILE_OPEN},
      {static_cast<BYTE>(FVIRTKEY | FCONTROL), static_cast<WORD>('Z'),
       ID_EDIT_UNDO},
      {static_cast<BYTE>(FVIRTKEY | FCONTROL), static_cast<WORD>('Y'),
       ID_EDIT_REDO},
      {static_cast<BYTE>(FVIRTKEY | FCONTROL | FSHIFT), static_cast<WORD>('Z'),
       ID_EDIT_REDO},
  }};
  accelerators_ = CreateAcceleratorTableW(
      accelerator_entries.data(), static_cast<int>(accelerator_entries.size()));
  const bool created = mdi_client_ != nullptr && brush_palette_ != nullptr &&
                       minimap_ != nullptr &&
                       status_bar_ != nullptr && accelerators_ != nullptr;
  if (created) {
    on_size();
    update_status();
  }
  return created;
}

void MainFrame::on_size() noexcept {
  if (mdi_client_ == nullptr || status_bar_ == nullptr) {
    return;
  }
  SendMessageW(status_bar_, WM_SIZE, 0, 0);
  RECT client{};
  RECT status{};
  GetClientRect(window_, &client);
  GetWindowRect(status_bar_, &status);
  const int status_height = status.bottom - status.top;
  const int content_height = (std::max)(0L, client.bottom - status_height);
  const int sidebar_width =
      (std::min)(kBrushPaletteWidth, static_cast<int>(client.right / 2));
  const int minimap_height = (std::min)(kMinimapHeight, content_height);
  MoveWindow(mdi_client_, sidebar_width, 0,
             (std::max)(0L, client.right - sidebar_width), content_height,
             TRUE);
  if (minimap_ != nullptr) {
    MoveWindow(minimap_, 0, 0, sidebar_width, minimap_height, TRUE);
  }
  if (brush_palette_ != nullptr) {
    MoveWindow(brush_palette_, 0, minimap_height, sidebar_width,
               brush_palette_visible_
                   ? (std::max)(0, content_height - minimap_height)
                   : 0,
               TRUE);
  }
}

void MainFrame::on_command(const UINT command) noexcept {
  switch (command) {
    case ID_FILE_NEW:
      (void)new_document();
      return;
    case ID_FILE_OPEN:
      choose_and_open_document();
      return;
    case ID_FILE_CLOSE:
      close_active_document();
      return;
    case ID_FILE_EXPORT_CHK:
      export_active_chk();
      return;
    case ID_FILE_EXIT:
      SendMessageW(window_, WM_CLOSE, 0, 0);
      return;
    case ID_VIEW_GRID:
      grid_visible_ = !grid_visible_;
      CheckMenuItem(menu_, ID_VIEW_GRID,
                    MF_BYCOMMAND |
                        (grid_visible_ ? MF_CHECKED : MF_UNCHECKED));
      set_active_grid_visible(mdi_client_, grid_visible_);
      return;
    case ID_VIEW_UNITS:
      units_visible_ = !units_visible_;
      CheckMenuItem(menu_, ID_VIEW_UNITS,
                    MF_BYCOMMAND |
                        (units_visible_ ? MF_CHECKED : MF_UNCHECKED));
      set_active_units_visible(mdi_client_, units_visible_);
      return;
    case ID_VIEW_BRUSH_PALETTE:
      brush_palette_visible_ = !brush_palette_visible_;
      CheckMenuItem(menu_, ID_VIEW_BRUSH_PALETTE,
                    MF_BYCOMMAND |
                        (brush_palette_visible_ ? MF_CHECKED : MF_UNCHECKED));
      ShowWindow(brush_palette_, brush_palette_visible_ ? SW_SHOW : SW_HIDE);
      on_size();
      return;
    case ID_EDIT_UNDO:
      (void)undo_active_document(mdi_client_);
      return;
    case ID_EDIT_REDO:
      (void)redo_active_document(mdi_client_);
      return;
    case ID_LAYER_TERRAIN:
    case ID_LAYER_DOODADS:
    case ID_LAYER_UNITS:
    case ID_LAYER_LOCATIONS:
    case ID_LAYER_SPRITES:
    case ID_LAYER_FOG: {
      switch (command) {
        case ID_LAYER_DOODADS:
          active_layer_ = EditorLayer::doodads;
          break;
        case ID_LAYER_UNITS:
          active_layer_ = EditorLayer::units;
          break;
        case ID_LAYER_LOCATIONS:
          active_layer_ = EditorLayer::locations;
          break;
        case ID_LAYER_SPRITES:
          active_layer_ = EditorLayer::sprites;
          break;
        case ID_LAYER_FOG:
          active_layer_ = EditorLayer::fog;
          break;
        default:
          active_layer_ = EditorLayer::terrain;
          break;
      }
      CheckMenuRadioItem(menu_, ID_LAYER_TERRAIN, ID_LAYER_FOG, command,
                         MF_BYCOMMAND);
      set_active_layer(mdi_client_, active_layer_);
      set_brush_palette_layer(brush_palette_, active_layer_);
      refresh_brush_palette(brush_palette_);
      update_status();
      return;
    }
    case ID_BRUSH_SIZE_1:
    case ID_BRUSH_SIZE_3:
    case ID_BRUSH_SIZE_5: {
      const std::uint8_t size = command == ID_BRUSH_SIZE_1
                                    ? 1U
                                    : (command == ID_BRUSH_SIZE_3 ? 3U : 5U);
      brush_size_ = size;
      CheckMenuRadioItem(menu_, ID_BRUSH_SIZE_1, ID_BRUSH_SIZE_5, command,
                         MF_BYCOMMAND);
      set_active_brush_size(mdi_client_, size);
      update_status();
      return;
    }
    case ID_SCENARIO_SUMMARY:
      show_scenario_summary();
      return;
    case ID_SCENARIO_PLAYERS: {
      EditorDocument* const document = active_editor_document(mdi_client_);
      if (document != nullptr &&
          show_player_settings_dialog(window_, instance_, *document)) {
        refresh_active_document_view(mdi_client_);
        refresh_minimap(minimap_);
        update_status();
      }
      return;
    }
    case ID_WINDOW_CASCADE:
      SendMessageW(mdi_client_, WM_MDICASCADE, 0, 0);
      return;
    case ID_WINDOW_TILE_HORIZONTAL:
      SendMessageW(mdi_client_, WM_MDITILE, MDITILE_HORIZONTAL, 0);
      return;
    case ID_WINDOW_TILE_VERTICAL:
      SendMessageW(mdi_client_, WM_MDITILE, MDITILE_VERTICAL, 0);
      return;
    case ID_WINDOW_ARRANGE_ICONS:
      SendMessageW(mdi_client_, WM_MDIICONARRANGE, 0, 0);
      return;
    case ID_HELP_ABOUT:
      MessageBoxW(
          window_,
          L"Starcraft Campaign Editor recovery\n\n"
          L"Initial reconstruction shell based on the 1998 StarEdit "
          L"executable architecture.\n\nRight-click clears the active brush. "
          L"In Unit mode, click and drag a unit to move it; double-click or "
          L"press F2 to edit its properties. "
          L"In Fog of War, drag to obscure and Shift-drag to reveal. "
          L"Raw CHK export is validated before "
          L"replacement.",
          L"About StarEdit", MB_OK | MB_ICONINFORMATION);
      return;
    default:
      return;
  }
}

void MainFrame::export_active_chk() noexcept {
  EditorDocument* const document = active_editor_document(mdi_client_);
  if (document == nullptr) {
    return;
  }
  std::array<wchar_t, 32768> path{};
  try {
    std::filesystem::path suggested = document->path().filename();
    suggested.replace_extension(L".chk");
    const std::wstring value = suggested.wstring();
    if (value.size() >= path.size()) {
      return;
    }
    std::copy(value.begin(), value.end(), path.begin());
  } catch (...) {
    return;
  }
  constexpr wchar_t filter[] =
      L"Raw scenario.chk files (*.chk)\0*.chk\0All files (*.*)\0*.*\0\0";
  OPENFILENAMEW save{};
  save.lStructSize = sizeof(save);
  save.hwndOwner = window_;
  save.hInstance = instance_;
  save.lpstrFilter = filter;
  save.lpstrFile = path.data();
  save.nMaxFile = static_cast<DWORD>(path.size());
  save.lpstrDefExt = L"chk";
  save.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |
               OFN_HIDEREADONLY;
  if (GetSaveFileNameW(&save) == FALSE) {
    return;
  }
  std::wstring error{};
  if (!document->export_raw_chk(path.data(), error)) {
    MessageBoxW(window_, error.c_str(), L"Raw CHK export failed",
                MB_OK | MB_ICONERROR);
    return;
  }
  std::wstring status = L"Exported validated raw CHK: ";
  status += path.data();
  SendMessageW(status_bar_, SB_SETTEXTW, 0,
               reinterpret_cast<LPARAM>(status.c_str()));
}

void MainFrame::choose_and_open_document() noexcept {
  std::array<wchar_t, 32768> path{};
  std::wstring initial_directory{};
  try {
    const std::filesystem::path maps = data_root_ / L"maps";
    std::error_code error{};
    if (std::filesystem::is_directory(maps, error)) {
      initial_directory = maps.wstring();
    }
  } catch (...) {
  }
  constexpr wchar_t filter[] =
      L"StarCraft Scenarios (*.scm;*.scx)\0*.scm;*.scx\0"
      L"Raw scenario.chk files (*.chk)\0*.chk\0"
      L"All files (*.*)\0*.*\0\0";
  OPENFILENAMEW open{};
  open.lStructSize = sizeof(open);
  open.hwndOwner = window_;
  open.hInstance = instance_;
  open.lpstrFilter = filter;
  open.lpstrFile = path.data();
  open.nMaxFile = static_cast<DWORD>(path.size());
  open.lpstrInitialDir = initial_directory.empty() ? nullptr
                                                   : initial_directory.c_str();
  open.lpstrDefExt = L"scm";
  open.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
               OFN_HIDEREADONLY;
  if (GetOpenFileNameW(&open) != FALSE) {
    (void)open_document(path.data());
  }
}

void MainFrame::close_active_document() noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client_, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, WM_CLOSE, 0, 0);
    update_status();
  }
}

bool MainFrame::close_all_documents() noexcept {
  while (true) {
    const HWND child = reinterpret_cast<HWND>(
        SendMessageW(mdi_client_, WM_MDIGETACTIVE, 0, 0));
    if (child == nullptr) {
      return true;
    }
    SendMessageW(child, WM_CLOSE, 0, 0);
    if (IsWindow(child) != FALSE) {
      return false;
    }
  }
}

void MainFrame::show_scenario_summary() const noexcept {
  const EditorDocument* const document = active_document(mdi_client_);
  if (document == nullptr) {
    MessageBoxW(window_, L"Open a scenario first.", L"Scenario Summary",
                MB_OK | MB_ICONINFORMATION);
    return;
  }
  try {
    std::wstring summary = L"File: " + document->path().wstring();
    summary += L"\nContainer: ";
    summary += document->source_is_archive() ? L"SCM/SCX archive" : L"raw CHK";
    summary += L"\nCHK dialect: ";
    summary += scenario_format_name(document->format());
    summary += L"\nDimensions: " + std::to_wstring(document->width()) + L" x " +
               std::to_wstring(document->height()) + L" tiles";
    summary += L"\nTileset: " + widen_ascii(document->tileset_name()) + L" (" +
               std::to_wstring(document->tileset_id()) + L")";
    summary += L"\nSections: " + std::to_wstring(document->section_count());
    summary += L"\nUnits: " + std::to_wstring(document->unit_count());
    summary += L"\nDoodads: " + std::to_wstring(document->doodad_count());
    summary += L"\nSprites: " + std::to_wstring(document->sprite_count());
    summary += L"\nISOM entries: ";
    summary += document->has_isom()
                   ? std::to_wstring(document->isom_entry_count())
                   : L"not present";
    summary += L"\nISOM CV5 group pairs: " +
               std::to_wstring(document->isom_group_pair_count());
    summary += L"\nISOM terrain types: " +
               std::to_wstring(document->isom_terrain_type_count());
    summary += L"\nModified: ";
    summary += document->modified() ? L"yes" : L"no";
    summary += L"\nLossless source round trip: ";
    summary += document->chk_round_trip_matches_source() ? L"yes" : L"no";
    MessageBoxW(window_, summary.c_str(), L"Scenario Summary",
                MB_OK | MB_ICONINFORMATION);
  } catch (...) {
    MessageBoxW(window_, L"The scenario summary could not be formatted.",
                L"Scenario Summary", MB_OK | MB_ICONERROR);
  }
}

void MainFrame::update_status() noexcept {
  if (status_bar_ == nullptr) {
    return;
  }
  const EditorDocument* const document = active_document(mdi_client_);
  EnableMenuItem(menu_, ID_FILE_CLOSE,
                 MF_BYCOMMAND |
                     (document == nullptr ? MF_GRAYED : MF_ENABLED));
  EnableMenuItem(menu_, ID_FILE_EXPORT_CHK,
                 MF_BYCOMMAND |
                     (document == nullptr ? MF_GRAYED : MF_ENABLED));
  EnableMenuItem(menu_, ID_EDIT_UNDO,
                 MF_BYCOMMAND |
                     (document != nullptr && document->can_undo() ? MF_ENABLED
                                                                  : MF_GRAYED));
  EnableMenuItem(menu_, ID_EDIT_REDO,
                 MF_BYCOMMAND |
                     (document != nullptr && document->can_redo() ? MF_ENABLED
                                                                  : MF_GRAYED));
  if (document == nullptr) {
    SendMessageW(status_bar_, SB_SETTEXTW, 0,
                 reinterpret_cast<LPARAM>(L"Ready"));
    return;
  }
  try {
    std::wstring status = document->title() + L"   " +
                          std::to_wstring(document->width()) + L" x " +
                          std::to_wstring(document->height()) + L"   " +
                          widen_ascii(document->tileset_name()) + L"   Units: " +
                          std::to_wstring(document->unit_count());
    if (document->modified()) {
      status += L"   Modified";
    }
    MapViewport viewport{};
    if (query_active_viewport(mdi_client_, viewport)) {
      status += L"   Zoom: " + std::to_wstring(viewport.zoom_percent) + L"%";
    }
    status += L"   Layer: ";
    status += editor_layer_name(active_layer_);
    if (active_layer_ == EditorLayer::terrain) {
      status += L"   Brush: " + std::to_wstring(brush_size_) + L" x " +
                std::to_wstring(brush_size_);
      status += L"   Right-click: pick terrain   Left-drag: paint";
    } else {
      status += L"   Right-click: pick object   Left-click: place   Delete: erase";
    }
    SendMessageW(status_bar_, SB_SETTEXTW, 0,
                 reinterpret_cast<LPARAM>(status.c_str()));
  } catch (...) {
    SendMessageW(status_bar_, SB_SETTEXTW, 0,
                 reinterpret_cast<LPARAM>(L"Scenario loaded"));
  }
}

LRESULT CALLBACK MainFrame::window_proc(const HWND window,
                                        const UINT message,
                                        const WPARAM wparam,
                                        const LPARAM lparam) noexcept {
  MainFrame* frame = reinterpret_cast<MainFrame*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
  if (message == WM_NCCREATE) {
    const auto* const create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
    frame = static_cast<MainFrame*>(create->lpCreateParams);
    frame->window_ = window;
    SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(frame));
  }
  if (frame == nullptr) {
    return DefFrameProcW(window, nullptr, message, wparam, lparam);
  }
  switch (message) {
    case WM_CREATE:
      return frame->on_create() ? 0 : -1;
    case WM_SIZE:
      frame->on_size();
      return 0;
    case WM_COMMAND:
      frame->on_command(LOWORD(wparam));
      break;
    case WM_CLOSE:
      if (frame->close_all_documents()) {
        DestroyWindow(window);
      }
      return 0;
    case WM_STAREDIT_DOCUMENT_ACTIVATED:
      set_brush_palette_layer(frame->brush_palette_, frame->active_layer_);
      refresh_brush_palette(frame->brush_palette_);
      refresh_minimap(frame->minimap_);
      if (lparam != 0) {
        set_brush_palette_selection(frame->brush_palette_,
                                    static_cast<std::uint16_t>(wparam));
      }
      frame->update_status();
      return 0;
    case WM_STAREDIT_DOCUMENT_CHANGED:
      refresh_minimap(frame->minimap_);
      frame->update_status();
      return 0;
    case WM_STAREDIT_BRUSH_CHANGED:
      if (lparam != 0) {
        set_brush_palette_selection(frame->brush_palette_,
                                    static_cast<std::uint16_t>(wparam));
      } else {
        clear_brush_palette_selection(frame->brush_palette_);
      }
      return 0;
    case WM_STAREDIT_VIEWPORT_CHANGED:
      refresh_minimap(frame->minimap_);
      frame->update_status();
      return 0;
    case WM_DESTROY:
      if (frame->accelerators_ != nullptr) {
        DestroyAcceleratorTable(frame->accelerators_);
        frame->accelerators_ = nullptr;
      }
      PostQuitMessage(0);
      return 0;
    default:
      break;
  }
  return DefFrameProcW(window, frame->mdi_client_, message, wparam, lparam);
}

}  // namespace staredit
