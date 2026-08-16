#pragma once

#include "starcraft/data/dat.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <cstdint>

namespace starcraft::lang {

struct UnitInitializationData {
  std::uint16_t unit_type{};
  std::uint16_t image_id{};
  std::uint16_t placement_width{};
  std::uint16_t placement_height{};
  CollisionExtents collision{};
  data::FlingyMovementTraits movement{};
  data::UnitSimulationTraits simulation{};
  bool is_building{};
  bool has_movement{};
};

[[nodiscard]] bool recover_unit_initialization(
    const data::CoreDataSet& data,
    std::uint16_t unit_type,
    UnitInitializationData& output) noexcept;  // CUnitInit.cpp::sub_42DEF0

[[nodiscard]] std::uint16_t initial_resource_amount(
    std::uint16_t unit_type) noexcept;

}  // namespace starcraft::lang
