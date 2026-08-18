#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace staredit::formats {

inline constexpr std::size_t missing_record_offset =
    static_cast<std::size_t>(-1);

struct PlacementRecordLayout {
  std::uint32_t section_tag{};
  std::size_t record_bytes{};
  std::size_t type_offset{};
  std::size_t x_offset{};
  std::size_t y_offset{};
  std::size_t owner_offset{missing_record_offset};
  std::size_t enabled_offset{missing_record_offset};
  std::size_t flags_offset{missing_record_offset};
};

struct PlacementRecord {
  std::uint16_t type{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint8_t owner{};
  bool enabled{true};
  std::uint16_t flags{};
  std::vector<std::uint8_t> raw{};
};

[[nodiscard]] bool parse_placement_records(
    const std::vector<std::uint8_t>& payload,
    const PlacementRecordLayout& layout,
    std::vector<PlacementRecord>& records) noexcept;

[[nodiscard]] bool serialize_placement_records(
    const std::vector<PlacementRecord>& records,
    const PlacementRecordLayout& layout,
    std::vector<std::uint8_t>& payload) noexcept;

[[nodiscard]] PlacementRecord make_placement_record(
    const PlacementRecordLayout& layout,
    std::uint16_t type,
    std::uint16_t x,
    std::uint16_t y,
    std::uint8_t owner = 0U) noexcept;

}  // namespace staredit::formats
