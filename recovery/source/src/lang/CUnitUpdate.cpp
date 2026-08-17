#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/game/image.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_path_collide.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
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

bool materialize_unit_overlay(
    const BootstrapStatus &status, ScenarioUnitPreview &unit,
    const starcraft::lang::IScriptState &source) noexcept {
  if (unit.asset_index >= status.unit_assets.size() ||
      source.overlay_event_count == 0U) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  if (source.overlay_image == asset.overlay_image_id && asset.overlay_ready) {
    starcraft::lang::IScriptState overlay{};
    if (!program.start(asset.overlay_iscript_id, 0U, overlay)) {
      return false;
    }
    const auto result = program.tick(overlay, 0U, 256U,
                                     &unit.iscript_state,
                                     status.scenario.tileset_id());
    if ((result != starcraft::lang::IScriptTickResult::yielded &&
         result != starcraft::lang::IScriptTickResult::sleeping) ||
        overlay.frame >= asset.overlay_frames.size()) {
      return false;
    }
    unit.overlay_iscript_state = overlay;
    unit.iscript_state.image_target_flags |= overlay.image_target_flags;
    unit.overlay_iscript_state.image_target_flags = 0U;
    unit.current_overlay_frame = overlay.frame;
    unit.overlay_ready = true;
    return true;
  }

  const auto dynamic_asset =
      std::find_if(status.unit_assets.begin(), status.unit_assets.end(),
                   [&source](const UnitRenderAsset &candidate) {
                     return candidate.image_id == source.overlay_image;
                   });
  if (dynamic_asset == status.unit_assets.end()) {
    return false;
  }
  starcraft::lang::IScriptState dynamic{};
  if (!program.start(dynamic_asset->iscript_id, 0U, dynamic)) {
    return false;
  }
  const auto result = program.tick(dynamic, 0U, 256U, &unit.iscript_state,
                                   status.scenario.tileset_id());
  if ((result != starcraft::lang::IScriptTickResult::yielded &&
       result != starcraft::lang::IScriptTickResult::sleeping) ||
      dynamic.frame >= dynamic_asset->sprite_frames.size()) {
    return false;
  }
  unit.dynamic_overlay_asset_index = static_cast<std::size_t>(
      dynamic_asset - status.unit_assets.begin());
  unit.dynamic_overlay_iscript_state = dynamic;
  unit.iscript_state.image_target_flags |= dynamic.image_target_flags;
  unit.dynamic_overlay_iscript_state.image_target_flags = 0U;
  unit.current_dynamic_overlay_frame = dynamic.frame;
  unit.dynamic_overlay_x_offset = source.overlay_x_offset;
  unit.dynamic_overlay_y_offset = source.overlay_y_offset;
  unit.dynamic_overlay_above = source.overlay_above;
  unit.dynamic_overlay_ready = true;
  return true;
}

bool restart_unit_animation(BootstrapStatus &status,
                            ScenarioUnitPreview &unit,
                            const std::uint8_t animation) noexcept {
  if (unit.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  const starcraft::lang::IScriptProgramView program{
      status.iscript_bytes.data(), status.iscript_bytes.size()};
  std::uint8_t resolved_animation = animation;
  // CImage.cpp::sub_411DF0 translates the first ground/air repeat request to
  // the matching attack-init action. The recovered order layer still invokes
  // this entry point for each weapon cycle, but the image action selected for
  // that first cycle must remain 2/3 rather than jumping straight to 5/6.
  if (animation == 5U && unit.last_animation != 5U &&
      unit.last_animation != 2U) {
    resolved_animation = 2U;
  } else if (animation == 6U && unit.last_animation != 6U &&
             unit.last_animation != 3U) {
    resolved_animation = 3U;
  }
  starcraft::lang::IScriptState next{};
  if (!program.valid() ||
      !program.start(asset.iscript_id, resolved_animation, next)) {
    return false;
  }
  const auto result =
      program.tick(next, 0, 256, nullptr, status.scenario.tileset_id());
  const bool yielded =
      result == starcraft::lang::IScriptTickResult::yielded ||
      result == starcraft::lang::IScriptTickResult::sleeping;
  const bool death_prefix =
      resolved_animation == 1U &&
      (next.overlay_event_count != 0U || next.sprite_event_count != 0U ||
       next.sound_event_count != 0U);
  if ((!yielded && !death_prefix) ||
      next.frame >= asset.sprite_frames.size()) {
    return false;
  }
  unit.iscript_state = next;
  if (next.flingy_velocity_event_count != 0U) {
    unit.movement_speed = next.flingy_velocity;
  }
  unit.current_sprite_frame = next.frame;
  // A building-death entry may attach its explosion and immediately reach an
  // opcode owned by CSprite rather than CImage. Keep one live main-image tick
  // so gameloop.cpp can advance the attached explosion and retire the unit;
  // otherwise the dying CUnit would be skipped forever.
  unit.iscript_ready = yielded || death_prefix;
  unit.last_animation = resolved_animation;
  if (next.sound_event_count != 0U) {
    (void)queue_positional_game_sound(status, next.sound_event, unit.x, unit.y);
  }
  if (next.sprite_event_count != 0U) {
    (void)spawn_iscript_sprite_effect(status, unit, next);
  }

  // CSprite.cpp::sub_41C060 walks pImageHead through every attached CImage,
  // not only the primary and dynamically materialized image. Dispatch the
  // same action to the asset's persistent attached image (turrets, flames,
  // worker layers, and similar overlays) so its death/working/idle script
  // cannot remain stuck after the primary changes state.
  if (unit.overlay_ready && asset.overlay_ready &&
      !asset.overlay_frames.empty()) {
    starcraft::lang::IScriptState overlay{};
    if (program.start(asset.overlay_iscript_id, resolved_animation, overlay)) {
      const auto overlay_result = program.tick(
          overlay, 0U, 256U, &unit.iscript_state,
          status.scenario.tileset_id());
      if ((overlay_result == starcraft::lang::IScriptTickResult::yielded ||
           overlay_result == starcraft::lang::IScriptTickResult::sleeping) &&
          overlay.frame < asset.overlay_frames.size()) {
        unit.overlay_iscript_state = overlay;
        unit.iscript_state.image_target_flags |= overlay.image_target_flags;
        unit.overlay_iscript_state.image_target_flags = 0U;
        unit.current_overlay_frame = overlay.frame;
        if (overlay.sound_event_count != 0U) {
          (void)queue_positional_game_sound(status, overlay.sound_event,
                                            unit.x, unit.y);
        }
      } else {
        unit.overlay_ready = false;
      }
    } else {
      unit.overlay_ready = false;
    }
  }

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
    if (program.start(dynamic_asset.iscript_id, resolved_animation, dynamic)) {
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
  (void)materialize_unit_overlay(status, unit, next);
  return true;
}

bool detach_unit_death_sprite(BootstrapStatus &status,
                              const ScenarioUnitPreview &unit) noexcept {
  // CUnitCombat.cpp::sub_428170 dispatches action 1 to every CImage attached
  // to the CSprite and immediately calls CUnitInit.cpp::sub_42FD60.  The
  // latter unlinks the CUnit but deliberately does not delete the CSprite:
  // the sprite and its death/explosion images continue executing IScript on
  // their own.  Keep that exact ownership split here.  A visual-only copy is
  // advanced in transient_images while the gameplay unit becomes dead now.
  try {
    ScenarioUnitPreview visual = unit;
    visual.unit_id = status.next_unit_id++;
    visual.selected = false;
    visual.moving = false;
    visual.active_order = ActiveUnitOrder::none;
    visual.order_target_id = 0U;
    visual.harvest_source_id = 0U;
    visual.transport_parent_id = 0U;
    visual.cargo_unit_ids.fill(0U);
    visual.production_active = false;
    visual.dying = false;
    visual.in_transport = false;
    visual.sprite_hidden = false;
    // The current CSprite list walk has already passed a sprite detached by
    // sub_42FD60. Mark its birth turn so CBulletRuntime starts advancing it
    // on the following simulation update rather than consuming a short death
    // script before it can ever be rendered.
    visual.action_timer = UINT16_MAX;
    visual.movement_path.clear();
    status.transient_images.push_back(std::move(visual));
    return true;
  } catch (...) {
    return false;
  }
}

bool advance_building_construction_animation(
    BootstrapStatus &status, ScenarioUnitPreview &building) noexcept {
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
  case 5: {
    // sub_423210 state five calls CUnitGUI.cpp::sub_42BA80(this, 0)
    // before action 15. Terran construction therefore hands off from the
    // units.dat construction image to the real building at three-fifths
    // life; it does not wait until the completion order.
    const auto terran_types = starcraft::lang::terran_buildable_unit_types();
    const bool terran_building =
        std::find(terran_types.unit_types,
                  terran_types.unit_types + terran_types.count,
                  building.unit_type) !=
        terran_types.unit_types + terran_types.count;
    if (const BuildableUnitVisual *const buildable =
            find_buildable_unit(status, building.unit_type);
        terran_building && buildable != nullptr &&
        buildable->asset_index != SIZE_MAX &&
        building.asset_index != buildable->asset_index) {
      (void)replace_preview_primary_image(status, building,
                                          buildable->asset_index);
    }
    (void)restart_unit_animation(status, building, 15U);
    building.construction_animation_phase = 6U;
    return true;
  }
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
      const bool active_construction_target =
          ignored_unit != nullptr &&
          (ignored_unit->active_order == ActiveUnitOrder::construct ||
           ignored_unit->active_order == ActiveUnitOrder::terran_build_exit) &&
          ignored_unit->order_target_id == unit.unit_id;
      if (&unit == ignored_unit || !unit.alive || !unit.is_building ||
          active_construction_target || unit.selection_width == 0 ||
          unit.selection_height == 0) {
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
  if (!obstacle.alive || obstacle.sprite_hidden || mover.selection_width == 0 ||
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
  if (!obstacle.alive || obstacle.sprite_hidden || mover.selection_width == 0 ||
      mover.selection_height == 0 || obstacle.selection_width == 0 ||
      obstacle.selection_height == 0) {
    return false;
  }
  return starcraft::lang::unit_collision_rects_overlap(
      collision_body_for(mover, mover_x, mover_y),
      collision_body_for(obstacle, obstacle.x, obstacle.y));
}

bool is_airborne(const ScenarioUnitPreview &unit) noexcept {
  // units.dat field 9 is byte_8DCD38. CUnitPathBuild.cpp::sub_438CC0 sends
  // elevations above the ground strata to NewStraightLinePath instead of
  // the terrain-region path builder.
  return unit.sprite_elevation > 3U;
}

const ScenarioUnitPreview *
find_live_unit_collision(const BootstrapStatus &status,
                         const ScenarioUnitPreview &mover, const int proposed_x,
                         const int proposed_y) noexcept {
  for (const ScenarioUnitPreview &obstacle : status.units) {
    const bool active_construction_target =
        (mover.active_order == ActiveUnitOrder::construct ||
         mover.active_order == ActiveUnitOrder::terran_build_exit) &&
        mover.order_target_id == obstacle.unit_id;
    if (&obstacle != &mover && obstacle.alive && !obstacle.sprite_hidden &&
        !active_construction_target &&
        mover.hangar_parent_id != obstacle.unit_id &&
        obstacle.hangar_parent_id != mover.unit_id &&
        is_airborne(obstacle) == is_airborne(mover) &&
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
    if (obstacle.unit_id != mover.unit_id && obstacle.alive &&
        !obstacle.sprite_hidden &&
        is_airborne(obstacle) == is_airborne(mover) &&
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
  const int half_width = static_cast<int>(unit.selection_width) / 2;
  const int half_height = static_cast<int>(unit.selection_height) / 2;
  const bool in_map = x >= half_width && y >= half_height &&
      x + static_cast<int>(unit.selection_width) - half_width <=
          static_cast<int>(status.pathing_map.pixel_width()) &&
      y + static_cast<int>(unit.selection_height) - half_height <=
          static_cast<int>(status.pathing_map.pixel_height());
  return in_map &&
         (is_airborne(unit) ||
          starcraft::lang::path_position_passable(
              status.pathing_map, x, y, unit.selection_width,
              unit.selection_height, no_obstacles)) &&
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

bool is_gas_refinery_type(const std::uint16_t unit_type) noexcept {
  // CUnitHarvest.cpp::sub_42C310/sub_42C340 use this exact three-type test.
  return unit_type == 110U || unit_type == 149U || unit_type == 157U;
}

bool eject_worker_from_refinery(BootstrapStatus &status,
                                ScenarioUnitPreview &worker,
                                const ScenarioUnitPreview &refinery) noexcept {
  // sub_42CA90 derives the worker-to-refinery direction with sub_405E80,
  // rotates it by -64 direction units, applies the dword_4E9DC8 vector at a
  // 32-pixel radius, and then passes that point to place_unit::sub_47FBF0.
  // The runtime uses the same direction convention and its recovered
  // eight-pixel placement search.
  const std::uint8_t radial_direction =
      starcraft::lang::direction_from_points(refinery.x, refinery.y, worker.x,
                                             worker.y);
  const std::uint8_t exit_direction =
      static_cast<std::uint8_t>(radial_direction - 64U);
  constexpr double pi = 3.14159265358979323846;
  const double angle =
      static_cast<double>(exit_direction) * (2.0 * pi / 256.0);
  const int requested_x = static_cast<int>(refinery.x) +
                          static_cast<int>(std::lround(std::sin(angle) * 32.0));
  const int requested_y = static_cast<int>(refinery.y) -
                          static_cast<int>(std::lround(std::cos(angle) * 32.0));
  worker.direction = radial_direction;
  worker.sprite_hidden = false;
  if (requested_x < 0 || requested_y < 0 || requested_x > UINT16_MAX ||
      requested_y > UINT16_MAX ||
      !settle_created_unit(status, worker,
                           static_cast<std::uint16_t>(requested_x),
                           static_cast<std::uint16_t>(requested_y))) {
    // The approach point is already outside the refinery. It is the exact
    // safe fallback when sub_47FBF0 cannot find another perimeter point.
    worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
    worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
    return false;
  }
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
          worker.unit_type != worker_type) {
        continue;
      }
      // place.cpp::sub_472F60 creates the complete four-worker melee roster at
      // the start location. Some supplied beta CHKs already contain one
      // matching worker beside a resource while the other three are supplied
      // by the retail start plan; settle every member of that opening roster
      // through the recovered created-unit perimeter search so all four begin
      // beside their base rather than preserving the CHK worker at the geyser.
      settled =
          settle_created_unit(status, worker, start.x, start.y) && settled;
    }
  }
  return settled;
}

bool plan_scv_path(BootstrapStatus &status, ScenarioUnitPreview &unit,
                   const std::uint16_t target_x,
                   const std::uint16_t target_y) noexcept {
  if (is_airborne(unit)) {
    // CUnitPathBuild.cpp::sub_438CC0 constructs a one-node
    // NewStraightLinePath for airborne flingies. It does not query terrain
    // regions, building footprints, or ground walkability.
    const int half_width = static_cast<int>(unit.selection_width) / 2;
    const int half_height = static_cast<int>(unit.selection_height) / 2;
    const int maximum_x = (std::max)(
        half_width, static_cast<int>(status.pathing_map.pixel_width()) -
                        (static_cast<int>(unit.selection_width) - half_width));
    const int maximum_y = (std::max)(
        half_height, static_cast<int>(status.pathing_map.pixel_height()) -
                         (static_cast<int>(unit.selection_height) - half_height));
    const auto x = static_cast<std::uint16_t>((std::clamp)(
        static_cast<int>(target_x), half_width, maximum_x));
    const auto y = static_cast<std::uint16_t>((std::clamp)(
        static_cast<int>(target_y), half_height, maximum_y));
    try {
      unit.movement_path.assign(1U, starcraft::lang::PathPoint{x, y});
    } catch (...) {
      return false;
    }
    unit.movement_path_index = 0U;
    unit.movement_final_x = x;
    unit.movement_final_y = y;
    unit.movement_target_x = x;
    unit.movement_target_y = y;
    unit.path_recheck_ticks = 0U;
    unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
    unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
    return true;
  }
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
  unit.path_recheck_ticks = 0U;
  unit.x_fixed = static_cast<std::int32_t>(unit.x) << 8U;
  unit.y_fixed = static_cast<std::int32_t>(unit.y) << 8U;
  return true;
}

void stop_unit_movement(BootstrapStatus &status,
                        ScenarioUnitPreview &unit) noexcept {
  unit.moving = false;
  unit.movement_speed = 0;
  unit.movement_path.clear();
  unit.movement_path_index = 0;
  unit.avoidance_ticks = 0;
  unit.collision_wait_ticks = 0;
  unit.path_recheck_ticks = 0;
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
        if (unit.sprite_hidden) {
          (void)eject_worker_from_refinery(status, unit, source);
        }
        (void)source.harvest_queue.release(unit.unit_id);
        break;
      }
    }
  }
  unit.sprite_hidden = false;
  stop_unit_movement(status, unit);
  unit.active_order = ActiveUnitOrder::none;
  unit.order_target_id = 0;
  unit.harvest_source_id = 0;
  unit.action_timer = 0;
  unit.action_phase = 0;
  unit.pending_technology = 28U;
  unit.pending_technology_x = 0U;
  unit.pending_technology_y = 0U;
}

std::size_t issue_scv_move_order(BootstrapStatus &status,
                                 const std::uint16_t target_x,
                                 const std::uint16_t target_y) noexcept {
  std::size_t issued{};
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.selected || !unit.alive || unit.owner != 0 || unit.is_building ||
        unit.unit_type == starcraft::lang::zerg_larva_type ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    if (unit.sprite_hidden) {
      cancel_unit_order(status, unit);
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
effective_unit_top_speed(const BootstrapStatus &status,
                         const ScenarioUnitPreview &unit) noexcept {
  // CImage.cpp::sub_415210 case 0x2C writes the walking animation's current
  // velocity to CFlingy+0x48; case 0x3F can replace the top speed at +0x40.
  // Iscript-controlled walkers commonly retain DAT's value 2 until action 11.
  std::uint32_t speed =
      unit.iscript_state.flingy_speed_event_count != 0U &&
              unit.iscript_state.flingy_speed != 0U
          ? unit.iscript_state.flingy_speed
          : (unit.iscript_state.flingy_velocity_event_count != 0U &&
                     unit.iscript_state.flingy_velocity != 0U
                 ? unit.iscript_state.flingy_velocity
                 : unit.movement_top_speed);
  std::uint8_t speed_upgrade{46U};
  switch (unit.unit_type) {
  case 2U: speed_upgrade = 17U; break;   // Vulture / Ion Thrusters
  case 42U: speed_upgrade = 26U; break;  // Overlord / Pneumatized Carapace
  case 37U: speed_upgrade = 27U; break;  // Zergling / Metabolic Boost
  case 38U: speed_upgrade = 29U; break;  // Hydralisk / Muscular Augments
  case 65U: speed_upgrade = 34U; break;  // Zealot / Leg Enhancements
  case 69U: speed_upgrade = 37U; break;  // Shuttle / Gravitic Drive
  case 84U: speed_upgrade = 39U; break;  // Observer / Gravitic Boosters
  case 70U: speed_upgrade = 42U; break;  // Scout / Gravitic Thrusters
  default: break;
  }
  // CUnitUpgrade::sub_446F60 sets CUnit+216 bit 0x10000000 for these exact
  // type/upgrade pairs. sub_4344C0 recomputes CUnit+64 as base + base/2.
  if (speed_upgrade < status.upgrade_levels.size() &&
      unit.owner < status.player_upgrade_levels.size() &&
      status.player_upgrade_levels[unit.owner][speed_upgrade] != 0U) {
    speed += speed >> 1U;
  }
  return speed;
}

std::uint32_t
effective_unit_weapon_range(const BootstrapStatus &status,
                            const ScenarioUnitPreview &unit) noexcept {
  std::uint8_t range_upgrade{46U};
  if (unit.unit_type == 0U) {
    range_upgrade = 16U; // Marine / U-238 Shells
  } else if (unit.unit_type == 38U) {
    range_upgrade = 30U; // Hydralisk / Grooved Spines
  } else if (unit.unit_type == 66U) {
    range_upgrade = 33U; // Dragoon / Singularity Charge
  }
  // CUnitCombat.cpp::sub_4255E0 and sub_4257B0 test exactly those three
  // player-table bytes and add 32 to weapons.dat's maximum range.
  return unit.weapon_range +
         (range_upgrade < status.upgrade_levels.size() &&
                  unit.owner < status.player_upgrade_levels.size() &&
                  status.player_upgrade_levels[unit.owner][range_upgrade] != 0U
              ? 32U
              : 0U);
}

std::uint32_t
effective_unit_weapon_cooldown(const BootstrapStatus &status,
                               const ScenarioUnitPreview &unit) noexcept {
  std::uint32_t cooldown = unit.weapon_cooldown;
  // sub_446F60's sole 0x20000000 case is upgrade 28 on type 37. The combat
  // cooldown helper treats that positive modifier as one halving.
  if (unit.unit_type == 37U &&
      unit.owner < status.player_upgrade_levels.size() &&
      status.player_upgrade_levels[unit.owner][28U] != 0U) {
    cooldown >>= 1U;
  }
  return cooldown;
}

bool unit_has_weapon_against(const ScenarioUnitPreview &unit,
                             const ScenarioUnitPreview &target) noexcept {
  // CUnitCarrier.cpp::sub_424710 selects Interceptor 73 for Carrier 72/82
  // and Scarab 85 for Reaver 83.  Those parent units intentionally have no
  // ordinary units.dat weapon: their hangar child is the attack vehicle.
  if (unit.unit_type == 72U || unit.unit_type == 82U) {
    return true;
  }
  if (unit.unit_type == 83U) {
    return !is_airborne(target);
  }
  return is_airborne(target) ? unit.has_air_weapon : unit.has_ground_weapon;
}

std::uint32_t effective_unit_weapon_range_against(
    const BootstrapStatus &status, const ScenarioUnitPreview &unit,
    const ScenarioUnitPreview &target) noexcept {
  if (unit.unit_type == 72U || unit.unit_type == 82U ||
      unit.unit_type == 83U) {
    // sub_4243A0/sub_424BA0 pass 32 * byte_8E0340[type] to the hangar target
    // search. seek_range is that CUnitInit-clamped units.dat field.
    return static_cast<std::uint32_t>(unit.seek_range) * 32U;
  }
  return is_airborne(target) ? unit.air_weapon_range
                             : effective_unit_weapon_range(status, unit);
}

ScenarioUnitPreview *find_unit_by_id(BootstrapStatus &status,
                                     const std::uint32_t unit_id) noexcept {
  const auto unit =
      std::find_if(status.units.begin(), status.units.end(),
                   [unit_id](const ScenarioUnitPreview &value) {
                     return value.unit_id == unit_id && value.alive &&
                            !value.dying;
                   });
  return unit == status.units.end() ? nullptr : &*unit;
}

const ScenarioUnitPreview *
find_unit_by_id(const BootstrapStatus &status,
                const std::uint32_t unit_id) noexcept {
  const auto unit =
      std::find_if(status.units.begin(), status.units.end(),
                   [unit_id](const ScenarioUnitPreview &value) {
                     return value.unit_id == unit_id && value.alive &&
                            !value.dying;
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

int interaction_range_for(const BootstrapStatus &status,
                          const ScenarioUnitPreview &unit,
                          const ScenarioUnitPreview &target,
                          const ActiveUnitOrder order) noexcept {
  // CUnitBuild.cpp::sub_422160 uses sub_429750(..., 5, target), while the
  // mineral approach in CUnitHarvest.cpp uses 0x18. Combat supplies its DAT
  // weapon range to that same collision-rectangle distance helper.
  if (order == ActiveUnitOrder::attack) {
    return static_cast<int>(
        effective_unit_weapon_range_against(status, unit, target));
  }
  if (order == ActiveUnitOrder::cast_technology &&
      unit.pending_technology < 28U) {
    if (unit.pending_technology == 23U) {
      return 192;
    }
    const std::uint8_t spell_order =
        technology_target_order(unit.pending_technology);
    if (spell_order < status.order_weapons.size()) {
      const std::uint8_t weapon = status.order_weapons[spell_order];
      if (weapon < status.weapon_traits.size()) {
        return static_cast<int>(status.weapon_traits[weapon].maximum_range);
      }
    }
  }
  if (order == ActiveUnitOrder::archon_warp) {
    // CUnitProtoss.cpp::sub_43C5B0 passes CUnit+72 >> 7 to the original
    // proximity helper while the reciprocal order-0x66 pair converges.
    return (std::max)(5, static_cast<int>(unit.movement_top_speed >> 7U));
  }
  if (order == ActiveUnitOrder::gather ||
      order == ActiveUnitOrder::return_cargo) {
    return 0x18;
  }
  if (order == ActiveUnitOrder::pickup_transport) {
    // CUnitTransport.cpp::sub_4439D0 tests sub_429750(this, 1, passenger)
    // while the transport is executing order 0x5B.
    return 1;
  }
  if (order == ActiveUnitOrder::return_hangar) {
    // CUnitCarrier.cpp::sub_424A20 docks a returning Interceptor once the
    // parent is within ten pixels.
    return 10;
  }
  return 5;
}

bool is_bunker_infantry(const std::uint16_t unit_type) noexcept {
  // The transport eligibility table consumed by sub_442EE0 assigns one cargo
  // space to Terran infantry. These are the ordinary and hero infantry IDs
  // represented by the licensed units.dat used by this target.
  constexpr std::array<std::uint16_t, 7> infantry{{
      0U, 1U, 16U, 20U, 23U, 32U, 34U,
  }};
  return std::find(infantry.begin(), infantry.end(), unit_type) !=
         infantry.end();
}

bool transport_accepts_unit(const BootstrapStatus &status,
                            const ScenarioUnitPreview &transport,
                            const ScenarioUnitPreview &passenger) noexcept {
  if (!transport.alive || transport.dying ||
      !transport.construction_complete || transport.owner != passenger.owner ||
      !passenger.alive || passenger.dying || passenger.is_building ||
      passenger.sprite_hidden || passenger.in_transport ||
      passenger.cargo_space_required == 0U ||
      transport.cargo_space_provided == 0U) {
    return false;
  }
  if (transport.is_building && transport.unit_type != 125U) {
    return false;
  }
  if (transport.unit_type == 125U && !is_bunker_infantry(passenger.unit_type)) {
    return false;
  }
  // sub_442EE0 rejects an Overlord until Ventral Sacs (upgrade 24) is set.
  if (transport.unit_type == 42U &&
      (transport.owner >= status.player_upgrade_levels.size() ||
       status.player_upgrade_levels[transport.owner][24U] == 0U)) {
    return false;
  }
  std::size_t used{};
  bool has_slot{};
  for (const std::uint32_t cargo_id : transport.cargo_unit_ids) {
    has_slot = has_slot || cargo_id == 0U;
    const ScenarioUnitPreview *const cargo = find_unit_by_id(status, cargo_id);
    if (cargo != nullptr) {
      used += cargo->cargo_space_required;
    }
  }
  // CUnitTransport.cpp::sub_442EE0 reads byte_8DE550 (units.dat field 45)
  // for capacity and byte_8DDC60 (field 44) for each occupant's size.
  return has_slot && used + passenger.cargo_space_required <=
                         transport.cargo_space_provided;
}

std::size_t unload_transport_units(BootstrapStatus &status,
                                   ScenarioUnitPreview &transport) noexcept {
  if (!transport.alive || transport.cargo_space_provided == 0U) {
    return 0U;
  }
  std::size_t unloaded{};
  for (std::uint32_t &cargo_id : transport.cargo_unit_ids) {
    ScenarioUnitPreview *const cargo = find_unit_by_id(status, cargo_id);
    if (cargo == nullptr) {
      continue;
    }
    // sub_443850 stops at the first occupied cargo slot whose sub_4434C0
    // placement attempt fails; later occupants remain inside the transport.
    if (!settle_created_unit(status, *cargo, transport.x, transport.y)) {
      break;
    }
    cargo->transport_parent_id = 0U;
    cargo->in_transport = false;
    cargo->sprite_hidden = false;
    cargo->active_order = ActiveUnitOrder::none;
    cargo->order_target_id = 0U;
    stop_unit_movement(status, *cargo);
    cargo_id = 0U;
    ++unloaded;
  }
  return unloaded;
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
  const int interaction_range =
      interaction_range_for(status, worker, target, order);
  if (worker.sprite_hidden) {
    cancel_unit_order(status, worker);
  }
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
    if (!unit.alive || unit.sprite_hidden || unit.selected) {
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

  std::uint8_t technology{28U};
  if (order == 117U) technology = 2U;
  else if (order == 136U) technology = 6U;
  else if (order == 138U) technology = 7U;
  else if (order == 109U) technology = 8U;
  else if (order == 137U) technology = 22U;
  else if (order == 143U) technology = 23U;
  else if (order == 132U) technology = 24U;
  else if (order == 142U) technology = 25U;
  if (technology != 28U) {
    ScenarioUnitPreview *const spell_target =
        hit == nullptr ? nullptr : find_unit_by_id(status, hit->unit_id);
    if (spell_target == nullptr && technology != 22U && technology != 24U &&
        technology != 25U) {
      return 0U;
    }
    std::array<std::uint32_t, 12> caster_ids{};
    std::size_t caster_count{};
    for (const ScenarioUnitPreview &candidate : status.units) {
      if (candidate.selected && candidate.alive && candidate.owner == 0U &&
          caster_count < caster_ids.size()) {
        caster_ids[caster_count++] = candidate.unit_id;
      }
    }
    const std::uint32_t spell_target_id =
        spell_target == nullptr ? 0U : spell_target->unit_id;
    std::size_t cast{};
    for (std::size_t index = 0U; index < caster_count; ++index) {
      ScenarioUnitPreview *const caster =
          find_unit_by_id(status, caster_ids[index]);
      ScenarioUnitPreview *const current_target =
          find_unit_by_id(status, spell_target_id);
      if (caster == nullptr) {
        continue;
      }
      const bool succeeded =
          current_target != nullptr
              ? cast_unit_technology(status, *caster, *current_target,
                                     technology)
              : cast_unit_technology_at(status, *caster, target_x, target_y,
                                        technology);
      cast += succeeded ? 1U : 0U;
    }
    return cast;
  }

  if (hit == nullptr || order == 7 || order == 15 || order == 0x32 ||
      order == 0x92U) {
    return issue_scv_move_order(status, target_x, target_y);
  }

  if (order == 0x5BU) {
    ScenarioUnitPreview *const passenger = find_unit_by_id(status, hit->unit_id);
    if (passenger == nullptr) {
      return 0U;
    }
    for (ScenarioUnitPreview &transport : status.units) {
      if (!transport.selected || transport.owner != 0U ||
          !transport_accepts_unit(status, transport, *passenger)) {
        continue;
      }
      status.last_issued_order = 0x5BU;
      // sub_4439D0 is the selected transport's order handler.  It moves the
      // transport to the passenger and only then dispatches passenger order
      // 0x59 (handled below as enter_transport).
      return begin_scv_interaction(status, transport, *passenger,
                                   ActiveUnitOrder::pickup_transport)
                 ? 1U
                 : 0U;
    }
    return 0U;
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
        worker.is_building ||
        worker.unit_type == starcraft::lang::zerg_larva_type ||
        worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (active_order == ActiveUnitOrder::attack &&
         !unit_has_weapon_against(worker, *hit))) {
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
    if (!unit.alive || unit.sprite_hidden || unit.selected) {
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
  } else if (hit->owner != 0U && hit->owner != 11U && !hit->cloaked) {
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
  } else if (hit->owner == 0U && hit->cargo_space_provided != 0U) {
    order = ActiveUnitOrder::enter_transport;
    status.last_issued_order = 89U;
  }
  if (order == ActiveUnitOrder::none) {
    status.last_issued_order = 7U;
    return issue_scv_move_order(status, hit->x, hit->y);
  }

  std::size_t issued{};
  for (ScenarioUnitPreview &worker : status.units) {
    const bool worker_only_order = order != ActiveUnitOrder::attack &&
                                   order != ActiveUnitOrder::enter_transport;
    if (!worker.alive || !worker.selected || worker.owner != 0U ||
        worker.is_building ||
        worker.unit_type == starcraft::lang::zerg_larva_type ||
        worker.movement_top_speed == 0U ||
        (worker_only_order && (worker.dat_flags & 0x08U) == 0U) ||
        (order == ActiveUnitOrder::attack &&
         !unit_has_weapon_against(worker, *hit)) ||
        (order == ActiveUnitOrder::enter_transport &&
         !transport_accepts_unit(status, *hit, worker))) {
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
        unit.stasis_ticks != 0U ||
        unit.movement_top_speed == 0U || unit.movement_acceleration == 0U) {
      continue;
    }
    const std::uint32_t top_speed = effective_unit_top_speed(status, unit);
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
    const bool airborne = is_airborne(unit);
    const auto position_passable = [&](const int x, const int y) noexcept {
      return airborne || starcraft::lang::path_position_passable(
          status.pathing_map, x, y, unit.selection_width,
          unit.selection_height, obstacles);
    };
    if ((!airborne && !collect_building_obstacles(status, &unit, obstacles)) ||
        !position_passable(proposed_x, proposed_y)) {
      // CUnitPathCollide.cpp::sub_439030 re-evaluates live collision state as
      // a unit advances. The recovered path object does not rebuild here on
      // every frame: sai_path.cpp::sub_4926E0 counts down path byte +17 and
      // CUnitPath.cpp reloads it with 25 before another blocked-path search.
      // Let the flingy turn and decelerate during that interval; this also
      // avoids repeatedly rebuilding an otherwise valid path at a corner.
      unit.movement_speed = unit.movement_speed > acceleration
                                ? unit.movement_speed - acceleration
                                : 0U;
      if (unit.path_recheck_ticks == 0U) {
        unit.path_recheck_ticks = 25U;
        changed = true;
        continue;
      }
      --unit.path_recheck_ticks;
      if (unit.path_recheck_ticks != 0U) {
        changed = true;
        continue;
      }
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
            !position_passable(escape_point.x, escape_point.y) ||
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
            !position_passable(candidate_x, candidate_y) ||
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

void apply_fixed_unit_damage(ScenarioUnitPreview &target,
                             std::uint32_t damage) noexcept {
  const std::uint32_t matrix = target.defensive_matrix_points;
  if (matrix != 0U) {
    const std::uint32_t absorbed = (std::min)(damage, matrix);
    target.defensive_matrix_points =
        static_cast<std::uint16_t>(matrix - absorbed);
    damage -= absorbed;
  }
  if (damage != 0U && target.shield_points != 0U) {
    const std::uint32_t absorbed = (std::min)(damage, target.shield_points);
    target.shield_points -= absorbed;
    damage -= absorbed;
  }
  if (damage != 0U) {
    target.hit_points =
        damage >= target.hit_points ? 0U : target.hit_points - damage;
  }
}

void destroy_unit(BootstrapStatus &status, ScenarioUnitPreview &target,
                  const std::uint8_t attacking_owner) noexcept {
  if (!target.alive || target.dying) {
    return;
  }
  if (target.hangar_parent_id != 0U) {
    ScenarioUnitPreview *const parent =
        find_unit_by_id(status, target.hangar_parent_id);
    if (parent != nullptr) {
      auto slot = std::find(parent->hangar_unit_ids.begin(),
                            parent->hangar_unit_ids.end(), target.unit_id);
      if (slot != parent->hangar_unit_ids.end()) {
        *slot = 0U;
      }
    }
    target.hangar_parent_id = 0U;
  }
  if (target.addon_parent_id != 0U) {
    ScenarioUnitPreview *const parent =
        find_unit_by_id(status, target.addon_parent_id);
    if (parent != nullptr && parent->attached_addon_id == target.unit_id) {
      parent->attached_addon_id = 0U;
    }
  }
  if (target.attached_addon_id != 0U) {
    ScenarioUnitPreview *const addon =
        find_unit_by_id(status, target.attached_addon_id);
    if (addon != nullptr && addon->addon_parent_id == target.unit_id) {
      addon->addon_parent_id = 0U;
    }
  }
  target.destroyed_by_owner = attacking_owner;
  target.selected = false;
  target.active_order = ActiveUnitOrder::none;
  target.moving = false;
  target.dying = true;
  target.sprite_hidden = false;
  (void)restart_unit_animation(status, target, 1U);
  (void)detach_unit_death_sprite(status, target);
  for (const std::uint32_t cargo_id : target.cargo_unit_ids) {
    ScenarioUnitPreview *const cargo = find_unit_by_id(status, cargo_id);
    if (cargo != nullptr) {
      cargo->destroyed_by_owner = attacking_owner;
      cargo->hit_points = 0U;
      cargo->alive = false;
      cargo->in_transport = false;
      cargo->transport_parent_id = 0U;
    }
  }
  target.cargo_unit_ids.fill(0U);
  for (const std::uint32_t fighter_id : target.hangar_unit_ids) {
    ScenarioUnitPreview *const fighter = find_unit_by_id(status, fighter_id);
    if (fighter != nullptr) {
      fighter->destroyed_by_owner = attacking_owner;
      fighter->hit_points = 0U;
      fighter->alive = false;
      fighter->hangar_parent_id = 0U;
    }
  }
  target.hangar_unit_ids.fill(0U);
  target.alive = false;
  target.dying = false;
  target.iscript_ready = false;
  target.dynamic_overlay_ready = false;
  target.overlay_ready = false;
}

bool advance_unit_actions(BootstrapStatus &status) noexcept {
  bool changed{};
  for (std::size_t worker_index = 0; worker_index < status.units.size();
       ++worker_index) {
    ScenarioUnitPreview &worker = status.units[worker_index];
    if (!worker.alive) {
      continue;
    }
    if (worker.archon_merging) {
      // CUnitProtoss.cpp::sub_43C7C0 keeps order 0x67 active while the
      // transformed Archon's build timer counts down.  Action 13 is the
      // merge/lightning IScript; only its completion returns the image to
      // normal init/idle and makes the unit complete.
      if (worker.construction_ticks_remaining != 0U) {
        --worker.construction_ticks_remaining;
      }
      if (worker.construction_ticks_remaining == 0U) {
        worker.archon_merging = false;
        worker.construction_complete = true;
        worker.hit_points = worker.max_hit_points;
        worker.shield_points = worker.max_shield_points;
        (void)restart_unit_animation(status, worker, 0U);
      }
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::none) {
      continue;
    }
    if (worker.stasis_ticks != 0U) {
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::move) {
      if (!worker.moving) {
        worker.active_order = ActiveUnitOrder::none;
      }
      continue;
    }
    if (worker.moving &&
        worker.active_order != ActiveUnitOrder::construct) {
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::terran_build_exit) {
      cancel_unit_order(status, worker);
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::terran_build) {
      (void)complete_terran_build_order(status, worker);
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::protoss_build) {
      (void)complete_protoss_build_order(status, worker);
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::zerg_build) {
      (void)complete_zerg_build_order(status, worker);
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::cast_technology_position) {
      const std::uint8_t technology = worker.pending_technology;
      const std::uint16_t target_x = worker.pending_technology_x;
      const std::uint16_t target_y = worker.pending_technology_y;
      if (technology >= 28U ||
          !cast_unit_technology_at(status, worker, target_x, target_y,
                                   technology)) {
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }
    ScenarioUnitPreview *target =
        find_unit_by_id(status, worker.order_target_id);
    if (target == nullptr || target == &worker) {
      if (worker.hangar_launched && worker.unit_type == 73U) {
        ScenarioUnitPreview *const parent =
            find_unit_by_id(status, worker.hangar_parent_id);
        if (parent != nullptr &&
            begin_scv_interaction(status, worker, *parent,
                                  ActiveUnitOrder::return_hangar)) {
          changed = true;
          continue;
        }
      }
      if (worker.hangar_launched && worker.unit_type == 85U) {
        // A Scarab is consumed once launched; if its focus dies before impact
        // it does not become a reusable docked hangar unit.
        ScenarioUnitPreview *const parent =
            find_unit_by_id(status, worker.hangar_parent_id);
        if (parent != nullptr) {
          auto slot = std::find(parent->hangar_unit_ids.begin(),
                                parent->hangar_unit_ids.end(), worker.unit_id);
          if (slot != parent->hangar_unit_ids.end()) {
            *slot = 0U;
          }
        }
        worker.alive = false;
        worker.sprite_hidden = true;
        worker.hangar_parent_id = 0U;
        changed = true;
        continue;
      }
      cancel_unit_order(status, worker);
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::construct) {
      (void)advance_terran_construction_order(status, worker, *target);
      changed = true;
      continue;
    }
    const int interaction_range = interaction_range_for(
        status, worker, *target, worker.active_order);
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
    if (worker.active_order == ActiveUnitOrder::pickup_transport) {
      if (!transport_accepts_unit(status, worker, *target)) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // Once order 0x5B reaches range one, sub_4439D0 stops the transport and
      // sends order 0x59 to the passenger.  sub_443730 then performs the
      // passenger's final approach and sub_443080 enters the cargo hold.
      stop_unit_movement(status, worker);
      worker.active_order = ActiveUnitOrder::none;
      worker.order_target_id = 0U;
      if (target->active_order != ActiveUnitOrder::enter_transport ||
          target->order_target_id != worker.unit_id) {
        (void)begin_scv_interaction(status, *target, worker,
                                    ActiveUnitOrder::enter_transport);
      }
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::enter_transport) {
      if (!transport_accepts_unit(status, *target, worker)) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      auto slot = std::find(target->cargo_unit_ids.begin(),
                            target->cargo_unit_ids.end(), 0U);
      if (slot == target->cargo_unit_ids.end()) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // CUnitTransport.cpp::sub_443080 writes the compact CUnit ID into the
      // first free cargo slot, sets in-transport bit 0x20, removes collision,
      // and dispatches action 12 before hiding the sprite.
      stop_unit_movement(status, worker);
      *slot = worker.unit_id;
      worker.transport_parent_id = target->unit_id;
      worker.in_transport = true;
      worker.sprite_hidden = true;
      worker.selected = false;
      worker.active_order = ActiveUnitOrder::none;
      worker.order_target_id = 0U;
      worker.action_phase = 0U;
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::return_hangar) {
      // sub_424A20 calls sub_423D50 at range ten.  The child remains a live
      // CUnit linked in the parent's hangar, but its sprite and collision are
      // removed until the next release.
      stop_unit_movement(status, worker);
      worker.x = target->x;
      worker.y = target->y;
      worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
      worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
      worker.sprite_hidden = true;
      worker.hangar_launched = false;
      worker.active_order = ActiveUnitOrder::none;
      worker.order_target_id = 0U;
      worker.action_phase = 0U;
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::cast_technology) {
      const std::uint8_t technology = worker.pending_technology;
      if (technology >= 28U ||
          !cast_unit_technology(status, worker, *target, technology)) {
        cancel_unit_order(status, worker);
      }
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::archon_warp) {
      if (target->active_order != ActiveUnitOrder::archon_warp ||
          target->order_target_id != worker.unit_id ||
          worker.unit_type != 67U || target->unit_type != 67U) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // sub_43C5B0 removes the partner, changes the surviving CUnit to type
      // 68, dispatches animation 13, and advances it to the post-warp order.
      stop_unit_movement(status, worker);
      stop_unit_movement(status, *target);
      target->alive = false;
      target->selected = false;
      target->active_order = ActiveUnitOrder::none;
      target->order_target_id = 0U;
      if (!configure_preview_type(status, worker, 68U)) {
        worker.alive = false;
      } else {
        worker.active_order = ActiveUnitOrder::none;
        worker.order_target_id = 0U;
        const std::uint16_t build_time =
            status.runtime_unit_types[68U]
                .initialization.simulation.build_time;
        worker.construction_ticks_total =
            starcraft::lang::production_total_ticks(build_time);
        worker.construction_ticks_remaining =
            worker.construction_ticks_total;
        worker.construction_complete =
            worker.construction_ticks_total == 0U;
        worker.archon_merging = !worker.construction_complete;
        (void)restart_unit_animation(status, worker, 13U);
      }
      changed = true;
      continue;
    }
    if (worker.active_order == ActiveUnitOrder::attack) {
      const bool air_target = is_airborne(*target);
      const bool has_weapon = unit_has_weapon_against(worker, *target);
      if (target->dying || target->owner == worker.owner || target->cloaked ||
          !has_weapon) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }

      if (worker.unit_type == 72U || worker.unit_type == 82U ||
          worker.unit_type == 83U) {
        // sub_4243A0 and sub_424BA0 remove exactly one child from the docked
        // hangar list for each launch cycle.  Interceptors remain linked and
        // return through order 0x19; Scarabs are consumed on impact.
        ScenarioUnitPreview *fighter{};
        for (const std::uint32_t fighter_id : worker.hangar_unit_ids) {
          ScenarioUnitPreview *const candidate =
              find_unit_by_id(status, fighter_id);
          if (candidate != nullptr && candidate->sprite_hidden &&
              !candidate->hangar_launched) {
            fighter = candidate;
            break;
          }
        }
        worker.direction = starcraft::lang::direction_from_points(
            worker.x, worker.y, target->x, target->y);
        worker.action_timer = 12U;
        if (fighter != nullptr) {
          fighter->x = worker.x;
          fighter->y = worker.y;
          fighter->x_fixed = static_cast<std::int32_t>(fighter->x) << 8U;
          fighter->y_fixed = static_cast<std::int32_t>(fighter->y) << 8U;
          fighter->direction = worker.direction;
          if (fighter->unit_type == 73U && worker.sprite_elevation != 0U) {
            fighter->sprite_elevation = static_cast<std::uint8_t>(
                worker.sprite_elevation - 1U);
          }
          fighter->sprite_hidden = false;
          fighter->hangar_launched = true;
          if (!begin_scv_interaction(status, *fighter, *target,
                                     ActiveUnitOrder::attack)) {
            fighter->sprite_hidden = true;
            fighter->hangar_launched = false;
            fighter->x = worker.x;
            fighter->y = worker.y;
            fighter->x_fixed = static_cast<std::int32_t>(fighter->x) << 8U;
            fighter->y_fixed = static_cast<std::int32_t>(fighter->y) << 8U;
          }
        }
        changed = true;
        continue;
      }

      const std::uint16_t weapon_damage =
          air_target ? worker.air_weapon_damage : worker.weapon_damage;
      const std::uint16_t weapon_damage_factor =
          air_target ? worker.air_weapon_damage_factor
                     : worker.weapon_damage_factor;
      const std::uint8_t weapon_damage_class =
          air_target ? worker.air_weapon_damage_class
                     : worker.weapon_damage_class;
      const std::uint8_t weapon_upgrade =
          air_target ? worker.air_weapon_upgrade : worker.weapon_upgrade;
      const std::uint8_t weapon_cooldown =
          air_target ? worker.air_weapon_cooldown : worker.weapon_cooldown;
      if (weapon_damage == 0U) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      const std::uint32_t weapon_level =
          worker.owner < status.player_upgrade_levels.size() &&
                  weapon_upgrade < status.upgrade_levels.size()
              ? status.player_upgrade_levels[worker.owner]
                                            [weapon_upgrade]
              : 0U;
      std::uint32_t damage =
          (static_cast<std::uint32_t>(weapon_damage) +
           weapon_level * weapon_damage_factor)
          << 8U;
      if (weapon_damage_class >= 1U && weapon_damage_class <= 3U &&
          target->armor_class >= 1U &&
          target->armor_class <= 3U) {
        damage = starcraft::lang::scale_damage(
            damage,
            static_cast<starcraft::lang::DamageClass>(
                weapon_damage_class),
            static_cast<starcraft::lang::ArmorClass>(target->armor_class));
      }
      const std::uint32_t armor_level =
          target->owner < status.player_upgrade_levels.size() &&
                  target->armor_upgrade < status.upgrade_levels.size()
              ? status.player_upgrade_levels[target->owner]
                                            [target->armor_upgrade]
              : 0U;
      const std::uint32_t armor =
          (static_cast<std::uint32_t>(target->armor) + armor_level) << 8U;
      damage = damage > armor ? damage - armor : 128U;
      damage = (std::max)(damage, 128U);
      // CUnitCombat.cpp::sub_427610 dispatches distinct ground/air attack
      // actions, including the Battlecruiser's air-weapon script.
      worker.direction = starcraft::lang::direction_from_points(
          worker.x, worker.y, target->x, target->y);
      (void)restart_unit_animation(status, worker, air_target ? 6U : 5U);

      const std::uint8_t weapon_type =
          air_target ? worker.air_weapon : worker.ground_weapon;
      bool projectile_fired{};
      if (weapon_type < status.weapon_traits.size() &&
          status.weapon_traits[weapon_type].has_projectile_graphic) {
        const std::uint8_t projectile_count =
            status.weapon_traits[weapon_type].projectile_count;
        for (std::uint8_t projectile = 0U; projectile < projectile_count;
             ++projectile) {
          projectile_fired =
              spawn_weapon_projectile(status, worker, *target, weapon_type,
                                      worker.hallucination ? 0U : damage) ||
              projectile_fired;
        }
      }
      if (!projectile_fired && !worker.hallucination) {
        apply_fixed_unit_damage(*target, damage);
      }

      const std::uint32_t effective_cooldown = air_target
                                                   ? weapon_cooldown
                                                   : effective_unit_weapon_cooldown(
                                                         status, worker);
      worker.action_timer = static_cast<std::uint16_t>(
          (std::max)(1U, effective_cooldown >> 1U));
      if (worker.unit_type == 85U) {
        // Scarab order sub_424CA0 reaches its focus, dispatches action 13,
        // and waits for the IScript signal that consumes the Scarab. Keep a
        // visual-only action-13 sprite while removing the hangar CUnit now.
        (void)restart_unit_animation(status, worker, 13U);
        (void)detach_unit_death_sprite(status, worker);
        ScenarioUnitPreview *const parent =
            find_unit_by_id(status, worker.hangar_parent_id);
        if (parent != nullptr) {
          auto slot = std::find(parent->hangar_unit_ids.begin(),
                                parent->hangar_unit_ids.end(), worker.unit_id);
          if (slot != parent->hangar_unit_ids.end()) {
            *slot = 0U;
          }
        }
        worker.alive = false;
        worker.sprite_hidden = true;
        worker.hangar_parent_id = 0U;
      }
      if (target->hit_points == 0) {
        destroy_unit(status, *target, worker.owner);
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
      const bool gas_refinery = is_gas_refinery_type(target->unit_type);
      if ((target->dat_flags & 0x2000U) == 0 || target->resource_amount == 0 ||
          worker.cargo_minerals != 0 || worker.cargo_gas != 0 ||
          (gas_refinery &&
           (!target->is_building || !target->construction_complete ||
            target->owner != worker.owner))) {
        cancel_unit_order(status, worker);
        changed = true;
        continue;
      }
      // CUnitHarvest.cpp::sub_42D3C0 uses an order timer of 125 while the
      // worker performs the harvest animation; sub_42D9C0 then consumes ten
      // resource units from the source.
      if (worker.action_phase == 0) {
        if (!gas_refinery && target->unit_type >= 176U &&
            target->unit_type <= 178U &&
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
        if (gas_refinery) {
          // WaitForGas (0x50) promotes one worker through sub_42C520. The
          // HarvestGas handler (0x51/sub_42CA90) then hides the CSprite and
          // waits 62 order ticks inside the refinery. No mineral animation or
          // mining weapon is played while the sprite is hidden.
          worker.sprite_hidden = true;
          worker.action_timer = 62U;
          worker.action_phase = 3U;
          changed = true;
          continue;
        }
      }
      if (worker.action_phase == 2) {
        if (!target->harvest_queue.is_active(worker.unit_id)) {
          continue;
        }
        worker.action_phase = 0;
      }
      if (worker.action_phase == 3U) {
        if (!gas_refinery) {
          cancel_unit_order(status, worker);
          changed = true;
          continue;
        }
        if (worker.action_timer != 0U) {
          --worker.action_timer;
          // CTarget.cpp::sub_41DAE0 decrements +92 before the order switch.
          // Therefore HarvestGas observes zero and ejects on the 62nd
          // update after sub_42CA90 stores 62, not one update later.
          if (worker.action_timer != 0U) {
            continue;
          }
        }
        target->resource_amount =
            target->resource_amount > 10U
                ? static_cast<std::uint16_t>(target->resource_amount - 10U)
                : 0U;
        worker.cargo_gas = 8U;
        (void)eject_worker_from_refinery(status, worker, *target);
        (void)target->harvest_queue.release(worker.unit_id);
        const ScenarioUnitPreview *nearest{};
        std::uint64_t nearest_distance = UINT64_MAX;
        for (const ScenarioUnitPreview &candidate : status.units) {
          if (!candidate.alive || candidate.owner != worker.owner ||
              (candidate.dat_flags & 0x1000U) == 0U) {
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
