#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/gds/tileset.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

bool build_minimap_preview(
    const starcraft::gds::TilesetData &tileset,
    const starcraft::game::MultiplayerScenario &scenario,
    SpritePreviewFrame &output, std::uint16_t &content_x,
    std::uint16_t &content_y, std::uint16_t &content_width,
    std::uint16_t &content_height,
    const std::vector<std::uint8_t> *const creep_tiles,
    const std::vector<std::uint8_t> *const creep_visual_tiles) noexcept {
  output = {};
  if (!tileset.valid() || !scenario.valid() ||
      tileset.palette().size() != 1024) {
    return false;
  }
  try {
    const std::uint16_t largest =
        (std::max)(scenario.width(), scenario.height());
    const std::uint16_t numerator = largest <= 64U ? 2U : 1U;
    const std::uint16_t denominator = largest > 128U ? 2U : 1U;
    content_width = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(scenario.width()) * numerator / denominator);
    content_height = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(scenario.height()) * numerator /
        denominator);
    if (content_width == 0 || content_height == 0 || content_width > 128U ||
        content_height > 128U) {
      return false;
    }
    content_x = static_cast<std::uint16_t>((128U - content_width) / 2U);
    content_y = static_cast<std::uint16_t>((128U - content_height) / 2U);
    output.width = 128;
    output.height = 128;
    output.bgra.assign(128U * 128U, 0xFF020202U);
    output.palette_indices.assign(128U * 128U, 0U);
    output.opacity.assign(128U * 128U, 0xFFU);
    const auto &palette = tileset.palette();
    for (std::uint16_t minimap_y = 0; minimap_y < content_height; ++minimap_y) {
      const std::uint16_t tile_y = static_cast<std::uint16_t>(
          (std::min)(static_cast<std::uint32_t>(scenario.height() - 1U),
                     static_cast<std::uint32_t>(minimap_y) * denominator /
                         numerator));
      for (std::uint16_t minimap_x = 0; minimap_x < content_width;
           ++minimap_x) {
        const std::uint16_t tile_x = static_cast<std::uint16_t>(
            (std::min)(static_cast<std::uint32_t>(scenario.width() - 1U),
                       static_cast<std::uint32_t>(minimap_x) * denominator /
                           numerator));
        std::uint16_t map_tile{};
        starcraft::gds::IndexedMapTile rendered{};
        if (!scenario.tile(tile_x, tile_y, map_tile)) {
          return false;
        }
        const std::size_t map_index =
            static_cast<std::size_t>(tile_y) * scenario.width() + tile_x;
        const std::size_t map_tile_count =
            static_cast<std::size_t>(scenario.width()) * scenario.height();
        if (creep_tiles != nullptr && creep_visual_tiles != nullptr &&
            creep_tiles->size() == map_tile_count &&
            creep_visual_tiles->size() == map_tile_count &&
            (*creep_tiles)[map_index] != 0U &&
            (*creep_visual_tiles)[map_index] >= 16U) {
          map_tile = (*creep_visual_tiles)[map_index];
        }
        if (!tileset.render(map_tile, rendered)) {
          return false;
        }
        // minimap.cpp::sub_46FFE0 emits one representative palette index per
        // MTXM tile for 65..128-tile maps. The source chain terminates in the
        // tileset graphics; sample the center of the recovered 32x32 megatile.
        const std::uint8_t palette_index =
            rendered.pixels[16U * starcraft::gds::IndexedMapTile::width + 16U];
        const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
        const std::uint32_t red = palette[color];
        const std::uint32_t green = palette[color + 1U];
        const std::uint32_t blue = palette[color + 2U];
        const std::size_t output_index =
            static_cast<std::size_t>(content_y + minimap_y) * 128U +
            content_x + minimap_x;
        output.palette_indices[output_index] = palette_index;
        output.bgra[output_index] =
            0xFF000000U | blue | (green << 8U) | (red << 16U);
      }
    }
    return true;
  } catch (...) {
    output = {};
    return false;
  }
}

void draw_minimap_gl(const BootstrapStatus &status) {
  if (!status.minimap_ready || status.minimap.width != 128U ||
      status.minimap.height != 128U || status.scenario_width == 0 ||
      status.scenario_height == 0) {
    return;
  }
  constexpr float vertical_scale = 400.0F / 480.0F;
  const float left = static_cast<float>(status.minimap_control.left);
  const float top =
      static_cast<float>(status.minimap_control.top) * vertical_scale;
  const float width = static_cast<float>(status.minimap_control.right -
                                         status.minimap_control.left + 1);
  const float height = static_cast<float>(status.minimap_control.bottom -
                                          status.minimap_control.top + 1) *
                       vertical_scale;
  const SpritePreviewFrame &map_frame =
      status.fog_of_war_enabled && status.fog_render_surfaces_ready
          ? status.fogged_minimap
          : status.minimap;
  draw_preview_frame_gl(map_frame, left, top, width, height);

  const float content_left = left + status.minimap_content_x * width / 128.0F;
  const float content_top = top + status.minimap_content_y * height / 128.0F;
  const float content_width = status.minimap_content_width * width / 128.0F;
  const float content_height = status.minimap_content_height * height / 128.0F;
  const float map_width = static_cast<float>(status.scenario_width) * 32.0F;
  const float map_height = static_cast<float>(status.scenario_height) * 32.0F;
  glDisable(GL_TEXTURE_2D);
  for (const ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || unit.sprite_hidden ||
        !fog_unit_visible(status, unit, status.local_player)) {
      continue;
    }
    std::uint8_t red{224};
    std::uint8_t green{224};
    std::uint8_t blue{224};
    if (status.team_colors_ready &&
        unit.owner < status.team_color_indices.size() &&
        status.game_palette.size() == 1024) {
      const std::uint8_t palette_index =
          status.team_color_indices[unit.owner][3];
      const std::size_t color = static_cast<std::size_t>(palette_index) * 4U;
      red = status.game_palette[color];
      green = status.game_palette[color + 1U];
      blue = status.game_palette[color + 2U];
    }
    if (unit.selected) {
      red = green = blue = 255;
    }
    const float x = content_left + unit.x * content_width / map_width;
    const float y = content_top + unit.y * content_height / map_height;
    const float radius = unit.is_building ? 1.5F : 1.0F;
    glColor4ub(red, green, blue, 255);
    glBegin(GL_QUADS);
    glVertex2f(x - radius, y - radius);
    glVertex2f(x + radius, y - radius);
    glVertex2f(x + radius, y + radius);
    glVertex2f(x - radius, y + radius);
    glEnd();
  }

  const float camera_left =
      content_left + status.camera_x * content_width / map_width;
  const float camera_top =
      content_top + status.camera_y * content_height / map_height;
  const float camera_right =
      content_left +
      (status.camera_x + kMapViewportWidth) * content_width / map_width;
  const float camera_bottom =
      content_top +
      (status.camera_y + kMapViewportHeight) * content_height / map_height;
  glColor4ub(255, 255, 255, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2f(camera_left, camera_top);
  glVertex2f(camera_right, camera_top);
  glVertex2f(camera_right, camera_bottom);
  glVertex2f(camera_left, camera_bottom);
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

bool center_camera_from_minimap(BootstrapStatus &status, const int game_x,
                                const int game_y) noexcept {
  if (!status.minimap_ready || status.minimap_content_width == 0 ||
      status.minimap_content_height == 0) {
    return false;
  }
  constexpr int vertical_numerator = 5;
  constexpr int vertical_denominator = 6;
  const int control_width =
      status.minimap_control.right - status.minimap_control.left + 1;
  const int control_height =
      status.minimap_control.bottom - status.minimap_control.top + 1;
  const int left = status.minimap_control.left +
                   status.minimap_content_x * control_width / 128;
  const int right = status.minimap_control.left +
                    (status.minimap_content_x + status.minimap_content_width) *
                        control_width / 128 -
                    1;
  const int top =
      (status.minimap_control.top * vertical_numerator / vertical_denominator) +
      status.minimap_content_y * control_height * vertical_numerator /
          (128 * vertical_denominator);
  const int bottom =
      (status.minimap_control.top * vertical_numerator / vertical_denominator) +
      (status.minimap_content_y + status.minimap_content_height) *
          control_height * vertical_numerator / (128 * vertical_denominator) -
      1;
  if (game_x < left || game_x > right || game_y < top || game_y > bottom) {
    return false;
  }
  const int world_x = static_cast<int>(
      static_cast<std::int64_t>(game_x - left) * status.scenario_width * 32 /
      (std::max)(1, right - left + 1));
  const int world_y = static_cast<int>(static_cast<std::int64_t>(game_y - top) *
                                       status.scenario_height * 32 /
                                       (std::max)(1, bottom - top + 1));
  // minimap.cpp::sub_46F520 records the scaled control rectangle and the
  // shared camera setter keeps the resulting origin on an eight-pixel grid.
  (void)set_camera_position(status,
                            world_x - static_cast<int>(kMapViewportWidth) / 2,
                            world_y - static_cast<int>(kMapViewportHeight) / 2);
  return true;
}

} // namespace starcraft::recovery
