#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/game/image.hpp"
#include "starcraft/gds/grp.hpp"
#include "starcraft/lang/iscript.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::recovery {

namespace {

[[nodiscard]] std::uint32_t read_location_u32(
    const std::uint8_t *const bytes) noexcept {
  return static_cast<std::uint32_t>(bytes[0]) |
         (static_cast<std::uint32_t>(bytes[1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

[[nodiscard]] bool decode_special_overlay_locations(
    const std::vector<std::uint8_t> &bytes, UnitRenderAsset &asset) {
  if (bytes.size() < 8U) {
    return false;
  }
  const std::uint32_t frame_count = read_location_u32(bytes.data());
  const std::uint32_t point_count = read_location_u32(bytes.data() + 4U);
  if (frame_count == 0U || point_count == 0U ||
      frame_count > (bytes.size() - 8U) / 4U ||
      point_count > SIZE_MAX / 2U / frame_count) {
    return false;
  }
  try {
    std::vector<std::int8_t> points(
        static_cast<std::size_t>(frame_count) * point_count * 2U);
    for (std::uint32_t frame = 0; frame < frame_count; ++frame) {
      const std::uint32_t offset =
          read_location_u32(bytes.data() + 8U + 4U * frame);
      const std::size_t payload = static_cast<std::size_t>(point_count) * 2U;
      if (offset > bytes.size() || payload > bytes.size() - offset) {
        return false;
      }
      const std::size_t destination =
          static_cast<std::size_t>(frame) * payload;
      for (std::size_t index = 0; index < payload; ++index) {
        points[destination + index] =
            static_cast<std::int8_t>(bytes[offset + index]);
      }
    }
    asset.special_overlay_frame_count = frame_count;
    asset.special_overlay_point_count = point_count;
    asset.special_overlay_points = std::move(points);
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace

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

  asset.special_overlay_path = data.image_special_overlay_path(image_id);
  if (!asset.special_overlay_path.empty()) {
    std::vector<std::uint8_t> locations;
    if (!storm.load_file(asset.special_overlay_path.c_str(), locations) ||
        !decode_special_overlay_locations(locations, asset)) {
      return false;
    }
  }

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

bool spawn_resource_overlay_effect(BootstrapStatus &status,
                                   const ScenarioUnitPreview &source,
                                   const std::uint8_t point) noexcept {
  if (source.asset_index >= status.unit_assets.size()) {
    return false;
  }
  const UnitRenderAsset &source_asset =
      status.unit_assets[source.asset_index];
  if (source.current_sprite_frame >=
          source_asset.special_overlay_frame_count ||
      point >= source_asset.special_overlay_point_count) {
    return false;
  }
  const std::size_t location =
      (static_cast<std::size_t>(source.current_sprite_frame) *
           source_asset.special_overlay_point_count +
       point) *
      2U;
  if (location + 1U >= source_asset.special_overlay_points.size()) {
    return false;
  }
  int x_offset = source_asset.special_overlay_points[location];
  const int y_offset = source_asset.special_overlay_points[location + 1U];
  if (source.iscript_state.mirrored) {
    x_offset = -x_offset;
  }

  // CImage.cpp::sub_415210 case 0x40 creates image 402+point while the
  // source has resources and 407+point after depletion, at the signed point
  // from dword_55A918[image][display-frame].
  const std::uint16_t image_id = static_cast<std::uint16_t>(
      (source.resource_amount != 0U ? 402U : 407U) + point);
  const auto effect_asset =
      std::find_if(status.unit_assets.begin(), status.unit_assets.end(),
                   [image_id](const UnitRenderAsset &candidate) {
                     return candidate.image_id == image_id;
                   });
  if (effect_asset == status.unit_assets.end()) {
    return false;
  }
  const std::size_t asset_index = static_cast<std::size_t>(
      effect_asset - status.unit_assets.begin());
  ScenarioUnitPreview effect{};
  effect.unit_id = status.next_unit_id++;
  effect.x = static_cast<std::uint16_t>((std::clamp)(
      static_cast<int>(source.x) + x_offset + source.iscript_state.x_offset,
      0, static_cast<int>(UINT16_MAX)));
  effect.y = static_cast<std::uint16_t>((std::clamp)(
      static_cast<int>(source.y) + y_offset + source.iscript_state.y_offset,
      0, static_cast<int>(UINT16_MAX)));
  effect.owner = source.owner;
  effect.sprite_elevation = static_cast<std::uint8_t>(
      (std::min)(255U, static_cast<unsigned>(source.sprite_elevation) + 1U));
  effect.asset_index = asset_index;
  effect.iscript_state = effect_asset->initial_iscript_state;
  effect.overlay_iscript_state = effect_asset->initial_overlay_iscript_state;
  effect.current_sprite_frame = effect_asset->initial_iscript_state.frame;
  effect.current_overlay_frame =
      effect_asset->initial_overlay_iscript_state.frame;
  effect.iscript_ready = effect_asset->iscript_ready;
  effect.overlay_ready = effect_asset->overlay_ready;
  try {
    status.transient_images.push_back(std::move(effect));
    return true;
  } catch (...) {
    return false;
  }
}

bool spawn_iscript_sprite_effect(
    BootstrapStatus &status, const ScenarioUnitPreview &source,
    const starcraft::lang::IScriptState &event) noexcept {
  if (event.sprite_event_count == 0U ||
      event.sprite_id >= status.sprite_image_ids.size()) {
    return false;
  }
  const std::uint16_t image_id = status.sprite_image_ids[event.sprite_id];
  const auto asset =
      std::find_if(status.unit_assets.begin(), status.unit_assets.end(),
                   [image_id](const UnitRenderAsset &candidate) {
                     return candidate.image_id == image_id;
                   });
  if (asset == status.unit_assets.end()) {
    return false;
  }
  const int effect_x = static_cast<int>(source.x) +
                       source.iscript_state.x_offset;
  const int effect_y = static_cast<int>(source.y) +
                       source.iscript_state.y_offset +
                       event.sprite_y_offset;
  ScenarioUnitPreview effect{};
  effect.unit_id = status.next_unit_id++;
  effect.x = static_cast<std::uint16_t>(
      (std::clamp)(effect_x, 0, static_cast<int>(UINT16_MAX)));
  effect.y = static_cast<std::uint16_t>(
      (std::clamp)(effect_y, 0, static_cast<int>(UINT16_MAX)));
  effect.x_fixed = static_cast<std::int32_t>(effect.x) << 8U;
  effect.y_fixed = static_cast<std::int32_t>(effect.y) << 8U;
  effect.owner = source.owner;
  effect.sprite_elevation = event.sprite_elevation;
  effect.asset_index =
      static_cast<std::size_t>(asset - status.unit_assets.begin());
  effect.iscript_state = asset->initial_iscript_state;
  effect.overlay_iscript_state = asset->initial_overlay_iscript_state;
  effect.current_sprite_frame = asset->initial_iscript_state.frame;
  effect.current_overlay_frame = asset->initial_overlay_iscript_state.frame;
  effect.iscript_ready = asset->iscript_ready;
  effect.overlay_ready = asset->overlay_ready;
  if (effect.iscript_state.sound_event_count != 0U) {
    (void)queue_positional_game_sound(status, effect.iscript_state.sound_event,
                                      effect.x, effect.y);
  }
  try {
    status.transient_images.push_back(std::move(effect));
    return true;
  } catch (...) {
    return false;
  }
}

void draw_scenario_unit_gl(const BootstrapStatus &status,
                           const ScenarioUnitPreview &unit) {
  if (!unit.alive || unit.sprite_hidden ||
      unit.asset_index >= status.unit_assets.size()) {
    return;
  }
  const UnitRenderAsset &asset = status.unit_assets[unit.asset_index];
  if ((!unit.iscript_ready || unit.iscript_state.hidden) &&
      !unit.dynamic_overlay_ready) {
    return;
  }
  if (asset.sprite_frames.empty()) {
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
  const SpritePreviewFrame &primary_frame = asset.sprite_frames[sprite_frame];
  const int primary_x = origin_x + static_cast<int>(primary_frame.x_offset) +
                        unit.iscript_state.x_offset;
  const int primary_y = origin_y + static_cast<int>(primary_frame.y_offset) +
                        unit.iscript_state.y_offset;
  const auto draw_image = [&](const SpritePreviewFrame &frame,
                              const std::uint8_t draw_function,
                              const std::uint8_t remapping, const float x,
                              const float y, const bool mirrored = false,
                              const bool composite_primary = false) {
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
          // sub_4BEB5C reads the byte already present in the destination
          // framebuffer. For an above-building warp image that byte is the
          // building's primary palette pixel, not merely terrain. Compose the
          // two licensed CPU palette layers directly; this preserves the
          // lookup exactly without a synchronous glReadPixels stall.
          if (composite_primary && destination_x >= primary_x &&
              destination_y >= primary_y &&
              destination_x < primary_x + primary_frame.width &&
              destination_y < primary_y + primary_frame.height &&
              primary_frame.palette_indices.size() ==
                  primary_frame.bgra.size() &&
              primary_frame.opacity.size() == primary_frame.bgra.size()) {
            const std::size_t local_y =
                static_cast<std::size_t>(destination_y - primary_y);
            std::size_t local_x =
                static_cast<std::size_t>(destination_x - primary_x);
            if (sprite_mirrored) {
              local_x = primary_frame.width - 1U - local_x;
            }
            const std::size_t primary_pixel =
                local_y * primary_frame.width + local_x;
            if (primary_frame.opacity[primary_pixel] != 0U) {
              destination_index =
                  primary_frame.palette_indices[primary_pixel];
            }
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
                 overlay_mirrored, above);
    }
  };
  if (!unit.construction_visible) {
    glColor4ub(255, 255, 255, unit.cloaked ? 96 : 255);
    draw_dynamic_overlay(false);
    draw_dynamic_overlay(true);
    glColor4ub(255, 255, 255, 255);
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
  // The cloak order installs the original translucent draw state while the
  // selection circle remains fully visible to the owning player.
  glColor4ub(255, 255, 255, unit.cloaked ? 96 : 255);
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
  const SpritePreviewFrame &frame = primary_frame;
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
                 overlay_mirrored, true);
    }
  }
  draw_dynamic_overlay(true);
  glColor4ub(255, 255, 255, 255);
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
