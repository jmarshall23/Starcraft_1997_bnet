#include "starcraft/lang/count.hpp"

#include "starcraft/runtime/invariant.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace starcraft::lang {
namespace {

constexpr char kOriginalSource[] = R"(U:\SWAR\lang\count.cpp)";

void require_unit_type(const std::uint16_t unit_type, const int line) noexcept {
  runtime::require(
      unit_type < kUnitTypeCount,
      kOriginalSource,
      line,
      "type < NUM_UNITS");
}

void require_player(const std::uint8_t player, const int line) noexcept {
  runtime::require(
      player < kPlayerCount,
      kOriginalSource,
      line,
      "player < MAX_PLAYERS");
}

std::size_t race_index(const Race race) noexcept {
  return static_cast<std::size_t>(race);
}

void add_wrapped(
    std::uint32_t& target,
    const std::int32_t delta,
    const std::uint32_t amount) noexcept {
  target += static_cast<std::uint32_t>(delta * static_cast<std::int32_t>(amount));
}

std::uint16_t supply_type_for_existing_unit(const game::Unit& unit) noexcept {
  if (unit.unit_type != 36 && unit.unit_type != 59) {
    return unit.unit_type;
  }
  runtime::require(
      unit.production_type_index < unit.production_types.size(),
      kOriginalSource,
      115,
      "production type index < queue length");
  return unit.production_types[unit.production_type_index];
}

}  // namespace

Race race_for_unit_type(
    const std::uint16_t unit_type,
    const UnitTraitsTable& traits) noexcept {
  require_unit_type(unit_type, 293);
  const auto flags = traits[unit_type].race_flags;
  if ((flags & unit_race_zerg) != 0) {
    return Race::zerg;
  }
  if ((flags & unit_race_terran) != 0) {
    return Race::terran;
  }
  if ((flags & unit_race_protoss) != 0) {
    return Race::protoss;
  }
  return Race::none;
}

std::uint32_t supply_maximum_for_unit_type(
    const std::uint16_t unit_type,
    const std::uint8_t player,
    const UnitTraitsTable& traits,
    const SupplyState& state) noexcept {
  require_unit_type(unit_type, 243);
  require_player(player, 244);
  const Race race = race_for_unit_type(unit_type, traits);
  if (race == Race::none) {
    return 0;
  }
  return state.maximum[race_index(race)][player];
}

std::uint32_t supply_provided_capped(
    const Race race,
    const std::uint8_t player,
    const SupplyState& state) noexcept {
  require_player(player, 305);
  const auto index = race_index(race);
  if (index >= kPlayableRaceCount) {
    return 0;
  }
  return std::min(state.provided[index][player], state.maximum[index][player]);
}

std::uint32_t supply_used(
    const Race race,
    const std::uint8_t player,
    const SupplyState& state) noexcept {
  require_player(player, 325);
  const auto index = race_index(race);
  if (index >= kPlayableRaceCount) {
    return 0;
  }
  return state.used[index][player];
}

std::uint32_t supply_available_for_unit_type(
    const std::uint16_t unit_type,
    const std::uint8_t player,
    const UnitTraitsTable& traits,
    const SupplyState& state,
    const bool use_maximum_supply) noexcept {
  require_unit_type(unit_type, 263);
  require_player(player, 264);
  const Race race = race_for_unit_type(unit_type, traits);
  if (race == Race::none) {
    return 0;
  }

  const auto index = race_index(race);
  const std::uint32_t available_before_usage = use_maximum_supply
      ? state.maximum[index][player]
      : std::min(state.provided[index][player], state.maximum[index][player]);

  // Unsigned subtraction preserves the original x86 behavior if inconsistent
  // game state temporarily reports more used supply than available supply.
  return available_before_usage - state.used[index][player];
}

void adjust_completed_unit_counts(
    const std::int32_t delta,
    const game::Unit& unit,
    const bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  require_unit_type(unit.unit_type, 13);
  require_player(unit.owner, 14);
  if (unit.is_excluded_from_counts()) {
    return;
  }

  const auto& unit_traits = traits[unit.unit_type];
  auto& per_type_count = state.completed_by_type[unit.unit_type][unit.owner];
  per_type_count += delta;
  runtime::require(
      per_type_count != -1,
      kOriginalSource,
      24,
      "completed unit count != -1");

  const Race race = race_for_unit_type(unit.unit_type, traits);
  if (race != Race::none) {
    add_wrapped(
        state.supply.provided[race_index(race)][unit.owner],
        delta,
        unit_traits.supply_provided);
  }

  if ((unit_traits.race_flags & unit_group_20) != 0) {
    state.completed_group_20[unit.owner] += delta;
  }
  if ((unit_traits.race_flags & unit_group_08) != 0) {
    state.completed_group_08[unit.owner] += delta;
  } else if ((unit_traits.race_flags & unit_group_10) != 0) {
    state.completed_group_10[unit.owner] += delta;
  }

  if (update_score) {
    if ((unit_traits.race_flags & unit_group_08) != 0) {
      state.completion_score_count_08[unit.owner] += delta;
      state.completion_score_08[unit.owner] += delta * unit_traits.score_value;
    } else if ((unit_traits.race_flags & unit_group_10) != 0) {
      state.completion_score_count_10[unit.owner] += delta;
      state.completion_score_10[unit.owner] += delta * unit_traits.score_value;
    }
  }
}

void mark_unit_counted_as_complete(
    game::Unit& unit,
    const bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  runtime::require(unit.is_counted_as_existing(), kOriginalSource, 60, "unit counted as existing");
  runtime::require(!unit.is_counted_as_complete(), kOriginalSource, 61, "unit not already counted complete");
  unit.count_state |= game::unit_count_complete;
  adjust_completed_unit_counts(1, unit, update_score, traits, state);
}

void unmark_unit_counted_as_complete(
    game::Unit& unit,
    const bool update_score,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  runtime::require(unit.is_complete(), kOriginalSource, 71, "unit status is complete");
  runtime::require(unit.is_counted_as_complete(), kOriginalSource, 72, "unit counted as complete");
  unit.count_state &= static_cast<std::uint8_t>(~game::unit_count_complete);
  adjust_completed_unit_counts(-1, unit, update_score, traits, state);
}

void adjust_existing_unit_counts(
    const std::int32_t delta,
    const game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  require_unit_type(unit.unit_type, 91);
  require_player(unit.owner, 92);
  if (unit.is_excluded_from_counts()) {
    return;
  }

  auto& existing_count = state.existing_by_type[unit.unit_type][unit.owner];
  existing_count += delta;
  runtime::require(
      state.completed_by_type[unit.unit_type][unit.owner] != -1,
      kOriginalSource,
      102,
      "completed unit count != -1");

  const auto& unit_traits = traits[unit.unit_type];
  const std::uint16_t supply_type = supply_type_for_existing_unit(unit);
  require_unit_type(supply_type, 103);
  const auto& supply_traits = traits[supply_type];
  std::uint32_t supply_required = supply_traits.supply_required;
  const bool special_production_type = unit.unit_type == 36 || unit.unit_type == 59;
  if ((supply_traits.dat_flags & unit_dat_double_supply) != 0 &&
      (special_production_type || !unit.is_complete())) {
    supply_required *= 2U;
  }

  const Race race = race_for_unit_type(unit.unit_type, traits);
  if (race != Race::none) {
    add_wrapped(state.supply.used[race_index(race)][unit.owner], delta, supply_required);
  }

  if ((unit_traits.race_flags & unit_group_20) != 0) {
    state.existing_group_20[unit.owner] += delta;
  }
  if ((unit_traits.race_flags & unit_group_08) != 0 || special_production_type) {
    state.existing_group_08[unit.owner] += delta;
  } else if ((unit_traits.race_flags & unit_group_10) != 0) {
    state.existing_group_10[unit.owner] += delta;
  }
}

void mark_unit_counted_as_existing(
    game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  runtime::require(!unit.is_counted_as_existing(), kOriginalSource, 145, "unit not already counted existing");
  unit.count_state |= game::unit_count_exists;
  adjust_existing_unit_counts(1, unit, traits, state);
}

void unmark_unit_counted_as_existing(
    game::Unit& unit,
    const UnitTraitsTable& traits,
    UnitCountState& state) noexcept {
  runtime::require(unit.is_counted_as_existing(), kOriginalSource, 155, "unit counted as existing");
  require_unit_type(unit.unit_type, 156);
  require_player(unit.owner, 157);
  unit.count_state &= static_cast<std::uint8_t>(~game::unit_count_exists);
  adjust_existing_unit_counts(-1, unit, traits, state);
}

}  // namespace starcraft::lang
