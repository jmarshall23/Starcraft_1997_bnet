#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "starcraft/game/unit.hpp"

namespace starcraft::lang {

inline constexpr std::size_t kUnitTypeCount = 228;
inline constexpr std::size_t kPlayerCount = 12;
inline constexpr std::size_t kPlayableRaceCount = 3;

enum class Race : std::uint8_t {
  zerg = 0,
  terran = 1,
  protoss = 2,
  none = 4,
};

enum UnitRaceFlag : std::uint8_t {
  unit_race_zerg = 0x01,
  unit_race_terran = 0x02,
  unit_race_protoss = 0x04,
  unit_group_08 = 0x08,
  unit_group_10 = 0x10,
  unit_group_20 = 0x20,
};

inline constexpr std::uint32_t unit_dat_double_supply = 0x00000400U;

// This byte is loaded from the unit DAT records at original VA 0x008E06D8.
struct UnitTypeTraits {
  std::uint8_t race_flags{};
  std::uint8_t supply_provided{};
  std::uint8_t supply_required{};
  std::uint8_t score_value{};
  std::uint32_t dat_flags{};
};

using UnitTraitsTable = std::array<UnitTypeTraits, kUnitTypeCount>;

struct SupplyState {
  // Original global arrays are grouped by race, each with twelve player dwords.
  std::array<std::array<std::uint32_t, kPlayerCount>, kPlayableRaceCount> provided{};
  std::array<std::array<std::uint32_t, kPlayerCount>, kPlayableRaceCount> used{};
  std::array<std::array<std::uint32_t, kPlayerCount>, kPlayableRaceCount> maximum{};
};

using PerPlayerCount = std::array<std::int32_t, kPlayerCount>;
using PerTypePlayerCount =
    std::array<std::array<std::int32_t, kPlayerCount>, kUnitTypeCount>;

struct UnitCountState {
  SupplyState supply{};
  PerTypePlayerCount completed_by_type{};
  PerTypePlayerCount existing_by_type{};

  PerPlayerCount completed_group_08{};
  PerPlayerCount completed_group_10{};
  PerPlayerCount completed_group_20{};
  PerPlayerCount completion_score_count_08{};
  PerPlayerCount completion_score_count_10{};
  PerPlayerCount completion_score_08{};
  PerPlayerCount completion_score_10{};

  PerPlayerCount existing_group_08{};
  PerPlayerCount existing_group_10{};
  PerPlayerCount existing_group_20{};
};

// Recovered from U:\\SWAR\\lang\\count.cpp.
[[nodiscard]] Race race_for_unit_type(
    std::uint16_t unit_type,
    const UnitTraitsTable& traits) noexcept;  // VA 0x0041B370

[[nodiscard]] std::uint32_t supply_maximum_for_unit_type(
    std::uint16_t unit_type,
    std::uint8_t player,
    const UnitTraitsTable& traits,
    const SupplyState& state) noexcept;  // VA 0x0041AF00

[[nodiscard]] std::uint32_t supply_provided_capped(
    Race race,
    std::uint8_t player,
    const SupplyState& state) noexcept;  // VA 0x0041B3C0

[[nodiscard]] std::uint32_t supply_used(
    Race race,
    std::uint8_t player,
    const SupplyState& state) noexcept;  // VA 0x0041B470

[[nodiscard]] std::uint32_t supply_available_for_unit_type(
    std::uint16_t unit_type,
    std::uint8_t player,
    const UnitTraitsTable& traits,
    const SupplyState& state,
    bool use_maximum_supply) noexcept;  // VA 0x0041AFB0

void adjust_completed_unit_counts(
    std::int32_t delta,
    const game::Unit& unit,
    bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041A720

void mark_unit_counted_as_complete(
    game::Unit& unit,
    bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041A6A0

void unmark_unit_counted_as_complete(
    game::Unit& unit,
    bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041A8E0

void adjust_existing_unit_counts(
    std::int32_t delta,
    const game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041AA00

void mark_unit_counted_as_existing(
    game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041A9A0

void unmark_unit_counted_as_existing(
    game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept;  // VA 0x0041ABE0

}  // namespace starcraft::lang
