#pragma once

#include <windows.h>

namespace staredit {

[[nodiscard]] bool register_minimap_class(HINSTANCE instance) noexcept;
[[nodiscard]] HWND create_minimap(HWND parent,
                                  HINSTANCE instance,
                                  HWND mdi_client) noexcept;
void refresh_minimap(HWND minimap) noexcept;

}  // namespace staredit
