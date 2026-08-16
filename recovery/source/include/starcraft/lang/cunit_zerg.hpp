#pragma once

#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint16_t zerg_larva_type = 35U;
inline constexpr std::uint16_t zerg_egg_type = 36U;
inline constexpr std::uint16_t zerg_drone_type = 41U;
inline constexpr std::uint8_t zerg_larva_limit = 3U;
inline constexpr std::uint8_t zerg_larva_spawn_period = 93U;

struct ZergLarvaSourceView {
  const std::uint16_t* unit_types{};
  std::size_t count{};
};

struct ZergUnitTypeView {
  const std::uint16_t* unit_types{};
  std::size_t count{};
};

[[nodiscard]] bool is_zerg_drone(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_zerg_buildable_unit_type(
    std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_zerg_town_hall(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_zerg_building_morph_target(
    std::uint16_t unit_type) noexcept;
[[nodiscard]] ZergLarvaSourceView zerg_larva_source_types() noexcept;
[[nodiscard]] ZergUnitTypeView zerg_buildable_unit_types() noexcept;
[[nodiscard]] ZergUnitTypeView zerg_building_morph_target_types() noexcept;

}  // namespace starcraft::lang
