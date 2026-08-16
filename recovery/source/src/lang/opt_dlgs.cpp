#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {
namespace {

constexpr std::size_t screen_index(const GameDialogScreen screen) noexcept {
  return static_cast<std::size_t>(screen);
}

const std::vector<GlueControl> &current_layout(
    const GameDialogRuntime &dialog) noexcept {
  if (dialog.screen == GameDialogScreen::score) {
    return dialog.score_controls;
  }
  const std::size_t index = screen_index(dialog.screen);
  return index < dialog.layouts.size() ? dialog.layouts[index]
                                       : dialog.layouts[0];
}

bool point_in_control(const GlueControl &control, const int x,
                      const int y) noexcept {
  return x >= control.left && x <= control.right && y >= control.top &&
         y <= control.bottom;
}

bool actionable(const GlueControl &control) noexcept {
  return control.type == 1U || control.type == 2U || control.type == 3U ||
         control.type == 4U || control.type == 6U || control.type == 14U;
}

bool visible_control(const GameDialogRuntime &dialog,
                     const GlueControl &control) noexcept {
  switch (dialog.screen) {
  case GameDialogScreen::game_menu:
    if (control.identifier == 1 || control.identifier == 2) {
      return false; // opt_dlgs.cpp hides Save/Load in multiplayer.
    }
    if (control.identifier == 7) {
      return !dialog.paused;
    }
    if (control.identifier == 8) {
      return dialog.paused;
    }
    break;
  case GameDialogScreen::abort_menu:
    if (control.identifier == 1) {
      return false; // Restart Mission is hidden in a multiplayer game.
    }
    break;
  case GameDialogScreen::victory:
    if (control.identifier == 1) {
      return false;
    }
    break;
  case GameDialogScreen::defeat:
    break;
  default:
    break;
  }
  return true;
}

std::int16_t control_at(const GameDialogRuntime &dialog, const int x,
                        const int game_y) noexcept {
  const int y = static_cast<int>(
      static_cast<float>(game_y) / hud_vertical_scale());
  for (const GlueControl &control : current_layout(dialog)) {
    if (visible_control(dialog, control) && actionable(control) &&
        point_in_control(control, x, y)) {
      return control.identifier;
    }
  }
  return -32768;
}

void enter_dialog(GameDialogRuntime &dialog,
                  const GameDialogScreen screen) noexcept {
  dialog.screen = screen;
  dialog.hovered_control = -1;
  dialog.pressed_control = -1;
}

void draw_solid_rect(const float left, const float top, const float right,
                     const float bottom, const std::uint8_t red,
                     const std::uint8_t green, const std::uint8_t blue,
                     const std::uint8_t alpha) noexcept {
  glDisable(GL_TEXTURE_2D);
  glColor4ub(red, green, blue, alpha);
  glBegin(GL_QUADS);
  glVertex2f(left, top * hud_vertical_scale());
  glVertex2f(right, top * hud_vertical_scale());
  glVertex2f(right, bottom * hud_vertical_scale());
  glVertex2f(left, bottom * hud_vertical_scale());
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
}

void dialog_bounds(const std::vector<GlueControl> &controls, int &left,
                   int &top, int &right, int &bottom) noexcept {
  left = 640;
  top = 480;
  right = 0;
  bottom = 0;
  for (const GlueControl &control : controls) {
    left = (std::min)(left, static_cast<int>(control.left));
    top = (std::min)(top, static_cast<int>(control.top));
    right = (std::max)(right, static_cast<int>(control.right));
    bottom = (std::max)(bottom, static_cast<int>(control.bottom));
  }
  left = (std::max)(0, left - 20);
  right = (std::min)(639, right + 20);
  top = (std::max)(0, top);
  bottom = (std::min)(479, bottom + 8);
}

void draw_dialog_tiles(const GameDialogRuntime &dialog, const int left,
                       const int top, const int right,
                       const int bottom) noexcept {
  draw_solid_rect(0.0F, 0.0F, 640.0F, 480.0F, 0U, 0U, 0U, 112U);
  draw_solid_rect(static_cast<float>(left), static_cast<float>(top),
                  static_cast<float>(right), static_cast<float>(bottom), 4U,
                  8U, 12U, 238U);
  if (dialog.dialog_tile_frames.size() < 9U) {
    draw_solid_rect(static_cast<float>(left), static_cast<float>(top),
                    static_cast<float>(right), static_cast<float>(top + 2),
                    120U, 88U, 36U, 255U);
    return;
  }
  const float corner = 24.0F;
  const float width = static_cast<float>(right - left);
  const float height = static_cast<float>(bottom - top);
  draw_preview_frame_gl(dialog.dialog_tile_frames[0], static_cast<float>(left),
                        static_cast<float>(top) * hud_vertical_scale(), corner,
                        corner * hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[1], left + corner,
                        static_cast<float>(top) * hud_vertical_scale(),
                        (std::max)(1.0F, width - 2.0F * corner),
                        corner * hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[2], right - corner,
                        static_cast<float>(top) * hud_vertical_scale(), corner,
                        corner * hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[3], static_cast<float>(left),
                        (top + corner) * hud_vertical_scale(), corner,
                        (std::max)(1.0F, height - 2.0F * corner) *
                            hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[5], right - corner,
                        (top + corner) * hud_vertical_scale(), corner,
                        (std::max)(1.0F, height - 2.0F * corner) *
                            hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[6], static_cast<float>(left),
                        (bottom - corner) * hud_vertical_scale(), corner,
                        corner * hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[7], left + corner,
                        (bottom - corner) * hud_vertical_scale(),
                        (std::max)(1.0F, width - 2.0F * corner),
                        corner * hud_vertical_scale());
  draw_preview_frame_gl(dialog.dialog_tile_frames[8], right - corner,
                        (bottom - corner) * hud_vertical_scale(), corner,
                        corner * hud_vertical_scale());
}

std::string table_string(const std::vector<std::uint8_t> &table,
                         const std::size_t one_based) {
  if (table.size() < 2U || one_based == 0U) {
    return {};
  }
  const std::size_t count = read_u16(table, 0U);
  if (one_based > count || 2U * (one_based + 1U) > table.size()) {
    return {};
  }
  const std::size_t offset = read_u16(table, 2U * one_based);
  if (offset >= table.size()) {
    return {};
  }
  std::string text;
  for (std::size_t index = offset; index < table.size() && table[index] != 0U;
       ++index) {
    const std::uint8_t value = table[index];
    if (value == '\r' || value == '\n') {
      text.push_back('\n');
    } else if (value >= 32U) {
      text.push_back(static_cast<char>(value));
    }
  }
  return text;
}

std::size_t table_count(const std::vector<std::uint8_t> &table) noexcept {
  return table.size() >= 2U ? read_u16(table, 0U) : 0U;
}

void draw_wrapped_text(const RecoveryWindowState &state,
                       const std::string_view source, float x, float y,
                       const std::size_t columns,
                       const std::size_t max_lines) noexcept {
  std::string line;
  std::size_t lines{};
  for (const char character : source) {
    if (character == '\n' || line.size() >= columns) {
      draw_glue_text_gl(state, line, x, y, 210U, 210U, 210U, false);
      line.clear();
      y += 13.0F;
      if (++lines >= max_lines) {
        return;
      }
      if (character == '\n') {
        continue;
      }
    }
    line.push_back(character);
  }
  if (!line.empty() && lines < max_lines) {
    draw_glue_text_gl(state, line, x, y, 210U, 210U, 210U, false);
  }
}

void draw_dialog_controls(const RecoveryWindowState &state) noexcept {
  const GameDialogRuntime &dialog = state.game_dialog;
  for (const GlueControl &control : current_layout(dialog)) {
    if (!visible_control(dialog, control) || control.text.empty()) {
      continue;
    }
    if (actionable(control)) {
      const bool hovered = dialog.hovered_control == control.identifier;
      const bool pressed = dialog.pressed_control == control.identifier;
      draw_solid_rect(static_cast<float>(control.left),
                      static_cast<float>(control.top),
                      static_cast<float>(control.right),
                      static_cast<float>(control.bottom),
                      pressed ? 88U : hovered ? 52U : 20U,
                      pressed ? 68U : hovered ? 62U : 34U,
                      pressed ? 28U : hovered ? 76U : 50U, 210U);
      draw_glue_centered_text_gl(
          state, control.text, control, pressed ? 255U : 220U,
          hovered ? 220U : 205U, hovered ? 96U : 180U,
          control.type == 14U || control.type == 2U);
      if (control.type == 4U || control.type == 3U) {
        bool checked{};
        if (dialog.screen == GameDialogScreen::sound_options) {
          checked = control.identifier == 3 ? dialog.unit_speech
                    : control.identifier == 4 ? dialog.acknowledgements
                                              : dialog.building_sounds;
        } else if (dialog.screen == GameDialogScreen::tips) {
          checked = dialog.show_tips;
        } else if (dialog.screen == GameDialogScreen::voice_options) {
          checked = control.identifier ==
                    static_cast<std::int16_t>(5U - dialog.voice_mode);
        }
        draw_solid_rect(static_cast<float>(control.left + 3),
                        static_cast<float>(control.top + 3),
                        static_cast<float>(control.left + 13),
                        static_cast<float>(control.top + 13),
                        checked ? 224U : 18U, checked ? 192U : 32U,
                        checked ? 64U : 38U, 255U);
      } else if (control.type == 6U) {
        std::uint8_t value{};
        if (dialog.screen == GameDialogScreen::sound_options) {
          value = control.identifier == 1 ? dialog.music_volume
                                           : dialog.digital_volume;
        } else {
          const std::uint8_t speed =
              control.identifier == 1 ? dialog.game_speed
              : control.identifier == 2 ? dialog.mouse_scroll
                                         : dialog.key_scroll;
          value = static_cast<std::uint8_t>(100U * speed / 6U);
        }
        const float knob = control.left +
                           (control.right - control.left) * value / 100.0F;
        draw_solid_rect(knob - 3.0F, static_cast<float>(control.bottom - 8),
                        knob + 3.0F, static_cast<float>(control.bottom - 1),
                        232U, 202U, 86U, 255U);
      }
    } else {
      draw_glue_centered_text_gl(state, control.text, control, 232U, 220U,
                                 176U, control.identifier == -1);
    }
  }

  if (dialog.screen == GameDialogScreen::keystroke_help) {
    constexpr std::size_t visible_lines = 16U;
    const std::size_t count = table_count(dialog.help_text_table);
    float y = 78.0F;
    for (std::size_t row = 0; row < visible_lines; ++row) {
      const std::size_t identifier = dialog.help_scroll + row + 1U;
      if (identifier > count) {
        break;
      }
      const std::string help = table_string(dialog.help_text_table, identifier);
      draw_glue_text_gl(state, help, 190.0F, y, 210U, 210U, 210U, false);
      y += 13.0F;
    }
  } else if (dialog.screen == GameDialogScreen::tips) {
    const std::string tip = table_string(dialog.tips_table, dialog.tip_index);
    draw_wrapped_text(state, tip, 206.0F, 142.0F, 38U, 7U);
  } else if (dialog.screen == GameDialogScreen::objectives) {
    // Melee.trg condition 14/action 1 and condition 2/action 2.
    draw_wrapped_text(state,
                      "Victory: eliminate all opposing players.\n"
                      "Defeat: lose all of your buildings.",
                      210.0F, 113.0F, 34U, 8U);
  }
}

void set_slider(GameDialogRuntime &dialog, const std::int16_t identifier,
                const int x) noexcept {
  const auto &controls = current_layout(dialog);
  const auto found = std::find_if(
      controls.begin(), controls.end(), [identifier](const GlueControl &value) {
        return value.identifier == identifier && value.type == 6U;
      });
  if (found == controls.end() || found->right <= found->left) {
    return;
  }
  const int value = 100 * ((std::clamp)(x, static_cast<int>(found->left),
                                        static_cast<int>(found->right)) -
                           found->left) /
                    (found->right - found->left);
  if (dialog.screen == GameDialogScreen::sound_options) {
    (identifier == 1 ? dialog.music_volume : dialog.digital_volume) =
        static_cast<std::uint8_t>(value);
  } else if (dialog.screen == GameDialogScreen::speed_options) {
    std::uint8_t &target = identifier == 1 ? dialog.game_speed
                           : identifier == 2 ? dialog.mouse_scroll
                                             : dialog.key_scroll;
    target = static_cast<std::uint8_t>((value * 6) / 100);
  }
}

GameDialogAction activate_control(RecoveryWindowState &state,
                                  const std::int16_t identifier, const int x,
                                  const std::uint32_t now) noexcept {
  GameDialogRuntime &dialog = state.game_dialog;
  switch (dialog.screen) {
  case GameDialogScreen::game_menu:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::none);
    } else if (identifier == 3) {
      enter_dialog(dialog, GameDialogScreen::options);
    } else if (identifier == 4) {
      enter_dialog(dialog, GameDialogScreen::help_menu);
    } else if (identifier == 5) {
      enter_dialog(dialog, GameDialogScreen::objectives);
    } else if (identifier == 6) {
      enter_dialog(dialog, GameDialogScreen::abort_menu);
    } else if (identifier == 7 || identifier == 8) {
      dialog.paused = identifier == 7;
      enter_dialog(dialog, GameDialogScreen::none);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::options:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::game_menu);
    } else if (identifier == 1) {
      enter_dialog(dialog, GameDialogScreen::sound_options);
    } else if (identifier == 2) {
      enter_dialog(dialog, GameDialogScreen::speed_options);
    } else if (identifier == 3) {
      enter_dialog(dialog, GameDialogScreen::voice_options);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::sound_options:
    if (identifier == 1 || identifier == 2) {
      set_slider(dialog, identifier, x);
      if (state.audio_ready) {
        alSourcef(identifier == 1 ? state.music_source : state.audio_source,
                  AL_GAIN,
                  static_cast<float>(identifier == 1 ? dialog.music_volume
                                                      : dialog.digital_volume) /
                      100.0F);
      }
    } else if (identifier == 3) {
      dialog.unit_speech = !dialog.unit_speech;
    } else if (identifier == 4) {
      dialog.acknowledgements = !dialog.acknowledgements;
    } else if (identifier == 5) {
      dialog.building_sounds = !dialog.building_sounds;
    } else if (identifier == -2 || identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::options);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::speed_options:
    if (identifier >= 1 && identifier <= 3) {
      set_slider(dialog, identifier, x);
    } else if (identifier == -2 || identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::options);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::voice_options:
    if (identifier >= 3 && identifier <= 5) {
      dialog.voice_mode = static_cast<std::uint8_t>(5 - identifier);
    } else if (identifier == -2 || identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::options);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::help_menu:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::game_menu);
    } else if (identifier == 1) {
      dialog.help_scroll = 0U;
      enter_dialog(dialog, GameDialogScreen::keystroke_help);
    } else if (identifier == 2) {
      enter_dialog(dialog, GameDialogScreen::tips);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::keystroke_help:
    if (identifier == -2 || identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::help_menu);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::tips:
    if (identifier == 1) {
      dialog.tip_index = dialog.tip_index >= 3U ? 1U : dialog.tip_index + 1U;
    } else if (identifier == 2) {
      dialog.show_tips = !dialog.show_tips;
    } else if (identifier == -2 || identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::help_menu);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::objectives:
    if (identifier == -3 || identifier == -2) {
      enter_dialog(dialog, GameDialogScreen::game_menu);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::abort_menu:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::game_menu);
    } else if (identifier == 1) {
      enter_dialog(dialog, GameDialogScreen::confirm_restart);
    } else if (identifier == 2) {
      enter_dialog(dialog, GameDialogScreen::confirm_quit_menu);
    } else if (identifier == 3) {
      enter_dialog(dialog, GameDialogScreen::confirm_quit_program);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::confirm_restart:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::abort_menu);
      return GameDialogAction::redraw;
    }
    return identifier == -2 ? GameDialogAction::restart_match
                            : GameDialogAction::none;
  case GameDialogScreen::confirm_quit_menu:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::abort_menu);
      return GameDialogAction::redraw;
    }
    if (identifier == 1) {
      dialog.observer_mode = true;
      dialog.match_active = true;
      dialog.paused = false;
      enter_dialog(dialog, GameDialogScreen::none);
      return GameDialogAction::redraw;
    }
    if (identifier == -2) {
      if (dialog.outcome == MatchOutcome::none) {
        dialog.outcome = MatchOutcome::defeat;
      }
      dialog.paused = true;
      dialog.match_active = false;
      if (!play_result_music(state, dialog.outcome) && state.music_playing) {
        alSourceStop(state.music_source);
        state.music_playing = false;
      }
      build_match_scores(state);
      dialog.score_started_tick = now;
      enter_dialog(dialog, GameDialogScreen::score);
      return GameDialogAction::redraw;
    }
    return GameDialogAction::none;
  case GameDialogScreen::confirm_quit_program:
    if (identifier == -3) {
      enter_dialog(dialog, GameDialogScreen::abort_menu);
      return GameDialogAction::redraw;
    }
    return identifier == -2 ? GameDialogAction::quit_program
                            : GameDialogAction::none;
  case GameDialogScreen::victory:
    if (identifier == 2) {
      dialog.match_active = true;
      dialog.paused = false;
      enter_dialog(dialog, GameDialogScreen::none);
    } else if (identifier == -2) {
      build_match_scores(state);
      dialog.score_started_tick = now;
      enter_dialog(dialog, GameDialogScreen::score);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::defeat:
    if (identifier == 1) {
      dialog.observer_mode = true;
      dialog.match_active = true;
      dialog.paused = false;
      enter_dialog(dialog, GameDialogScreen::none);
    } else if (identifier == -2) {
      build_match_scores(state);
      dialog.score_started_tick = now;
      enter_dialog(dialog, GameDialogScreen::score);
    }
    return GameDialogAction::redraw;
  case GameDialogScreen::score:
    if (identifier >= 3 && identifier <= 6) {
      dialog.score_category =
          static_cast<ScoreCategory>(identifier - 3);
      dialog.score_started_tick = now;
      return GameDialogAction::redraw;
    }
    return identifier == 7 ? GameDialogAction::return_to_menu
                           : GameDialogAction::none;
  default:
    return GameDialogAction::none;
  }
}

bool load_layout(starcraft::runtime::StormModule &storm, const char *path,
                 std::vector<GlueControl> &controls) {
  std::vector<std::uint8_t> bytes;
  return storm.load_file(path, bytes) && parse_glue_layout(bytes, controls);
}

bool load_pcx(starcraft::runtime::StormModule &storm, const std::string &path,
              SpritePreviewFrame &frame) {
  starcraft::runtime::DecodedPcx pcx{};
  return storm.load_pcx(path.c_str(), pcx) &&
         decode_pcx_frame(pcx, false, frame);
}

} // namespace

bool initialize_game_dialog_assets(GameDialogRuntime &dialog,
                                   const BootstrapStatus &status) noexcept {
  dialog = {};
  const std::filesystem::path root = locate_input_root();
  if (root.empty()) {
    return false;
  }
  starcraft::runtime::StormModule storm{root / L"storm.dll"};
  void *base{};
  void *patch{};
  if (!storm.loaded() ||
      !storm.open_archive(root / L"StarDat.mpq", &base, 1000U) ||
      !storm.open_archive(root / L"patch_rt.mpq", &patch, 2000U)) {
    if (patch != nullptr) {
      (void)storm.close_archive(patch);
    }
    if (base != nullptr) {
      (void)storm.close_archive(base);
    }
    return false;
  }

  const auto layout = [&](const GameDialogScreen screen,
                          const char *const path) {
    return load_layout(storm, path, dialog.layouts[screen_index(screen)]);
  };
  std::vector<std::uint8_t> tile_group;
  std::uint16_t tile_width{};
  std::uint16_t tile_height{};
  bool loaded =
      load_layout(storm, R"(rez\stat_f10.bin)", dialog.hud_menu_controls) &&
      layout(GameDialogScreen::game_menu, R"(rez\gamemenu.bin)") &&
      layout(GameDialogScreen::options, R"(rez\options.bin)") &&
      layout(GameDialogScreen::sound_options, R"(rez\snd_dlg.bin)") &&
      layout(GameDialogScreen::speed_options, R"(rez\spd_dlg.bin)") &&
      layout(GameDialogScreen::voice_options, R"(rez\voice.bin)") &&
      layout(GameDialogScreen::help_menu, R"(rez\helpmenu.bin)") &&
      layout(GameDialogScreen::keystroke_help, R"(rez\help.bin)") &&
      layout(GameDialogScreen::tips, R"(rez\tips_dlg.bin)") &&
      layout(GameDialogScreen::objectives, R"(rez\objctdlg.bin)") &&
      layout(GameDialogScreen::abort_menu, R"(rez\abrtmenu.bin)") &&
      layout(GameDialogScreen::confirm_restart, R"(rez\restart.bin)") &&
      layout(GameDialogScreen::confirm_quit_menu, R"(rez\quit2mnu.bin)") &&
      layout(GameDialogScreen::confirm_quit_program, R"(rez\quit.bin)") &&
      layout(GameDialogScreen::victory, R"(rez\wmission.bin)") &&
      layout(GameDialogScreen::defeat, R"(rez\lmission.bin)") &&
      load_layout(storm, R"(rez\gluScore.bin)", dialog.score_controls) &&
      storm.load_file(R"(dlgs\tile.grp)", tile_group) &&
      decode_preview_frames(tile_group, status.hud_palette,
                            dialog.dialog_tile_frames, tile_width,
                            tile_height) &&
      dialog.dialog_tile_frames.size() == 9U &&
      storm.load_file(R"(rez\help_txt.tbl)", dialog.help_text_table) &&
      storm.load_file(R"(rez\tips.tbl)", dialog.tips_table) &&
      storm.load_file(R"(rez\gluAll.tbl)", dialog.glue_text_table);

  constexpr std::array<const char *, 6> score_themes{{
      R"(glue\scoreZd\)", R"(glue\scoreZv\)", R"(glue\scoreTd\)",
      R"(glue\scoreTv\)", R"(glue\scorePd\)", R"(glue\scorePv\)",
  }};
  for (std::size_t index = 0; loaded && index < score_themes.size(); ++index) {
    loaded = load_pcx(storm, std::string{score_themes[index]} + "pMain.pcx",
                      dialog.score_backgrounds[index]) &&
             load_pcx(storm,
                      std::string{score_themes[index]} + "scorebox.pcx",
                      dialog.score_boxes[index]);
  }

  const bool closed = storm.close_archive(patch) && storm.close_archive(base);
  if (!dialog.hud_menu_controls.empty()) {
    for (GlueControl &control : dialog.hud_menu_controls) {
      if (control.identifier == 1 && control.text.find("MENU") != std::string::npos) {
        control.text = "MENU";
      }
    }
  }
  dialog.assets_ready = loaded && closed;
  return dialog.assets_ready;
}

bool game_dialog_active(const RecoveryWindowState &state) noexcept {
  return state.game_dialog.assets_ready &&
         state.game_dialog.screen != GameDialogScreen::none;
}

void open_game_menu(RecoveryWindowState &state) noexcept {
  if (!state.game_dialog.assets_ready ||
      (!state.game_dialog.match_active && !state.game_dialog.observer_mode) ||
      state.game_dialog.screen == GameDialogScreen::victory ||
      state.game_dialog.screen == GameDialogScreen::defeat ||
      state.game_dialog.screen == GameDialogScreen::score) {
    return;
  }
  enter_dialog(state.game_dialog, GameDialogScreen::game_menu);
}

GameDialogAction game_dialog_mouse_move(RecoveryWindowState &state, const int x,
                                        const int y) noexcept {
  if (!game_dialog_active(state)) {
    return GameDialogAction::none;
  }
  const std::int16_t previous = state.game_dialog.hovered_control;
  state.game_dialog.hovered_control = control_at(state.game_dialog, x, y);
  return previous == state.game_dialog.hovered_control
             ? GameDialogAction::none
             : GameDialogAction::redraw;
}

GameDialogAction game_dialog_left_down(RecoveryWindowState &state, const int x,
                                       const int y) noexcept {
  if (!game_dialog_active(state)) {
    return GameDialogAction::none;
  }
  state.game_dialog.pressed_control = control_at(state.game_dialog, x, y);
  return state.game_dialog.pressed_control == -32768
             ? GameDialogAction::none
             : GameDialogAction::redraw;
}

GameDialogAction game_dialog_left_up(RecoveryWindowState &state, const int x,
                                     const int y,
                                     const std::uint32_t now) noexcept {
  if (!game_dialog_active(state)) {
    return GameDialogAction::none;
  }
  const std::int16_t released = control_at(state.game_dialog, x, y);
  const std::int16_t pressed = state.game_dialog.pressed_control;
  state.game_dialog.pressed_control = -1;
  if (pressed == -32768 || pressed != released) {
    return GameDialogAction::redraw;
  }
  return activate_control(state, released, x, now);
}

GameDialogAction game_dialog_key_down(RecoveryWindowState &state,
                                      const WPARAM key,
                                      const std::uint32_t now) noexcept {
  GameDialogRuntime &dialog = state.game_dialog;
  if (key == VK_F10) {
    if (dialog.screen == GameDialogScreen::none) {
      open_game_menu(state);
    } else if (dialog.screen != GameDialogScreen::victory &&
               dialog.screen != GameDialogScreen::defeat &&
               dialog.screen != GameDialogScreen::score) {
      enter_dialog(dialog, GameDialogScreen::none);
    }
    return GameDialogAction::redraw;
  }
  if (!game_dialog_active(state)) {
    return GameDialogAction::none;
  }
  if (dialog.screen == GameDialogScreen::keystroke_help) {
    constexpr std::size_t visible_lines = 16U;
    const std::size_t count = table_count(dialog.help_text_table);
    const std::size_t maximum = count > visible_lines
                                    ? count - visible_lines
                                    : 0U;
    if (key == VK_DOWN || key == VK_NEXT) {
      dialog.help_scroll = (std::min)(
          maximum, dialog.help_scroll + (key == VK_NEXT ? visible_lines : 1U));
      return GameDialogAction::redraw;
    }
    if (key == VK_UP || key == VK_PRIOR) {
      const std::size_t amount = key == VK_PRIOR ? visible_lines : 1U;
      dialog.help_scroll = dialog.help_scroll > amount
                               ? dialog.help_scroll - amount
                               : 0U;
      return GameDialogAction::redraw;
    }
  }
  if (key == VK_ESCAPE) {
    switch (dialog.screen) {
    case GameDialogScreen::game_menu:
      return activate_control(state, -3, 0, now);
    case GameDialogScreen::options:
    case GameDialogScreen::help_menu:
    case GameDialogScreen::objectives:
    case GameDialogScreen::abort_menu:
      return activate_control(state, -3, 0, now);
    case GameDialogScreen::confirm_restart:
    case GameDialogScreen::confirm_quit_menu:
    case GameDialogScreen::confirm_quit_program:
      return activate_control(state, -3, 0, now);
    case GameDialogScreen::sound_options:
    case GameDialogScreen::speed_options:
    case GameDialogScreen::voice_options:
    case GameDialogScreen::keystroke_help:
    case GameDialogScreen::tips:
      return activate_control(state, -3, 0, now);
    case GameDialogScreen::victory:
    case GameDialogScreen::defeat:
      return activate_control(state, -2, 0, now);
    case GameDialogScreen::score:
      return GameDialogAction::return_to_menu;
    default:
      break;
    }
  }
  if (key == VK_RETURN && dialog.screen == GameDialogScreen::score) {
    return GameDialogAction::return_to_menu;
  }
  return GameDialogAction::none;
}

void draw_game_dialog_gl(const RecoveryWindowState &state,
                         const std::uint32_t now) noexcept {
  if (!game_dialog_active(state)) {
    return;
  }
  if (state.game_dialog.screen == GameDialogScreen::score) {
    draw_score_screen_gl(state, now);
    return;
  }
  const auto &controls = current_layout(state.game_dialog);
  int left{};
  int top{};
  int right{};
  int bottom{};
  dialog_bounds(controls, left, top, right, bottom);
  draw_dialog_tiles(state.game_dialog, left, top, right, bottom);
  draw_dialog_controls(state);
}

} // namespace starcraft::recovery
