#include "formats/retail_chk.hpp"

#include "formats/chk_document.hpp"
#include "formats/object_sections.hpp"
#include "formats/scenario_sections.hpp"
#include "starcraft/data/chk.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace staredit::formats {
namespace {

constexpr std::uint32_t kVersionTag =
    starcraft::data::chk_fourcc('V', 'E', 'R', ' ');
constexpr std::uint32_t kRetailDoodadTag =
    starcraft::data::chk_fourcc('D', 'D', '2', ' ');
constexpr std::uint32_t kRetailSpriteTag =
    starcraft::data::chk_section_sprites_retail;
constexpr PlacementRecordLayout kBetaUnits{
    starcraft::data::chk_section_units, 14U, 4U, 0U, 2U, 12U};
constexpr PlacementRecordLayout kRetailUnits{
    starcraft::data::chk_section_units, 36U, 8U, 4U, 6U, 16U};
constexpr PlacementRecordLayout kBetaDoodads{
    starcraft::data::chk_section_doodads, 6U, 0U, 2U, 4U};
constexpr PlacementRecordLayout kRetailDoodads{
    kRetailDoodadTag, 8U, 0U, 2U, 4U, 6U, 7U};
constexpr PlacementRecordLayout kBetaSprites{
    starcraft::data::chk_section_sprites, 6U, 0U, 2U, 4U};
constexpr PlacementRecordLayout kRetailSprites{
    kRetailSpriteTag, 10U, 0U, 2U, 4U, 6U, missing_record_offset, 8U};

bool parse_section(const ChkDocument& document,
                   const PlacementRecordLayout& layout,
                   std::vector<PlacementRecord>& records) noexcept {
  const ChkSection* const section = document.section(layout.section_tag);
  if (section == nullptr) {
    records.clear();
    return true;
  }
  return parse_placement_records(section->payload, layout, records);
}

bool replace_or_append(ChkDocument& document,
                       const std::uint32_t tag,
                       std::vector<std::uint8_t> payload) noexcept {
  return document.count(tag) != 0U
             ? document.replace_section(tag, 0U, std::move(payload))
             : document.append_section(tag, std::move(payload));
}

}  // namespace

bool convert_to_retail_chk(ChkDocument& document,
                           std::string& error) noexcept {
  error.clear();
  if (!document.valid()) {
    error = "The CHK document is invalid.";
    return false;
  }
  if (document.dialect().dialect == ChkDialect::retail_chk) {
    return true;
  }
  if (document.dialect().dialect != ChkDialect::beta_chk) {
    error = "The CHK dialect cannot be converted safely.";
    return false;
  }
  try {
    std::vector<PlacementRecord> beta_units{};
    std::vector<PlacementRecord> beta_doodads{};
    std::vector<PlacementRecord> beta_sprites{};
    if (!parse_section(document, kBetaUnits, beta_units) ||
        !parse_section(document, kBetaDoodads, beta_doodads) ||
        !parse_section(document, kBetaSprites, beta_sprites)) {
      error = "A beta placement section has an invalid record width.";
      return false;
    }

    const auto convert = [](const std::vector<PlacementRecord>& source,
                            const PlacementRecordLayout& layout,
                            const bool unit) {
      std::vector<PlacementRecord> converted{};
      converted.reserve(source.size());
      for (std::size_t index = 0U; index < source.size(); ++index) {
        const PlacementRecord& input = source[index];
        PlacementRecord output = make_placement_record(
            layout, input.type, input.x, input.y, input.owner);
        output.enabled = input.enabled;
        output.flags = input.flags;
        if (unit && output.raw.size() == 36U) {
          const std::uint32_t instance = static_cast<std::uint32_t>(index + 1U);
          output.raw[0U] = static_cast<std::uint8_t>(instance);
          output.raw[1U] = static_cast<std::uint8_t>(instance >> 8U);
          output.raw[2U] = static_cast<std::uint8_t>(instance >> 16U);
          output.raw[3U] = static_cast<std::uint8_t>(instance >> 24U);
          output.raw[17U] = 100U;
          output.raw[18U] = 100U;
          output.raw[19U] = 100U;
        }
        converted.push_back(std::move(output));
      }
      return converted;
    };
    std::vector<PlacementRecord> retail_units =
        convert(beta_units, kRetailUnits, true);
    std::vector<PlacementRecord> retail_doodads =
        convert(beta_doodads, kRetailDoodads, false);
    std::vector<PlacementRecord> retail_sprites =
        convert(beta_sprites, kRetailSprites, false);
    std::vector<std::uint8_t> unit_payload{};
    std::vector<std::uint8_t> doodad_payload{};
    std::vector<std::uint8_t> sprite_payload{};
    if (!serialize_placement_records(retail_units, kRetailUnits,
                                     unit_payload) ||
        !serialize_placement_records(retail_doodads, kRetailDoodads,
                                     doodad_payload) ||
        !serialize_placement_records(retail_sprites, kRetailSprites,
                                     sprite_payload)) {
      error = "The retail placement sections could not be serialized.";
      return false;
    }

    ChkDocument converted = document;
    if (!replace_or_append(converted, starcraft::data::chk_section_units,
                           std::move(unit_payload)) ||
        !replace_or_append(converted, kRetailDoodadTag,
                           std::move(doodad_payload)) ||
        !replace_or_append(converted, kRetailSpriteTag,
                           std::move(sprite_payload))) {
      error = "The retail placement sections could not be installed.";
      return false;
    }
    (void)converted.erase_sections(starcraft::data::chk_section_doodads);
    (void)converted.erase_sections(starcraft::data::chk_section_sprites);

    if (const ChkSection* const force =
            converted.section(starcraft::data::chk_section_forces);
        force != nullptr && force->payload.size() == 16U) {
      std::vector<std::uint8_t> retail_force = force->payload;
      retail_force.resize(20U, 0U);
      if (!converted.replace_section(starcraft::data::chk_section_forces, 0U,
                                     std::move(retail_force))) {
        error = "The retail force flags could not be added.";
        return false;
      }
    }

    std::vector<std::uint8_t> version =
        converted.section(kVersionTag) == nullptr
            ? std::vector<std::uint8_t>(2U, 0U)
            : converted.section(kVersionTag)->payload;
    version.resize((std::max)(version.size(), std::size_t{2U}), 0U);
    version[0U] = 59U;
    version[1U] = 0U;
    if (!replace_or_append(converted, kVersionTag, std::move(version)) ||
        converted.dialect().dialect != ChkDialect::retail_chk) {
      error = "The retail VER section could not be installed.";
      return false;
    }
    document = std::move(converted);
    return true;
  } catch (...) {
    error = "There was not enough memory to convert the CHK to retail.";
    return false;
  }
}

}  // namespace staredit::formats
