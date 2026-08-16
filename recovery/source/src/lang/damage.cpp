#include "starcraft/lang/damage.hpp"

#include "starcraft/runtime/invariant.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::lang {
namespace {

constexpr char kOriginalSource[] = R"(U:\SWAR\lang\damage.cpp)";

// Exact dwords at 0x004F15AC..0x004F15F8. The first dword in each row repeats
// the row's numeric damage-class identity, followed by an unused zero column;
// armor classes 1..3 therefore occupy columns 2..4.
constexpr std::array<std::array<std::uint32_t, 5>, 4> kDamagePercent256{{
    {{0, 0, 0, 0, 0}},
    {{1, 0, 128, 192, 256}},
    {{2, 0, 256, 128, 64}},
    {{3, 0, 256, 256, 192}},
}};

constexpr std::array<std::uint32_t, 4> kArmorClassIdentity{{0, 1, 2, 3}};

}  // namespace

std::uint32_t scale_damage(
    const std::uint32_t damage,
    const DamageClass damage_class,
    const ArmorClass armor_class) noexcept {
  const auto damage_index = static_cast<std::size_t>(damage_class);
  const auto armor_index = static_cast<std::size_t>(armor_class);

  runtime::require(
      damage_index != 0 && damage_index < kDamagePercent256.size(),
      kOriginalSource,
      30,
      "dc != DC_UNKNOWN && dc < DC_COUNT");
  runtime::require(
      armor_index != 0 && armor_index < kArmorClassIdentity.size(),
      kOriginalSource,
      31,
      "ac != AC_UNKNOWN && ac < AC_COUNT");
  runtime::require(
      kArmorClassIdentity[armor_index] == armor_index,
      kOriginalSource,
      34,
      "armor-class table identity");
  runtime::require(
      kDamagePercent256[damage_index][0] == damage_index,
      kOriginalSource,
      35,
      "damage-class table identity");

  // Unsigned arithmetic deliberately specifies the original x86 low-dword wrap.
  return (damage * kDamagePercent256[damage_index][armor_index + 1U]) >> 8U;
}

}  // namespace starcraft::lang
