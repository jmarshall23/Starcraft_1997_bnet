#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool load_unit_portrait(starcraft::runtime::StormModule &storm,
                        const starcraft::data::CoreDataSet &data,
                        const std::uint16_t unit_type, const std::uint8_t owner,
                        SmackerPortrait &output) noexcept {
  try {
    for (std::uint8_t variant = 0; variant < 5U; ++variant) {
      const std::string path =
          data.unit_portrait_path(unit_type, owner, false, variant);
      SmackerPortrait portrait{};
      if (!load_smacker_animation(storm, path.c_str(), false, portrait)) {
        continue;
      }
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
  return asset != nullptr && advance_smacker_animation(asset->portrait, now);
}

} // namespace starcraft::recovery
