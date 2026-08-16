#pragma once

#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint16_t protoss_shield_battery_type = 172U;
inline constexpr std::uint16_t protoss_probe_type = 64U;

struct ProtossUnitTypeView {
  const std::uint16_t* unit_types{};
  std::size_t count{};
};

struct ProtossShieldTransfer {
  std::uint32_t shield_points{};
  std::uint16_t battery_energy{};
  std::uint16_t transferred{};
};

[[nodiscard]] bool is_protoss_templar_type(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_protoss_shield_battery(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_protoss_probe(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_protoss_buildable_unit_type(
    std::uint16_t unit_type) noexcept;
[[nodiscard]] ProtossUnitTypeView protoss_buildable_unit_types() noexcept;

// CUnitProtoss.cpp::sub_43D1D0 (VA 0x0043D1D0): transfer at most one
// fixed-point shield point (256) per update, bounded by both deficit and
// battery energy.
[[nodiscard]] ProtossShieldTransfer transfer_shield_battery_energy(
    std::uint32_t current_shields,
    std::uint32_t maximum_shields,
    std::uint16_t battery_energy) noexcept;

}  // namespace starcraft::lang
