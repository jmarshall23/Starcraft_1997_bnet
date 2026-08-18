#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace staredit {

struct ObjectArtFrame {
  std::uint16_t width{};
  std::uint16_t height{};
  std::uint16_t canvas_width{};
  std::uint16_t canvas_height{};
  std::int16_t x_offset{};
  std::int16_t y_offset{};
  std::vector<std::uint8_t> palette_indices{};
  std::vector<std::uint8_t> opacity{};
  std::vector<std::uint32_t> bgra{};
};

using TeamColorTable = std::array<std::array<std::uint8_t, 8>, 12>;

void composite_object_art(
    const ObjectArtFrame& art,
    const std::vector<std::uint8_t>& palette,
    const TeamColorTable& team_colors,
    std::uint8_t owner,
    std::vector<std::uint32_t>& destination,
    int destination_width,
    int destination_height,
    int center_x,
    int center_y) noexcept;

void composite_object_art_scaled(
    const ObjectArtFrame& art,
    const std::vector<std::uint8_t>& palette,
    const TeamColorTable& team_colors,
    std::uint8_t owner,
    std::vector<std::uint32_t>& destination,
    int destination_width,
    int destination_height,
    int center_x,
    int center_y,
    int scale_percent) noexcept;

[[nodiscard]] bool make_object_art_preview(
    const ObjectArtFrame& art,
    const std::vector<std::uint8_t>& palette,
    const TeamColorTable& team_colors,
    std::uint8_t owner,
    std::uint32_t background,
    int width,
    int height,
    std::vector<std::uint32_t>& output) noexcept;

}  // namespace staredit
