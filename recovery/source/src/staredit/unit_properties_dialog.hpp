#pragma once

#include <cstddef>

#include <windows.h>

namespace staredit {

class EditorDocument;

[[nodiscard]] bool show_unit_properties_dialog(
    HWND parent,
    HINSTANCE instance,
    EditorDocument& document,
    std::size_t unit_index) noexcept;

}  // namespace staredit
