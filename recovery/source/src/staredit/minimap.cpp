#include "minimap.hpp"

#include "document.hpp"
#include "map_view.hpp"

#include <algorithm>
#include <cstdint>
#include <new>
#include <vector>

namespace staredit {
namespace {

constexpr wchar_t kMinimapClass[] = L"StarEditMiniMap";
constexpr UINT kRefreshMinimap = WM_APP + 301U;
constexpr int kHeaderHeight = 24;
constexpr int kMargin = 5;

struct MinimapState {
  HWND mdi_client{};
};

MinimapState* state_from_window(const HWND window) noexcept {
  return reinterpret_cast<MinimapState*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
}

RECT map_rectangle(const RECT& client, const EditorDocument& document) noexcept {
  const int available_width = (std::max)(0L, client.right - 2 * kMargin);
  const int available_height =
      (std::max)(0L, client.bottom - kHeaderHeight - 2 * kMargin);
  if (available_width == 0 || available_height == 0 ||
      document.width() == 0U || document.height() == 0U) {
    return {};
  }
  int width = available_width;
  int height = static_cast<int>(
      static_cast<std::int64_t>(width) * document.height() / document.width());
  if (height > available_height) {
    height = available_height;
    width = static_cast<int>(static_cast<std::int64_t>(height) *
                             document.width() / document.height());
  }
  const int left = (client.right - width) / 2;
  const int top = kHeaderHeight + (available_height - height) / 2 + kMargin;
  return {left, top, left + width, top + height};
}

void paint_minimap(const HWND window, const MinimapState& state) noexcept {
  PAINTSTRUCT paint{};
  const HDC dc = BeginPaint(window, &paint);
  RECT client{};
  GetClientRect(window, &client);
  FillRect(dc, &client,
           reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_BTNFACE + 1)));
  SetBkMode(dc, TRANSPARENT);
  RECT header{6, 0, client.right - 4, kHeaderHeight};
  DrawTextW(dc, L"Minimap", -1, &header,
            DT_LEFT | DT_SINGLELINE | DT_VCENTER);
  const EditorDocument* const document = active_document(state.mdi_client);
  if (document == nullptr) {
    RECT empty{8, kHeaderHeight + 8, client.right - 8, client.bottom - 8};
    DrawTextW(dc, L"No map open", -1, &empty,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    EndPaint(window, &paint);
    return;
  }
  const RECT map = map_rectangle(client, *document);
  const int output_width = map.right - map.left;
  const int output_height = map.bottom - map.top;
  if (output_width <= 0 || output_height <= 0) {
    EndPaint(window, &paint);
    return;
  }
  try {
    std::vector<std::uint32_t> pixels(
        static_cast<std::size_t>(output_width) * output_height,
        0xFF202020U);
    for (int output_y = 0; output_y < output_height; ++output_y) {
      const std::uint16_t tile_y = static_cast<std::uint16_t>(
          static_cast<std::uint64_t>(output_y) * document->height() /
          output_height);
      for (int output_x = 0; output_x < output_width; ++output_x) {
        const std::uint16_t tile_x = static_cast<std::uint16_t>(
            static_cast<std::uint64_t>(output_x) * document->width() /
            output_width);
        std::uint16_t tile_id{};
        const EditorDocument::TilePixels* tile{};
        if (document->tile_at(tile_x, tile_y, tile_id)) {
          tile = document->tile_pixels(tile_id);
        }
        if (tile != nullptr) {
          pixels[static_cast<std::size_t>(output_y) * output_width + output_x] =
              (*tile)[16U * EditorDocument::tile_width + 16U];
        }
      }
    }
    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = output_width;
    bitmap.bmiHeader.biHeight = -output_height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(dc, map.left, map.top, output_width, output_height, 0, 0,
                  output_width, output_height, pixels.data(), &bitmap,
                  DIB_RGB_COLORS, SRCCOPY);
    MapViewport viewport{};
    if (query_active_viewport(state.mdi_client, viewport) &&
        viewport.map_width > 0 && viewport.map_height > 0) {
      RECT visible{
          map.left + viewport.world_x * output_width / viewport.map_width,
          map.top + viewport.world_y * output_height / viewport.map_height,
          map.left + (viewport.world_x + viewport.client_width) * output_width /
                         viewport.map_width,
          map.top + (viewport.world_y + viewport.client_height) * output_height /
                        viewport.map_height};
      visible.right = (std::min)(visible.right, map.right);
      visible.bottom = (std::min)(visible.bottom, map.bottom);
      const HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      if (pen != nullptr) {
        const HGDIOBJ old_pen = SelectObject(dc, pen);
        const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        Rectangle(dc, visible.left, visible.top, visible.right, visible.bottom);
        SelectObject(dc, old_brush);
        SelectObject(dc, old_pen);
        DeleteObject(pen);
      }
    }
  } catch (...) {
    FillRect(dc, &map,
             reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_3DSHADOW + 1)));
  }
  EndPaint(window, &paint);
}

void center_from_client(const HWND window,
                        const MinimapState& state,
                        const int x,
                        const int y) noexcept {
  const EditorDocument* const document = active_document(state.mdi_client);
  if (document == nullptr) {
    return;
  }
  RECT client{};
  GetClientRect(window, &client);
  const RECT map = map_rectangle(client, *document);
  if (x < map.left || y < map.top || x >= map.right || y >= map.bottom) {
    return;
  }
  const int world_x = static_cast<int>(
      static_cast<std::int64_t>(x - map.left) * document->width() *
      EditorDocument::tile_width / (map.right - map.left));
  const int world_y = static_cast<int>(
      static_cast<std::int64_t>(y - map.top) * document->height() *
      EditorDocument::tile_height / (map.bottom - map.top));
  center_active_view(state.mdi_client, world_x, world_y);
  InvalidateRect(window, nullptr, FALSE);
}

LRESULT CALLBACK minimap_proc(const HWND window,
                              const UINT message,
                              const WPARAM wparam,
                              const LPARAM lparam) noexcept {
  MinimapState* state = state_from_window(window);
  switch (message) {
    case WM_CREATE: {
      const auto* const create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
      state = new (std::nothrow) MinimapState{};
      if (state == nullptr) {
        return -1;
      }
      state->mdi_client = reinterpret_cast<HWND>(create->lpCreateParams);
      SetWindowLongPtrW(window, GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>(state));
      return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
      if (state != nullptr &&
          (message == WM_LBUTTONDOWN || (wparam & MK_LBUTTON) != 0U)) {
        center_from_client(window, *state,
                           static_cast<short>(LOWORD(lparam)),
                           static_cast<short>(HIWORD(lparam)));
      }
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT:
      if (state != nullptr) {
        paint_minimap(window, *state);
        return 0;
      }
      break;
    case kRefreshMinimap:
      InvalidateRect(window, nullptr, FALSE);
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

bool register_minimap_class(const HINSTANCE instance) noexcept {
  WNDCLASSEXW window_class{};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = minimap_proc;
  window_class.hInstance = instance;
  window_class.hCursor = LoadCursorW(nullptr, IDC_CROSS);
  window_class.hbrBackground =
      reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_BTNFACE + 1));
  window_class.lpszClassName = kMinimapClass;
  return RegisterClassExW(&window_class) != 0;
}

HWND create_minimap(const HWND parent,
                    const HINSTANCE instance,
                    const HWND mdi_client) noexcept {
  return CreateWindowExW(WS_EX_CLIENTEDGE, kMinimapClass, nullptr,
                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         0, 0, 0, 0, parent, nullptr, instance, mdi_client);
}

void refresh_minimap(const HWND minimap) noexcept {
  if (minimap != nullptr) {
    SendMessageW(minimap, kRefreshMinimap, 0, 0);
  }
}

}  // namespace staredit
