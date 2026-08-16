#include "starcraft/gds/bitmap.hpp"

#include "starcraft/runtime/invariant.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace starcraft::gds {
namespace {

constexpr char kOriginalSource[] = R"(U:\SWAR\lang\GDS\bmpcle.cpp)";

}  // namespace

std::uint16_t clear_bitmap(MetaBitmap& bitmap, const std::uint8_t color) noexcept {
  runtime::require(bitmap.pixels != nullptr, kOriginalSource, 26, "pcurrentbmp");
  const auto pixel_count = static_cast<std::size_t>(bitmap.width) *
                           static_cast<std::size_t>(bitmap.height);
  std::memset(bitmap.pixels, color, pixel_count);
  return static_cast<std::uint16_t>(color) * 0x0101U;
}

}  // namespace starcraft::gds
