#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "starcraft/gds/meta_bitmap.hpp"

namespace starcraft::gds {

// On-disk GRP headers used by the original renderer. A frame descriptor begins
// immediately after GrpHeader and its data offset is relative to the group base.
#pragma pack(push, 1)
struct GrpHeader {
  std::uint16_t frame_count{};
  std::uint16_t maximum_width{};
  std::uint16_t maximum_height{};
};

struct GrpFrameHeader {
  std::uint8_t x_offset{};
  std::uint8_t y_offset{};
  std::uint8_t width_low{};
  std::uint8_t height{};
  std::uint32_t data_offset{};
};
#pragma pack(pop)

static_assert(sizeof(GrpHeader) == 6);
static_assert(sizeof(GrpFrameHeader) == 8);
static_assert(offsetof(GrpFrameHeader, data_offset) == 4);

inline constexpr std::uint32_t grp_extended_width = 0x80000000U;

// Recovered from U:\SWAR\lang\GDS\grptobmp.cpp, original VA 0x004D8490.
// An out-of-range frame index deliberately selects frame zero, matching the
// original. The returned bitmap aliases storage owned by the GRP buffer.
[[nodiscard]] std::uint8_t* grp_frame_to_bitmap(
    std::uint8_t* group,
    std::uint8_t frame_index,
    MetaBitmap& bitmap) noexcept;

struct GrpFramePlacement {
  std::int16_t x_offset{};
  std::int16_t y_offset{};
  std::int16_t canvas_width{};
  std::int16_t canvas_height{};
};

// Extended form recovered from GDS/grp2bmpx.cpp, original VA 0x004D7DC0.
[[nodiscard]] std::uint8_t* grp_frame_to_bitmap_extended(
    std::uint8_t* group,
    std::uint8_t frame_index,
    MetaBitmap& bitmap,
    GrpFramePlacement& placement) noexcept;

struct DecodedGrpFrame {
  std::uint8_t x_offset{};
  std::uint8_t y_offset{};
  std::uint16_t width{};
  std::uint16_t height{};
  std::uint16_t canvas_width{};
  std::uint16_t canvas_height{};
  std::vector<std::uint8_t> pixels{};
  std::vector<std::uint8_t> opacity{};
};

[[nodiscard]] std::uint16_t grp_frame_count(
    const std::uint8_t* group,
    std::size_t group_size) noexcept;

// Bounds-checked decoder for the scanline RLE referenced by a GRP frame.
// Transparent runs are initialized to transparent_index.
[[nodiscard]] bool decode_grp_frame(
    const std::uint8_t* group,
    std::size_t group_size,
    std::uint16_t frame_index,
    DecodedGrpFrame& output,
    std::uint8_t transparent_index = 0) noexcept;

}  // namespace starcraft::gds
