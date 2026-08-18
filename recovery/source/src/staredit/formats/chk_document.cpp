#include "formats/chk_document.hpp"

#include "starcraft/data/chk.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace staredit::formats {
namespace {

constexpr std::uint32_t kVersionTag =
    starcraft::data::chk_fourcc('V', 'E', 'R', ' ');
constexpr std::uint32_t kRetailSpriteTag =
    starcraft::data::chk_fourcc('T', 'H', 'G', '2');

std::uint32_t read_u32(const std::uint8_t* const bytes) noexcept {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

void append_u32(std::vector<std::uint8_t>& output,
                const std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>(value));
  output.push_back(static_cast<std::uint8_t>(value >> 8U));
  output.push_back(static_cast<std::uint8_t>(value >> 16U));
  output.push_back(static_cast<std::uint8_t>(value >> 24U));
}

}  // namespace

bool ChkDocument::parse(const std::uint8_t* const bytes,
                        const std::size_t size,
                        std::string& error) noexcept {
  reset();
  error.clear();
  if (bytes == nullptr || size == 0U) {
    error = "The CHK stream is empty.";
    return false;
  }
  try {
    std::vector<std::uint8_t> owned(bytes, bytes + size);
    return parse(std::move(owned), error);
  } catch (...) {
    error = "There was not enough memory to copy the CHK stream.";
    return false;
  }
}

bool ChkDocument::parse(std::vector<std::uint8_t> bytes,
                        std::string& error) noexcept {
  reset();
  error.clear();
  if (bytes.empty()) {
    error = "The CHK stream is empty.";
    return false;
  }
  try {
    std::vector<ChkSection> parsed{};
    std::size_t cursor{};
    while (cursor < bytes.size()) {
      if (bytes.size() - cursor < 8U) {
        error = "A CHK section header is truncated.";
        return false;
      }
      const std::uint32_t tag = read_u32(bytes.data() + cursor);
      const std::uint32_t payload_size = read_u32(bytes.data() + cursor + 4U);
      cursor += 8U;
      if (payload_size > bytes.size() - cursor) {
        error = "A CHK section payload extends beyond the stream.";
        return false;
      }
      ChkSection section{};
      section.tag = tag;
      section.payload.assign(bytes.begin() + static_cast<std::ptrdiff_t>(cursor),
                             bytes.begin() + static_cast<std::ptrdiff_t>(
                                                 cursor + payload_size));
      parsed.push_back(std::move(section));
      cursor += payload_size;
    }
    sections_ = std::move(parsed);
    original_bytes_ = std::move(bytes);
    valid_ = true;
    dirty_ = false;
    update_dialect();
    return true;
  } catch (...) {
    reset();
    error = "There was not enough memory to parse the CHK stream.";
    return false;
  }
}

bool ChkDocument::valid() const noexcept { return valid_; }
bool ChkDocument::dirty() const noexcept { return dirty_; }
std::size_t ChkDocument::original_size() const noexcept {
  return original_bytes_.size();
}
std::size_t ChkDocument::section_count() const noexcept {
  return valid_ ? sections_.size() : 0U;
}

std::size_t ChkDocument::count(const std::uint32_t tag) const noexcept {
  return valid_ ? static_cast<std::size_t>(std::count_if(
                      sections_.begin(), sections_.end(),
                      [tag](const ChkSection& section) {
                        return section.tag == tag;
                      }))
                : 0U;
}

const std::vector<ChkSection>& ChkDocument::sections() const noexcept {
  return sections_;
}

const ChkSection* ChkDocument::section(const std::uint32_t tag,
                                       const std::size_t occurrence) const noexcept {
  if (!valid_) {
    return nullptr;
  }
  std::size_t match{};
  for (const ChkSection& candidate : sections_) {
    if (candidate.tag == tag && match++ == occurrence) {
      return &candidate;
    }
  }
  return nullptr;
}

ChkSection* ChkDocument::find_section(const std::uint32_t tag,
                                      const std::size_t occurrence) noexcept {
  if (!valid_) {
    return nullptr;
  }
  std::size_t match{};
  for (ChkSection& candidate : sections_) {
    if (candidate.tag == tag && match++ == occurrence) {
      return &candidate;
    }
  }
  return nullptr;
}

const ChkDialectInfo& ChkDocument::dialect() const noexcept { return dialect_; }

bool ChkDocument::replace_section(const std::uint32_t tag,
                                  const std::size_t occurrence,
                                  std::vector<std::uint8_t> payload) noexcept {
  ChkSection* const destination = find_section(tag, occurrence);
  if (destination == nullptr) {
    return false;
  }
  try {
    destination->payload = std::move(payload);
    dirty_ = true;
    update_dialect();
    return true;
  } catch (...) {
    return false;
  }
}

bool ChkDocument::append_section(const std::uint32_t tag,
                                 std::vector<std::uint8_t> payload) noexcept {
  if (!valid_ || payload.size() > UINT32_MAX) {
    return false;
  }
  try {
    sections_.push_back({tag, std::move(payload)});
    dirty_ = true;
    update_dialect();
    return true;
  } catch (...) {
    return false;
  }
}

std::size_t ChkDocument::erase_sections(const std::uint32_t tag) noexcept {
  if (!valid_) {
    return 0U;
  }
  const std::size_t previous = sections_.size();
  sections_.erase(
      std::remove_if(sections_.begin(), sections_.end(),
                     [tag](const ChkSection& section) {
                       return section.tag == tag;
                     }),
      sections_.end());
  const std::size_t removed = previous - sections_.size();
  if (removed != 0U) {
    dirty_ = true;
    update_dialect();
  }
  return removed;
}

bool ChkDocument::erase_section(const std::uint32_t tag,
                                const std::size_t occurrence) noexcept {
  std::size_t found{};
  for (auto section = sections_.begin(); section != sections_.end();
       ++section) {
    if (section->tag != tag) {
      continue;
    }
    if (found++ != occurrence) {
      continue;
    }
    sections_.erase(section);
    dirty_ = true;
    update_dialect();
    return true;
  }
  return false;
}

bool ChkDocument::serialize(std::vector<std::uint8_t>& output,
                            std::string& error) const noexcept {
  output.clear();
  error.clear();
  if (!valid_) {
    error = "The CHK document is not valid.";
    return false;
  }
  try {
    std::size_t total{};
    for (const ChkSection& section : sections_) {
      if (section.payload.size() > UINT32_MAX ||
          total > (std::numeric_limits<std::size_t>::max)() - 8U ||
          total + 8U > (std::numeric_limits<std::size_t>::max)() -
                           section.payload.size()) {
        error = "The CHK document is too large to serialize.";
        return false;
      }
      total += 8U + section.payload.size();
    }
    output.reserve(total);
    for (const ChkSection& section : sections_) {
      append_u32(output, section.tag);
      append_u32(output, static_cast<std::uint32_t>(section.payload.size()));
      output.insert(output.end(), section.payload.begin(), section.payload.end());
    }
    return true;
  } catch (...) {
    output.clear();
    error = "There was not enough memory to serialize the CHK document.";
    return false;
  }
}

bool ChkDocument::matches_original() const noexcept {
  std::vector<std::uint8_t> serialized{};
  std::string error{};
  return serialize(serialized, error) && serialized == original_bytes_;
}

void ChkDocument::reset() noexcept {
  sections_.clear();
  original_bytes_.clear();
  dialect_ = {};
  valid_ = false;
  dirty_ = false;
}

void ChkDocument::update_dialect() noexcept {
  const ChkSection* const version = section(kVersionTag);
  const ChkSection* const units = section(starcraft::data::chk_section_units);
  dialect_ = detect_chk_dialect(
      version == nullptr ? nullptr : version->payload.data(),
      version == nullptr ? 0U : version->payload.size(),
      units == nullptr ? 0U : units->payload.size(),
      section(starcraft::data::chk_section_sprites) != nullptr,
      section(kRetailSpriteTag) != nullptr);
}

}  // namespace staredit::formats
