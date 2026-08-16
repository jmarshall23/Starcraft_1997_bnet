#include "starcraft/lang/cunit_zerg.hpp"

#include <array>

namespace starcraft::lang {
namespace {

constexpr std::array<std::uint16_t, 3> kLarvaSourceTypes{{131U, 132U, 133U}};
constexpr std::array<std::uint16_t, 5> kBuildingMorphTargetTypes{{
    132U, 133U, 137U, 144U, 146U,
}};

// statcmd.cpp's recovered cards 236 and 239 expose these exact Drone morphs.
// CUnitZBuild.cpp::sub_4475E0 consumes the selected Drone for every ordinary
// entry and handles the Extractor (149) as its geyser-specific branch.
constexpr std::array<std::uint16_t, 11> kBuildableUnitTypes{{
    131U, 143U, 149U, 142U, 139U, 135U,
    134U, 141U, 138U, 140U, 136U,
}};

}  // namespace

bool is_zerg_drone(const std::uint16_t unit_type) noexcept {
  return unit_type == zerg_drone_type;
}

bool is_zerg_buildable_unit_type(const std::uint16_t unit_type) noexcept {
  for (const std::uint16_t buildable : kBuildableUnitTypes) {
    if (unit_type == buildable) {
      return true;
    }
  }
  return false;
}

bool is_zerg_town_hall(const std::uint16_t unit_type) noexcept {
  // CUnitZerg.cpp::sub_448940 and sub_4495C0 use exactly these three types.
  for (const std::uint16_t source_type : kLarvaSourceTypes) {
    if (unit_type == source_type) {
      return true;
    }
  }
  return false;
}

bool is_zerg_building_morph_target(const std::uint16_t unit_type) noexcept {
  for (const std::uint16_t target : kBuildingMorphTargetTypes) {
    if (unit_type == target) {
      return true;
    }
  }
  return false;
}

ZergLarvaSourceView zerg_larva_source_types() noexcept {
  return {kLarvaSourceTypes.data(), kLarvaSourceTypes.size()};
}

ZergUnitTypeView zerg_buildable_unit_types() noexcept {
  return {kBuildableUnitTypes.data(), kBuildableUnitTypes.size()};
}

ZergUnitTypeView zerg_building_morph_target_types() noexcept {
  return {kBuildingMorphTargetTypes.data(), kBuildingMorphTargetTypes.size()};
}

}  // namespace starcraft::lang
