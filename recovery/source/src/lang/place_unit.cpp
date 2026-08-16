#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <cstddef>

namespace starcraft::lang {

bool melee_unit_types(
    const std::uint8_t chk_race,
    MeleeUnitTypes& output) noexcept {
  // CHK race order is Zerg, Terran, Protoss. place_unit.cpp::sub_480B60
  // selects workers 41/7/64 and sub_480C80 selects bases 131/106/154.
  switch (chk_race) {
    case 0:
      output = {41, 131};
      return true;
    case 1:
      output = {7, 106};
      return true;
    case 2:
      output = {64, 154};
      return true;
    default:
      output = {};
      return false;
  }
}

PlacementPoint align_melee_base(
    const std::uint16_t start_x,
    const std::uint16_t start_y,
    const std::uint16_t placement_width,
    const std::uint16_t placement_height) noexcept {
  const std::uint16_t half_width = placement_width / 2U;
  const std::uint16_t half_height = placement_height / 2U;
  return {
      static_cast<std::uint16_t>(
          half_width + ((start_x - half_width) & 0xFFE0U)),
      static_cast<std::uint16_t>(
          half_height + ((start_y - half_height) & 0xFFE0U)),
  };
}

bool build_retail_melee_start_plan(
    const std::array<game::ScenarioPlayer, 12>& players,
    const std::array<game::ScenarioStartLocation, 12>& start_locations,
    const ExistingMeleeUnit* const existing_units,
    const std::size_t existing_unit_count,
    const std::array<MeleeBaseDimensions, 3>& base_dimensions,
    std::vector<MeleeStartSpawn>& output) noexcept {
  output.clear();
  if (existing_units == nullptr && existing_unit_count != 0U) {
    return false;
  }
  try {
    for (std::size_t player = 0; player < 8U; ++player) {
      if (players[player].ownership == 0U) {
        continue;
      }
      MeleeUnitTypes types{};
      const std::uint8_t race = players[player].race;
      const game::ScenarioStartLocation& start = start_locations[player];
      if (!melee_unit_types(race, types) || !start.present ||
          base_dimensions[race].width == 0U ||
          base_dimensions[race].height == 0U) {
        output.clear();
        return false;
      }
      bool has_base{};
      std::size_t worker_count{};
      for (std::size_t index = 0; index < existing_unit_count; ++index) {
        const ExistingMeleeUnit& existing = existing_units[index];
        if (!existing.alive || existing.owner != player) {
          continue;
        }
        has_base = has_base || existing.unit_type == types.base;
        worker_count += existing.unit_type == types.worker ? 1U : 0U;
      }
      if (!has_base) {
        const PlacementPoint base = align_melee_base(
            start.x,
            start.y,
            base_dimensions[race].width,
            base_dimensions[race].height);
        output.push_back({
            types.base,
            static_cast<std::uint16_t>(base.x),
            static_cast<std::uint16_t>(base.y),
            static_cast<std::uint8_t>(player),
        });
      }
      while (worker_count < retail_melee_starting_worker_count) {
        output.push_back({
            types.worker,
            start.x,
            start.y,
            static_cast<std::uint8_t>(player),
        });
        ++worker_count;
      }
    }
    return true;
  } catch (...) {
    output.clear();
    return false;
  }
}

bool find_created_unit_position(
    const int requested_x,
    const int requested_y,
    const int map_pixel_width,
    const int map_pixel_height,
    const CollisionExtents& mover,
    const CollisionExtents* const initial_obstacle,
    const PlacementPassable passable,
    void* const context,
    PlacementPoint& output) noexcept {
  if (passable == nullptr || map_pixel_width <= 0 || map_pixel_height <= 32) {
    return false;
  }
  if (passable(requested_x, requested_y, context)) {
    output = {requested_x, requested_y};
    return true;
  }

  int half_width = 8;
  int half_height = 8;
  if (initial_obstacle != nullptr) {
    half_width = (std::max)(
        8, mover.left + initial_obstacle->right + 2);
    half_height = (std::max)(
        8, mover.top + initial_obstacle->bottom + 2);
  }

  const int search_left = (std::max)(0, requested_x - 128);
  const int search_top = (std::max)(0, requested_y - 128);
  const int search_right = (std::min)(map_pixel_width - 1, requested_x + 127);
  const int search_bottom =
      (std::min)(map_pixel_height - 1, requested_y + 127);
  const auto align_down_8 = [](const int value) { return value & ~7; };
  const auto align_up_8 = [](const int value) { return (value + 7) & ~7; };
  const auto accept = [&](const int x, const int y) {
    if (x < search_left || x > search_right || y < search_top ||
        y > search_bottom || !passable(x, y, context)) {
      return false;
    }
    output = {x, y};
    return true;
  };

  while (half_width <= 128 || half_height <= 128) {
    const int left = (std::max)(
        search_left, align_down_8(requested_x - half_width));
    const int right = (std::min)(
        search_right, align_up_8(requested_x + half_width));
    const int top = (std::max)(
        search_top, align_down_8(requested_y - half_height));
    const int bottom = (std::min)(
        search_bottom, align_up_8(requested_y + half_height));

    // StarCraft.exe place_unit.cpp::sub_480040: bottom left-to-right, right
    // bottom-to-top, top right-to-left, then left top-to-bottom.  Its caller
    // sub_47FBF0 expands each failed perimeter by sixteen pixels.
    for (int x = left; x <= right; x += 8) {
      if (accept(x, bottom)) {
        return true;
      }
    }
    for (int y = bottom - 8; y >= top; y -= 8) {
      if (accept(right, y)) {
        return true;
      }
    }
    for (int x = right - 8; x >= left; x -= 8) {
      if (accept(x, top)) {
        return true;
      }
    }
    for (int y = top + 8; y < bottom; y += 8) {
      if (accept(left, y)) {
        return true;
      }
    }
    half_width += 16;
    half_height += 16;
  }
  return false;
}

}  // namespace starcraft::lang
