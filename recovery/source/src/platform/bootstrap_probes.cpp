#include "bootstrap_runtime.hpp"

#include "starcraft/game/image.hpp"
#include "starcraft/lang/count.hpp"
#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_harvest.hpp"
#include "starcraft/lang/cunit_path_collide.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/flingy.hpp"
#include "starcraft/lang/pathfinding.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery {

int run_bootstrap_probes(const char *const command_line, const HWND window,
                         RecoveryWindowState &window_state,
                         BootstrapStatus &status, bool &handled) {
  handled = false;
  const bool game_flow_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-game-flow") != nullptr;
  const bool glue_probe =
      command_line != nullptr &&
      (std::strstr(command_line, "--probe-glue") != nullptr ||
       game_flow_probe);
  const bool opengl_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-opengl") != nullptr;
  const bool selection_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-selection") != nullptr;
  const bool command_panel_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-command-panel") != nullptr;
  const bool worker_build_cards_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-worker-build-cards") != nullptr;
  const bool race_construction_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-race-construction") != nullptr;
  const bool race_building_cards_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-race-building-cards") != nullptr;
  const bool status_panel_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-status-panel") != nullptr;
  const bool multi_status_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-multi-status") != nullptr;
  const bool production_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-production") != nullptr;
  const bool all_production_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-all-production") != nullptr;
  const bool harvest_queue_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-harvest-queue") != nullptr;
  const bool geyser_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-geysers") != nullptr;
  const bool building_working_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-building-working") != nullptr;
  const bool resource_feedback_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-resource-feedback") != nullptr;
  const bool resource_strip_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-resource-strip") != nullptr;
  const bool unit_audio_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-unit-audio") != nullptr;
  const bool music_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-music") != nullptr;
  const bool movement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-movement") != nullptr;
  const bool drone_movement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-drone-movement") != nullptr;
  const bool pathfinding_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-pathfinding") != nullptr;
  const bool unit_avoidance_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-unit-avoidance") != nullptr;
  const bool portrait_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-portrait") != nullptr;
  const bool worker_actions_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-worker-actions") != nullptr;
  const bool harvest_visual_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-harvest-visual") != nullptr;
  const bool smart_orders_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-smart-orders") != nullptr;
  const bool refinery_placement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-refinery-placement") != nullptr;
  const bool minimap_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-minimap") != nullptr;
  const bool command_target_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-command-targets") != nullptr;
  const bool building_placement_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-building-placement") != nullptr;
  const bool construction_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-construction") != nullptr;
  const bool construction_status_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-status-construction") != nullptr;
  const bool addon_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-addon") != nullptr;
  const bool creep_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-creep") != nullptr;
  const bool melee_start_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-melee-start") != nullptr;
  const bool camera_probe =
      command_line != nullptr &&
      std::strstr(command_line, "--probe-camera") != nullptr;
  constexpr char capture_option[] = "--capture-opengl=";
  const char *const capture_argument =
      command_line == nullptr ? nullptr
                              : std::strstr(command_line, capture_option);
  const char *const capture_path =
      capture_argument == nullptr
          ? nullptr
          : capture_argument + sizeof(capture_option) - 1U;
  constexpr char lobby_capture_option[] = "--capture-glue-lobby=";
  const char *const lobby_capture_argument =
      command_line == nullptr
          ? nullptr
          : std::strstr(command_line, lobby_capture_option);
  const char *const lobby_capture_path =
      lobby_capture_argument == nullptr
          ? nullptr
          : lobby_capture_argument + sizeof(lobby_capture_option) - 1U;
  constexpr char game_capture_option[] = "--capture-glue-game=";
  const char *const game_capture_argument =
      command_line == nullptr
          ? nullptr
          : std::strstr(command_line, game_capture_option);
  const char *const game_capture_path =
      game_capture_argument == nullptr
          ? nullptr
          : game_capture_argument + sizeof(game_capture_option) - 1U;

  if (glue_probe || opengl_probe || selection_probe || command_panel_probe ||
      worker_build_cards_probe ||
      race_construction_probe || race_building_cards_probe ||
      production_probe || all_production_probe || harvest_queue_probe ||
      geyser_probe || building_working_probe || resource_feedback_probe ||
      resource_strip_probe || unit_audio_probe || music_probe ||
      movement_probe || pathfinding_probe || portrait_probe ||
      command_target_probe || worker_actions_probe || harvest_visual_probe ||
      smart_orders_probe || refinery_placement_probe ||
      building_placement_probe || construction_probe || addon_probe ||
      creep_probe || melee_start_probe || unit_avoidance_probe ||
      minimap_probe || camera_probe || status_panel_probe ||
      multi_status_probe || construction_status_probe ||
      capture_path != nullptr || lobby_capture_path != nullptr ||
      game_capture_path != nullptr) {
    handled = true;
    if (glue_probe) {
      const auto has_control = [](const std::vector<GlueControl> &controls,
                                  const std::int16_t identifier,
                                  const char *const text) {
        return std::any_of(
            controls.begin(), controls.end(),
            [identifier, text](const GlueControl &control) {
              return control.identifier == identifier &&
                     control.text == text;
            });
      };
      const bool layouts_valid =
          window_state.glue.assets_ready &&
          has_control(window_state.glue.main_controls, 2, "Exit") &&
          has_control(window_state.glue.main_controls, 3, "Single Player") &&
          has_control(window_state.glue.main_controls, 4, "Multiplayer") &&
          has_control(window_state.glue.main_controls, 5,
                      "Campaign Editor") &&
          has_control(window_state.glue.connection_controls, -1,
                      "Select Connection") &&
          has_control(window_state.glue.connection_controls, 9, "Ok") &&
          has_control(window_state.glue.connection_controls, 10, "Cancel") &&
          has_control(window_state.glue.lobby_controls, 6, "Ok") &&
          has_control(window_state.glue.lobby_controls, 7, "Cancel") &&
          !window_state.glue.maps.empty();
      const bool title_rendered =
          layouts_valid && render_opengl(window, window_state);
      const GlueAction title_action =
          glue_key_down(window_state.glue, VK_RETURN, GetTickCount());
      const bool main_rendered =
          title_action == GlueAction::redraw &&
          window_state.glue.screen == GlueScreen::main_menu &&
          render_opengl(window, window_state);
      const bool main_captured =
          capture_path == nullptr ||
          (main_rendered &&
           capture_opengl_bmp(window, window_state, capture_path));
      const GlueAction multiplayer_action = activate_main_menu_control(
          window_state.glue, 4, GetTickCount());
      const bool connection_rendered =
          multiplayer_action == GlueAction::redraw &&
          window_state.glue.screen == GlueScreen::connection &&
          render_opengl(window, window_state);
      const GlueAction skirmish_action = activate_connection_control(
          window_state.glue, 9, 0, 0, GetTickCount());
      constexpr char map_index_option[] = "--probe-map-index=";
      const char *const map_index_argument =
          command_line == nullptr
              ? nullptr
              : std::strstr(command_line, map_index_option);
      const std::size_t requested_map =
          map_index_argument == nullptr
              ? 0U
              : static_cast<std::size_t>(std::strtoul(
                    map_index_argument + sizeof(map_index_option) - 1U,
                    nullptr, 10));
      if (requested_map >= window_state.glue.maps.size()) {
        DestroyWindow(window);
        return 13;
      }
      window_state.glue.selected_map = requested_map;
      const std::string selected_map_path =
          window_state.glue.maps[window_state.glue.selected_map].path;
      const bool map_rendered =
          skirmish_action == GlueAction::redraw &&
          window_state.glue.screen == GlueScreen::map_selection &&
          render_opengl(window, window_state);
      const GlueAction map_action = activate_map_selection_control(
          window_state.glue, 6, 0, 0, GetTickCount());
      constexpr char local_race_option[] = "--probe-local-race=";
      const char *const local_race_argument =
          command_line == nullptr
              ? nullptr
              : std::strstr(command_line, local_race_option);
      const std::uint8_t requested_race =
          local_race_argument == nullptr
              ? window_state.glue.lobby_slots[0].race
              : static_cast<std::uint8_t>(std::strtoul(
                    local_race_argument + sizeof(local_race_option) - 1U,
                    nullptr, 10));
      if (requested_race >= 3U) {
        DestroyWindow(window);
        return 14;
      }
      window_state.glue.lobby_slots[0].race = requested_race;
      const bool lobby_rendered = map_action == GlueAction::redraw &&
                                  window_state.glue.screen == GlueScreen::lobby &&
                                  render_opengl(window, window_state);
      const bool lobby_captured =
          lobby_capture_path == nullptr ||
          (lobby_rendered && capture_opengl_bmp(
                                 window, window_state, lobby_capture_path));
      const std::uint32_t ready_now = GetTickCount();
      const GlueAction lobby_action = activate_lobby_control(
          window_state.glue, 6, 0, 0, ready_now);
      const bool ready_rendered = lobby_action == GlueAction::redraw &&
                                  window_state.glue.screen == GlueScreen::ready &&
                                  render_opengl(window, window_state);
      const GlueAction ready_action =
          advance_glue(window_state.glue, ready_now + 3001U);
      const bool flow_valid = ready_action == GlueAction::start_game &&
                              start_selected_glue_map(window_state) &&
                              window_state.glue.screen == GlueScreen::gameplay &&
                              status.assets_ready &&
                              status.map_name == selected_map_path &&
                              status.local_race == requested_race;
      bool glue_worker_card_verified = !worker_build_cards_probe;
      if (flow_valid && worker_build_cards_probe) {
        starcraft::lang::MeleeUnitTypes local_types{};
        const bool types_ready =
            starcraft::lang::melee_unit_types(requested_race, local_types);
        std::size_t matching_workers{};
        std::size_t matching_bases{};
        std::size_t mismatched_start_units{};
        for (const ScenarioUnitPreview &unit : status.units) {
          if (!unit.alive || unit.owner != 0U ||
              !starcraft::lang::is_melee_starting_unit_type(unit.unit_type)) {
            continue;
          }
          matching_workers += unit.unit_type == local_types.worker ? 1U : 0U;
          matching_bases += unit.unit_type == local_types.base ? 1U : 0U;
          mismatched_start_units +=
              unit.unit_type != local_types.worker &&
                      unit.unit_type != local_types.base
                  ? 1U
                  : 0U;
        }
        const auto worker =
            types_ready
                ? std::find_if(status.units.begin(), status.units.end(),
                               [&](const ScenarioUnitPreview &unit) {
                                 return unit.alive && unit.owner == 0U &&
                                        unit.unit_type == local_types.worker;
                               })
                : status.units.end();
        const auto base =
            types_ready
                ? std::find_if(status.units.begin(), status.units.end(),
                               [&](const ScenarioUnitPreview &unit) {
                                 return unit.alive && unit.owner == 0U &&
                                        unit.unit_type == local_types.base;
                               })
                : status.units.end();
        if (worker != status.units.end() && base != status.units.end()) {
          clear_selection(status);
          worker->selected = true;
          const CommandCardView card = command_card_for(status);
          const std::uint16_t expected_icon =
              requested_race == 0U ? 257U
              : requested_race == 1U ? 234U
                                     : 272U;
          const std::uint16_t expected_argument =
              requested_race == 0U ? 236U
              : requested_race == 1U ? 237U
                                     : 238U;
          const bool worker_card_matches = std::any_of(
              card.buttons, card.buttons + card.count,
              [=](const CommandButtonVisual &button) {
                return button.position == 7U &&
                       button.icon == expected_icon &&
                       button.argument == expected_argument &&
                       button.action ==
                           CommandButtonVisual::Action::open_card;
              });

          clear_selection(status);
          base->selected = true;
          constexpr std::array<std::string_view, 3> expected_base_names{{
              "Zerg Hatchery",
              "Terran Command Center",
              "Protoss Nexus",
          }};
          const std::string base_name = printable_status_text(status_text(
              status, static_cast<std::uint16_t>(local_types.base + 1U)));
          const UnitPortraitAsset *const portrait = selected_portrait(status);
          const CommandCardView base_card = command_card_for(status);
          const bool terran_scv_card =
              base_card.count != 0U &&
              std::any_of(base_card.buttons,
                          base_card.buttons + base_card.count,
                          [](const CommandButtonVisual &button) {
                            return button.action ==
                                       CommandButtonVisual::Action::train_unit &&
                                   button.argument == 7U;
                          });
          glue_worker_card_verified =
              worker_card_matches && matching_workers == 4U &&
              matching_bases == 1U && mismatched_start_units == 0U &&
              base_name == expected_base_names[requested_race] &&
              local_types.base < status.wireframe_frames.size() &&
              !status.wireframe_frames[local_types.base].bgra.empty() &&
              portrait != nullptr && portrait->unit_type == local_types.base &&
              (requested_race == 1U ? terran_scv_card : !terran_scv_card);
        }
      }
      bool glue_drone_movement_verified = !drone_movement_probe;
      int glue_drone_movement_stage{};
      if (flow_valid && drone_movement_probe) {
        const auto worker = std::find_if(
            status.units.begin(), status.units.end(),
            [](const ScenarioUnitPreview &unit) {
              return unit.alive && unit.owner == 0U &&
                     unit.unit_type == starcraft::lang::zerg_drone_type;
            });
        if (worker != status.units.end()) {
          glue_drone_movement_stage = 1;
          const std::uint32_t worker_id = worker->unit_id;
          const std::uint16_t initial_x = worker->x;
          const std::uint16_t initial_y = worker->y;
          if (worker->movement_top_speed != 0U &&
              worker->movement_acceleration != 0U &&
              worker->movement_turn_speed != 0U) {
            glue_drone_movement_stage = 2;
            std::vector<starcraft::lang::PathObstacle> obstacles;
            starcraft::lang::PathPoint target{};
            const bool obstacles_ready =
                collect_building_obstacles(status, &*worker, obstacles);
            constexpr std::array<std::array<int, 2>, 8> offsets{{
                {{128, 0}}, {{-128, 0}}, {{0, 128}}, {{0, -128}},
                {{96, 96}}, {{-96, 96}}, {{96, -96}}, {{-96, -96}},
            }};
            for (const auto &offset : offsets) {
              const int candidate_x = static_cast<int>(worker->x) + offset[0];
              const int candidate_y = static_cast<int>(worker->y) + offset[1];
              std::vector<starcraft::lang::PathPoint> path;
              const bool target_hits_unit = std::any_of(
                  status.units.begin(), status.units.end(),
                  [&](const ScenarioUnitPreview &unit) {
                    if (!unit.alive || unit.unit_id == worker_id) {
                      return false;
                    }
                    const int half_width =
                        static_cast<int>(unit.selection_width) / 2;
                    const int half_height =
                        static_cast<int>(unit.selection_height) / 2;
                    return candidate_x >= static_cast<int>(unit.x) - half_width &&
                           candidate_x <= static_cast<int>(unit.x) + half_width &&
                           candidate_y >= static_cast<int>(unit.y) - half_height &&
                           candidate_y <= static_cast<int>(unit.y) + half_height;
                  });
              if (!obstacles_ready ||
                  target_hits_unit ||
                  !creation_position_passable(status, *worker, candidate_x,
                                              candidate_y) ||
                  !starcraft::lang::find_unit_path(
                      status.pathing_map, worker->x, worker->y, candidate_x,
                      candidate_y, worker->selection_width,
                      worker->selection_height, obstacles, path) ||
                  path.empty()) {
                continue;
              }
              target = {static_cast<std::uint16_t>(candidate_x),
                        static_cast<std::uint16_t>(candidate_y)};
              break;
            }
            if (target.x != 0U || target.y != 0U) {
              glue_drone_movement_stage = 3;
              clear_selection(status);
              ScenarioUnitPreview *selected = find_unit_by_id(status, worker_id);
              if (selected != nullptr) {
                selected->selected = true;
                status.placement_active = false;
                status.active_command_card = 0U;
                cancel_command_target(status);
                status.last_issued_order = 0xFFU;
                (void)set_camera_position(
                    status,
                    (static_cast<int>(initial_x) + target.x) / 2 -
                        kMapViewportWidth / 2,
                    (static_cast<int>(initial_y) + target.y) / 2 - 160);
                RECT client{};
                const int game_x = static_cast<int>(target.x) - status.camera_x;
                const int game_y = static_cast<int>(target.y) - status.camera_y;
                if (GetClientRect(window, &client) && client.right > 0 &&
                    client.bottom > 0 && game_x >= 0 &&
                    game_x < kMapViewportWidth && game_y >= 0 &&
                    game_y < kMapViewportHeight) {
                  const LPARAM click = MAKELPARAM(
                      game_x * client.right / kMapViewportWidth,
                      game_y * client.bottom / kMapViewportHeight);
                  SendMessageA(window, WM_RBUTTONDOWN, MK_RBUTTON, click);
                  selected = find_unit_by_id(status, worker_id);
                  if (status.last_issued_order == 7U) {
                    glue_drone_movement_stage = 4;
                  }
                  if (selected != nullptr && selected->moving &&
                      selected->active_order == ActiveUnitOrder::move) {
                    glue_drone_movement_stage = 5;
                    for (int tick = 0; tick < 64; ++tick) {
                      SendMessageA(window, WM_TIMER, 1, 0);
                    }
                    selected = find_unit_by_id(status, worker_id);
                    if (selected != nullptr) {
                      const int distance =
                          std::abs(static_cast<int>(selected->x) - initial_x) +
                          std::abs(static_cast<int>(selected->y) - initial_y);
                      glue_drone_movement_verified = distance >= 8;
                      glue_drone_movement_stage =
                          glue_drone_movement_verified
                              ? 6
                              : glue_drone_movement_stage;
                    }
                  }
                }
              }
            }
          }
        }
      }
      const bool selected_game_rendered =
          flow_valid && glue_worker_card_verified &&
          glue_drone_movement_verified &&
          render_opengl(window, window_state);
      bool game_flow_verified = !game_flow_probe;
      int game_flow_stage{};
      if (flow_valid && game_flow_probe &&
          window_state.game_dialog.assets_ready &&
          window_state.game_dialog.match_active) {
        const auto activate = [&](const std::int16_t identifier) {
          const auto &dialog = window_state.game_dialog;
          const auto &controls =
              dialog.screen == GameDialogScreen::score
                  ? dialog.score_controls
                  : dialog.layouts[static_cast<std::size_t>(dialog.screen)];
          const auto control = std::find_if(
              controls.begin(), controls.end(),
              [identifier](const GlueControl &value) {
                return value.identifier == identifier;
              });
          if (control == controls.end()) {
            return GameDialogAction::none;
          }
          const int x = (control->left + control->right) / 2;
          const int y = static_cast<int>(
              ((control->top + control->bottom) / 2) * hud_vertical_scale());
          (void)game_dialog_left_down(window_state, x, y);
          return game_dialog_left_up(window_state, x, y, GetTickCount());
        };

        bool hud_button_verified{};
        const auto menu_button = std::find_if(
            window_state.game_dialog.hud_menu_controls.begin(),
            window_state.game_dialog.hud_menu_controls.end(),
            [](const GlueControl &control) {
              return control.identifier == 1 && control.text == "MENU";
            });
        if (menu_button != window_state.game_dialog.hud_menu_controls.end()) {
          const int menu_x = (menu_button->left + menu_button->right) / 2;
          const int menu_y = static_cast<int>(
              ((menu_button->top + menu_button->bottom) / 2) *
              hud_vertical_scale());
          if (hud_menu_button_at(window_state, menu_x, menu_y)) {
            open_game_menu(window_state);
            hud_button_verified =
                window_state.game_dialog.screen ==
                GameDialogScreen::game_menu;
            (void)game_dialog_key_down(window_state, VK_F10, GetTickCount());
          }
        }
        (void)game_dialog_key_down(window_state, VK_F10, GetTickCount());
        if (hud_button_verified &&
            window_state.game_dialog.screen == GameDialogScreen::game_menu &&
            render_opengl(window, window_state)) {
          game_flow_stage = 1;
          (void)activate(3);
          if (window_state.game_dialog.screen == GameDialogScreen::options) {
            (void)activate(1);
            if (window_state.game_dialog.screen ==
                GameDialogScreen::sound_options) {
              (void)activate(-3);
              (void)activate(-3);
              (void)activate(4);
              (void)activate(2);
              if (window_state.game_dialog.screen == GameDialogScreen::tips) {
                const std::size_t old_tip = window_state.game_dialog.tip_index;
                (void)activate(1);
                if (window_state.game_dialog.tip_index != old_tip) {
                  game_flow_stage = 2;
                  (void)activate(-2);
                  (void)activate(-3);
                  (void)activate(5);
                  if (window_state.game_dialog.screen ==
                      GameDialogScreen::objectives) {
                    (void)activate(-3);
                    (void)activate(6);
                    (void)activate(2);
                    if (window_state.game_dialog.screen ==
                        GameDialogScreen::confirm_quit_menu) {
                      game_flow_stage = 3;
                      (void)activate(-3);
                      (void)activate(-3);
                      (void)activate(-3);
                    }
                  }
                }
              }
            }
          }
        }
        if (game_flow_stage == 3 &&
            window_state.game_dialog.screen == GameDialogScreen::none) {
          for (ScenarioUnitPreview &unit : status.units) {
            if (unit.owner > 0U && unit.owner < 8U && unit.is_building) {
              unit.destroyed_by_owner = 0U;
              unit.alive = false;
            }
          }
          evaluate_melee_outcome(window_state);
          if (window_state.game_dialog.screen == GameDialogScreen::victory &&
              render_opengl(window, window_state)) {
            game_flow_stage = 4;
            (void)activate(-2);
            if (window_state.game_dialog.screen == GameDialogScreen::score) {
              game_flow_stage = 5;
            }
            if (game_flow_stage == 5 &&
                !window_state.game_dialog.score_rows.empty()) {
              const auto local_score = std::find_if(
                  window_state.game_dialog.score_rows.begin(),
                  window_state.game_dialog.score_rows.end(),
                  [](const MatchScoreRow &row) {
                    return row.player == 0U && row.razed != 0U;
                  });
              if (local_score != window_state.game_dialog.score_rows.end()) {
                game_flow_stage = 6;
              }
            }
            if (game_flow_stage == 6 && render_opengl(window, window_state)) {
              game_flow_stage = 7;
              for (std::int16_t category = 3; category <= 6; ++category) {
                (void)activate(category);
              }
              const bool categories_valid =
                  window_state.game_dialog.score_category ==
                      ScoreCategory::resources &&
                  render_opengl(window, window_state);
              window_state.game_dialog.screen = GameDialogScreen::none;
              if (categories_valid && start_selected_glue_map(window_state)) {
                for (ScenarioUnitPreview &unit : status.units) {
                  if (unit.owner == 0U && unit.is_building) {
                    unit.destroyed_by_owner = 1U;
                    unit.alive = false;
                  }
                }
                evaluate_melee_outcome(window_state);
                if (window_state.game_dialog.screen ==
                        GameDialogScreen::defeat &&
                    render_opengl(window, window_state)) {
                  game_flow_stage = 8;
                  (void)activate(-2);
                  const bool defeat_scores_valid = std::any_of(
                      window_state.game_dialog.score_rows.begin(),
                      window_state.game_dialog.score_rows.end(),
                      [](const MatchScoreRow &row) {
                        return row.player == 0U &&
                               row.structures_lost != 0U;
                      });
                  const GameDialogAction finished =
                      defeat_scores_valid ? activate(7)
                                          : GameDialogAction::none;
                  game_flow_verified =
                      window_state.game_dialog.screen ==
                          GameDialogScreen::score &&
                      finished == GameDialogAction::return_to_menu;
                  game_flow_stage = game_flow_verified ? 9 : game_flow_stage;
                }
              }
            }
          }
        }
      }
      const bool selected_game_captured =
          game_capture_path == nullptr ||
          (selected_game_rendered &&
           capture_opengl_bmp(window, window_state, game_capture_path));
      if (!flow_valid && lobby_capture_path != nullptr) {
        (void)render_opengl(window, window_state);
        (void)capture_opengl_bmp(window, window_state, lobby_capture_path);
      }
      int glue_failure_code = 12;
      if (!flow_valid) {
        if (window_state.glue.message.find("console PCX") !=
            std::string::npos) {
          glue_failure_code = 20;
        } else if (window_state.glue.message.find("wireframe") !=
                   std::string::npos) {
          glue_failure_code = 21;
        } else if (window_state.glue.message.find("status panel asset") !=
                   std::string::npos) {
          glue_failure_code = 22;
        } else if (window_state.glue.message.find("advisor sound") !=
                   std::string::npos) {
          glue_failure_code = 23;
        } else if (window_state.glue.message.find("response sound") !=
                   std::string::npos) {
          glue_failure_code = 24;
        } else if (window_state.glue.message.find("music track") !=
                   std::string::npos) {
          glue_failure_code = 25;
        } else if (window_state.glue.message.find("command panel") !=
                   std::string::npos) {
          glue_failure_code = 26;
        } else if (window_state.glue.message.find("unit color table") !=
                   std::string::npos) {
          glue_failure_code = 27;
        }
      } else if (!glue_drone_movement_verified) {
        glue_failure_code = 30 + glue_drone_movement_stage;
      } else if (!game_flow_verified) {
        glue_failure_code = 50 + game_flow_stage;
      }
      DestroyWindow(window);
      return title_rendered && main_rendered && main_captured &&
                     connection_rendered && map_rendered && lobby_rendered &&
                     lobby_captured && ready_rendered &&
                     selected_game_rendered && selected_game_captured &&
                      glue_worker_card_verified &&
                      glue_drone_movement_verified && game_flow_verified
                 ? 0
                 : glue_failure_code;
    }
    window_state.glue.screen = GlueScreen::gameplay;
    bool selection_verified = true;
    if (selection_probe) {
      selection_verified = false;
      const auto visible =
          std::find_if(status.units.begin(), status.units.end(),
                       [&status](const ScenarioUnitPreview &unit) {
                         const int game_x = unit.x - status.camera_x;
                         const int game_y = unit.y - status.camera_y;
                         return game_x >= 0 && game_y >= 0 &&
                                game_x < kMapViewportWidth &&
                                game_y < kMapViewportHeight;
                       });
      if (visible != status.units.end()) {
        RECT client{};
        const ScenarioUnitPreview &unit = *visible;
        const int game_x = unit.x - status.camera_x;
        const int game_y = unit.y - status.camera_y;
        if (GetClientRect(window, &client) && client.right > 0 &&
            client.bottom > 0 && game_x >= 0 && game_y >= 0 &&
            game_x < kMapViewportWidth && game_y < kMapViewportHeight) {
          const int client_x = game_x * client.right / kMapViewportWidth;
          const int client_y = game_y * client.bottom / kMapViewportHeight;
          const LPARAM click = MAKELPARAM(client_x, client_y);
          SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
          SendMessageA(window, WM_LBUTTONUP, 0, click);
          selection_verified =
              selection_count(status) == 1 && visible->selected;
        }
      }
    }
    bool camera_verified = true;
    if (camera_probe) {
      const std::uint16_t initial_x = status.camera_x;
      const std::uint16_t initial_y = status.camera_y;
      const int maximum_x =
          static_cast<int>(status.scenario_width) * 32 - kMapViewportWidth;
      const WPARAM key = initial_x < maximum_x ? VK_RIGHT : VK_LEFT;
      SendMessageA(window, WM_KEYDOWN, key, 0);
      for (int tick = 0; tick < 7; ++tick) {
        SendMessageA(window, WM_TIMER, 1, 0);
      }
      SendMessageA(window, WM_KEYUP, key, 0);
      camera_verified = status.terrain_ready && status.camera_y == initial_y &&
                        status.camera_x != initial_x &&
                        (status.camera_x & 7U) == 0;
    }
    RECT probe_client{};
    const bool probe_client_ready = GetClientRect(window, &probe_client) &&
                                    probe_client.right > 0 &&
                                    probe_client.bottom > 0;
    const auto client_point = [&](const int game_x,
                                  const int game_y) -> LPARAM {
      return MAKELPARAM(game_x * probe_client.right / kMapViewportWidth,
                        game_y * probe_client.bottom / kMapViewportHeight);
    };
    const auto click_command = [&](const std::uint16_t position) -> bool {
      if (!probe_client_ready || position == 0 ||
          position > status.command_controls.size()) {
        return false;
      }
      const CommandControl &control = status.command_controls[position - 1U];
      const int game_x = (control.left + control.right) / 2;
      const int game_y = ((control.top + control.bottom) / 2) * 5 / 6;
      const LPARAM click = client_point(game_x, game_y);
      SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
      SendMessageA(window, WM_LBUTTONUP, 0, click);
      return true;
    };

    bool worker_build_cards_verified = true;
    int worker_build_cards_probe_stage{};
    if (worker_build_cards_probe) {
      worker_build_cards_verified = status.assets_ready;
      const auto card_has = [](const CommandCardView card,
                               const std::uint16_t position,
                               const std::uint16_t icon,
                               const std::uint16_t argument,
                               const CommandButtonVisual::Action action) {
        return std::any_of(
            card.buttons, card.buttons + card.count,
            [=](const CommandButtonVisual &button) {
              return button.position == position && button.icon == icon &&
                     button.argument == argument && button.action == action;
            });
      };
      ScenarioUnitPreview worker{};
      worker.unit_id = status.next_unit_id++;
      worker.owner = 0U;
      worker.selected = true;
      worker_build_cards_verified =
          worker_build_cards_verified &&
          configure_preview_type(status, worker,
                                 starcraft::lang::zerg_drone_type);
      worker_build_cards_probe_stage = worker_build_cards_verified ? 1 : 0;
      if (worker_build_cards_verified) {
        clear_selection(status);
        status.units.push_back(worker);
        status.units.back().selected = true;
        status.active_command_card = 0U;
        const CommandCardView drone = command_card_for(status);
        worker_build_cards_verified =
            drone.count == 7U &&
            card_has(drone, 7U, 257U, 236U,
                     CommandButtonVisual::Action::open_card) &&
            card_has(drone, 8U, 258U, 239U,
                     CommandButtonVisual::Action::open_card);
        activate_command_button(status, 7U);
        const CommandCardView zerg_basic = command_card_for(status);
        worker_build_cards_verified =
            worker_build_cards_verified && status.active_command_card == 236U &&
            zerg_basic.count == 7U &&
            card_has(zerg_basic, 1U, 131U, 131U,
                     CommandButtonVisual::Action::begin_building_placement) &&
            card_has(zerg_basic, 3U, 149U, 149U,
                     CommandButtonVisual::Action::begin_building_placement);
        status.active_command_card = 239U;
        const CommandCardView zerg_advanced = command_card_for(status);
        worker_build_cards_verified =
            worker_build_cards_verified && zerg_advanced.count == 6U &&
            card_has(zerg_advanced, 5U, 136U, 136U,
                     CommandButtonVisual::Action::begin_building_placement);
        worker_build_cards_probe_stage =
            worker_build_cards_verified ? 2 : worker_build_cards_probe_stage;

        ScenarioUnitPreview &same_worker = status.units.back();
        worker_build_cards_verified =
            worker_build_cards_verified &&
            configure_preview_type(status, same_worker,
                                   starcraft::lang::protoss_probe_type);
        status.active_command_card = 0U;
        const CommandCardView probe = command_card_for(status);
        worker_build_cards_verified =
            worker_build_cards_verified && probe.count == 7U &&
            card_has(probe, 7U, 272U, 238U,
                     CommandButtonVisual::Action::open_card) &&
            card_has(probe, 8U, 273U, 241U,
                     CommandButtonVisual::Action::open_card);
        status.active_command_card = 238U;
        const CommandCardView protoss_basic = command_card_for(status);
        worker_build_cards_verified =
            worker_build_cards_verified && protoss_basic.count == 9U &&
            card_has(protoss_basic, 1U, 154U, 154U,
                     CommandButtonVisual::Action::begin_building_placement) &&
            card_has(protoss_basic, 3U, 157U, 157U,
                     CommandButtonVisual::Action::begin_building_placement);
        status.active_command_card = 241U;
        const CommandCardView protoss_advanced = command_card_for(status);
        worker_build_cards_verified =
            worker_build_cards_verified && protoss_advanced.count == 9U &&
            card_has(protoss_advanced, 8U, 170U, 170U,
                     CommandButtonVisual::Action::begin_building_placement);
        worker_build_cards_probe_stage =
            worker_build_cards_verified ? 3 : worker_build_cards_probe_stage;
        status.units.pop_back();
      }
    }

    bool race_building_cards_verified = true;
    int race_building_cards_probe_stage{};
    if (race_building_cards_probe) {
      struct ExpectedBuildingCard {
        std::uint16_t unit_type;
        std::size_t count;
        std::uint16_t sentinel_position;
        std::uint16_t sentinel_icon;
        std::uint16_t sentinel_argument;
        CommandButtonVisual::Action sentinel_action;
      };
      constexpr std::array<ExpectedBuildingCard, 15> zerg_cards{{
          {131U, 2U, 7U, 132U, 132U,
           CommandButtonVisual::Action::morph_building},
          {132U, 5U, 7U, 133U, 133U,
           CommandButtonVisual::Action::morph_building},
          {133U, 4U, 6U, 295U, 26U,
           CommandButtonVisual::Action::upgrade_technology},
          {134U, 1U, 1U, 134U, 134U,
           CommandButtonVisual::Action::place_nydus_exit},
          {135U, 2U, 2U, 268U, 30U,
           CommandButtonVisual::Action::upgrade_technology},
          {136U, 4U, 4U, 294U, 32U,
           CommandButtonVisual::Action::upgrade_technology},
          {137U, 3U, 4U, 44U, 13U,
           CommandButtonVisual::Action::research_technology},
          {138U, 5U, 5U, 311U, 12U,
           CommandButtonVisual::Action::research_technology},
          {139U, 3U, 4U, 296U, 3U,
           CommandButtonVisual::Action::upgrade_technology},
          {140U, 1U, 1U, 269U, 20U,
           CommandButtonVisual::Action::research_technology},
          {141U, 3U, 7U, 137U, 137U,
           CommandButtonVisual::Action::morph_building},
          {142U, 2U, 2U, 264U, 28U,
           CommandButtonVisual::Action::upgrade_technology},
          {143U, 2U, 8U, 146U, 146U,
           CommandButtonVisual::Action::morph_building},
          {144U, 2U, 3U, 230U, 0U,
           CommandButtonVisual::Action::begin_attack_target},
          {146U, 2U, 3U, 230U, 0U,
           CommandButtonVisual::Action::begin_attack_target},
      }};
      constexpr std::array<ExpectedBuildingCard, 13> protoss_cards{{
          {154U, 2U, 1U, 64U, 64U,
           CommandButtonVisual::Action::train_unit},
          {155U, 4U, 3U, 84U, 84U,
           CommandButtonVisual::Action::train_unit},
          {159U, 2U, 2U, 315U, 38U,
           CommandButtonVisual::Action::upgrade_technology},
          {160U, 4U, 3U, 67U, 67U,
           CommandButtonVisual::Action::train_unit},
          {162U, 2U, 3U, 230U, 0U,
           CommandButtonVisual::Action::begin_attack_target},
          {163U, 1U, 4U, 306U, 34U,
           CommandButtonVisual::Action::upgrade_technology},
          {164U, 3U, 4U, 281U, 33U,
           CommandButtonVisual::Action::upgrade_technology},
          {165U, 5U, 6U, 274U, 21U,
           CommandButtonVisual::Action::research_technology},
          {166U, 3U, 5U, 308U, 15U,
           CommandButtonVisual::Action::upgrade_technology},
          {167U, 4U, 3U, 71U, 71U,
           CommandButtonVisual::Action::train_unit},
          {169U, 3U, 3U, 319U, 43U,
           CommandButtonVisual::Action::upgrade_technology},
          {170U, 4U, 4U, 320U, 44U,
           CommandButtonVisual::Action::upgrade_technology},
          {171U, 3U, 3U, 314U, 37U,
           CommandButtonVisual::Action::upgrade_technology},
      }};
      const auto verify_cards = [&](const auto &expected,
                                    const std::size_t expected_total) {
        std::size_t total{};
        for (const ExpectedBuildingCard &entry : expected) {
          ScenarioUnitPreview building{};
          building.unit_id = status.next_unit_id++;
          building.unit_type = entry.unit_type;
          building.owner = 0U;
          building.selected = true;
          building.is_building = true;
          building.construction_complete = true;
          clear_selection(status);
          status.units.push_back(building);
          status.units.back().selected = true;
          status.active_command_card = 0U;
          const CommandCardView card = command_card_for(status);
          const bool sentinel =
              card.count != 0U &&
              std::any_of(card.buttons, card.buttons + card.count,
                          [&](const CommandButtonVisual &button) {
                            return button.position == entry.sentinel_position &&
                                   button.icon == entry.sentinel_icon &&
                                   button.argument == entry.sentinel_argument &&
                                   button.action == entry.sentinel_action;
                          });
          total += card.count;
          status.units.pop_back();
          if (card.count != entry.count || !sentinel) {
            return false;
          }
        }
        return total == expected_total;
      };
      race_building_cards_verified =
          verify_cards(zerg_cards, 41U) &&
          verify_cards(protoss_cards, 40U);
      race_building_cards_probe_stage =
          race_building_cards_verified ? 1 : 0;
      if (race_building_cards_verified) {
        constexpr std::array<std::uint16_t, 4> retail_empty_cards{{
            149U, 156U, 157U, 172U,
        }};
        for (const std::uint16_t unit_type : retail_empty_cards) {
          ScenarioUnitPreview building{};
          building.unit_id = status.next_unit_id++;
          building.unit_type = unit_type;
          building.owner = 0U;
          building.selected = true;
          building.is_building = true;
          building.construction_complete = true;
          clear_selection(status);
          status.units.push_back(building);
          status.units.back().selected = true;
          status.active_command_card = 0U;
          race_building_cards_verified =
              race_building_cards_verified &&
              command_card_for(status).count == 0U;
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 2 : 1;
      }
      if (race_building_cards_verified) {
        status.player_minerals = 100000U;
        status.player_gas = 100000U;
        status.researched_technologies[11U] = false;
        ScenarioUnitPreview hatchery{};
        hatchery.unit_id = status.next_unit_id++;
        hatchery.owner = 0U;
        race_building_cards_verified =
            configure_preview_type(status, hatchery, 131U);
        if (race_building_cards_verified) {
          clear_selection(status);
          hatchery.selected = true;
          const std::uint32_t hatchery_id = hatchery.unit_id;
          status.units.push_back(hatchery);
          const std::uint32_t minerals_before = status.player_minerals;
          const std::uint32_t gas_before = status.player_gas;
          const starcraft::data::TechnologyResearchTraits burrowing =
              status.technology_traits[11U];
          race_building_cards_verified = click_command(1U);
          ScenarioUnitPreview *researching =
              find_unit_by_id(status, hatchery_id);
          race_building_cards_verified =
              researching != nullptr && researching->active_technology == 11U &&
              researching->technology_ticks_remaining != 0U &&
              status.player_minerals ==
                  minerals_before - burrowing.mineral_cost &&
              status.player_gas == gas_before - burrowing.gas_cost &&
              command_card_for(status).count == 1U &&
              command_card_for(status).buttons[0].action ==
                  CommandButtonVisual::Action::cancel_research;
          const std::uint16_t ticks =
              researching == nullptr ? 0U
                                     : researching->technology_ticks_remaining;
          for (std::uint32_t tick = 0;
               race_building_cards_verified && tick <= ticks; ++tick) {
            (void)advance_technology_research(status);
          }
          researching = find_unit_by_id(status, hatchery_id);
          race_building_cards_verified =
              race_building_cards_verified && researching != nullptr &&
              researching->active_technology == 28U &&
              status.researched_technologies[11U] &&
              command_card_for(status).count == 1U &&
              command_card_for(status).buttons[0].action ==
                  CommandButtonVisual::Action::morph_building;
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 3 : 2;
      }
      if (race_building_cards_verified) {
        ScenarioUnitPreview hatchery{};
        hatchery.unit_id = status.next_unit_id++;
        hatchery.owner = 0U;
        race_building_cards_verified =
            configure_preview_type(status, hatchery, 131U);
        if (race_building_cards_verified) {
          clear_selection(status);
          hatchery.selected = true;
          const std::uint32_t hatchery_id = hatchery.unit_id;
          status.units.push_back(hatchery);
          race_building_cards_verified = click_command(7U);
          ScenarioUnitPreview *morphing = find_unit_by_id(status, hatchery_id);
          race_building_cards_verified =
              morphing != nullptr && !morphing->construction_complete &&
              morphing->construction_target_type == 132U &&
              morphing->construction_ticks_remaining != 0U;
          const std::uint16_t ticks =
              morphing == nullptr ? 0U : morphing->construction_ticks_remaining;
          for (std::uint32_t tick = 0;
               race_building_cards_verified && tick <= ticks + 12U; ++tick) {
            (void)advance_zerg_building_construction(status);
          }
          morphing = find_unit_by_id(status, hatchery_id);
          race_building_cards_verified =
              race_building_cards_verified && morphing != nullptr &&
              morphing->unit_type == 132U &&
              morphing->construction_complete &&
              morphing->construction_target_type == 0xFFFFU;
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 4 : 3;
      }
      if (race_building_cards_verified) {
        ScenarioUnitPreview den{};
        den.unit_id = status.next_unit_id++;
        den.owner = 0U;
        status.upgrade_levels[29U] = 0U;
        race_building_cards_verified =
            configure_preview_type(status, den, 135U);
        if (race_building_cards_verified) {
          clear_selection(status);
          den.selected = true;
          const std::uint32_t den_id = den.unit_id;
          status.units.push_back(den);
          const std::uint32_t minerals_before = status.player_minerals;
          const std::uint32_t gas_before = status.player_gas;
          const starcraft::data::UpgradeResearchTraits traits =
              status.upgrade_traits[29U];
          race_building_cards_verified = click_command(1U);
          ScenarioUnitPreview *upgrading = find_unit_by_id(status, den_id);
          race_building_cards_verified =
              upgrading != nullptr && upgrading->active_upgrade == 29U &&
              upgrading->technology_ticks_remaining != 0U;
          race_building_cards_verified =
              race_building_cards_verified && click_command(9U);
          upgrading = find_unit_by_id(status, den_id);
          race_building_cards_verified =
              race_building_cards_verified && upgrading != nullptr &&
              upgrading->active_upgrade == 46U &&
              upgrading->technology_ticks_remaining == 0U &&
              status.player_minerals ==
                  minerals_before - traits.mineral_cost +
                      3U * traits.mineral_cost / 4U &&
              status.player_gas == gas_before - traits.gas_cost +
                                       3U * traits.gas_cost / 4U;
          if (race_building_cards_verified) {
            status.player_minerals = 100000U;
            status.player_gas = 100000U;
            race_building_cards_verified = click_command(1U);
            upgrading = find_unit_by_id(status, den_id);
            const std::uint16_t ticks =
                upgrading == nullptr ? 0U
                                     : upgrading->technology_ticks_remaining;
            for (std::uint32_t tick = 0;
                 race_building_cards_verified && tick <= ticks; ++tick) {
              (void)advance_technology_research(status);
            }
            upgrading = find_unit_by_id(status, den_id);
            race_building_cards_verified =
                race_building_cards_verified && upgrading != nullptr &&
                upgrading->active_upgrade == 46U &&
                status.upgrade_levels[29U] == 1U;
          }
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 5 : 4;
      }
      if (race_building_cards_verified) {
        ScenarioUnitPreview canal{};
        canal.unit_id = status.next_unit_id++;
        canal.owner = 0U;
        race_building_cards_verified =
            configure_preview_type(status, canal, 134U);
        if (race_building_cards_verified) {
          clear_selection(status);
          canal.selected = true;
          const std::uint32_t canal_id = canal.unit_id;
          status.units.push_back(canal);
          race_building_cards_verified = click_command(1U);
          race_building_cards_verified =
              race_building_cards_verified && status.placement_active &&
              status.placement_unit_type == 134U &&
              status.nydus_parent_id == canal_id;
          const BuildableUnitVisual *const nydus =
              find_buildable_unit(status, 134U);
          bool placement_found{};
          if (race_building_cards_verified && nydus != nullptr) {
            for (int y = 160;
                 y + 160 < status.pathing_map.pixel_height() &&
                 !placement_found;
                 y += 32) {
              for (int x = 160;
                   x + 160 < status.pathing_map.pixel_width(); x += 32) {
                if (placement_is_valid(status, *nydus,
                                       static_cast<std::uint16_t>(x),
                                       static_cast<std::uint16_t>(y))) {
                  status.placement_x = static_cast<std::uint16_t>(x);
                  status.placement_y = static_cast<std::uint16_t>(y);
                  status.placement_valid = true;
                  placement_found = true;
                  break;
                }
              }
            }
          }
          race_building_cards_verified =
              race_building_cards_verified && placement_found &&
              place_current_building(status);
          const ScenarioUnitPreview *const parent =
              find_unit_by_id(status, canal_id);
          const ScenarioUnitPreview *const exit =
              parent == nullptr
                  ? nullptr
                  : find_unit_by_id(status, parent->attached_addon_id);
          race_building_cards_verified =
              race_building_cards_verified && parent != nullptr &&
              exit != nullptr && exit->unit_type == 134U &&
              exit->addon_parent_id == canal_id &&
              status.nydus_parent_id == 0U;
          if (exit != nullptr && status.units.back().unit_id == exit->unit_id) {
            status.units.pop_back();
          }
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 6 : 5;
      }
      if (race_building_cards_verified) {
        ScenarioUnitPreview archives{};
        archives.unit_id = status.next_unit_id++;
        archives.owner = 0U;
        status.researched_technologies[22U] = false;
        race_building_cards_verified =
            configure_preview_type(status, archives, 165U);
        if (race_building_cards_verified) {
          clear_selection(status);
          archives.selected = true;
          const std::uint32_t archives_id = archives.unit_id;
          status.units.push_back(archives);
          race_building_cards_verified = click_command(1U);
          ScenarioUnitPreview *researching =
              find_unit_by_id(status, archives_id);
          race_building_cards_verified =
              researching != nullptr && researching->active_technology == 22U &&
              command_card_for(status).count == 1U &&
              command_card_for(status).buttons[0].action ==
                  CommandButtonVisual::Action::cancel_research;
          status.units.pop_back();
        }
        race_building_cards_probe_stage =
            race_building_cards_verified ? 7 : 6;
      }
    }

    bool all_production_verified = true;
    int all_production_probe_stage{};
    if (all_production_probe) {
      constexpr std::array<std::pair<std::uint16_t, std::size_t>, 11>
          expected_cards{{
              {35, 9},
              {106, 1},
              {108, 1},
              {111, 3},
              {113, 3},
              {114, 4},
              {130, 1},
              {154, 1},
              {155, 3},
              {160, 3},
              {167, 3},
          }};
      all_production_verified = status.assets_ready;
      all_production_probe_stage = all_production_verified ? 1 : 0;
      const starcraft::lang::UnitProductionProducerView producers =
          starcraft::lang::production_producer_types();
      all_production_verified =
          all_production_verified && producers.count == expected_cards.size();
      all_production_probe_stage =
          all_production_verified ? 2 : all_production_probe_stage;
      for (std::size_t producer_index = 0;
           all_production_verified && producer_index < expected_cards.size();
           ++producer_index) {
        const std::uint16_t producer_type =
            expected_cards[producer_index].first;
        const RuntimeUnitType &producer =
            status.runtime_unit_types[producer_type];
        const starcraft::lang::UnitProductionButtonView buttons =
            starcraft::lang::production_buttons_for(producer_type);
        all_production_verified =
            producers.unit_types[producer_index] == producer_type &&
            producer.ready &&
            producer.asset_index < status.unit_assets.size() &&
            buttons.count == expected_cards[producer_index].second;
        starcraft::lang::UnitProductionQueue queue{};
        for (std::size_t button_index = 0;
             all_production_verified && button_index < buttons.count;
             ++button_index) {
          const starcraft::lang::UnitProductionButton &button =
              buttons.buttons[button_index];
          const RuntimeUnitType &product =
              status.runtime_unit_types[button.product_type];
          ScenarioUnitPreview preview{};
          all_production_verified =
              button.producer_type == producer_type &&
              button.position == button_index + 1U &&
              button.icon == button.product_type && product.ready &&
              product.asset_index < status.unit_assets.size() &&
              starcraft::lang::production_total_ticks(
                  product.initialization.simulation.build_time) != 0U &&
              queue.enqueue(button.product_type) &&
              queue.front() == button.product_type &&
              configure_preview_type(status, preview, button.product_type) &&
              preview.unit_type == button.product_type &&
              preview.asset_index == product.asset_index &&
              preview.selection_width != 0U && preview.selection_height != 0U &&
              ((producer_type == starcraft::lang::zerg_larva_type) ==
               (button.kind ==
                starcraft::lang::UnitProductionKind::zerg_larva_morph));
          queue.advance();
          if (all_production_verified) {
            ScenarioUnitPreview source{};
            source.unit_id = status.next_unit_id++;
            source.owner = 0U;
            all_production_verified =
                configure_preview_type(status, source, producer_type);
            bool synthetic_center_found{};
            for (int y = 160; all_production_verified &&
                              y + 160 < status.pathing_map.pixel_height() &&
                              !synthetic_center_found;
                 y += 32) {
              for (int x = 160; x + 160 < status.pathing_map.pixel_width();
                   x += 32) {
                if (!creation_position_passable(status, source, x, y)) {
                  continue;
                }
                source.x = static_cast<std::uint16_t>(x);
                source.y = static_cast<std::uint16_t>(y);
                source.x_fixed = x << 8U;
                source.y_fixed = y << 8U;
                // Verify that this producer center has a legal completion
                // perimeter for the product before using it in the card
                // probe.  The temporary source is the initial collider just
                // as it is in sub_47FBF0 during real production.
                status.units.push_back(source);
                ScenarioUnitPreview trial{};
                trial.unit_id = status.next_unit_id;
                trial.owner = source.owner;
                synthetic_center_found =
                    configure_preview_type(status, trial,
                                           button.product_type) &&
                    settle_created_unit(status, trial, source.x, source.y);
                status.units.pop_back();
                if (synthetic_center_found) {
                  break;
                }
              }
            }
            all_production_verified =
                all_production_verified && synthetic_center_found &&
                source.production_queue.enqueue(button.product_type);
            source.production_kind = button.kind;
            source.production_active = true;
            source.production_started = 0U;
            if (button.kind ==
                starcraft::lang::UnitProductionKind::zerg_larva_morph) {
              all_production_verified =
                  configure_preview_type(status, source,
                                         starcraft::lang::zerg_egg_type) &&
                  all_production_verified;
            }
            const std::uint32_t source_id = source.unit_id;
            const std::size_t first_result_index = status.units.size();
            status.units.push_back(std::move(source));
            all_production_verified =
                all_production_verified &&
                advance_unit_production(status, UINT32_MAX);
            const auto completed = std::find_if(
                status.units.begin() +
                    static_cast<std::ptrdiff_t>(first_result_index),
                status.units.end(),
                [button, source_id](const ScenarioUnitPreview &unit) {
                  return unit.alive && unit.unit_type == button.product_type &&
                         (button.kind == starcraft::lang::UnitProductionKind::
                                             zerg_larva_morph
                              ? unit.unit_id == source_id
                              : unit.unit_id != source_id);
                });
            bool completed_clear = completed != status.units.end();
            if (completed_clear &&
                button.kind == starcraft::lang::UnitProductionKind::train) {
              const ScenarioUnitPreview *const completed_pointer = &*completed;
              completed_clear = std::none_of(
                  status.units.begin(), status.units.end(),
                  [completed_pointer](const ScenarioUnitPreview &other) {
                    return other.alive && &other != completed_pointer &&
                           unit_footprints_overlap_at(
                               *completed_pointer, completed_pointer->x,
                               completed_pointer->y, other);
                  });
            }
            all_production_verified =
                all_production_verified && completed != status.units.end() &&
                completed_clear &&
                completed->asset_index == product.asset_index &&
                completed->iscript_ready && !completed->iscript_state.hidden &&
                completed->current_sprite_frame <
                    status.unit_assets[completed->asset_index]
                        .sprite_frames.size() &&
                completed->max_hit_points ==
                    product.initialization.simulation.max_hit_points;
            // Every iteration uses a synthetic producer.  Remove it and its
            // result so later cards test placement rather than accumulating
            // unrelated blockers at the same coordinate.
            status.units.erase(
                status.units.begin() +
                    static_cast<std::ptrdiff_t>(first_result_index),
                status.units.end());
          }
        }
      }
      all_production_probe_stage =
          all_production_verified ? 4 : all_production_probe_stage;
      const starcraft::lang::ZergLarvaSourceView larva_sources =
          starcraft::lang::zerg_larva_source_types();
      for (std::size_t source_index = 0; source_index < larva_sources.count;
           ++source_index) {
        const std::uint16_t town_hall_type =
            larva_sources.unit_types[source_index];
        const RuntimeUnitType &town_hall =
            status.runtime_unit_types[town_hall_type];
        all_production_verified =
            all_production_verified &&
            starcraft::lang::is_zerg_town_hall(town_hall_type) &&
            town_hall.ready &&
            town_hall.asset_index < status.unit_assets.size();
      }
      all_production_probe_stage =
          all_production_verified ? 5 : all_production_probe_stage;
    }

    bool geysers_verified = true;
    int geyser_probe_stage{};
    if (geyser_probe) {
      constexpr std::array<std::pair<std::uint16_t, std::uint16_t>, 8>
          expected_geysers{{
              {2432, 2976},
              {672, 2976},
              {2944, 1856},
              {128, 1824},
              {128, 1216},
              {2944, 1216},
              {640, 96},
              {2432, 96},
          }};
      const RuntimeUnitType &runtime = status.runtime_unit_types[188U];
      geysers_verified =
          status.geyser_asset_index < status.unit_assets.size() &&
          runtime.ready && runtime.asset_index == status.geyser_asset_index;
      geyser_probe_stage = geysers_verified ? 1 : 0;
      std::size_t geyser_count{};
      for (const ScenarioUnitPreview &unit : status.units) {
        if (!unit.alive || unit.unit_type != 188U) {
          continue;
        }
        ++geyser_count;
        const bool expected_position =
            std::find(expected_geysers.begin(), expected_geysers.end(),
                      std::pair<std::uint16_t, std::uint16_t>{
                          unit.x, unit.y}) != expected_geysers.end();
        geysers_verified = geysers_verified && unit.owner == 11U &&
                           unit.resource_amount == 5000U && expected_position &&
                           unit.asset_index == status.geyser_asset_index &&
                           unit.selection_width != 0U &&
                           unit.selection_height != 0U;
      }
      geysers_verified =
          geysers_verified && geyser_count == expected_geysers.size();
      geyser_probe_stage = geysers_verified ? 2 : geyser_probe_stage;
      // Put the primary executable's upper-left CHK geyser in the middle of
      // the 640x400 world viewport. The old probe only counted records and
      // could pass while no geyser reached the renderer.
      (void)set_camera_position(status, 640 - kMapViewportWidth / 2, 0);
      const auto visible_geyser = std::find_if(
          status.units.begin(), status.units.end(),
          [&status](const ScenarioUnitPreview &unit) {
            if (!unit.alive || unit.unit_type != 188U || !unit.iscript_ready ||
                unit.iscript_state.hidden ||
                unit.asset_index >= status.unit_assets.size()) {
              return false;
            }
            const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
            const int screen_x = static_cast<int>(unit.x) - status.camera_x;
            const int screen_y = static_cast<int>(unit.y) - status.camera_y;
            return screen_x >= 0 && screen_x < kMapViewportWidth &&
                   screen_y >= 0 && screen_y < kMapViewportHeight &&
                   unit.current_sprite_frame < asset.sprite_frames.size() &&
                   !asset.sprite_frames[unit.current_sprite_frame].bgra.empty();
          });
      geysers_verified = geysers_verified && status.terrain_ready &&
                         status.camera_x == 320U && status.camera_y == 0U &&
                         visible_geyser != status.units.end();
      geyser_probe_stage = geysers_verified ? 3 : geyser_probe_stage;
    }

    bool minimap_verified = true;
    if (minimap_probe) {
      minimap_verified = false;
      const std::uint16_t initial_camera_x = status.camera_x;
      const std::uint16_t initial_camera_y = status.camera_y;
      const std::size_t non_background_pixels =
          static_cast<std::size_t>(std::count_if(
              status.minimap.bgra.begin(), status.minimap.bgra.end(),
              [](const std::uint32_t pixel) { return pixel != 0xFF020202U; }));
      const std::size_t varied_pixels =
          status.minimap.bgra.empty()
              ? 0U
              : static_cast<std::size_t>(std::count_if(
                    status.minimap.bgra.begin(), status.minimap.bgra.end(),
                    [&status](const std::uint32_t pixel) {
                      return pixel != status.minimap.bgra.front();
                    }));
      if (probe_client_ready && status.minimap_ready &&
          status.minimap.width == 128U && status.minimap.height == 128U &&
          status.minimap_content_width != 0U &&
          status.minimap_content_height != 0U && non_background_pixels != 0U &&
          varied_pixels != 0U) {
        constexpr int vertical_numerator = 5;
        constexpr int vertical_denominator = 6;
        const int control_width =
            status.minimap_control.right - status.minimap_control.left + 1;
        const int control_height =
            status.minimap_control.bottom - status.minimap_control.top + 1;
        const int content_left = status.minimap_control.left +
                                 status.minimap_content_x * control_width / 128;
        const int content_right =
            status.minimap_control.left +
            (status.minimap_content_x + status.minimap_content_width) *
                control_width / 128 -
            1;
        const int content_top = status.minimap_control.top *
                                    vertical_numerator / vertical_denominator +
                                status.minimap_content_y * control_height *
                                    vertical_numerator /
                                    (128 * vertical_denominator);
        const int content_bottom =
            status.minimap_control.top * vertical_numerator /
                vertical_denominator +
            (status.minimap_content_y + status.minimap_content_height) *
                control_height * vertical_numerator /
                (128 * vertical_denominator) -
            1;
        const int maximum_camera_x =
            (std::max)(0, static_cast<int>(status.scenario_width) * 32 -
                              kMapViewportWidth);
        const int maximum_camera_y =
            (std::max)(0, static_cast<int>(status.scenario_height) * 32 -
                              kMapViewportHeight);
        const bool choose_far_right = initial_camera_x < maximum_camera_x / 2;
        const bool choose_far_bottom = initial_camera_y < maximum_camera_y / 2;
        const int minimap_x =
            choose_far_right ? content_right - 1 : content_left + 1;
        const int minimap_y =
            choose_far_bottom ? content_bottom - 1 : content_top + 1;
        const LPARAM minimap_click = client_point(minimap_x, minimap_y);
        SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, minimap_click);
        SendMessageA(window, WM_MOUSEMOVE, MK_LBUTTON, minimap_click);
        SendMessageA(window, WM_LBUTTONUP, 0, minimap_click);
        minimap_verified =
            status.terrain_ready && !window_state.minimap_dragging &&
            (status.camera_x != initial_camera_x ||
             status.camera_y != initial_camera_y) &&
            (status.camera_x & 7U) == 0U && (status.camera_y & 7U) == 0U &&
            status.camera_x <= maximum_camera_x &&
            status.camera_y <= maximum_camera_y;
      }
    }

    const bool needs_scv = production_probe || movement_probe ||
                           pathfinding_probe || worker_actions_probe ||
                           harvest_visual_probe || smart_orders_probe ||
                           command_target_probe || building_placement_probe ||
                           refinery_placement_probe || construction_probe ||
                           unit_avoidance_probe || multi_status_probe ||
                           construction_status_probe || resource_feedback_probe;
    bool command_panel_verified = true;
    const std::size_t scvs_before_production = static_cast<std::size_t>(
        std::count_if(status.units.begin(), status.units.end(),
                      [](const ScenarioUnitPreview &unit) {
                        return unit.alive && unit.owner == 0U &&
                               unit.unit_type == 7U;
                      }));
    if (command_panel_probe || status_panel_probe || building_working_probe ||
        needs_scv) {
      command_panel_verified = false;
      if (status_panel_probe) {
        status.player_minerals = 1000;
      }
      const auto command_center =
          std::find_if(status.units.begin(), status.units.end(),
                       [](const ScenarioUnitPreview &unit) {
                         return unit.unit_type == 106 && unit.owner == 0;
                       });
      if (command_center != status.units.end()) {
        clear_selection(status);
        command_center->selected = true;
        command_panel_verified =
            click_command(1) && status.last_command_position == 1;
      }
    }
    const auto active_local_producer = [&status]() -> ScenarioUnitPreview * {
      const auto producer =
          std::find_if(status.units.begin(), status.units.end(),
                       [](ScenarioUnitPreview &unit) {
                         return unit.alive && unit.owner == 0U &&
                                unit.production_active &&
                                !unit.production_queue.empty();
                       });
      return producer == status.units.end() ? nullptr : &*producer;
    };
    const auto last_local_scv = [&status]() -> ScenarioUnitPreview * {
      const auto worker = std::find_if(
          status.units.rbegin(), status.units.rend(),
          [](ScenarioUnitPreview &unit) {
            return unit.alive && unit.owner == 0U && unit.unit_type == 7U;
          });
      return worker == status.units.rend() ? nullptr : &*worker;
    };
    bool harvest_queue_verified = true;
    int harvest_queue_probe_stage{};
    if (harvest_queue_probe) {
      std::array<std::uint32_t, 2> worker_ids{};
      std::size_t worker_count{};
      for (const ScenarioUnitPreview &unit : status.units) {
        if (unit.alive && unit.owner == 0U && unit.unit_type == 7U &&
            worker_count < worker_ids.size()) {
          worker_ids[worker_count++] = unit.unit_id;
        }
      }
      const auto mineral =
          std::find_if(status.units.begin(), status.units.end(),
                       [](const ScenarioUnitPreview &unit) {
                         return unit.alive && unit.unit_type >= 176U &&
                                unit.unit_type <= 178U &&
                                unit.resource_amount >= 20U;
                       });
      harvest_queue_verified =
          worker_count == worker_ids.size() && mineral != status.units.end();
      harvest_queue_probe_stage = harvest_queue_verified ? 1 : 0;
      const std::uint32_t mineral_id =
          mineral == status.units.end() ? 0U : mineral->unit_id;
      const std::uint16_t resource_before =
          mineral == status.units.end() ? 0U : mineral->resource_amount;
      if (harvest_queue_verified) {
        mineral->harvest_queue.clear();
        for (const std::uint32_t worker_id : worker_ids) {
          ScenarioUnitPreview *const worker =
              find_unit_by_id(status, worker_id);
          ScenarioUnitPreview *const source =
              find_unit_by_id(status, mineral_id);
          harvest_queue_verified =
              harvest_queue_verified && worker != nullptr && source != nullptr;
          if (worker != nullptr && source != nullptr) {
            cancel_unit_order(status, *worker);
            harvest_queue_verified =
                begin_scv_interaction(status, *worker, *source,
                                      ActiveUnitOrder::gather) &&
                harvest_queue_verified;
          }
        }
      }
      harvest_queue_probe_stage =
          harvest_queue_verified ? 2 : harvest_queue_probe_stage;
      bool observed_waiter{};
      bool observed_promotion{};
      bool workers_remained_distinct{};
      std::uint32_t first_active{};
      for (int tick = 0; harvest_queue_verified && tick < 2048; ++tick) {
        SendMessageA(window, WM_TIMER, 1, 0);
        ScenarioUnitPreview *const source = find_unit_by_id(status, mineral_id);
        ScenarioUnitPreview *const first =
            find_unit_by_id(status, worker_ids[0]);
        ScenarioUnitPreview *const second =
            find_unit_by_id(status, worker_ids[1]);
        harvest_queue_verified =
            source != nullptr && first != nullptr && second != nullptr &&
            first->alive && second->alive &&
            first->asset_index < status.unit_assets.size() &&
            second->asset_index < status.unit_assets.size();
        if (!harvest_queue_verified) {
          break;
        }
        const std::uint32_t active = source->harvest_queue.active_worker();
        if (!observed_waiter && active != 0U &&
            source->harvest_queue.queued_count() == 1U) {
          observed_waiter = true;
          first_active = active;
          workers_remained_distinct =
              !unit_footprints_overlap_at(*first, first->x, first->y, *second);
        }
        if (observed_waiter && active != 0U && active != first_active &&
            source->resource_amount < resource_before) {
          observed_promotion = true;
          break;
        }
      }
      harvest_queue_verified = harvest_queue_verified && observed_waiter &&
                               observed_promotion && workers_remained_distinct;
      harvest_queue_probe_stage =
          harvest_queue_verified ? 3 : harvest_queue_probe_stage;
      for (const std::uint32_t worker_id : worker_ids) {
        ScenarioUnitPreview *const worker = find_unit_by_id(status, worker_id);
        if (worker != nullptr) {
          cancel_unit_order(status, *worker);
        }
      }
      ScenarioUnitPreview *const source = find_unit_by_id(status, mineral_id);
      if (source != nullptr) {
        source->harvest_queue.clear();
      }
    }
    bool status_panel_verified = true;
    if (status_panel_probe) {
      const bool queued_twice = command_panel_verified && click_command(1);
      ScenarioUnitPreview *const producer = active_local_producer();
      const std::uint16_t queued_type =
          producer == nullptr ? starcraft::lang::cunit_production_empty
                              : producer->production_queue.front();
      const std::uint32_t duration =
          queued_type < status.runtime_unit_types.size()
              ? static_cast<std::uint32_t>(
                    starcraft::lang::production_total_ticks(
                        status.runtime_unit_types[queued_type]
                            .initialization.simulation.build_time)) *
                    kSimulationTickMilliseconds
              : 0U;
      if (queued_twice && duration != 0U && producer != nullptr) {
        producer->production_started = GetTickCount() - duration / 2U;
      }
      status_panel_verified =
          queued_twice && status.status_panel_ready &&
          status.status_progress_art_ready && status.group_wireframe_ready &&
          status.status_wireframe_control.left == 168 &&
          status.status_wireframe_control.top == 388 &&
          status.status_wireframe_control.right == 231 &&
          status.status_wireframe_control.bottom == 451 &&
          status.status_queue_controls.front().left == 242 &&
          status.status_queue_controls.front().top == 402 &&
          status.status_selection_controls.front().left == 168 &&
          status.status_selection_controls.front().top == 396 &&
          !status_text(status, 107).empty() && producer != nullptr &&
          producer->production_active &&
          producer->production_queue.count() == 2U;
    }
    bool resource_feedback_verified = true;
    if (resource_feedback_probe) {
      ScenarioUnitPreview *const producer = active_local_producer();
      const std::size_t queued_before =
          producer == nullptr ? 0U : producer->production_queue.count();
      const std::uint32_t mineral_before = status.player_minerals;
      const std::uint32_t errors_before = status.resource_error_count;
      const std::uint32_t sounds_before = window_state.audio_play_count;
      const bool rejected_click = click_command(1);
      resource_feedback_verified =
          rejected_click &&
          mineral_before < status.scv_simulation.mineral_cost &&
          status.player_minerals == mineral_before && producer != nullptr &&
          producer->production_queue.count() == queued_before &&
          status.resource_error_count == errors_before + 1U &&
          status.system_message ==
              printable_status_text(status_text(status, 781U)) &&
          static_cast<std::int32_t>(status.system_message_until -
                                    GetTickCount()) > 0 &&
          status.pending_resource_error_sound == 0xFFU &&
          window_state.audio_ready &&
          window_state.audio_play_count == sounds_before + 1U;
    }
    bool resource_strip_verified = true;
    int resource_strip_probe_stage{};
    if (resource_strip_probe) {
      const std::array<std::uint32_t, 2> supply = local_supply(status);
      for (int tick = 0; tick < 32; ++tick) {
        advance_resource_display(status);
      }
      resource_strip_probe_stage =
          status.resource_panel_ready && status.unit_traits_ready ? 1 : 0;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 1 &&
                  status.resource_supply_control.left == 436 &&
                  status.resource_supply_control.top == 0 &&
                  status.resource_gas_control.left == 368 &&
                  status.resource_gas_control.top == 0 &&
                  status.resource_mineral_control.left == 300 &&
                  status.resource_mineral_control.top == 0
              ? 2
              : resource_strip_probe_stage;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 2 && status.displayed_minerals != 0U &&
                  status.displayed_minerals <= status.player_minerals &&
                  status.displayed_gas == status.player_gas
              ? 3
              : resource_strip_probe_stage;
      resource_strip_probe_stage =
          resource_strip_probe_stage == 3 && supply[1] != 0U
              ? 4
              : resource_strip_probe_stage;
      resource_strip_verified = resource_strip_probe_stage == 4;
    }
    bool music_verified = true;
    if (music_probe) {
      ALint source_state{};
      if (window_state.audio_context != nullptr) {
        (void)alcMakeContextCurrent(window_state.audio_context);
        alGetSourcei(window_state.music_source, AL_SOURCE_STATE, &source_state);
      }
      music_verified =
          status.music_available &&
          status.music_path == R"(music\terran1.wav)" &&
          !status.music_wave.empty() && window_state.audio_ready &&
          window_state.music_playing && window_state.music_buffer != 0U &&
          window_state.music_source != 0U && source_state == AL_PLAYING;
    }
    bool unit_audio_verified = true;
    if (unit_audio_probe) {
      unit_audio_verified = false;
      clear_selection(status);
      const auto audible = std::find_if(
          status.units.begin(), status.units.end(),
          [&status](const ScenarioUnitPreview &unit) {
            const int game_x = static_cast<int>(unit.x) - status.camera_x;
            const int game_y = static_cast<int>(unit.y) - status.camera_y;
            return unit.alive && unit.owner == 0U &&
                   unit.unit_type < status.unit_sound_ranges.size() &&
                   status.unit_sound_ranges[unit.unit_type].what_first != 0U &&
                   game_x >= 0 && game_y >= 0 && game_x < kMapViewportWidth &&
                   game_y < kMapViewportHeight &&
                   !hud_pixel_opaque(status, game_x, game_y);
          });
      if (audible != status.units.end() && probe_client_ready) {
        const std::uint32_t sounds_before = window_state.audio_play_count;
        const int game_x = static_cast<int>(audible->x) - status.camera_x;
        const int game_y = static_cast<int>(audible->y) - status.camera_y;
        const LPARAM click = client_point(game_x, game_y);
        SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, click);
        SendMessageA(window, WM_LBUTTONUP, 0, click);
        const UnitSoundRanges &ranges =
            status.unit_sound_ranges[audible->unit_type];
        ALint music_state{};
        alGetSourcei(window_state.music_source, AL_SOURCE_STATE, &music_state);
        unit_audio_verified =
            audible->selected && selection_count(status) == 1U &&
            status.pending_game_sound == 0xFFFFU &&
            status.last_game_sound >= ranges.what_first &&
            status.last_game_sound <= ranges.what_last &&
            window_state.audio_play_count == sounds_before + 1U &&
            music_state == AL_PLAYING;
      }
    }
    bool building_working_verified = true;
    if (building_working_probe) {
      building_working_verified = false;
      ScenarioUnitPreview *const producer_pointer = active_local_producer();
      if (producer_pointer != nullptr) {
        ScenarioUnitPreview &producer = *producer_pointer;
        const std::uint16_t initial_overlay_pc =
            producer.dynamic_overlay_iscript_state.program_counter;
        for (int tick = 0; tick < 10; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        if (producer.dynamic_overlay_asset_index < status.unit_assets.size()) {
          const UnitRenderAsset &working_asset =
              status.unit_assets[producer.dynamic_overlay_asset_index];
          building_working_verified =
              producer.production_active && producer.unit_type == 106U &&
              producer.last_animation == 19U &&
              producer.dynamic_overlay_ready &&
              producer.dynamic_overlay_asset_index ==
                  status.command_center_working_asset_index &&
              working_asset.image_id == 247U &&
              producer.dynamic_overlay_iscript_state.program_counter !=
                  initial_overlay_pc;
        }
      }
    }
    bool production_verified = true;
    if (needs_scv) {
      std::uint32_t producer_id{};
      ScenarioUnitPreview *producer_pointer = active_local_producer();
      if (producer_pointer != nullptr) {
        producer_id = producer_pointer->unit_id;
      }
      production_verified = producer_pointer != nullptr &&
                            producer_pointer->last_animation == 19U &&
                            producer_pointer->dynamic_overlay_ready;
      if (production_verified) {
        const std::uint16_t queued_type =
            producer_pointer->production_queue.front();
        producer_pointer->production_started =
            GetTickCount() -
            static_cast<std::uint32_t>(starcraft::lang::production_total_ticks(
                status.runtime_unit_types[queued_type]
                    .initialization.simulation.build_time)) *
                kSimulationTickMilliseconds;
        SendMessageA(window, WM_TIMER, 1, 0);
        const ScenarioUnitPreview *const producer =
            find_unit_by_id(status, producer_id);
        ScenarioUnitPreview *const produced_worker = last_local_scv();
        const std::size_t scvs_after = static_cast<std::size_t>(std::count_if(
            status.units.begin(), status.units.end(),
            [](const ScenarioUnitPreview &unit) {
              return unit.alive && unit.owner == 0U && unit.unit_type == 7U;
            }));
        bool spawn_clear = produced_worker != nullptr && producer != nullptr;
        if (spawn_clear) {
          spawn_clear =
              (produced_worker->x != producer->x ||
               produced_worker->y != producer->y) &&
              std::abs(static_cast<int>(produced_worker->x) - producer->x) <=
                  128 &&
              std::abs(static_cast<int>(produced_worker->y) - producer->y) <=
                  128;
          for (const ScenarioUnitPreview &other : status.units) {
            if (!spawn_clear) {
              break;
            }
            if (other.alive && &other != produced_worker) {
              spawn_clear = !unit_footprints_overlap_at(
                  *produced_worker, produced_worker->x, produced_worker->y,
                  other);
            }
          }
          static const std::vector<starcraft::lang::PathObstacle> no_obstacles;
          spawn_clear =
              spawn_clear &&
              starcraft::lang::path_position_passable(
                  status.pathing_map, produced_worker->x, produced_worker->y,
                  produced_worker->selection_width,
                  produced_worker->selection_height, no_obstacles);
        }
        production_verified =
            producer != nullptr && !producer->production_active &&
            scvs_after == scvs_before_production + 1U &&
            produced_worker != nullptr && spawn_clear &&
            produced_worker->iscript_ready &&
            !produced_worker->iscript_state.hidden &&
            produced_worker->asset_index < status.unit_assets.size() &&
            produced_worker->current_sprite_frame <
                status.unit_assets[produced_worker->asset_index]
                    .sprite_frames.size() &&
            producer->last_animation == 20U && !producer->dynamic_overlay_ready;
      }
    }
    bool smart_orders_verified = true;
    if (smart_orders_probe) {
      smart_orders_verified = false;
      ScenarioUnitPreview *const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview &worker = *worker_pointer;
        worker.selected =
            worker.alive && worker.owner == 0U && worker.unit_type == 7U;
        const auto mineral =
            std::find_if(status.units.begin(), status.units.end(),
                         [](const ScenarioUnitPreview &unit) {
                           return unit.alive && unit.resource_amount != 0U &&
                                  (unit.dat_flags & 0x2000U) != 0U;
                         });
        const auto enemy =
            std::find_if(status.units.begin(), status.units.end(),
                         [](const ScenarioUnitPreview &unit) {
                           return unit.alive && unit.owner != 0U &&
                                  unit.owner != 11U &&
                                  unit.max_hit_points != 0U;
                         });
        bool gathered{};
        if (worker.selected && mineral != status.units.end()) {
          gathered =
              issue_scv_smart_order(status, mineral->x, mineral->y) == 1U &&
              worker.active_order == ActiveUnitOrder::gather &&
              worker.order_target_id == mineral->unit_id &&
              status.last_issued_order == 0x4EU;
          cancel_unit_order(status, worker);
        }
        bool attacked{};
        if (gathered && enemy != status.units.end()) {
          attacked = issue_scv_smart_order(status, enemy->x, enemy->y) == 1U &&
                     worker.active_order == ActiveUnitOrder::attack &&
                     worker.order_target_id == enemy->unit_id &&
                     status.last_issued_order == 9U;
          cancel_unit_order(status, worker);
        }
        smart_orders_verified = gathered && attacked;
      }
    }
    bool harvest_visual_verified = true;
    if (harvest_visual_probe) {
      harvest_visual_verified = false;
      ScenarioUnitPreview *const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview &worker = *worker_pointer;
        worker.selected =
            worker.alive && worker.owner == 0U && worker.unit_type == 7U;
        auto mineral = std::find_if(status.units.begin(), status.units.end(),
                                    [](const ScenarioUnitPreview &unit) {
                                      return unit.alive &&
                                             unit.unit_type >= 176U &&
                                             unit.unit_type <= 178U &&
                                             unit.resource_amount >= 10U;
                                    });
        if (worker.selected && mineral != status.units.end() &&
            begin_scv_interaction(status, worker, *mineral,
                                  ActiveUnitOrder::gather)) {
          worker.x = worker.movement_final_x;
          worker.y = worker.movement_final_y;
          worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
          worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
          stop_unit_movement(status, worker);
          bool animated{};
          bool cutter_event{};
          const std::uint32_t sounds_before = window_state.audio_play_count;
          for (int tick = 0; tick < 140 && worker.active_order !=
                                               ActiveUnitOrder::return_cargo;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
            animated = animated ||
                       (worker.iscript_ready && worker.last_animation == 15U &&
                        (worker.current_sprite_frame == 51U ||
                         worker.current_sprite_frame == 102U));
            cutter_event = cutter_event ||
                           (worker.iscript_state.weapon_event_count != 0U &&
                            worker.iscript_state.weapon_event == 8U);
          }
          const UnitRenderAsset *const cargo_asset =
              status.mineral_cargo_asset_index < status.unit_assets.size()
                  ? &status.unit_assets[status.mineral_cargo_asset_index]
                  : nullptr;
          const std::uint16_t harvest_x = worker.x;
          const std::uint16_t harvest_y = worker.y;
          for (int tick = 0;
               tick < 48 && worker.cargo_minerals != 0U && worker.moving &&
               std::abs(static_cast<int>(worker.x) - harvest_x) +
                       std::abs(static_cast<int>(worker.y) - harvest_y) <
                   64;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
          }
          (void)set_camera_position(
              status, static_cast<int>(worker.x) - kMapViewportWidth / 2,
              static_cast<int>(worker.y) - kMapViewportHeight / 2);
          harvest_visual_verified =
              worker.active_order == ActiveUnitOrder::return_cargo &&
              worker.cargo_minerals == 8U && animated && cutter_event &&
              window_state.audio_play_count > sounds_before &&
              cargo_asset != nullptr && cargo_asset->image_id == 357U &&
              cargo_asset->overlay_ready &&
              cargo_asset->overlay_image_id == 365U &&
              render_opengl(window, window_state);
        }
      }
    }
    bool multi_status_verified = true;
    if (multi_status_probe) {
      multi_status_verified = false;
      if (production_verified && !status.units.empty()) {
        ScenarioUnitPreview *command_center{};
        ScenarioUnitPreview *scv{};
        for (ScenarioUnitPreview &unit : status.units) {
          if (unit.alive && unit.owner == 0 && unit.unit_type == 106U) {
            command_center = &unit;
          } else if (unit.alive && unit.owner == 0 && unit.unit_type == 7U) {
            scv = &unit;
          }
        }
        if (command_center != nullptr && scv != nullptr) {
          clear_selection(status);
          command_center->selected = true;
          scv->selected = true;
          multi_status_verified =
              selection_count(status) == 2U && status.group_wireframe_ready &&
              command_center->unit_type <
                  status.group_wireframe_frames.size() &&
              scv->unit_type < status.group_wireframe_frames.size();
        }
      }
    }
    bool movement_verified = true;
    int movement_probe_stage{};
    if (movement_probe) {
      movement_verified = false;
      ScenarioUnitPreview *const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        movement_probe_stage = 1;
        clear_selection(status);
        ScenarioUnitPreview &scv = *scv_pointer;
        scv.selected = true;
        const std::uint16_t initial_x = scv.x;
        const std::uint16_t initial_y = scv.y;
        const int game_x = scv.x - status.camera_x;
        const int game_y = scv.y - status.camera_y;
        const int target_game_x = game_x > 320 ? game_x - 96 : game_x + 96;
        const int target_game_y = (std::clamp)(game_y, 32, 240);
        SendMessageA(window, WM_RBUTTONDOWN, MK_RBUTTON,
                     client_point(target_game_x, target_game_y));
        const bool order_started =
            scv.moving && command_card_for(status).count == 9;
        for (int tick = 0; tick < 16; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        const bool scv_moved =
            order_started && (scv.x != initial_x || scv.y != initial_y);
        movement_probe_stage = scv_moved ? 2 : movement_probe_stage;

        // Walking-control units enter action 11 with flingy.dat's sentinel
        // speed and receive their actual fixed-point speed through iscript
        // opcode 0x3F. Exercise a Marine through the same right-click path;
        // this specifically guards against the old "only SCVs move" result.
        cancel_unit_order(status, scv);
        const bool marine_ready = configure_preview_type(status, scv, 0U);
        movement_probe_stage = marine_ready ? 3 : movement_probe_stage;
        starcraft::lang::PathPoint marine_start{};
        starcraft::lang::PathPoint marine_target{};
        bool marine_path_ready{};
        if (marine_ready) {
          std::vector<starcraft::lang::PathObstacle> obstacles;
          marine_path_ready =
              collect_building_obstacles(status, &scv, obstacles);
          for (int y = 96;
               marine_path_ready && y < status.pathing_map.pixel_height() - 96;
               y += 32) {
            for (int x = 96; x < status.pathing_map.pixel_width() - 224;
                 x += 32) {
              std::vector<starcraft::lang::PathPoint> path;
              if (!creation_position_passable(status, scv, x, y) ||
                  !creation_position_passable(status, scv, x + 128, y) ||
                  !starcraft::lang::find_unit_path(
                      status.pathing_map, x, y, x + 128, y, scv.selection_width,
                      scv.selection_height, obstacles, path) ||
                  path.empty()) {
                continue;
              }
              marine_start = {static_cast<std::uint16_t>(x),
                              static_cast<std::uint16_t>(y)};
              marine_target = {static_cast<std::uint16_t>(x + 128),
                               static_cast<std::uint16_t>(y)};
              marine_path_ready = true;
              break;
            }
            if (marine_target.x != 0U) {
              break;
            }
          }
          marine_path_ready = marine_target.x != 0U;
        }
        movement_probe_stage = marine_path_ready ? 4 : movement_probe_stage;
        if (marine_path_ready) {
          scv.x = marine_start.x;
          scv.y = marine_start.y;
          scv.x_fixed = static_cast<std::int32_t>(scv.x) << 8U;
          scv.y_fixed = static_cast<std::int32_t>(scv.y) << 8U;
          // The selected probe path points east (direction 64). Start north
          // so the probe covers CFlingy facing rotation as well as translation.
          scv.direction = 0U;
          (void)set_camera_position(
              status, static_cast<int>(scv.x) - kMapViewportWidth / 2,
              static_cast<int>(scv.y) - 160);
          const int marine_target_game_x = marine_target.x - status.camera_x;
          const int marine_target_game_y = marine_target.y - status.camera_y;
          SendMessageA(
              window, WM_RBUTTONDOWN, MK_RBUTTON,
              client_point(marine_target_game_x, marine_target_game_y));
        }
        const std::uint16_t marine_initial_x = scv.x;
        const std::uint16_t marine_initial_y = scv.y;
        const std::uint8_t marine_initial_direction = scv.direction;
        movement_probe_stage = scv.moving ? 5 : movement_probe_stage;
        movement_probe_stage =
            scv.last_animation == 11U ? 6 : movement_probe_stage;
        const bool marine_iscript_speed =
            scv.iscript_state.flingy_velocity_event_count != 0U ||
            scv.iscript_state.flingy_speed_event_count != 0U;
        movement_probe_stage = marine_iscript_speed ? 7 : movement_probe_stage;
        movement_probe_stage =
            effective_unit_top_speed(scv) > scv.movement_top_speed
                ? 8
                : movement_probe_stage;
        const bool marine_order_started =
            marine_ready && scv.moving && scv.last_animation == 11U &&
            marine_iscript_speed &&
            effective_unit_top_speed(scv) > scv.movement_top_speed;
        for (int tick = 0; tick < 16; ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
        }
        const int marine_distance =
            std::abs(static_cast<int>(scv.x) - marine_initial_x) +
            std::abs(static_cast<int>(scv.y) - marine_initial_y);
        const bool marine_turning_image =
            scv.asset_index < status.unit_assets.size() &&
            status.unit_assets[scv.asset_index].graphics_turns;
        movement_probe_stage = marine_turning_image ? 9 : movement_probe_stage;
        const starcraft::game::ImageFacingFrame initial_facing =
            starcraft::game::image_facing_frame(marine_initial_direction);
        const starcraft::game::ImageFacingFrame current_facing =
            starcraft::game::image_facing_frame(scv.direction);
        const bool marine_turned =
            scv.direction != marine_initial_direction &&
            (current_facing.frame != initial_facing.frame ||
             current_facing.mirrored != initial_facing.mirrored);
        movement_probe_stage = marine_turned ? 10 : movement_probe_stage;
        movement_verified = scv_moved && marine_order_started &&
                            marine_distance >= 8 && marine_turning_image &&
                            marine_turned;
        movement_probe_stage = movement_verified ? 11 : movement_probe_stage;
      }
    }
    bool pathfinding_verified = true;
    if (pathfinding_probe) {
      pathfinding_verified = false;
      ScenarioUnitPreview *const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview &scv = *scv_pointer;
        scv.selected = true;
        std::vector<starcraft::lang::PathObstacle> obstacles;
        if (collect_building_obstacles(status, &scv, obstacles)) {
          for (const ScenarioUnitPreview &building : status.units) {
            if (!building.is_building || pathfinding_verified) {
              continue;
            }
            const int half_worker_width = scv.selection_width / 2;
            const int half_worker_height = scv.selection_height / 2;
            const int half_building_width = building.selection_width / 2;
            const int half_building_height = building.selection_height / 2;
            const int clearance_x =
                half_building_width + half_worker_width + 24;
            const int clearance_y =
                half_building_height + half_worker_height + 24;
            const std::array<std::array<int, 4>, 2> candidates{{
                {{static_cast<int>(building.x) - clearance_x, building.y,
                  static_cast<int>(building.x) + clearance_x, building.y}},
                {{building.x, static_cast<int>(building.y) - clearance_y,
                  building.x, static_cast<int>(building.y) + clearance_y}},
            }};
            for (const auto &candidate : candidates) {
              if (candidate[0] < 0 || candidate[1] < 0 || candidate[2] < 0 ||
                  candidate[3] < 0 ||
                  candidate[0] >= status.pathing_map.pixel_width() ||
                  candidate[2] >= status.pathing_map.pixel_width() ||
                  candidate[1] >= status.pathing_map.pixel_height() ||
                  candidate[3] >= status.pathing_map.pixel_height() ||
                  !starcraft::lang::path_position_passable(
                      status.pathing_map, candidate[0], candidate[1],
                      scv.selection_width, scv.selection_height, obstacles) ||
                  !starcraft::lang::path_position_passable(
                      status.pathing_map, candidate[2], candidate[3],
                      scv.selection_width, scv.selection_height, obstacles)) {
                continue;
              }
              scv.x = static_cast<std::uint16_t>(candidate[0]);
              scv.y = static_cast<std::uint16_t>(candidate[1]);
              scv.x_fixed = static_cast<std::int32_t>(scv.x) << 8U;
              scv.y_fixed = static_cast<std::int32_t>(scv.y) << 8U;
              scv.direction = starcraft::lang::direction_from_points(
                  scv.x, scv.y, static_cast<std::uint16_t>(candidate[2]),
                  static_cast<std::uint16_t>(candidate[3]));
              const bool order_started =
                  issue_scv_move_order(
                      status, static_cast<std::uint16_t>(candidate[2]),
                      static_cast<std::uint16_t>(candidate[3])) == 1 &&
                  scv.movement_path.size() >= 2;
              bool remained_passable = order_started;
              for (int tick = 0; tick < 768 && scv.moving && remained_passable;
                   ++tick) {
                SendMessageA(window, WM_TIMER, 1, 0);
                remained_passable = starcraft::lang::path_position_passable(
                    status.pathing_map, scv.x, scv.y, scv.selection_width,
                    scv.selection_height, obstacles);
              }
              pathfinding_verified = order_started && remained_passable &&
                                     !scv.moving && scv.x == candidate[2] &&
                                     scv.y == candidate[3];
              if (pathfinding_verified) {
                break;
              }
            }
          }
        }
      }
    }
    bool unit_avoidance_verified = true;
    int unit_avoidance_probe_stage{};
    if (unit_avoidance_probe) {
      unit_avoidance_verified = false;
      ScenarioUnitPreview *const produced_pointer = last_local_scv();
      if (production_verified && produced_pointer != nullptr) {
        unit_avoidance_probe_stage = 1;
        ScenarioUnitPreview &produced = *produced_pointer;
        std::vector<starcraft::lang::PathObstacle> obstacles;
        // units.dat flag 0x08 makes workers pass mobile CUnits in the original
        // collision predicate; CUnitHarvest serializes their resource access.
        // Exercise CUnitPathCollide's mobile/mobile branch with Vulture type 2
        // (the Factory card's first product) instead of falsifying the SCV's
        // recovered worker property. Its CFlingy uses the ordinary speed path;
        // infantry type 0 uses movement-control 2's walking state machine.
        if (configure_preview_type(status, produced, 2U) &&
            collect_building_obstacles(status, &produced, obstacles)) {
          std::array<starcraft::lang::PathPoint, 4> crossing{};
          bool crossing_found{};
          const auto clear_of_existing_units = [&](const int x, const int y) {
            return std::none_of(
                status.units.begin(), status.units.end(),
                [&produced, x, y](const ScenarioUnitPreview &other) {
                  return other.alive && other.unit_id != produced.unit_id &&
                         unit_footprints_overlap_at(produced, x, y, other);
                });
          };
          for (int y = 96;
               y + 64 < status.pathing_map.pixel_height() && !crossing_found;
               y += 16) {
            for (int x = 96; x + 64 < status.pathing_map.pixel_width();
                 x += 16) {
              crossing = {{
                  {static_cast<std::uint16_t>(x - 64),
                   static_cast<std::uint16_t>(y)},
                  {static_cast<std::uint16_t>(x + 64),
                   static_cast<std::uint16_t>(y)},
                  {static_cast<std::uint16_t>(x),
                   static_cast<std::uint16_t>(y - 64)},
                  {static_cast<std::uint16_t>(x),
                   static_cast<std::uint16_t>(y + 64)},
              }};
              bool points_clear = clear_of_existing_units(x, y);
              for (const starcraft::lang::PathPoint &point : crossing) {
                points_clear = points_clear &&
                               clear_of_existing_units(point.x, point.y) &&
                               starcraft::lang::path_position_passable(
                                   status.pathing_map, point.x, point.y,
                                   produced.selection_width,
                                   produced.selection_height, obstacles);
              }
              std::vector<starcraft::lang::PathPoint> horizontal;
              std::vector<starcraft::lang::PathPoint> vertical;
              crossing_found =
                  points_clear &&
                  starcraft::lang::find_unit_path(
                      status.pathing_map, crossing[0].x, crossing[0].y,
                      crossing[1].x, crossing[1].y, produced.selection_width,
                      produced.selection_height, obstacles, horizontal) &&
                  horizontal.size() == 1U &&
                  starcraft::lang::find_unit_path(
                      status.pathing_map, crossing[2].x, crossing[2].y,
                      crossing[3].x, crossing[3].y, produced.selection_width,
                      produced.selection_height, obstacles, vertical) &&
                  vertical.size() == 1U;
              if (crossing_found) {
                break;
              }
            }
          }
          if (crossing_found) {
            unit_avoidance_probe_stage = 2;
            const std::uint32_t first_id = produced.unit_id;
            produced.x = crossing[0].x;
            produced.y = crossing[0].y;
            produced.x_fixed = static_cast<std::int32_t>(produced.x) << 8U;
            produced.y_fixed = static_cast<std::int32_t>(produced.y) << 8U;
            produced.selected = false;
            cancel_unit_order(status, produced);
            ScenarioUnitPreview second = produced;
            second.unit_id = status.next_unit_id++;
            second.x = crossing[2].x;
            second.y = crossing[2].y;
            second.x_fixed = static_cast<std::int32_t>(second.x) << 8U;
            second.y_fixed = static_cast<std::int32_t>(second.y) << 8U;
            const std::uint32_t second_id = second.unit_id;
            status.units.push_back(std::move(second));
            ScenarioUnitPreview *first = find_unit_by_id(status, first_id);
            ScenarioUnitPreview *other = find_unit_by_id(status, second_id);
            if (first != nullptr && other != nullptr &&
                plan_scv_path(status, *first, crossing[1].x, crossing[1].y) &&
                plan_scv_path(status, *other, crossing[3].x, crossing[3].y)) {
              unit_avoidance_probe_stage = 3;
              first->moving = true;
              first->active_order = ActiveUnitOrder::move;
              other->moving = true;
              other->active_order = ActiveUnitOrder::move;
              bool separated = true;
              bool avoidance_observed{};
              for (int tick = 0;
                   tick < 1536 && separated && (first->moving || other->moving);
                   ++tick) {
                SendMessageA(window, WM_TIMER, 1, 0);
                first = find_unit_by_id(status, first_id);
                other = find_unit_by_id(status, second_id);
                if (first == nullptr || other == nullptr) {
                  separated = false;
                  break;
                }
                separated = !unit_footprints_overlap_at(*first, first->x,
                                                        first->y, *other);
                avoidance_observed = avoidance_observed ||
                                     first->avoidance_ticks != 0U ||
                                     other->avoidance_ticks != 0U ||
                                     first->collision_wait_ticks != 0U ||
                                     other->collision_wait_ticks != 0U;
              }
              unit_avoidance_verified =
                  separated && avoidance_observed && !first->moving &&
                  !other->moving && first->x == crossing[1].x &&
                  first->y == crossing[1].y && other->x == crossing[3].x &&
                  other->y == crossing[3].y;
              unit_avoidance_probe_stage = avoidance_observed ? 4 : 3;
              if (avoidance_observed && !separated) {
                unit_avoidance_probe_stage = 12;
              } else if (avoidance_observed &&
                         (first->moving || other->moving)) {
                unit_avoidance_probe_stage = 11;
              } else if (avoidance_observed && (first->x != crossing[1].x ||
                                                first->y != crossing[1].y ||
                                                other->x != crossing[3].x ||
                                                other->y != crossing[3].y)) {
                unit_avoidance_probe_stage = 10;
              }
              unit_avoidance_probe_stage =
                  unit_avoidance_verified ? 5 : unit_avoidance_probe_stage;
            }
          }
        }
      }
    }
    bool portrait_verified = true;
    if (portrait_probe) {
      portrait_verified = false;
      clear_selection(status);
      for (ScenarioUnitPreview &unit : status.units) {
        unit.selected = true;
        UnitPortraitAsset *const portrait = selected_portrait(status);
        if (portrait != nullptr && portrait->portrait.ready &&
            portrait->portrait.frame.width != 0 &&
            portrait->portrait.frame.height != 0 &&
            portrait->portrait.frame.bgra.size() ==
                static_cast<std::size_t>(portrait->portrait.frame.width) *
                    portrait->portrait.frame.height) {
          const std::uint32_t next_tick = portrait->portrait.last_frame_tick +
                                          portrait->portrait.frame_delay_ms;
          portrait_verified = advance_selected_portrait(status, next_tick) &&
                              portrait->portrait.ready &&
                              !portrait->portrait.frame.bgra.empty();
          break;
        }
        unit.selected = false;
      }
    }
    bool worker_actions_verified = true;
    if (worker_actions_probe) {
      worker_actions_verified = false;
      ScenarioUnitPreview *const worker_pointer = last_local_scv();
      if (production_verified && worker_pointer != nullptr) {
        clear_selection(status);
        ScenarioUnitPreview &worker = *worker_pointer;
        worker.selected = true;
        const auto move_to_interaction_point =
            [&](ScenarioUnitPreview &target, const ActiveUnitOrder order) {
              if (!begin_scv_interaction(status, worker, target, order)) {
                return false;
              }
              worker.x = worker.movement_final_x;
              worker.y = worker.movement_final_y;
              worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
              worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
              stop_unit_movement(status, worker);
              return unit_edge_distance(worker, target) <=
                     (order == ActiveUnitOrder::attack
                          ? static_cast<int>(worker.weapon_range)
                          : 8);
            };

        auto enemy = std::find_if(status.units.begin(), status.units.end(),
                                  [](const ScenarioUnitPreview &unit) {
                                    return unit.alive && unit.owner != 0 &&
                                           unit.owner != 11 &&
                                           unit.max_hit_points != 0;
                                  });
        bool attack_verified{};
        if (enemy != status.units.end()) {
          enemy->hit_points = 128;
          attack_verified =
              move_to_interaction_point(*enemy, ActiveUnitOrder::attack);
          if (attack_verified) {
            SendMessageA(window, WM_TIMER, 1, 0);
            attack_verified = !enemy->alive && enemy->hit_points == 0 &&
                              worker.active_order == ActiveUnitOrder::none;
          }
        }

        auto command_center = std::find_if(
            status.units.begin(), status.units.end(),
            [](const ScenarioUnitPreview &unit) {
              return unit.alive && unit.owner == 0 && unit.unit_type == 106;
            });
        bool repair_verified{};
        if (attack_verified && command_center != status.units.end()) {
          const std::uint32_t damage =
              (std::max)(256U, static_cast<std::uint32_t>(
                                   command_center->repair_step) *
                                   2U);
          command_center->hit_points = command_center->max_hit_points - damage;
          status.player_minerals = 10000;
          status.player_gas = 10000;
          repair_verified = move_to_interaction_point(*command_center,
                                                      ActiveUnitOrder::repair);
          for (int tick = 0;
               tick < 8 && repair_verified &&
               command_center->hit_points < command_center->max_hit_points;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
          }
          repair_verified =
              repair_verified &&
              command_center->hit_points == command_center->max_hit_points &&
              worker.active_order == ActiveUnitOrder::none;
        }

        auto mineral = std::find_if(status.units.begin(), status.units.end(),
                                    [](const ScenarioUnitPreview &unit) {
                                      return unit.alive &&
                                             unit.unit_type >= 176U &&
                                             unit.unit_type <= 178U &&
                                             unit.resource_amount >= 10U;
                                    });
        bool gather_verified{};
        if (repair_verified && mineral != status.units.end()) {
          const std::uint16_t resource_before = mineral->resource_amount;
          const std::uint32_t minerals_before = status.player_minerals;
          const std::uint32_t sounds_before = window_state.audio_play_count;
          bool mining_animation_observed{};
          bool mining_weapon_observed{};
          gather_verified =
              move_to_interaction_point(*mineral, ActiveUnitOrder::gather);
          for (int tick = 0;
               tick < 140 && gather_verified &&
               worker.active_order != ActiveUnitOrder::return_cargo;
               ++tick) {
            SendMessageA(window, WM_TIMER, 1, 0);
            mining_animation_observed =
                mining_animation_observed ||
                (worker.iscript_ready && worker.last_animation == 15U &&
                 (worker.current_sprite_frame == 51U ||
                  worker.current_sprite_frame == 102U));
            mining_weapon_observed =
                mining_weapon_observed ||
                (worker.iscript_state.weapon_event_count != 0U &&
                 worker.iscript_state.weapon_event == 8U);
          }
          const bool cargo_assets_valid =
              status.mineral_cargo_asset_index < status.unit_assets.size() &&
              status.terran_gas_cargo_asset_index < status.unit_assets.size() &&
              status.unit_assets[status.mineral_cargo_asset_index].image_id ==
                  357U &&
              status.unit_assets[status.terran_gas_cargo_asset_index]
                      .image_id == 359U;
          gather_verified =
              gather_verified &&
              worker.active_order == ActiveUnitOrder::return_cargo &&
              worker.cargo_minerals == 8 &&
              mineral->resource_amount + 10U == resource_before &&
              mining_animation_observed && mining_weapon_observed &&
              window_state.audio_play_count > sounds_before &&
              cargo_assets_valid && render_opengl(window, window_state);
          if (gather_verified) {
            ScenarioUnitPreview *const depot =
                find_unit_by_id(status, worker.order_target_id);
            gather_verified = depot != nullptr;
            if (depot != nullptr) {
              worker.x = worker.movement_final_x;
              worker.y = worker.movement_final_y;
              worker.x_fixed = static_cast<std::int32_t>(worker.x) << 8U;
              worker.y_fixed = static_cast<std::int32_t>(worker.y) << 8U;
              stop_unit_movement(status, worker);
              SendMessageA(window, WM_TIMER, 1, 0);
              gather_verified =
                  status.player_minerals == minerals_before + 8U &&
                  worker.cargo_minerals == 0 &&
                  (worker.active_order == ActiveUnitOrder::gather ||
                   worker.active_order == ActiveUnitOrder::none);
            }
          }
        }
        worker_actions_verified =
            attack_verified && repair_verified && gather_verified;
      }
    }
    bool refinery_placement_verified = true;
    int refinery_probe_stage{};
    if (refinery_placement_probe) {
      refinery_placement_verified = false;
      const BuildableUnitVisual *const refinery =
          find_buildable_unit(status, 110U);
      auto geyser = std::find_if(status.units.begin(), status.units.end(),
                                 [](const ScenarioUnitPreview &unit) {
                                   return unit.alive &&
                                          unit.unit_type == 188U &&
                                          unit.resource_amount != 0;
                                 });
      if (refinery != nullptr && geyser == status.units.end() &&
          refinery->asset_index < status.unit_assets.size()) {
        const UnitRenderAsset &asset =
            status.unit_assets[refinery->asset_index];
        ScenarioUnitPreview synthetic{};
        synthetic.unit_id = status.next_unit_id++;
        synthetic.unit_type = 188;
        synthetic.owner = 11;
        synthetic.asset_index = refinery->asset_index;
        synthetic.selection_width = refinery->placement_width;
        synthetic.selection_height = refinery->placement_height;
        synthetic.iscript_state = asset.initial_iscript_state;
        synthetic.overlay_iscript_state = asset.initial_overlay_iscript_state;
        synthetic.current_sprite_frame = asset.initial_iscript_state.frame;
        synthetic.current_overlay_frame =
            asset.initial_overlay_iscript_state.frame;
        synthetic.iscript_ready = asset.iscript_ready;
        synthetic.overlay_ready = asset.overlay_ready;
        synthetic.is_building = true;
        apply_simulation_traits(synthetic, refinery->simulation);
        synthetic.dat_flags |= 0x2000U;
        synthetic.resource_amount = 5000;
        status.units.push_back(std::move(synthetic));
        ScenarioUnitPreview &inserted = status.units.back();
        const int half_width = refinery->placement_width / 2;
        const int half_height = refinery->placement_height / 2;
        bool site_found{};
        for (int y = half_height;
             y < status.pathing_map.pixel_height() - half_height && !site_found;
             y += 32) {
          for (int x = half_width;
               x < status.pathing_map.pixel_width() - half_width; x += 32) {
            inserted.x = static_cast<std::uint16_t>(x);
            inserted.y = static_cast<std::uint16_t>(y);
            inserted.x_fixed = x << 8U;
            inserted.y_fixed = y << 8U;
            if (placement_is_valid(status, *refinery, inserted.x, inserted.y)) {
              site_found = true;
              break;
            }
          }
        }
        if (!site_found) {
          inserted.alive = false;
        }
        geyser = std::find_if(status.units.begin(), status.units.end(),
                              [](const ScenarioUnitPreview &unit) {
                                return unit.alive && unit.unit_type == 188U &&
                                       unit.resource_amount != 0;
                              });
      }
      refinery_probe_stage = refinery != nullptr ? 1 : 0;
      refinery_probe_stage =
          geyser != status.units.end() ? 2 : refinery_probe_stage;
      if (refinery != nullptr && geyser != status.units.end()) {
        refinery_probe_stage = 3;
        clear_selection(status);
        const auto worker = std::find_if(
            status.units.rbegin(), status.units.rend(),
            [](const ScenarioUnitPreview &unit) {
              return unit.alive && unit.owner == 0 && unit.unit_type == 7;
            });
        if (worker != status.units.rend()) {
          worker->selected = true;
        }
        status.player_minerals = 10000;
        status.player_gas = 10000;
        status.placement_active = true;
        status.placement_unit_type = 110U;
        status.placement_x = geyser->x;
        status.placement_y = geyser->y;
        status.placement_valid = placement_is_valid(
            status, *refinery, status.placement_x, status.placement_y);
        refinery_probe_stage =
            status.placement_valid ? 4 : refinery_probe_stage;
        const std::uint32_t geyser_id = geyser->unit_id;
        const std::uint16_t resource_amount = geyser->resource_amount;
        const bool placed =
            status.placement_valid && place_current_building(status);
        refinery_probe_stage = placed ? 5 : refinery_probe_stage;
        const ScenarioUnitPreview *const consumed =
            find_unit_by_id(status, geyser_id);
        const auto created =
            std::find_if(status.units.rbegin(), status.units.rend(),
                         [resource_amount](const ScenarioUnitPreview &unit) {
                           return unit.alive && unit.unit_type == 110U &&
                                  unit.resource_amount == resource_amount;
                         });
        refinery_placement_verified =
            placed && consumed == nullptr && created != status.units.rend();
        refinery_probe_stage = consumed == nullptr ? 6 : refinery_probe_stage;
        refinery_probe_stage =
            created != status.units.rend() ? 7 : refinery_probe_stage;
      }
    }
    bool command_target_verified = true;
    int command_target_probe_stage{};
    if (command_target_probe) {
      command_target_verified = false;
      ScenarioUnitPreview *const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        command_target_probe_stage = 1;
        clear_selection(status);
        ScenarioUnitPreview &scv = *scv_pointer;
        scv.selected = true;
        status.active_command_card = 0;
        cancel_command_target(status);
        const int scv_game_x = scv.x - status.camera_x;
        const int scv_game_y = scv.y - status.camera_y;
        int target_game_x{-1};
        int target_game_y{-1};
        for (int y = 32; y <= 240 && target_game_x < 0; y += 16) {
          for (int x = 32; x <= 608; x += 16) {
            if (std::abs(x - scv_game_x) < 64 &&
                std::abs(y - scv_game_y) < 64) {
              continue;
            }
            const int world_x = x + status.camera_x;
            const int world_y = y + status.camera_y;
            const bool occupied = std::any_of(
                status.units.begin(), status.units.end(),
                [world_x, world_y](const ScenarioUnitPreview &unit) {
                  return world_x >= unit.x - unit.selection_width / 2 &&
                         world_x <= unit.x + unit.selection_width / 2 &&
                         world_y >= unit.y - unit.selection_height / 2 &&
                         world_y <= unit.y + unit.selection_height / 2;
                });
            if (!occupied) {
              target_game_x = x;
              target_game_y = y;
              break;
            }
          }
        }
        const bool move_target_started =
            target_game_x >= 0 && click_command(1) &&
            status.command_target_active && status.target_unit_order == 0x32 &&
            status.target_terrain_order == 7 &&
            command_card_for(status).count == 1;
        command_target_probe_stage =
            move_target_started ? 2 : command_target_probe_stage;
        const LPARAM target = client_point(target_game_x, target_game_y);
        if (move_target_started) {
          SendMessageA(window, WM_MOUSEMOVE, 0, target);
          SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON, target);
          SendMessageA(window, WM_LBUTTONUP, 0, target);
        }
        const bool move_target_issued =
            move_target_started && !status.command_target_active &&
            status.last_issued_order == 7 && scv.moving;
        command_target_probe_stage =
            move_target_issued ? 3 : command_target_probe_stage;
        const bool stopped = move_target_issued && click_command(2) &&
                             status.last_command_opcode == 30 && !scv.moving;
        command_target_probe_stage = stopped ? 4 : command_target_probe_stage;
        const bool attack_started =
            stopped && click_command(3) && status.command_target_active &&
            status.target_unit_order == 9 && status.target_terrain_order == 15;
        command_target_probe_stage =
            attack_started ? 5 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool repair_started =
            attack_started && !status.command_target_active &&
            click_command(4) && status.command_target_active &&
            status.target_unit_order == 0x24 &&
            status.target_terrain_order == 7;
        command_target_probe_stage =
            repair_started ? 6 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool gather_started =
            repair_started && !status.command_target_active &&
            click_command(5) && status.command_target_active &&
            status.target_unit_order == 0x4E &&
            status.target_terrain_order == 7;
        command_target_probe_stage =
            gather_started ? 7 : command_target_probe_stage;
        SendMessageA(window, WM_KEYDOWN, VK_ESCAPE, 0);
        const bool cargo_returned =
            gather_started && !status.command_target_active &&
            click_command(6) && status.last_command_opcode == 34 && scv.moving;
        command_target_probe_stage =
            cargo_returned ? 8 : command_target_probe_stage;
        command_target_verified = cargo_returned;
      }
    }
    bool building_placement_verified = true;
    if (building_placement_probe) {
      building_placement_verified = false;
      ScenarioUnitPreview *const scv_pointer = last_local_scv();
      if (production_verified && scv_pointer != nullptr && probe_client_ready) {
        clear_selection(status);
        scv_pointer->selected = true;
        status.active_command_card = 0;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        const std::size_t command_centers_before = static_cast<std::size_t>(
            std::count_if(status.units.begin(), status.units.end(),
                          [](const ScenarioUnitPreview &unit) {
                            return unit.unit_type == 106;
                          }));
        const bool basic_card_opened =
            click_command(7) && status.active_command_card == 237;
        const bool placement_started = basic_card_opened && click_command(1) &&
                                       status.placement_active &&
                                       status.placement_unit_type == 106;
        int placement_game_x{};
        int placement_game_y{};
        bool valid_target{};
        if (placement_started) {
          for (int y = 48; y < 250 && !valid_target; y += 16) {
            for (int x = 48; x < 592; x += 16) {
              if (update_building_placement(status, x, y) &&
                  status.placement_valid) {
                placement_game_x = x;
                placement_game_y = y;
                valid_target = true;
                break;
              }
            }
          }
        }
        if (valid_target) {
          SendMessageA(window, WM_LBUTTONDOWN, MK_LBUTTON,
                       client_point(placement_game_x, placement_game_y));
        }
        const std::size_t command_centers_after = static_cast<std::size_t>(
            std::count_if(status.units.begin(), status.units.end(),
                          [](const ScenarioUnitPreview &unit) {
                            return unit.unit_type == 106;
                          }));
        building_placement_verified =
            valid_target && !status.placement_active &&
            command_centers_after == command_centers_before + 1U;
      }
    }
    bool race_construction_verified = true;
    int race_construction_probe_stage{};
    if (race_construction_probe) {
      race_construction_verified = false;
      status.player_minerals = 10000U;
      status.player_gas = 10000U;
      const auto insert_worker =
          [&](const std::uint16_t unit_type) -> std::uint32_t {
        ScenarioUnitPreview worker{};
        worker.unit_id = status.next_unit_id++;
        worker.owner = 0U;
        if (!configure_preview_type(status, worker, unit_type)) {
          return 0U;
        }
        for (int y = 64; y < status.pathing_map.pixel_height() - 64;
             y += 32) {
          for (int x = 64; x < status.pathing_map.pixel_width() - 64;
               x += 32) {
            if (!creation_position_passable(status, worker, x, y)) {
              continue;
            }
            worker.x = static_cast<std::uint16_t>(x);
            worker.y = static_cast<std::uint16_t>(y);
            worker.x_fixed = x << 8U;
            worker.y_fixed = y << 8U;
            status.units.push_back(std::move(worker));
            return status.units.back().unit_id;
          }
        }
        return 0U;
      };
      const auto place_for_worker =
          [&](const std::uint32_t worker_id,
              const std::uint16_t building_type) -> bool {
        ScenarioUnitPreview *const worker = find_unit_by_id(status, worker_id);
        const BuildableUnitVisual *const buildable =
            find_buildable_unit(status, building_type);
        if (worker == nullptr || buildable == nullptr) {
          return false;
        }
        clear_selection(status);
        worker->selected = true;
        status.placement_active = true;
        status.placement_unit_type = building_type;
        const int half_width = buildable->placement_width / 2;
        const int half_height = buildable->placement_height / 2;
        for (int y = half_height;
             y + half_height <= status.pathing_map.pixel_height(); y += 32) {
          for (int x = half_width;
               x + half_width <= status.pathing_map.pixel_width(); x += 32) {
            status.placement_x = static_cast<std::uint16_t>(x);
            status.placement_y = static_cast<std::uint16_t>(y);
            if (!placement_is_valid(status, *buildable, status.placement_x,
                                    status.placement_y)) {
              continue;
            }
            status.placement_valid = true;
            return place_current_building(status);
          }
        }
        return false;
      };

      const std::uint32_t drone_id =
          insert_worker(starcraft::lang::zerg_drone_type);
      const std::size_t before_drone_morph = status.units.size();
      const bool drone_placed =
          drone_id != 0U && place_for_worker(drone_id, 131U);
      ScenarioUnitPreview *zerg_building = find_unit_by_id(status, drone_id);
      const bool drone_morphed =
          drone_placed && status.units.size() == before_drone_morph &&
          zerg_building != nullptr && zerg_building->unit_type == 131U &&
          !zerg_building->construction_complete &&
          zerg_building->construction_builder_id == 0U;
      race_construction_probe_stage = drone_morphed ? 1 : 0;
      if (drone_morphed) {
        zerg_building->construction_ticks_remaining = 1U;
        for (int tick = 0; tick < 128 && !zerg_building->construction_complete;
             ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
          zerg_building = find_unit_by_id(status, drone_id);
          if (zerg_building == nullptr) {
            break;
          }
        }
      }
      const bool zerg_completed =
          zerg_building != nullptr && zerg_building->construction_complete &&
          zerg_building->hit_points == zerg_building->max_hit_points;
      race_construction_probe_stage =
          zerg_completed ? 2 : race_construction_probe_stage;
      if (zerg_building != nullptr) {
        zerg_building->alive = false;
        zerg_building->selected = false;
      }

      const std::uint32_t probe_id =
          insert_worker(starcraft::lang::protoss_probe_type);
      const std::size_t before_probe_build = status.units.size();
      const bool nexus_placed =
          probe_id != 0U && place_for_worker(probe_id, 154U);
      ScenarioUnitPreview *probe = find_unit_by_id(status, probe_id);
      ScenarioUnitPreview *nexus =
          nexus_placed && status.units.size() == before_probe_build + 1U
              ? &status.units.back()
              : nullptr;
      const std::uint32_t nexus_id = nexus == nullptr ? 0U : nexus->unit_id;
      const bool probe_released =
          nexus != nullptr && nexus->unit_type == 154U &&
          !nexus->construction_complete &&
          nexus->construction_builder_id == 0U && probe != nullptr &&
          probe->alive && probe->unit_type == starcraft::lang::protoss_probe_type &&
          probe->active_order == ActiveUnitOrder::none;
      race_construction_probe_stage =
          probe_released ? 3 : race_construction_probe_stage;
      if (probe_released) {
        nexus->construction_ticks_remaining = 1U;
        for (int tick = 0; tick < 128 && !nexus->construction_complete;
             ++tick) {
          SendMessageA(window, WM_TIMER, 1, 0);
          nexus = find_unit_by_id(status, nexus_id);
          if (nexus == nullptr) {
            break;
          }
        }
      }
      const bool protoss_completed =
          nexus != nullptr && nexus->construction_complete &&
          nexus->hit_points == nexus->max_hit_points;
      race_construction_verified = zerg_completed && probe_released &&
                                   protoss_completed;
      race_construction_probe_stage =
          !drone_morphed   ? 0
          : !zerg_completed
              ? static_cast<int>(20 +
                                 zerg_building->construction_animation_phase)
          : !probe_released ? 2
          : !protoss_completed
              ? static_cast<int>(10 + nexus->construction_animation_phase)
              : 4;
    }
    bool construction_verified = true;
    bool construction_status_verified = true;
    bool construction_animation_verified = true;
    bool building_idle_animation_verified = true;
    int construction_probe_stage{};
    if (construction_probe || construction_status_probe) {
      construction_verified = !construction_probe;
      construction_status_verified = !construction_status_probe;
      construction_animation_verified = !construction_probe;
      building_idle_animation_verified = !construction_probe;
      const BuildableUnitVisual *const command_center =
          find_buildable_unit(status, 106U);
      construction_probe_stage = command_center != nullptr ? 1 : 0;
      auto worker = std::find_if(status.units.rbegin(), status.units.rend(),
                                 [](const ScenarioUnitPreview &unit) {
                                   return unit.alive && unit.owner == 0 &&
                                          unit.unit_type == 7;
                                 });
      construction_probe_stage =
          worker != status.units.rend() ? 2 : construction_probe_stage;
      if (production_verified && command_center != nullptr &&
          worker != status.units.rend()) {
        construction_probe_stage = 3;
        clear_selection(status);
        worker->selected = true;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        status.placement_active = true;
        status.placement_unit_type = 106U;
        bool site_found{};
        const int half_width = command_center->placement_width / 2;
        const int half_height = command_center->placement_height / 2;
        for (int y = half_height;
             y + half_height <= status.pathing_map.pixel_height() &&
             !site_found;
             y += 32) {
          for (int x = half_width;
               x + half_width <= status.pathing_map.pixel_width(); x += 32) {
            status.placement_x = static_cast<std::uint16_t>(x);
            status.placement_y = static_cast<std::uint16_t>(y);
            site_found =
                placement_is_valid(status, *command_center, status.placement_x,
                                   status.placement_y);
            if (site_found) {
              break;
            }
          }
        }
        construction_probe_stage = site_found ? 4 : construction_probe_stage;
        status.placement_valid = site_found;
        const bool placed = site_found && place_current_building(status);
        construction_probe_stage = placed ? 5 : construction_probe_stage;
        ScenarioUnitPreview *const created =
            placed && !status.units.empty() ? &status.units.back() : nullptr;
        ScenarioUnitPreview *const builder =
            created == nullptr
                ? nullptr
                : find_unit_by_id(status, created->construction_builder_id);
        const bool started =
            created != nullptr && created->unit_type == 106U &&
            !created->construction_complete &&
            created->construction_ticks_total != 0U &&
            created->construction_ticks_remaining ==
                created->construction_ticks_total &&
            created->hit_points ==
                (std::max)(1U, created->max_hit_points / 10U) &&
            builder != nullptr &&
            builder->active_order == ActiveUnitOrder::construct &&
            builder->order_target_id == created->unit_id;
        construction_probe_stage =
            created != nullptr ? 6 : construction_probe_stage;
        construction_probe_stage =
            builder != nullptr ? 7 : construction_probe_stage;
        construction_probe_stage = started ? 8 : construction_probe_stage;
        bool advanced{};
        bool incomplete_card_gated{};
        if (started) {
          clear_selection(status);
          created->selected = true;
          incomplete_card_gated = command_card_for(status).count == 0U;
          created->selected = false;
          builder->selected = true;
          builder->x = builder->movement_final_x;
          builder->y = builder->movement_final_y;
          builder->x_fixed = static_cast<std::int32_t>(builder->x) << 8U;
          builder->y_fixed = static_cast<std::int32_t>(builder->y) << 8U;
          stop_unit_movement(status, *builder);
          const std::uint16_t remaining_before =
              created->construction_ticks_remaining;
          const std::uint32_t life_before = created->hit_points;
          SendMessageA(window, WM_TIMER, 1, 0);
          advanced =
              created->construction_ticks_remaining + 1U == remaining_before &&
              created->hit_points > life_before &&
              !created->construction_complete;
          construction_probe_stage = advanced ? 9 : construction_probe_stage;
          if (construction_status_probe) {
            created->construction_ticks_remaining = static_cast<std::uint16_t>(
                (std::max)(1U, static_cast<unsigned>(
                                   created->construction_ticks_total) /
                                   2U));
            const std::uint32_t initial_life =
                (std::max)(1U, created->max_hit_points / 10U);
            const std::uint32_t completed_ticks =
                created->construction_ticks_total -
                created->construction_ticks_remaining;
            created->hit_points =
                (std::min)(created->max_hit_points,
                           initial_life +
                               completed_ticks * created->repair_step);
            clear_selection(status);
            created->selected = true;
            construction_status_verified =
                incomplete_card_gated && advanced &&
                !created->construction_complete &&
                created->construction_ticks_remaining != 0U &&
                created->construction_ticks_remaining <
                    created->construction_ticks_total &&
                created->hit_points > initial_life &&
                status.status_construction_label_control.left == 240 &&
                status.status_construction_progress_control.left == 263;
          }
          if (construction_probe) {
            created->construction_animation_phase = 0U;
            created->hit_points = created->max_hit_points / 5U + 1U;
            const bool crossed_one =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 1U;
            const bool played_one =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 2U;
            created->hit_points = 2U * created->max_hit_points / 5U + 1U;
            const bool crossed_two =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 3U;
            const bool played_two =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 4U;
            created->hit_points = 3U * created->max_hit_points / 5U + 1U;
            const bool crossed_three =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 5U;
            const bool played_three =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 6U;
            created->hit_points = 4U * created->max_hit_points / 5U + 1U;
            const bool crossed_four =
                advance_building_construction_animation(status, *created) &&
                created->construction_animation_phase == 7U;
            construction_animation_verified =
                crossed_one && played_one && crossed_two && played_two &&
                crossed_three && played_three && crossed_four &&
                created->iscript_ready;
            created->construction_ticks_remaining = 1;
            SendMessageA(window, WM_TIMER, 1, 0);
            const std::uint16_t idle_program_before =
                created->iscript_state.program_counter;
            const std::uint8_t idle_sleep_before =
                created->iscript_state.sleep_ticks;
            SendMessageA(window, WM_TIMER, 1, 0);
            building_idle_animation_verified =
                created->iscript_ready &&
                (created->iscript_state.program_counter !=
                     idle_program_before ||
                 created->iscript_state.sleep_ticks != idle_sleep_before);
            clear_selection(status);
            created->selected = true;
            construction_probe_stage =
                created->construction_complete ? 10 : construction_probe_stage;
            construction_probe_stage =
                created->construction_ticks_remaining == 0U
                    ? 11
                    : construction_probe_stage;
            construction_probe_stage =
                created->hit_points == created->max_hit_points
                    ? 12
                    : construction_probe_stage;
            construction_probe_stage = created->construction_builder_id == 0U
                                           ? 13
                                           : construction_probe_stage;
            construction_probe_stage =
                builder->active_order == ActiveUnitOrder::none
                    ? 14
                    : construction_probe_stage;
            construction_probe_stage = command_card_for(status).count == 4U
                                           ? 15
                                           : construction_probe_stage;
            construction_verified =
                incomplete_card_gated && advanced &&
                construction_animation_verified &&
                building_idle_animation_verified &&
                created->construction_complete &&
                created->construction_ticks_remaining == 0U &&
                created->hit_points == created->max_hit_points &&
                created->construction_builder_id == 0U &&
                builder->active_order == ActiveUnitOrder::none &&
                command_card_for(status).count == 4U;
            construction_probe_stage =
                construction_verified ? 16 : construction_probe_stage;
          }
        }
      }
    }
    bool addon_verified = true;
    if (addon_probe) {
      addon_verified = false;
      ScenarioUnitPreview *parent{};
      for (ScenarioUnitPreview &unit : status.units) {
        if (unit.alive && unit.owner == 0 && unit.unit_type == 106U &&
            unit.construction_complete && unit.attached_addon_id == 0U) {
          parent = &unit;
          break;
        }
      }
      const BuildableUnitVisual *const addon =
          find_buildable_unit(status, 107U);
      std::uint16_t expected_x{};
      std::uint16_t expected_y{};
      if (parent != nullptr && addon != nullptr && probe_client_ready &&
          addon_center_for_parent(*addon, *parent, expected_x, expected_y)) {
        clear_selection(status);
        parent->selected = true;
        status.player_minerals = 10000;
        status.player_gas = 10000;
        const bool exact_site =
            placement_is_valid(status, *addon, expected_x, expected_y);
        const bool wrong_site_rejected = !placement_is_valid(
            status, *addon, static_cast<std::uint16_t>(expected_x + 32U),
            expected_y);
        const std::size_t units_before = status.units.size();
        const bool command_accepted =
            exact_site && wrong_site_rejected && click_command(7);
        ScenarioUnitPreview *const created =
            command_accepted && status.units.size() == units_before + 1U
                ? &status.units.back()
                : nullptr;
        const bool linked = created != nullptr && created->unit_type == 107U &&
                            created->addon_parent_id == parent->unit_id &&
                            parent->attached_addon_id == created->unit_id &&
                            created->x == expected_x &&
                            created->y == expected_y &&
                            !created->construction_complete &&
                            created->hit_points ==
                                (std::max)(1U, created->max_hit_points / 10U);
        if (linked) {
          created->construction_ticks_remaining = 1;
          SendMessageA(window, WM_TIMER, 1, 0);
          const std::size_t units_after = status.units.size();
          (void)click_command(8);
          addon_verified = created->construction_complete &&
                           created->hit_points == created->max_hit_points &&
                           parent->attached_addon_id == created->unit_id &&
                           status.units.size() == units_after;
        }
      }
    }
    bool creep_verified = true;
    int creep_probe_stage{};
    if (creep_probe) {
      creep_verified = false;
      BuildableUnitVisual requires_creep{};
      requires_creep.unit_type = 143U;
      requires_creep.placement_width = 32;
      requires_creep.placement_height = 32;
      requires_creep.simulation.dat_flags = 0x20001U;
      std::uint16_t target_x{};
      std::uint16_t target_y{};
      std::uint16_t source_x{};
      std::uint16_t source_y{};
      bool site_found{};
      const auto unoccupied = [&](const int x, const int y) {
        ScenarioUnitPreview footprint{};
        footprint.selection_width = 32;
        footprint.selection_height = 32;
        return std::none_of(
            status.units.begin(), status.units.end(),
            [&footprint, x, y](const ScenarioUnitPreview &unit) {
              return unit.alive &&
                     unit_footprints_overlap_at(footprint, x, y, unit);
            });
      };
      for (int y = 16;
           y < status.pathing_map.pixel_height() - 16 && !site_found; y += 32) {
        for (int x = 16; x + 112 < status.pathing_map.pixel_width(); x += 32) {
          std::uint16_t target_tile{};
          std::uint16_t source_tile{};
          if (status.scenario.tile(static_cast<std::uint16_t>(x / 32),
                                   static_cast<std::uint16_t>(y / 32),
                                   target_tile) &&
              status.scenario.tile(static_cast<std::uint16_t>((x + 96) / 32),
                                   static_cast<std::uint16_t>(y / 32),
                                   source_tile) &&
              status.terrain_tileset.buildable(target_tile) &&
              status.terrain_tileset.buildable(source_tile) &&
              unoccupied(x, y) && unoccupied(x + 96, y)) {
            target_x = static_cast<std::uint16_t>(x);
            target_y = static_cast<std::uint16_t>(y);
            source_x = static_cast<std::uint16_t>(x + 96);
            source_y = target_y;
            site_found = true;
            break;
          }
        }
      }
      if (site_found) {
        creep_probe_stage = 1;
        (void)rebuild_creep_tiles(status);
        const std::vector<std::uint8_t> creep_before = status.creep_tiles;
        const bool rejected_without_creep =
            !placement_is_valid(status, requires_creep, target_x, target_y);
        creep_probe_stage = rejected_without_creep ? 2 : creep_probe_stage;
        ScenarioUnitPreview source{};
        source.unit_id = status.next_unit_id++;
        source.x = source_x;
        source.y = source_y;
        source.x_fixed = static_cast<std::int32_t>(source.x) << 8U;
        source.y_fixed = static_cast<std::int32_t>(source.y) << 8U;
        source.unit_type = 131U;
        source.owner = 1;
        source.selection_width = 32;
        source.selection_height = 32;
        source.is_building = true;
        const std::uint32_t source_id = source.unit_id;
        status.units.push_back(std::move(source));
        const bool spread =
            rebuild_creep_tiles(status) &&
            placement_is_valid(status, requires_creep, target_x, target_y);
        creep_probe_stage = spread ? 3 : creep_probe_stage;
        bool exact_ellipse =
            spread && creep_before.size() == status.creep_tiles.size() &&
            status.creep_visual_tiles.size() == status.creep_tiles.size();
        for (int tile_y = 0; exact_ellipse && tile_y < status.scenario_height;
             ++tile_y) {
          const int dy = 32 * tile_y - static_cast<int>(source_y) + 16;
          for (int tile_x = 0; tile_x < status.scenario_width; ++tile_x) {
            const int dx = 32 * tile_x - static_cast<int>(source_x) + 16;
            const std::size_t tile =
                static_cast<std::size_t>(tile_y) * status.scenario_width +
                tile_x;
            const bool in_recovered_ellipse =
                256LL * dy * dy + 100LL * dx * dx <= 10240000LL;
            const bool expected =
                creep_before[tile] != 0U || in_recovered_ellipse;
            if ((status.creep_tiles[tile] != 0U) != expected ||
                (expected && (status.creep_visual_tiles[tile] < 16U ||
                              status.creep_visual_tiles[tile] > 24U))) {
              exact_ellipse = false;
              break;
            }
          }
        }
        creep_probe_stage = exact_ellipse ? 4 : creep_probe_stage;
        ScenarioUnitPreview *const inserted =
            find_unit_by_id(status, source_id);
        if (inserted != nullptr) {
          inserted->alive = false;
        }
        const bool receded =
            rebuild_creep_tiles(status) &&
            !placement_is_valid(status, requires_creep, target_x, target_y) &&
            status.creep_tiles == creep_before;
        creep_probe_stage = receded ? 5 : creep_probe_stage;
        creep_verified = rejected_without_creep && spread && exact_ellipse &&
                         receded && status.terrain_ready &&
                         status.minimap_ready;
        creep_probe_stage = creep_verified ? 6 : creep_probe_stage;
      }
    }
    bool melee_start_verified = true;
    if (melee_start_probe) {
      melee_start_verified = status.local_race < 3U;
      for (std::size_t player = 0; melee_start_verified && player < 8U;
           ++player) {
        if (!status.active_players[player]) {
          continue;
        }
        const std::uint8_t race = status.scenario.players()[player].race;
        const starcraft::game::ScenarioStartLocation &start =
            status.scenario.start_locations()[player];
        if (race >= 3U || !start.present) {
          melee_start_verified = false;
          break;
        }
        starcraft::lang::MeleeUnitTypes unit_types{};
        if (!starcraft::lang::melee_unit_types(race, unit_types)) {
          melee_start_verified = false;
          break;
        }
        const std::uint16_t worker_type = unit_types.worker;
        const std::uint16_t base_type = unit_types.base;
        std::size_t workers{};
        const ScenarioUnitPreview *base{};
        for (const ScenarioUnitPreview &unit : status.units) {
          if (!unit.alive || unit.owner != player) {
            continue;
          }
          if (unit.unit_type == worker_type) {
            ++workers;
            melee_start_verified =
                melee_start_verified &&
                std::abs(static_cast<int>(unit.x) - start.x) <= 128 &&
                std::abs(static_cast<int>(unit.y) - start.y) <= 128 &&
                !(unit.x == start.x && unit.y == start.y);
          } else if (unit.unit_type == base_type && base == nullptr) {
            base = &unit;
          }
        }
        if (base == nullptr) {
          melee_start_verified = false;
          break;
        }
        const starcraft::lang::PlacementPoint expected_base =
            starcraft::lang::align_melee_base(start.x, start.y,
                                              base->selection_width,
                                              base->selection_height);
        melee_start_verified =
            workers == starcraft::lang::retail_melee_starting_worker_count &&
            base->x == expected_base.x && base->y == expected_base.y;
        for (std::size_t left = 0;
             melee_start_verified && left < status.units.size(); ++left) {
          const ScenarioUnitPreview &worker = status.units[left];
          if (!worker.alive || worker.owner != player ||
              worker.unit_type != worker_type) {
            continue;
          }
          for (std::size_t right = 0; right < status.units.size(); ++right) {
            const ScenarioUnitPreview &other = status.units[right];
            if (left == right || !other.alive || other.owner != player ||
                (other.unit_type != worker_type &&
                 other.unit_type != base_type)) {
              continue;
            }
            if (unit_footprints_overlap_at(worker, worker.x, worker.y, other)) {
              melee_start_verified = false;
              break;
            }
          }
        }
      }
      const std::array<std::uint32_t, 2> supply = local_supply(status);
      melee_start_verified =
          melee_start_verified && supply[0] == 8U &&
          supply[1] == starcraft::lang::retail_melee_base_supply_internal;
    }
    const bool rendered =
        status.assets_ready && render_opengl(window, window_state);
    const bool captured =
        capture_path == nullptr ||
        (rendered && capture_opengl_bmp(window, window_state, capture_path));
    DestroyWindow(window);
    if (command_target_probe && !command_target_verified) {
      return 20 + command_target_probe_stage;
    }
    if (creep_probe && !creep_verified) {
      return 120 + creep_probe_stage;
    }
    if (refinery_placement_probe && !refinery_placement_verified) {
      return 40 + refinery_probe_stage;
    }
    if (construction_probe && !construction_verified) {
      return 60 + construction_probe_stage;
    }
    if (resource_strip_probe && !resource_strip_verified) {
      return 80 + resource_strip_probe_stage;
    }
    if (unit_avoidance_probe && !unit_avoidance_verified) {
      return 160 + unit_avoidance_probe_stage;
    }
    if (harvest_queue_probe && !harvest_queue_verified) {
      return 180 + harvest_queue_probe_stage;
    }
    if (geyser_probe && !geysers_verified) {
      return 190 + geyser_probe_stage;
    }
    if (all_production_probe && !all_production_verified) {
      return 200 + all_production_probe_stage;
    }
    if (movement_probe && !movement_verified) {
      return 220 + movement_probe_stage;
    }
    if (worker_build_cards_probe && !worker_build_cards_verified) {
      return 260 + worker_build_cards_probe_stage;
    }
    if (race_construction_probe && !race_construction_verified) {
      return 280 + race_construction_probe_stage;
    }
    if (race_building_cards_probe && !race_building_cards_verified) {
      return 300 + race_building_cards_probe_stage;
    }
    return rendered && captured && selection_verified &&
                   command_panel_verified && production_verified &&
                   movement_verified && pathfinding_verified &&
                   portrait_verified && worker_actions_verified &&
                   harvest_visual_verified && building_working_verified &&
                   smart_orders_verified && refinery_placement_verified &&
                   command_target_verified && building_placement_verified &&
                   construction_verified && addon_verified && creep_verified &&
                   melee_start_verified && construction_animation_verified &&
                   building_idle_animation_verified &&
                   unit_avoidance_verified && minimap_verified &&
                   camera_verified && status_panel_verified &&
                   multi_status_verified && construction_status_verified &&
                   resource_feedback_verified && resource_strip_verified &&
                   unit_audio_verified && music_verified &&
                   harvest_queue_verified && geysers_verified &&
                   all_production_verified
                   && worker_build_cards_verified &&
                   race_construction_verified &&
                   race_building_cards_verified
               ? 0
               : 11;
  }

  return 0;
}

} // namespace starcraft::recovery
