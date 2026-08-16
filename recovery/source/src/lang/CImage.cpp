#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/game/image.hpp"
#include "starcraft/gds/grp.hpp"
#include "starcraft/lang/iscript.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {

bool apply_preview_draw_function(const std::uint8_t draw_function,
                                 std::vector<SpritePreviewFrame> &frames) {
  if (draw_function == 0 || draw_function == 14) {
    // Renderer 14 is the worker-inventory image path. sub_411280 stores the
    // carried amount on the image; its GRP pixels and attached cargo overlay
    // remain ordinary palette imagery for the bootstrap's true-color pass.
    return true;
  }
  if (draw_function != 9) {
    return false;
  }

  // Original renderer 9 treats the GRP as a mask and replaces each covered
  // destination palette index through byte_72F8D0. A half-black source-over
  // mask preserves that shadow behavior on this temporary true-color surface.
  for (SpritePreviewFrame &frame : frames) {
    for (std::uint32_t &pixel : frame.bgra) {
      pixel = (pixel & 0xFF000000U) != 0 ? 0x80000000U : 0U;
    }
  }
  return true;
}

bool load_image_preview(starcraft::runtime::StormModule &storm,
                        const starcraft::data::CoreDataSet &data,
                        const std::uint16_t image_id,
                        const std::vector<std::uint8_t> &palette,
                        std::string &path,
                        std::vector<SpritePreviewFrame> &frames,
                        std::uint16_t &canvas_width,
                        std::uint16_t &canvas_height) {
  path = data.image_grp_path(image_id);
  std::vector<std::uint8_t> group;
  return !path.empty() && storm.load_file(path.c_str(), group) &&
         decode_preview_frames(group, palette, frames, canvas_width,
                               canvas_height);
}

bool start_image_script(const std::vector<std::uint8_t> &bytes,
                        const std::uint16_t script_id,
                        starcraft::lang::IScriptState &state,
                        const starcraft::lang::IScriptState *const parent,
                        const std::uint16_t tileset_frame_offset) {
  const starcraft::lang::IScriptProgramView program{bytes.data(), bytes.size()};
  if (!program.valid() || !program.start(script_id, 0, state)) {
    return false;
  }
  const auto result = program.tick(state, 0, 256, parent, tileset_frame_offset);
  return result == starcraft::lang::IScriptTickResult::yielded ||
         result == starcraft::lang::IScriptTickResult::sleeping;
}

bool load_unit_render_asset(starcraft::runtime::StormModule &storm,
                            const starcraft::data::CoreDataSet &data,
                            const std::vector<std::uint8_t> &palette,
                            const std::vector<std::uint8_t> &iscript_bytes,
                            const std::uint16_t image_id,
                            const std::uint16_t tileset_frame_offset,
                            UnitRenderAsset &output) {
  UnitRenderAsset asset{};
  asset.image_id = image_id;
  starcraft::data::ImageRenderTraits main_traits{};
  if (!load_image_preview(storm, data, image_id, palette, asset.sprite_path,
                          asset.sprite_frames, asset.sprite_canvas_width,
                          asset.sprite_canvas_height) ||
      !data.image_render_traits(image_id, main_traits) ||
      !apply_preview_draw_function(main_traits.draw_function,
                                   asset.sprite_frames) ||
      !data.image_iscript_id(image_id, asset.iscript_id) ||
      !start_image_script(iscript_bytes, asset.iscript_id,
                          asset.initial_iscript_state, nullptr,
                          tileset_frame_offset) ||
      asset.initial_iscript_state.frame >= asset.sprite_frames.size()) {
    return false;
  }
  asset.image_draw_function = main_traits.draw_function;
  asset.graphics_turns = main_traits.graphics_turns;
  asset.iscript_ready = true;

  if (asset.initial_iscript_state.overlay_event_count != 0) {
    asset.overlay_image_id = asset.initial_iscript_state.overlay_image;
    asset.overlay_above = asset.initial_iscript_state.overlay_above;
    asset.overlay_x_offset = asset.initial_iscript_state.overlay_x_offset;
    asset.overlay_y_offset = asset.initial_iscript_state.overlay_y_offset;
    std::uint16_t overlay_canvas_width{};
    std::uint16_t overlay_canvas_height{};
    starcraft::data::ImageRenderTraits overlay_traits{};
    if (!load_image_preview(storm, data, asset.overlay_image_id, palette,
                            asset.overlay_path, asset.overlay_frames,
                            overlay_canvas_width, overlay_canvas_height) ||
        !data.image_render_traits(asset.overlay_image_id, overlay_traits) ||
        !apply_preview_draw_function(overlay_traits.draw_function,
                                     asset.overlay_frames) ||
        !data.image_iscript_id(asset.overlay_image_id,
                               asset.overlay_iscript_id) ||
        !start_image_script(iscript_bytes, asset.overlay_iscript_id,
                            asset.initial_overlay_iscript_state,
                            &asset.initial_iscript_state,
                            tileset_frame_offset) ||
        asset.initial_overlay_iscript_state.frame >=
            asset.overlay_frames.size()) {
      return false;
    }
    asset.overlay_draw_function = overlay_traits.draw_function;
    asset.overlay_ready = true;
  }
  output = std::move(asset);
  return true;
}

void draw_scenario_unit_gl(const BootstrapStatus &status,
                           const ScenarioUnitPreview &unit) {
  if (!unit.alive || unit.asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  if (!unit.iscript_ready || unit.iscript_state.hidden ||
      asset.sprite_frames.empty()) {
    return;
  }
  std::size_t sprite_frame = unit.current_sprite_frame;
  bool sprite_mirrored = unit.iscript_state.mirrored;
  if (asset.graphics_turns) {
    const starcraft::game::ImageFacingFrame facing =
        starcraft::game::image_facing_frame(unit.direction);
    sprite_mirrored = sprite_mirrored != facing.mirrored;
    if (sprite_frame + facing.frame < asset.sprite_frames.size()) {
      sprite_frame += facing.frame;
    }
  }
  if (sprite_frame >= asset.sprite_frames.size()) {
    return;
  }
  const int origin_x = unit.x - status.camera_x -
                       static_cast<int>(asset.sprite_canvas_width) / 2;
  const int origin_y = unit.y - status.camera_y -
                       static_cast<int>(asset.sprite_canvas_height) / 2;
  const auto draw_dynamic_overlay = [&](const bool above) {
    if (!unit.dynamic_overlay_ready || unit.dynamic_overlay_above != above ||
        unit.dynamic_overlay_iscript_state.hidden ||
        unit.dynamic_overlay_asset_index >= status.unit_assets.size()) {
      return;
    }
    const UnitRenderAsset &overlay_asset =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    if (unit.current_dynamic_overlay_frame >=
        overlay_asset.sprite_frames.size()) {
      return;
    }
    const SpritePreviewFrame &overlay =
        overlay_asset.sprite_frames[unit.current_dynamic_overlay_frame];
    const float overlay_x =
        static_cast<float>(origin_x + static_cast<int>(overlay.x_offset) +
                           unit.dynamic_overlay_x_offset +
                           unit.dynamic_overlay_iscript_state.x_offset);
    const float overlay_y =
        static_cast<float>(origin_y + static_cast<int>(overlay.y_offset) +
                           unit.dynamic_overlay_y_offset +
                           unit.dynamic_overlay_iscript_state.y_offset);
    if (overlay_asset.image_draw_function == 0) {
      draw_team_colored_frame_gl(status, overlay, unit.owner, overlay_x,
                                 overlay_y, overlay.width, overlay.height);
    } else {
      draw_preview_frame_gl(overlay, overlay_x, overlay_y, overlay.width,
                            overlay.height);
    }
  };
  if (unit.selected) {
    constexpr float pi = 3.14159265358979323846F;
    const float center_x = static_cast<float>(unit.x - status.camera_x);
    const float center_y = static_cast<float>(unit.y - status.camera_y);
    const float radius_x = static_cast<float>(unit.selection_width) / 2.0F;
    const float radius_y = static_cast<float>(unit.selection_height) / 2.0F;
    glDisable(GL_TEXTURE_2D);
    glColor4ub(32, 255, 32, 255);
    glLineWidth(2.0F);
    glBegin(GL_LINE_LOOP);
    for (int segment = 0; segment < 48; ++segment) {
      const float angle = 2.0F * pi * static_cast<float>(segment) / 48.0F;
      glVertex2f(center_x + radius_x * std::cos(angle),
                 center_y + radius_y * std::sin(angle));
    }
    glEnd();
    glLineWidth(1.0F);
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
  }
  if (unit.overlay_ready && !asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    const SpritePreviewFrame &overlay =
        asset.overlay_frames[unit.current_overlay_frame];
    const float overlay_x = static_cast<float>(
        origin_x + static_cast<int>(overlay.x_offset) + asset.overlay_x_offset +
        unit.overlay_iscript_state.x_offset);
    const float overlay_y = static_cast<float>(
        origin_y + static_cast<int>(overlay.y_offset) + asset.overlay_y_offset +
        unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(status, overlay, unit.owner, overlay_x,
                                 overlay_y, overlay.width, overlay.height);
    } else {
      draw_preview_frame_gl(overlay, overlay_x, overlay_y, overlay.width,
                            overlay.height);
    }
  }
  draw_dynamic_overlay(false);
  const SpritePreviewFrame &frame = asset.sprite_frames[sprite_frame];
  const float frame_x =
      static_cast<float>(origin_x + static_cast<int>(frame.x_offset) +
                         unit.iscript_state.x_offset);
  const float frame_y =
      static_cast<float>(origin_y + static_cast<int>(frame.y_offset) +
                         unit.iscript_state.y_offset);
  if (asset.image_draw_function == 0) {
    draw_team_colored_frame_gl(status, frame, unit.owner, frame_x, frame_y,
                               frame.width, frame.height, sprite_mirrored);
  } else {
    draw_preview_frame_gl(frame, frame_x, frame_y, frame.width, frame.height,
                          nullptr, sprite_mirrored);
  }
  if (unit.overlay_ready && asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    const SpritePreviewFrame &overlay =
        asset.overlay_frames[unit.current_overlay_frame];
    const float overlay_x = static_cast<float>(
        origin_x + static_cast<int>(overlay.x_offset) + asset.overlay_x_offset +
        unit.overlay_iscript_state.x_offset);
    const float overlay_y = static_cast<float>(
        origin_y + static_cast<int>(overlay.y_offset) + asset.overlay_y_offset +
        unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(status, overlay, unit.owner, overlay_x,
                                 overlay_y, overlay.width, overlay.height);
    } else {
      draw_preview_frame_gl(overlay, overlay_x, overlay_y, overlay.width,
                            overlay.height);
    }
  }
  draw_dynamic_overlay(true);
  const std::size_t cargo_asset_index =
      unit.cargo_minerals != 0U ? status.mineral_cargo_asset_index
      : unit.cargo_gas != 0U    ? status.terran_gas_cargo_asset_index
                                : SIZE_MAX;
  if (cargo_asset_index < status.unit_assets.size() &&
      cargo_asset_index != unit.asset_index) {
    // CUnitInv.cpp::sub_430FB0 at 0x00430FB0 attaches image
    // resource-unit-type + 137 to the worker sprite. Mineral patches map to
    // image 357 and a Terran Refinery maps to image 359. It is inserted as
    // the sprite's head image, so draw it over the worker body.
    const UnitRenderAsset &cargo_asset = status.unit_assets[cargo_asset_index];
    ScenarioUnitPreview cargo{};
    cargo.x = unit.x;
    cargo.y = unit.y;
    cargo.owner = unit.owner;
    cargo.asset_index = cargo_asset_index;
    cargo.direction = unit.direction;
    cargo.iscript_state = cargo_asset.initial_iscript_state;
    cargo.overlay_iscript_state = cargo_asset.initial_overlay_iscript_state;
    cargo.current_sprite_frame = cargo_asset.initial_iscript_state.frame;
    cargo.current_overlay_frame =
        cargo_asset.initial_overlay_iscript_state.frame;
    cargo.iscript_ready = cargo_asset.iscript_ready;
    cargo.overlay_ready = cargo_asset.overlay_ready;
    draw_scenario_unit_gl(status, cargo);
  }
  if (unit.max_hit_points != 0 &&
      (unit.selected || unit.hit_points < unit.max_hit_points)) {
    const int bar_width =
        (std::max)(16, static_cast<int>(unit.selection_width));
    const int left = unit.x - status.camera_x - bar_width / 2;
    const int top = unit.y - status.camera_y - unit.selection_height / 2 - 7;
    const int filled =
        static_cast<int>(static_cast<std::uint64_t>(bar_width - 2) *
                         unit.hit_points / unit.max_hit_points);
    glDisable(GL_TEXTURE_2D);
    glColor4ub(0, 0, 0, 255);
    glBegin(GL_QUADS);
    glVertex2i(left, top);
    glVertex2i(left + bar_width, top);
    glVertex2i(left + bar_width, top + 5);
    glVertex2i(left, top + 5);
    glEnd();
    glColor4ub(unit.hit_points * 3U >= unit.max_hit_points * 2U ? 32 : 255,
               unit.hit_points * 3U >= unit.max_hit_points ? 224 : 48, 32, 255);
    glBegin(GL_QUADS);
    glVertex2i(left + 1, top + 1);
    glVertex2i(left + 1 + filled, top + 1);
    glVertex2i(left + 1 + filled, top + 4);
    glVertex2i(left + 1, top + 4);
    glEnd();
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
  }
}

} // namespace starcraft::recovery
