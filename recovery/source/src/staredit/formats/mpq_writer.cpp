#include "formats/mpq_writer.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <vector>

namespace staredit::formats {
namespace {

constexpr std::uint32_t kMpqSignature = 0x1A51504DU;
constexpr std::uint32_t kMpqFileExists = 0x80000000U;
constexpr std::uint32_t kMpqFileSingleUnit = 0x01000000U;
constexpr std::uint32_t kHashTableKey = 0xC3AF3770U;
constexpr std::uint32_t kBlockTableKey = 0xEC83B3A3U;
// The retail StarEdit archives use a 1,000-entry table even for the single
// scenario.chk member.  The older Storm build shipped with this recovery
// target also expects the file data to precede the encrypted tables, so keep
// that classic layout instead of emitting the smaller modern arrangement.
constexpr std::size_t kHashEntries = 1000U;

std::array<std::uint32_t, 0x500> make_crypt_table() noexcept {
  std::array<std::uint32_t, 0x500> table{};
  std::uint32_t seed = 0x00100001U;
  for (std::uint32_t first = 0U; first < 0x100U; ++first) {
    std::uint32_t index = first;
    for (std::uint32_t round = 0U; round < 5U; ++round) {
      seed = (seed * 125U + 3U) % 0x2AAAABU;
      const std::uint32_t high = (seed & 0xFFFFU) << 16U;
      seed = (seed * 125U + 3U) % 0x2AAAABU;
      table[index] = high | (seed & 0xFFFFU);
      index += 0x100U;
    }
  }
  return table;
}

std::uint32_t hash_string(const std::array<std::uint32_t, 0x500>& crypt,
                          const std::string_view value,
                          const std::uint32_t type) noexcept {
  std::uint32_t seed1 = 0x7FED7FEDU;
  std::uint32_t seed2 = 0xEEEEEEEEU;
  for (const char raw : value) {
    const unsigned char normalized =
        raw == '/' ? static_cast<unsigned char>('\\')
                   : static_cast<unsigned char>(std::toupper(
                         static_cast<unsigned char>(raw)));
    seed1 = crypt[(type << 8U) + normalized] ^ (seed1 + seed2);
    seed2 = normalized + seed1 + seed2 + (seed2 << 5U) + 3U;
  }
  return seed1;
}

void encrypt_table(const std::array<std::uint32_t, 0x500>& crypt,
                   std::vector<std::uint32_t>& values,
                   std::uint32_t seed1) noexcept {
  std::uint32_t seed2 = 0xEEEEEEEEU;
  for (std::uint32_t& value : values) {
    seed2 += crypt[0x400U + (seed1 & 0xFFU)];
    const std::uint32_t plain = value;
    value = plain ^ (seed1 + seed2);
    seed1 = ((~seed1 << 21U) + 0x11111111U) | (seed1 >> 11U);
    seed2 = plain + seed2 + (seed2 << 5U) + 3U;
  }
}

void append_u16(std::vector<std::uint8_t>& output,
                const std::uint16_t value) {
  output.push_back(static_cast<std::uint8_t>(value));
  output.push_back(static_cast<std::uint8_t>(value >> 8U));
}

void append_u32(std::vector<std::uint8_t>& output,
                const std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>(value));
  output.push_back(static_cast<std::uint8_t>(value >> 8U));
  output.push_back(static_cast<std::uint8_t>(value >> 16U));
  output.push_back(static_cast<std::uint8_t>(value >> 24U));
}

void append_words(std::vector<std::uint8_t>& output,
                  const std::vector<std::uint32_t>& values) {
  for (const std::uint32_t value : values) {
    append_u32(output, value);
  }
}

}  // namespace

bool write_single_file_mpq(const std::filesystem::path& path,
                           const std::string_view archived_path,
                           const std::vector<std::uint8_t>& bytes,
                           std::string& error) noexcept {
  error.clear();
  if (archived_path.empty() || bytes.size() > UINT32_MAX) {
    error = "The MPQ file name or payload is invalid.";
    return false;
  }
  try {
    constexpr std::uint32_t header_bytes = 32U;
    constexpr std::uint32_t hash_bytes =
        static_cast<std::uint32_t>(kHashEntries * 16U);
    constexpr std::uint32_t block_bytes = 16U;
    constexpr std::uint32_t file_offset = header_bytes;
    if (bytes.size() > UINT32_MAX - header_bytes - hash_bytes - block_bytes) {
      error = "The scenario is too large for a version-1 MPQ.";
      return false;
    }
    const std::uint32_t hash_offset =
        file_offset + static_cast<std::uint32_t>(bytes.size());
    const std::uint32_t block_offset = hash_offset + hash_bytes;
    const std::uint32_t archive_bytes =
        block_offset + block_bytes;
    const auto crypt = make_crypt_table();

    std::vector<std::uint32_t> hash_table(kHashEntries * 4U, UINT32_MAX);
    // The legacy Storm hash probe masks with table_size - 1.  Original
    // StarEdit archives retain the historical 1,000-entry size despite that
    // value not being a power of two, so reproduce the same masked slot.
    const std::size_t slot =
        hash_string(crypt, archived_path, 0U) & (kHashEntries - 1U);
    hash_table[slot * 4U] = hash_string(crypt, archived_path, 1U);
    hash_table[slot * 4U + 1U] = hash_string(crypt, archived_path, 2U);
    hash_table[slot * 4U + 2U] = 0U;  // neutral locale and platform
    hash_table[slot * 4U + 3U] = 0U;  // block-table index
    encrypt_table(crypt, hash_table, kHashTableKey);

    std::vector<std::uint32_t> block_table{
        file_offset, static_cast<std::uint32_t>(bytes.size()),
        static_cast<std::uint32_t>(bytes.size()),
        kMpqFileExists | kMpqFileSingleUnit};
    encrypt_table(crypt, block_table, kBlockTableKey);

    std::vector<std::uint8_t> archive{};
    archive.reserve(archive_bytes);
    append_u32(archive, kMpqSignature);
    append_u32(archive, header_bytes);
    append_u32(archive, archive_bytes);
    append_u16(archive, 0U);  // format version 1
    append_u16(archive, 3U);  // 4 KiB sectors
    append_u32(archive, hash_offset);
    append_u32(archive, block_offset);
    append_u32(archive, static_cast<std::uint32_t>(kHashEntries));
    append_u32(archive, 1U);
    archive.insert(archive.end(), bytes.begin(), bytes.end());
    append_words(archive, hash_table);
    append_words(archive, block_table);
    if (archive.size() != archive_bytes) {
      error = "The MPQ layout size did not balance.";
      return false;
    }

    std::ofstream output{path, std::ios::binary | std::ios::trunc};
    if (!output) {
      error = "The MPQ destination could not be created.";
      return false;
    }
    output.write(reinterpret_cast<const char*>(archive.data()),
                 static_cast<std::streamsize>(archive.size()));
    output.flush();
    if (!output) {
      error = "The MPQ destination could not be written.";
      return false;
    }
    return true;
  } catch (...) {
    error = "There was not enough memory to write the MPQ archive.";
    return false;
  }
}

}  // namespace staredit::formats
