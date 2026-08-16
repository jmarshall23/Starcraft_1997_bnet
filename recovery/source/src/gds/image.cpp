#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/gds/grp.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

bool decode_preview_frames(const std::vector<std::uint8_t> &group,
                           const std::vector<std::uint8_t> &palette,
                           std::vector<SpritePreviewFrame> &frames,
                           std::uint16_t &canvas_width,
                           std::uint16_t &canvas_height,
                           const std::vector<std::uint8_t> *const index_remap) {
  if (group.empty() || palette.size() != 1024) {
    return false;
  }
  const std::uint16_t available_frames =
      starcraft::gds::grp_frame_count(group.data(), group.size());
  const std::uint16_t frame_count = available_frames;
  if (frame_count == 0) {
    return false;
  }

  std::vector<SpritePreviewFrame> decoded_frames;
  decoded_frames.reserve(frame_count);
  for (std::uint16_t frame_index = 0; frame_index < frame_count;
       ++frame_index) {
    starcraft::gds::DecodedGrpFrame decoded{};
    if (!starcraft::gds::decode_grp_frame(group.data(), group.size(),
                                          frame_index, decoded)) {
      return false;
    }
    SpritePreviewFrame frame{};
    frame.x_offset = decoded.x_offset;
    frame.y_offset = decoded.y_offset;
    frame.width = decoded.width;
    frame.height = decoded.height;
    frame.palette_indices = decoded.pixels;
    frame.opacity = decoded.opacity;
    frame.bgra.resize(decoded.pixels.size());
    for (std::size_t pixel = 0; pixel < decoded.pixels.size(); ++pixel) {
      if (decoded.opacity[pixel] == 0) {
        frame.bgra[pixel] = 0;
        continue;
      }
      std::uint8_t palette_index = decoded.pixels[pixel];
      if (index_remap != nullptr && palette_index < index_remap->size()) {
        palette_index = (*index_remap)[palette_index];
      }
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = palette[color];
      const std::uint32_t green = palette[color + 1];
      const std::uint32_t blue = palette[color + 2];
      frame.bgra[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    canvas_width = decoded.canvas_width;
    canvas_height = decoded.canvas_height;
    decoded_frames.push_back(std::move(frame));
  }
  frames = std::move(decoded_frames);
  return true;
}

bool decode_pcx_frame(const starcraft::runtime::DecodedPcx &image,
                      const bool transparent_zero, SpritePreviewFrame &frame) {
  frame = {};
  if (image.width == 0 || image.height == 0 || image.width > 0xFFFFU ||
      image.height > 0xFFFFU || image.palette.size() != 1024 ||
      image.pixels.size() !=
          static_cast<std::size_t>(image.width) * image.height) {
    return false;
  }
  try {
    frame.width = static_cast<std::uint16_t>(image.width);
    frame.height = static_cast<std::uint16_t>(image.height);
    frame.bgra.resize(image.pixels.size());
    for (std::size_t pixel = 0; pixel < image.pixels.size(); ++pixel) {
      const std::uint8_t palette_index = image.pixels[pixel];
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = image.palette[color];
      const std::uint32_t green = image.palette[color + 1];
      const std::uint32_t blue = image.palette[color + 2];
      const std::uint32_t alpha =
          transparent_zero && palette_index == 0 ? 0U : 0xFF000000U;
      frame.bgra[pixel] = alpha | blue | (green << 8U) | (red << 16U);
    }
    return true;
  } catch (...) {
    frame = {};
    return false;
  }
}

void draw_preview_frame_gl(const SpritePreviewFrame &frame, const float x,
                           const float y, const float width, const float height,
                           const std::uint32_t *const override_bgra,
                           const bool mirrored) {
  if (frame.width == 0 || frame.height == 0 || frame.bgra.empty()) {
    return;
  }

  GLuint texture{};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width, frame.height, 0, kGlBgra,
               GL_UNSIGNED_BYTE,
               override_bgra == nullptr ? frame.bgra.data() : override_bgra);
  glBegin(GL_QUADS);
  glTexCoord2f(mirrored ? 1.0F : 0.0F, 0.0F);
  glVertex2f(x, y);
  glTexCoord2f(mirrored ? 0.0F : 1.0F, 0.0F);
  glVertex2f(x + width, y);
  glTexCoord2f(mirrored ? 0.0F : 1.0F, 1.0F);
  glVertex2f(x + width, y + height);
  glTexCoord2f(mirrored ? 1.0F : 0.0F, 1.0F);
  glVertex2f(x, y + height);
  glEnd();
  glDeleteTextures(1, &texture);
}

void draw_preview_frame_fraction_gl(const SpritePreviewFrame &frame,
                                    const float x, const float y,
                                    const float width, const float height,
                                    const float fraction) {
  if (frame.width == 0 || frame.height == 0 || frame.bgra.empty() ||
      fraction <= 0.0F) {
    return;
  }
  const float clamped = (std::clamp)(fraction, 0.0F, 1.0F);
  GLuint texture{};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width, frame.height, 0, kGlBgra,
               GL_UNSIGNED_BYTE, frame.bgra.data());
  glBegin(GL_QUADS);
  glTexCoord2f(0.0F, 0.0F);
  glVertex2f(x, y);
  glTexCoord2f(clamped, 0.0F);
  glVertex2f(x + width * clamped, y);
  glTexCoord2f(clamped, 1.0F);
  glVertex2f(x + width * clamped, y + height);
  glTexCoord2f(0.0F, 1.0F);
  glVertex2f(x, y + height);
  glEnd();
  glDeleteTextures(1, &texture);
}

void draw_team_colored_frame_gl(const BootstrapStatus &status,
                                const SpritePreviewFrame &frame,
                                const std::uint8_t owner, const float x,
                                const float y, const float width,
                                const float height, const bool mirrored) {
  if (!status.team_colors_ready || owner >= status.team_color_indices.size() ||
      status.game_palette.size() != 1024 ||
      frame.palette_indices.size() != frame.bgra.size() ||
      frame.opacity.size() != frame.bgra.size()) {
    draw_preview_frame_gl(frame, x, y, width, height, nullptr, mirrored);
    return;
  }
  try {
    std::vector<std::uint32_t> translated = frame.bgra;
    for (std::size_t pixel = 0; pixel < translated.size(); ++pixel) {
      const std::uint8_t source = frame.palette_indices[pixel];
      if (frame.opacity[pixel] == 0 || source < 8 || source > 15) {
        continue;
      }
      const std::uint8_t palette_index =
          status.team_color_indices[owner][source - 8U];
      const std::size_t color = 4U * palette_index;
      const std::uint32_t red = status.game_palette[color];
      const std::uint32_t green = status.game_palette[color + 1U];
      const std::uint32_t blue = status.game_palette[color + 2U];
      translated[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    draw_preview_frame_gl(frame, x, y, width, height, translated.data(),
                          mirrored);
  } catch (...) {
    draw_preview_frame_gl(frame, x, y, width, height, nullptr, mirrored);
  }
}

} // namespace starcraft::recovery
