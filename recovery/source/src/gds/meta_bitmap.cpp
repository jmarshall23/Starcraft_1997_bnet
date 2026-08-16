#include "starcraft/gds/meta_bitmap.hpp"

#include "starcraft/runtime/storm.hpp"

#include <cstddef>

namespace starcraft::gds {
namespace {

constexpr char kOriginalSource[] = R"(U:\SWAR\lang\GDS\metabmp.cpp)";

}  // namespace

bool MetaBitmap::allocate(runtime::StormModule& storm) noexcept {
  // Original VA 0x004D34B0, source line 22.
  const auto pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
  pixels = static_cast<std::uint8_t*>(storm.allocate(pixel_count, kOriginalSource, 22));
  return pixels != nullptr;
}

bool MetaBitmap::release(runtime::StormModule& storm) noexcept {
  // Original VA 0x004D34E0, source line 29.
  const bool released = storm.free(pixels, kOriginalSource, 29);
  pixels = nullptr;
  return released;
}

}  // namespace starcraft::gds
