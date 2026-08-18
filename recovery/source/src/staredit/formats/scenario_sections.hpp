#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace staredit::formats {

enum class ChkStringFormat : std::uint8_t {
  classic_u16,
  extended_u32,
};

struct ChkStringEntry {
  bool present{};
  std::string value{};
};

// A semantic view of STR / STRx. String ids are one-based; id zero means no
// string. Re-serialization retains entry order so references in unimplemented
// CHK sections continue to point at the same strings.
class ChkStringTable final {
 public:
  [[nodiscard]] bool parse(const std::vector<std::uint8_t>& payload,
                           ChkStringFormat format,
                           std::string& error) noexcept;
  [[nodiscard]] bool serialize(std::vector<std::uint8_t>& payload,
                               std::string& error) const noexcept;
  [[nodiscard]] std::string_view value(std::uint32_t id) const noexcept;
  [[nodiscard]] bool find_or_append(std::string_view value,
                                    std::uint16_t& id) noexcept;
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] ChkStringFormat format() const noexcept;

 private:
  ChkStringFormat format_{ChkStringFormat::classic_u16};
  std::vector<ChkStringEntry> entries_{};
};

struct ScenarioPropertyReferences {
  std::uint16_t name_string_id{};
  std::uint16_t description_string_id{};
};

[[nodiscard]] bool parse_scenario_property_references(
    const std::vector<std::uint8_t>& payload,
    ScenarioPropertyReferences& properties) noexcept;
[[nodiscard]] bool write_scenario_property_references(
    const ScenarioPropertyReferences& properties,
    std::vector<std::uint8_t>& payload) noexcept;

constexpr std::size_t force_player_count = 8U;
constexpr std::size_t force_count = 4U;
constexpr std::uint8_t force_flag_random_start = 0x01U;
constexpr std::uint8_t force_flag_allies = 0x02U;
constexpr std::uint8_t force_flag_allied_victory = 0x04U;
constexpr std::uint8_t force_flag_shared_vision = 0x08U;

struct ForceSectionData {
  std::array<std::uint8_t, force_player_count> player_force{};
  std::array<std::uint16_t, force_count> name_string_ids{};
  std::array<std::uint8_t, force_count> flags{};
  bool supports_flags{};
};

// The recovered beta maps use the 16-byte layout (assignments + name ids).
// Retail CHK adds four flag bytes, producing the standard 20-byte layout.
[[nodiscard]] bool parse_force_section(const std::vector<std::uint8_t>& payload,
                                       ForceSectionData& forces) noexcept;
[[nodiscard]] bool write_force_section(const ForceSectionData& forces,
                                       std::vector<std::uint8_t>& payload) noexcept;

}  // namespace staredit::formats
