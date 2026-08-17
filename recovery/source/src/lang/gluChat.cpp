#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace starcraft::recovery {
namespace {

constexpr std::int16_t kSlotNameBase = 100;
constexpr std::int16_t kSlotRaceBase = 200;
constexpr std::int16_t kPopupRowBase = 300;

const GlueControl *control_with_id(const std::vector<GlueControl> &controls,
                                   const std::int16_t identifier) noexcept {
  for (const GlueControl &control : controls) {
    if (control.identifier == identifier) {
      return &control;
    }
  }
  return nullptr;
}

void set_message(GlueRuntime &glue, const char *const message,
                 const std::uint32_t now) noexcept {
  try {
    glue.message = message;
    glue.message_until = now + 4500U;
  } catch (...) {
    glue.message.clear();
    glue.message_until = 0U;
  }
}

void draw_control_image(const RecoveryWindowState &state,
                        const GlueImage &image) noexcept {
  const GlueControl *const control =
      control_with_id(state.glue.lobby_controls, image.control_identifier);
  if (control == nullptr) {
    return;
  }
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
  glues_control_rect(state.glue, *control, left, top, right, bottom);
  draw_preview_frame_gl(
      image.frame, static_cast<float>(left),
      static_cast<float>(top) * hud_vertical_scale(),
      static_cast<float>(right - left + 1),
      static_cast<float>(bottom - top + 1) * hud_vertical_scale());
}

std::size_t active_slot_count(const GlueRuntime &glue) noexcept {
  std::size_t count{};
  for (const GlueLobbySlot &slot : glue.lobby_slots) {
    count += slot.ownership != 0U ? 1U : 0U;
  }
  return count;
}

std::pair<float, float> panel_offset(const RecoveryWindowState &state,
                                     const std::int16_t identifier) noexcept {
  const GlueControl *const panel =
      control_with_id(state.glue.lobby_controls, identifier);
  if (panel == nullptr) {
    return {};
  }
  std::int16_t left{};
  std::int16_t top{};
  std::int16_t right{};
  std::int16_t bottom{};
  glues_control_rect(state.glue, *panel, left, top, right, bottom);
  return {static_cast<float>(left - panel->left),
          static_cast<float>(top - panel->top)};
}

} // namespace

GlueAction activate_lobby_control(GlueRuntime &glue,
                                  const std::int16_t identifier, const int,
                                  const int, const std::uint32_t now) noexcept {
  if (identifier >= kPopupRowBase && identifier < kPopupRowBase + 3 &&
      glue.popup_control != -1) {
    const std::int16_t owner = glue.popup_control;
    const std::uint8_t choice =
        static_cast<std::uint8_t>(identifier - kPopupRowBase);
    glue.popup_control = -1;
    glue.popup_row = -1;
    if (owner >= kSlotNameBase && owner < kSlotNameBase + 12 && choice < 2U) {
      GlueLobbySlot &slot = glue.lobby_slots[static_cast<std::size_t>(
          owner - kSlotNameBase)];
      if (!slot.local) {
        if (choice == 0U) {
          slot.ownership = 5U;
          slot.name = "Computer";
          if (slot.race >= 3U) {
            slot.race = 0U;
          }
        } else {
          slot.ownership = 0U;
          slot.name.clear();
        }
      }
    } else if (owner >= kSlotRaceBase && owner < kSlotRaceBase + 12 &&
               choice < 3U) {
      GlueLobbySlot &slot = glue.lobby_slots[static_cast<std::size_t>(
          owner - kSlotRaceBase)];
      if (slot.ownership != 0U) {
        slot.race = choice;
      }
    }
    return GlueAction::redraw;
  }
  if (identifier == 7) {
    return glues_leave_screen(glue, GlueScreen::map_selection,
                              GlueAction::none, now);
  }
  if (identifier == 15) {
    return glues_leave_screen(glue, GlueScreen::map_selection,
                              GlueAction::none, now);
  }
  if (identifier >= kSlotNameBase &&
      identifier < kSlotNameBase +
                       static_cast<std::int16_t>(glue.lobby_slots.size())) {
    GlueLobbySlot &slot =
        glue.lobby_slots[static_cast<std::size_t>(identifier - kSlotNameBase)];
    if (slot.local) {
      glue.popup_control = -1;
      return GlueAction::redraw;
    }
    glue.popup_control = glue.popup_control == identifier ? -1 : identifier;
    return GlueAction::redraw;
  }
  if (identifier >= kSlotRaceBase &&
      identifier < kSlotRaceBase +
                       static_cast<std::int16_t>(glue.lobby_slots.size())) {
    GlueLobbySlot &slot =
        glue.lobby_slots[static_cast<std::size_t>(identifier - kSlotRaceBase)];
    if (slot.ownership != 0U) {
      glue.popup_control = glue.popup_control == identifier ? -1 : identifier;
    } else {
      glue.popup_control = -1;
    }
    return GlueAction::redraw;
  }
  if (identifier != 6) {
    return GlueAction::none;
  }
  // gluChat.cpp's original Okay handler in sub_45C7E0 calls
  // gluChatSlot.cpp::sub_45E160 and only begins the game when that recovered
  // slot count is at least two.
  if (active_slot_count(glue) < 2U) {
    set_message(glue, "At least two player slots are required.", now);
    return GlueAction::redraw;
  }
  glue.ready_deadline = now + 3000U + 240U;
  return glues_leave_screen(glue, GlueScreen::ready, GlueAction::none, now);
}

void draw_lobby_gl(const RecoveryWindowState &state) noexcept {
  draw_preview_frame_gl(state.glue.connection_background, 0.0F, 0.0F,
                        static_cast<float>(kMapViewportWidth),
                        static_cast<float>(kMapViewportHeight));
  for (const GlueImage &image : state.glue.lobby_images) {
    draw_control_image(state, image);
  }
  draw_lobby_slots_gl(state);

  const auto [left_x, left_y] = panel_offset(state, 3);
  const auto [right_x, right_y] = panel_offset(state, 2);

  if (state.glue.selected_map < state.glue.maps.size()) {
    const GlueMapEntry &map = state.glue.maps[state.glue.selected_map];
    draw_glue_styled_text_gl(state, "Game Name:", 404.0F + right_x,
                             69.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(
        state, state.glue.online_lobby ? "Battle.net Game" : "Local Skirmish",
        412.0F + right_x, 88.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Game Type:", 404.0F + right_x,
                             149.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Melee", 412.0F + right_x,
                             167.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Map Name:", 404.0F + right_x,
                             181.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, map.name, 412.0F + right_x,
                             200.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Map Size:", 428.0F + right_x,
                             242.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(
        state,
        std::to_string(map.width) + "x" + std::to_string(map.height),
        536.0F + right_x, 242.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Game Speed:", 428.0F + right_x,
                             260.0F + right_y, GlueFontStyle::gold);
    draw_glue_styled_text_gl(state, "Fastest", 536.0F + right_x,
                             260.0F + right_y, GlueFontStyle::gold);
  }
  draw_glue_styled_text_gl(state, "Observers", 32.0F + left_x,
                           275.0F + left_y, GlueFontStyle::gold);

  for (const std::int16_t identifier : {std::int16_t{6}, std::int16_t{7}}) {
    const GlueControl *const button =
        control_with_id(state.glue.lobby_controls, identifier);
    if (button == nullptr) {
      continue;
    }
    const bool hovered = state.glue.hovered_control == identifier;
    const bool pressed = state.glue.pressed_control == identifier;
    draw_glue_centered_styled_text_gl(
        state, button->text, *button,
        glue_control_font_style(*button, hovered || pressed),
        false);
  }
}

} // namespace starcraft::recovery
