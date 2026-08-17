#pragma once

#include "starcraft/runtime/storm.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace starcraft::gds {

struct FontGlyph {
  std::uint8_t width{};
  std::uint8_t height{};
  std::uint8_t x_offset{};
  std::uint8_t y_offset{};
  // One entry per glyph pixel. 0xFF means transparent; values 0..7 select a
  // shade from the active TFont row.
  std::vector<std::uint8_t> shades{};
};

struct BitmapFont {
  std::uint8_t low_character{};
  std::uint8_t high_character{};
  std::uint8_t maximum_width{};
  std::uint8_t maximum_height{};
  std::vector<FontGlyph> glyphs{};

  [[nodiscard]] const FontGlyph *glyph(unsigned char character) const noexcept;
  [[nodiscard]] float text_width(std::string_view text) const noexcept;
};

// The retail loader first decrypts font.gid with its deterministic bootstrap
// key, then uses the recovered GID contents to decrypt the FNT resource.  This
// implements the same SHA-0/IDEA container path recovered at 0x004180E0 and
// the glyph format consumed by GDS/font.cpp.
[[nodiscard]] bool load_starcraft_font(starcraft::runtime::StormModule &storm,
                                       const char *path,
                                       BitmapFont &font) noexcept;

} // namespace starcraft::gds
