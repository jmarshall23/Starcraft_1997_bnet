#pragma once

#include <cstddef>
#include <cstdint>

namespace starcraft::runtime {
class StormModule;
}

namespace starcraft::gds {

// Layout recovered from GDS/metabmp.cpp at 0x004D34B0 and 0x004D34E0.
#pragma pack(push, 1)
struct MetaBitmap {
  std::int16_t width{};
  std::int16_t height{};
  std::uint8_t* pixels{};

  [[nodiscard]] bool allocate(runtime::StormModule& storm) noexcept;
  [[nodiscard]] bool release(runtime::StormModule& storm) noexcept;
};
#pragma pack(pop)

static_assert(offsetof(MetaBitmap, width) == 0);
static_assert(offsetof(MetaBitmap, height) == 2);
static_assert(offsetof(MetaBitmap, pixels) == 4);
static_assert(sizeof(MetaBitmap) == 8, "MetaBitmap is an x86 ABI structure");

}  // namespace starcraft::gds
