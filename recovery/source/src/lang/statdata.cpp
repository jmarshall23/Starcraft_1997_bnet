#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {

bool parse_dialog_control(const std::vector<std::uint8_t> &layout,
                          const std::int16_t identifier,
                          CommandControl &output) noexcept {
  if (layout.size() < 70 || read_u16(layout, 34) != 0) {
    return false;
  }
  const std::int16_t root_x = static_cast<std::int16_t>(read_u16(layout, 4));
  const std::int16_t root_y = static_cast<std::int16_t>(read_u16(layout, 6));
  std::uint32_t offset = read_u32(layout, 66);
  while (offset != 0 && offset + 70U <= layout.size()) {
    if (static_cast<std::int16_t>(read_u16(layout, offset + 32U)) ==
        identifier) {
      output.position = static_cast<std::uint16_t>(identifier);
      output.left = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 4U)));
      output.top = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 6U)));
      output.right = static_cast<std::int16_t>(
          root_x + static_cast<std::int16_t>(read_u16(layout, offset + 8U)));
      output.bottom = static_cast<std::int16_t>(
          root_y + static_cast<std::int16_t>(read_u16(layout, offset + 10U)));
      return output.right >= output.left && output.bottom >= output.top;
    }
    offset = read_u32(layout, offset);
  }
  return false;
}

bool parse_status_panel_controls(const std::vector<std::uint8_t> &layout,
                                 BootstrapStatus &status) noexcept {
  if (!parse_dialog_control(layout, 1, status.status_wireframe_control) ||
      !parse_dialog_control(layout, -5, status.status_name_control) ||
      !parse_dialog_control(layout, -7, status.status_health_control) ||
      !parse_dialog_control(layout, -8, status.status_aux_control) ||
      !parse_dialog_control(layout, -15, status.status_action_label_control) ||
      !parse_dialog_control(layout, 7, status.status_progress_control) ||
      !parse_dialog_control(layout, -30,
                            status.status_construction_label_control) ||
      !parse_dialog_control(layout, 13,
                            status.status_construction_progress_control)) {
    return false;
  }
  for (std::size_t index = 0; index < status.status_queue_controls.size();
       ++index) {
    if (!parse_dialog_control(layout, static_cast<std::int16_t>(2 + index),
                              status.status_queue_controls[index])) {
      return false;
    }
  }
  for (std::size_t index = 0; index < status.status_selection_controls.size();
       ++index) {
    if (!parse_dialog_control(layout, static_cast<std::int16_t>(33 + index),
                              status.status_selection_controls[index])) {
      return false;
    }
  }
  return true;
}

std::string_view status_text(const BootstrapStatus &status,
                             const std::uint16_t one_based_id) noexcept {
  const starcraft::data::StringTableView table{status.stat_text_table.data(),
                                               status.stat_text_table.size()};
  return table.one_based(one_based_id);
}

std::string printable_status_text(const std::string_view source) {
  std::string output;
  output.reserve(source.size());
  for (const unsigned char value : source) {
    if (value >= 32U && value < 127U) {
      output.push_back(static_cast<char>(value));
    }
  }
  return output;
}

std::size_t status_selection_unit_at(const BootstrapStatus &status,
                                     const int game_x,
                                     const int game_y) noexcept {
  if (!status.status_panel_ready) {
    return SIZE_MAX;
  }
  std::size_t selected_count{};
  for (const ScenarioUnitPreview &unit : status.units) {
    selected_count += unit.alive && unit.selected ? 1U : 0U;
  }
  if (selected_count <= 1U) {
    return SIZE_MAX;
  }
  std::size_t slot{};
  for (std::size_t unit_index = 0;
       unit_index < status.units.size() &&
       slot < status.status_selection_controls.size();
       ++unit_index) {
    const ScenarioUnitPreview &unit = status.units[unit_index];
    if (!unit.alive || !unit.selected) {
      continue;
    }
    const CommandControl &control = status.status_selection_controls[slot++];
    const int top = static_cast<int>(control.top * hud_vertical_scale());
    const int bottom =
        static_cast<int>((control.bottom + 1) * hud_vertical_scale()) - 1;
    if (game_x >= control.left && game_x <= control.right && game_y >= top &&
        game_y <= bottom) {
      return unit_index;
    }
  }
  return SIZE_MAX;
}

} // namespace starcraft::recovery
