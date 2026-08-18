#include "formats/object_sections.hpp"

#include <algorithm>

namespace staredit::formats {
namespace {

std::uint16_t read_u16(const std::uint8_t* const bytes,
                       const std::size_t offset) noexcept {
  return static_cast<std::uint16_t>(bytes[offset]) |
         static_cast<std::uint16_t>(
             static_cast<std::uint16_t>(bytes[offset + 1U]) << 8U);
}

void write_u16(std::uint8_t* const bytes,
               const std::size_t offset,
               const std::uint16_t value) noexcept {
  bytes[offset] = static_cast<std::uint8_t>(value);
  bytes[offset + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

bool word_offset_valid(const PlacementRecordLayout& layout,
                       const std::size_t offset) noexcept {
  return offset != missing_record_offset && offset + 2U <= layout.record_bytes;
}

bool byte_offset_valid(const PlacementRecordLayout& layout,
                       const std::size_t offset) noexcept {
  return offset != missing_record_offset && offset < layout.record_bytes;
}

bool layout_valid(const PlacementRecordLayout& layout) noexcept {
  return layout.record_bytes != 0U &&
         word_offset_valid(layout, layout.type_offset) &&
         word_offset_valid(layout, layout.x_offset) &&
         word_offset_valid(layout, layout.y_offset) &&
         (layout.owner_offset == missing_record_offset ||
          byte_offset_valid(layout, layout.owner_offset)) &&
         (layout.enabled_offset == missing_record_offset ||
          byte_offset_valid(layout, layout.enabled_offset)) &&
         (layout.flags_offset == missing_record_offset ||
          word_offset_valid(layout, layout.flags_offset));
}

void encode_record(const PlacementRecord& record,
                   const PlacementRecordLayout& layout,
                   std::uint8_t* const output) noexcept {
  write_u16(output, layout.type_offset, record.type);
  write_u16(output, layout.x_offset, record.x);
  write_u16(output, layout.y_offset, record.y);
  if (byte_offset_valid(layout, layout.owner_offset)) {
    output[layout.owner_offset] = record.owner;
  }
  if (byte_offset_valid(layout, layout.enabled_offset)) {
    output[layout.enabled_offset] = record.enabled ? 1U : 0U;
  }
  if (word_offset_valid(layout, layout.flags_offset)) {
    write_u16(output, layout.flags_offset, record.flags);
  }
}

}  // namespace

bool parse_placement_records(const std::vector<std::uint8_t>& payload,
                             const PlacementRecordLayout& layout,
                             std::vector<PlacementRecord>& records) noexcept {
  records.clear();
  if (!layout_valid(layout) || payload.size() % layout.record_bytes != 0U) {
    return false;
  }
  try {
    const std::size_t count = payload.size() / layout.record_bytes;
    records.reserve(count);
    for (std::size_t index = 0U; index < count; ++index) {
      const std::uint8_t* const raw =
          payload.data() + index * layout.record_bytes;
      PlacementRecord record{};
      record.type = read_u16(raw, layout.type_offset);
      record.x = read_u16(raw, layout.x_offset);
      record.y = read_u16(raw, layout.y_offset);
      if (byte_offset_valid(layout, layout.owner_offset)) {
        record.owner = raw[layout.owner_offset];
      }
      if (byte_offset_valid(layout, layout.enabled_offset)) {
        record.enabled = raw[layout.enabled_offset] != 0U;
      }
      if (word_offset_valid(layout, layout.flags_offset)) {
        record.flags = read_u16(raw, layout.flags_offset);
      }
      record.raw.assign(raw, raw + layout.record_bytes);
      records.push_back(std::move(record));
    }
    return true;
  } catch (...) {
    records.clear();
    return false;
  }
}

bool serialize_placement_records(
    const std::vector<PlacementRecord>& records,
    const PlacementRecordLayout& layout,
    std::vector<std::uint8_t>& payload) noexcept {
  payload.clear();
  if (!layout_valid(layout)) {
    return false;
  }
  try {
    payload.resize(records.size() * layout.record_bytes);
    for (std::size_t index = 0U; index < records.size(); ++index) {
      std::uint8_t* const output =
          payload.data() + index * layout.record_bytes;
      const PlacementRecord& record = records[index];
      if (record.raw.size() == layout.record_bytes) {
        std::copy(record.raw.begin(), record.raw.end(), output);
      } else {
        std::fill(output, output + layout.record_bytes,
                  static_cast<std::uint8_t>(0U));
      }
      encode_record(record, layout, output);
    }
    return true;
  } catch (...) {
    payload.clear();
    return false;
  }
}

PlacementRecord make_placement_record(const PlacementRecordLayout& layout,
                                      const std::uint16_t type,
                                      const std::uint16_t x,
                                      const std::uint16_t y,
                                      const std::uint8_t owner) noexcept {
  PlacementRecord record{};
  record.type = type;
  record.x = x;
  record.y = y;
  record.owner = owner;
  record.enabled = true;
  try {
    record.raw.resize(layout.record_bytes);
    if (layout_valid(layout)) {
      encode_record(record, layout, record.raw.data());
      if (layout.record_bytes == 36U && layout.type_offset == 8U &&
          layout.owner_offset == 16U) {
        record.raw[17U] = 100U;
        record.raw[18U] = 100U;
        record.raw[19U] = 100U;
      }
    }
  } catch (...) {
    record.raw.clear();
  }
  return record;
}

}  // namespace staredit::formats
