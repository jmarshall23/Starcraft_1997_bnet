#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace starcraft::lang {

// los.cpp::sub_46AC00/sub_46AEF0 builds these dependency-ordered cells for
// the twelve sight masks consumed by mask.cpp. The first inner_count entries
// are unconditionally visible; each remaining cell can be reached through
// either of its two nearer dependencies.
struct SightMaskCell {
  std::int8_t x{};
  std::int8_t y{};
  std::array<std::uint16_t, 2> dependencies{};
  bool inner{};
};

struct SightMask {
  std::uint8_t width{};
  std::uint8_t height{};
  std::uint16_t inner_count{};
  std::vector<SightMaskCell> cells{};
};

[[nodiscard]] const SightMask& sight_mask(std::uint8_t type) noexcept;

}  // namespace starcraft::lang
