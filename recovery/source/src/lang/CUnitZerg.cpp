#include "starcraft/lang/cunit_zerg.hpp"

#include <array>

namespace starcraft::lang {
namespace {

constexpr std::array<std::uint16_t, 3> kLarvaSourceTypes{{131U, 132U, 133U}};

}  // namespace

bool is_zerg_town_hall(const std::uint16_t unit_type) noexcept {
  // CUnitZerg.cpp::sub_448940 and sub_4495C0 use exactly these three types.
  for (const std::uint16_t source_type : kLarvaSourceTypes) {
    if (unit_type == source_type) {
      return true;
    }
  }
  return false;
}

ZergLarvaSourceView zerg_larva_source_types() noexcept {
  return {kLarvaSourceTypes.data(), kLarvaSourceTypes.size()};
}

}  // namespace starcraft::lang
