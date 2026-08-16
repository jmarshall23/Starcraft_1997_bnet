#include "starcraft/gds/grp.hpp"

#include "starcraft/runtime/invariant.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace starcraft::gds {
namespace {

constexpr char kOriginalSource[] = R"(U:\SWAR\lang\GDS\grptobmp.cpp)";

std::uint16_t read_u16(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint16_t>(bytes[0]) |
         static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[1]) << 8U);
}

std::uint32_t read_u32(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

}  // namespace

std::uint8_t* grp_frame_to_bitmap(
    std::uint8_t* const group,
    const std::uint8_t frame_index,
    MetaBitmap& bitmap) noexcept {
  runtime::require(group != nullptr, kOriginalSource, 31, "pgroup && pbmp");

  const auto* const header = reinterpret_cast<const GrpHeader*>(group);
  const std::uint8_t selected = frame_index < header->frame_count ? frame_index : 0;
  const auto* const frames = reinterpret_cast<const GrpFrameHeader*>(group + sizeof(GrpHeader));
  const GrpFrameHeader& frame = frames[selected];

  std::uint32_t data_offset = frame.data_offset;
  bitmap.width = frame.width_low;
  if ((data_offset & grp_extended_width) != 0) {
    data_offset &= ~grp_extended_width;
    bitmap.width = static_cast<std::int16_t>(bitmap.width + 256);
  }
  bitmap.height = frame.height;
  bitmap.pixels = group + data_offset;
  return bitmap.pixels;
}

std::uint8_t* grp_frame_to_bitmap_extended(
    std::uint8_t* const group,
    const std::uint8_t frame_index,
    MetaBitmap& bitmap,
    GrpFramePlacement& placement) noexcept {
  runtime::require(group != nullptr, R"(U:\SWAR\lang\GDS\grp2bmpx.cpp)", 36,
                   "pgroup && pbmp && dx && dy && maxx && maxy");
  const auto* const header = reinterpret_cast<const GrpHeader*>(group);
  const std::uint8_t selected = frame_index < header->frame_count ? frame_index : 0;
  const auto* const frames = reinterpret_cast<const GrpFrameHeader*>(group + sizeof(GrpHeader));
  placement.x_offset = frames[selected].x_offset;
  placement.y_offset = frames[selected].y_offset;
  placement.canvas_width = static_cast<std::int16_t>(header->maximum_width);
  placement.canvas_height = static_cast<std::int16_t>(header->maximum_height);
  return grp_frame_to_bitmap(group, selected, bitmap);
}

bool decode_grp_frame(
    const std::uint8_t* const group,
    const std::size_t group_size,
    const std::uint16_t frame_index,
    DecodedGrpFrame& output,
    const std::uint8_t transparent_index) noexcept {
  if (group == nullptr || group_size < sizeof(GrpHeader)) {
    return false;
  }
  const std::uint16_t frame_count = read_u16(group);
  const std::size_t descriptor_bytes =
      sizeof(GrpHeader) + static_cast<std::size_t>(frame_count) * sizeof(GrpFrameHeader);
  if (frame_count == 0 || descriptor_bytes > group_size) {
    return false;
  }

  const std::uint16_t selected = frame_index < frame_count ? frame_index : 0;
  const std::uint8_t* const descriptor =
      group + sizeof(GrpHeader) + static_cast<std::size_t>(selected) * sizeof(GrpFrameHeader);
  std::uint16_t width = descriptor[2];
  const std::uint16_t height = descriptor[3];
  std::uint32_t data_offset = read_u32(descriptor + 4);
  if ((data_offset & grp_extended_width) != 0) {
    data_offset &= ~grp_extended_width;
    width = static_cast<std::uint16_t>(width + 256U);
  }
  if (width == 0 || height == 0 || data_offset > group_size ||
      static_cast<std::size_t>(height) * sizeof(std::uint16_t) > group_size - data_offset ||
      static_cast<std::size_t>(width) >
          std::numeric_limits<std::size_t>::max() / static_cast<std::size_t>(height)) {
    return false;
  }

  try {
    DecodedGrpFrame decoded{};
    decoded.x_offset = descriptor[0];
    decoded.y_offset = descriptor[1];
    decoded.width = width;
    decoded.height = height;
    decoded.canvas_width = read_u16(group + 2);
    decoded.canvas_height = read_u16(group + 4);
    decoded.pixels.assign(static_cast<std::size_t>(width) * height, transparent_index);
    decoded.opacity.assign(static_cast<std::size_t>(width) * height, 0);
    // game\icons.grp is the beta UI's raw-frame GRP variant: every distinct
    // data offset owns exactly width*height palette bytes, and frames may
    // alias the same block. sub_453800 hands these bytes directly to the DLG
    // bitmap controls instead of walking scanline command streams.
    std::size_t next_data_offset = group_size;
    for (std::uint16_t other = 0; other < frame_count; ++other) {
      const std::uint8_t* const other_descriptor =
          group + sizeof(GrpHeader) + static_cast<std::size_t>(other) *
                                          sizeof(GrpFrameHeader);
      const std::uint32_t candidate =
          read_u32(other_descriptor + 4) & ~grp_extended_width;
      if (candidate > data_offset && candidate < next_data_offset) {
        next_data_offset = candidate;
      }
    }
    const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
    if (data_offset <= group_size && next_data_offset - data_offset == pixel_count) {
      std::memcpy(decoded.pixels.data(), group + data_offset, pixel_count);
      for (std::size_t pixel = 0; pixel < pixel_count; ++pixel) {
        decoded.opacity[pixel] =
            decoded.pixels[pixel] == transparent_index ? 0U : 0xFFU;
      }
      output = std::move(decoded);
      return true;
    }
    for (std::uint16_t row = 0; row < height; ++row) {
      const std::size_t line_offset = read_u16(group + data_offset + 2U * row);
      if (line_offset > group_size - data_offset) {
        return false;
      }
      std::size_t cursor = data_offset + line_offset;
      std::size_t x{};
      while (x < width) {
        if (cursor >= group_size) {
          return false;
        }
        const std::uint8_t command = group[cursor++];
        if ((command & 0x80U) != 0) {
          const std::size_t run = command & 0x7FU;
          if (run == 0 || run > width - x) {
            return false;
          }
          x += run;
        } else if ((command & 0x40U) != 0) {
          const std::size_t run = command & 0x3FU;
          if (run == 0 || run > width - x || cursor >= group_size) {
            return false;
          }
          const std::uint8_t color = group[cursor++];
          const std::size_t output_offset = static_cast<std::size_t>(row) * width + x;
          std::memset(decoded.pixels.data() + output_offset, color, run);
          std::memset(decoded.opacity.data() + output_offset, 0xFF, run);
          x += run;
        } else {
          const std::size_t run = command;
          if (run == 0 || run > width - x || run > group_size - cursor) {
            return false;
          }
          std::memcpy(
              decoded.pixels.data() + static_cast<std::size_t>(row) * width + x,
              group + cursor,
              run);
          std::memset(
              decoded.opacity.data() + static_cast<std::size_t>(row) * width + x,
              0xFF,
              run);
          cursor += run;
          x += run;
        }
      }
    }
    output = std::move(decoded);
    return true;
  } catch (...) {
    return false;
  }
}

std::uint16_t grp_frame_count(
    const std::uint8_t* const group,
    const std::size_t group_size) noexcept {
  return group != nullptr && group_size >= sizeof(std::uint16_t) ? read_u16(group) : 0;
}

}  // namespace starcraft::gds
