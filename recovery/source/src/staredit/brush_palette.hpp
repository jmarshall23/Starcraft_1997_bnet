#pragma once

#include <cstdint>

#include <windows.h>

#include "editor_layer.hpp"

namespace staredit {

[[nodiscard]] bool register_brush_palette_class(HINSTANCE instance) noexcept;
[[nodiscard]] HWND create_brush_palette(HWND parent,
                                        HINSTANCE instance,
                                        HWND mdi_client) noexcept;
void refresh_brush_palette(HWND palette) noexcept;
void set_brush_palette_layer(HWND palette, EditorLayer layer) noexcept;
void set_brush_palette_selection(HWND palette, std::uint16_t tile_id) noexcept;
void clear_brush_palette_selection(HWND palette) noexcept;
[[nodiscard]] bool get_brush_palette_selection(
    HWND palette,
    std::uint16_t& tile_id) noexcept;

}  // namespace staredit
