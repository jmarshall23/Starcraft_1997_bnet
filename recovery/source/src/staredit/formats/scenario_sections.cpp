#include "formats/scenario_sections.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <utility>

namespace staredit::formats {
namespace {

std::uint16_t read_u16(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint16_t>(bytes[0]) |
         static_cast<std::uint16_t>(
             static_cast<std::uint16_t>(bytes[1]) << 8U);
}

std::uint32_t read_u32(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

void write_u16(std::vector<std::uint8_t>& bytes,
               const std::size_t offset,
               const std::uint16_t value) noexcept {
  bytes[offset] = static_cast<std::uint8_t>(value);
  bytes[offset + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

void write_u32(std::vector<std::uint8_t>& bytes,
               const std::size_t offset,
               const std::uint32_t value) noexcept {
  bytes[offset] = static_cast<std::uint8_t>(value);
  bytes[offset + 1U] = static_cast<std::uint8_t>(value >> 8U);
  bytes[offset + 2U] = static_cast<std::uint8_t>(value >> 16U);
  bytes[offset + 3U] = static_cast<std::uint8_t>(value >> 24U);
}

}  // namespace

bool ChkStringTable::parse(const std::vector<std::uint8_t>& payload,
                           const ChkStringFormat format,
                           std::string& error) noexcept {
  entries_.clear();
  format_ = format;
  error.clear();
  const std::size_t field_bytes =
      format == ChkStringFormat::classic_u16 ? 2U : 4U;
  if (payload.size() < field_bytes) {
    error = "The string table header is truncated.";
    return false;
  }
  const std::uint32_t count =
      format == ChkStringFormat::classic_u16
          ? read_u16(payload.data())
          : read_u32(payload.data());
  if (count > (std::numeric_limits<std::size_t>::max)() / field_bytes - 1U) {
    error = "The string table entry count is too large.";
    return false;
  }
  const std::size_t header_bytes = field_bytes * (1U + count);
  if (header_bytes > payload.size()) {
    error = "The string table offset array is truncated.";
    return false;
  }
  try {
    entries_.reserve(count);
    for (std::size_t index = 0U; index < count; ++index) {
      const std::size_t offset_at = field_bytes * (index + 1U);
      const std::uint32_t string_offset =
          format == ChkStringFormat::classic_u16
              ? read_u16(payload.data() + offset_at)
              : read_u32(payload.data() + offset_at);
      if (string_offset == 0U) {
        entries_.push_back({false, {}});
        continue;
      }
      if (string_offset < header_bytes || string_offset >= payload.size()) {
        error = "A string table offset is outside the string data.";
        entries_.clear();
        return false;
      }
      const auto begin = payload.begin() + string_offset;
      const auto end = std::find(begin, payload.end(), std::uint8_t{0U});
      if (end == payload.end()) {
        error = "A string table entry is not NUL terminated.";
        entries_.clear();
        return false;
      }
      entries_.push_back(
          {true, std::string(begin, end)});
    }
    return true;
  } catch (...) {
    entries_.clear();
    error = "There was not enough memory to parse the string table.";
    return false;
  }
}

bool ChkStringTable::serialize(std::vector<std::uint8_t>& payload,
                               std::string& error) const noexcept {
  payload.clear();
  error.clear();
  const std::size_t field_bytes =
      format_ == ChkStringFormat::classic_u16 ? 2U : 4U;
  const std::uint64_t maximum_count =
      format_ == ChkStringFormat::classic_u16 ? UINT16_MAX : UINT32_MAX;
  if (entries_.size() > maximum_count ||
      entries_.size() > (std::numeric_limits<std::size_t>::max)() /
                                field_bytes -
                            1U) {
    error = "The string table contains too many entries.";
    return false;
  }
  try {
    const std::size_t header_bytes = field_bytes * (entries_.size() + 1U);
    payload.assign(header_bytes, 0U);
    if (format_ == ChkStringFormat::classic_u16) {
      write_u16(payload, 0U, static_cast<std::uint16_t>(entries_.size()));
    } else {
      write_u32(payload, 0U, static_cast<std::uint32_t>(entries_.size()));
    }
    std::map<std::string, std::uint32_t> written{};
    for (std::size_t index = 0U; index < entries_.size(); ++index) {
      const ChkStringEntry& entry = entries_[index];
      if (!entry.present) {
        continue;
      }
      std::uint32_t offset{};
      const auto existing = written.find(entry.value);
      if (existing != written.end()) {
        offset = existing->second;
      } else {
        const std::uint64_t next_size =
            static_cast<std::uint64_t>(payload.size()) + entry.value.size() +
            1U;
        const std::uint64_t maximum_offset =
            format_ == ChkStringFormat::classic_u16 ? UINT16_MAX : UINT32_MAX;
        if (payload.size() > maximum_offset || next_size > UINT32_MAX) {
          payload.clear();
          error = "The string data exceeds the selected STR offset width.";
          return false;
        }
        offset = static_cast<std::uint32_t>(payload.size());
        payload.insert(payload.end(), entry.value.begin(), entry.value.end());
        payload.push_back(0U);
        written.emplace(entry.value, offset);
      }
      const std::size_t offset_at = field_bytes * (index + 1U);
      if (format_ == ChkStringFormat::classic_u16) {
        write_u16(payload, offset_at, static_cast<std::uint16_t>(offset));
      } else {
        write_u32(payload, offset_at, offset);
      }
    }
    return true;
  } catch (...) {
    payload.clear();
    error = "There was not enough memory to serialize the string table.";
    return false;
  }
}

std::string_view ChkStringTable::value(const std::uint32_t id) const noexcept {
  if (id == 0U || id > entries_.size() || !entries_[id - 1U].present) {
    return {};
  }
  return entries_[id - 1U].value;
}

bool ChkStringTable::find_or_append(const std::string_view value,
                                    std::uint16_t& id) noexcept {
  if (value.empty()) {
    id = 0U;
    return true;
  }
  for (std::size_t index = 0U; index < entries_.size(); ++index) {
    if (entries_[index].present && entries_[index].value == value) {
      if (index >= UINT16_MAX) {
        return false;
      }
      id = static_cast<std::uint16_t>(index + 1U);
      return true;
    }
  }
  if (entries_.size() >= UINT16_MAX) {
    return false;
  }
  try {
    entries_.push_back({true, std::string{value}});
    id = static_cast<std::uint16_t>(entries_.size());
    return true;
  } catch (...) {
    return false;
  }
}

std::size_t ChkStringTable::size() const noexcept { return entries_.size(); }
ChkStringFormat ChkStringTable::format() const noexcept { return format_; }

bool parse_scenario_property_references(
    const std::vector<std::uint8_t>& payload,
    ScenarioPropertyReferences& properties) noexcept {
  properties = {};
  if (payload.size() < 4U) {
    return false;
  }
  properties.name_string_id = read_u16(payload.data());
  properties.description_string_id = read_u16(payload.data() + 2U);
  return true;
}

bool write_scenario_property_references(
    const ScenarioPropertyReferences& properties,
    std::vector<std::uint8_t>& payload) noexcept {
  try {
    payload.resize((std::max)(payload.size(), std::size_t{4U}), 0U);
    write_u16(payload, 0U, properties.name_string_id);
    write_u16(payload, 2U, properties.description_string_id);
    return true;
  } catch (...) {
    return false;
  }
}

bool parse_force_section(const std::vector<std::uint8_t>& payload,
                         ForceSectionData& forces) noexcept {
  forces = {};
  if (payload.size() != 16U && payload.size() < 20U) {
    return false;
  }
  std::copy_n(payload.begin(), forces.player_force.size(),
              forces.player_force.begin());
  for (std::size_t force = 0U; force < force_count; ++force) {
    forces.name_string_ids[force] =
        read_u16(payload.data() + force_player_count + 2U * force);
  }
  forces.supports_flags = payload.size() >= 20U;
  if (forces.supports_flags) {
    std::copy_n(payload.begin() + 16U, forces.flags.size(),
                forces.flags.begin());
  }
  return true;
}

bool write_force_section(const ForceSectionData& forces,
                         std::vector<std::uint8_t>& payload) noexcept {
  const std::size_t minimum = forces.supports_flags ? 20U : 16U;
  if ((!payload.empty() && payload.size() != 16U && payload.size() < 20U) ||
      (!forces.supports_flags && payload.size() >= 20U)) {
    return false;
  }
  try {
    payload.resize((std::max)(payload.size(), minimum), 0U);
    std::copy(forces.player_force.begin(), forces.player_force.end(),
              payload.begin());
    for (std::size_t force = 0U; force < force_count; ++force) {
      write_u16(payload, force_player_count + 2U * force,
                forces.name_string_ids[force]);
    }
    if (forces.supports_flags) {
      std::copy(forces.flags.begin(), forces.flags.end(),
                payload.begin() + 16U);
    }
    return true;
  } catch (...) {
    return false;
  }
}

}  // namespace staredit::formats
