#pragma once

#include <cstdint>
#include <memory>

#include <windows.h>

#include "editor_layer.hpp"

namespace staredit {

class EditorDocument;

struct MapViewport {
  int world_x{};
  int world_y{};
  int client_width{};
  int client_height{};
  int map_width{};
  int map_height{};
  int zoom_percent{100};
};

[[nodiscard]] bool register_map_view_class(HINSTANCE instance) noexcept;
[[nodiscard]] HWND create_map_view(
    HWND mdi_client,
    HINSTANCE instance,
    std::unique_ptr<EditorDocument>& document) noexcept;
[[nodiscard]] const EditorDocument* active_document(HWND mdi_client) noexcept;
[[nodiscard]] EditorDocument* active_editor_document(HWND mdi_client) noexcept;
[[nodiscard]] bool undo_active_document(HWND mdi_client) noexcept;
[[nodiscard]] bool redo_active_document(HWND mdi_client) noexcept;
void set_active_grid_visible(HWND mdi_client, bool visible) noexcept;
void set_active_units_visible(HWND mdi_client, bool visible) noexcept;
void set_active_brush(HWND mdi_client, std::uint16_t tile_id) noexcept;
void clear_active_brush(HWND mdi_client) noexcept;
void refresh_active_document_view(HWND mdi_client) noexcept;
void set_active_brush_size(HWND mdi_client, std::uint8_t size) noexcept;
void set_active_layer(HWND mdi_client, EditorLayer layer) noexcept;
[[nodiscard]] bool query_active_viewport(HWND mdi_client,
                                         MapViewport& viewport) noexcept;
void center_active_view(HWND mdi_client, int world_x, int world_y) noexcept;

}  // namespace staredit
