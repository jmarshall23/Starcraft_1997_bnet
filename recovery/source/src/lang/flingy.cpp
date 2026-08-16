#include "starcraft/lang/flingy.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::lang {
namespace {

// Exact fixed-point tangent thresholds at original VA 0x004EA5C8.
constexpr std::array<std::uint32_t, 64> kDirectionThresholds{{
    7U, 13U, 19U, 26U, 32U, 38U, 45U, 51U, 58U, 65U, 71U, 78U, 85U, 92U,
    99U, 107U, 114U, 122U, 129U, 137U, 146U, 154U, 163U, 172U, 181U, 190U,
    200U, 211U, 221U, 233U, 244U, 256U, 269U, 283U, 297U, 312U, 329U, 346U,
    364U, 384U, 405U, 428U, 452U, 479U, 509U, 542U, 578U, 619U, 664U, 716U,
    775U, 844U, 926U, 1023U, 1141U, 1287U, 1476U, 1726U, 2076U, 2600U, 3471U,
    5211U, 10429U, UINT32_MAX,
}};

std::uint32_t threshold_index(const std::uint32_t ratio) noexcept {
  std::uint32_t lower{};
  std::uint32_t upper = static_cast<std::uint32_t>(kDirectionThresholds.size());
  while (lower != upper) {
    const std::uint32_t middle = (lower + upper) >> 1U;
    if (ratio <= kDirectionThresholds[middle]) {
      upper = middle;
    } else {
      lower = middle + 1;
    }
  }
  return lower;
}

}  // namespace

std::uint8_t direction_from_points(
    const std::int32_t from_x,
    const std::int32_t from_y,
    const std::int32_t to_x,
    const std::int32_t to_y) noexcept {
  const std::int32_t delta_x =
      static_cast<std::int32_t>(static_cast<std::uint32_t>(to_x) - from_x);
  const std::int32_t delta_y =
      static_cast<std::int32_t>(static_cast<std::uint32_t>(to_y) - from_y);
  if (delta_x == 0) {
    return delta_y > 0 ? 128U : 0U;
  }

  const std::int32_t numerator =
      static_cast<std::int32_t>(static_cast<std::uint32_t>(delta_y) << 8U);
  std::int32_t ratio = numerator / delta_x;
  const bool negative = ratio < 0;
  if (negative) {
    ratio = -ratio;
  }
  std::int32_t index = static_cast<std::int32_t>(threshold_index(
      static_cast<std::uint32_t>(ratio)));
  if (negative) {
    index = -index;
  }

  if (delta_x < 0) {
    const std::int32_t direction = index + 192;
    return direction == 256 ? 0U : static_cast<std::uint8_t>(direction);
  }
  return static_cast<std::uint8_t>(index + 64);
}

std::int8_t clamp_turn_delta(
    const std::uint8_t current_direction,
    const std::uint8_t desired_direction,
    const std::int32_t maximum_turn) noexcept {
  const auto delta = static_cast<std::int8_t>(desired_direction - current_direction);
  if (delta >= maximum_turn) {
    return static_cast<std::int8_t>(maximum_turn);
  }
  if (delta <= -maximum_turn) {
    return static_cast<std::int8_t>(-maximum_turn);
  }
  return delta;
}

std::uint16_t turn_distance(
    const std::uint8_t first_direction,
    const std::uint8_t second_direction) noexcept {
  const std::uint16_t clockwise = static_cast<std::uint8_t>(second_direction - first_direction);
  return clockwise > 128U ? static_cast<std::uint16_t>(256U - clockwise) : clockwise;
}

}  // namespace starcraft::lang
