#pragma once

#include <cstdint>

namespace starcraft::lang {

// The binary only preserves numeric identities for these pre-release classes.
// Conventional StarCraft names will be applied after callers and data files agree.
enum class DamageClass : std::uint8_t {
  class_1 = 1,
  class_2 = 2,
  class_3 = 3,
};

enum class ArmorClass : std::uint8_t {
  class_1 = 1,
  class_2 = 2,
  class_3 = 3,
};

// Recovered from VA 0x0044B150, U:\\SWAR\\lang\\damage.cpp.
// Multipliers are unsigned 8.8 fixed point and the result truncates.
[[nodiscard]] std::uint32_t scale_damage(
    std::uint32_t damage,
    DamageClass damage_class,
    ArmorClass armor_class) noexcept;

}  // namespace starcraft::lang
