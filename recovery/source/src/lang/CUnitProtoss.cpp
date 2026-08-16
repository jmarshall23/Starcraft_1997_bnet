#include "starcraft/lang/cunit_protoss.hpp"

#include <algorithm>

namespace starcraft::lang {

bool is_protoss_templar_type(const std::uint16_t unit_type) noexcept {
  // CUnitProtoss.cpp::sub_43C5B0 asserts this exact pair before executing the
  // High Templar/Dark Templar merge-order path.
  return unit_type == 67U || unit_type == 79U;
}

bool is_protoss_shield_battery(const std::uint16_t unit_type) noexcept {
  // sub_43D070, sub_43D120, sub_43D1D0, and sub_43D280 all assert type 172.
  return unit_type == protoss_shield_battery_type;
}

ProtossShieldTransfer transfer_shield_battery_energy(
    const std::uint32_t current_shields,
    const std::uint32_t maximum_shields,
    const std::uint16_t battery_energy) noexcept {
  ProtossShieldTransfer result{
      (std::min)(current_shields, maximum_shields), battery_energy, 0U};
  const std::uint32_t deficit = maximum_shields - result.shield_points;
  const std::uint32_t available = (std::min)(
      static_cast<std::uint32_t>(battery_energy), 256U);
  const std::uint32_t transfer = (std::min)(deficit, available);
  result.shield_points += transfer;
  result.battery_energy = static_cast<std::uint16_t>(battery_energy - transfer);
  result.transferred = static_cast<std::uint16_t>(transfer);
  return result;
}

}  // namespace starcraft::lang
