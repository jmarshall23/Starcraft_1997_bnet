#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::game {

inline constexpr std::uint32_t unit_status_complete = 0x00000001U;
inline constexpr std::uint32_t unit_status_excluded_from_counts = 0x40000000U;

inline constexpr std::uint8_t unit_count_exists = 0x01U;
inline constexpr std::uint8_t unit_count_complete = 0x02U;

// Partial CUnit layout for the December 1997 build. Unknown ranges remain
// opaque until callers establish their types. Array-stride operations at
// 0x00425440 and 0x004254A0 prove the complete object size is 0x128 bytes.
#pragma pack(push, 1)
struct Unit {
  std::array<std::byte, 88> opaque_000{};
  std::uint8_t owner{};  // +0x58 / +88
  std::array<std::byte, 15> opaque_059{};
  std::uint16_t unit_type{};  // +0x68 / +104
  std::array<std::byte, 46> opaque_06A{};
  std::array<std::uint16_t, 6> production_types{};  // +0x98 / +152
  std::uint8_t production_type_index{};  // +0xA4 / +164
  std::uint8_t id_generation{};  // +0xA5 / +165
  std::array<std::byte, 50> opaque_0A6{};
  std::uint32_t status_flags{};  // +0xD8 / +216
  std::array<std::byte, 24> opaque_0DC{};
  std::uint8_t count_state{};  // +0xF4 / +244
  std::array<std::byte, 51> opaque_0F5{};

  [[nodiscard]] bool is_complete() const noexcept {
    return (status_flags & unit_status_complete) != 0;
  }

  [[nodiscard]] bool is_excluded_from_counts() const noexcept {
    return (status_flags & unit_status_excluded_from_counts) != 0;
  }

  [[nodiscard]] bool is_counted_as_existing() const noexcept {
    return (count_state & unit_count_exists) != 0;
  }

  [[nodiscard]] bool is_counted_as_complete() const noexcept {
    return (count_state & unit_count_complete) != 0;
  }
};
#pragma pack(pop)

static_assert(offsetof(Unit, owner) == 0x58);
static_assert(offsetof(Unit, unit_type) == 0x68);
static_assert(offsetof(Unit, production_types) == 0x98);
static_assert(offsetof(Unit, production_type_index) == 0xA4);
static_assert(offsetof(Unit, id_generation) == 0xA5);
static_assert(offsetof(Unit, status_flags) == 0xD8);
static_assert(offsetof(Unit, count_state) == 0xF4);
static_assert(sizeof(Unit) == 0x128);

}  // namespace starcraft::game
