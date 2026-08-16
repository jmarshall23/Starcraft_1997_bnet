#pragma once

#include <cstdint>

namespace starcraft::lang {

// Recovered from U:\SWAR\lang\CFlingy.cpp.
[[nodiscard]] std::uint8_t direction_from_points(
    std::int32_t from_x,
    std::int32_t from_y,
    std::int32_t to_x,
    std::int32_t to_y) noexcept;  // VA 0x00405E80

[[nodiscard]] std::int8_t clamp_turn_delta(
    std::uint8_t current_direction,
    std::uint8_t desired_direction,
    std::int32_t maximum_turn) noexcept;  // VA 0x00405F50

[[nodiscard]] std::uint16_t turn_distance(
    std::uint8_t first_direction,
    std::uint8_t second_direction) noexcept;  // VA 0x00405F70

}  // namespace starcraft::lang
