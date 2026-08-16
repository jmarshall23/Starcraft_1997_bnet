#pragma once

#include <cstdint>

#include "starcraft/gds/meta_bitmap.hpp"

namespace starcraft::gds {

// Recovered from U:\SWAR\lang\GDS\bmpcle.cpp, original VA 0x004D4AF0.
// Returns the color replicated into a 16-bit word, matching AX on return from
// the original x86 fill routine.
[[nodiscard]] std::uint16_t clear_bitmap(
    MetaBitmap& bitmap,
    std::uint8_t color) noexcept;

}  // namespace starcraft::gds
