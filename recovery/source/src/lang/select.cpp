#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

void clear_selection(BootstrapStatus &status) noexcept {
  for (ScenarioUnitPreview &unit : status.units) {
    unit.selected = false;
  }
}

std::size_t selection_count(const BootstrapStatus &status) noexcept {
  std::size_t count{};
  for (const ScenarioUnitPreview &unit : status.units) {
    count += unit.selected ? 1U : 0U;
  }
  return count;
}

void complete_selection_drag(RecoveryWindowState &state) noexcept {
  BootstrapStatus *const status = state.status;
  if (status == nullptr) {
    return;
  }
  status->active_command_card = 0;
  status->placement_active = false;
  status->placement_valid = false;
  status->placement_unit_type = 0xFFFFU;
  cancel_command_target(*status);
  const bool additive = (GetKeyState(VK_SHIFT) & 0x8000) != 0 ||
                        (GetKeyState(VK_CONTROL) & 0x8000) != 0;
  const int delta_x =
      std::abs(state.selection_current_x - state.selection_start_x);
  const int delta_y =
      std::abs(state.selection_current_y - state.selection_start_y);

  if (delta_x <= 4 && delta_y <= 4) {
    // usel.cpp::sub_4B51B0 at 0x004B51B0 defines a click as a drag no
    // larger than four pixels. sub_4B51F0 uses Shift or Control to toggle.
    std::size_t hit = status->units.size();
    std::uint64_t best_distance = UINT64_MAX;
    for (std::size_t index = 0; index < status->units.size(); ++index) {
      const ScenarioUnitPreview &unit = status->units[index];
      if (!unit.alive) {
        continue;
      }
      const int center_x = unit.x - status->camera_x;
      const int center_y = unit.y - status->camera_y;
      const int half_width = static_cast<int>(unit.selection_width) / 2;
      const int half_height = static_cast<int>(unit.selection_height) / 2;
      if (state.selection_current_x < center_x - half_width ||
          state.selection_current_x > center_x + half_width ||
          state.selection_current_y < center_y - half_height ||
          state.selection_current_y > center_y + half_height) {
        continue;
      }
      const std::int64_t dx = state.selection_current_x - center_x;
      const std::int64_t dy = state.selection_current_y - center_y;
      const std::uint64_t distance =
          static_cast<std::uint64_t>(dx * dx + dy * dy);
      if (distance < best_distance) {
        best_distance = distance;
        hit = index;
      }
    }
    if (!additive) {
      clear_selection(*status);
    }
    if (hit < status->units.size()) {
      status->units[hit].selected =
          additive ? !status->units[hit].selected : true;
      if (status->units[hit].selected &&
          queue_unit_response(*status, status->units[hit], false)) {
        (void)play_pending_game_sound(state);
      }
    }
    return;
  }

  if (!additive) {
    clear_selection(*status);
  }
  const int left =
      (std::min)(state.selection_start_x, state.selection_current_x);
  const int right =
      (std::max)(state.selection_start_x, state.selection_current_x);
  const int top =
      (std::min)(state.selection_start_y, state.selection_current_y);
  const int bottom =
      (std::max)(state.selection_start_y, state.selection_current_y);
  std::size_t selected = selection_count(*status);
  for (ScenarioUnitPreview &unit : status->units) {
    if (!unit.alive) {
      continue;
    }
    const int center_x = unit.x - status->camera_x;
    const int center_y = unit.y - status->camera_y;
    if (!unit.selected && unit.owner == 0 && center_x >= left &&
        center_x <= right && center_y >= top && center_y <= bottom &&
        selected < 12) {
      unit.selected = true;
      ++selected;
    }
  }
  const ScenarioUnitPreview *const primary = first_selected_unit(*status);
  if (primary != nullptr && queue_unit_response(*status, *primary, false)) {
    (void)play_pending_game_sound(state);
  }
}

const ScenarioUnitPreview *
first_selected_unit(const BootstrapStatus &status) noexcept {
  const auto selected = std::find_if(status.units.begin(), status.units.end(),
                                     [](const ScenarioUnitPreview &unit) {
                                       return unit.alive && unit.selected;
                                     });
  return selected == status.units.end() ? nullptr : &*selected;
}

void draw_selection_drag_gl(const RecoveryWindowState &state) noexcept {
  if (!state.selection_dragging) {
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(32, 255, 32, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(state.selection_start_x, state.selection_start_y);
  glVertex2i(state.selection_current_x, state.selection_start_y);
  glVertex2i(state.selection_current_x, state.selection_current_y);
  glVertex2i(state.selection_start_x, state.selection_current_y);
  glEnd();
  glColor4ub(255, 255, 255, 255);
  glEnable(GL_TEXTURE_2D);
}

} // namespace starcraft::recovery
