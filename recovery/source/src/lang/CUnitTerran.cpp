#include "starcraft/lang/cunit_terran.hpp"

#include <array>
#include <climits>

namespace starcraft::lang {
namespace {

// The two SCV command cards in the primary executable expose these twelve
// ordinary buildings and six add-ons. Keeping the identities here removes
// the race-specific inventory from the Win32 bootstrap.
constexpr std::array<std::uint16_t, 18> kBuildableUnitTypes{{
    106U, 109U, 110U, 111U, 122U, 124U, 112U, 125U, 113U,
    114U, 116U, 123U, 107U, 108U, 115U, 117U, 118U, 120U,
}};

}  // namespace

bool is_terran_scv(const std::uint16_t unit_type) noexcept {
  return unit_type == terran_scv_type;
}

bool is_terran_ghost_type(const std::uint16_t unit_type) noexcept {
  // CUnitTerran.cpp::sub_442470 and sub_4424C0 assert this exact pair.
  return unit_type == 1U || unit_type == 16U;
}

std::uint16_t terran_addon_parent_type(
    const std::uint16_t addon_type) noexcept {
  switch (addon_type) {
    case 107U:
    case 108U:
      return 106U;
    case 115U:
    case 120U:
      return 116U;
    case 117U:
      return 113U;
    case 118U:
      return 114U;
    default:
      return UINT16_MAX;
  }
}

TerranUnitTypeView terran_buildable_unit_types() noexcept {
  return {kBuildableUnitTypes.data(), kBuildableUnitTypes.size()};
}

bool terran_addon_center(
    const std::uint16_t parent_type,
    const std::uint16_t addon_type,
    const std::uint16_t parent_x,
    const std::uint16_t parent_y,
    const std::uint16_t parent_width,
    const std::uint16_t parent_height,
    const std::uint16_t addon_width,
    const std::uint16_t addon_height,
    const std::int16_t addon_x,
    const std::int16_t addon_y,
    TerranAddonPlacement& output) noexcept {
  if (terran_addon_parent_type(addon_type) != parent_type) {
    return false;
  }
  // sub_4429A0 obtains the owning building's upper-left origin by removing
  // half its dword_8DFFB0 footprint, then applies dword_8DF730[addon] and half
  // the add-on footprint. These are the same packed DAT values, made typed.
  const int x = static_cast<int>(parent_x) - parent_width / 2 + addon_x +
                addon_width / 2;
  const int y = static_cast<int>(parent_y) - parent_height / 2 + addon_y +
                addon_height / 2;
  if (x < 0 || y < 0 || x > UINT16_MAX || y > UINT16_MAX) {
    return false;
  }
  output.center_x = static_cast<std::uint16_t>(x);
  output.center_y = static_cast<std::uint16_t>(y);
  return true;
}

}  // namespace starcraft::lang
