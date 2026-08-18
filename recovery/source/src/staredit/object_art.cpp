#include "object_art.hpp"

#include <algorithm>
#include <cstdint>

namespace staredit {
namespace {

std::uint32_t source_pixel(const ObjectArtFrame& art,
                           const std::vector<std::uint8_t>& palette,
                           const TeamColorTable& team_colors,
                           const std::uint8_t owner,
                           const std::size_t pixel) noexcept {
  if (pixel >= art.bgra.size()) {
    return 0U;
  }
  if (owner >= team_colors.size() || palette.size() != 1024U ||
      pixel >= art.palette_indices.size()) {
    return art.bgra[pixel];
  }
  const std::uint8_t source = art.palette_indices[pixel];
  if (source < 8U || source > 15U) {
    return art.bgra[pixel];
  }
  const std::uint8_t translated = team_colors[owner][source - 8U];
  const std::size_t color = 4U * translated;
  return 0xFF000000U | static_cast<std::uint32_t>(palette[color + 2U]) |
         (static_cast<std::uint32_t>(palette[color + 1U]) << 8U) |
         (static_cast<std::uint32_t>(palette[color]) << 16U);
}

bool visible_pixel(const ObjectArtFrame& art,
                   const std::size_t pixel) noexcept {
  return pixel < art.bgra.size() &&
         (pixel >= art.opacity.size() ? (art.bgra[pixel] >> 24U) != 0U
                                      : art.opacity[pixel] != 0U);
}

}  // namespace

void composite_object_art(const ObjectArtFrame& art,
                          const std::vector<std::uint8_t>& palette,
                          const TeamColorTable& team_colors,
                          const std::uint8_t owner,
                          std::vector<std::uint32_t>& destination,
                          const int destination_width,
                          const int destination_height,
                          const int center_x,
                          const int center_y) noexcept {
  if (art.width == 0U || art.height == 0U || art.canvas_width == 0U ||
      art.canvas_height == 0U || destination_width <= 0 ||
      destination_height <= 0 ||
      destination.size() != static_cast<std::size_t>(destination_width) *
                                destination_height) {
    return;
  }
  const int left = center_x - static_cast<int>(art.canvas_width) / 2 +
                   art.x_offset;
  const int top = center_y - static_cast<int>(art.canvas_height) / 2 +
                  art.y_offset;
  for (std::size_t source_y = 0U; source_y < art.height; ++source_y) {
    const int output_y = top + static_cast<int>(source_y);
    if (output_y < 0 || output_y >= destination_height) {
      continue;
    }
    for (std::size_t source_x = 0U; source_x < art.width; ++source_x) {
      const int output_x = left + static_cast<int>(source_x);
      const std::size_t source = source_y * art.width + source_x;
      if (output_x < 0 || output_x >= destination_width ||
          !visible_pixel(art, source)) {
        continue;
      }
      destination[static_cast<std::size_t>(output_y) * destination_width +
                  output_x] =
          source_pixel(art, palette, team_colors, owner, source);
    }
  }
}

void composite_object_art_scaled(
    const ObjectArtFrame& art,
    const std::vector<std::uint8_t>& palette,
    const TeamColorTable& team_colors,
    const std::uint8_t owner,
    std::vector<std::uint32_t>& destination,
    const int destination_width,
    const int destination_height,
    const int center_x,
    const int center_y,
    const int scale_percent) noexcept {
  if (scale_percent <= 0 || art.width == 0U || art.height == 0U ||
      art.canvas_width == 0U || art.canvas_height == 0U ||
      destination_width <= 0 || destination_height <= 0 ||
      destination.size() != static_cast<std::size_t>(destination_width) *
                                destination_height) {
    return;
  }
  const int scaled_canvas_width = (std::max)(
      1, static_cast<int>(static_cast<std::int64_t>(art.canvas_width) *
                          scale_percent / 100));
  const int scaled_canvas_height = (std::max)(
      1, static_cast<int>(static_cast<std::int64_t>(art.canvas_height) *
                          scale_percent / 100));
  const int scaled_width = (std::max)(
      1, static_cast<int>(static_cast<std::int64_t>(art.width) *
                          scale_percent / 100));
  const int scaled_height = (std::max)(
      1, static_cast<int>(static_cast<std::int64_t>(art.height) *
                          scale_percent / 100));
  const int left = center_x - scaled_canvas_width / 2 +
                   static_cast<int>(static_cast<std::int64_t>(art.x_offset) *
                                    scale_percent / 100);
  const int top = center_y - scaled_canvas_height / 2 +
                  static_cast<int>(static_cast<std::int64_t>(art.y_offset) *
                                   scale_percent / 100);
  for (int output_y = 0; output_y < scaled_height; ++output_y) {
    const int target_y = top + output_y;
    if (target_y < 0 || target_y >= destination_height) {
      continue;
    }
    const std::size_t source_y = static_cast<std::size_t>(
        static_cast<std::int64_t>(output_y) * art.height / scaled_height);
    for (int output_x = 0; output_x < scaled_width; ++output_x) {
      const int target_x = left + output_x;
      if (target_x < 0 || target_x >= destination_width) {
        continue;
      }
      const std::size_t source_x = static_cast<std::size_t>(
          static_cast<std::int64_t>(output_x) * art.width / scaled_width);
      const std::size_t source = source_y * art.width + source_x;
      if (visible_pixel(art, source)) {
        destination[static_cast<std::size_t>(target_y) * destination_width +
                    target_x] =
            source_pixel(art, palette, team_colors, owner, source);
      }
    }
  }
}

bool make_object_art_preview(const ObjectArtFrame& art,
                             const std::vector<std::uint8_t>& palette,
                             const TeamColorTable& team_colors,
                             const std::uint8_t owner,
                             const std::uint32_t background,
                             const int width,
                             const int height,
                             std::vector<std::uint32_t>& output) noexcept {
  output.clear();
  if (width <= 0 || height <= 0 || art.canvas_width == 0U ||
      art.canvas_height == 0U) {
    return false;
  }
  try {
    output.assign(static_cast<std::size_t>(width) * height, background);
    const int available = (std::min)(width - 2, height - 2);
    if (available <= 0) {
      return false;
    }
    const std::int64_t horizontal =
        static_cast<std::int64_t>(available) * 65536 / art.canvas_width;
    const std::int64_t vertical =
        static_cast<std::int64_t>(available) * 65536 / art.canvas_height;
    const std::int64_t scale = (std::min)(horizontal, vertical);
    const int scaled_canvas_width = static_cast<int>(
        static_cast<std::int64_t>(art.canvas_width) * scale / 65536);
    const int scaled_canvas_height = static_cast<int>(
        static_cast<std::int64_t>(art.canvas_height) * scale / 65536);
    const int canvas_left = (width - scaled_canvas_width) / 2;
    const int canvas_top = (height - scaled_canvas_height) / 2;
    const int frame_left = canvas_left + static_cast<int>(
        static_cast<std::int64_t>(art.x_offset) * scale / 65536);
    const int frame_top = canvas_top + static_cast<int>(
        static_cast<std::int64_t>(art.y_offset) * scale / 65536);
    const int scaled_width = (std::max)(
        1, static_cast<int>(static_cast<std::int64_t>(art.width) * scale /
                            65536));
    const int scaled_height = (std::max)(
        1, static_cast<int>(static_cast<std::int64_t>(art.height) * scale /
                            65536));
    for (int output_y = 0; output_y < scaled_height; ++output_y) {
      const int target_y = frame_top + output_y;
      if (target_y < 0 || target_y >= height) {
        continue;
      }
      const std::size_t source_y = static_cast<std::size_t>(
          static_cast<std::int64_t>(output_y) * art.height / scaled_height);
      for (int output_x = 0; output_x < scaled_width; ++output_x) {
        const int target_x = frame_left + output_x;
        if (target_x < 0 || target_x >= width) {
          continue;
        }
        const std::size_t source_x = static_cast<std::size_t>(
            static_cast<std::int64_t>(output_x) * art.width / scaled_width);
        const std::size_t source = source_y * art.width + source_x;
        if (visible_pixel(art, source)) {
          output[static_cast<std::size_t>(target_y) * width + target_x] =
              source_pixel(art, palette, team_colors, owner, source);
        }
      }
    }
    return true;
  } catch (...) {
    output.clear();
    return false;
  }
}

}  // namespace staredit
