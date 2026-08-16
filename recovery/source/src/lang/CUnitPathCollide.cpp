#include "starcraft/lang/cunit_path_collide.hpp"

#include <cstdlib>

namespace starcraft::lang {

UnitCollisionRect unit_collision_rect(const UnitCollisionBody& unit) noexcept {
  return {
      unit.x - unit.extents.left,
      unit.y - unit.extents.top,
      unit.x + unit.extents.right,
      unit.y + unit.extents.bottom,
  };
}

bool live_unit_blocks_movement(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept {
  // CUnitPathCollide.cpp::sub_4393D0/sub_43AD40 first reject sprites at
  // elevation 11+, collision-disabled units, the two ignored status classes,
  // the mover itself, and attached subunits.
  if (mover.id == obstacle.id || obstacle.sprite_elevation >= 11U ||
      (obstacle.status_flags & cunit_status_collision_enabled) == 0U ||
      (obstacle.status_flags & (cunit_status_collision_ignored |
                                cunit_status_collision_ignored_secondary)) != 0U ||
      ((obstacle.status_flags & cunit_status_subunit) != 0U &&
       obstacle.related_subunit)) {
    return false;
  }

  // Larvae (type 35) only collide with buildings. Other units ignore larvae.
  if (mover.unit_type == 35U) {
    if ((obstacle.status_flags & cunit_status_building) == 0U) {
      return false;
    }
  } else if (obstacle.unit_type == 35U) {
    return false;
  }

  // dword_8E1450 is the units.dat special-ability flags table. Its 0x08 bit
  // is the worker classification (confirmed independently by CUnitHarvest
  // assertions), and sub_4393D0/sub_43AD40 let workers pass mobile units.
  // Resource-source crowding is serialized by CUnitHarvest's worker queue.
  return (mover.unit_type_flags & cunit_dat_flag_worker) == 0U ||
         (obstacle.status_flags & cunit_status_building) != 0U;
}

bool unit_collision_rects_overlap(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept {
  if (!live_unit_blocks_movement(mover, obstacle)) {
    return false;
  }
  const UnitCollisionRect left = unit_collision_rect(mover);
  const UnitCollisionRect right = unit_collision_rect(obstacle);
  // sub_43AD40 uses strict comparisons at all four edges.
  return right.right > left.left && right.left < left.right &&
         right.bottom > left.top && right.top < left.bottom;
}

UnitContactSide unit_contact_side(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle) noexcept {
  const UnitCollisionRect moving = unit_collision_rect(mover);
  const UnitCollisionRect fixed = unit_collision_rect(obstacle);
  if (fixed.right < moving.left) {
    return UnitContactSide::left;
  }
  if (fixed.left > moving.right) {
    return UnitContactSide::right;
  }
  if (fixed.bottom < moving.top) {
    return UnitContactSide::above;
  }
  if (fixed.top > moving.bottom) {
    return UnitContactSide::below;
  }

  // Exact overlap-depth ordering from sub_43A070: right is the initial
  // candidate, then left, above, and below replace it only on a strict win.
  UnitContactSide side = UnitContactSide::right;
  int depth = std::abs(fixed.right - moving.left);
  const int left_depth = std::abs(fixed.left - moving.right);
  if (left_depth < depth) {
    side = UnitContactSide::left;
    depth = left_depth;
  }
  const int above_depth = std::abs(fixed.bottom - moving.top);
  if (above_depth < depth) {
    side = UnitContactSide::above;
    depth = above_depth;
  }
  const int below_depth = std::abs(fixed.top - moving.bottom);
  if (below_depth < depth) {
    side = UnitContactSide::below;
  }
  return side;
}

CardinalEscapeDirections cardinal_escape_directions(
    const UnitContactSide contact,
    const std::uint8_t movement_direction) noexcept {
  // Direction is StarCraft's 0=N, 64=E, 128=S, 192=W byte angle. The original
  // reduces it to the same four quadrants at unit+47 >> 6.
  const std::uint8_t quadrant = movement_direction >> 6U;
  switch (contact) {
    case UnitContactSide::above:
      if (quadrant == 0U) {
        return {{{64U, 192U}}, 2U};
      }
      if (quadrant == 3U) {
        return {{{192U, 64U}}, 2U};
      }
      break;
    case UnitContactSide::right:
      if (quadrant == 0U) {
        return {{{0U, 128U}}, 2U};
      }
      if (quadrant == 1U) {
        return {{{128U, 0U}}, 2U};
      }
      break;
    case UnitContactSide::below:
      if (quadrant == 1U) {
        return {{{64U, 192U}}, 2U};
      }
      if (quadrant == 2U) {
        return {{{192U, 64U}}, 2U};
      }
      break;
    case UnitContactSide::left:
      if (quadrant == 3U) {
        return {{{0U, 128U}}, 2U};
      }
      if (quadrant == 2U) {
        return {{{128U, 0U}}, 2U};
      }
      break;
  }
  return {};
}

CollisionEscapePoints collision_escape_points(
    const UnitCollisionBody& mover,
    const UnitCollisionBody& obstacle,
    const std::uint8_t movement_direction) noexcept {
  const CardinalEscapeDirections directions = cardinal_escape_directions(
      unit_contact_side(mover, obstacle), movement_direction);
  const UnitCollisionRect fixed = unit_collision_rect(obstacle);
  CollisionEscapePoints result{};
  for (std::size_t index = 0; index < directions.count; ++index) {
    const std::uint8_t direction = directions.directions[index];
    CollisionEscapePoint point{mover.x, mover.y, direction};
    // sub_439B90 writes one pixel beyond the obstacle plus the mover's DAT
    // extent, validates that heading, then tries the opposite heading.
    switch (direction) {
      case 0U:
        point.y = fixed.top - mover.extents.bottom - 1;
        break;
      case 64U:
        point.x = fixed.right + mover.extents.left + 1;
        break;
      case 128U:
        point.y = fixed.bottom + mover.extents.top + 1;
        break;
      case 192U:
        point.x = fixed.left - mover.extents.right - 1;
        break;
      default:
        continue;
    }
    result.points[result.count++] = point;
  }
  return result;
}

}  // namespace starcraft::lang
