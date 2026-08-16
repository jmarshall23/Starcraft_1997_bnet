#pragma once

#include <cstdint>

namespace starcraft::staredit::mapcomn {

struct CreepBounds {
  int left{};
  int top{};
  int right{};
  int bottom{};
  bool elliptical{};
};

// Recovered from U:\SWAR\staredit\mapcomn\creep.cpp.
[[nodiscard]] bool creep_uses_ellipse(
    std::uint16_t unit_type,
    bool completed_flag) noexcept;  // VA 0x004D155C

[[nodiscard]] bool creep_bounds(
    std::uint16_t unit_type,
    int center_x,
    int center_y,
    std::uint16_t placement_width,
    std::uint16_t placement_height,
    bool completed_flag,
    int map_tile_width,
    int map_tile_height,
    CreepBounds& output) noexcept;  // VA 0x004D15CF

[[nodiscard]] bool creep_ellipse_contains_tile(
    int center_x,
    int center_y,
    int tile_x,
    int tile_y) noexcept;  // VA 0x004D17A2

[[nodiscard]] std::uint8_t choose_creep_visual_tile() noexcept;  // VA 0x004D0E07

}  // namespace starcraft::staredit::mapcomn
