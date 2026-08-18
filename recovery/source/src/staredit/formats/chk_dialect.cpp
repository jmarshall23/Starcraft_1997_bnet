#include "formats/chk_dialect.hpp"

#include "starcraft/data/chk.hpp"

namespace staredit::formats {
namespace {

constexpr std::size_t kBetaUnitBytes = 14U;
constexpr std::size_t kRetailUnitBytes = 36U;
constexpr std::size_t kBetaSpriteBytes = 6U;
constexpr std::size_t kRetailSpriteBytes = 10U;
constexpr std::uint32_t kBetaDoodadTag =
    starcraft::data::chk_section_doodads;
constexpr std::uint32_t kRetailDoodadTag =
    starcraft::data::chk_section_doodads_retail;
constexpr std::uint32_t kRetailSpriteTag =
    starcraft::data::chk_section_sprites_retail;

UnitRecordLayout beta_units() noexcept {
  return {kBetaUnitBytes, 0U, 2U, 4U, 12U};
}

UnitRecordLayout retail_units() noexcept {
  return {kRetailUnitBytes, 4U, 6U, 8U, 16U};
}

}  // namespace

ChkDialectInfo detect_chk_dialect(const std::uint8_t* const version_payload,
                                  const std::size_t version_bytes,
                                  const std::size_t unit_section_bytes,
                                  const bool has_beta_sprites,
                                  const bool has_retail_sprites) noexcept {
  ChkDialectInfo result{};
  if (version_payload != nullptr && version_bytes >= 2U) {
    result.version = static_cast<std::uint16_t>(version_payload[0]) |
                     static_cast<std::uint16_t>(
                         static_cast<std::uint16_t>(version_payload[1]) << 8U);
    result.has_version = true;
    result.dialect = result.version >= 59U ? ChkDialect::retail_chk
                                           : ChkDialect::beta_chk;
  } else if (has_retail_sprites && !has_beta_sprites) {
    result.dialect = ChkDialect::retail_chk;
  } else if (has_beta_sprites && !has_retail_sprites) {
    result.dialect = ChkDialect::beta_chk;
  } else if (unit_section_bytes != 0U) {
    const bool beta_aligned = unit_section_bytes % kBetaUnitBytes == 0U;
    const bool retail_aligned = unit_section_bytes % kRetailUnitBytes == 0U;
    if (beta_aligned != retail_aligned) {
      result.dialect = beta_aligned ? ChkDialect::beta_chk
                                    : ChkDialect::retail_chk;
    }
  }

  if (result.dialect == ChkDialect::beta_chk) {
    result.units = beta_units();
    result.doodads = {kBetaDoodadTag, 6U, 0U, 2U, 4U};
    result.sprites = {starcraft::data::chk_section_sprites,
                      kBetaSpriteBytes, 0U, 2U, 4U};
  } else if (result.dialect == ChkDialect::retail_chk) {
    result.units = retail_units();
    result.doodads = {kRetailDoodadTag, 8U, 0U, 2U, 4U, 6U, 7U};
    result.sprites = {kRetailSpriteTag, kRetailSpriteBytes,
                      0U, 2U, 4U, 6U, missing_record_offset, 8U};
  }
  return result;
}

const wchar_t* chk_dialect_name(const ChkDialect dialect) noexcept {
  switch (dialect) {
    case ChkDialect::beta_chk:
      return L"beta CHK";
    case ChkDialect::retail_chk:
      return L"retail CHK";
    case ChkDialect::unknown_chk:
      return L"unknown CHK";
  }
  return L"unknown CHK";
}

}  // namespace staredit::formats
