#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {

bool parse_resource_panel_controls(const std::vector<std::uint8_t> &layout,
                                   BootstrapStatus &status) noexcept {
  // statres.cpp::sub_4ABA90 at 0x004ABA90 selects supply controls 1, 2, and 3
  // for CHK races 0 (Zerg), 2 (Protoss), and 1 (Terran), then uses control 4
  // for gas and control 5 for minerals. Preserve that non-linear association.
  return parse_dialog_control(layout, 1,
                              status.resource_supply_controls[0]) &&
         parse_dialog_control(layout, 3,
                              status.resource_supply_controls[1]) &&
         parse_dialog_control(layout, 2,
                              status.resource_supply_controls[2]) &&
         parse_dialog_control(layout, 4, status.resource_gas_control) &&
         parse_dialog_control(layout, 5, status.resource_mineral_control);
}

void post_resource_error(BootstrapStatus &status, const bool gas) noexcept {
  // Resource checks at 0x00402000/0x00402240 prioritize minerals, then gas.
  // They call 0x00456180 with stat_txt IDs 781/782 and sound IDs race+124/
  // race+127. The local-player helper posts the message for 7000 ms.
  const std::uint16_t text_id = gas ? 782U : 781U;
  const std::string_view archived = status_text(status, text_id);
  status.system_message = archived.empty() ? (gas ? "Not enough Vespene gas."
                                                  : "Not enough minerals.")
                                           : printable_status_text(archived);
  status.system_message_until = GetTickCount() + 7000U;
  status.pending_resource_error_sound = gas ? 1U : 0U;
  ++status.resource_error_count;
}

bool resource_cost_available(BootstrapStatus &status,
                             const std::uint32_t minerals,
                             const std::uint32_t gas) noexcept {
  if (status.player_minerals < minerals) {
    post_resource_error(status, false);
    return false;
  }
  if (status.player_gas < gas) {
    post_resource_error(status, true);
    return false;
  }
  return true;
}

void advance_resource_display(BootstrapStatus &status) noexcept {
  // statres.cpp::sub_4ABC80 at 0x004ABC80 eases each displayed resource
  // counter by delta/16 plus its sign instead of snapping to the live total.
  const auto approach = [](std::uint32_t &displayed,
                           const std::uint32_t actual) {
    if (displayed == actual) {
      return;
    }
    const std::int64_t delta = static_cast<std::int64_t>(actual) - displayed;
    const std::int64_t step = delta / 16 + (delta <= 0 ? -1 : 1);
    const std::int64_t next = static_cast<std::int64_t>(displayed) + step;
    displayed = static_cast<std::uint32_t>(
        (std::clamp)(next, static_cast<std::int64_t>(0),
                     static_cast<std::int64_t>(UINT32_MAX)));
  };
  approach(status.displayed_gas, status.player_gas);
  approach(status.displayed_minerals, status.player_minerals);
}

std::array<std::uint32_t, 2>
local_supply(const BootstrapStatus &status) noexcept {
  if (!status.unit_traits_ready || status.local_race >= 3U) {
    return {{0U, 0U}};
  }
  starcraft::lang::UnitTraitsTable effective_traits = status.unit_traits;
  starcraft::lang::MeleeUnitTypes local_types{};
  if (!starcraft::lang::melee_unit_types(status.local_race, local_types)) {
    return {{0U, 0U}};
  }
  // Explicit requested retail compatibility override; the licensed beta DAT
  // predates the ten-supply starting-base balance.
  effective_traits[local_types.base].supply_provided =
      starcraft::lang::retail_melee_base_supply_internal;
  starcraft::lang::UnitCountState counts{};
  for (auto &race : counts.supply.maximum) {
    race.fill(400U);
  }
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || unit.unit_type >= effective_traits.size() ||
        unit.owner >= starcraft::lang::kPlayerCount) {
      continue;
    }
    starcraft::game::Unit counted{};
    counted.owner = unit.owner;
    counted.unit_type = unit.unit_type;
    if (unit.construction_complete) {
      counted.status_flags |= starcraft::game::unit_status_complete;
    }
    starcraft::lang::mark_unit_counted_as_existing(counted, effective_traits,
                                                   counts);
    if (unit.construction_complete) {
      starcraft::lang::mark_unit_counted_as_complete(counted, false,
                                                     effective_traits, counts);
    }
  }
  const auto race = static_cast<starcraft::lang::Race>(status.local_race);
  std::uint32_t used = starcraft::lang::supply_used(race, 0U, counts.supply);
  for (const ScenarioUnitPreview &producer : status.units) {
    if (!producer.alive || producer.owner != 0U) {
      continue;
    }
    for (std::size_t index = 0; index < producer.production_queue.count();
         ++index) {
      const std::uint16_t type = producer.production_queue.at(index);
      if (type < effective_traits.size()) {
        used += effective_traits[type].supply_required;
      }
    }
  }
  return {{
      used,
      starcraft::lang::supply_provided_capped(race, 0U, counts.supply),
  }};
}

std::size_t resource_supply_icon_frame(const std::uint8_t race) noexcept {
  // statres.cpp::sub_4ABA90 passes frame 4 at 0x004ABAD8 for Zerg, frame 5 at
  // 0x004ABB0E for Protoss, and frame 6 at 0x004ABB3C for Terran. CHK race
  // order is Zerg, Terran, Protoss, hence the intentionally non-linear table.
  constexpr std::array<std::size_t, 3> frames{{4U, 6U, 5U}};
  return race < frames.size() ? frames[race] : 0U;
}

void draw_resource_strip_gl(const RecoveryWindowState &state) {
  const BootstrapStatus *const status = state.status;
  if (status == nullptr || !status->resource_panel_ready) {
    return;
  }
  // game\icons.grp is the raw-frame group loaded by statres.cpp::sub_4AB900.
  // Frame 0 is minerals. The call at 0x004ABB51 passes CHK race + 1 for gas;
  // supply is frame 4 Zerg, frame 5 Protoss, and frame 6 Terran.
  if (status->resource_icons_ready && status->local_race < 3U &&
      status->resource_icon_frames.size() >= 7U) {
    const auto draw_icon = [](const SpritePreviewFrame &frame,
                              const CommandControl &control) {
      draw_preview_frame_gl(
          frame, static_cast<float>(control.left + frame.x_offset),
          static_cast<float>(control.top + frame.y_offset) *
              hud_vertical_scale(),
          static_cast<float>(frame.width),
          static_cast<float>(frame.height) * hud_vertical_scale());
    };
    draw_icon(status->resource_icon_frames[0],
              status->resource_mineral_control);
    draw_icon(status->resource_icon_frames[1U + status->local_race],
              status->resource_gas_control);
    draw_icon(status->resource_icon_frames[resource_supply_icon_frame(
                  status->local_race)],
              status->resource_supply_controls[status->local_race]);
  }
  const auto draw_right_aligned =
      [&state](const CommandControl &source, const std::string &text,
               const std::uint8_t red, const std::uint8_t green,
               const std::uint8_t blue) {
        CommandControl control = source;
        control.left = static_cast<std::int16_t>(
            (std::max)(static_cast<int>(control.left),
                       static_cast<int>(control.right) -
                           static_cast<int>(text.size()) * 6));
        draw_status_text_gl(state, control, text, red, green, blue);
      };
  char value[32]{};
  const std::array<std::uint32_t, 2> supply = local_supply(*status);
  std::snprintf(value, sizeof(value), "%u/%u", (supply[0] + 1U) >> 1U,
                (supply[1] + 1U) >> 1U);
  const CommandControl &supply_control =
      status->resource_supply_controls[status->local_race < 3U
                                           ? status->local_race
                                           : 0U];
  draw_right_aligned(supply_control, value,
                     supply[0] <= supply[1] ? 220 : 255,
                     supply[0] <= supply[1] ? 220 : 72,
                     supply[0] <= supply[1] ? 220 : 48);
  std::snprintf(value, sizeof(value), "%u", status->displayed_gas);
  draw_right_aligned(status->resource_gas_control, value, 96, 230, 128);
  std::snprintf(value, sizeof(value), "%u", status->displayed_minerals);
  draw_right_aligned(status->resource_mineral_control, value, 80, 220, 255);
}

} // namespace starcraft::recovery
