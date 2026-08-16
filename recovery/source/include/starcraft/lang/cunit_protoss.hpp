#pragma once

#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint16_t protoss_shield_battery_type = 172U;

struct ProtossShieldTransfer {
  std::uint32_t shield_points{};
  std::uint16_t battery_energy{};
  std::uint16_t transferred{};
};

[[nodiscard]] bool is_protoss_templar_type(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_protoss_shield_battery(std::uint16_t unit_type) noexcept;

// CUnitProtoss.cpp::sub_43D1D0 (VA 0x0043D1D0): transfer at most one
// fixed-point shield point (256) per update, bounded by both deficit and
// battery energy.
[[nodiscard]] ProtossShieldTransfer transfer_shield_battery_energy(
    std::uint32_t current_shields,
    std::uint32_t maximum_shields,
    std::uint16_t battery_energy) noexcept;

}  // namespace starcraft::lang
