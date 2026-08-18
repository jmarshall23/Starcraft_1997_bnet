#include "brush_palette.hpp"

#include "document.hpp"
#include "map_view.hpp"
#include "resource.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <cwctype>
#include <vector>

#include <commctrl.h>

namespace staredit {
namespace {

constexpr wchar_t kBrushPaletteClass[] = L"StarEditBrushPalette";
constexpr UINT kRefreshPalette = WM_APP + 201U;
constexpr UINT kSetPaletteSelection = WM_APP + 202U;
constexpr UINT kGetPaletteSelection = WM_APP + 203U;
constexpr UINT kSetPaletteLayer = WM_APP + 204U;
constexpr UINT kClearPaletteSelection = WM_APP + 205U;
constexpr int kHeaderHeight = 30;
constexpr int kCellWidth = 54;
constexpr int kCellHeight = 58;
constexpr int kTileSize = 40;
constexpr int kUnitHeaderHeight = 58;
constexpr int kUnitCellWidth = 108;
constexpr int kUnitCellHeight = 104;
constexpr int kUnitTileSize = 84;

struct BrushPaletteState {
  HWND mdi_client{};
  int first_row{};
  std::optional<std::uint16_t> selected_tile{};
  EditorLayer layer{EditorLayer::terrain};
  HWND search{};
  std::wstring query{};
  std::vector<std::uint16_t> filtered_brushes{};
};

BrushPaletteState* state_from_window(const HWND window) noexcept {
  return reinterpret_cast<BrushPaletteState*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
}

std::optional<std::uint16_t> normalize_selection(
    const BrushPaletteState& state,
    const std::uint16_t tile_id) noexcept {
  const EditorDocument* const document = active_document(state.mdi_client);
  if (document == nullptr) {
    return std::nullopt;
  }
  for (const std::uint16_t brush : document->object_brushes(state.layer)) {
    if ((state.layer == EditorLayer::terrain &&
         document->same_terrain_group(brush, tile_id)) ||
        (state.layer != EditorLayer::terrain && brush == tile_id)) {
      return brush;
    }
  }
  return std::nullopt;
}

int header_height(const BrushPaletteState& state) noexcept {
  return state.layer == EditorLayer::units ? kUnitHeaderHeight : kHeaderHeight;
}

int cell_width(const BrushPaletteState& state) noexcept {
  return state.layer == EditorLayer::units ? kUnitCellWidth : kCellWidth;
}

int cell_height(const BrushPaletteState& state) noexcept {
  return state.layer == EditorLayer::units ? kUnitCellHeight : kCellHeight;
}

int tile_size(const BrushPaletteState& state) noexcept {
  return state.layer == EditorLayer::units ? kUnitTileSize : kTileSize;
}

int column_count(const HWND window, const BrushPaletteState& state) noexcept {
  RECT client{};
  GetClientRect(window, &client);
  if (state.layer == EditorLayer::units) {
    return (std::max)(1, (std::min)(2, static_cast<int>(client.right) /
                                           kUnitCellWidth));
  }
  return (std::max)(1, static_cast<int>(client.right) / kCellWidth);
}

const std::vector<std::uint16_t>& visible_brushes(
    const BrushPaletteState& state,
    const EditorDocument& document) noexcept {
  return state.layer == EditorLayer::units
             ? state.filtered_brushes
             : document.object_brushes(state.layer);
}

std::wstring lower(std::wstring value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](const wchar_t character) {
                   return static_cast<wchar_t>(std::towlower(character));
                 });
  return value;
}

void rebuild_filter(BrushPaletteState& state) noexcept {
  state.filtered_brushes.clear();
  const EditorDocument* const document = active_document(state.mdi_client);
  if (document == nullptr || state.layer != EditorLayer::units) {
    return;
  }
  try {
    const std::wstring needle = lower(state.query);
    for (const std::uint16_t brush :
         document->object_brushes(EditorLayer::units)) {
      const std::string_view source =
          document->object_brush_name(EditorLayer::units, brush);
      std::wstring name{source.begin(), source.end()};
      name = lower(std::move(name));
      const std::wstring number = std::to_wstring(brush);
      if (needle.empty() || name.find(needle) != std::wstring::npos ||
          number.find(needle) != std::wstring::npos) {
        state.filtered_brushes.push_back(brush);
      }
    }
  } catch (...) {
    state.filtered_brushes.clear();
  }
}

void update_scrollbar(const HWND window, BrushPaletteState& state) noexcept {
  RECT client{};
  GetClientRect(window, &client);
  const EditorDocument* const document = active_document(state.mdi_client);
  const std::size_t items = document == nullptr
                                ? 0U
                                : visible_brushes(state, *document).size();
  const int columns = column_count(window, state);
  const int rows = static_cast<int>(
      (items + static_cast<std::size_t>(columns) - 1U) /
      static_cast<std::size_t>(columns));
  const int visible_height =
      (std::max)(0L, client.bottom - header_height(state));
  SCROLLINFO info{};
  info.cbSize = sizeof(info);
  info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
  info.nMin = 0;
  info.nMax = (std::max)(0, rows - 1);
  info.nPage = static_cast<UINT>(
      (std::max)(1, visible_height / cell_height(state)));
  info.nPos = state.first_row;
  SetScrollInfo(window, SB_VERT, &info, TRUE);
  info.fMask = SIF_POS;
  GetScrollInfo(window, SB_VERT, &info);
  state.first_row = info.nPos;
}

void scroll_palette(const HWND window,
                    BrushPaletteState& state,
                    const int request,
                    const int track_position) noexcept {
  SCROLLINFO info{};
  info.cbSize = sizeof(info);
  info.fMask = SIF_ALL;
  if (GetScrollInfo(window, SB_VERT, &info) == FALSE) {
    return;
  }
  int position = info.nPos;
  switch (request) {
    case SB_LINEUP:
      --position;
      break;
    case SB_LINEDOWN:
      ++position;
      break;
    case SB_PAGEUP:
      position -= static_cast<int>(info.nPage);
      break;
    case SB_PAGEDOWN:
      position += static_cast<int>(info.nPage);
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      position = track_position;
      break;
    case SB_TOP:
      position = info.nMin;
      break;
    case SB_BOTTOM:
      position = info.nMax;
      break;
    default:
      return;
  }
  info.fMask = SIF_POS;
  info.nPos = position;
  SetScrollInfo(window, SB_VERT, &info, TRUE);
  GetScrollInfo(window, SB_VERT, &info);
  state.first_row = info.nPos;
  InvalidateRect(window, nullptr, FALSE);
}

void draw_tile(const HDC dc,
               const int x,
               const int y,
               const int size,
               const EditorDocument::TilePixels& pixels) noexcept {
  BITMAPINFO bitmap{};
  bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmap.bmiHeader.biWidth = static_cast<LONG>(EditorDocument::tile_width);
  bitmap.bmiHeader.biHeight = -static_cast<LONG>(EditorDocument::tile_height);
  bitmap.bmiHeader.biPlanes = 1;
  bitmap.bmiHeader.biBitCount = 32;
  bitmap.bmiHeader.biCompression = BI_RGB;
  StretchDIBits(dc, x, y, size, size, 0, 0,
                static_cast<int>(EditorDocument::tile_width),
                static_cast<int>(EditorDocument::tile_height), pixels.data(),
                &bitmap, DIB_RGB_COLORS, SRCCOPY);
}

void draw_pixels(const HDC dc,
                 const int x,
                 const int y,
                 const int width,
                 const int height,
                 const std::vector<std::uint32_t>& pixels) noexcept {
  if (pixels.size() != static_cast<std::size_t>(width) * height) {
    return;
  }
  BITMAPINFO bitmap{};
  bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmap.bmiHeader.biWidth = width;
  bitmap.bmiHeader.biHeight = -height;
  bitmap.bmiHeader.biPlanes = 1;
  bitmap.bmiHeader.biBitCount = 32;
  bitmap.bmiHeader.biCompression = BI_RGB;
  StretchDIBits(dc, x, y, width, height, 0, 0, width, height, pixels.data(),
                &bitmap, DIB_RGB_COLORS, SRCCOPY);
}

std::uint32_t system_bgra(const int color_index) noexcept {
  const COLORREF color = GetSysColor(color_index);
  return 0xFF000000U | static_cast<std::uint32_t>(GetBValue(color)) |
         (static_cast<std::uint32_t>(GetGValue(color)) << 8U) |
         (static_cast<std::uint32_t>(GetRValue(color)) << 16U);
}

void paint_palette(const HWND window, const BrushPaletteState& state) noexcept {
  PAINTSTRUCT paint{};
  const HDC dc = BeginPaint(window, &paint);
  RECT client{};
  GetClientRect(window, &client);
  FillRect(dc, &client,
           reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_BTNFACE + 1)));

  const EditorDocument* const document = active_document(state.mdi_client);
  std::wstring header = editor_layer_name(state.layer);
  if (state.layer != EditorLayer::locations && state.layer != EditorLayer::fog) {
    header += L" Brushes";
  }
  if (document != nullptr) {
    if (state.layer == EditorLayer::locations) {
      header += L" (";
      header += std::to_wstring(document->location_markers().size());
      header += L")";
    } else if (state.layer != EditorLayer::fog) {
      header += L" (";
      if (state.layer == EditorLayer::units && !state.query.empty()) {
        header += std::to_wstring(state.filtered_brushes.size());
        header += L"/";
      }
      header += std::to_wstring(document->object_brushes(state.layer).size());
      header += L")";
    }
  }
  RECT header_rect{6, 0, client.right - 4, kHeaderHeight};
  SetBkMode(dc, TRANSPARENT);
  DrawTextW(dc, header.c_str(), static_cast<int>(header.size()), &header_rect,
            DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
  MoveToEx(dc, 0, header_height(state) - 1, nullptr);
  LineTo(dc, client.right, header_height(state) - 1);

  if (document == nullptr) {
    RECT empty{8, header_height(state) + 8, client.right - 8,
               client.bottom - 8};
    DrawTextW(dc, L"Open a scenario to view its terrain tiles.", -1, &empty,
              DT_LEFT | DT_TOP | DT_WORDBREAK);
    EndPaint(window, &paint);
    return;
  }

  if (state.layer == EditorLayer::locations || state.layer == EditorLayer::fog) {
    RECT help{8, header_height(state) + 10, client.right - 8,
              client.bottom - 8};
    const wchar_t* const text =
        state.layer == EditorLayer::locations
            ? L"Drag on the map to create a location. Hover a location and press Delete to remove it."
            : L"Drag on the map to add fog. Hold Shift while dragging to reveal terrain. Brush size applies.";
    DrawTextW(dc, text, -1, &help, DT_LEFT | DT_TOP | DT_WORDBREAK);
    EndPaint(window, &paint);
    return;
  }

  const auto& brushes = visible_brushes(state, *document);
  const int columns = column_count(window, state);
  const int palette_top = header_height(state);
  const int cell_w = cell_width(state);
  const int cell_h = cell_height(state);
  const int preview_size = tile_size(state);
  const int visible_rows = (std::max)(
      0, (static_cast<int>(client.bottom) - palette_top + cell_h - 1) /
             cell_h);
  const std::size_t first =
      static_cast<std::size_t>(state.first_row) * columns;
  const std::size_t last = (std::min)(
      brushes.size(),
      first + static_cast<std::size_t>(visible_rows) * columns);
  for (std::size_t index = first; index < last; ++index) {
    const std::size_t relative = index - first;
    const int column = static_cast<int>(relative % columns);
    const int row = static_cast<int>(relative / columns);
    const int left = column * cell_w;
    const int top = palette_top + row * cell_h;
    const std::uint16_t tile_id = brushes[index];
    const EditorDocument::TilePixels* const tile = state.layer == EditorLayer::terrain
                                                      ? document->tile_pixels(tile_id)
                                                      : nullptr;
    if (tile != nullptr) {
      draw_tile(dc, left + (cell_w - preview_size) / 2, top + 2,
                preview_size, *tile);
    } else if (state.layer != EditorLayer::terrain) {
      const ObjectArtFrame* const art =
          document->object_art(state.layer, tile_id);
      std::vector<std::uint32_t> preview{};
      if (art != nullptr && make_object_art_preview(
                                *art, document->art_palette(),
                                document->team_colors(), 0U,
                                system_bgra(COLOR_BTNFACE), preview_size,
                                preview_size, preview)) {
        draw_pixels(dc, left + (cell_w - preview_size) / 2, top + 2,
                    preview_size, preview_size, preview);
      } else {
        const int art_left = left + (cell_w - preview_size) / 2;
        RECT missing{art_left, top + 2, art_left + preview_size,
                     top + 2 + preview_size};
        FrameRect(dc, &missing, GetSysColorBrush(COLOR_3DSHADOW));
      }
    } else {
      const int art_left = left + (cell_w - preview_size) / 2;
      RECT missing{art_left, top + 2, art_left + preview_size,
                   top + 2 + preview_size};
      FillRect(dc, &missing,
               reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_3DSHADOW + 1)));
    }
    if (state.selected_tile.has_value() && *state.selected_tile == tile_id) {
      const HPEN selection = CreatePen(PS_SOLID, 2, RGB(32, 96, 240));
      if (selection != nullptr) {
        const HGDIOBJ old_pen = SelectObject(dc, selection);
        const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        Rectangle(dc, left + 3, top, left + cell_w - 3,
                  top + preview_size + 6);
        SelectObject(dc, old_brush);
        SelectObject(dc, old_pen);
        DeleteObject(selection);
      }
    }
    const std::string_view brush_name =
        state.layer == EditorLayer::terrain
            ? document->terrain_brush_name(tile_id)
            : document->object_brush_name(state.layer, tile_id);
    std::wstring label = brush_name.empty()
                             ? std::to_wstring(tile_id)
                             : std::wstring{brush_name.begin(),
                                            brush_name.end()};
    if (state.layer == EditorLayer::doodads && brush_name.empty()) {
      label = L"D" + label;
    }
    RECT label_rect{left + 2, top + preview_size + 4, left + cell_w - 2,
                    top + cell_h};
    DrawTextW(dc, label.c_str(), static_cast<int>(label.size()), &label_rect,
              DT_CENTER | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS);
  }
  EndPaint(window, &paint);
}

void select_at(const HWND window,
               BrushPaletteState& state,
               const int x,
               const int y) noexcept {
  const EditorDocument* const document = active_document(state.mdi_client);
  const int header = header_height(state);
  const int cell_w = cell_width(state);
  const int cell_h = cell_height(state);
  if (document == nullptr || y < header || x < 0) {
    return;
  }
  const int columns = column_count(window, state);
  const int column = x / cell_w;
  if (column >= columns) {
    return;
  }
  const int row = state.first_row + (y - header) / cell_h;
  const std::size_t index = static_cast<std::size_t>(row) * columns + column;
  const auto& brushes = visible_brushes(state, *document);
  if (index >= brushes.size()) {
    return;
  }
  state.selected_tile = brushes[index];
  set_active_brush(state.mdi_client, *state.selected_tile);
  InvalidateRect(window, nullptr, FALSE);
}

LRESULT CALLBACK brush_palette_proc(const HWND window,
                                    const UINT message,
                                    const WPARAM wparam,
                                    const LPARAM lparam) noexcept {
  BrushPaletteState* state = state_from_window(window);
  switch (message) {
    case WM_CREATE: {
      const auto* const create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
      state = new (std::nothrow) BrushPaletteState{};
      if (state == nullptr) {
        return -1;
      }
      state->mdi_client = reinterpret_cast<HWND>(create->lpCreateParams);
      SetWindowLongPtrW(window, GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>(state));
      state->search = CreateWindowExW(
          0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
          6, 32, 0, 22, window,
          reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_BRUSH_SEARCH)),
          create->hInstance, nullptr);
      if (state->search != nullptr) {
        SendMessageW(state->search, WM_SETFONT,
                     reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),
                     TRUE);
        SendMessageW(state->search, EM_SETCUEBANNER, TRUE,
                     reinterpret_cast<LPARAM>(L"Search units..."));
      }
      update_scrollbar(window, *state);
      return 0;
    }
    case WM_SIZE:
      if (state != nullptr) {
        if (state->search != nullptr) {
          MoveWindow(state->search, 6, 32,
                     (std::max)(0, static_cast<int>(LOWORD(lparam)) - 12), 22,
                     TRUE);
        }
        update_scrollbar(window, *state);
      }
      return 0;
    case WM_COMMAND:
      if (state != nullptr && LOWORD(wparam) == IDC_BRUSH_SEARCH &&
          HIWORD(wparam) == EN_CHANGE && state->search != nullptr) {
        const int length = GetWindowTextLengthW(state->search);
        try {
          std::wstring query(static_cast<std::size_t>(length) + 1U, L'\0');
          GetWindowTextW(state->search, query.data(), length + 1);
          query.resize(static_cast<std::size_t>(length));
          state->query = std::move(query);
        } catch (...) {
          state->query.clear();
        }
        state->first_row = 0;
        rebuild_filter(*state);
        update_scrollbar(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_VSCROLL:
      if (state != nullptr) {
        SCROLLINFO track{};
        track.cbSize = sizeof(track);
        track.fMask = SIF_TRACKPOS;
        GetScrollInfo(window, SB_VERT, &track);
        scroll_palette(window, *state, LOWORD(wparam), track.nTrackPos);
      }
      return 0;
    case WM_MOUSEWHEEL:
      if (state != nullptr) {
        scroll_palette(window, *state, SB_THUMBPOSITION,
                       state->first_row -
                           GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA * 3);
      }
      return 0;
    case WM_LBUTTONDOWN:
      if (state != nullptr) {
        select_at(window, *state, static_cast<short>(LOWORD(lparam)),
                  static_cast<short>(HIWORD(lparam)));
      }
      return 0;
    case WM_RBUTTONDOWN:
      if (state != nullptr) {
        state->selected_tile.reset();
        clear_active_brush(state->mdi_client);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT:
      if (state != nullptr) {
        paint_palette(window, *state);
        return 0;
      }
      break;
    case kRefreshPalette:
      if (state != nullptr) {
        state->first_row = 0;
        state->selected_tile.reset();
        rebuild_filter(*state);
        update_scrollbar(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kSetPaletteSelection:
      if (state != nullptr) {
        state->selected_tile = normalize_selection(
            *state, static_cast<std::uint16_t>(wparam));
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kSetPaletteLayer:
      if (state != nullptr &&
          wparam <= static_cast<WPARAM>(EditorLayer::fog)) {
        state->layer = static_cast<EditorLayer>(wparam);
        state->first_row = 0;
        state->selected_tile.reset();
        rebuild_filter(*state);
        if (state->search != nullptr) {
          ShowWindow(state->search,
                     state->layer == EditorLayer::units ? SW_SHOW : SW_HIDE);
        }
        update_scrollbar(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kClearPaletteSelection:
      if (state != nullptr) {
        state->selected_tile.reset();
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kGetPaletteSelection:
      if (state != nullptr && state->selected_tile.has_value() &&
          lparam != 0) {
        *reinterpret_cast<std::uint16_t*>(lparam) = *state->selected_tile;
        return 1;
      }
      return 0;
    case WM_NCDESTROY:
      SetWindowLongPtrW(window, GWLP_USERDATA, 0);
      delete state;
      break;
    default:
      break;
  }
  return DefWindowProcW(window, message, wparam, lparam);
}

}  // namespace

bool register_brush_palette_class(const HINSTANCE instance) noexcept {
  WNDCLASSEXW window_class{};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = brush_palette_proc;
  window_class.hInstance = instance;
  window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  window_class.hbrBackground =
      reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_BTNFACE + 1));
  window_class.lpszClassName = kBrushPaletteClass;
  return RegisterClassExW(&window_class) != 0;
}

HWND create_brush_palette(const HWND parent,
                          const HINSTANCE instance,
                          const HWND mdi_client) noexcept {
  return CreateWindowExW(WS_EX_CLIENTEDGE, kBrushPaletteClass, nullptr,
                         WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS,
                         0, 0, 0, 0, parent, nullptr, instance, mdi_client);
}

void refresh_brush_palette(const HWND palette) noexcept {
  if (palette != nullptr) {
    SendMessageW(palette, kRefreshPalette, 0, 0);
  }
}

void set_brush_palette_layer(const HWND palette,
                             const EditorLayer layer) noexcept {
  if (palette != nullptr) {
    SendMessageW(palette, kSetPaletteLayer, static_cast<WPARAM>(layer), 0);
  }
}

void set_brush_palette_selection(const HWND palette,
                                 const std::uint16_t tile_id) noexcept {
  if (palette != nullptr) {
    SendMessageW(palette, kSetPaletteSelection, tile_id, 0);
  }
}

void clear_brush_palette_selection(const HWND palette) noexcept {
  if (palette != nullptr) {
    SendMessageW(palette, kClearPaletteSelection, 0, 0);
  }
}

bool get_brush_palette_selection(const HWND palette,
                                 std::uint16_t& tile_id) noexcept {
  return palette != nullptr &&
         SendMessageW(palette, kGetPaletteSelection, 0,
                      reinterpret_cast<LPARAM>(&tile_id)) != 0;
}

}  // namespace staredit
