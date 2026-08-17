#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

bool sprite_draws_before(const ScenarioUnitPreview &left,
                         const ScenarioUnitPreview &right) noexcept {
  // Priority.cpp::sub_482620 packs CSprite+0x11 as the most-significant
  // priority byte. For elevation classes 0..3 it then packs CSprite.y;
  // elevated sprites omit y so that airborne classes remain above all ground
  // sprites. CSprite+0x14 is the final stable tie-breaker; unit_id preserves
  // that allocation order in the recovered runtime.
  if (left.sprite_elevation != right.sprite_elevation) {
    return left.sprite_elevation < right.sprite_elevation;
  }
  if (left.sprite_elevation <= 3U && left.y != right.y) {
    return left.y < right.y;
  }
  return left.unit_id < right.unit_id;
}

bool sprite_intersects_world_viewport(
    const BootstrapStatus &status,
    const ScenarioUnitPreview &unit) noexcept {
  if (!unit.alive || unit.sprite_hidden ||
      unit.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  // The original low-level sprite blitters clip before touching GRP pixels.
  // OpenGL otherwise uploads and translates every unit frame on the map,
  // including every offscreen geyser plume. Canvas bounds plus the signed
  // CImage/attached-image offset range form a conservative equivalent clip.
  int half_width = static_cast<int>(asset.sprite_canvas_width) / 2 + 256;
  int half_height = static_cast<int>(asset.sprite_canvas_height) / 2 + 256;
  if (unit.dynamic_overlay_ready &&
      unit.dynamic_overlay_asset_index < status.unit_assets.size()) {
    const UnitRenderAsset &overlay =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    half_width = (std::max)(
        half_width, static_cast<int>(overlay.sprite_canvas_width) / 2 + 256);
    half_height = (std::max)(
        half_height, static_cast<int>(overlay.sprite_canvas_height) / 2 + 256);
  }
  const int screen_x = static_cast<int>(unit.x) - status.camera_x;
  const int screen_y = static_cast<int>(unit.y) - status.camera_y;
  return screen_x + half_width >= 0 &&
         screen_x - half_width < kMapViewportWidth &&
         screen_y + half_height >= 0 &&
         screen_y - half_height < kMapViewportHeight;
}

bool render_opengl(const HWND window, RecoveryWindowState &state) noexcept {
  if (state.device_context == nullptr || state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    return false;
  }
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 ||
      client.bottom <= 0) {
    return false;
  }
  while (glGetError() != GL_NO_ERROR) {
  }
  const PresentationViewport viewport =
      presentation_viewport(client.right, client.bottom);
  if (viewport.width <= 0 || viewport.height <= 0) {
    return false;
  }
  glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, kMapViewportWidth, kMapViewportHeight, 0.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  const BootstrapStatus *const status = state.status;
  if (glue_active(state.glue)) {
    (void)render_glue(state);
  } else if (status != nullptr && status->terrain_ready) {
    const SpritePreviewFrame &terrain_frame =
        status->fog_of_war_enabled && status->fog_render_surfaces_ready
            ? status->fogged_terrain
            : status->terrain;
    draw_preview_frame_gl(terrain_frame, 0.0F, 0.0F,
                          static_cast<float>(kMapViewportWidth),
                          static_cast<float>(kMapViewportHeight));
    draw_pylon_power_fields_gl(*status);
    try {
      std::vector<const ScenarioUnitPreview *> sprites;
      sprites.reserve(status->units.size() + status->transient_images.size());
      for (const ScenarioUnitPreview &unit : status->units) {
        if (sprite_intersects_world_viewport(*status, unit) &&
            fog_unit_visible(*status, unit)) {
          sprites.push_back(&unit);
        }
      }
      for (const ScenarioUnitPreview &effect : status->transient_images) {
        if (sprite_intersects_world_viewport(*status, effect) &&
            fog_unit_visible(*status, effect)) {
          sprites.push_back(&effect);
        }
      }
      std::stable_sort(
          sprites.begin(), sprites.end(),
          [](const ScenarioUnitPreview *const left,
             const ScenarioUnitPreview *const right) {
            return sprite_draws_before(*left, *right);
          });
      for (const ScenarioUnitPreview *const unit : sprites) {
        draw_scenario_unit_gl(*status, *unit);
      }
    } catch (...) {
      for (const ScenarioUnitPreview &unit : status->units) {
        if (fog_unit_visible(*status, unit)) {
          draw_scenario_unit_gl(*status, unit);
        }
      }
    }
    draw_building_placement_gl(*status);
    draw_selection_drag_gl(state);
    if (status->hud_ready) {
      draw_preview_frame_gl(status->hud, 0.0F, 0.0F,
                            static_cast<float>(kMapViewportWidth),
                            static_cast<float>(kMapViewportHeight));
    }
    draw_resource_strip_gl(state);
    draw_system_message_gl(state, GetTickCount());
    draw_minimap_gl(*status);
    draw_selected_portrait_gl(*status);
    draw_selected_status_panel_gl(state, GetTickCount());
    draw_selected_command_panel_gl(state);
    draw_command_target_gl(state);
    draw_hud_menu_button_gl(state);
    draw_game_dialog_gl(state, GetTickCount());
  }
  draw_debug_console(state, viewport);
  glFlush();
  std::array<std::array<GLint, 2>, 4> sample_points{{
      {{(std::clamp)(viewport.x + viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        viewport.height / 4}},
      {{(std::clamp)(viewport.x + 3 * viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        viewport.height / 4}},
      {{(std::clamp)(viewport.x + viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        3 * viewport.height / 4}},
      {{(std::clamp)(viewport.x + 3 * viewport.width / 4, 0,
                     static_cast<int>(client.right) - 1),
        3 * viewport.height / 4}},
  }};
  bool has_colored_pixel = !state.validate_render_pixels;
  if (state.validate_render_pixels) {
    // glReadPixels synchronizes the CPU with the driver. Keep this exact
    // surface sanity check for the explicit renderer probe, not every
    // 42-millisecond gameplay frame.
    glReadBuffer(GL_BACK);
    for (const auto &point : sample_points) {
      std::array<GLubyte, 4> pixel{};
      glReadPixels(point[0], point[1], 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                   pixel.data());
      has_colored_pixel =
          has_colored_pixel || pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0;
    }
  }
  const bool commands_valid = glGetError() == GL_NO_ERROR;
  const bool decoded_score_surface =
      state.game_dialog.screen == GameDialogScreen::score &&
      state.status != nullptr && state.status->local_race < 3U &&
      !state.game_dialog
           .score_backgrounds[2U * state.status->local_race +
                              (state.game_dialog.outcome ==
                                       MatchOutcome::victory
                                   ? 1U
                                   : 0U)]
           .bgra.empty();
  return SwapBuffers(state.device_context) != FALSE && commands_valid &&
         (has_colored_pixel || decoded_score_surface);
}

} // namespace starcraft::recovery
