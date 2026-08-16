#pragma once

#include "starcraft/game/scenario.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::lang {

inline constexpr std::uint8_t retail_melee_starting_worker_count = 4;
inline constexpr std::uint8_t retail_melee_base_supply_internal = 20;

struct MeleeUnitTypes {
  std::uint16_t worker{};
  std::uint16_t base{};
};

struct PlacementPoint {
  int x{};
  int y{};
};

struct CollisionExtents {
  int left{};
  int top{};
  int right{};
  int bottom{};
};

struct ExistingMeleeUnit {
  std::uint16_t unit_type{};
  std::uint8_t owner{};
  bool alive{};
};

struct MeleeBaseDimensions {
  std::uint16_t width{};
  std::uint16_t height{};
};

struct MeleeStartSpawn {
  std::uint16_t unit_type{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint8_t owner{};
};

using PlacementPassable = bool (*)(int x, int y, void* context) noexcept;

// Recovered from U:\SWAR\lang\place_unit.cpp.
[[nodiscard]] bool melee_unit_types(
    std::uint8_t chk_race,
    MeleeUnitTypes& output) noexcept;  // VAs 0x00480B60, 0x00480C80

[[nodiscard]] bool is_melee_starting_unit_type(
    std::uint16_t unit_type) noexcept;
[[nodiscard]] bool melee_starting_unit_matches_race(
    std::uint16_t unit_type,
    std::uint8_t chk_race) noexcept;

[[nodiscard]] PlacementPoint align_melee_base(
    std::uint16_t start_x,
    std::uint16_t start_y,
    std::uint16_t placement_width,
    std::uint16_t placement_height) noexcept;  // VA 0x00480C80

[[nodiscard]] bool build_retail_melee_start_plan(
    const std::array<game::ScenarioPlayer, 12>& players,
    const std::array<game::ScenarioStartLocation, 12>& start_locations,
    const ExistingMeleeUnit* existing_units,
    std::size_t existing_unit_count,
    const std::array<MeleeBaseDimensions, 3>& base_dimensions,
    std::vector<MeleeStartSpawn>& output) noexcept;

// StarCraft.exe CUnitInit.cpp::sub_42EE60 calls place_unit.cpp::sub_47FBF0
// when an incomplete mobile CUnit is ready to become visible.  It searches
// an expanding, eight-pixel-aligned perimeter around the unit's existing
// sprite center when that center is occupied.
[[nodiscard]] bool find_created_unit_position(
    int requested_x,
    int requested_y,
    int map_pixel_width,
    int map_pixel_height,
    const CollisionExtents& mover,
    const CollisionExtents* initial_obstacle,
    PlacementPassable passable,
    void* context,
    PlacementPoint& output) noexcept;  // VA 0x0047FBF0

}  // namespace starcraft::lang
