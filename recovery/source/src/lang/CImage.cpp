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
  if (draw_function == 0 || draw_function == 8 || draw_function == 12 ||
      draw_function == 13 || draw_function == 14) {
    // Renderer 14 is the worker-inventory image path. sub_411280 stores the
    // carried amount on the image; its GRP pixels and attached cargo overlay
    // remain ordinary palette imagery for the bootstrap's true-color pass.
    // Renderer 12 dispatches to the same sub_4BE449/sub_4BF3CA pair as the
    // normal renderer. Renderer 8's sub_4BEB5C uses the GRP pixels as the
    // high byte of a destination-palette lookup; retain those source pixels
    // and their exact opacity here so the OpenGL composition path has the
    // recovered warp/power mask instead of rejecting the image.
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
  asset.image_remapping = main_traits.remapping;
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
    asset.overlay_remapping = overlay_traits.remapping;
    asset.overlay_graphics_turns = overlay_traits.graphics_turns;
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
  const auto draw_image = [&](const SpritePreviewFrame &frame,
                              const std::uint8_t draw_function,
                              const std::uint8_t remapping, const float x,
                              const float y, const bool mirrored = false) {
    if (draw_function == 13U) {
      // CImage.cpp::sub_410260 forces renderer 13 on images 532+N and stores
      // the selection-set color in CImage+0x38. sub_409820 selects one of the
      // three eight-shade rows loaded from game\tselect.pcx. This bootstrap's
      // selected set is local, so it uses the exact row zero palette indices.
      if (frame.palette_indices.size() == frame.bgra.size() &&
          frame.opacity.size() == frame.bgra.size() &&
          status.selection_colors_ready && status.game_palette.size() == 1024U) {
        try {
          std::vector<std::uint32_t> translated(frame.bgra.size(), 0U);
          for (std::size_t pixel = 0; pixel < translated.size(); ++pixel) {
            const std::uint8_t source = frame.palette_indices[pixel];
            if (frame.opacity[pixel] != 0U && source >= 1U && source <= 8U) {
              const std::uint8_t palette_index =
                  status.selection_color_indices[0][source - 1U];
              const std::size_t color = 4U * palette_index;
              const std::uint32_t red = status.game_palette[color];
              const std::uint32_t green = status.game_palette[color + 1U];
              const std::uint32_t blue = status.game_palette[color + 2U];
              translated[pixel] =
                  0xFF000000U | blue | (green << 8U) | (red << 16U);
            }
          }
          draw_preview_frame_gl(frame, x, y, frame.width, frame.height,
                                translated.data(), mirrored);
          return;
        } catch (...) {
        }
      }
      draw_preview_frame_gl(frame, x, y, frame.width, frame.height, nullptr,
                            mirrored);
      return;
    }
    if (draw_function != 8U) {
      draw_preview_frame_gl(frame, x, y, frame.width, frame.height, nullptr,
                            mirrored);
      return;
    }
    if (remapping >= status.image_color_shifts.size() ||
        status.image_color_shifts[remapping].size() < 256U ||
        (status.image_color_shifts[remapping].size() & 0xFFU) != 0U ||
        status.game_palette.size() != 1024U ||
        frame.palette_indices.size() != frame.bgra.size() ||
        frame.opacity.size() != frame.bgra.size() ||
        status.terrain.bgra.size() !=
            static_cast<std::size_t>(kMapViewportWidth) * kMapViewportHeight ||
        status.terrain.palette_indices.size() != status.terrain.bgra.size()) {
      // Renderer 8 GRPs are lookup masks. Drawing their literal source
      // palette produces the large blue/lavender rectangles seen around the
      // Pylon field. If its licensed lookup is unavailable, keep the mask
      // transparent rather than displaying data that the original never
      // presented as art.
      return;
    }
    try {
      std::vector<std::uint32_t> shifted(frame.bgra.size(), 0U);
      const auto &lookup = status.image_color_shifts[remapping];
      for (std::size_t source_y = 0; source_y < frame.height; ++source_y) {
        for (std::size_t source_x = 0; source_x < frame.width; ++source_x) {
          const std::size_t pixel = source_y * frame.width + source_x;
          if (frame.opacity[pixel] == 0U) {
            continue;
          }
          const int destination_x = static_cast<int>(x) +
                                    static_cast<int>(source_x);
          const int destination_y = static_cast<int>(y) +
                                    static_cast<int>(source_y);
          std::uint8_t destination_index{};
          if (destination_x >= 0 && destination_y >= 0 &&
              destination_x < kMapViewportWidth &&
              destination_y < kMapViewportHeight) {
            destination_index = status.terrain.palette_indices[
                static_cast<std::size_t>(destination_y) * kMapViewportWidth +
                destination_x];
          }
          const std::size_t lookup_index =
              (static_cast<std::size_t>(frame.palette_indices[pixel]) << 8U) |
              destination_index;
          if (lookup_index >= lookup.size()) {
            // A draw-function-8 image is a lookup mask, never literal GRP
            // color art. Corrupt/out-of-range mask rows therefore remain
            // transparent instead of exposing the raw lavender mask.
            continue;
          }
          const std::uint8_t shifted_index = lookup[lookup_index];
          const std::size_t color = 4U * shifted_index;
          shifted[pixel] =
              0xFF000000U | status.game_palette[color + 2U] |
              (static_cast<std::uint32_t>(status.game_palette[color + 1U])
               << 8U) |
              (static_cast<std::uint32_t>(status.game_palette[color]) << 16U);
        }
      }
      draw_preview_frame_gl(frame, x, y, frame.width, frame.height,
                            shifted.data(), mirrored);
    } catch (...) {
      draw_preview_frame_gl(frame, x, y, frame.width, frame.height, nullptr,
                            mirrored);
    }
  };
  const auto draw_dynamic_overlay = [&](const bool above) {
    if (!unit.dynamic_overlay_ready || unit.dynamic_overlay_above != above ||
        unit.dynamic_overlay_iscript_state.hidden ||
        unit.dynamic_overlay_asset_index >= status.unit_assets.size()) {
      return;
    }
    const UnitRenderAsset &overlay_asset =
        status.unit_assets[unit.dynamic_overlay_asset_index];
    std::size_t overlay_frame = unit.current_dynamic_overlay_frame;
    bool overlay_mirrored = unit.dynamic_overlay_iscript_state.mirrored;
    if (overlay_asset.graphics_turns) {
      const starcraft::game::ImageFacingFrame facing =
          starcraft::game::image_facing_frame(unit.direction);
      overlay_mirrored = overlay_mirrored != facing.mirrored;
      if (overlay_frame + facing.frame < overlay_asset.sprite_frames.size()) {
        overlay_frame += facing.frame;
      }
    }
    if (overlay_frame >= overlay_asset.sprite_frames.size()) {
      return;
    }
    const SpritePreviewFrame &overlay =
        overlay_asset.sprite_frames[overlay_frame];
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
                                 overlay_y, overlay.width, overlay.height,
                                 overlay_mirrored);
    } else {
      draw_image(overlay, overlay_asset.image_draw_function,
                 overlay_asset.image_remapping, overlay_x, overlay_y,
                 overlay_mirrored);
    }
  };
  if (!unit.construction_visible) {
    draw_dynamic_overlay(false);
    draw_dynamic_overlay(true);
    return;
  }
  if (unit.selected &&
      unit.selection_circle_asset_index < status.unit_assets.size()) {
    // CSprite::sub_41C550 attaches image 532+sprites.dat[3] below the
    // primary image and CImage::sub_410260 applies sprites.dat[4] to Y.
    const UnitRenderAsset &circle_asset =
        status.unit_assets[unit.selection_circle_asset_index];
    const std::size_t circle_frame =
        circle_asset.initial_iscript_state.frame;
    if (circle_frame < circle_asset.sprite_frames.size()) {
      const SpritePreviewFrame &circle =
          circle_asset.sprite_frames[circle_frame];
      const float circle_x = static_cast<float>(
          unit.x - status.camera_x - circle_asset.sprite_canvas_width / 2 +
          static_cast<int>(circle.x_offset));
      const float circle_y = static_cast<float>(
          unit.y - status.camera_y - circle_asset.sprite_canvas_height / 2 +
          static_cast<int>(circle.y_offset) + unit.selection_circle_y_offset);
      draw_image(circle, 13U, 0U, circle_x, circle_y);
    }
  }
  if (unit.overlay_ready && !asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    std::size_t overlay_frame = unit.current_overlay_frame;
    bool overlay_mirrored = unit.overlay_iscript_state.mirrored;
    if (asset.overlay_graphics_turns) {
      const starcraft::game::ImageFacingFrame facing =
          starcraft::game::image_facing_frame(unit.direction);
      overlay_mirrored = overlay_mirrored != facing.mirrored;
      if (overlay_frame + facing.frame < asset.overlay_frames.size()) {
        overlay_frame += facing.frame;
      }
    }
    const SpritePreviewFrame &overlay = asset.overlay_frames[overlay_frame];
    const float overlay_x = static_cast<float>(
        origin_x + static_cast<int>(overlay.x_offset) + asset.overlay_x_offset +
        unit.overlay_iscript_state.x_offset);
    const float overlay_y = static_cast<float>(
        origin_y + static_cast<int>(overlay.y_offset) + asset.overlay_y_offset +
        unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(status, overlay, unit.owner, overlay_x,
                                 overlay_y, overlay.width, overlay.height,
                                 overlay_mirrored);
    } else {
      draw_image(overlay, asset.overlay_draw_function,
                 asset.overlay_remapping, overlay_x, overlay_y,
                 overlay_mirrored);
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
    draw_image(frame, asset.image_draw_function, asset.image_remapping,
               frame_x, frame_y, sprite_mirrored);
  }
  if (unit.overlay_ready && asset.overlay_above &&
      !unit.overlay_iscript_state.hidden && !asset.overlay_frames.empty() &&
      unit.current_overlay_frame < asset.overlay_frames.size()) {
    std::size_t overlay_frame = unit.current_overlay_frame;
    bool overlay_mirrored = unit.overlay_iscript_state.mirrored;
    if (asset.overlay_graphics_turns) {
      const starcraft::game::ImageFacingFrame facing =
          starcraft::game::image_facing_frame(unit.direction);
      overlay_mirrored = overlay_mirrored != facing.mirrored;
      if (overlay_frame + facing.frame < asset.overlay_frames.size()) {
        overlay_frame += facing.frame;
      }
    }
    const SpritePreviewFrame &overlay = asset.overlay_frames[overlay_frame];
    const float overlay_x = static_cast<float>(
        origin_x + static_cast<int>(overlay.x_offset) + asset.overlay_x_offset +
        unit.overlay_iscript_state.x_offset);
    const float overlay_y = static_cast<float>(
        origin_y + static_cast<int>(overlay.y_offset) + asset.overlay_y_offset +
        unit.overlay_iscript_state.y_offset);
    if (asset.overlay_draw_function == 0) {
      draw_team_colored_frame_gl(status, overlay, unit.owner, overlay_x,
                                 overlay_y, overlay.width, overlay.height,
                                 overlay_mirrored);
    } else {
      draw_image(overlay, asset.overlay_draw_function,
                 asset.overlay_remapping, overlay_x, overlay_y,
                 overlay_mirrored);
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
