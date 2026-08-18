#include "formats/isom_section.hpp"

#include <limits>

namespace staredit::formats {
namespace {

constexpr std::size_t kLinksPerEntry = 4U;
constexpr std::size_t kBytesPerEntry = kLinksPerEntry * sizeof(std::uint16_t);

std::uint16_t read_u16(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint16_t>(bytes[0]) |
         static_cast<std::uint16_t>(
             static_cast<std::uint16_t>(bytes[1]) << 8U);
}

}  // namespace

bool IsomSection::initialize(const std::uint16_t map_width,
                             const std::uint16_t map_height) noexcept {
  reset();
  if (map_width == 0U || map_height == 0U) {
    return false;
  }
  const std::size_t columns = static_cast<std::size_t>(map_width) / 2U + 1U;
  const std::size_t rows = static_cast<std::size_t>(map_height) + 1U;
  if (columns > (std::numeric_limits<std::size_t>::max)() / rows) {
    return false;
  }
  try {
    entries_.assign(columns * rows, IsomEntry{});
    columns_ = columns;
    rows_ = rows;
    valid_ = true;
    return true;
  } catch (...) {
    reset();
    return false;
  }
}

bool IsomSection::parse(const std::uint8_t* const payload,
                        const std::size_t payload_size,
                        const std::uint16_t map_width,
                        const std::uint16_t map_height,
                        std::string& error) noexcept {
  reset();
  error.clear();
  if (payload == nullptr || map_width == 0U || map_height == 0U) {
    error = "The ISOM section has no map dimensions or payload.";
    return false;
  }
  const std::size_t columns = static_cast<std::size_t>(map_width) / 2U + 1U;
  const std::size_t rows = static_cast<std::size_t>(map_height) + 1U;
  if (columns > (std::numeric_limits<std::size_t>::max)() / rows ||
      columns * rows >
          (std::numeric_limits<std::size_t>::max)() / kBytesPerEntry) {
    error = "The ISOM dimensions overflow the address space.";
    return false;
  }
  const std::size_t count = columns * rows;
  if (payload_size != count * kBytesPerEntry) {
    error = "The ISOM payload does not match DIM dimensions.";
    return false;
  }
  try {
    entries_.resize(count);
    for (std::size_t index = 0U; index < count; ++index) {
      const std::uint8_t* const source = payload + index * kBytesPerEntry;
      for (std::size_t link = 0U; link < kLinksPerEntry; ++link) {
        entries_[index].links[link].raw = read_u16(source + 2U * link);
      }
    }
    columns_ = columns;
    rows_ = rows;
    valid_ = true;
    return true;
  } catch (...) {
    reset();
    error = "There was not enough memory to decode the ISOM section.";
    return false;
  }
}

bool IsomSection::serialize(std::vector<std::uint8_t>& payload) const noexcept {
  payload.clear();
  if (!valid_) {
    return false;
  }
  try {
    payload.resize(entries_.size() * kBytesPerEntry);
    for (std::size_t index = 0U; index < entries_.size(); ++index) {
      for (std::size_t link = 0U; link < kLinksPerEntry; ++link) {
        const std::uint16_t value = entries_[index].links[link].raw;
        payload[index * kBytesPerEntry + 2U * link] =
            static_cast<std::uint8_t>(value);
        payload[index * kBytesPerEntry + 2U * link + 1U] =
            static_cast<std::uint8_t>(value >> 8U);
      }
    }
    return true;
  } catch (...) {
    payload.clear();
    return false;
  }
}

bool IsomSection::valid() const noexcept { return valid_; }
std::size_t IsomSection::column_count() const noexcept { return columns_; }
std::size_t IsomSection::row_count() const noexcept { return rows_; }
std::size_t IsomSection::entry_count() const noexcept {
  return valid_ ? entries_.size() : 0U;
}

const IsomEntry* IsomSection::entry(const std::size_t column,
                                    const std::size_t row) const noexcept {
  return valid_ && column < columns_ && row < rows_
             ? &entries_[row * columns_ + column]
             : nullptr;
}

IsomEntry* IsomSection::mutable_entry(const std::size_t column,
                                      const std::size_t row) noexcept {
  return valid_ && column < columns_ && row < rows_
             ? &entries_[row * columns_ + column]
             : nullptr;
}

void IsomSection::reset() noexcept {
  entries_.clear();
  columns_ = 0U;
  rows_ = 0U;
  valid_ = false;
}

}  // namespace staredit::formats
