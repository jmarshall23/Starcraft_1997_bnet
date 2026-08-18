#include "map_view.hpp"

#include "document.hpp"
#include "resource.h"
#include "terrain/terrain_brush.hpp"
#include "unit_properties_dialog.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace staredit {
namespace {

constexpr wchar_t kMapViewClass[] = L"StarEditMap";
constexpr UINT kSetGridVisible = WM_APP + 101U;
constexpr UINT kSetUnitsVisible = WM_APP + 102U;
constexpr UINT kUndoDocument = WM_APP + 103U;
constexpr UINT kRedoDocument = WM_APP + 104U;
constexpr UINT kSetBrushTile = WM_APP + 105U;
constexpr UINT kSetBrushSize = WM_APP + 106U;
constexpr UINT kSetEditorLayer = WM_APP + 107U;
constexpr UINT kQueryViewport = WM_APP + 108U;
constexpr UINT kCenterViewport = WM_APP + 109U;
constexpr UINT kClearBrush = WM_APP + 110U;
constexpr UINT kRefreshDocument = WM_APP + 111U;
constexpr std::array<int, 5> kZoomLevels{{25, 50, 100, 200, 400}};
constexpr std::size_t kDefaultZoomIndex = 2U;

struct MapViewState {
  explicit MapViewState(std::unique_ptr<EditorDocument> value) noexcept
      : document(std::move(value)) {}

  std::unique_ptr<EditorDocument> document{};
  int scroll_x{};
  int scroll_y{};
  int hover_tile_x{-1};
  int hover_tile_y{-1};
  int hover_world_x{-1};
  int hover_world_y{-1};
  std::optional<std::uint16_t> brush_tile{};
  std::optional<std::uint16_t> object_brush{};
  std::optional<std::size_t> selected_unit{};
  EditorLayer layer{EditorLayer::terrain};
  std::size_t zoom_index{kDefaultZoomIndex};
  std::uint8_t brush_size{1U};
  bool show_grid{};
  bool show_units{true};
  bool painting{};
  bool fog_obscured{true};
  bool location_dragging{};
  bool unit_dragging{};
  int unit_drag_offset_x{};
  int unit_drag_offset_y{};
  int location_start_x{};
  int location_start_y{};
  int last_paint_tile_x{-1};
  int last_paint_tile_y{-1};
  std::vector<std::uint32_t> cached_pixels{};
  int cached_width{};
  int cached_height{};
  bool cache_valid{};
};

int zoom_percent(const MapViewState& state) noexcept {
  return kZoomLevels[state.zoom_index];
}

int client_to_world_delta(const int client_distance,
                          const int zoom) noexcept {
  return static_cast<int>(static_cast<std::int64_t>(client_distance) * 100 /
                          zoom);
}

int client_to_world_extent(const int client_distance,
                           const int zoom) noexcept {
  return static_cast<int>(
      (static_cast<std::int64_t>(client_distance) * 100 + zoom - 1) / zoom);
}

int world_to_client_delta(const int world_distance,
                          const int zoom) noexcept {
  return static_cast<int>(static_cast<std::int64_t>(world_distance) * zoom /
                          100);
}

int world_to_client(const int world_coordinate,
                    const int scroll,
                    const int zoom) noexcept {
  return world_to_client_delta(world_coordinate - scroll, zoom);
}

struct CreatePayload {
  std::unique_ptr<EditorDocument>* document{};
};

MapViewState* state_from_window(const HWND window) noexcept {
  return reinterpret_cast<MapViewState*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
}

void notify_document_changed(const HWND window) noexcept {
  const HWND mdi_client = GetParent(window);
  SendMessageW(GetParent(mdi_client), WM_STAREDIT_DOCUMENT_CHANGED, 0, 0);
}

void notify_viewport_changed(const HWND window) noexcept {
  const HWND mdi_client = GetParent(window);
  SendMessageW(GetParent(mdi_client), WM_STAREDIT_VIEWPORT_CHANGED, 0, 0);
}

void notify_brush_changed(const HWND window,
                          const std::uint16_t tile_id) noexcept {
  const HWND mdi_client = GetParent(window);
  SendMessageW(GetParent(mdi_client), WM_STAREDIT_BRUSH_CHANGED, tile_id, 1);
}

void notify_brush_cleared(const HWND window) noexcept {
  const HWND mdi_client = GetParent(window);
  SendMessageW(GetParent(mdi_client), WM_STAREDIT_BRUSH_CHANGED, 0, 0);
}

void invalidate_map_cache(MapViewState& state) noexcept {
  state.cache_valid = false;
}

void update_child_title(const HWND window, const MapViewState& state) noexcept {
  try {
    std::wstring title = state.document->title();
    if (state.document->modified()) {
      title += L" *";
    }
    const std::optional<std::uint16_t> brush =
        state.layer == EditorLayer::terrain ? state.brush_tile
                                            : state.object_brush;
    title += L"  [";
    title += editor_layer_name(state.layer);
    if (brush.has_value()) {
      title += L" ";
      const std::string_view name = state.layer == EditorLayer::terrain
                                        ? state.document->terrain_brush_name(*brush)
                                        : std::string_view{};
      if (name.empty()) {
        title += std::to_wstring(*brush);
      } else {
        title.append(name.begin(), name.end());
      }
      if (state.layer == EditorLayer::terrain && state.brush_size != 1U) {
        title += L"  ";
        title += std::to_wstring(state.brush_size);
        title += L"x";
        title += std::to_wstring(state.brush_size);
      }
    } else if (state.layer == EditorLayer::units &&
               state.selected_unit.has_value()) {
      UnitAttributes attributes{};
      if (state.document->unit_attributes(*state.selected_unit, attributes)) {
        title += L" Selected ";
        const std::string_view name = state.document->object_brush_name(
            EditorLayer::units, attributes.type);
        if (name.empty()) {
          title += std::to_wstring(attributes.type);
        } else {
          title.append(name.begin(), name.end());
        }
      }
    }
    title += L"]";
    SetWindowTextW(window, title.c_str());
  } catch (...) {
    SetWindowTextW(window, state.document->title().c_str());
  }
}

bool tile_from_client(const MapViewState& state,
                      const int client_x,
                      const int client_y,
                      std::uint16_t& tile_x,
                      std::uint16_t& tile_y) noexcept {
  const int zoom = zoom_percent(state);
  const int world_x =
      state.scroll_x + client_to_world_delta(client_x, zoom);
  const int world_y =
      state.scroll_y + client_to_world_delta(client_y, zoom);
  if (world_x < 0 || world_y < 0) {
    return false;
  }
  const int x = world_x / static_cast<int>(EditorDocument::tile_width);
  const int y = world_y / static_cast<int>(EditorDocument::tile_height);
  if (x < 0 || y < 0 || x >= state.document->width() ||
      y >= state.document->height()) {
    return false;
  }
  tile_x = static_cast<std::uint16_t>(x);
  tile_y = static_cast<std::uint16_t>(y);
  return true;
}

bool world_from_client(const MapViewState& state,
                       const int client_x,
                       const int client_y,
                       std::uint16_t& world_x,
                       std::uint16_t& world_y) noexcept {
  const int zoom = zoom_percent(state);
  const int x = state.scroll_x + client_to_world_delta(client_x, zoom);
  const int y = state.scroll_y + client_to_world_delta(client_y, zoom);
  const int map_width = static_cast<int>(state.document->width()) *
                        static_cast<int>(EditorDocument::tile_width);
  const int map_height = static_cast<int>(state.document->height()) *
                         static_cast<int>(EditorDocument::tile_height);
  if (x < 0 || y < 0 || x >= map_width || y >= map_height) {
    return false;
  }
  world_x = static_cast<std::uint16_t>(x);
  world_y = static_cast<std::uint16_t>(y);
  return true;
}

void update_hover(MapViewState& state,
                  const int client_x,
                  const int client_y) noexcept {
  std::uint16_t tile_x{};
  std::uint16_t tile_y{};
  if (tile_from_client(state, client_x, client_y, tile_x, tile_y)) {
    state.hover_tile_x = tile_x;
    state.hover_tile_y = tile_y;
    const int zoom = zoom_percent(state);
    state.hover_world_x =
        state.scroll_x + client_to_world_delta(client_x, zoom);
    state.hover_world_y =
        state.scroll_y + client_to_world_delta(client_y, zoom);
  } else {
    state.hover_tile_x = -1;
    state.hover_tile_y = -1;
    state.hover_world_x = -1;
    state.hover_world_y = -1;
  }
}

bool select_brush(MapViewState& state,
                  const int client_x,
                  const int client_y) noexcept {
  if (state.layer == EditorLayer::locations || state.layer == EditorLayer::fog) {
    return false;
  }
  if (state.layer != EditorLayer::terrain) {
    std::uint16_t world_x{};
    std::uint16_t world_y{};
    std::uint16_t object_type{};
    if (!world_from_client(state, client_x, client_y, world_x, world_y) ||
        !state.document->object_at(state.layer, world_x, world_y,
                                   object_type)) {
      return false;
    }
    state.object_brush = object_type;
    return true;
  }
  std::uint16_t tile_x{};
  std::uint16_t tile_y{};
  std::uint16_t brush_tile{};
  if (!tile_from_client(state, client_x, client_y, tile_x, tile_y) ||
      !state.document->terrain_brush_at(tile_x, tile_y, brush_tile)) {
    return false;
  }
  state.brush_tile = brush_tile;
  return true;
}

bool paint_at(MapViewState& state,
              const int client_x,
              const int client_y) noexcept {
  if (state.layer == EditorLayer::fog) {
    std::uint16_t tile_x{};
    std::uint16_t tile_y{};
    return tile_from_client(state, client_x, client_y, tile_x, tile_y) &&
           state.document->paint_fog(tile_x, tile_y, state.brush_size,
                                     state.fog_obscured);
  }
  if (state.layer == EditorLayer::locations) {
    return false;
  }
  if (state.layer != EditorLayer::terrain) {
    std::uint16_t world_x{};
    std::uint16_t world_y{};
    return state.object_brush.has_value() &&
           world_from_client(state, client_x, client_y, world_x, world_y) &&
           state.document->place_object(state.layer, *state.object_brush,
                                        world_x, world_y);
  }
  if (!state.brush_tile.has_value()) {
    return false;
  }
  std::uint16_t tile_x{};
  std::uint16_t tile_y{};
  if (!tile_from_client(state, client_x, client_y, tile_x, tile_y)) {
    return false;
  }
  if (state.last_paint_tile_x == static_cast<int>(tile_x) &&
      state.last_paint_tile_y == static_cast<int>(tile_y)) {
    return false;
  }
  state.last_paint_tile_x = tile_x;
  state.last_paint_tile_y = tile_y;
  if (state.document->logical_terrain_ready()) {
    return state.document->paint_terrain(
        tile_x, tile_y, *state.brush_tile, state.brush_size);
  }
  const terrain::TerrainBrush brush{*state.brush_tile, state.brush_size,
                                    state.brush_size};
  std::vector<terrain::TerrainStampCell> stamp{};
  if (!terrain::make_rectangular_stamp(
          brush, tile_x, tile_y, state.document->width(),
          state.document->height(), stamp)) {
    return false;
  }
  for (const terrain::TerrainStampCell& cell : stamp) {
    std::uint16_t variant{};
    if (!state.document->terrain_variant(cell.tile_id, cell.x, cell.y,
                                         variant) ||
        !state.document->paint_tile(cell.x, cell.y, variant)) {
      return false;
    }
  }
  return true;
}

void finish_stroke(const HWND window, MapViewState& state) noexcept {
  if (!state.painting) {
    return;
  }
  state.painting = false;
  state.last_paint_tile_x = -1;
  state.last_paint_tile_y = -1;
  if (GetCapture() == window) {
    ReleaseCapture();
  }
  if (state.layer == EditorLayer::fog) {
    (void)state.document->commit_fog_edit();
  } else {
    (void)state.document->commit_tile_edit();
  }
  invalidate_map_cache(state);
  update_child_title(window, state);
  notify_document_changed(window);
  InvalidateRect(window, nullptr, FALSE);
}

void cancel_stroke(const HWND window, MapViewState& state) noexcept {
  if (!state.painting) {
    return;
  }
  state.painting = false;
  state.last_paint_tile_x = -1;
  state.last_paint_tile_y = -1;
  if (state.layer == EditorLayer::fog) {
    state.document->cancel_fog_edit();
  } else {
    state.document->cancel_tile_edit();
  }
  if (GetCapture() == window) {
    ReleaseCapture();
  }
  InvalidateRect(window, nullptr, FALSE);
}

void update_scrollbars(const HWND window, MapViewState& state) noexcept {
  RECT client{};
  if (GetClientRect(window, &client) == FALSE) {
    return;
  }
  const int map_width = static_cast<int>(state.document->width()) *
                        static_cast<int>(EditorDocument::tile_width);
  const int map_height = static_cast<int>(state.document->height()) *
                         static_cast<int>(EditorDocument::tile_height);
  const int zoom = zoom_percent(state);

  SCROLLINFO horizontal{};
  horizontal.cbSize = sizeof(horizontal);
  horizontal.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
  horizontal.nMin = 0;
  horizontal.nMax = (std::max)(map_width - 1, 0);
  horizontal.nPage = static_cast<UINT>((std::max)(
      client_to_world_extent(client.right, zoom), 1));
  horizontal.nPos = state.scroll_x;
  SetScrollInfo(window, SB_HORZ, &horizontal, TRUE);

  SCROLLINFO vertical{};
  vertical.cbSize = sizeof(vertical);
  vertical.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
  vertical.nMin = 0;
  vertical.nMax = (std::max)(map_height - 1, 0);
  vertical.nPage = static_cast<UINT>((std::max)(
      client_to_world_extent(client.bottom, zoom), 1));
  vertical.nPos = state.scroll_y;
  SetScrollInfo(window, SB_VERT, &vertical, TRUE);

  horizontal.fMask = SIF_POS;
  GetScrollInfo(window, SB_HORZ, &horizontal);
  vertical.fMask = SIF_POS;
  GetScrollInfo(window, SB_VERT, &vertical);
  state.scroll_x = horizontal.nPos;
  state.scroll_y = vertical.nPos;
}

void scroll_window_axis(const HWND window,
                        MapViewState& state,
                        const int bar,
                        const int request,
                        const int track_position) noexcept {
  SCROLLINFO info{};
  info.cbSize = sizeof(info);
  info.fMask = SIF_ALL;
  if (GetScrollInfo(window, bar, &info) == FALSE) {
    return;
  }
  int position = info.nPos;
  switch (request) {
    case SB_LINELEFT:
      position -= static_cast<int>(EditorDocument::tile_width);
      break;
    case SB_LINERIGHT:
      position += static_cast<int>(EditorDocument::tile_width);
      break;
    case SB_PAGELEFT:
      position -= static_cast<int>(info.nPage);
      break;
    case SB_PAGERIGHT:
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
  SetScrollInfo(window, bar, &info, TRUE);
  GetScrollInfo(window, bar, &info);
  if (bar == SB_HORZ) {
    state.scroll_x = info.nPos;
  } else {
    state.scroll_y = info.nPos;
  }
  invalidate_map_cache(state);
  InvalidateRect(window, nullptr, FALSE);
}

void zoom_at_client(const HWND window,
                    MapViewState& state,
                    const int wheel_delta,
                    const int client_x,
                    const int client_y) noexcept {
  if (wheel_delta == 0) {
    return;
  }
  const int previous_zoom = zoom_percent(state);
  const int anchor_world_x =
      state.scroll_x + client_to_world_delta(client_x, previous_zoom);
  const int anchor_world_y =
      state.scroll_y + client_to_world_delta(client_y, previous_zoom);
  const int steps = (std::max)(1, std::abs(wheel_delta) / WHEEL_DELTA);
  for (int step = 0; step < steps; ++step) {
    if (wheel_delta > 0 && state.zoom_index + 1U < kZoomLevels.size()) {
      ++state.zoom_index;
    } else if (wheel_delta < 0 && state.zoom_index != 0U) {
      --state.zoom_index;
    }
  }
  const int next_zoom = zoom_percent(state);
  if (next_zoom == previous_zoom) {
    return;
  }
  state.scroll_x =
      anchor_world_x - client_to_world_delta(client_x, next_zoom);
  state.scroll_y =
      anchor_world_y - client_to_world_delta(client_y, next_zoom);
  update_scrollbars(window, state);
  invalidate_map_cache(state);
  update_hover(state, client_x, client_y);
  InvalidateRect(window, nullptr, FALSE);
  notify_viewport_changed(window);
}

std::uint32_t fallback_tile_color(const std::uint16_t tile_id,
                                  const std::size_t x,
                                  const std::size_t y) noexcept {
  const std::uint32_t shade = ((x / 8U + y / 8U) & 1U) != 0U ? 18U : 0U;
  const std::uint32_t red = 48U + ((tile_id * 13U + shade) & 63U);
  const std::uint32_t green = 48U + ((tile_id * 7U + shade) & 63U);
  const std::uint32_t blue = 48U + ((tile_id * 3U + shade) & 63U);
  return 0xFF000000U | blue | (green << 8U) | (red << 16U);
}

void draw_units(const HDC dc, const MapViewState& state) noexcept {
  constexpr std::array<COLORREF, 12> colors{{
      RGB(244, 44, 36), RGB(36, 72, 224), RGB(44, 180, 168),
      RGB(136, 64, 156), RGB(244, 140, 28), RGB(112, 72, 40),
      RGB(224, 224, 224), RGB(244, 220, 44), RGB(36, 156, 64),
      RGB(216, 180, 52), RGB(160, 132, 96), RGB(96, 196, 232),
  }};
  const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
  const int zoom = zoom_percent(state);
  for (const UnitMarker& unit : state.document->unit_markers()) {
    if (state.document->object_art(EditorLayer::units, unit.type) != nullptr) {
      continue;
    }
    const int x = world_to_client(unit.x, state.scroll_x, zoom);
    const int y = world_to_client(unit.y, state.scroll_y, zoom);
    const COLORREF color = colors[unit.owner % colors.size()];
    const HPEN pen = CreatePen(PS_SOLID, unit.type == 214U ? 2 : 1, color);
    if (pen == nullptr) {
      continue;
    }
    const HGDIOBJ old_pen = SelectObject(dc, pen);
    if (unit.type == 214U) {
      Rectangle(dc, x - 8, y - 8, x + 9, y + 9);
      MoveToEx(dc, x - 11, y, nullptr);
      LineTo(dc, x + 12, y);
      MoveToEx(dc, x, y - 11, nullptr);
      LineTo(dc, x, y + 12);
    } else {
      Ellipse(dc, x - 4, y - 4, x + 5, y + 5);
    }
    SelectObject(dc, old_pen);
    DeleteObject(pen);
  }
  SelectObject(dc, old_brush);
}

void draw_unit_selection(const HDC dc, const MapViewState& state) noexcept {
  if (state.layer != EditorLayer::units || !state.selected_unit.has_value() ||
      *state.selected_unit >= state.document->unit_markers().size()) {
    return;
  }
  const UnitMarker& unit = state.document->unit_markers()[*state.selected_unit];
  int center_world_x = unit.x;
  int center_world_y = unit.y;
  if (state.unit_dragging && state.hover_world_x >= 0 &&
      state.hover_world_y >= 0) {
    center_world_x = state.hover_world_x - state.unit_drag_offset_x;
    center_world_y = state.hover_world_y - state.unit_drag_offset_y;
  }
  int half_width = 12;
  int half_height = 12;
  if (const ObjectArtFrame* const art =
          state.document->object_art(EditorLayer::units, unit.type)) {
    half_width = (std::max)(half_width,
                            static_cast<int>(art->canvas_width) / 2);
    half_height = (std::max)(half_height,
                             static_cast<int>(art->canvas_height) / 2);
  }
  const int zoom = zoom_percent(state);
  const int left = world_to_client(center_world_x - half_width, state.scroll_x,
                                   zoom);
  const int top = world_to_client(center_world_y - half_height, state.scroll_y,
                                  zoom);
  const int right = world_to_client(center_world_x + half_width,
                                    state.scroll_x, zoom);
  const int bottom = world_to_client(center_world_y + half_height,
                                     state.scroll_y, zoom);
  const HPEN pen = CreatePen(PS_SOLID, 2, RGB(64, 160, 255));
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
  Rectangle(dc, left, top, right, bottom);
  const int corner = 7;
  MoveToEx(dc, left, top + corner, nullptr);
  LineTo(dc, left, top);
  LineTo(dc, left + corner, top);
  MoveToEx(dc, right - corner, top, nullptr);
  LineTo(dc, right, top);
  LineTo(dc, right, top + corner);
  MoveToEx(dc, right, bottom - corner, nullptr);
  LineTo(dc, right, bottom);
  LineTo(dc, right - corner, bottom);
  MoveToEx(dc, left + corner, bottom, nullptr);
  LineTo(dc, left, bottom);
  LineTo(dc, left, bottom - corner);
  SelectObject(dc, old_brush);
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

bool edit_selected_unit(const HWND window, MapViewState& state) noexcept {
  if (state.layer != EditorLayer::units || !state.selected_unit.has_value() ||
      *state.selected_unit >= state.document->unit_markers().size()) {
    return false;
  }
  if (!show_unit_properties_dialog(window, GetModuleHandleW(nullptr),
                                   *state.document, *state.selected_unit)) {
    return false;
  }
  invalidate_map_cache(state);
  update_child_title(window, state);
  notify_document_changed(window);
  InvalidateRect(window, nullptr, FALSE);
  return true;
}

void draw_doodads(const HDC dc, const MapViewState& state) noexcept {
  const HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 220, 48));
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
  const int zoom = zoom_percent(state);
  for (const DoodadMarker& doodad : state.document->doodad_markers()) {
    const int x = world_to_client(doodad.x, state.scroll_x, zoom);
    const int y = world_to_client(doodad.y, state.scroll_y, zoom);
    Rectangle(dc, x - 8, y - 8, x + 9, y + 9);
  }
  SelectObject(dc, old_brush);
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

void draw_sprites(const HDC dc, const MapViewState& state) noexcept {
  const HPEN pen = CreatePen(PS_SOLID, 2, RGB(240, 72, 224));
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const int zoom = zoom_percent(state);
  for (const SpriteMarker& sprite : state.document->sprite_markers()) {
    if (state.document->object_art(EditorLayer::sprites, sprite.type) !=
        nullptr) {
      continue;
    }
    const int x = world_to_client(sprite.x, state.scroll_x, zoom);
    const int y = world_to_client(sprite.y, state.scroll_y, zoom);
    POINT diamond[5]{{x, y - 6}, {x + 6, y}, {x, y + 6},
                     {x - 6, y}, {x, y - 6}};
    Polyline(dc, diamond, static_cast<int>(std::size(diamond)));
  }
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

void draw_locations(const HDC dc, const MapViewState& state) noexcept {
  const int zoom = zoom_percent(state);
  const HPEN pen = CreatePen(PS_SOLID, 2, RGB(64, 220, 255));
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, RGB(224, 252, 255));
  for (const LocationMarker& marker : state.document->location_markers()) {
    RECT rectangle{
        world_to_client(static_cast<int>(marker.left), state.scroll_x, zoom),
        world_to_client(static_cast<int>(marker.top), state.scroll_y, zoom),
        world_to_client(static_cast<int>(marker.right), state.scroll_x, zoom),
        world_to_client(static_cast<int>(marker.bottom), state.scroll_y, zoom)};
    Rectangle(dc, rectangle.left, rectangle.top, rectangle.right,
              rectangle.bottom);
    std::wstring label = L"Location " + std::to_wstring(marker.slot + 1U);
    rectangle.left += 4;
    rectangle.top += 2;
    DrawTextW(dc, label.c_str(), static_cast<int>(label.size()), &rectangle,
              DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
  }
  if (state.location_dragging && state.hover_world_x >= 0 &&
      state.hover_world_y >= 0) {
    const int left = world_to_client(
        (std::min)(state.location_start_x, state.hover_world_x), state.scroll_x,
        zoom);
    const int top = world_to_client(
        (std::min)(state.location_start_y, state.hover_world_y), state.scroll_y,
        zoom);
    const int right = world_to_client(
        (std::max)(state.location_start_x, state.hover_world_x), state.scroll_x,
        zoom);
    const int bottom = world_to_client(
        (std::max)(state.location_start_y, state.hover_world_y), state.scroll_y,
        zoom);
    Rectangle(dc, left, top, right, bottom);
  }
  SelectObject(dc, old_brush);
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

void composite_objects(std::vector<std::uint32_t>& pixels,
                       const int width,
                       const int height,
                       const MapViewState& state,
                       const bool include_static,
                       const bool include_preview) noexcept {
  const int zoom = zoom_percent(state);
  if (include_static && state.show_units) {
    for (const UnitMarker& unit : state.document->unit_markers()) {
      const ObjectArtFrame* const art =
          state.document->object_art(EditorLayer::units, unit.type);
      if (art != nullptr) {
        composite_object_art_scaled(
            *art, state.document->art_palette(), state.document->team_colors(),
            unit.owner, pixels, width, height,
            world_to_client(unit.x, state.scroll_x, zoom),
            world_to_client(unit.y, state.scroll_y, zoom), zoom);
      }
    }
  }
  if (include_static && state.layer == EditorLayer::sprites) {
    for (const SpriteMarker& sprite : state.document->sprite_markers()) {
      const ObjectArtFrame* const art =
          state.document->object_art(EditorLayer::sprites, sprite.type);
      if (art != nullptr) {
        composite_object_art_scaled(
            *art, state.document->art_palette(), state.document->team_colors(),
            sprite.owner, pixels, width, height,
            world_to_client(sprite.x, state.scroll_x, zoom),
            world_to_client(sprite.y, state.scroll_y, zoom), zoom);
      }
    }
  }
  if (include_preview && state.layer != EditorLayer::terrain &&
      state.layer != EditorLayer::locations && state.layer != EditorLayer::fog &&
      state.object_brush.has_value() &&
      state.hover_world_x >= 0 && state.hover_world_y >= 0) {
    const ObjectArtFrame* const art =
        state.document->object_art(state.layer, *state.object_brush);
    if (art != nullptr) {
      composite_object_art_scaled(
          *art, state.document->art_palette(), state.document->team_colors(),
          0U, pixels, width, height,
          world_to_client(state.hover_world_x, state.scroll_x, zoom),
          world_to_client(state.hover_world_y, state.scroll_y, zoom), zoom);
    }
  }
}

void draw_grid(const HWND window, const HDC dc, const MapViewState& state) noexcept {
  RECT client{};
  GetClientRect(window, &client);
  const HPEN pen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const int zoom = zoom_percent(state);
  const int world_size = static_cast<int>(EditorDocument::tile_width);
  const int size = world_to_client_delta(world_size, zoom);
  const int start_x = -world_to_client_delta(state.scroll_x % world_size, zoom);
  const int start_y = -world_to_client_delta(state.scroll_y % world_size, zoom);
  for (int x = start_x; x < client.right; x += size) {
    MoveToEx(dc, x, 0, nullptr);
    LineTo(dc, x, client.bottom);
  }
  for (int y = start_y; y < client.bottom; y += size) {
    MoveToEx(dc, 0, y, nullptr);
    LineTo(dc, client.right, y);
  }
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

void draw_brush_cursor(const HDC dc, const MapViewState& state) noexcept {
  if (state.hover_tile_x < 0 || state.hover_tile_y < 0) {
    return;
  }
  const int zoom = zoom_percent(state);
  const int world_size = static_cast<int>(EditorDocument::tile_width);
  const int size = world_to_client_delta(world_size, zoom);
  if (state.layer == EditorLayer::locations) {
    return;
  }
  if (state.layer != EditorLayer::terrain && state.layer != EditorLayer::fog) {
    const int center_x = world_to_client(
        state.hover_tile_x * world_size + world_size / 2, state.scroll_x,
        zoom);
    const int center_y = world_to_client(
        state.hover_tile_y * world_size + world_size / 2, state.scroll_y,
        zoom);
    const HPEN pen = CreatePen(
        PS_SOLID, 2,
        state.object_brush.has_value() ? RGB(64, 255, 96) : RGB(255, 220, 64));
    if (pen == nullptr) {
      return;
    }
    const HGDIOBJ old_pen = SelectObject(dc, pen);
    MoveToEx(dc, center_x - 9, center_y, nullptr);
    LineTo(dc, center_x + 10, center_y);
    MoveToEx(dc, center_x, center_y - 9, nullptr);
    LineTo(dc, center_x, center_y + 10);
    SelectObject(dc, old_pen);
    DeleteObject(pen);
    return;
  }
  const int radius = state.brush_size / 2;
  const int left = world_to_client(
      (state.hover_tile_x - radius) * world_size, state.scroll_x, zoom);
  const int top = world_to_client(
      (state.hover_tile_y - radius) * world_size, state.scroll_y, zoom);
  const int extent = state.brush_size * size;
  const COLORREF color = state.layer == EditorLayer::fog
                             ? (state.fog_obscured ? RGB(120, 180, 255)
                                                   : RGB(255, 224, 96))
                             : (state.brush_tile.has_value()
                                    ? RGB(64, 255, 96)
                                    : RGB(255, 220, 64));
  const HPEN pen = CreatePen(PS_SOLID, 2, color);
  if (pen == nullptr) {
    return;
  }
  const HGDIOBJ old_pen = SelectObject(dc, pen);
  const HGDIOBJ old_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
  Rectangle(dc, left, top, left + extent, top + extent);
  SelectObject(dc, old_brush);
  SelectObject(dc, old_pen);
  DeleteObject(pen);
}

bool render_cached_map(MapViewState& state,
                       const int client_width,
                       const int client_height) noexcept {
  try {
    state.cached_pixels.assign(
        static_cast<std::size_t>(client_width) * client_height, 0xFF303030U);
    const int zoom = zoom_percent(state);
    const int tile_size = static_cast<int>(EditorDocument::tile_width);
    for (int output_y = 0; output_y < client_height; ++output_y) {
      const int world_y = state.scroll_y +
                          client_to_world_delta(output_y, zoom);
      if (world_y < 0 ||
          world_y >= static_cast<int>(state.document->height()) * tile_size) {
        continue;
      }
      const int tile_y = world_y / tile_size;
      const int source_y = world_y % tile_size;
      int previous_tile_x = -1;
      std::uint16_t tile_id{};
      const EditorDocument::TilePixels* tile{};
      for (int output_x = 0; output_x < client_width; ++output_x) {
        const int world_x = state.scroll_x +
                            client_to_world_delta(output_x, zoom);
        if (world_x < 0 ||
            world_x >= static_cast<int>(state.document->width()) * tile_size) {
          continue;
        }
        const int tile_x = world_x / tile_size;
        if (tile_x != previous_tile_x) {
          previous_tile_x = tile_x;
          tile = state.document->tile_at(static_cast<std::uint16_t>(tile_x),
                                         static_cast<std::uint16_t>(tile_y),
                                         tile_id)
                     ? state.document->tile_pixels(tile_id)
                     : nullptr;
        }
        const int source_x = world_x % tile_size;
        std::uint32_t color =
            tile != nullptr
                ? (*tile)[static_cast<std::size_t>(source_y) * tile_size +
                          source_x]
                : fallback_tile_color(tile_id,
                                      static_cast<std::size_t>(source_x),
                                      static_cast<std::size_t>(source_y));
        if (state.layer == EditorLayer::fog &&
            state.document->fog_at(static_cast<std::uint16_t>(tile_x),
                                   static_cast<std::uint16_t>(tile_y))) {
          color = 0xFF000000U | ((color & 0x00FCFCFCU) >> 2U);
        }
        state.cached_pixels[static_cast<std::size_t>(output_y) * client_width +
                            output_x] = color;
      }
    }
    composite_objects(state.cached_pixels, client_width, client_height, state,
                      true, false);
    state.cached_width = client_width;
    state.cached_height = client_height;
    state.cache_valid = true;
    return true;
  } catch (...) {
    state.cached_pixels.clear();
    state.cache_valid = false;
    return false;
  }
}

void paint_map(const HWND window, MapViewState& state) noexcept {
  PAINTSTRUCT paint{};
  const HDC dc = BeginPaint(window, &paint);
  RECT client{};
  GetClientRect(window, &client);
  const int client_width = client.right - client.left;
  const int client_height = client.bottom - client.top;
  if (client_width <= 0 || client_height <= 0) {
    EndPaint(window, &paint);
    return;
  }

  try {
    if ((!state.cache_valid || state.cached_width != client_width ||
         state.cached_height != client_height) &&
        !render_cached_map(state, client_width, client_height)) {
      throw std::bad_alloc{};
    }
    const bool preview = state.layer != EditorLayer::terrain &&
                         state.layer != EditorLayer::locations &&
                         state.layer != EditorLayer::fog &&
                         state.object_brush.has_value() &&
                         state.hover_world_x >= 0 && state.hover_world_y >= 0;
    const std::vector<std::uint32_t>* pixels = &state.cached_pixels;
    std::vector<std::uint32_t> preview_pixels{};
    if (preview) {
      preview_pixels = state.cached_pixels;
      composite_objects(preview_pixels, client_width, client_height, state,
                        false, true);
      pixels = &preview_pixels;
    }

    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = client_width;
    bitmap.bmiHeader.biHeight = -client_height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    SetDIBitsToDevice(dc, 0, 0, client_width, client_height, 0, 0, 0,
                      client_height, pixels->data(), &bitmap, DIB_RGB_COLORS);
    if (state.show_grid) {
      draw_grid(window, dc, state);
    }
    if (state.show_units) {
      draw_units(dc, state);
    }
    if (state.layer == EditorLayer::doodads) {
      draw_doodads(dc, state);
    } else if (state.layer == EditorLayer::sprites) {
      draw_sprites(dc, state);
    } else if (state.layer == EditorLayer::locations) {
      draw_locations(dc, state);
    }
    draw_unit_selection(dc, state);
    draw_brush_cursor(dc, state);
  } catch (...) {
    FillRect(dc, &client, static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH)));
  }
  EndPaint(window, &paint);
}

LRESULT CALLBACK map_view_proc(const HWND window,
                               const UINT message,
                               const WPARAM wparam,
                               const LPARAM lparam) noexcept {
  MapViewState* state = state_from_window(window);
  switch (message) {
    case WM_CREATE: {
      const auto* const create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
      const auto* const mdi =
          static_cast<const MDICREATESTRUCTW*>(create->lpCreateParams);
      auto* const payload = reinterpret_cast<CreatePayload*>(mdi->lParam);
      if (payload == nullptr || payload->document == nullptr ||
          *payload->document == nullptr) {
        return -1;
      }
      state = new (std::nothrow)
          MapViewState{std::move(*payload->document)};
      if (state == nullptr) {
        return -1;
      }
      SetWindowLongPtrW(window, GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>(state));
      update_scrollbars(window, *state);
      return 0;
    }
    case WM_SIZE:
      if (state != nullptr) {
        update_scrollbars(window, *state);
        invalidate_map_cache(*state);
        notify_viewport_changed(window);
      }
      break;
    case WM_HSCROLL:
    case WM_VSCROLL:
      if (state != nullptr) {
        SCROLLINFO track{};
        track.cbSize = sizeof(track);
        track.fMask = SIF_TRACKPOS;
        const int bar = message == WM_HSCROLL ? SB_HORZ : SB_VERT;
        GetScrollInfo(window, bar, &track);
        scroll_window_axis(window, *state, bar, LOWORD(wparam),
                           track.nTrackPos);
        notify_viewport_changed(window);
      }
      return 0;
    case WM_MOUSEWHEEL:
      if (state != nullptr) {
        const int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        POINT point{static_cast<short>(LOWORD(lparam)),
                    static_cast<short>(HIWORD(lparam))};
        (void)ScreenToClient(window, &point);
        cancel_stroke(window, *state);
        state->unit_dragging = false;
        state->location_dragging = false;
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        zoom_at_client(window, *state, delta, point.x, point.y);
      }
      return 0;
    case WM_MOUSEMOVE:
      if (state != nullptr) {
        const int x = static_cast<short>(LOWORD(lparam));
        const int y = static_cast<short>(HIWORD(lparam));
        update_hover(*state, x, y);
        if (state->painting && (wparam & MK_LBUTTON) != 0U) {
          if (paint_at(*state, x, y)) {
            invalidate_map_cache(*state);
          }
        } else if (state->painting) {
          finish_stroke(window, *state);
        }
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_LBUTTONDOWN:
      if (state != nullptr) {
        SetFocus(window);
        const int x = static_cast<short>(LOWORD(lparam));
        const int y = static_cast<short>(HIWORD(lparam));
        update_hover(*state, x, y);
        if (state->layer == EditorLayer::locations) {
          if (state->hover_world_x >= 0 && state->hover_world_y >= 0) {
            state->location_start_x = state->hover_world_x;
            state->location_start_y = state->hover_world_y;
            state->location_dragging = true;
            SetCapture(window);
          }
        } else if (state->layer == EditorLayer::fog) {
          if (state->document->begin_fog_edit()) {
            state->painting = true;
            state->fog_obscured = (wparam & MK_SHIFT) == 0U;
            SetCapture(window);
            if (paint_at(*state, x, y)) {
              invalidate_map_cache(*state);
            }
          }
        } else if (state->layer == EditorLayer::units &&
                   !state->object_brush.has_value()) {
          std::uint16_t world_x{};
          std::uint16_t world_y{};
          std::size_t selected{};
          if (world_from_client(*state, x, y, world_x, world_y) &&
              state->document->unit_index_at(world_x, world_y, selected)) {
            state->selected_unit = selected;
            const UnitMarker& marker =
                state->document->unit_markers()[selected];
            state->unit_drag_offset_x =
                static_cast<int>(world_x) - marker.x;
            state->unit_drag_offset_y =
                static_cast<int>(world_y) - marker.y;
            state->unit_dragging = true;
            SetCapture(window);
          } else {
            state->selected_unit.reset();
          }
          update_child_title(window, *state);
        } else if (state->layer != EditorLayer::terrain) {
          state->selected_unit.reset();
          if (!state->object_brush.has_value()) {
            if (select_brush(*state, x, y)) {
              update_child_title(window, *state);
              notify_brush_changed(window, *state->object_brush);
            }
          } else if (paint_at(*state, x, y)) {
            invalidate_map_cache(*state);
            update_child_title(window, *state);
            notify_document_changed(window);
            InvalidateRect(window, nullptr, FALSE);
          }
        } else if (!state->brush_tile.has_value()) {
          if (select_brush(*state, x, y)) {
            update_child_title(window, *state);
            notify_brush_changed(window, *state->brush_tile);
          }
        } else if (state->document->begin_tile_edit()) {
          state->painting = true;
          state->last_paint_tile_x = -1;
          state->last_paint_tile_y = -1;
          SetCapture(window);
          if (paint_at(*state, x, y)) {
            invalidate_map_cache(*state);
          }
        }
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_LBUTTONUP:
      if (state != nullptr) {
        if (state->location_dragging) {
          const int end_x = state->hover_world_x;
          const int end_y = state->hover_world_y;
          state->location_dragging = false;
          if (GetCapture() == window) {
            ReleaseCapture();
          }
          if (end_x >= 0 && end_y >= 0 &&
              state->document->create_location(
                  static_cast<std::uint32_t>(state->location_start_x),
                  static_cast<std::uint32_t>(state->location_start_y),
                  static_cast<std::uint32_t>(end_x),
                  static_cast<std::uint32_t>(end_y))) {
            update_child_title(window, *state);
            notify_document_changed(window);
          }
          InvalidateRect(window, nullptr, FALSE);
        } else if (state->unit_dragging) {
          state->unit_dragging = false;
          if (GetCapture() == window) {
            ReleaseCapture();
          }
          if (state->selected_unit.has_value() &&
              *state->selected_unit < state->document->unit_markers().size() &&
              state->hover_world_x >= 0 && state->hover_world_y >= 0) {
            const UnitMarker& original = state->document->unit_markers()[
                *state->selected_unit];
            const int maximum_x =
                static_cast<int>(state->document->width()) *
                    static_cast<int>(EditorDocument::tile_width) -
                1;
            const int maximum_y =
                static_cast<int>(state->document->height()) *
                    static_cast<int>(EditorDocument::tile_height) -
                1;
            const int target_x = (std::clamp)(
                state->hover_world_x - state->unit_drag_offset_x, 0, maximum_x);
            const int target_y = (std::clamp)(
                state->hover_world_y - state->unit_drag_offset_y, 0, maximum_y);
            if ((target_x != original.x || target_y != original.y) &&
                state->document->move_unit(
                    *state->selected_unit,
                    static_cast<std::uint16_t>(target_x),
                    static_cast<std::uint16_t>(target_y))) {
              invalidate_map_cache(*state);
              update_child_title(window, *state);
              notify_document_changed(window);
            }
          }
          InvalidateRect(window, nullptr, FALSE);
        } else {
          finish_stroke(window, *state);
        }
      }
      return 0;
    case WM_LBUTTONDBLCLK:
      if (state != nullptr && state->layer == EditorLayer::units &&
          !state->object_brush.has_value()) {
        const int x = static_cast<short>(LOWORD(lparam));
        const int y = static_cast<short>(HIWORD(lparam));
        std::uint16_t world_x{};
        std::uint16_t world_y{};
        std::size_t selected{};
        update_hover(*state, x, y);
        if (world_from_client(*state, x, y, world_x, world_y) &&
            state->document->unit_index_at(world_x, world_y, selected)) {
          state->selected_unit = selected;
          state->unit_dragging = false;
          if (GetCapture() == window) {
            ReleaseCapture();
          }
          update_child_title(window, *state);
          InvalidateRect(window, nullptr, FALSE);
          (void)edit_selected_unit(window, *state);
        }
      }
      return 0;
    case WM_RBUTTONDOWN:
      if (state != nullptr) {
        cancel_stroke(window, *state);
        state->location_dragging = false;
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        state->brush_tile.reset();
        state->object_brush.reset();
        state->selected_unit.reset();
        state->unit_dragging = false;
        update_child_title(window, *state);
        notify_brush_cleared(window);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_KEYDOWN:
      if (state != nullptr) {
        if (wparam == VK_ESCAPE) {
          cancel_stroke(window, *state);
          state->location_dragging = false;
          state->unit_dragging = false;
          if (GetCapture() == window) {
            ReleaseCapture();
          }
          InvalidateRect(window, nullptr, FALSE);
          return 0;
        }
        if ((wparam == VK_RETURN || wparam == VK_F2) &&
            state->layer == EditorLayer::units) {
          (void)edit_selected_unit(window, *state);
          return 0;
        }
        if (wparam == VK_DELETE && state->layer == EditorLayer::units &&
            state->selected_unit.has_value()) {
          if (state->document->erase_unit(*state->selected_unit)) {
            state->selected_unit.reset();
            invalidate_map_cache(*state);
            update_child_title(window, *state);
            notify_document_changed(window);
            InvalidateRect(window, nullptr, FALSE);
          }
          return 0;
        }
        if (wparam == VK_DELETE && state->layer == EditorLayer::locations) {
          if (state->hover_world_x >= 0 && state->hover_world_y >= 0 &&
              state->document->erase_location_at(
                  static_cast<std::uint16_t>(state->hover_world_x),
                  static_cast<std::uint16_t>(state->hover_world_y))) {
            update_child_title(window, *state);
            notify_document_changed(window);
            InvalidateRect(window, nullptr, FALSE);
          }
          return 0;
        }
        if (wparam == VK_DELETE && state->layer != EditorLayer::terrain &&
            state->layer != EditorLayer::fog) {
          if (state->hover_world_x >= 0 && state->hover_world_y >= 0 &&
              state->document->erase_object_at(
                  state->layer,
                  static_cast<std::uint16_t>(state->hover_world_x),
                  static_cast<std::uint16_t>(state->hover_world_y))) {
            update_child_title(window, *state);
            invalidate_map_cache(*state);
            notify_document_changed(window);
            InvalidateRect(window, nullptr, FALSE);
          }
          return 0;
        }
      }
      break;
    case WM_CAPTURECHANGED:
      if (state != nullptr && state->location_dragging &&
          reinterpret_cast<HWND>(lparam) != window) {
        state->location_dragging = false;
        InvalidateRect(window, nullptr, FALSE);
      }
      if (state != nullptr && state->unit_dragging &&
          reinterpret_cast<HWND>(lparam) != window) {
        state->unit_dragging = false;
        InvalidateRect(window, nullptr, FALSE);
      }
      if (state != nullptr && state->painting &&
          reinterpret_cast<HWND>(lparam) != window) {
        if (state->layer == EditorLayer::fog) {
          state->document->cancel_fog_edit();
        } else {
          state->document->cancel_tile_edit();
        }
        state->painting = false;
        invalidate_map_cache(*state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT:
      if (state != nullptr) {
        paint_map(window, *state);
        return 0;
      }
      break;
    case kSetGridVisible:
      if (state != nullptr) {
        state->show_grid = wparam != 0U;
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kSetUnitsVisible:
      if (state != nullptr) {
        state->show_units = wparam != 0U;
        invalidate_map_cache(*state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kUndoDocument:
      if (state != nullptr) {
        cancel_stroke(window, *state);
        const bool changed = state->document->undo();
        if (changed) {
          state->selected_unit.reset();
          invalidate_map_cache(*state);
          update_child_title(window, *state);
          notify_document_changed(window);
          InvalidateRect(window, nullptr, FALSE);
        }
        return changed ? 1 : 0;
      }
      return 0;
    case kRedoDocument:
      if (state != nullptr) {
        cancel_stroke(window, *state);
        const bool changed = state->document->redo();
        if (changed) {
          state->selected_unit.reset();
          invalidate_map_cache(*state);
          update_child_title(window, *state);
          notify_document_changed(window);
          InvalidateRect(window, nullptr, FALSE);
        }
        return changed ? 1 : 0;
      }
      return 0;
    case kSetBrushTile:
      if (state != nullptr) {
        const std::uint16_t brush = static_cast<std::uint16_t>(wparam);
        const auto& choices = state->document->object_brushes(state->layer);
        const bool valid = state->layer == EditorLayer::terrain
                               ? state->document->valid_terrain_tile(brush)
                               : std::find(choices.begin(), choices.end(),
                                           brush) != choices.end();
        if (!valid) {
          return 0;
        }
        if (state->layer == EditorLayer::terrain) {
          state->brush_tile = brush;
        } else {
          state->object_brush = brush;
          state->selected_unit.reset();
        }
        update_child_title(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kSetBrushSize:
      if (state != nullptr) {
        const auto size = static_cast<std::uint8_t>(wparam);
        if (terrain::valid_brush_dimensions(
                terrain::TerrainBrush{0U, size, size})) {
          state->brush_size = size;
          update_child_title(window, *state);
          InvalidateRect(window, nullptr, FALSE);
        }
      }
      return 0;
    case kClearBrush:
      if (state != nullptr) {
        state->brush_tile.reset();
        state->object_brush.reset();
        state->selected_unit.reset();
        state->unit_dragging = false;
        state->selected_unit.reset();
        state->unit_dragging = false;
        update_child_title(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kRefreshDocument:
      if (state != nullptr) {
        invalidate_map_cache(*state);
        update_child_title(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kSetEditorLayer:
      if (state != nullptr && wparam <= static_cast<WPARAM>(EditorLayer::fog)) {
        cancel_stroke(window, *state);
        state->layer = static_cast<EditorLayer>(wparam);
        state->object_brush.reset();
        state->location_dragging = false;
        invalidate_map_cache(*state);
        update_child_title(window, *state);
        InvalidateRect(window, nullptr, FALSE);
      }
      return 0;
    case kQueryViewport:
      if (state != nullptr && lparam != 0) {
        RECT client{};
        GetClientRect(window, &client);
        auto* const viewport = reinterpret_cast<MapViewport*>(lparam);
        const int zoom = zoom_percent(*state);
        viewport->world_x = state->scroll_x;
        viewport->world_y = state->scroll_y;
        viewport->client_width =
            client_to_world_extent(client.right, zoom);
        viewport->client_height =
            client_to_world_extent(client.bottom, zoom);
        viewport->map_width = static_cast<int>(state->document->width()) *
                              static_cast<int>(EditorDocument::tile_width);
        viewport->map_height = static_cast<int>(state->document->height()) *
                               static_cast<int>(EditorDocument::tile_height);
        viewport->zoom_percent = zoom;
        return 1;
      }
      return 0;
    case kCenterViewport:
      if (state != nullptr) {
        RECT client{};
        GetClientRect(window, &client);
        const int zoom = zoom_percent(*state);
        state->scroll_x = static_cast<int>(wparam) -
                          client_to_world_extent(client.right, zoom) / 2;
        state->scroll_y = static_cast<int>(lparam) -
                          client_to_world_extent(client.bottom, zoom) / 2;
        update_scrollbars(window, *state);
        invalidate_map_cache(*state);
        InvalidateRect(window, nullptr, FALSE);
        notify_viewport_changed(window);
      }
      return 0;
    case WM_MDIACTIVATE:
      if (reinterpret_cast<HWND>(lparam) == window) {
        const HWND mdi_client = GetParent(window);
        const std::optional<std::uint16_t> brush =
            state == nullptr
                ? std::nullopt
                : (state->layer == EditorLayer::terrain ? state->brush_tile
                                                        : state->object_brush);
        SendMessageW(GetParent(mdi_client), WM_STAREDIT_DOCUMENT_ACTIVATED,
                     brush.has_value() ? *brush : 0U,
                     brush.has_value() ? 1 : 0);
      }
      break;
    case WM_CLOSE:
      if (state != nullptr && state->document->modified()) {
        const int choice = MessageBoxW(
            window,
            L"This map has unsaved edits. Use File > Save to write a retail "
            L"SCX archive.\n\nDiscard these edits and close the map?",
            L"Unsaved map edits", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
        if (choice != IDYES) {
          return 0;
        }
      }
      break;
    case WM_NCDESTROY:
      SetWindowLongPtrW(window, GWLP_USERDATA, 0);
      delete state;
      break;
    default:
      break;
  }
  return DefMDIChildProcW(window, message, wparam, lparam);
}

}  // namespace

bool register_map_view_class(const HINSTANCE instance) noexcept {
  WNDCLASSEXW window_class{};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  window_class.lpfnWndProc = map_view_proc;
  window_class.cbWndExtra = 0;
  window_class.hInstance = instance;
  window_class.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_STAREDIT_ICON));
  window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  window_class.hbrBackground = nullptr;
  window_class.lpszClassName = kMapViewClass;
  window_class.hIconSm = window_class.hIcon;
  return RegisterClassExW(&window_class) != 0;
}

HWND create_map_view(HWND mdi_client,
                     const HINSTANCE instance,
                     std::unique_ptr<EditorDocument>& document) noexcept {
  if (mdi_client == nullptr || document == nullptr) {
    return nullptr;
  }
  CreatePayload payload{&document};
  MDICREATESTRUCTW create{};
  create.szClass = kMapViewClass;
  create.szTitle = document->title().c_str();
  create.hOwner = instance;
  create.x = CW_USEDEFAULT;
  create.y = CW_USEDEFAULT;
  create.cx = CW_USEDEFAULT;
  create.cy = CW_USEDEFAULT;
  create.style = WS_HSCROLL | WS_VSCROLL;
  create.lParam = reinterpret_cast<LPARAM>(&payload);
  const HWND child = reinterpret_cast<HWND>(SendMessageW(
      mdi_client, WM_MDICREATE, 0, reinterpret_cast<LPARAM>(&create)));
  if (child != nullptr) {
    SendMessageW(mdi_client, WM_MDIMAXIMIZE,
                 reinterpret_cast<WPARAM>(child), 0);
  }
  return child;
}

const EditorDocument* active_document(const HWND mdi_client) noexcept {
  return active_editor_document(mdi_client);
}

EditorDocument* active_editor_document(const HWND mdi_client) noexcept {
  if (mdi_client == nullptr) {
    return nullptr;
  }
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  const MapViewState* const state = state_from_window(child);
  return state == nullptr ? nullptr : state->document.get();
}

bool undo_active_document(const HWND mdi_client) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  return child != nullptr && SendMessageW(child, kUndoDocument, 0, 0) != 0;
}

bool redo_active_document(const HWND mdi_client) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  return child != nullptr && SendMessageW(child, kRedoDocument, 0, 0) != 0;
}

void set_active_grid_visible(const HWND mdi_client, const bool visible) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kSetGridVisible, visible ? 1U : 0U, 0);
  }
}

void set_active_units_visible(const HWND mdi_client, const bool visible) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kSetUnitsVisible, visible ? 1U : 0U, 0);
  }
}

void set_active_brush(const HWND mdi_client,
                      const std::uint16_t tile_id) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kSetBrushTile, tile_id, 0);
  }
}

void clear_active_brush(const HWND mdi_client) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kClearBrush, 0, 0);
  }
}

void refresh_active_document_view(const HWND mdi_client) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kRefreshDocument, 0, 0);
  }
}

void set_active_brush_size(const HWND mdi_client,
                           const std::uint8_t size) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kSetBrushSize, size, 0);
  }
}

void set_active_layer(const HWND mdi_client, const EditorLayer layer) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kSetEditorLayer, static_cast<WPARAM>(layer), 0);
  }
}

bool query_active_viewport(const HWND mdi_client,
                           MapViewport& viewport) noexcept {
  viewport = {};
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  return child != nullptr &&
         SendMessageW(child, kQueryViewport, 0,
                      reinterpret_cast<LPARAM>(&viewport)) != 0;
}

void center_active_view(const HWND mdi_client,
                        const int world_x,
                        const int world_y) noexcept {
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(mdi_client, WM_MDIGETACTIVE, 0, 0));
  if (child != nullptr) {
    SendMessageW(child, kCenterViewport, static_cast<WPARAM>(world_x),
                 static_cast<LPARAM>(world_y));
  }
}

}  // namespace staredit
