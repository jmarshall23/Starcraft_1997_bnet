#include "starcraft/gds/font.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

namespace starcraft::gds {
namespace {

constexpr std::uint32_t legacy_rotate_left(const std::uint32_t value,
                                           const unsigned count) noexcept {
  // The recovered x86 implementation uses SAR for the high half rather than
  // SHR (491C89 and 491CB8). Preserve that signed shift exactly.
  return (value << count) |
         static_cast<std::uint32_t>(static_cast<std::int32_t>(value) >>
                                    (32U - count));
}

std::uint16_t read_u16(const std::uint8_t *const bytes) noexcept {
  return static_cast<std::uint16_t>(bytes[0]) |
         static_cast<std::uint16_t>(bytes[1] << 8U);
}

std::uint32_t read_u32(const std::uint8_t *const bytes) noexcept {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

void write_u16(std::uint8_t *const bytes, const std::uint16_t value) noexcept {
  bytes[0] = static_cast<std::uint8_t>(value);
  bytes[1] = static_cast<std::uint8_t>(value >> 8U);
}

struct Sha0State {
  std::array<std::uint32_t, 5> words{{0x67452301U, 0xEFCDAB89U,
                                      0x98BADCFEU, 0x10325476U,
                                      0xC3D2E1F0U}};

  void update_block(const std::uint8_t *const block) noexcept {
    std::array<std::uint32_t, 80> schedule{};
    for (std::size_t index = 0; index < 16U; ++index) {
      schedule[index] = read_u32(block + index * 4U);
    }
    for (std::size_t index = 16U; index < schedule.size(); ++index) {
      // This is SHA-0, not SHA-1: the expanded word is not rotated.
      schedule[index] = schedule[index - 3U] ^ schedule[index - 8U] ^
                        schedule[index - 14U] ^ schedule[index - 16U];
    }
    std::uint32_t a = words[0];
    std::uint32_t b = words[1];
    std::uint32_t c = words[2];
    std::uint32_t d = words[3];
    std::uint32_t e = words[4];
    for (std::size_t index = 0; index < schedule.size(); ++index) {
      std::uint32_t function{};
      std::uint32_t constant{};
      if (index < 20U) {
        function = (b & c) | (~b & d);
        constant = 0x5A827999U;
      } else if (index < 40U) {
        function = b ^ c ^ d;
        constant = 0x6ED9EBA1U;
      } else if (index < 60U) {
        function = (b & c) | (b & d) | (c & d);
        constant = 0x8F1BBCDCU;
      } else {
        function = b ^ c ^ d;
        constant = 0xCA62C1D6U;
      }
      const std::uint32_t temporary =
          legacy_rotate_left(a, 5U) + function + e + constant +
          schedule[index];
      e = d;
      d = c;
      c = legacy_rotate_left(b, 30U);
      b = a;
      a = temporary;
    }
    words[0] += a;
    words[1] += b;
    words[2] += c;
    words[3] += d;
    words[4] += e;
  }

  [[nodiscard]] std::array<std::uint8_t, 20> bytes() const noexcept {
    std::array<std::uint8_t, 20> result{};
    for (std::size_t index = 0; index < words.size(); ++index) {
      const std::uint32_t value = words[index];
      result[index * 4U] = static_cast<std::uint8_t>(value);
      result[index * 4U + 1U] = static_cast<std::uint8_t>(value >> 8U);
      result[index * 4U + 2U] = static_cast<std::uint8_t>(value >> 16U);
      result[index * 4U + 3U] = static_cast<std::uint8_t>(value >> 24U);
    }
    return result;
  }
};

std::uint16_t idea_multiply(const std::uint16_t left,
                            const std::uint16_t right) noexcept {
  const std::uint32_t a = left == 0U ? 65536U : left;
  const std::uint32_t b = right == 0U ? 65536U : right;
  const std::uint32_t product = (a * b) % 65537U;
  return static_cast<std::uint16_t>(product == 65536U ? 0U : product);
}

std::uint16_t idea_inverse(const std::uint16_t value) noexcept {
  if (value == 0U || value == 1U) {
    return value;
  }
  std::int32_t old_remainder = 65537;
  std::int32_t remainder = value;
  std::int32_t old_coefficient = 0;
  std::int32_t coefficient = 1;
  while (remainder != 0) {
    const std::int32_t quotient = old_remainder / remainder;
    const std::int32_t next_remainder =
        old_remainder - quotient * remainder;
    old_remainder = remainder;
    remainder = next_remainder;
    const std::int32_t next_coefficient =
        old_coefficient - quotient * coefficient;
    old_coefficient = coefficient;
    coefficient = next_coefficient;
  }
  if (old_coefficient < 0) {
    old_coefficient += 65537;
  }
  return static_cast<std::uint16_t>(old_coefficient == 65536
                                        ? 0
                                        : old_coefficient);
}

std::array<std::uint16_t, 52>
idea_expand_key(const std::uint8_t *const key) noexcept {
  std::array<std::uint16_t, 52> subkeys{};
  for (std::size_t index = 0; index < 8U; ++index) {
    subkeys[index] = read_u16(key + index * 2U);
  }
  for (std::size_t index = 8U; index < subkeys.size(); ++index) {
    switch (index & 7U) {
    case 6U:
      subkeys[index] = static_cast<std::uint16_t>(
          (subkeys[index - 7U] << 9U) | (subkeys[index - 14U] >> 7U));
      break;
    case 7U:
      subkeys[index] = static_cast<std::uint16_t>(
          (subkeys[index - 15U] << 9U) | (subkeys[index - 14U] >> 7U));
      break;
    default:
      subkeys[index] = static_cast<std::uint16_t>(
          (subkeys[index - 7U] << 9U) | (subkeys[index - 6U] >> 7U));
      break;
    }
  }
  return subkeys;
}

std::array<std::uint16_t, 52>
idea_invert_key(const std::array<std::uint16_t, 52> &encryption) noexcept {
  std::array<std::uint16_t, 52> decryption{};
  std::size_t output{};
  int input = 48;
  decryption[output++] = idea_inverse(encryption[input]);
  decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 1]);
  decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 2]);
  decryption[output++] = idea_inverse(encryption[input + 3]);
  for (int round = 0; round < 7; ++round) {
    input -= 6;
    decryption[output++] = encryption[input + 4];
    decryption[output++] = encryption[input + 5];
    decryption[output++] = idea_inverse(encryption[input]);
    decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 2]);
    decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 1]);
    decryption[output++] = idea_inverse(encryption[input + 3]);
  }
  input -= 6;
  decryption[output++] = encryption[input + 4];
  decryption[output++] = encryption[input + 5];
  decryption[output++] = idea_inverse(encryption[input]);
  decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 1]);
  decryption[output++] = static_cast<std::uint16_t>(-encryption[input + 2]);
  decryption[output++] = idea_inverse(encryption[input + 3]);
  return decryption;
}

void idea_crypt_block(const std::array<std::uint16_t, 52> &key,
                      const std::uint8_t *const input,
                      std::uint8_t *const output) noexcept {
  std::uint16_t x1 = read_u16(input);
  std::uint16_t x2 = read_u16(input + 2U);
  std::uint16_t x3 = read_u16(input + 4U);
  std::uint16_t x4 = read_u16(input + 6U);
  std::size_t subkey{};
  for (std::size_t round = 0; round < 8U; ++round) {
    x1 = idea_multiply(x1, key[subkey++]);
    x2 = static_cast<std::uint16_t>(x2 + key[subkey++]);
    x3 = static_cast<std::uint16_t>(x3 + key[subkey++]);
    x4 = idea_multiply(x4, key[subkey++]);
    std::uint16_t first = idea_multiply(
        static_cast<std::uint16_t>(x1 ^ x3), key[subkey++]);
    const std::uint16_t second = idea_multiply(
        static_cast<std::uint16_t>(first + (x2 ^ x4)), key[subkey++]);
    first = static_cast<std::uint16_t>(first + second);
    const std::uint16_t old_x2 = x2;
    x1 = static_cast<std::uint16_t>(x1 ^ second);
    x2 = static_cast<std::uint16_t>(x3 ^ second);
    x3 = static_cast<std::uint16_t>(old_x2 ^ first);
    x4 = static_cast<std::uint16_t>(x4 ^ first);
  }
  write_u16(output, x1);
  write_u16(output + 2U, static_cast<std::uint16_t>(x3 + key[subkey + 1U]));
  write_u16(output + 4U, static_cast<std::uint16_t>(x2 + key[subkey + 2U]));
  write_u16(output + 6U, idea_multiply(x4, key[subkey + 3U]));
  write_u16(output, idea_multiply(x1, key[subkey]));
}

std::uint8_t msvc_rand_byte(std::uint32_t &seed) noexcept {
  seed = seed * 214013U + 2531011U;
  return static_cast<std::uint8_t>((seed >> 16U) & 0x7FFFU);
}

std::array<std::uint8_t, 20> bootstrap_key() noexcept {
  std::array<std::uint8_t, 20> key{};
  std::uint32_t seed = 0x150BU;
  std::size_t index{};
  while (index + 1U < key.size()) {
    const std::uint8_t value = msvc_rand_byte(seed);
    if (value != 0U) {
      key[index++] = value;
    }
  }
  return key;
}

bool decrypt_resource(std::vector<std::uint8_t> &bytes,
                      const std::uint8_t *const key) noexcept {
  if (key == nullptr || bytes.size() <= 8U) {
    return false;
  }
  const std::size_t payload_size = bytes.size() - 8U;
  if ((payload_size & 63U) != 0U) {
    return false;
  }

  std::array<std::uint8_t, 112> random{};
  std::uint32_t seed = 0x4FA7U;
  for (std::uint8_t &value : random) {
    value = msvc_rand_byte(seed);
  }
  std::array<std::uint8_t, 64> repeated_key{};
  const std::size_t key_length = std::strlen(
      reinterpret_cast<const char *>(key));
  if (key_length == 0U) {
    return false;
  }
  for (std::size_t index = 0; index < repeated_key.size(); ++index) {
    repeated_key[index] = key[index % key_length];
  }
  Sha0State key_hash{};
  key_hash.update_block(repeated_key.data());
  const auto digest = key_hash.bytes();
  for (std::size_t index = 0; index < random.size(); ++index) {
    random[index] ^= digest[index % digest.size()];
  }
  const auto idea_key = idea_invert_key(idea_expand_key(random.data()));
  Sha0State stream_hash{};
  stream_hash.update_block(random.data() + 48U);

  std::array<std::uint8_t, 64> transformed{};
  for (std::size_t block_index = 0; block_index < payload_size / 64U;
       ++block_index) {
    std::uint8_t *const block = bytes.data() + block_index * 64U;
    if ((block_index & 7U) == 0U) {
      for (std::size_t offset = 0; offset < 64U; offset += 8U) {
        idea_crypt_block(idea_key, block + offset,
                         transformed.data() + offset);
      }
    } else {
      std::copy_n(block, 64U, transformed.begin());
    }
    const auto state_bytes = stream_hash.bytes();
    for (std::size_t index = 0; index < 64U; ++index) {
      block[index] = static_cast<std::uint8_t>(
          transformed[index] - state_bytes[(63U - index) % 20U]);
    }
    if ((block_index & 15U) == 0U) {
      stream_hash.update_block(block);
    }
  }

  const std::uint8_t *const trailer = bytes.data() + payload_size;
  const std::size_t decoded_size =
      payload_size + static_cast<std::size_t>(trailer[5]) - 64U;
  if (trailer[4] != 0U || decoded_size > payload_size ||
      read_u32(trailer) != stream_hash.words[0]) {
    return false;
  }
  bytes.resize(decoded_size);
  return true;
}

bool parse_font(const std::vector<std::uint8_t> &bytes,
                BitmapFont &font) {
  font = {};
  if (bytes.size() < 12U || std::memcmp(bytes.data(), "FONT", 4U) != 0) {
    return false;
  }
  const std::uint8_t low = bytes[4];
  const std::uint8_t high = bytes[5];
  if (low > high) {
    return false;
  }
  const std::size_t glyph_count = static_cast<std::size_t>(high - low) + 1U;
  if (8U + glyph_count * 4U > bytes.size()) {
    return false;
  }
  BitmapFont parsed{};
  parsed.low_character = low;
  parsed.high_character = high;
  parsed.maximum_width = bytes[6];
  parsed.maximum_height = bytes[7];
  parsed.glyphs.reserve(glyph_count);
  for (std::size_t index = 0; index < glyph_count; ++index) {
    const std::size_t offset = read_u32(bytes.data() + 8U + index * 4U);
    const std::size_t next = index + 1U < glyph_count
                                 ? read_u32(bytes.data() + 12U + index * 4U)
                                 : bytes.size();
    if (offset + 4U > bytes.size() || next < offset + 4U ||
        next > bytes.size()) {
      return false;
    }
    FontGlyph glyph{};
    glyph.width = bytes[offset];
    glyph.height = bytes[offset + 1U];
    glyph.x_offset = bytes[offset + 2U];
    glyph.y_offset = bytes[offset + 3U];
    const std::size_t cells =
        static_cast<std::size_t>(glyph.width) * glyph.height;
    glyph.shades.assign(cells, 0xFFU);
    std::size_t position{};
    for (std::size_t encoded = offset + 4U;
         encoded < next && position < cells; ++encoded) {
      const std::uint8_t value = bytes[encoded];
      position += value >> 3U;
      if (position < cells) {
        glyph.shades[position++] = value & 7U;
      }
    }
    parsed.glyphs.push_back(std::move(glyph));
  }
  font = std::move(parsed);
  return true;
}

} // namespace

const FontGlyph *BitmapFont::glyph(const unsigned char character) const noexcept {
  if (character < low_character || character > high_character) {
    return nullptr;
  }
  const std::size_t index = character - low_character;
  return index < glyphs.size() ? &glyphs[index] : nullptr;
}

float BitmapFont::text_width(const std::string_view text) const noexcept {
  float width{};
  for (const unsigned char character : text) {
    if (character == ' ') {
      width += static_cast<float>(maximum_width / 2U);
    } else if (const FontGlyph *const value = glyph(character)) {
      width += static_cast<float>(value->width + value->x_offset + 1U);
    } else if (const FontGlyph *const fallback = glyph('?')) {
      width += static_cast<float>(fallback->width + fallback->x_offset + 1U);
    }
  }
  return width;
}

bool load_starcraft_font(starcraft::runtime::StormModule &storm,
                         const char *const path,
                         BitmapFont &font) noexcept {
  font = {};
  if (path == nullptr) {
    return false;
  }
  try {
    std::vector<std::uint8_t> gid;
    std::vector<std::uint8_t> bytes;
    const auto initial_key = bootstrap_key();
    if (!storm.load_file(R"(font\font.gid)", gid) ||
        !decrypt_resource(gid, initial_key.data())) {
      return false;
    }
    if (gid.empty() || gid.size() > 20U || !storm.load_file(path, bytes)) {
      return false;
    }
    gid.push_back(0U);
    return decrypt_resource(bytes, gid.data()) && parse_font(bytes, font);
  } catch (...) {
    font = {};
    return false;
  }
}

} // namespace starcraft::gds
