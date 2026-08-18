#include "../platform/bootstrap_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace starcraft::recovery {
namespace {

constexpr std::int16_t kSlotNameBase = 100;
constexpr std::int16_t kSlotRaceBase = 200;
constexpr std::int16_t kPopupRowBase = 300;
constexpr int kPopupRowHeight = 18;

const GlueControl *control_with_id(const std::vector<GlueControl> &controls,
                                   const std::int16_t identifier) noexcept {
  for (const GlueControl &control : controls) {
    if (control.identifier == identifier) {
      return &control;
    }
  }
  return nullptr;
}

bool point_in_control(const GlueControl &control, const int x,
                      const int y) noexcept {
  return x >= control.left && x <= control.right && y >= control.top &&
         y <= control.bottom;
}

std::string_view race_name(const std::uint8_t race) noexcept {
  constexpr std::array<std::string_view, 3> names{{"Zerg", "Terran",
                                                   "Protoss"}};
  return race < names.size() ? names[race] : std::string_view{""};
}

const GlueControl *slot_control(const GlueRuntime &glue,
                                const std::int16_t identifier) noexcept {
  if (identifier >= kSlotNameBase && identifier < kSlotNameBase + 12) {
    const std::int16_t row = identifier - kSlotNameBase;
    return control_with_id(glue.lobby_controls,
                           static_cast<std::int16_t>(26 + row * 4));
  }
  if (identifier >= kSlotRaceBase && identifier < kSlotRaceBase + 12) {
    const std::int16_t row = identifier - kSlotRaceBase;
    return control_with_id(glue.lobby_controls,
                           static_cast<std::int16_t>(27 + row * 4));
  }
  return nullptr;
}

int popup_top(const GlueControl &control, const int rows) noexcept {
  const int below = control.bottom + 1;
  return below + rows * kPopupRowHeight <= 250
             ? below
             : control.top - rows * kPopupRowHeight;
}

void draw_combo_field(const RecoveryWindowState &state,
                      const GlueControl &control, const bool enabled,
                      const bool hovered, const bool open) noexcept {
  // The read-only Battle.snp CodeView reference names its equivalent owner
  // ComboDraw in COmbo.cpp. Keep the StarCraft glue implementation local: its
  // executable has no surviving symbol proving that it used the same name.
  const std::uint8_t border = open || hovered ? 150U : enabled ? 92U : 48U;
  glDisable(GL_TEXTURE_2D);
  glColor4ub(3U, 10U, 24U, 190U);
  glBegin(GL_QUADS);
  glVertex2f(static_cast<float>(control.left - 2),
             static_cast<float>(control.top - 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.right + 2),
             static_cast<float>(control.top - 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.right + 2),
             static_cast<float>(control.bottom + 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.left - 2),
             static_cast<float>(control.bottom + 1) * hud_vertical_scale());
  glEnd();
  glColor4ub(24U, border, enabled ? 190U : 74U, 255U);
  glBegin(GL_LINE_LOOP);
  glVertex2f(static_cast<float>(control.left - 2),
             static_cast<float>(control.top - 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.right + 2),
             static_cast<float>(control.top - 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.right + 2),
             static_cast<float>(control.bottom + 1) * hud_vertical_scale());
  glVertex2f(static_cast<float>(control.left - 2),
             static_cast<float>(control.bottom + 1) * hud_vertical_scale());
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
  if (state.glue.network_dialog_frames.size() > 52U) {
    const std::size_t frame = !enabled ? 52U : open || hovered ? 50U : 51U;
    const SpritePreviewFrame &arrow = state.glue.network_dialog_frames[frame];
    draw_preview_frame_gl(
        arrow, static_cast<float>(control.right - arrow.width),
        static_cast<float>(control.top + 3) * hud_vertical_scale(),
        static_cast<float>(arrow.width),
        static_cast<float>(arrow.height) * hud_vertical_scale());
  }
}

GlueControl transformed_control(const RecoveryWindowState &state,
                                const GlueControl &control) {
  GlueControl transformed = control;
  glues_control_rect(state.glue, control, transformed.left, transformed.top,
                     transformed.right, transformed.bottom);
  return transformed;
}

} // namespace

std::int16_t lobby_control_at(const GlueRuntime &glue, const int x,
                              const int y) noexcept {
  if (glue.popup_control != -1) {
    const GlueControl *const owner = slot_control(glue, glue.popup_control);
    const int rows = glue.popup_control >= kSlotRaceBase
                         ? 3
                         : glue.online_lobby ? 3 : 2;
    if (owner != nullptr) {
      if (point_in_control(*owner, x, y)) {
        return glue.popup_control;
      }
      const int top = popup_top(*owner, rows);
      if (x >= owner->left - 2 && x <= owner->right + 2 && y >= top &&
          y < top + rows * kPopupRowHeight) {
        return static_cast<std::int16_t>(kPopupRowBase +
                                         (y - top) / kPopupRowHeight);
      }
    }
    return -1;
  }
  for (const std::int16_t identifier :
       {std::int16_t{6}, std::int16_t{7}, std::int16_t{15}}) {
    const GlueControl *const control =
        control_with_id(glue.lobby_controls, identifier);
    if (control != nullptr && point_in_control(*control, x, y)) {
      return identifier;
    }
  }
  for (std::size_t row = 0; row < glue.lobby_slots.size(); ++row) {
    const std::int16_t first = static_cast<std::int16_t>(25 + row * 4U);
    const GlueControl *const name =
        control_with_id(glue.lobby_controls,
                        static_cast<std::int16_t>(first + 1));
    const GlueControl *const race =
        control_with_id(glue.lobby_controls,
                        static_cast<std::int16_t>(first + 2));
    if (name != nullptr && point_in_control(*name, x, y)) {
      return static_cast<std::int16_t>(kSlotNameBase + row);
    }
    if (race != nullptr && point_in_control(*race, x, y)) {
      return static_cast<std::int16_t>(kSlotRaceBase + row);
    }
  }
  return -1;
}

void draw_lobby_slots_gl(const RecoveryWindowState &state) noexcept {
  for (std::size_t row = 0; row < state.glue.lobby_slots.size(); ++row) {
    const GlueLobbySlot &slot = state.glue.lobby_slots[row];
    const std::int16_t first = static_cast<std::int16_t>(25 + row * 4U);
    const GlueControl *const number =
        control_with_id(state.glue.lobby_controls, first);
    const GlueControl *const name = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 1));
    const GlueControl *const race = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 2));
    const GlueControl *const status = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 3));
    if (number == nullptr || name == nullptr || race == nullptr ||
        status == nullptr) {
      continue;
    }
    const GlueControl number_rect = transformed_control(state, *number);
    const GlueControl name_rect = transformed_control(state, *name);
    const GlueControl race_rect = transformed_control(state, *race);
    const GlueControl status_rect = transformed_control(state, *status);
    const bool active = slot.ownership != 0U;
    const std::int16_t name_id =
        static_cast<std::int16_t>(kSlotNameBase + row);
    const std::int16_t race_id =
        static_cast<std::int16_t>(kSlotRaceBase + row);
    const bool selected = state.glue.hovered_control == name_id ||
                          state.glue.hovered_control == race_id;
    const GlueFontStyle style =
        selected ? GlueFontStyle::bright_green
                 : active ? GlueFontStyle::gold
                          : GlueFontStyle::disabled;
    const bool name_editable =
        !slot.local &&
        (!state.glue.online_lobby ||
         (state.glue.battle_net.game_host && slot.network_configurable &&
          !slot.network_player));
    const bool race_editable = active && !state.glue.online_lobby;
    draw_combo_field(state, name_rect, name_editable,
                     state.glue.hovered_control == name_id,
                     state.glue.popup_control == name_id);
    draw_combo_field(state, race_rect, race_editable,
                     state.glue.hovered_control == race_id,
                     state.glue.popup_control == race_id);
    draw_glue_styled_text_gl(state, std::to_string(row + 1U),
                             static_cast<float>(number_rect.left),
                             static_cast<float>(number_rect.bottom - 2), style);
    draw_glue_styled_text_gl(state,
                             active ? slot.name : slot.open ? "Open" : "Closed",
                             static_cast<float>(name_rect.left),
                             static_cast<float>(name_rect.bottom - 2), style);
    draw_glue_styled_text_gl(state, active ? race_name(slot.race) : "---",
                             static_cast<float>(race_rect.left),
                             static_cast<float>(race_rect.bottom - 2), style);
    draw_glue_styled_text_gl(
        state,
        active ? slot.network_player ? "OK" : "CPU" : "",
        static_cast<float>(status_rect.left),
        static_cast<float>(status_rect.bottom - 2), style);
  }

  if (state.glue.popup_control != -1) {
    const GlueControl *const owner =
        slot_control(state.glue, state.glue.popup_control);
    const bool race_popup = state.glue.popup_control >= kSlotRaceBase;
    const int rows = race_popup ? 3 : state.glue.online_lobby ? 3 : 2;
    if (owner != nullptr) {
      const int top = popup_top(*owner, rows);
      glDisable(GL_TEXTURE_2D);
      glColor4ub(2U, 8U, 20U, 245U);
      glBegin(GL_QUADS);
      glVertex2f(static_cast<float>(owner->left - 2),
                 static_cast<float>(top) * hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->right + 2),
                 static_cast<float>(top) * hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->right + 2),
                 static_cast<float>(top + rows * kPopupRowHeight) *
                     hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->left - 2),
                 static_cast<float>(top + rows * kPopupRowHeight) *
                     hud_vertical_scale());
      glEnd();
      glColor4ub(24U, 150U, 190U, 255U);
      glBegin(GL_LINE_LOOP);
      glVertex2f(static_cast<float>(owner->left - 2),
                 static_cast<float>(top) * hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->right + 2),
                 static_cast<float>(top) * hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->right + 2),
                 static_cast<float>(top + rows * kPopupRowHeight) *
                     hud_vertical_scale());
      glVertex2f(static_cast<float>(owner->left - 2),
                 static_cast<float>(top + rows * kPopupRowHeight) *
                     hud_vertical_scale());
      glEnd();
      glColor4ub(255U, 255U, 255U, 255U);
      glEnable(GL_TEXTURE_2D);
      constexpr std::array<std::string_view, 3> races{{"Zerg", "Terran",
                                                       "Protoss"}};
      constexpr std::array<std::string_view, 2> slots{{"Computer", "Closed"}};
      constexpr std::array<std::string_view, 3> online_slots{{
          "Open", "Computer", "Closed"}};
      for (int row = 0; row < rows; ++row) {
        const bool hovered = state.glue.hovered_control ==
                             kPopupRowBase + row;
        if (hovered) {
          glDisable(GL_TEXTURE_2D);
          glColor4ub(36U, 82U, 124U, 220U);
          glBegin(GL_QUADS);
          glVertex2f(static_cast<float>(owner->left),
                     static_cast<float>(top + row * kPopupRowHeight) *
                         hud_vertical_scale());
          glVertex2f(static_cast<float>(owner->right),
                     static_cast<float>(top + row * kPopupRowHeight) *
                         hud_vertical_scale());
          glVertex2f(static_cast<float>(owner->right),
                     static_cast<float>(top + (row + 1) * kPopupRowHeight) *
                         hud_vertical_scale());
          glVertex2f(static_cast<float>(owner->left),
                     static_cast<float>(top + (row + 1) * kPopupRowHeight) *
                         hud_vertical_scale());
          glEnd();
          glColor4ub(255U, 255U, 255U, 255U);
          glEnable(GL_TEXTURE_2D);
        }
        draw_glue_styled_text_gl(
            state,
            race_popup
                ? races[static_cast<std::size_t>(row)]
                : state.glue.online_lobby
                      ? online_slots[static_cast<std::size_t>(row)]
                      : slots[static_cast<std::size_t>(row)],
            static_cast<float>(owner->left + 4),
            static_cast<float>(top + row * kPopupRowHeight + 14),
            hovered ? GlueFontStyle::bright_green : GlueFontStyle::gold);
      }
    }
  }
}

} // namespace starcraft::recovery
