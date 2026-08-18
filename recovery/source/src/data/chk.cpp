#include "starcraft/data/chk.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace starcraft::data {

ChkView::ChkView(const std::uint8_t* const bytes, const std::size_t size) noexcept
    : bytes_(bytes), size_(size) {
  if (bytes == nullptr || size == 0) {
    return;
  }
  std::size_t cursor{};
  while (cursor < size) {
    std::uint32_t payload_size{};
    if (size - cursor < 8 || !read_u32(cursor + 4, payload_size)) {
      return;
    }
    cursor += 8;
    if (payload_size > size - cursor) {
      return;
    }
    cursor += payload_size;
    ++section_count_;
  }
  valid_ = cursor == size;
}

bool ChkView::valid() const noexcept { return valid_; }

std::size_t ChkView::section_count() const noexcept {
  return valid_ ? section_count_ : 0;
}

bool ChkView::section_at(const std::size_t index, ChkSection& output) const noexcept {
  if (!valid_ || index >= section_count_) {
    return false;
  }
  std::size_t cursor{};
  for (std::size_t current = 0; current <= index; ++current) {
    std::uint32_t tag{};
    std::uint32_t payload_size{};
    if (!read_u32(cursor, tag) || !read_u32(cursor + 4, payload_size)) {
      return false;
    }
    if (current == index) {
      output = {tag, bytes_ + cursor + 8, payload_size};
      return true;
    }
    cursor += 8 + payload_size;
  }
  return false;
}

bool ChkView::section(
    const std::uint32_t tag,
    const std::size_t occurrence,
    ChkSection& output) const noexcept {
  if (!valid_) {
    return false;
  }
  std::size_t cursor{};
  std::size_t match{};
  while (cursor < size_) {
    std::uint32_t candidate{};
    std::uint32_t payload_size{};
    if (!read_u32(cursor, candidate) || !read_u32(cursor + 4, payload_size)) {
      return false;
    }
    if (candidate == tag) {
      if (match == occurrence) {
        output = {candidate, bytes_ + cursor + 8, payload_size};
        return true;
      }
      ++match;
    }
    cursor += 8 + payload_size;
  }
  return false;
}

std::size_t ChkView::count(const std::uint32_t tag) const noexcept {
  if (!valid_) {
    return 0;
  }
  std::size_t matches{};
  std::size_t cursor{};
  while (cursor < size_) {
    std::uint32_t candidate{};
    std::uint32_t payload_size{};
    if (!read_u32(cursor, candidate) || !read_u32(cursor + 4, payload_size)) {
      return 0;
    }
    matches += candidate == tag ? 1U : 0U;
    cursor += 8 + payload_size;
  }
  return matches;
}

bool ChkView::dimensions(std::uint16_t& width, std::uint16_t& height) const noexcept {
  ChkSection value{};
  if (!section(chk_section_dimensions, 0, value) || value.size != 4) {
    return false;
  }
  width = static_cast<std::uint16_t>(value.bytes[0]) |
          static_cast<std::uint16_t>(static_cast<std::uint16_t>(value.bytes[1]) << 8U);
  height = static_cast<std::uint16_t>(value.bytes[2]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(value.bytes[3]) << 8U);
  return width != 0 && height != 0;
}

bool ChkView::tileset(std::uint16_t& tileset_id) const noexcept {
  ChkSection value{};
  if (!section(chk_section_era, 0, value) || value.size < 2) {
    return false;
  }
  tileset_id = static_cast<std::uint16_t>(value.bytes[0]) |
               static_cast<std::uint16_t>(static_cast<std::uint16_t>(value.bytes[1]) << 8U);
  return true;
}

bool ChkView::player_ownership(
    const std::size_t player,
    std::uint8_t& ownership) const noexcept {
  ChkSection value{};
  if (player >= chk_player_slot_count || !section(chk_section_ownership, 0, value) ||
      value.size != chk_player_slot_count) {
    return false;
  }
  ownership = value.bytes[player];
  return true;
}

bool ChkView::player_race(const std::size_t player, std::uint8_t& race) const noexcept {
  ChkSection value{};
  if (player >= chk_player_slot_count || !section(chk_section_races, 0, value) ||
      value.size != chk_player_slot_count) {
    return false;
  }
  race = value.bytes[player];
  return true;
}

std::size_t ChkView::unit_count() const noexcept {
  ChkSection value{};
  const std::size_t record_bytes = unit_record_bytes();
  return record_bytes != 0U && section(chk_section_units, 0, value) &&
                 value.size % record_bytes == 0U
             ? value.size / record_bytes
             : 0;
}

bool ChkView::unit(const std::size_t index, BetaUnitPlacement& output) const noexcept {
  ChkSection value{};
  if (!section(chk_section_units, 0, value) || value.size % sizeof(BetaUnitPlacement) != 0 ||
      index >= value.size / sizeof(BetaUnitPlacement)) {
    return false;
  }
  const std::size_t offset = index * sizeof(BetaUnitPlacement);
  std::memcpy(&output, value.bytes + offset, sizeof(output));
  return true;
}

bool ChkView::unit(const std::size_t index, UnitPlacement& output) const noexcept {
  output = {};
  ChkSection value{};
  const std::size_t record_bytes = unit_record_bytes();
  if (record_bytes == 0U || !section(chk_section_units, 0, value) ||
      value.size % record_bytes != 0U || index >= value.size / record_bytes) {
    return false;
  }
  const std::uint8_t* const record = value.bytes + index * record_bytes;
  const std::size_t x_offset = record_bytes == 36U ? 4U : 0U;
  const std::size_t y_offset = record_bytes == 36U ? 6U : 2U;
  const std::size_t type_offset = record_bytes == 36U ? 8U : 4U;
  const std::size_t owner_offset = record_bytes == 36U ? 16U : 12U;
  output.x = static_cast<std::uint16_t>(record[x_offset]) |
             static_cast<std::uint16_t>(record[x_offset + 1U] << 8U);
  output.y = static_cast<std::uint16_t>(record[y_offset]) |
             static_cast<std::uint16_t>(record[y_offset + 1U] << 8U);
  output.unit_type = static_cast<std::uint16_t>(record[type_offset]) |
                     static_cast<std::uint16_t>(record[type_offset + 1U]
                                                << 8U);
  output.owner = record[owner_offset];
  return true;
}

std::size_t ChkView::sprite_count() const noexcept {
  ChkSection value{};
  return section(chk_section_sprites, 0, value) && value.size % sizeof(BetaSpritePlacement) == 0
      ? value.size / sizeof(BetaSpritePlacement)
      : 0;
}

bool ChkView::sprite(const std::size_t index, BetaSpritePlacement& output) const noexcept {
  ChkSection value{};
  if (!section(chk_section_sprites, 0, value) || value.size % sizeof(BetaSpritePlacement) != 0 ||
      index >= value.size / sizeof(BetaSpritePlacement)) {
    return false;
  }
  const std::size_t offset = index * sizeof(BetaSpritePlacement);
  std::memcpy(&output, value.bytes + offset, sizeof(output));
  return true;
}

bool ChkView::tile(
    const std::uint16_t x,
    const std::uint16_t y,
    std::uint16_t& tile_id) const noexcept {
  std::uint16_t width{};
  std::uint16_t height{};
  ChkSection tiles{};
  if (!dimensions(width, height) || x >= width || y >= height ||
      !section(chk_section_tiles, 0, tiles)) {
    return false;
  }
  const std::size_t tile_count = static_cast<std::size_t>(width) * height;
  if (tile_count > std::numeric_limits<std::size_t>::max() / 2 ||
      tiles.size != tile_count * 2) {
    return false;
  }
  const std::size_t offset = 2U * (static_cast<std::size_t>(y) * width + x);
  tile_id = static_cast<std::uint16_t>(tiles.bytes[offset]) |
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(tiles.bytes[offset + 1]) << 8U);
  return true;
}

bool ChkView::read_u16(const std::size_t offset, std::uint16_t& output) const noexcept {
  if (offset > size_ || size_ - offset < 2) {
    return false;
  }
  output = static_cast<std::uint16_t>(bytes_[offset]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes_[offset + 1]) << 8U);
  return true;
}

bool ChkView::read_u32(const std::size_t offset, std::uint32_t& output) const noexcept {
  if (offset > size_ || size_ - offset < 4) {
    return false;
  }
  output = static_cast<std::uint32_t>(bytes_[offset]) |
           (static_cast<std::uint32_t>(bytes_[offset + 1]) << 8U) |
           (static_cast<std::uint32_t>(bytes_[offset + 2]) << 16U) |
           (static_cast<std::uint32_t>(bytes_[offset + 3]) << 24U);
  return true;
}

std::size_t ChkView::unit_record_bytes() const noexcept {
  ChkSection units{};
  if (!section(chk_section_units, 0U, units)) {
    return 0U;
  }
  constexpr std::uint32_t version_tag = chk_fourcc('V', 'E', 'R', ' ');
  ChkSection version{};
  if (section(version_tag, 0U, version) && version.size >= 2U) {
    const std::uint16_t value =
        static_cast<std::uint16_t>(version.bytes[0U]) |
        static_cast<std::uint16_t>(version.bytes[1U] << 8U);
    return value >= 59U ? 36U : sizeof(BetaUnitPlacement);
  }
  const bool beta = units.size % sizeof(BetaUnitPlacement) == 0U;
  const bool retail = units.size % 36U == 0U;
  if (beta != retail) {
    return beta ? sizeof(BetaUnitPlacement) : 36U;
  }
  ChkSection beta_sprites{};
  const bool has_beta_sprites = section(chk_section_sprites, 0U, beta_sprites);
  return has_beta_sprites ? sizeof(BetaUnitPlacement) : (retail ? 36U : 0U);
}

}  // namespace starcraft::data
