#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool decode_smacker_frame(SmackerPortrait &portrait) noexcept {
  if (portrait.decoder == nullptr || portrait.frame.width == 0 ||
      portrait.frame.height == 0) {
    return false;
  }
  const unsigned char *const palette = smk_get_palette(portrait.decoder);
  const unsigned char *const video = smk_get_video(portrait.decoder);
  if (palette == nullptr || video == nullptr) {
    return false;
  }
  try {
    const std::size_t pixels =
        static_cast<std::size_t>(portrait.frame.width) * portrait.frame.height;
    portrait.frame.bgra.resize(pixels);
    portrait.frame.palette_indices.assign(video, video + pixels);
    portrait.frame.opacity.assign(pixels, 0xFFU);
    for (std::size_t pixel = 0; pixel < pixels; ++pixel) {
      const std::size_t color = static_cast<std::size_t>(video[pixel]) * 3U;
      const std::uint32_t red = palette[color];
      const std::uint32_t green = palette[color + 1U];
      const std::uint32_t blue = palette[color + 2U];
      portrait.frame.bgra[pixel] =
          0xFF000000U | blue | (green << 8U) | (red << 16U);
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool load_unit_portrait(starcraft::runtime::StormModule &storm,
                        const starcraft::data::CoreDataSet &data,
                        const std::uint16_t unit_type, const std::uint8_t owner,
                        SmackerPortrait &output) noexcept {
  try {
    for (std::uint8_t variant = 0; variant < 5U; ++variant) {
      const std::string path =
          data.unit_portrait_path(unit_type, owner, false, variant);
      std::vector<std::uint8_t> bytes;
      if (path.empty() || !storm.load_file(path.c_str(), bytes) ||
          bytes.empty() || bytes.size() > ULONG_MAX) {
        continue;
      }
      SmackerPortrait portrait{};
      portrait.decoder = smk_open_memory(
          bytes.data(), static_cast<unsigned long>(bytes.size()));
      if (portrait.decoder == nullptr) {
        continue;
      }
      unsigned long width{};
      unsigned long height{};
      unsigned long frame_count{};
      double microseconds_per_frame{};
      unsigned char y_scale{};
      if (smk_info_video(portrait.decoder, &width, &height, &y_scale) != 0 ||
          smk_info_all(portrait.decoder, nullptr, &frame_count,
                       &microseconds_per_frame) != 0 ||
          width == 0 || height == 0 || width > UINT16_MAX ||
          height > UINT16_MAX || frame_count == 0 || frame_count > UINT32_MAX) {
        continue;
      }
      portrait.frame.width = static_cast<std::uint16_t>(width);
      portrait.frame.height = static_cast<std::uint16_t>(height);
      portrait.frame_count = static_cast<std::uint32_t>(frame_count);
      portrait.frame_delay_ms = static_cast<std::uint32_t>(
          (std::max)(1.0, std::abs(microseconds_per_frame) / 1000.0));
      portrait.path = path;
      (void)smk_enable_all(portrait.decoder, SMK_VIDEO_TRACK);
      const char first = smk_first(portrait.decoder);
      if (first == SMK_ERROR || first == SMK_DONE ||
          !decode_smacker_frame(portrait)) {
        continue;
      }
      portrait.ready = true;
      output = std::move(portrait);
      return true;
    }
  } catch (...) {
  }
  return false;
}

const UnitPortraitAsset *
selected_portrait(const BootstrapStatus &status) noexcept {
  const ScenarioUnitPreview *const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return nullptr;
  }
  const auto portrait =
      std::find_if(status.portraits.begin(), status.portraits.end(),
                   [selected](const UnitPortraitAsset &asset) {
                     return asset.unit_type == selected->unit_type &&
                            asset.owner == selected->owner &&
                            asset.portrait.ready;
                   });
  return portrait == status.portraits.end() ? nullptr : &*portrait;
}

UnitPortraitAsset *selected_portrait(BootstrapStatus &status) noexcept {
  const ScenarioUnitPreview *const selected = first_selected_unit(status);
  if (selected == nullptr) {
    return nullptr;
  }
  const auto portrait =
      std::find_if(status.portraits.begin(), status.portraits.end(),
                   [selected](const UnitPortraitAsset &asset) {
                     return asset.unit_type == selected->unit_type &&
                            asset.owner == selected->owner &&
                            asset.portrait.ready;
                   });
  return portrait == status.portraits.end() ? nullptr : &*portrait;
}

void draw_selected_portrait_gl(const BootstrapStatus &status) {
  const UnitPortraitAsset *const asset = selected_portrait(status);
  if (!status.portrait_panel_ready || asset == nullptr) {
    return;
  }
  constexpr float vertical_scale = 400.0F / 480.0F;
  const CommandControl &control = status.portrait_control;
  draw_preview_frame_gl(asset->portrait.frame, static_cast<float>(control.left),
                        static_cast<float>(control.top) * vertical_scale,
                        static_cast<float>(control.right - control.left + 1),
                        static_cast<float>(control.bottom - control.top + 1) *
                            vertical_scale);
}

bool advance_selected_portrait(BootstrapStatus &status,
                               const std::uint32_t now) noexcept {
  UnitPortraitAsset *const asset = selected_portrait(status);
  if (asset == nullptr || asset->portrait.decoder == nullptr ||
      now - asset->portrait.last_frame_tick < asset->portrait.frame_delay_ms) {
    return false;
  }
  char result = smk_next(asset->portrait.decoder);
  if (result == SMK_DONE) {
    result = smk_first(asset->portrait.decoder);
  }
  if (result == SMK_ERROR || result == SMK_DONE ||
      !decode_smacker_frame(asset->portrait)) {
    asset->portrait.ready = false;
    return false;
  }
  asset->portrait.last_frame_tick = now;
  return true;
}

} // namespace starcraft::recovery
