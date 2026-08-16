#pragma once

#include "starcraft/lang/place_unit.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint32_t cunit_status_building = 0x00000004U;
inline constexpr std::uint32_t cunit_status_collision_enabled = 0x00080000U;
inline constexpr std::uint32_t cunit_status_collision_ignored = 0x00020000U;
inline constexpr std::uint32_t cunit_status_collision_ignored_secondary = 0x00100000U;
inline constexpr std::uint32_t cunit_status_subunit = 0x00200000U;
inline constexpr std::uint32_t cunit_dat_flag_worker = 0x00000008U;

struct UnitCollisionBody {
  std::uint32_t id{};
  std::uint16_t unit_type{};
  int x{};
  int y{};
  CollisionExtents extents{};
  std::uint32_t status_flags{cunit_status_collision_enabled};
  std::uint32_t unit_type_flags{};
  std::uint8_t sprite_elevation{};
  bool related_subunit{};
};

struct UnitCollisionRect {
  int left{};
  int top{};
  int right{};
  int bottom{};
};

enum class UnitContactSide : std::uint8_t {
  above = 0,
  right = 1,
  below = 2,
  left = 3,
};

struct CardinalEscapeDirections {
  std::array<std::uint8_t, 2> directions{};
  std::size_t count{};
};

struct CollisionEscapePoint {
  int x{};
  int y{};
  std::uint8_t direction{};
};

struct CollisionEscapePoints {
  std::array<CollisionEscapePoint, 2> points{};
  std::size_t count{};
};

[[nodiscard]] UnitCollisionRect unit_collision_rect(
    const UnitCollisionBody& unit) noexcept;

[[nodiscard]] bool unit_collision_rects_overlap(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept;

[[nodiscard]] bool live_unit_blocks_movement(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept;

[[nodiscard]] UnitContactSide unit_contact_side(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept;  // VA 0x0043A070

[[nodiscard]] CardinalEscapeDirections cardinal_escape_directions(
    UnitContactSide contact,
    std::uint8_t movement_direction) noexcept;  // VA 0x00439B90

[[nodiscard]] CollisionEscapePoints collision_escape_points(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle,
    std::uint8_t movement_direction) noexcept;  // VA 0x00439B90

}  // namespace starcraft::lang
