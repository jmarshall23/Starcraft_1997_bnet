#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/game/image.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_path_collide.hpp"
#include "starcraft/lang/damage.hpp"
#include "starcraft/lang/flingy.hpp"
#include "starcraft/lang/pathfinding.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace starcraft::recovery {

bool restart_unit_animation(const BootstrapStatus &status,
                            ScenarioUnitPreview &unit,
                            const std::uint8_t animation) noexcept {
  if (unit.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  starcraft::lang::IScriptState next{};
  if (!program.valid() || !program.start(asset.iscript_id, animation, next)) {
    return false;
  }
  const auto result =
      program.tick(next, 0, 256, nullptr, status.scenario.tileset_id());
  if ((result != starcraft::lang::IScriptTickResult::yielded &&
       result != starcraft::lang::IScriptTickResult::sleeping) ||
      next.frame >= asset.sprite_frames.size()) {
    return false;
  }
  unit.iscript_state = next;
  if (next.flingy_velocity_event_count != 0U) {
    unit.movement_speed = next.flingy_velocity;
  }
  unit.current_sprite_frame = next.frame;
  unit.iscript_ready = true;
  unit.last_animation = animation;

  // sub_41C060 dispatches the requested action to every image currently
  // attached to the sprite. Action 20 makes the Command Center's image-247
  // working layer execute its terminating script while image 246 returns to
  // idle. Preserve the layer for its first yielded tick; the game loop removes
  // it when the script reaches END on the following tick.
  if (unit.dynamic_overlay_ready &&
      unit.dynamic_overlay_asset_index < status.unit_assets.size()) {
    const UnitRenderAsset &dynamic_asset =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    starcraft::lang::IScriptState dynamic{};
    if (program.start(dynamic_asset.iscript_id, animation, dynamic)) {
      const auto dynamic_result = program.tick(
          dynamic, 0, 256, &unit.iscript_state, status.scenario.tileset_id());
      if ((dynamic_result == starcraft::lang::IScriptTickResult::yielded ||
           dynamic_result == starcraft::lang::IScriptTickResult::sleeping) &&
          dynamic.frame < dynamic_asset.sprite_frames.size()) {
        unit.dynamic_overlay_iscript_state = dynamic;
        unit.iscript_state.image_target_flags |= dynamic.image_target_flags;
        unit.dynamic_overlay_iscript_state.image_target_flags = 0U;
        unit.current_dynamic_overlay_frame = dynamic.frame;
      } else {
        unit.dynamic_overlay_ready = false;
      }
    }
  }
  if (next.overlay_event_count != 0U && asset.overlay_ready &&
      next.overlay_image == asset.overlay_image_id) {
    // sub_41C060 dispatches an action over the sprite image list. Restart a
    // known attached image created by the new main-image action as well; the
    // Command Center working action recreates its image-247 activity layer.
    starcraft::lang::IScriptState overlay{};
    if (program.start(asset.overlay_iscript_id, 0, overlay)) {
      const auto overlay_result = program.tick(
          overlay, 0, 256, &unit.iscript_state, status.scenario.tileset_id());
      if ((overlay_result == starcraft::lang::IScriptTickResult::yielded ||
           overlay_result == starcraft::lang::IScriptTickResult::sleeping) &&
          overlay.frame < asset.overlay_frames.size()) {
        unit.overlay_iscript_state = overlay;
        unit.iscript_state.image_target_flags |= overlay.image_target_flags;
        unit.overlay_iscript_state.image_target_flags = 0U;
        unit.current_overlay_frame = overlay.frame;
        unit.overlay_ready = true;
      }
    }
  } else if (next.overlay_event_count != 0U) {
    const auto dynamic_asset =
        std::find_if(status.unit_assets.begin(), status.unit_assets.end(),
                     [&next](const UnitRenderAsset &candidate) {
                       return candidate.image_id == next.overlay_image;
                     });
    if (dynamic_asset != status.unit_assets.end()) {
      const std::size_t dynamic_index =
          static_cast<std::size_t>(dynamic_asset - status.unit_assets.begin());
      starcraft::lang::IScriptState dynamic{};
      if (program.start(dynamic_asset->iscript_id, 0U, dynamic)) {
        const auto dynamic_result = program.tick(
            dynamic, 0, 256, &unit.iscript_state, status.scenario.tileset_id());
        if ((dynamic_result == starcraft::lang::IScriptTickResult::yielded ||
             dynamic_result == starcraft::lang::IScriptTickResult::sleeping) &&
            dynamic.frame < dynamic_asset->sprite_frames.size()) {
          unit.dynamic_overlay_asset_index = dynamic_index;
          unit.dynamic_overlay_iscript_state = dynamic;
          unit.iscript_state.image_target_flags |= dynamic.image_target_flags;
          unit.dynamic_overlay_iscript_state.image_target_flags = 0U;
          unit.current_dynamic_overlay_frame = dynamic.frame;
          unit.dynamic_overlay_x_offset = next.overlay_x_offset;
          unit.dynamic_overlay_y_offset = next.overlay_y_offset;
          unit.dynamic_overlay_above = next.overlay_above;
          unit.dynamic_overlay_ready = true;
        }
      }
    }
  }
  return true;
}

bool advance_building_construction_animation(
    const BootstrapStatus &status, ScenarioUnitPreview &building) noexcept {
  if (!building.alive || !building.is_building ||
      building.construction_complete || building.max_hit_points == 0U) {
    return false;
  }
  // CUnitBuild.cpp::sub_423210 at 0x00423210 advances its byte +90 state
  // one case per update. Life crossings at 1/5, 2/5, and 3/5 dispatch
  // IScript animations 13, 14, and 15; the fourth crossing is state-only.
  switch (building.construction_animation_phase) {
  case 0:
    if (building.hit_points > building.max_hit_points / 5U) {
      building.construction_animation_phase = 1U;
      return true;
    }
    break;
  case 1:
    (void)restart_unit_animation(status, building, 13U);
    building.construction_animation_phase = 2U;
    return true;
  case 2:
    if (building.hit_points > 2U * building.max_hit_points / 5U) {
      building.construction_animation_phase = 3U;
      return true;
    }
    break;
  case 3:
    (void)restart_unit_animation(status, building, 14U);
    building.construction_animation_phase = 4U;
    return true;
  case 4:
    if (building.hit_points > 3U * building.max_hit_points / 5U) {
      building.construction_animation_phase = 5U;
      return true;
    }
    break;
  case 5:
    (void)restart_unit_animation(status, building, 15U);
    building.construction_animation_phase = 6U;
    return true;
  case 6:
    if (building.hit_points > 4U * building.max_hit_points / 5U) {
      building.construction_animation_phase = 7U;
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

bool collect_building_obstacles(
    const BootstrapStatus &status, const ScenarioUnitPreview *ignored_unit,
    std::vector<starcraft::lang::PathObstacle> &output) noexcept {
  output.clear();
  try {
    output.reserve(status.units.size());
    for (const ScenarioUnitPreview &unit : status.units) {
      if (&unit == ignored_unit || !unit.alive || !unit.is_building ||
          unit.selection_width == 0 || unit.selection_height == 0) {
        continue;
      }
      // CUnitPathCollide.cpp::sub_43AC00/sub_43AD40 build the blocking
      // rectangle from the four units.dat collision extents at
      // dword_8E0990/dword_8E0994.  The placement dimensions are deliberately
      // larger for structures such as a Hatchery; using them here traps a
      // correctly settled Drone in the non-colliding apron and makes every
      // movement tick re-plan from the same point.
      const bool has_dat_extents =
          unit.collision_left != 0U || unit.collision_top != 0U ||
          unit.collision_right != 0U || unit.collision_bottom != 0U;
      const int left =
          static_cast<int>(unit.x) -
          (has_dat_extents ? unit.collision_left : unit.selection_width / 2);
      const int top =
          static_cast<int>(unit.y) -
          (has_dat_extents ? unit.collision_top : unit.selection_height / 2);
      const int right =
          static_cast<int>(unit.x) +
          (has_dat_extents ? unit.collision_right
                           : unit.selection_width - unit.selection_width / 2);
      const int bottom =
          static_cast<int>(unit.y) +
          (has_dat_extents
               ? unit.collision_bottom
               : unit.selection_height - unit.selection_height / 2);
      output.push_back({
          left,
          top,
          right,
          bottom,
      });
    }
    return true;
  } catch (...) {
    output.clear();
    return false;
  }
}

starcraft::lang::UnitCollisionBody
collision_body_for(const ScenarioUnitPreview &mover, const int mover_x,
                   const int mover_y) noexcept {
  const bool has_dat_extents =
      mover.collision_left != 0U || mover.collision_top != 0U ||
      mover.collision_right != 0U || mover.collision_bottom != 0U;
  return {
      mover.unit_id,
      mover.unit_type,
      mover_x,
      mover_y,
      {
          has_dat_extents ? mover.collision_left : mover.selection_width / 2,
          has_dat_extents ? mover.collision_top : mover.selection_height / 2,
          has_dat_extents ? mover.collision_right
                          : mover.selection_width - mover.selection_width / 2,
          has_dat_extents ? mover.collision_bottom
                          : mover.selection_height - mover.selection_height / 2,
      },
      starcraft::lang::cunit_status_collision_enabled |
          (mover.is_building ? starcraft::lang::cunit_status_building : 0U),
      mover.dat_flags,
      0,
      false,
  };
}

bool unit_footprints_overlap_at(const ScenarioUnitPreview &mover,
                                const int mover_x, const int mover_y,
                                const ScenarioUnitPreview &obstacle) noexcept {
  if (!obstacle.alive || mover.selection_width == 0 ||
      mover.selection_height == 0 || obstacle.selection_width == 0 ||
      obstacle.selection_height == 0) {
    return false;
  }
  const starcraft::lang::UnitCollisionRect moving =
      starcraft::lang::unit_collision_rect(
          collision_body_for(mover, mover_x, mover_y));
  const starcraft::lang::UnitCollisionRect fixed =
      starcraft::lang::unit_collision_rect(
          collision_body_for(obstacle, obstacle.x, obstacle.y));
  return fixed.right > moving.left && fixed.left < moving.right &&
         fixed.bottom > moving.top && fixed.top < moving.bottom;
}

bool unit_rectangles_overlap_at(const ScenarioUnitPreview &mover,
                                const int mover_x, const int mover_y,
                                const ScenarioUnitPreview &obstacle) noexcept {
  if (!obstacle.alive || mover.selection_width == 0 ||
      mover.selection_height == 0 || obstacle.selection_width == 0 ||
      obstacle.selection_height == 0) {
    return false;
  }
  return starcraft::lang::unit_collision_rects_overlap(
      collision_body_for(mover, mover_x, mover_y),
      collision_body_for(obstacle, obstacle.x, obstacle.y));
}

const ScenarioUnitPreview *
find_live_unit_collision(const BootstrapStatus &status,
                         const ScenarioUnitPreview &mover, const int proposed_x,
                         const int proposed_y) noexcept {
  for (const ScenarioUnitPreview &obstacle : status.units) {
    if (&obstacle != &mover && obstacle.alive &&
        unit_rectangles_overlap_at(mover, proposed_x, proposed_y, obstacle)) {
      return &obstacle;
    }
  }
  return nullptr;
}

const ScenarioUnitPreview *find_live_unit_footprint_collision(
    const BootstrapStatus &status, const ScenarioUnitPreview &mover,
    const int proposed_x, const int proposed_y) noexcept {
  for (const ScenarioUnitPreview &obstacle : status.units) {
    if (&obstacle != &mover && obstacle.alive &&
        unit_footprints_overlap_at(mover, proposed_x, proposed_y, obstacle)) {
      return &obstacle;
    }
  }
  return nullptr;
}

bool creation_position_passable(const BootstrapStatus &status,
                                const ScenarioUnitPreview &unit, const int x,
                                const int y) noexcept {
  static const std::vector<starcraft::lang::PathObstacle> no_obstacles;
  return x >= 0 && y >= 0 && x <= UINT16_MAX && y <= UINT16_MAX &&
         starcraft::lang::path_position_passable(
             status.pathing_map, x, y, unit.selection_width,
             unit.selection_height, no_obstacles) &&
         find_live_unit_footprint_collision(status, unit, x, y) == nullptr;
}

struct CreatedUnitPlacementContext {
  const BootstrapStatus *status{};
  const ScenarioUnitPreview *unit{};
};

bool creation_position_passable_callback(const int x, const int y,
                                         void *const context) noexcept {
  const auto *const placement =
      static_cast<const CreatedUnitPlacementContext *>(context);
  return placement != nullptr && placement->status != nullptr &&
         placement->unit != nullptr &&
         creation_position_passable(*placement->status, *placement->unit, x, y);
}

bool settle_created_unit(BootstrapStatus &status, ScenarioUnitPreview &unit,
                         const std::uint16_t requested_x,
                         const std::uint16_t requested_y) noexcept {
  // StarCraft.exe CUnitInit.cpp::sub_42EE60 passes the hidden trainee's
  // current sprite center to place_unit.cpp::sub_47FBF0.  For building
  // production that center is the producer center written by
  // CUnitBuild.cpp::sub_421EF0; the placement search, not a fabricated exit
  // offset, chooses the first clear eight-pixel-aligned perimeter point.
  const auto extents_for = [](const ScenarioUnitPreview &value) {
    const bool has_dat_extents =
        value.collision_left != 0U || value.collision_top != 0U ||
        value.collision_right != 0U || value.collision_bottom != 0U;
    return starcraft::lang::CollisionExtents{
        has_dat_extents ? value.collision_left : value.selection_width / 2,
        has_dat_extents ? value.collision_top : value.selection_height / 2,
        has_dat_extents ? value.collision_right
                        : value.selection_width - value.selection_width / 2,
        has_dat_extents ? value.collision_bottom
                        : value.selection_height - value.selection_height / 2,
    };
  };
  const ScenarioUnitPreview *const collision =
      find_live_unit_footprint_collision(status, unit, requested_x,
                                         requested_y);
  const starcraft::lang::CollisionExtents mover_extents = extents_for(unit);
  const starcraft::lang::CollisionExtents obstacle_extents =
      collision == nullptr ? starcraft::lang::CollisionExtents{}
                           : extents_for(*collision);
  CreatedUnitPlacementContext context{&status, &unit};
  starcraft::lang::PlacementPoint placement{};
  if (!starcraft::lang::find_created_unit_position(
          requested_x, requested_y, status.pathing_map.pixel_width(),
          status.pathing_map.pixel_height(), mover_extents,
          collision == nullptr ? nullptr : &obstacle_extents,
          creation_position_passable_callback, &context, placement)) {
    return false;
  }
  unit.x = static_cast<std::uint16_t>(placement.x);
  unit.y = static_cast<std::uint16_t>(placement.y);
  unit.x_fixed = placement.x << 8;
  unit.y_fixed = placement.y << 8;
  return true;
}

bool settle_melee_starting_workers(BootstrapStatus &status) noexcept {
  if (!status.pathing_map.valid() || !status.scenario.valid()) {
    return false;
  }
  bool settled = true;
  for (std::size_t player = 0; player < 8U; ++player) {
    if (!status.active_players[player]) {
      continue;
    }
    const std::uint8_t race = status.scenario.players()[player].race;
    const starcraft::game::ScenarioStartLocation &start =
        status.scenario.start_locations()[player];
    starcraft::lang::MeleeUnitTypes unit_types{};
    if (!starcraft::lang::melee_unit_types(race, unit_types) ||
        !start.present) {
      settled = false;
      continue;
    }
    const std::uint16_t worker_type = unit_types.worker;
    for (ScenarioUnitPreview &worker : status.units) {
      if (!worker.alive || worker.owner != player ||
          worker.unit_type != worker_type || worker.x != start.x ||
          worker.y != start.y) {
        continue;
      }
      settled =
          settle_created_unit(status, worker, start.x, start.y) && settled;
    }
  }
  return settled;
}

bool plan_scv_path(BootstrapStatus &status, ScenarioUnitPreview &unit,
                   const std::uint16_t target_x,
                   const std::uint16_t target_y) noexcept {
  std::vector<starcraft::lang::PathObstacle> obstacles;
  std::vector<starcraft::lang::PathPoint> path;
  if (!collect_building_obstacles(status, &unit, obstacles) ||
      !starcraft::lang::find_unit_path(
          status.pathing_map, unit.x, unit.y, target_x, target_y,
          unit.selection_width, unit.selection_height, obstacles, path) ||
      path.empty()) {
    return false;
  }
  unit.movement_path = std::move(path);
  unit.movement_path_index = 0;
  // find_unit_path mirrors CUnitPath's altered-destination behavior when the
  // requested center is not passable. Keep the actual terminal point; using
  // the rejected request here makes collision replans repeatedly steer units
  // toward map corners.
  unit.movement_final_x = unit.movement_path.back().x;
  unit.movement_final_y = unit.movement_path.back().y;
  unit.movement_target_x = unit.movement_path.front().x;
  unit.movement_target_y = unit.movement_path.front().y;
  unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
  unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
  return true;
}

void stop_unit_movement(const BootstrapStatus &status,
                        ScenarioUnitPreview &unit) noexcept {
  unit.moving = false;
  unit.movement_speed = 0;
  unit.movement_path.clear();
  unit.movement_path_index = 0;
  unit.avoidance_ticks = 0;
  unit.collision_wait_ticks = 0;
  unit.avoidance_turn = 0;
  // CFlingy.cpp::sub_406670 at 0x00406670 dispatches image animation 12
  // when movement stops. Fall back to the recovered init animation only if
  // this image does not publish that action.
  if (!restart_unit_animation(status, unit, 12) &&
      unit.asset_index < status.unit_assets.size()) {
    const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
    unit.iscript_state = asset.initial_iscript_state;
    unit.current_sprite_frame = asset.initial_iscript_state.frame;
    unit.iscript_ready = asset.iscript_ready;
  }
}

void cancel_unit_order(BootstrapStatus &status,
                       ScenarioUnitPreview &unit) noexcept {
  if (unit.active_order == ActiveUnitOrder::construct &&
      unit.order_target_id != 0) {
    for (ScenarioUnitPreview &target : status.units) {
      if (target.unit_id == unit.order_target_id &&
          target.construction_builder_id == unit.unit_id) {
        target.construction_builder_id = 0;
        break;
      }
    }
  }
  if (unit.harvest_source_id != 0U) {
    for (ScenarioUnitPreview &source : status.units) {
      if (source.unit_id == unit.harvest_source_id) {
        (void)source.harvest_queue.release(unit.unit_id);
        break;
      }
    }
  }
  stop_unit_movement(status, unit);
  unit.active_order = ActiveUnitOrder::none;
  unit.order_target_id = 0;
  unit.harvest_source_id = 0;
  unit.action_timer = 0;
  unit.action_phase = 0;
}

std::size_t issue_scv_move_order(BootstrapStatus &status,
                                 const std::uint16_t target_x,
                                 const std::uint16_t target_y) noexcept {
  std::size_t issued{};
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.selected || !unit.alive || unit.owner != 0 || unit.is_building ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    if (!plan_scv_path(status, unit, target_x, target_y)) {
      cancel_unit_order(status, unit);
      continue;
    }
    if (!unit.moving) {
      unit.movement_speed = 0;
      // The same 0x00406670 state transition dispatches animation 11 when a
      // stationary CFlingy begins moving.
      (void)restart_unit_animation(status, unit, 11);
    }
    unit.moving = true;
    unit.active_order = ActiveUnitOrder::move;
    unit.order_target_id = 0;
    unit.action_timer = 0;
    ++issued;
  }
  return issued;
}

std::uint32_t
effective_unit_top_speed(const ScenarioUnitPreview &unit) noexcept {
  // CImage.cpp::sub_415210 case 0x2C writes the walking animation's current
  // velocity to CFlingy+0x48; case 0x3F can replace the top speed at +0x40.
  // Iscript-controlled walkers commonly retain DAT's value 2 until action 11.
  return unit.iscript_state.flingy_speed_event_count != 0U &&
                 unit.iscript_state.flingy_speed != 0U
             ? unit.iscript_state.flingy_speed
             : (unit.iscript_state.flingy_velocity_event_count != 0U &&
                        unit.iscript_state.flingy_velocity != 0U
                    ? unit.iscript_state.flingy_velocity
                    : unit.movement_top_speed);
}

ScenarioUnitPreview *find_unit_by_id(BootstrapStatus &status,
                                     const std::uint32_t unit_id) noexcept {
  const auto unit =
      std::find_if(status.units.begin(), status.units.end(),
                   [unit_id](const ScenarioUnitPreview &value) {
                     return value.unit_id == unit_id && value.alive;
                   });
  return unit == status.units.end() ? nullptr : &*unit;
}

const ScenarioUnitPreview *
find_unit_by_id(const BootstrapStatus &status,
                const std::uint32_t unit_id) noexcept {
  const auto unit =
      std::find_if(status.units.begin(), status.units.end(),
                   [unit_id](const ScenarioUnitPreview &value) {
                     return value.unit_id == unit_id && value.alive;
                   });
  return unit == status.units.end() ? nullptr : &*unit;
}

int unit_edge_distance(const ScenarioUnitPreview &left,
                       const ScenarioUnitPreview &right) noexcept {
  const auto extents = [](const ScenarioUnitPreview &unit) {
    const bool from_dat = unit.collision_left != 0U ||
                          unit.collision_top != 0U ||
                          unit.collision_right != 0U ||
                          unit.collision_bottom != 0U;
    return std::array<int, 4>{{
        from_dat ? unit.collision_left : unit.selection_width / 2,
        from_dat ? unit.collision_top : unit.selection_height / 2,
        from_dat ? unit.collision_right
                 : unit.selection_width - unit.selection_width / 2 - 1,
        from_dat ? unit.collision_bottom
                 : unit.selection_height - unit.selection_height / 2 - 1,
    }};
  };
  const auto left_extents = extents(left);
  const auto right_extents = extents(right);
  const int left_min_x = static_cast<int>(left.x) - left_extents[0];
  const int left_min_y = static_cast<int>(left.y) - left_extents[1];
  const int left_max_x = static_cast<int>(left.x) + left_extents[2];
  const int left_max_y = static_cast<int>(left.y) + left_extents[3];
  const int right_min_x = static_cast<int>(right.x) - right_extents[0];
  const int right_min_y = static_cast<int>(right.y) - right_extents[1];
  const int right_max_x = static_cast<int>(right.x) + right_extents[2];
  const int right_max_y = static_cast<int>(right.y) + right_extents[3];
  const int dx = left_max_x < right_min_x
                     ? right_min_x - left_max_x - 1
                 : right_max_x < left_min_x
                     ? left_min_x - right_max_x - 1
                     : 0;
  const int dy = left_max_y < right_min_y
                     ? right_min_y - left_max_y - 1
                 : right_max_y < left_min_y
                     ? left_min_y - right_max_y - 1
                     : 0;
  return static_cast<int>(std::lround(
      std::sqrt(static_cast<double>(dx) * dx + static_cast<double>(dy) * dy)));
}

int interaction_range_for(const ScenarioUnitPreview &unit,
                          const ActiveUnitOrder order) noexcept {
  // CUnitBuild.cpp::sub_422160 uses sub_429750(..., 5, target), while the
  // mineral approach in CUnitHarvest.cpp uses 0x18. Combat supplies its DAT
  // weapon range to that same collision-rectangle distance helper.
  if (order == ActiveUnitOrder::attack) {
    return static_cast<int>(unit.weapon_range);
  }
  if (order == ActiveUnitOrder::gather ||
      order == ActiveUnitOrder::return_cargo) {
    return 0x18;
  }
  return 5;
}

bool plan_scv_interaction_path(BootstrapStatus &status,
                               ScenarioUnitPreview &worker,
                               const ScenarioUnitPreview &target,
                               const int interaction_range) noexcept {
  const bool worker_dat_extents =
      worker.collision_left != 0U || worker.collision_top != 0U ||
      worker.collision_right != 0U || worker.collision_bottom != 0U;
  const bool target_dat_extents =
      target.collision_left != 0U || target.collision_top != 0U ||
      target.collision_right != 0U || target.collision_bottom != 0U;
  const int worker_left = worker_dat_extents
                              ? worker.collision_left
                              : worker.selection_width / 2;
  const int worker_top = worker_dat_extents
                             ? worker.collision_top
                             : worker.selection_height / 2;
  const int worker_right =
      worker_dat_extents
          ? worker.collision_right
          : worker.selection_width - worker.selection_width / 2 - 1;
  const int worker_bottom =
      worker_dat_extents
          ? worker.collision_bottom
          : worker.selection_height - worker.selection_height / 2 - 1;
  const int target_left =
      static_cast<int>(target.x) -
      (target_dat_extents ? target.collision_left
                          : target.selection_width / 2);
  const int target_top =
      static_cast<int>(target.y) -
      (target_dat_extents ? target.collision_top
                          : target.selection_height / 2);
  const int target_right =
      static_cast<int>(target.x) +
      (target_dat_extents
           ? target.collision_right
           : target.selection_width - target.selection_width / 2 - 1);
  const int target_bottom =
      static_cast<int>(target.y) +
      (target_dat_extents
           ? target.collision_bottom
           : target.selection_height - target.selection_height / 2 - 1);
  const int minimum_x = worker_left;
  const int minimum_y = worker_top;
  const int maximum_x =
      static_cast<int>(status.pathing_map.pixel_width()) -
      worker_right - 1;
  const int maximum_y =
      static_cast<int>(status.pathing_map.pixel_height()) -
      worker_bottom - 1;
  if (maximum_x < minimum_x || maximum_y < minimum_y) {
    return false;
  }
  std::vector<starcraft::lang::PathPoint> points;
  try {
    const int horizontal_begin = target_left - worker_right;
    const int horizontal_end = target_right + worker_left;
    const int vertical_begin = target_top - worker_bottom;
    const int vertical_end = target_bottom + worker_top;
    points.reserve(static_cast<std::size_t>(
        8 + 2 * ((horizontal_end - horizontal_begin) / 8 + 1) +
        2 * ((vertical_end - vertical_begin) / 8 + 1)));
    const auto append = [&](const int x, const int y) {
      if (x < minimum_x || x > maximum_x || y < minimum_y ||
          y > maximum_y) {
        return;
      }
      const starcraft::lang::PathPoint point{
          static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y)};
      if (std::find_if(points.begin(), points.end(),
                       [&point](const starcraft::lang::PathPoint &candidate) {
                         return candidate.x == point.x &&
                                candidate.y == point.y;
                       }) == points.end()) {
        points.push_back(point);
      }
    };
    for (int gap = 0; gap <= interaction_range; ++gap) {
      const int left_x = target_left - worker_right - 1 - gap;
      const int right_x = target_right + worker_left + 1 + gap;
      for (int y = vertical_begin; y <= vertical_end; y += 8) {
        append(left_x, y);
        append(right_x, y);
      }
      append(left_x, target.y);
      append(right_x, target.y);
      const int top_y = target_top - worker_bottom - 1 - gap;
      const int bottom_y = target_bottom + worker_top + 1 + gap;
      for (int x = horizontal_begin; x <= horizontal_end; x += 8) {
        append(x, top_y);
        append(x, bottom_y);
      }
      append(target.x, top_y);
      append(target.x, bottom_y);
    }
  } catch (...) {
    return false;
  }
  std::sort(points.begin(), points.end(),
            [&worker](const starcraft::lang::PathPoint &a,
                      const starcraft::lang::PathPoint &b) {
              const std::int64_t a_dx = static_cast<int>(a.x) - worker.x;
              const std::int64_t a_dy = static_cast<int>(a.y) - worker.y;
              const std::int64_t b_dx = static_cast<int>(b.x) - worker.x;
              const std::int64_t b_dy = static_cast<int>(b.y) - worker.y;
              return a_dx * a_dx + a_dy * a_dy < b_dx * b_dx + b_dy * b_dy;
            });
  for (const starcraft::lang::PathPoint &point : points) {
    if (!plan_scv_path(status, worker, point.x, point.y)) {
      continue;
    }
    ScenarioUnitPreview terminal = worker;
    terminal.x = worker.movement_final_x;
    terminal.y = worker.movement_final_y;
    if (unit_edge_distance(terminal, target) <= interaction_range) {
      return true;
    }
  }
  return false;
}

bool begin_scv_interaction(BootstrapStatus &status, ScenarioUnitPreview &worker,
                           const ScenarioUnitPreview &target,
                           const ActiveUnitOrder order) noexcept {
  const bool worker_only_order = order == ActiveUnitOrder::repair ||
                                 order == ActiveUnitOrder::construct ||
                                 order == ActiveUnitOrder::gather ||
                                 order == ActiveUnitOrder::return_cargo;
  const int interaction_range = interaction_range_for(worker, order);
  if (!worker.alive || worker.is_building || worker.movement_top_speed == 0U ||
      (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
      !target.alive || !plan_scv_interaction_path(
                           status, worker, target, interaction_range)) {
    cancel_unit_order(status, worker);
    return false;
  }
  if (!worker.moving) {
    worker.movement_speed = 0;
    (void)restart_unit_animation(status, worker, 11);
  }
  worker.moving = true;
  worker.active_order = order;
  worker.order_target_id = target.unit_id;
  worker.action_timer = 0;
  worker.action_phase = 0;
  if (order == ActiveUnitOrder::gather) {
    worker.harvest_source_id = target.unit_id;
  }
  return true;
}

void cancel_command_target(BootstrapStatus &status) noexcept {
  status.command_target_active = false;
  status.target_unit_order = 0;
  status.target_terrain_order = 0;
}

void begin_command_target(BootstrapStatus &status,
                          const std::uint8_t unit_order,
                          const std::uint8_t terrain_order) noexcept {
  // target.cpp::sub_4B0120 at 0x004B0120 stores two distinct order IDs:
  // usel.cpp::sub_4B4730 at 0x004B4730 selects the first for a clicked unit
  // and the second for empty terrain.
  status.active_command_card = 0;
  status.placement_active = false;
  status.placement_valid = false;
  status.placement_unit_type = 0xFFFFU;
  status.command_target_active = true;
  status.target_unit_order = unit_order;
  status.target_terrain_order = terrain_order;
}

std::size_t issue_active_scv_target(BootstrapStatus &status,
                                    const std::uint16_t world_x,
                                    const std::uint16_t world_y) noexcept {
  if (!status.command_target_active) {
    return 0;
  }
  const ScenarioUnitPreview *hit{};
  std::uint64_t best_distance = UINT64_MAX;
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || unit.selected) {
      continue;
    }
    const int half_width = static_cast<int>(unit.selection_width) / 2;
    const int half_height = static_cast<int>(unit.selection_height) / 2;
    if (static_cast<int>(world_x) < static_cast<int>(unit.x) - half_width ||
        static_cast<int>(world_x) > static_cast<int>(unit.x) + half_width ||
        static_cast<int>(world_y) < static_cast<int>(unit.y) - half_height ||
        static_cast<int>(world_y) > static_cast<int>(unit.y) + half_height) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(world_x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(world_y) - unit.y;
    const std::uint64_t distance =
        static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < best_distance) {
      best_distance = distance;
      hit = &unit;
    }
  }
  const std::uint8_t order =
      hit == nullptr ? status.target_terrain_order : status.target_unit_order;
  const std::uint16_t target_x = hit == nullptr ? world_x : hit->x;
  const std::uint16_t target_y = hit == nullptr ? world_y : hit->y;
  status.last_issued_order = order;
  cancel_command_target(status);

  if (hit == nullptr || order == 7 || order == 15 || order == 0x32) {
    return issue_scv_move_order(status, target_x, target_y);
  }

  ActiveUnitOrder active_order{ActiveUnitOrder::none};
  if (order == 9 && hit->owner != 0) {
    active_order = ActiveUnitOrder::attack;
  } else if (order == 0x24 && hit->owner == 0 && hit->is_building &&
             hit->hit_points < hit->max_hit_points) {
    active_order = hit->construction_complete ? ActiveUnitOrder::repair
                                              : ActiveUnitOrder::construct;
  } else if (order == 0x4E && (hit->dat_flags & 0x2000U) != 0 &&
             hit->resource_amount != 0) {
    active_order = ActiveUnitOrder::gather;
  }
  if (active_order == ActiveUnitOrder::none) {
    return 0;
  }
  std::size_t issued{};
  for (ScenarioUnitPreview &worker : status.units) {
    const bool worker_only_order = active_order != ActiveUnitOrder::attack;
    if (!worker.selected || !worker.alive || worker.owner != 0 ||
        worker.is_building || worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (active_order == ActiveUnitOrder::attack &&
         !worker.has_ground_weapon)) {
      continue;
    }
    if (begin_scv_interaction(status, worker, *hit, active_order)) {
      if (active_order == ActiveUnitOrder::construct) {
        ScenarioUnitPreview *const building =
            find_unit_by_id(status, worker.order_target_id);
        if (building != nullptr) {
          building->construction_builder_id = worker.unit_id;
        }
      }
      ++issued;
    }
  }
  return issued;
}

std::size_t issue_scv_return_cargo(BootstrapStatus &status) noexcept;

std::size_t issue_scv_smart_order(BootstrapStatus &status,
                                  const std::uint16_t world_x,
                                  const std::uint16_t world_y) noexcept {
  const ScenarioUnitPreview *hit{};
  std::uint64_t best_distance = UINT64_MAX;
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || unit.selected) {
      continue;
    }
    const int half_width = static_cast<int>(unit.selection_width) / 2;
    const int half_height = static_cast<int>(unit.selection_height) / 2;
    if (static_cast<int>(world_x) < static_cast<int>(unit.x) - half_width ||
        static_cast<int>(world_x) > static_cast<int>(unit.x) + half_width ||
        static_cast<int>(world_y) < static_cast<int>(unit.y) - half_height ||
        static_cast<int>(world_y) > static_cast<int>(unit.y) + half_height) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(world_x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(world_y) - unit.y;
    const std::uint64_t distance =
        static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < best_distance) {
      best_distance = distance;
      hit = &unit;
    }
  }
  if (hit == nullptr) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, world_x, world_y);
  }

  ActiveUnitOrder order{ActiveUnitOrder::none};
  if ((hit->dat_flags & 0x2000U) != 0U && hit->resource_amount != 0U) {
    // rclick.cpp's worker classification selects the harvest order for a
    // resource-bearing target before testing hostile ownership.
    order = ActiveUnitOrder::gather;
    status.last_issued_order = 0x4EU;
  } else if (hit->owner != 0U && hit->owner != 11U) {
    order = ActiveUnitOrder::attack;
    status.last_issued_order = 9U;
  } else if (hit->owner == 0U && hit->is_building &&
             (!hit->construction_complete ||
              hit->hit_points < hit->max_hit_points)) {
    order = hit->construction_complete ? ActiveUnitOrder::repair
                                       : ActiveUnitOrder::construct;
    status.last_issued_order = 0x24U;
  } else if (hit->owner == 0U && (hit->dat_flags & 0x1000U) != 0U) {
    status.last_command_opcode = 34U;
    return issue_scv_return_cargo(status);
  }
  if (order == ActiveUnitOrder::none) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, hit->x, hit->y);
  }

  std::size_t issued{};
  for (ScenarioUnitPreview &worker : status.units) {
    const bool worker_only_order = order != ActiveUnitOrder::attack;
    if (!worker.alive || !worker.selected || worker.owner != 0U ||
        worker.is_building || worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (order == ActiveUnitOrder::attack && !worker.has_ground_weapon)) {
      continue;
    }
    if (begin_scv_interaction(status, worker, *hit, order)) {
      if (order == ActiveUnitOrder::construct) {
        ScenarioUnitPreview *const building =
            find_unit_by_id(status, worker.order_target_id);
        if (building != nullptr) {
          building->construction_builder_id = worker.unit_id;
        }
      }
      ++issued;
    }
  }
  // rclick.cpp classifies every selected CUnit. If the target-specific order
  // did not apply to any selected unit (for example Marines on a mineral
  // patch), the mobile selection still receives the ordinary terrain move.
  if (issued == 0U) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, hit->x, hit->y);
  }
  return issued;
}

std::size_t issue_scv_return_cargo(BootstrapStatus &status) noexcept {
  const ScenarioUnitPreview *worker = first_selected_unit(status);
  if (worker == nullptr || (worker->dat_flags & 0x08U) == 0U) {
    return 0;
  }
  const ScenarioUnitPreview *nearest{};
  std::uint64_t nearest_distance = UINT64_MAX;
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || (unit.dat_flags & 0x1000U) == 0 ||
        unit.owner != worker->owner) {
      continue;
    }
    const std::int64_t dx = static_cast<std::int64_t>(worker->x) - unit.x;
    const std::int64_t dy = static_cast<std::int64_t>(worker->y) - unit.y;
    const std::uint64_t distance =
        static_cast<std::uint64_t>(dx * dx + dy * dy);
    if (distance < nearest_distance) {
      nearest_distance = distance;
      nearest = &unit;
    }
  }
  if (nearest == nullptr) {
    return 0;
  }
  status.last_command_opcode = 34;
  std::size_t issued{};
  for (ScenarioUnitPreview &selected : status.units) {
    if (!selected.selected || !selected.alive ||
        (selected.dat_flags & 0x08U) == 0U ||
        selected.owner != nearest->owner) {
      continue;
    }
    issued += begin_scv_interaction(status, selected, *nearest,
                                    ActiveUnitOrder::return_cargo)
                  ? 1U
                  : 0U;
  }
  return issued;
}

bool advance_unit_movement(BootstrapStatus &status) noexcept {
  bool changed{};
  constexpr double pi = 3.14159265358979323846;
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || !unit.moving || unit.is_building ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    const std::uint32_t top_speed = effective_unit_top_speed(unit);
    const std::uint32_t acceleration = unit.movement_acceleration;
    const std::int32_t delta_x =
        static_cast<std::int32_t>(unit.movement_target_x) - unit.x;
    const std::int32_t delta_y =
        static_cast<std::int32_t>(unit.movement_target_y) - unit.y;
    const double distance = std::sqrt(static_cast<double>(delta_x) * delta_x +
                                      static_cast<double>(delta_y) * delta_y);
    if (distance <= static_cast<double>(unit.movement_speed) / 256.0 + 1.0 &&
        find_live_unit_collision(status, unit, unit.movement_target_x,
                                 unit.movement_target_y) == nullptr) {
      unit.x = unit.movement_target_x;
      unit.y = unit.movement_target_y;
      unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
      unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
      if (unit.movement_path_index + 1U < unit.movement_path.size()) {
        ++unit.movement_path_index;
        unit.movement_target_x = unit.movement_path[unit.movement_path_index].x;
        unit.movement_target_y = unit.movement_path[unit.movement_path_index].y;
      } else {
        stop_unit_movement(status, unit);
        if (unit.active_order == ActiveUnitOrder::move) {
          unit.active_order = ActiveUnitOrder::none;
        }
      }
      changed = true;
      continue;
    }

    const std::uint8_t desired = starcraft::lang::direction_from_points(
        unit.x, unit.y, unit.movement_target_x, unit.movement_target_y);
    const std::int8_t turn = starcraft::lang::clamp_turn_delta(
        unit.direction, desired, unit.movement_turn_speed);
    unit.direction = static_cast<std::uint8_t>(unit.direction + turn);
    unit.movement_speed =
        (std::min)(top_speed, unit.movement_speed + acceleration);
    const std::int32_t map_width =
        static_cast<std::int32_t>(status.scenario_width) * 32;
    const std::int32_t map_height =
        static_cast<std::int32_t>(status.scenario_height) * 32;
    const auto movement_candidate = [&](const std::uint8_t direction) {
      const double angle = static_cast<double>(direction) * (2.0 * pi / 256.0);
      std::int32_t x_fixed =
          unit.x_fixed + static_cast<std::int32_t>(std::lround(
                             std::sin(angle) * unit.movement_speed));
      std::int32_t y_fixed =
          unit.y_fixed - static_cast<std::int32_t>(std::lround(
                             std::cos(angle) * unit.movement_speed));
      x_fixed = (std::clamp)(x_fixed, 0, (std::max)(0, (map_width - 1) << 8U));
      y_fixed = (std::clamp)(y_fixed, 0, (std::max)(0, (map_height - 1) << 8U));
      return std::array<std::int32_t, 2>{{x_fixed, y_fixed}};
    };
    std::array<std::int32_t, 2> proposed = movement_candidate(unit.direction);
    std::uint16_t proposed_x = static_cast<std::uint16_t>(proposed[0] >> 8U);
    std::uint16_t proposed_y = static_cast<std::uint16_t>(proposed[1] >> 8U);
    std::vector<starcraft::lang::PathObstacle> obstacles;
    if (!collect_building_obstacles(status, &unit, obstacles) ||
        !starcraft::lang::path_position_passable(
            status.pathing_map, proposed_x, proposed_y, unit.selection_width,
            unit.selection_height, obstacles)) {
      // CUnitPathCollide.cpp::sub_439030 re-evaluates live collision state as
      // a unit advances. Re-plan here so a building placed after the order was
      // issued cannot be crossed by a stale path.
      const std::uint16_t final_x = unit.movement_final_x;
      const std::uint16_t final_y = unit.movement_final_y;
      if (!plan_scv_path(status, unit, final_x, final_y)) {
        stop_unit_movement(status, unit);
      }
      changed = true;
      continue;
    }

    const ScenarioUnitPreview *collision =
        find_live_unit_collision(status, unit, proposed_x, proposed_y);
    if (collision != nullptr) {
      // CUnitPathCollide.cpp::sub_4393D0 filters the live units intersecting
      // the next footprint. sub_43A070 classifies the contacting rectangle
      // side, sub_439B90 selects a cardinal escape, and sub_43AC00 validates
      // the alternative against both terrain and units. The old bootstrap
      // used ID parity and six guessed angle offsets here. Use the recovered
      // contact-side/cardinal ordering instead.
      const starcraft::lang::UnitCollisionBody moving_body =
          collision_body_for(unit, proposed_x, proposed_y);
      const starcraft::lang::UnitCollisionBody fixed_body =
          collision_body_for(*collision, collision->x, collision->y);
      const starcraft::lang::CollisionEscapePoints escape =
          starcraft::lang::collision_escape_points(moving_body, fixed_body,
                                                   unit.direction);
      bool avoided{};
      for (std::size_t escape_index = 0; escape_index < escape.count;
           ++escape_index) {
        const starcraft::lang::CollisionEscapePoint &escape_point =
            escape.points[escape_index];
        if (escape_point.x < 0 || escape_point.y < 0 ||
            escape_point.x > UINT16_MAX || escape_point.y > UINT16_MAX ||
            !starcraft::lang::path_position_passable(
                status.pathing_map, escape_point.x, escape_point.y,
                unit.selection_width, unit.selection_height, obstacles) ||
            find_live_unit_collision(status, unit, escape_point.x,
                                     escape_point.y) != nullptr) {
          continue;
        }
        const std::array<std::int32_t, 2> candidate =
            movement_candidate(escape_point.direction);
        const std::uint16_t candidate_x =
            static_cast<std::uint16_t>(candidate[0] >> 8U);
        const std::uint16_t candidate_y =
            static_cast<std::uint16_t>(candidate[1] >> 8U);
        if ((candidate_x == unit.x && candidate_y == unit.y) ||
            !starcraft::lang::path_position_passable(
                status.pathing_map, candidate_x, candidate_y,
                unit.selection_width, unit.selection_height, obstacles) ||
            find_live_unit_collision(status, unit, candidate_x, candidate_y) !=
                nullptr) {
          continue;
        }
        const starcraft::lang::PathPoint waypoint{
            static_cast<std::uint16_t>(escape_point.x),
            static_cast<std::uint16_t>(escape_point.y),
        };
        if (unit.movement_target_x != waypoint.x ||
            unit.movement_target_y != waypoint.y) {
          const std::size_t insertion =
              (std::min)(unit.movement_path_index, unit.movement_path.size());
          unit.movement_path.insert(unit.movement_path.begin() +
                                        static_cast<std::ptrdiff_t>(insertion),
                                    waypoint);
          unit.movement_target_x = waypoint.x;
          unit.movement_target_y = waypoint.y;
        }
        proposed = candidate;
        proposed_x = candidate_x;
        proposed_y = candidate_y;
        unit.direction = escape_point.direction;
        unit.avoidance_turn = 0;
        unit.avoidance_ticks = 1;
        unit.collision_wait_ticks = 0;
        avoided = true;
        break;
      }
      if (!avoided) {
        unit.collision_wait_ticks = static_cast<std::uint8_t>(
            (std::min)(255U,
                       static_cast<unsigned>(unit.collision_wait_ticks) + 1U));
        unit.movement_speed = unit.movement_speed > acceleration
                                  ? unit.movement_speed - acceleration
                                  : 0U;
        changed = true;
        continue;
      }
    } else {
      unit.collision_wait_ticks = 0;
      if (unit.avoidance_ticks != 0U) {
        --unit.avoidance_ticks;
      } else {
        unit.avoidance_turn = 0;
      }
    }
    unit.x_fixed = proposed[0];
    unit.y_fixed = proposed[1];
    unit.x = static_cast<std::uint16_t>(unit.x_fixed >> 8U);
    unit.y = static_cast<std::uint16_t>(unit.y_fixed >> 8U);
    changed = true;
  }
  return changed;
}

bool advance_unit_actions(BootstrapStatus &status) noexcept {
  bool changed{};
  for (std::size_t worker_index = 0; worker_index < status.units.size();
       ++worker_index) {
    ScenarioUnitPreview &worker = status.units[worker_index];
    if (!worker.alive || worker.active_order == ActiveUnitOrder::none) {
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::move) {
      if (!worker.moving) {
        worker.active_order = ActiveUnitOrder::none;
      }
      continue;
    }
    if (worker.moving) {
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::protoss_build) {
      (void)complete_protoss_build_order(status, worker);
      changed = true;
      continue;
    }
    ScenarioUnitPreview *target =
        find_unit_by_id(status, worker.order_target_id);
    if (target == nullptr || target == &worker) {
      cancel_unit_order(status, worker);
      changed = true;
      continue;
    }
    const int interaction_range =
        interaction_range_for(worker, worker.active_order);
    if (unit_edge_distance(worker, *target) > interaction_range) {
      const ActiveUnitOrder order = worker.active_order;
      const std::uint32_t source_id = worker.harvest_source_id;
      if (!begin_scv_interaction(status, worker, *target, order)) {
        cancel_unit_order(status, worker);
      }
      worker.harvest_source_id = source_id;
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::attack &&
        worker.action_timer != 0) {
      --worker.action_timer;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::attack) {
      if (target->owner == worker.owner || !worker.has_ground_weapon ||
          worker.weapon_damage == 0) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      std::uint32_t damage = static_cast<std::uint32_t>(worker.weapon_damage)
                             << 8U;
      if (worker.weapon_damage_class >= 1U &&
          worker.weapon_damage_class <= 3U && target->armor_class >= 1U &&
          target->armor_class <= 3U) {
        damage = starcraft::lang::scale_damage(
            damage,
            static_cast<starcraft::lang::DamageClass>(
                worker.weapon_damage_class),
            static_cast<starcraft::lang::ArmorClass>(target->armor_class));
      }
      const std::uint32_t armor = static_cast<std::uint32_t>(target->armor)
                                  << 8U;
      damage = damage > armor ? damage - armor : 128U;
      damage = (std::max)(damage, 128U);
      target->hit_points =
          damage >= target->hit_points ? 0U : target->hit_points - damage;
      // CUnitCombat.cpp::sub_427610 dispatches action 5 for a ground target
      // and action 6 only for an air target. This runtime currently exposes
      // the ground weapon path, so action 5 is the exact attack animation
      // (notably the Zealot's psi-blade sequence).
      worker.direction = starcraft::lang::direction_from_points(
          worker.x, worker.y, target->x, target->y);
      (void)restart_unit_animation(status, worker, 5U);
      worker.action_timer = static_cast<std::uint16_t>(
          (std::max)(1U, static_cast<unsigned>(worker.weapon_cooldown) >> 1U));
      if (target->hit_points == 0) {
        if (target->addon_parent_id != 0U) {
          ScenarioUnitPreview *const parent =
              find_unit_by_id(status, target->addon_parent_id);
          if (parent != nullptr &&
              parent->attached_addon_id == target->unit_id) {
            parent->attached_addon_id = 0;
          }
        }
        if (target->attached_addon_id != 0U) {
          ScenarioUnitPreview *const addon =
              find_unit_by_id(status, target->attached_addon_id);
          if (addon != nullptr && addon->addon_parent_id == target->unit_id) {
            addon->addon_parent_id = 0;
          }
        }
        target->destroyed_by_owner = worker.owner;
        target->alive = false;
        target->selected = false;
        target->active_order = ActiveUnitOrder::none;
        target->moving = false;
        // Preserve the just-started attack animation. cancel_unit_order calls
        // stop_unit_movement, which immediately replaces it with action 12
        // and made lethal melee attacks appear to have no animation at all.
        worker.active_order = ActiveUnitOrder::none;
        worker.order_target_id = 0U;
        worker.action_phase = 0U;
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::construct) {
      if (!target->is_building || target->owner != worker.owner ||
          target->construction_complete ||
          target->construction_ticks_total == 0U) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      target->construction_builder_id = worker.unit_id;
      if (worker.action_phase == 0U) {
        // CUnitBuild.cpp::sub_422160 runs the worker's construction animation
        // while applying the target's +168 fixed-point build increment.
        (void)restart_unit_animation(status, worker, 15);
        worker.action_phase = 1;
      }
      if (target->construction_ticks_remaining != 0U) {
        --target->construction_ticks_remaining;
      }
      const std::uint32_t initial_life =
          (std::max)(1U, target->max_hit_points / 10U);
      const std::uint32_t elapsed = target->construction_ticks_total -
                                    target->construction_ticks_remaining;
      target->hit_points =
          initial_life + static_cast<std::uint32_t>(
                             static_cast<std::uint64_t>(target->max_hit_points -
                                                        initial_life) *
                             elapsed / target->construction_ticks_total);
      (void)advance_building_construction_animation(status, *target);
      if (target->construction_ticks_remaining == 0U) {
        target->hit_points = target->max_hit_points;
        target->construction_complete = true;
        target->construction_builder_id = 0;
        // CUnitInit.cpp::sub_42EBB0 dispatches animation 16 at completion.
        (void)restart_unit_animation(status, *target, 16U);
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::repair) {
      if (!target->is_building || target->owner != worker.owner ||
          target->hit_points >= target->max_hit_points) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      const std::uint32_t repair =
          (std::min)(static_cast<std::uint32_t>((
                         std::max)(1U,
                                   static_cast<unsigned>(target->repair_step))),
                     target->max_hit_points - target->hit_points);
      const std::uint64_t threshold =
          static_cast<std::uint64_t>((std::max)(1U, target->max_hit_points)) *
          3U;
      const std::uint64_t mineral_accumulator =
          worker.repair_mineral_accumulator +
          static_cast<std::uint64_t>(repair) * target->mineral_cost;
      const std::uint64_t gas_accumulator =
          worker.repair_gas_accumulator +
          static_cast<std::uint64_t>(repair) * target->gas_cost;
      const std::uint32_t mineral_charge =
          static_cast<std::uint32_t>(mineral_accumulator / threshold);
      const std::uint32_t gas_charge =
          static_cast<std::uint32_t>(gas_accumulator / threshold);
      if (mineral_charge > status.player_minerals ||
          gas_charge > status.player_gas) {
        post_resource_error(status, mineral_charge <= status.player_minerals &&
                                        gas_charge != 0U);
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      status.player_minerals -= mineral_charge;
      status.player_gas -= gas_charge;
      worker.repair_mineral_accumulator =
          static_cast<std::uint32_t>(mineral_accumulator % threshold);
      worker.repair_gas_accumulator =
          static_cast<std::uint32_t>(gas_accumulator % threshold);
      target->hit_points += repair;
      (void)restart_unit_animation(status, worker, 15);
      if (target->hit_points >= target->max_hit_points) {
        target->hit_points = target->max_hit_points;
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::gather) {
      if ((target->dat_flags & 0x2000U) == 0 || target->resource_amount == 0 ||
          worker.cargo_minerals != 0 || worker.cargo_gas != 0) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // CUnitHarvest.cpp::sub_42D3C0 uses an order timer of 125 while the
      // worker performs the harvest animation; sub_42D9C0 then consumes ten
      // resource units from the source.
      if (worker.action_phase == 0) {
        if (target->unit_type >= 176U && target->unit_type <= 178U &&
            target->harvest_queue.active_worker() != 0U &&
            target->harvest_queue.active_worker() != worker.unit_id) {
          // CUnitHarvest.cpp::sub_42CE30 does not immediately queue a worker
          // behind an occupied mineral patch. sub_43F6A0 searches a 512-pixel
          // CUnitSearches rectangle and sub_43F6C0 accepts the nearest visible,
          // accessible, non-busy mineral field. Only when that search fails
          // does sub_42C210 link the worker into the original source queue.
          const ScenarioUnitPreview *alternate{};
          std::uint64_t alternate_distance = UINT64_MAX;
          for (const ScenarioUnitPreview &candidate : status.units) {
            if (!candidate.alive || candidate.unit_id == target->unit_id ||
                candidate.unit_type < 176U || candidate.unit_type > 178U ||
                candidate.resource_amount == 0U ||
                candidate.harvest_queue.active_worker() != 0U) {
              continue;
            }
            const std::int64_t dx = static_cast<int>(candidate.x) - worker.x;
            const std::int64_t dy = static_cast<int>(candidate.y) - worker.y;
            const std::uint64_t distance =
                static_cast<std::uint64_t>(dx * dx + dy * dy);
            if (distance <= 512U * 512U && distance < alternate_distance) {
              alternate = &candidate;
              alternate_distance = distance;
            }
          }
          if (alternate != nullptr) {
            if (!begin_scv_interaction(status, worker, *alternate,
                                       ActiveUnitOrder::gather)) {
              cancel_unit_order(status, worker);
            }
            changed = true;
            continue;
          }
        }
        const starcraft::lang::HarvestAdmission admission =
            target->harvest_queue.request(worker.unit_id);
        if (admission == starcraft::lang::HarvestAdmission::queued ||
            (admission == starcraft::lang::HarvestAdmission::already_present &&
             !target->harvest_queue.is_active(worker.unit_id))) {
          // sub_42C210 links a busy worker into the resource queue. It stays
          // visible and idle until sub_42C610 promotes it from the tail. The
          // retail movement layer can let workers cross while routing, but it
          // does not leave the waiter occupying the active worker's final
          // footprint. Reuse place_unit.cpp::sub_47FBF0's recovered
          // eight-pixel perimeter search to settle that waiting position.
          worker.action_phase = 2;
          if (find_live_unit_footprint_collision(status, worker, worker.x,
                                                 worker.y) != nullptr) {
            (void)settle_created_unit(status, worker, worker.x, worker.y);
          }
          continue;
        }
      }
      if (worker.action_phase == 2) {
        if (!target->harvest_queue.is_active(worker.unit_id)) {
          continue;
        }
        worker.action_phase = 0;
      }
      if (worker.action_phase == 0) {
        worker.action_timer = 125;
        worker.action_phase = 1;
        const std::uint32_t previous_weapon_events =
            worker.iscript_state.weapon_event_count;
        (void)restart_unit_animation(status, worker, 15);
        if (worker.iscript_state.weapon_event_count != previous_weapon_events &&
            (worker.iscript_state.weapon_event == 8U ||
             worker.iscript_state.weapon_event == 42U)) {
          // Probe Working emits its weapon-42 opcode in the first tick that
          // restart_unit_animation executes. Materialize that event here;
          // the normal image-update pass handles later loop iterations.
          (void)spawn_worker_mining_effect(
              status, worker, worker.iscript_state.weapon_event);
        }
        continue;
      }
      if (worker.action_timer != 0) {
        --worker.action_timer;
        continue;
      }
      worker.action_phase = 0;
      target->resource_amount =
          target->resource_amount > 10
              ? static_cast<std::uint16_t>(target->resource_amount - 10)
              : 0;
      if (target->unit_type >= 176U && target->unit_type <= 178U) {
        worker.cargo_minerals = 8;
      } else {
        worker.cargo_gas = 8;
      }
      (void)target->harvest_queue.release(worker.unit_id);
      const ScenarioUnitPreview *nearest{};
      std::uint64_t nearest_distance = UINT64_MAX;
      for (const ScenarioUnitPreview &candidate : status.units) {
        if (!candidate.alive || candidate.owner != worker.owner ||
            (candidate.dat_flags & 0x1000U) == 0) {
          continue;
        }
        const std::int64_t dx = static_cast<int>(candidate.x) - worker.x;
        const std::int64_t dy = static_cast<int>(candidate.y) - worker.y;
        const std::uint64_t distance =
            static_cast<std::uint64_t>(dx * dx + dy * dy);
        if (distance < nearest_distance) {
          nearest_distance = distance;
          nearest = &candidate;
        }
      }
      if (nearest == nullptr ||
          !begin_scv_interaction(status, worker, *nearest,
                                 ActiveUnitOrder::return_cargo)) {
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }

    if (worker.active_order == ActiveUnitOrder::return_cargo) {
      if ((target->dat_flags & 0x1000U) == 0 || target->owner != worker.owner) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      if (worker.owner == 0U) {
        status.player_minerals += worker.cargo_minerals;
        status.player_gas += worker.cargo_gas;
        status.player_mineral_stock[0] = status.player_minerals;
        status.player_gas_stock[0] = status.player_gas;
      } else if (worker.owner < status.player_mineral_stock.size()) {
        status.player_mineral_stock[worker.owner] += worker.cargo_minerals;
        status.player_gas_stock[worker.owner] += worker.cargo_gas;
      }
      if (worker.owner < status.minerals_gathered.size()) {
        status.minerals_gathered[worker.owner] += worker.cargo_minerals;
        status.gas_gathered[worker.owner] += worker.cargo_gas;
      }
      worker.cargo_minerals = 0;
      worker.cargo_gas = 0;
      ScenarioUnitPreview *const source =
          find_unit_by_id(status, worker.harvest_source_id);
      if (source != nullptr && source->resource_amount != 0 &&
          (source->dat_flags & 0x2000U) != 0) {
        (void)begin_scv_interaction(status, worker, *source,
                                    ActiveUnitOrder::gather);
      } else {
        cancel_unit_order(status, worker);
      }
      changed = true;
    }
  }
  return changed;
}

} // namespace starcraft::recovery
