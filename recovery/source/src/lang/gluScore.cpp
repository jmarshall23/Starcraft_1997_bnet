#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace starcraft::recovery {
namespace {

const GlueControl *score_control(const GameDialogRuntime &dialog,
                                 const std::int16_t identifier) noexcept {
  for (const GlueControl &control : dialog.score_controls) {
    if (control.identifier == identifier) {
      return &control;
    }
  }
  return nullptr;
}

void draw_score_three_piece(const RecoveryWindowState &state,
                            const GlueControl &control,
                            const std::size_t first) noexcept {
  const std::uint8_t race = state.status == nullptr
                                ? 0U
                                : (std::min)(state.status->local_race,
                                             static_cast<std::uint8_t>(2U));
  const auto &frames = state.game_dialog.dialog_control_frames[race];
  if (first + 2U >= frames.size()) {
    return;
  }
  const float top = static_cast<float>(control.top) * hud_vertical_scale();
  const float height =
      static_cast<float>(control.bottom - control.top + 1) *
      hud_vertical_scale();
  const int left_width = frames[first].width;
  const int right_width = frames[first + 2U].width;
  const int middle_width =
      (std::max)(1, static_cast<int>(control.right - control.left + 1) -
                        left_width - right_width);
  draw_preview_frame_gl(frames[first], static_cast<float>(control.left), top,
                        static_cast<float>(left_width), height);
  draw_preview_frame_gl(frames[first + 1U],
                        static_cast<float>(control.left + left_width), top,
                        static_cast<float>(middle_width), height);
  draw_preview_frame_gl(
      frames[first + 2U],
      static_cast<float>(control.right - right_width + 1), top,
      static_cast<float>(right_width), height);
}

void draw_score_tab(const RecoveryWindowState &state,
                    const GlueControl &control, const bool selected,
                    const bool hovered) noexcept {
  draw_score_three_piece(state, control,
                         121U + (selected ? 6U : hovered ? 3U : 0U));
}

std::uint32_t animated_value(const std::uint32_t target,
                             const std::uint32_t elapsed) noexcept {
  // gluScore.cpp::sub_464730 advances each displayed field every 20 ms by
  // one eighth of the remaining difference, with a minimum step of four.
  std::uint32_t value{};
  const std::uint32_t steps = (std::min)(elapsed / 20U, 512U);
  for (std::uint32_t step = 0; step < steps && value < target; ++step) {
    const std::uint32_t difference = target - value;
    value += (std::max)(difference / 8U, 4U);
    value = (std::min)(value, target);
  }
  return value;
}

std::string score_text(const GameDialogRuntime &dialog,
                       const std::size_t one_based) {
  const auto &table = dialog.glue_text_table;
  if (table.size() < 2U || one_based == 0U ||
      one_based > read_u16(table, 0U) ||
      2U * (one_based + 1U) > table.size()) {
    return {};
  }
  const std::size_t offset = read_u16(table, 2U * one_based);
  if (offset >= table.size()) {
    return {};
  }
  std::string text;
  for (std::size_t index = offset; index < table.size() && table[index] != 0U;
       ++index) {
    if (table[index] >= 32U) {
      text.push_back(static_cast<char>(table[index]));
    }
  }
  return text;
}

std::array<std::string, 3> headings(const GameDialogRuntime &dialog,
                                    const ScoreCategory category) {
  std::size_t first{};
  switch (category) {
  case ScoreCategory::overview:
    first = 2U;
    break;
  case ScoreCategory::units:
    first = 5U;
    break;
  case ScoreCategory::structures:
    first = 8U;
    break;
  case ScoreCategory::resources:
    first = 11U;
    break;
  }
  return {{score_text(dialog, first), score_text(dialog, first + 1U),
           score_text(dialog, first + 2U)}};
}

std::array<std::uint32_t, 3> category_values(
    const MatchScoreRow &row, const ScoreCategory category) noexcept {
  switch (category) {
  case ScoreCategory::overview:
    return {{row.unit_score, row.structure_score, row.resource_score}};
  case ScoreCategory::units:
    return {{row.produced, row.killed, row.lost}};
  case ScoreCategory::structures:
    return {{row.built, row.razed, row.structures_lost}};
  case ScoreCategory::resources:
    return {{row.gas, row.minerals, row.resources_spent}};
  }
  return {};
}

std::uint32_t category_total(const MatchScoreRow &row,
                             const ScoreCategory category) noexcept {
  switch (category) {
  case ScoreCategory::overview:
    return row.total;
  case ScoreCategory::units:
    return row.unit_score;
  case ScoreCategory::structures:
    return row.structure_score;
  case ScoreCategory::resources:
    return row.resource_score;
  }
  return 0U;
}

void draw_number(const RecoveryWindowState &state,
                 const GlueControl *const control, const std::uint32_t value,
                 const bool total = false) noexcept {
  if (control == nullptr) {
    return;
  }
  draw_game_dialog_centered_styled_text_gl(
      state, std::to_string(value), *control,
      total ? GlueFontStyle::green : GlueFontStyle::bright_green, false);
}

} // namespace

void draw_score_screen_gl(const RecoveryWindowState &state,
                          const std::uint32_t now) noexcept {
  const GameDialogRuntime &dialog = state.game_dialog;
  const std::uint8_t race = state.status == nullptr
                                ? 0U
                                : (std::min)(state.status->local_race,
                                             static_cast<std::uint8_t>(2U));
  const std::size_t theme = 2U * race +
                            (dialog.outcome == MatchOutcome::victory ? 1U : 0U);
  if (theme < dialog.score_backgrounds.size() &&
      !dialog.score_backgrounds[theme].bgra.empty()) {
    draw_preview_frame_gl(dialog.score_backgrounds[theme], 0.0F, 0.0F,
                          static_cast<float>(kMapViewportWidth),
                          static_cast<float>(kMapViewportHeight));
  }

  std::string title = score_text(
      dialog, dialog.outcome == MatchOutcome::victory ? 15U : 14U);
  if (title.empty()) {
    title = dialog.outcome == MatchOutcome::victory ? "Victory!" : "Defeat!";
  }
  const GlueControl *const title_control = score_control(dialog, 2);
  if (title_control != nullptr) {
    draw_game_dialog_centered_styled_text_gl(
        state, title, *title_control,
        dialog.outcome == MatchOutcome::victory ? GlueFontStyle::error
                                                 : GlueFontStyle::normal,
        true);
  }

  for (std::int16_t identifier = 3; identifier <= 6; ++identifier) {
    const GlueControl *const control = score_control(dialog, identifier);
    if (control == nullptr) {
      continue;
    }
    const bool selected = identifier >= 3 && identifier <= 6 &&
                          identifier - 3 ==
                              static_cast<int>(dialog.score_category);
    const bool hovered = dialog.hovered_control == identifier;
    draw_score_tab(state, *control, selected, hovered);
    draw_game_dialog_centered_styled_text_gl(
        state, control->text, *control,
        selected || hovered ? GlueFontStyle::bright_green
                            : GlueFontStyle::green,
        true);
  }
  if (const GlueControl *const ok = score_control(dialog, 7); ok != nullptr) {
    const bool hovered = dialog.hovered_control == 7;
    const bool pressed = dialog.pressed_control == 7;
    draw_score_three_piece(state, *ok,
                           112U + (pressed ? 3U : hovered ? 6U : 0U));
    draw_game_dialog_centered_styled_text_gl(
        state, ok->text, *ok,
        hovered || pressed ? GlueFontStyle::bright_green
                           : GlueFontStyle::green,
        true);
  }

  const auto labels = headings(dialog, dialog.score_category);
  for (std::size_t column = 0; column < labels.size(); ++column) {
    const GlueControl *const control =
        score_control(dialog, static_cast<std::int16_t>(9U + column));
    if (control != nullptr) {
      draw_game_dialog_centered_styled_text_gl(
          state, labels[column], *control, GlueFontStyle::gold, false);
    }
  }
  if (const GlueControl *const total = score_control(dialog, 8);
      total != nullptr) {
    draw_game_dialog_centered_styled_text_gl(
        state, total->text.empty() ? "Total Score" : total->text, *total,
        GlueFontStyle::green, false);
  }

  const std::uint32_t elapsed = now - dialog.score_started_tick;
  for (std::size_t row_index = 0;
       row_index < dialog.score_rows.size() && row_index < 8U; ++row_index) {
    const MatchScoreRow &row = dialog.score_rows[row_index];
    const std::int16_t base =
        static_cast<std::int16_t>(12U + 6U * row_index);
    if (theme < dialog.score_boxes.size() &&
        !dialog.score_boxes[theme].bgra.empty()) {
      for (std::int16_t column = 0; column < 4; ++column) {
        const GlueControl *const value_control =
            score_control(dialog, base + 2 + column);
        if (value_control != nullptr) {
          draw_preview_frame_gl(
              dialog.score_boxes[theme],
              static_cast<float>(value_control->left),
              static_cast<float>(value_control->top) * hud_vertical_scale(),
              static_cast<float>(dialog.score_boxes[theme].width),
              static_cast<float>(dialog.score_boxes[theme].height) *
                  hud_vertical_scale());
        }
      }
    }
    const GlueControl *const name = score_control(dialog, base + 1);
    if (name != nullptr) {
      const bool local = state.status != nullptr &&
                         row.player == state.status->local_player;
      draw_game_dialog_centered_styled_text_gl(
          state, row.name, *name,
          row.victorious ? GlueFontStyle::error
                         : local ? GlueFontStyle::green
                                 : GlueFontStyle::gold,
          false);
    }
    const auto values = category_values(row, dialog.score_category);
    for (std::size_t column = 0; column < values.size(); ++column) {
      draw_number(state, score_control(dialog, base + 2 +
                                                   static_cast<std::int16_t>(column)),
                  animated_value(values[column], elapsed));
    }
    draw_number(state, score_control(dialog, base + 5),
                animated_value(category_total(row, dialog.score_category),
                               elapsed),
                true);
  }
}

} // namespace starcraft::recovery
