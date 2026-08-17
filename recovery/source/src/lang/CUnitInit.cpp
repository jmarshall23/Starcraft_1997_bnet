#include "starcraft/lang/cunit_init.hpp"

namespace starcraft::lang {

bool recover_unit_initialization(
    const data::CoreDataSet& data,
    const std::uint16_t unit_type,
    UnitInitializationData& output) noexcept {
  UnitInitializationData recovered{};
  recovered.unit_type = unit_type;
  recovered.has_selection_circle = data.unit_selection_circle(
      unit_type, recovered.selection_circle_image_id,
      recovered.selection_circle_y_offset);
  std::uint16_t left{};
  std::uint16_t top{};
  std::uint16_t right{};
  std::uint16_t bottom{};
  if (!data.unit_image_id(unit_type, recovered.image_id) ||
      !data.unit_placement_size(
          unit_type, recovered.placement_width, recovered.placement_height) ||
      !data.unit_collision_extents(unit_type, left, top, right, bottom) ||
      !data.unit_sprite_elevation(unit_type, recovered.sprite_elevation) ||
      !data.unit_is_building(unit_type, recovered.is_building) ||
      !data.unit_simulation_traits(unit_type, recovered.simulation)) {
    return false;
  }
  recovered.collision = {left, top, right, bottom};
  recovered.has_movement = data.unit_movement_traits(unit_type, recovered.movement);
  output = recovered;
  return true;
}

std::uint16_t initial_resource_amount(const std::uint16_t unit_type) noexcept {
  // CUnitInit.cpp::sub_42DEF0 initializes all three mineral fields to 1500
  // and the neutral vespene geyser field to 5000.
  if (unit_type >= 176U && unit_type <= 178U) {
    return 1500U;
  }
  return unit_type == 188U ? 5000U : 0U;
}

}  // namespace starcraft::lang
