#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool decode_smacker_frame(SmackerPortrait &animation) noexcept {
  if (animation.decoder == nullptr || animation.frame.width == 0U ||
      animation.frame.height == 0U) {
    return false;
  }
  const unsigned char *const palette = smk_get_palette(animation.decoder);
  const unsigned char *const video = smk_get_video(animation.decoder);
  if (palette == nullptr || video == nullptr) {
    return false;
  }
  try {
    const std::size_t pixels =
        static_cast<std::size_t>(animation.frame.width) *
        animation.frame.height;
    animation.frame.bgra.resize(pixels);
    animation.frame.palette_indices.assign(video, video + pixels);
    animation.frame.opacity.resize(pixels);
    for (std::size_t pixel = 0; pixel < pixels; ++pixel) {
      const std::size_t color = static_cast<std::size_t>(video[pixel]) * 3U;
      const std::uint32_t red = palette[color];
      const std::uint32_t green = palette[color + 1U];
      const std::uint32_t blue = palette[color + 2U];
      const bool transparent = animation.transparent_zero && video[pixel] == 0U;
      animation.frame.opacity[pixel] = transparent ? 0U : 0xFFU;
      animation.frame.bgra[pixel] =
          (transparent ? 0U : 0xFF000000U) | blue | (green << 8U) |
          (red << 16U);
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool load_smacker_animation(starcraft::runtime::StormModule &storm,
                            const char *const path,
                            const bool transparent_zero,
                            SmackerAnimation &output) noexcept {
  if (path == nullptr || *path == '\0') {
    return false;
  }
  try {
    std::vector<std::uint8_t> bytes;
    if (!storm.load_file(path, bytes) || bytes.empty() ||
        bytes.size() > ULONG_MAX) {
      return false;
    }
    SmackerAnimation animation{};
    animation.encoded = std::move(bytes);
    animation.decoder = smk_open_memory(
        animation.encoded.data(),
        static_cast<unsigned long>(animation.encoded.size()));
    if (animation.decoder == nullptr) {
      return false;
    }
    unsigned long width{};
    unsigned long height{};
    unsigned long frame_count{};
    double microseconds_per_frame{};
    unsigned char y_scale{};
    if (smk_info_video(animation.decoder, &width, &height, &y_scale) != 0 ||
        smk_info_all(animation.decoder, nullptr, &frame_count,
                     &microseconds_per_frame) != 0 ||
        width == 0U || height == 0U || width > UINT16_MAX ||
        height > UINT16_MAX || frame_count == 0U || frame_count > UINT32_MAX) {
      return false;
    }
    animation.frame.width = static_cast<std::uint16_t>(width);
    animation.frame.height = static_cast<std::uint16_t>(height);
    animation.frame_count = static_cast<std::uint32_t>(frame_count);
    animation.frame_delay_ms = static_cast<std::uint32_t>(
        (std::max)(1.0, std::abs(microseconds_per_frame) / 1000.0));
    animation.path = path;
    animation.transparent_zero = transparent_zero;
    (void)smk_enable_all(animation.decoder, SMK_VIDEO_TRACK);
    const char first = smk_first(animation.decoder);
    if (first == SMK_ERROR || first == SMK_DONE ||
        !decode_smacker_frame(animation)) {
      return false;
    }
    animation.ready = true;
    output = std::move(animation);
    return true;
  } catch (...) {
    return false;
  }
}

bool advance_smacker_animation(SmackerAnimation &animation,
                               const std::uint32_t now) noexcept {
  if (!animation.ready || animation.decoder == nullptr ||
      now - animation.last_frame_tick < animation.frame_delay_ms) {
    return false;
  }
  char result = smk_next(animation.decoder);
  if (result == SMK_DONE) {
    result = smk_first(animation.decoder);
  }
  if (result == SMK_ERROR || result == SMK_DONE ||
      !decode_smacker_frame(animation)) {
    animation.ready = false;
    return false;
  }
  animation.last_frame_tick = now;
  return true;
}

} // namespace starcraft::recovery
