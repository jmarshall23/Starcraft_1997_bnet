#pragma once

#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint16_t terran_scv_type = 7U;
inline constexpr std::uint32_t terran_addon_dat_flag = 0x00000002U;

struct TerranUnitTypeView {
  const std::uint16_t* unit_types{};
  std::size_t count{};
};

struct TerranAddonPlacement {
  std::uint16_t center_x{};
  std::uint16_t center_y{};
};

[[nodiscard]] bool is_terran_scv(std::uint16_t unit_type) noexcept;
[[nodiscard]] bool is_terran_ghost_type(std::uint16_t unit_type) noexcept;
[[nodiscard]] std::uint16_t terran_addon_parent_type(
    std::uint16_t addon_type) noexcept;
[[nodiscard]] TerranUnitTypeView terran_buildable_unit_types() noexcept;

// The coordinate part of CUnitTerran.cpp::sub_4429A0 (VA 0x004429A0).
[[nodiscard]] bool terran_addon_center(
    std::uint16_t parent_type,
    std::uint16_t addon_type,
    std::uint16_t parent_x,
    std::uint16_t parent_y,
    std::uint16_t parent_width,
    std::uint16_t parent_height,
    std::uint16_t addon_width,
    std::uint16_t addon_height,
    std::int16_t addon_x,
    std::int16_t addon_y,
    TerranAddonPlacement& output) noexcept;

}  // namespace starcraft::lang
