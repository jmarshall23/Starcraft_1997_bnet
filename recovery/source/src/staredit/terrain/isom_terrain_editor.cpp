#include "terrain/isom_terrain_editor.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <utility>

namespace staredit::terrain {
namespace {

constexpr std::uint16_t kClearEditorFlags = 0x7FFEU;
constexpr std::uint16_t kModified = 0x0001U;
constexpr std::uint16_t kVisited = 0x8000U;
constexpr std::uint16_t kSameTypeLink = 255U;
constexpr std::size_t kMaximumTerrainGroups = 1024U;

struct Point {
  int x{};
  int y{};
};

constexpr std::array<Point, 4> kNeighborOffsets{{
    {-1, -1}, {1, -1}, {1, 1}, {-1, 1},
}};

constexpr std::array<IsomQuadrant, 4> kOppositeQuadrants{{
    IsomQuadrant::bottom_right,
    IsomQuadrant::bottom_left,
    IsomQuadrant::top_left,
    IsomQuadrant::top_right,
}};

constexpr Point add(const Point point, const Point offset) noexcept {
  return {point.x + offset.x, point.y + offset.y};
}

Point quadrant_rect(const Point diamond,
                    const IsomQuadrant quadrant) noexcept {
  switch (quadrant) {
    case IsomQuadrant::top_left:
      return {diamond.x - 1, diamond.y - 1};
    case IsomQuadrant::top_right:
      return {diamond.x, diamond.y - 1};
    case IsomQuadrant::bottom_right:
      return diamond;
    case IsomQuadrant::bottom_left:
      return {diamond.x - 1, diamond.y};
  }
  return diamond;
}

class EditContext final {
 public:
  EditContext(formats::IsomSection& isom,
              std::vector<std::uint16_t>& editor_tiles,
              std::vector<std::uint16_t>& game_tiles,
              const std::uint16_t map_width,
              const std::uint16_t map_height,
              const IsomCatalog& catalog,
              const IsomTopology& topology,
              const starcraft::gds::TilesetData& tileset) noexcept
      : isom_(isom),
        editor_tiles_(editor_tiles),
        game_tiles_(game_tiles),
        map_width_(map_width),
        map_height_(map_height),
        catalog_(catalog),
        topology_(topology),
        tileset_(tileset) {}

  [[nodiscard]] bool paint(const std::uint16_t tile_x,
                           const std::uint16_t tile_y,
                           const std::uint16_t terrain_type,
                           const std::uint16_t extent) {
    const std::uint16_t isom_value =
        catalog_.terrain_type_isom_value(terrain_type);
    if (isom_value == 0U || isom_value >= catalog_.links().size() ||
        catalog_.links()[isom_value].terrain_type == 0U || extent == 0U) {
      return false;
    }
    Point center{static_cast<int>(tile_x / 2U), static_cast<int>(tile_y)};
    if (((center.x + center.y) & 1) != 0) {
      if (center.x + 1 < static_cast<int>(isom_.column_count())) {
        ++center.x;
      } else {
        --center.x;
      }
    }
    if (!diamond_valid(center) || !in_bounds(center)) {
      return false;
    }

    int brush_min = -static_cast<int>(extent) / 2;
    int brush_max = brush_min + static_cast<int>(extent);
    if ((extent & 1U) == 0U) {
      ++brush_min;
      ++brush_max;
    }
    std::deque<Point> pending{};
    for (int offset_x = brush_min; offset_x < brush_max; ++offset_x) {
      for (int offset_y = brush_min; offset_y < brush_max; ++offset_y) {
        const Point diamond{center.x + offset_x - offset_y,
                            center.y + offset_x + offset_y};
        if (!diamond_valid(diamond) || !in_bounds(diamond)) {
          continue;
        }
        set_diamond(diamond, isom_value);
        if (offset_x == brush_min || offset_x == brush_max - 1 ||
            offset_y == brush_min || offset_y == brush_max - 1) {
          for (const Point offset : kNeighborOffsets) {
            const Point neighbor = add(diamond, offset);
            if (diamond_needs_update(neighbor)) {
              pending.push_back(neighbor);
            }
          }
        }
      }
    }
    update_radially(pending);
    return regenerate_tiles();
  }

 private:
  struct NeighborInfo {
    std::uint16_t link_id{};
    std::uint16_t isom_value{};
    bool modified{};
  };

  struct BestMatch {
    std::uint16_t isom_value{};
    std::uint16_t count{};
  };

  [[nodiscard]] bool in_bounds(const Point point) const noexcept {
    return point.x >= 0 && point.y >= 0 &&
           static_cast<std::size_t>(point.x) < isom_.column_count() &&
           static_cast<std::size_t>(point.y) < isom_.row_count();
  }

  [[nodiscard]] static bool diamond_valid(const Point point) noexcept {
    return ((point.x + point.y) & 1) == 0;
  }

  [[nodiscard]] const formats::IsomEntry* entry(
      const Point point) const noexcept {
    return in_bounds(point)
               ? isom_.entry(static_cast<std::size_t>(point.x),
                             static_cast<std::size_t>(point.y))
               : nullptr;
  }

  [[nodiscard]] formats::IsomEntry* mutable_entry(
      const Point point) noexcept {
    return in_bounds(point)
               ? isom_.mutable_entry(static_cast<std::size_t>(point.x),
                                     static_cast<std::size_t>(point.y))
               : nullptr;
  }

  [[nodiscard]] std::uint16_t central_value(
      const Point point) const noexcept {
    const formats::IsomEntry* const value = entry(point);
    return value == nullptr
               ? 0U
               : static_cast<std::uint16_t>(
                     (value->links[0].raw & kClearEditorFlags) >> 4U);
  }

  [[nodiscard]] bool central_modified(const Point point) const noexcept {
    const formats::IsomEntry* const value = entry(point);
    return value != nullptr && (value->links[0].raw & kModified) != 0U;
  }

  [[nodiscard]] bool visited(const Point point) const noexcept {
    const formats::IsomEntry* const value = entry(point);
    return value != nullptr && (value->links[2].raw & kVisited) != 0U;
  }

  void mark_visited(const Point point) noexcept {
    formats::IsomEntry* const value = mutable_entry(point);
    if (value != nullptr) {
      value->links[2].raw |= kVisited;
    }
  }

  [[nodiscard]] bool diamond_needs_update(const Point point) const noexcept {
    return diamond_valid(point) && in_bounds(point) &&
           !central_modified(point) && central_value(point) != 0U;
  }

  void set_quadrant(const Point rect,
                    const IsomQuadrant quadrant,
                    const std::uint16_t isom_value) noexcept {
    formats::IsomEntry* const value = mutable_entry(rect);
    if (value == nullptr) {
      return;
    }
    const std::uint16_t base = static_cast<std::uint16_t>(isom_value << 4U);
    std::size_t first_side{};
    std::size_t second_side{};
    std::uint16_t first_flags{};
    std::uint16_t second_flags{};
    switch (quadrant) {
      case IsomQuadrant::top_left:
        first_side = 2U; second_side = 3U;
        first_flags = 0x0U; second_flags = 0x2U;
        break;
      case IsomQuadrant::top_right:
        first_side = 0U; second_side = 3U;
        first_flags = 0x4U; second_flags = 0x6U;
        break;
      case IsomQuadrant::bottom_right:
        first_side = 0U; second_side = 1U;
        first_flags = 0x8U; second_flags = 0xAU;
        break;
      case IsomQuadrant::bottom_left:
        first_side = 1U; second_side = 2U;
        first_flags = 0xCU; second_flags = 0xEU;
        break;
    }
    value->links[first_side].raw =
        static_cast<std::uint16_t>(base | first_flags | kModified);
    value->links[second_side].raw =
        static_cast<std::uint16_t>(base | second_flags | kModified);
  }

  void set_diamond(const Point diamond,
                   const std::uint16_t isom_value) noexcept {
    for (std::size_t quadrant = 0U; quadrant < 4U; ++quadrant) {
      const auto value = static_cast<IsomQuadrant>(quadrant);
      set_quadrant(quadrant_rect(diamond, value), value, isom_value);
    }
  }

  void load_neighbors(const Point diamond,
                      std::array<NeighborInfo, 4>& neighbors,
                      std::uint16_t& max_modified_type) const noexcept {
    max_modified_type = 0U;
    for (std::size_t index = 0U; index < neighbors.size(); ++index) {
      const Point neighbor = add(diamond, kNeighborOffsets[index]);
      if (!in_bounds(neighbor)) {
        continue;
      }
      NeighborInfo& output = neighbors[index];
      output.modified = central_modified(neighbor);
      output.isom_value = central_value(neighbor);
      if (output.isom_value >= catalog_.links().size()) {
        output.isom_value = 0U;
        continue;
      }
      const IsomShapeLinks& shape = catalog_.links()[output.isom_value];
      output.link_id = shape.link_id(kOppositeQuadrants[index]);
      if (output.modified && shape.terrain_type > max_modified_type) {
        max_modified_type = shape.terrain_type;
      }
    }
  }

  [[nodiscard]] std::uint16_t count_matches(
      const IsomShapeLinks& candidate,
      const std::array<NeighborInfo, 4>& neighbors) const noexcept {
    std::uint16_t matches{};
    for (std::size_t quadrant = 0U; quadrant < neighbors.size();
         ++quadrant) {
      const NeighborInfo& neighbor = neighbors[quadrant];
      if (neighbor.isom_value >= catalog_.links().size()) {
        return 0U;
      }
      const IsomShapeLinks& neighbor_shape =
          catalog_.links()[neighbor.isom_value];
      const std::uint16_t candidate_link =
          candidate.link_id(static_cast<IsomQuadrant>(quadrant));
      if (neighbor.link_id == candidate_link &&
          (candidate_link < kSameTypeLink ||
           candidate.terrain_type == neighbor_shape.terrain_type)) {
        ++matches;
      } else if (neighbor.modified) {
        return 0U;
      }
    }
    return matches;
  }

  void search(const std::uint16_t starting_type,
              const std::array<NeighborInfo, 4>& neighbors,
              BestMatch& best) const noexcept {
    const std::size_t type_count = catalog_.terrain_type_count();
    const bool until_higher = starting_type == type_count / 2U + 1U;
    const bool until_end = starting_type == 0U;
    std::size_t isom_value =
        catalog_.terrain_type_isom_value(starting_type);
    while (isom_value < catalog_.links().size()) {
      const IsomShapeLinks& candidate = catalog_.links()[isom_value];
      if (!until_end && candidate.terrain_type != starting_type &&
          (!until_higher || candidate.terrain_type > starting_type)) {
        break;
      }
      const std::uint16_t matches = count_matches(candidate, neighbors);
      if (matches > best.count) {
        best = {static_cast<std::uint16_t>(isom_value), matches};
      }
      ++isom_value;
    }
  }

  [[nodiscard]] std::optional<std::uint16_t> best_match(
      const Point diamond) const noexcept {
    std::array<NeighborInfo, 4> neighbors{};
    std::uint16_t max_modified_type{};
    load_neighbors(diamond, neighbors, max_modified_type);
    BestMatch best{};
    const std::uint16_t previous = central_value(diamond);
    if (previous < catalog_.links().size()) {
      const std::uint16_t previous_type =
          catalog_.links()[previous].terrain_type;
      const std::size_t type_count = catalog_.terrain_type_count();
      const std::size_t mapped_index =
          static_cast<std::size_t>(max_modified_type) * type_count +
          previous_type;
      if (mapped_index < catalog_.terrain_type_map().size()) {
        search(catalog_.terrain_type_map()[mapped_index], neighbors, best);
      }
    }
    search(max_modified_type, neighbors, best);
    search(static_cast<std::uint16_t>(
               catalog_.terrain_type_count() / 2U + 1U),
           neighbors, best);
    return best.isom_value == previous
               ? std::optional<std::uint16_t>{}
               : std::optional<std::uint16_t>{best.isom_value};
  }

  void update_radially(std::deque<Point>& pending) {
    while (!pending.empty()) {
      const Point diamond = pending.front();
      pending.pop_front();
      if (!diamond_needs_update(diamond) || visited(diamond)) {
        continue;
      }
      mark_visited(diamond);
      const std::optional<std::uint16_t> match = best_match(diamond);
      if (!match.has_value()) {
        continue;
      }
      if (*match != 0U) {
        set_diamond(diamond, *match);
      }
      for (const Point offset : kNeighborOffsets) {
        const Point neighbor = add(diamond, offset);
        if (diamond_needs_update(neighbor)) {
          pending.push_back(neighbor);
        }
      }
    }
  }

  [[nodiscard]] std::uint16_t tile_group(
      const std::uint16_t tile) const noexcept {
    return static_cast<std::uint16_t>((tile >> 4U) & 0x07FFU);
  }

  void set_tile(const std::size_t index,
                const std::uint16_t value) noexcept {
    const std::uint16_t old_editor = editor_tiles_[index];
    const std::uint16_t old_game = game_tiles_[index];
    editor_tiles_[index] = value;
    // Doodad megatiles occupy CV5 groups 1024+, so preserve those overlays
    // while regenerating the editor TILE layer beneath them.
    if (old_game == old_editor || tile_group(old_game) < kMaximumTerrainGroups) {
      game_tiles_[index] = value;
    }
  }

  [[nodiscard]] std::uint16_t choose_member(
      const std::uint16_t group,
      const std::size_t x,
      const std::size_t y) const noexcept {
    starcraft::gds::TerrainGroupVariants variants{};
    if (!tileset_.terrain_group_variants(group, variants)) {
      return 0U;
    }
    const std::uint32_t hash =
        static_cast<std::uint32_t>(x) * 73856093U ^
        static_cast<std::uint32_t>(y) * 19349663U ^
        static_cast<std::uint32_t>(group) * 83492791U;
    std::uint16_t tile{};
    if (variants.rare_count != 0U && hash % 20U == 0U) {
      tile = variants.rare[(hash / 20U) % variants.rare_count];
    } else if (variants.common_count != 0U) {
      tile = variants.common[hash % variants.common_count];
    } else if (variants.rare_count != 0U) {
      tile = variants.rare[hash % variants.rare_count];
    }
    return static_cast<std::uint16_t>(tile & 0x000FU);
  }

  [[nodiscard]] bool rect_hash(const std::size_t x,
                               const std::size_t y,
                               std::uint32_t& hash) const noexcept {
    const formats::IsomEntry* const rect = isom_.entry(x, y);
    if (rect == nullptr) {
      return false;
    }
    std::array<std::uint16_t, 4> sides{};
    for (std::size_t side = 0U; side < sides.size(); ++side) {
      sides[side] = rect->links[side].raw;
    }
    return catalog_.isom_rect_hash(sides, hash);
  }

  [[nodiscard]] bool update_tile_pair(const std::size_t isom_x,
                                      const std::size_t isom_y) noexcept {
    if (isom_x + 1U >= isom_.column_count() ||
        isom_y + 1U >= isom_.row_count()) {
      return true;
    }
    const std::size_t left_x = 2U * isom_x;
    const std::size_t right_x = left_x + 1U;
    if (right_x >= map_width_ || isom_y >= map_height_) {
      return true;
    }
    std::uint32_t hash{};
    if (!rect_hash(isom_x, isom_y, hash)) {
      return false;
    }
    const std::vector<std::uint16_t>* candidates = topology_.candidates(hash);
    const std::size_t left_index = isom_y * map_width_ + left_x;
    const std::size_t right_index = left_index + 1U;
    if (candidates == nullptr || candidates->empty()) {
      set_tile(left_index, 0U);
      set_tile(right_index, 0U);
      return true;
    }

    std::uint16_t destination = candidates->front();
    if (isom_y > 0U) {
      const std::uint16_t above = tile_group(
          editor_tiles_[(isom_y - 1U) * map_width_ + left_x]);
      starcraft::gds::Cv5TileGroup above_group{};
      if (tileset_.tile_group(above, above_group)) {
        const std::uint16_t bottom_connection =
            above_group.stack_connections[3];
        for (const std::uint16_t candidate : *candidates) {
          starcraft::gds::Cv5TileGroup candidate_group{};
          if (tileset_.tile_group(candidate, candidate_group) &&
              candidate_group.stack_connections[1] == bottom_connection) {
            destination = candidate;
            break;
          }
        }
      }
    }
    const std::uint16_t member = choose_member(destination, isom_x, isom_y);
    set_tile(left_index,
             static_cast<std::uint16_t>((destination << 4U) | member));
    set_tile(right_index,
             static_cast<std::uint16_t>(((destination + 1U) << 4U) | member));

    std::size_t stack_top_y = isom_y;
    std::uint16_t current = tile_group(editor_tiles_[left_index]);
    while (stack_top_y > 0U) {
      starcraft::gds::Cv5TileGroup current_group{};
      if (!tileset_.tile_group(current, current_group) ||
          current_group.stack_connections[1] == 0U) {
        break;
      }
      const std::uint16_t above = tile_group(
          editor_tiles_[(stack_top_y - 1U) * map_width_ + left_x]);
      starcraft::gds::Cv5TileGroup above_group{};
      if (!tileset_.tile_group(above, above_group) ||
          current_group.stack_connections[1] !=
              above_group.stack_connections[3]) {
        break;
      }
      --stack_top_y;
      current = above;
    }
    std::size_t stack_left = stack_top_y * map_width_ + left_x;
    std::size_t stack_right = stack_left + 1U;
    set_tile(stack_left, static_cast<std::uint16_t>(
                             (tile_group(editor_tiles_[stack_left]) << 4U) |
                             member));
    set_tile(stack_right, static_cast<std::uint16_t>(
                              (tile_group(editor_tiles_[stack_right]) << 4U) |
                              member));

    for (std::size_t y = stack_top_y + 1U; y < map_height_; ++y) {
      const std::size_t previous_index = (y - 1U) * map_width_ + left_x;
      const std::size_t row_left = y * map_width_ + left_x;
      const std::size_t row_right = row_left + 1U;
      const std::uint16_t previous_group_id =
          tile_group(editor_tiles_[previous_index]);
      const std::uint16_t next_group_id = tile_group(editor_tiles_[row_left]);
      starcraft::gds::Cv5TileGroup previous_group{};
      starcraft::gds::Cv5TileGroup next_group{};
      if (!tileset_.tile_group(previous_group_id, previous_group) ||
          !tileset_.tile_group(next_group_id, next_group) ||
          previous_group.stack_connections[3] == 0U ||
          next_group.stack_connections[1] == 0U) {
        break;
      }
      std::uint16_t left_group = next_group_id;
      std::uint16_t right_group = tile_group(editor_tiles_[row_right]);
      const std::uint16_t connection = previous_group.stack_connections[3];
      if (connection != next_group.stack_connections[1]) {
        std::uint32_t row_hash{};
        if (!rect_hash(isom_x, y, row_hash)) {
          return false;
        }
        const auto* row_candidates = topology_.candidates(row_hash);
        if (row_candidates != nullptr) {
          for (const std::uint16_t candidate : *row_candidates) {
            starcraft::gds::Cv5TileGroup candidate_group{};
            if (tileset_.tile_group(candidate, candidate_group) &&
                candidate_group.stack_connections[1] == connection) {
              left_group = candidate;
              right_group = static_cast<std::uint16_t>(candidate + 1U);
              break;
            }
          }
        }
      }
      set_tile(row_left,
               static_cast<std::uint16_t>((left_group << 4U) | member));
      set_tile(row_right,
               static_cast<std::uint16_t>((right_group << 4U) | member));
    }
    return true;
  }

  [[nodiscard]] bool regenerate_tiles() noexcept {
    bool success = true;
    for (std::size_t y = 0U; y < isom_.row_count(); ++y) {
      for (std::size_t x = 0U; x < isom_.column_count(); ++x) {
        formats::IsomEntry* const rect = isom_.mutable_entry(x, y);
        if (rect == nullptr) {
          success = false;
          continue;
        }
        const bool terrain_modified =
            ((rect->links[0].raw | rect->links[2].raw) & kModified) != 0U;
        if (terrain_modified && !update_tile_pair(x, y)) {
          success = false;
        }
      }
    }
    for (std::size_t y = 0U; y < isom_.row_count(); ++y) {
      for (std::size_t x = 0U; x < isom_.column_count(); ++x) {
        formats::IsomEntry* const rect = isom_.mutable_entry(x, y);
        if (rect != nullptr) {
          for (formats::IsomLink& link : rect->links) {
            link.raw &= kClearEditorFlags;
          }
        }
      }
    }
    return success;
  }

  formats::IsomSection& isom_;
  std::vector<std::uint16_t>& editor_tiles_;
  std::vector<std::uint16_t>& game_tiles_;
  std::size_t map_width_{};
  std::size_t map_height_{};
  const IsomCatalog& catalog_;
  const IsomTopology& topology_;
  const starcraft::gds::TilesetData& tileset_;
};

}  // namespace

bool IsomTerrainEditor::paint(
    formats::IsomSection& isom,
    std::vector<std::uint16_t>& editor_tiles,
    std::vector<std::uint16_t>& game_tiles,
    const std::uint16_t map_width,
    const std::uint16_t map_height,
    const std::uint16_t tile_x,
    const std::uint16_t tile_y,
    const std::uint16_t terrain_type,
    const std::uint16_t brush_extent,
    const IsomCatalog& catalog,
    const IsomTopology& topology,
    const starcraft::gds::TilesetData& tileset) noexcept {
  if (!isom.valid() || !catalog.valid() || !topology.valid() ||
      map_width == 0U || map_height == 0U || tile_x >= map_width ||
      tile_y >= map_height ||
      editor_tiles.size() != static_cast<std::size_t>(map_width) * map_height ||
      game_tiles.size() != editor_tiles.size()) {
    return false;
  }
  try {
    formats::IsomSection old_isom = isom;
    std::vector<std::uint16_t> old_editor = editor_tiles;
    std::vector<std::uint16_t> old_game = game_tiles;
    try {
      EditContext context{isom, editor_tiles, game_tiles, map_width, map_height,
                          catalog, topology, tileset};
      if (context.paint(tile_x, tile_y, terrain_type, brush_extent)) {
        return true;
      }
    } catch (...) {
    }
    isom = std::move(old_isom);
    editor_tiles = std::move(old_editor);
    game_tiles = std::move(old_game);
    return false;
  } catch (...) {
    return false;
  }
}

}  // namespace staredit::terrain
