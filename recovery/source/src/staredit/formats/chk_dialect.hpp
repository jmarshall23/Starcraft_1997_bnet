#pragma once

#include <cstddef>
#include <cstdint>

#include "formats/object_sections.hpp"

namespace staredit::formats {

enum class ChkDialect {
  beta_chk,
  retail_chk,
  unknown_chk,
};

struct UnitRecordLayout {
  std::size_t record_bytes{};
  std::size_t x_offset{};
  std::size_t y_offset{};
  std::size_t type_offset{};
  std::size_t owner_offset{};
};

struct ChkDialectInfo {
  ChkDialect dialect{ChkDialect::unknown_chk};
  std::uint16_t version{};
  bool has_version{};
  UnitRecordLayout units{};
  PlacementRecordLayout doodads{};
  PlacementRecordLayout sprites{};
};

[[nodiscard]] ChkDialectInfo detect_chk_dialect(
    const std::uint8_t* version_payload,
    std::size_t version_bytes,
    std::size_t unit_section_bytes,
    bool has_beta_sprites,
    bool has_retail_sprites) noexcept;

[[nodiscard]] const wchar_t* chk_dialect_name(ChkDialect dialect) noexcept;

}  // namespace staredit::formats
