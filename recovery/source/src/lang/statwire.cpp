#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace starcraft::recovery {

std::vector<std::uint32_t>
translated_wireframe(const BootstrapStatus &status,
                     const SpritePreviewFrame &frame,
                     const ScenarioUnitPreview &unit) {
  // statwire.cpp::sub_4AC130/sub_4AC2B0 use the original tables at
  // 0x00501B08 and 0x00501B68. Their entries index the 24 bytes decoded from
  // game\twire.pcx and replace GRP palette ranges 208..211 and 216..219.
  constexpr std::array<std::uint8_t, 40> life_colors{{
      10, 10, 10, 10, 10, 10, 10, 0, 10, 10, 0, 0, 10, 10, 0, 1, 10, 10, 1, 1,
      10, 0,  1,  1,  10, 1,  1,  1, 0,  1,  1, 1, 0,  1,  1, 1, 1,  1,  1, 1,
  }};
  constexpr std::array<std::uint8_t, 24> body_colors{{
      14, 6,  7,  8,  19, 11, 13, 6,  19, 11, 10, 16,
      1,  18, 20, 16, 1,  18, 20, 12, 0,  1,  18, 10,
  }};
  if (status.hud_palette.size() != 1024 ||
      status.wireframe_color_table.size() != 24 ||
      frame.palette_indices.size() != frame.bgra.size() ||
      frame.opacity.size() != frame.bgra.size() || unit.max_hit_points == 0) {
    return frame.bgra;
  }
  std::vector<std::uint32_t> translated = frame.bgra;
  const std::uint32_t rounded_life = (unit.hit_points + 255U) >> 8U;
  const std::uint32_t rounded_maximum =
      (std::max)(1U, unit.max_hit_points >> 8U);
  const std::size_t life_level =
      (std::min)(9U,
                 static_cast<unsigned>(9U * rounded_life / rounded_maximum));
  const std::size_t body_level =
      (std::min)(5U,
                 static_cast<unsigned>(5U * rounded_life / rounded_maximum));
  for (std::size_t pixel = 0; pixel < translated.size(); ++pixel) {
    if (frame.opacity[pixel] == 0) {
      continue;
    }
    const std::uint8_t source = frame.palette_indices[pixel];
    std::uint8_t table_index{0xFFU};
    if (source >= 208U && source <= 211U) {
      table_index = life_colors[4U * life_level + source - 208U];
    } else if (source >= 216U && source <= 219U) {
      table_index = body_colors[4U * body_level + source - 216U];
    }
    if (table_index >= status.wireframe_color_table.size()) {
      continue;
    }
    const std::uint8_t palette_index =
        status.wireframe_color_table[table_index];
    const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
    const std::uint32_t red = status.hud_palette[color];
    const std::uint32_t green = status.hud_palette[color + 1U];
    const std::uint32_t blue = status.hud_palette[color + 2U];
    translated[pixel] = 0xFF000000U | blue | (green << 8U) | (red << 16U);
  }
  return translated;
}

void draw_wireframe_in_control_gl(const BootstrapStatus &status,
                                  const ScenarioUnitPreview &unit,
                                  const SpritePreviewFrame &frame,
                                  const CommandControl &control) {
  const std::vector<std::uint32_t> translated =
      translated_wireframe(status, frame, unit);
  draw_preview_frame_gl(
      frame, static_cast<float>(control.left + frame.x_offset),
      static_cast<float>(control.top + frame.y_offset) * hud_vertical_scale(),
      static_cast<float>(frame.width),
      static_cast<float>(frame.height) * hud_vertical_scale(),
      translated.empty() ? nullptr : translated.data());
}

void draw_status_slot_background_gl(const BootstrapStatus &status,
                                    const CommandControl &control) {
  if (!status.command_panel_frames.empty()) {
    const SpritePreviewFrame &panel = status.command_panel_frames.front();
    draw_preview_frame_gl(panel, static_cast<float>(control.left),
                          static_cast<float>(control.top) *
                              hud_vertical_scale(),
                          static_cast<float>(control.right - control.left + 1),
                          static_cast<float>(control.bottom - control.top + 1) *
                              hud_vertical_scale());
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(24, 50, 96, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2f(control.left, control.top * hud_vertical_scale());
  glVertex2f(control.right + 1.0F, control.top * hud_vertical_scale());
  glVertex2f(control.right + 1.0F,
             (control.bottom + 1.0F) * hud_vertical_scale());
  glVertex2f(control.left, (control.bottom + 1.0F) * hud_vertical_scale());
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

void draw_status_progress_gl(const BootstrapStatus &status,
                             const CommandControl &control,
                             const float fraction) {
  const float x = static_cast<float>(control.left);
  const float y = static_cast<float>(control.top) * hud_vertical_scale();
  const float width = static_cast<float>(control.right - control.left + 1);
  const float height = static_cast<float>(control.bottom - control.top + 1) *
                       hud_vertical_scale();
  if (status.status_progress_art_ready) {
    draw_preview_frame_gl(status.status_progress_empty, x, y, width, height);
    draw_preview_frame_fraction_gl(status.status_progress_full, x, y, width,
                                   height, fraction);
    return;
  }
  glDisable(GL_TEXTURE_2D);
  glColor4ub(5, 12, 20, 255);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
  glColor4ub(32, 190, 64, 255);
  glBegin(GL_QUADS);
  glVertex2f(x + 1.0F, y + 1.0F);
  glVertex2f(x + 1.0F + (width - 2.0F) * (std::clamp)(fraction, 0.0F, 1.0F),
             y + 1.0F);
  glVertex2f(x + 1.0F + (width - 2.0F) * (std::clamp)(fraction, 0.0F, 1.0F),
             y + height - 1.0F);
  glVertex2f(x + 1.0F, y + height - 1.0F);
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

const char *active_order_text(const ActiveUnitOrder order) noexcept {
  switch (order) {
  case ActiveUnitOrder::move:
    return "Moving";
  case ActiveUnitOrder::attack:
    return "Attacking";
  case ActiveUnitOrder::repair:
    return "Repairing";
  case ActiveUnitOrder::construct:
    return "Building";
  case ActiveUnitOrder::gather:
    return "Gathering";
  case ActiveUnitOrder::return_cargo:
    return "Returning Cargo";
  case ActiveUnitOrder::return_hangar:
    return "Returning";
  default:
    return "Idle";
  }
}

void draw_selected_status_panel_gl(const RecoveryWindowState &state,
                                   const std::uint32_t now) {
  const BootstrapStatus *const status = state.status;
  if (status == nullptr || !status->status_panel_ready) {
    return;
  }
  std::array<const ScenarioUnitPreview *, 12> selected{};
  std::size_t selected_count{};
  for (const ScenarioUnitPreview &unit : status->units) {
    if (unit.alive && unit.selected && selected_count < selected.size()) {
      selected[selected_count++] = &unit;
    }
  }
  if (selected_count == 0) {
    return;
  }
  if (selected_count > 1) {
    if (!status->group_wireframe_ready) {
      return;
    }
    for (std::size_t index = 0; index < selected_count; ++index) {
      const ScenarioUnitPreview &unit = *selected[index];
      if (unit.unit_type >= status->group_wireframe_frames.size()) {
        continue;
      }
      const CommandControl &control = status->status_selection_controls[index];
      draw_status_slot_background_gl(*status, control);
      draw_wireframe_in_control_gl(
          *status, unit, status->group_wireframe_frames[unit.unit_type],
          control);
    }
    return;
  }

  const ScenarioUnitPreview &unit = *selected.front();
  if (status->wireframe_ready &&
      unit.unit_type < status->wireframe_frames.size()) {
    draw_wireframe_in_control_gl(*status, unit,
                                 status->wireframe_frames[unit.unit_type],
                                 status->status_wireframe_control);
  }
  draw_status_text_gl(
      state, status->status_name_control,
      status_text(*status, static_cast<std::uint16_t>(unit.unit_type + 1U)),
      180, 210, 255);

  char health[48]{};
  const std::uint32_t current_life = (unit.hit_points + 255U) >> 8U;
  const std::uint32_t maximum_life = (std::max)(1U, unit.max_hit_points >> 8U);
  std::snprintf(health, sizeof(health), "%u/%u", current_life, maximum_life);
  const bool high_life = unit.hit_points * 3U >= unit.max_hit_points * 2U;
  const bool medium_life = unit.hit_points * 3U >= unit.max_hit_points;
  draw_status_text_gl(state, status->status_health_control, health,
                      high_life ? 32 : 255,
                      high_life ? 230 : (medium_life ? 205 : 64), 48);

  char auxiliary[64]{};
  if (unit.unit_type == 72U || unit.unit_type == 82U ||
      unit.unit_type == 83U) {
    const std::size_t count = static_cast<std::size_t>(std::count_if(
        unit.hangar_unit_ids.begin(), unit.hangar_unit_ids.end(),
        [](const std::uint32_t id) { return id != 0U; }));
    const bool expanded =
        unit.owner < status->player_upgrade_levels.size() &&
        ((unit.unit_type == 83U &&
          status->player_upgrade_levels[unit.owner][36U] != 0U) ||
         (unit.unit_type != 83U &&
          status->player_upgrade_levels[unit.owner][43U] != 0U));
    std::snprintf(auxiliary, sizeof(auxiliary), "%s: %zu/%u",
                  unit.unit_type == 83U ? "Scarabs" : "Interceptors", count,
                  expanded ? 10U : 5U);
  } else if (unit.unit_type >= 176U && unit.unit_type <= 178U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Minerals: %u",
                  unit.resource_amount);
  } else if (unit.unit_type == 188U || unit.unit_type == 110U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Vespene Gas: %u",
                  unit.resource_amount);
  } else if (unit.cargo_minerals != 0U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Minerals: %u",
                  unit.cargo_minerals);
  } else if (unit.cargo_gas != 0U) {
    std::snprintf(auxiliary, sizeof(auxiliary), "Vespene Gas: %u",
                  unit.cargo_gas);
  } else {
    std::snprintf(auxiliary, sizeof(auxiliary), "Armor: %u", unit.armor);
  }
  draw_status_text_gl(state, status->status_aux_control, auxiliary, 96, 170,
                      255);

  // statdraw.cpp::sub_4A89B0/sub_4A9B00 copy the eight compact CUnit cargo
  // IDs, then place occupants in three exact statdata.bin layouts. Four-space
  // passengers start at control 18/20, two-space at 21/22, and one-space at
  // 25/27 depending on whether the transport capacity is greater than four.
  if (unit.cargo_space_provided != 0U) {
    std::array<CargoStatusSlot, 8> slots{};
    const std::size_t slot_count = cargo_status_slots(*status, unit, slots);
    for (std::size_t index = 0U; index < slot_count; ++index) {
      const ScenarioUnitPreview *const cargo =
          find_unit_by_id(*status, slots[index].unit_id);
      if (cargo == nullptr || slots[index].control_id < 18U ||
          slots[index].control_id > 32U) {
        continue;
      }
      const CommandControl &control =
          status->status_cargo_controls[slots[index].control_id - 18U];
      draw_status_slot_background_gl(*status, control);
      if (status->group_wireframe_ready &&
          cargo->unit_type < status->group_wireframe_frames.size()) {
        draw_wireframe_in_control_gl(
            *status, *cargo,
            status->group_wireframe_frames[cargo->unit_type], control);
      }
    }
  }

  if (!unit.construction_complete && unit.construction_ticks_total != 0U) {
    draw_status_text_gl(state, status->status_construction_label_control,
                        unit.archon_merging ? std::string_view{"Warping"}
                                            : status_text(*status, 750),
                        240, 210, 96);
    const float complete =
        1.0F - static_cast<float>(unit.construction_ticks_remaining) /
                   unit.construction_ticks_total;
    draw_status_progress_gl(
        *status, status->status_construction_progress_control, complete);
    return;
  }

  if (unit.production_active && !unit.production_queue.empty()) {
    draw_status_text_gl(state, status->status_action_label_control,
                        status_text(*status, 725), 240, 210, 96);
    for (std::size_t index = 0; index < unit.production_queue.count();
         ++index) {
      const std::uint16_t queued_type = unit.production_queue.at(index);
      const CommandControl &control = status->status_queue_controls[index];
      draw_status_slot_background_gl(*status, control);
      if (queued_type < status->command_icon_frames.size()) {
        draw_preview_frame_gl(
            status->command_icon_frames[queued_type],
            static_cast<float>(control.left),
            static_cast<float>(control.top) * hud_vertical_scale(),
            static_cast<float>(control.right - control.left + 1),
            static_cast<float>(control.bottom - control.top + 1) *
                hud_vertical_scale());
      }
    }
    const std::uint16_t queued_type = unit.production_queue.front();
    const std::uint32_t duration =
        queued_type < status->runtime_unit_types.size()
            ? static_cast<std::uint32_t>(
                  starcraft::lang::production_total_ticks(
                      status->runtime_unit_types[queued_type]
                          .initialization.simulation.build_time)) *
                  kSimulationTickMilliseconds
            : 0U;
    const float progress =
        duration == 0U
            ? 0.0F
            : static_cast<float>(
                  (std::min)(duration, now - unit.production_started)) /
                  duration;
    draw_status_progress_gl(*status, status->status_progress_control, progress);
    return;
  }

  if (unit.technology_ticks_total != 0U &&
      unit.technology_ticks_remaining != 0U) {
    const std::string_view label =
        unit.active_technology < status->technology_traits.size()
            ? std::string_view{"Researching"}
            : std::string_view{"Upgrading"};
    draw_status_text_gl(state, status->status_action_label_control, label,
                        240, 210, 96);
    const float progress =
        1.0F - static_cast<float>(unit.technology_ticks_remaining) /
                   unit.technology_ticks_total;
    draw_status_progress_gl(*status, status->status_progress_control,
                            progress);
    return;
  }

  draw_status_text_gl(state, status->status_action_label_control,
                      active_order_text(unit.active_order), 175, 175, 175);
}

std::size_t cargo_status_slots(
    const BootstrapStatus &status, const ScenarioUnitPreview &transport,
    std::array<CargoStatusSlot, 8> &slots) noexcept {
  slots.fill({});
  if (transport.cargo_space_provided == 0U) {
    return 0U;
  }
  std::size_t count{};
  const bool large_transport = transport.cargo_space_provided > 4U;
  const auto append_size = [&](const std::uint8_t cargo_size,
                               const std::uint16_t first_control) {
    std::uint16_t control_id = first_control;
    for (const std::uint32_t cargo_id : transport.cargo_unit_ids) {
      const ScenarioUnitPreview *const cargo =
          find_unit_by_id(status, cargo_id);
      if (cargo == nullptr || cargo->cargo_space_required != cargo_size) {
        continue;
      }
      if (count < slots.size() && control_id >= 18U && control_id <= 32U) {
        slots[count++] = {cargo_id, control_id};
      }
      ++control_id;
    }
  };
  append_size(4U, large_transport ? 18U : 20U);
  append_size(2U, large_transport ? 21U : 22U);
  append_size(1U, large_transport ? 25U : 27U);
  return count;
}

} // namespace starcraft::recovery
