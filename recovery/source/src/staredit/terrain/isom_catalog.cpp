#include "terrain/isom_catalog.hpp"

#include <algorithm>
#include <array>
#include <deque>
#include <limits>
#include <utility>

// The logical-terrain tables and shape reconstruction follow the MIT-licensed
// IsomTerrain reference implementation, cross-checked against StarEdit's CV5
// and ISOM access patterns: https://github.com/TheNitesWhoSay/IsomTerrain

namespace staredit::terrain {
namespace {

constexpr std::size_t kMaximumTerrainGroups = 1024U;
constexpr std::uint16_t kSoftLinkMaximum = 48U;
constexpr std::uint16_t kBottomLeft = 49U;
constexpr std::uint16_t kTopRight = 50U;
constexpr std::uint16_t kBottomRight = 51U;
constexpr std::uint16_t kTopLeft = 52U;
constexpr std::uint16_t kFarRight = 53U;
constexpr std::uint16_t kFarLeft = 54U;
constexpr std::uint16_t kLeftHand = 55U;
constexpr std::uint16_t kRightHand = 56U;
constexpr std::uint16_t kTrBlNorthWest = 255U;
constexpr std::uint16_t kTrBlSouthEast = 256U;
constexpr std::uint16_t kTlBrNorthEast = 257U;
constexpr std::uint16_t kTlBrSouthWest = 258U;
constexpr std::size_t kShapeCount = 14U;
constexpr std::size_t kQuadrantCount = 4U;
constexpr std::size_t kSideCount = 4U;
constexpr std::size_t kNoGroup = (std::numeric_limits<std::size_t>::max)();

enum Side : std::size_t {
  left,
  top,
  right,
  bottom,
};

enum ShapeId : std::size_t {
  edge_north_west,
  edge_north_east,
  edge_south_east,
  edge_south_west,
  jut_out_north,
  jut_out_east,
  jut_out_south,
  jut_out_west,
  jut_in_east,
  jut_in_west,
  jut_in_north,
  jut_in_south,
  horizontal,
  vertical,
};

struct ShapeQuadrantPattern {
  std::array<std::uint16_t, kSideCount> links{};
  std::uint16_t link_id{};
  bool stack_top{};
};

struct ShapePattern {
  std::array<ShapeQuadrantPattern, kQuadrantCount> quadrants{};
};

constexpr std::size_t quadrant_index(const IsomQuadrant quadrant) noexcept {
  return static_cast<std::size_t>(quadrant);
}

ShapeQuadrantPattern pattern(
    const std::uint16_t left_link = 0U,
    const std::uint16_t top_link = 0U,
    const std::uint16_t right_link = 0U,
    const std::uint16_t bottom_link = 0U,
    const std::uint16_t link_id = 0U,
    const bool stack_top = false) noexcept {
  return {{{left_link, top_link, right_link, bottom_link}}, link_id,
          stack_top};
}

std::array<ShapePattern, kShapeCount> shape_patterns() noexcept {
  std::array<ShapePattern, kShapeCount> shapes{};
  auto& nw = shapes[edge_north_west].quadrants;
  nw[1] = pattern(0U, 0U, kBottomRight, kBottomRight,
                  kTrBlNorthWest, true);
  nw[2] = pattern(kBottomRight, kBottomRight);
  nw[3] = pattern(0U, 0U, kBottomRight, kFarRight,
                  kTrBlNorthWest, true);

  auto& ne = shapes[edge_north_east].quadrants;
  ne[0] = pattern(kBottomLeft, 0U, 0U, kBottomLeft,
                  kTlBrNorthEast, true);
  ne[2] = pattern(kBottomLeft, 0U, 0U, kFarLeft,
                  kTlBrNorthEast, true);
  ne[3] = pattern(0U, kBottomLeft, kBottomLeft);

  auto& se = shapes[edge_south_east].quadrants;
  se[0] = pattern(0U, 0U, kTopLeft, kTopLeft);
  se[1] = pattern(kTopLeft, kFarLeft, 0U, 0U,
                  kTrBlSouthEast);
  se[3] = pattern(kTopLeft, kTopLeft, 0U, 0U,
                  kTrBlSouthEast);

  auto& sw = shapes[edge_south_west].quadrants;
  sw[0] = pattern(0U, kFarRight, kTopRight, 0U,
                  kTlBrSouthWest);
  sw[1] = pattern(kTopRight, 0U, 0U, kTopRight);
  sw[2] = pattern(0U, kTopRight, kTopRight, 0U,
                  kTlBrSouthWest);

  auto& out_n = shapes[jut_out_north].quadrants;
  out_n[2] = pattern(kBottomLeft, 0U, 0U, kBottomLeft,
                     kTlBrNorthEast, true);
  out_n[3] = pattern(0U, 0U, kBottomRight, kBottomRight,
                     kTrBlNorthWest, true);

  auto& out_e = shapes[jut_out_east].quadrants;
  out_e[0] = pattern(kBottomLeft, 0U, 0U, kFarLeft,
                     kTlBrNorthEast, true);
  out_e[3] = pattern(kTopLeft, kFarLeft, 0U, 0U,
                     kTrBlSouthEast);

  auto& out_s = shapes[jut_out_south].quadrants;
  out_s[0] = pattern(0U, kTopRight, kTopRight, 0U,
                     kTlBrSouthWest);
  out_s[1] = pattern(kTopLeft, kTopLeft, 0U, 0U,
                     kTrBlSouthEast);

  auto& out_w = shapes[jut_out_west].quadrants;
  out_w[1] = pattern(0U, 0U, kBottomRight, kFarRight,
                     kTrBlNorthWest, true);
  out_w[2] = pattern(0U, kFarRight, kTopRight, 0U,
                     kTlBrSouthWest);

  auto& in_e = shapes[jut_in_east].quadrants;
  in_e[0] = pattern(0U, kFarRight, kTopRight, 0U,
                    kTlBrSouthWest);
  in_e[1] = pattern(kRightHand, 0U, 0U, kRightHand);
  in_e[2] = pattern(kRightHand, kRightHand);
  in_e[3] = pattern(0U, 0U, kBottomRight, kFarRight,
                    kTrBlNorthWest);

  auto& in_w = shapes[jut_in_west].quadrants;
  in_w[0] = pattern(0U, 0U, kLeftHand, kLeftHand);
  in_w[1] = pattern(kTopLeft, kFarLeft, 0U, 0U,
                    kTrBlSouthEast);
  in_w[2] = pattern(kBottomLeft, 0U, 0U, kFarLeft,
                    kTlBrNorthEast);
  in_w[3] = pattern(0U, kLeftHand, kLeftHand);

  auto& in_n = shapes[jut_in_north].quadrants;
  in_n[0] = pattern(kBottomLeft, 0U, 0U, kBottomLeft,
                    kTlBrNorthEast, true);
  in_n[1] = pattern(0U, 0U, kBottomRight, kBottomRight,
                    kTrBlNorthWest, true);
  in_n[2] = pattern(kBottomRight, kBottomRight);
  in_n[3] = pattern(0U, kBottomLeft, kBottomLeft);

  auto& in_s = shapes[jut_in_south].quadrants;
  in_s[0] = pattern(0U, 0U, kTopLeft, kTopLeft);
  in_s[1] = pattern(kTopRight, 0U, 0U, kTopRight);
  in_s[2] = pattern(0U, kTopRight, kTopRight, 0U,
                    kTlBrSouthWest);
  in_s[3] = pattern(kTopLeft, kTopLeft, 0U, 0U,
                    kTrBlSouthEast);

  auto& horiz = shapes[horizontal].quadrants;
  horiz[0] = pattern(0U, kTopRight, kTopRight, 0U,
                     kTlBrSouthWest);
  horiz[1] = pattern(kTopLeft, kTopLeft, 0U, 0U,
                     kTrBlSouthEast);
  horiz[2] = pattern(kBottomLeft, 0U, 0U, kBottomLeft,
                     kTlBrNorthEast);
  horiz[3] = pattern(0U, 0U, kBottomRight, kBottomRight,
                     kTrBlNorthWest);

  auto& vert = shapes[vertical].quadrants;
  vert[0] = pattern(kBottomLeft, 0U, 0U, kFarLeft,
                    kTlBrNorthEast);
  vert[1] = pattern(0U, 0U, kBottomRight, kFarRight,
                    kTrBlNorthWest);
  vert[2] = pattern(0U, kFarRight, kTopRight, 0U,
                    kTlBrSouthWest);
  vert[3] = pattern(kTopLeft, kFarLeft, 0U, 0U,
                    kTrBlSouthEast);
  return shapes;
}

bool is_shape_quadrant(
    const std::array<std::uint16_t, 4>& links) noexcept {
  const bool none_hard = std::all_of(
      links.begin(), links.end(), [](const std::uint16_t value) {
        return value <= kSoftLinkMaximum;
      });
  const bool all_hard = std::all_of(
      links.begin(), links.end(), [](const std::uint16_t value) {
        return value > kSoftLinkMaximum;
      });
  return !none_hard && !all_hard;
}

bool matches(const ShapeQuadrantPattern& check,
             const starcraft::gds::Cv5TileGroup& group) noexcept {
  for (std::size_t side = 0U; side < kSideCount; ++side) {
    const std::uint16_t actual = group.directional_links[side];
    const std::uint16_t expected = check.links[side];
    if (actual != expected &&
        (actual > kSoftLinkMaximum || expected > kSoftLinkMaximum)) {
      return false;
    }
  }
  const bool no_stack_above = group.stack_connections[top] == 0U;
  return no_stack_above || !check.stack_top;
}

void populate_quadrant(IsomQuadrantLinks& destination,
                       const IsomQuadrant quadrant,
                       const starcraft::gds::Cv5TileGroup& group) noexcept {
  switch (quadrant) {
    case IsomQuadrant::top_left:
      destination.first = group.directional_links[right];
      destination.second = group.directional_links[bottom];
      break;
    case IsomQuadrant::top_right:
      destination.first = group.directional_links[left];
      destination.second = group.directional_links[bottom];
      break;
    case IsomQuadrant::bottom_right:
      destination.first = group.directional_links[left];
      destination.second = group.directional_links[top];
      break;
    case IsomQuadrant::bottom_left:
      destination.first = group.directional_links[top];
      destination.second = group.directional_links[right];
      break;
  }
}

}  // namespace

std::uint16_t IsomShapeLinks::link_id(
    const IsomQuadrant quadrant) const noexcept {
  return quadrants[quadrant_index(quadrant)].link_id;
}

std::uint16_t IsomShapeLinks::edge_link(
    const std::uint16_t raw_isom) const noexcept {
  switch (raw_isom & 0x000EU) {
    case 0x0000U:
      return quadrants[0].first;
    case 0x0002U:
      return quadrants[0].second;
    case 0x0004U:
      return quadrants[1].first;
    case 0x0006U:
      return quadrants[1].second;
    case 0x0008U:
      return quadrants[2].first;
    case 0x000AU:
      return quadrants[2].second;
    case 0x000CU:
      return quadrants[3].first;
    default:
      return quadrants[3].second;
  }
}

bool IsomCatalog::build(
    const std::uint16_t tileset_id,
    const starcraft::gds::TilesetData& tileset) noexcept {
  reset();
  diagnostic_code_ = 0U;
  if (!tileset.valid() || !load_definition(tileset_id)) {
    diagnostic_code_ = 1U;
    reset();
    return false;
  }
  try {
    if (!expand_terrain_type_map(compressed_type_map_) ||
        !generate_links(tileset)) {
      reset();
      return false;
    }
    tileset_id_ = tileset_id;
    valid_ = !links_.empty() && !brushes_.empty();
    return valid_;
  } catch (...) {
    reset();
    return false;
  }
}

bool IsomCatalog::valid() const noexcept { return valid_; }
std::uint16_t IsomCatalog::tileset_id() const noexcept { return tileset_id_; }
std::uint32_t IsomCatalog::diagnostic_code() const noexcept {
  return diagnostic_code_;
}
std::size_t IsomCatalog::terrain_type_count() const noexcept {
  return valid_ ? terrain_types_.size() : 0U;
}
const std::vector<IsomShapeLinks>& IsomCatalog::links() const noexcept {
  return links_;
}
const std::vector<std::uint16_t>& IsomCatalog::terrain_type_map()
    const noexcept {
  return terrain_type_map_;
}
const std::vector<IsomBrush>& IsomCatalog::brushes() const noexcept {
  return brushes_;
}

std::uint16_t IsomCatalog::terrain_type_isom_value(
    const std::uint16_t terrain_type) const noexcept {
  return valid_ && terrain_type < terrain_types_.size()
             ? terrain_types_[terrain_type].isom_value
             : 0U;
}

const IsomBrush* IsomCatalog::brush_for_preview_tile(
    const std::uint16_t tile_id) const noexcept {
  const std::uint16_t group = static_cast<std::uint16_t>(tile_id >> 4U);
  for (const IsomBrush& brush : brushes_) {
    if ((brush.preview_tile >> 4U) == group) {
      return &brush;
    }
  }
  return nullptr;
}

const IsomBrush* IsomCatalog::brush_for_terrain_type(
    const std::uint16_t terrain_type) const noexcept {
  for (const IsomBrush& brush : brushes_) {
    if (brush.terrain_type == terrain_type) {
      return &brush;
    }
  }
  return nullptr;
}

const IsomBrush* IsomCatalog::brush_for_transition_type(
    const std::uint16_t terrain_type) const noexcept {
  if (const IsomBrush* const direct = brush_for_terrain_type(terrain_type)) {
    return direct;
  }
  const std::size_t count = terrain_types_.size();
  if (!valid_ || terrain_type >= count ||
      terrain_type_map_.size() != count * count) {
    return nullptr;
  }
  for (const IsomBrush& brush : brushes_) {
    if (terrain_type_map_[static_cast<std::size_t>(brush.terrain_type) * count +
                          terrain_type] == terrain_type) {
      return &brush;
    }
  }
  return nullptr;
}

bool IsomCatalog::isom_rect_hash(
    const std::array<std::uint16_t, 4>& raw_sides,
    std::uint32_t& hash) const noexcept {
  hash = 0U;
  if (!valid_) {
    return false;
  }
  std::uint16_t last_terrain_type{};
  for (const std::uint16_t raw : raw_sides) {
    const std::uint16_t clean = raw & 0x7FFEU;
    const std::size_t isom_value = clean >> 4U;
    if (isom_value >= links_.size()) {
      hash = 0U;
      return false;
    }
    const IsomShapeLinks& shape = links_[isom_value];
    const std::uint16_t edge = shape.edge_link(clean);
    hash = (hash | edge) << 6U;
    if (shape.terrain_type != 0U && edge > kSoftLinkMaximum) {
      last_terrain_type = shape.terrain_type;
    }
  }
  hash |= last_terrain_type;
  return true;
}

void IsomCatalog::reset() noexcept {
  terrain_types_.clear();
  source_terrain_types_.clear();
  explicit_groups_.clear();
  compressed_type_map_.clear();
  terrain_type_map_.clear();
  links_.clear();
  brushes_.clear();
  tileset_id_ = 0U;
  valid_ = false;
}

bool IsomCatalog::load_definition(const std::uint16_t tileset_id) noexcept {
  const auto initialize = [this](const std::size_t count) {
    terrain_types_.assign(count, TerrainTypeInfo{});
    source_terrain_types_.resize(count);
    explicit_groups_.assign(count, {});
    for (std::size_t index = 0U; index < count; ++index) {
      source_terrain_types_[index] = static_cast<std::uint16_t>(index);
    }
  };
  const auto set = [this](const std::size_t index,
                          const std::uint16_t isom_value,
                          const std::int16_t sort = -1,
                          const std::uint16_t link_id = 0U,
                          const std::string_view name = {}) {
    terrain_types_[index] = {isom_value, sort, link_id, name};
  };
  try {
    switch (tileset_id) {
      case 0U:  // Badlands
        initialize(36U);
        set(0U, 10U);
        set(2U, 1U, 0, 1U, "Dirt");
        set(3U, 2U, 2, 2U, "High Dirt");
        set(4U, 9U, 1, 4U, "Mud");
        set(5U, 3U, 3, 3U, "Water");
        set(6U, 4U, 4, 5U, "Grass");
        set(7U, 7U, 5, 6U, "High Grass");
        set(14U, 5U, 7, 9U, "Asphalt");
        set(15U, 6U, 8, 10U, "Rocky Ground");
        set(18U, 8U, 6, 7U, "Structure");
        set(20U, 41U); set(21U, 69U); set(22U, 111U);
        set(27U, 83U); set(28U, 55U); set(31U, 97U);
        set(34U, 13U); set(35U, 27U);
        explicit_groups_[2U] = {2U};
        explicit_groups_[3U] = {4U};
        explicit_groups_[4U] = {20U};
        explicit_groups_[5U] = {6U};
        explicit_groups_[6U] = {8U};
        explicit_groups_[7U] = {10U};
        explicit_groups_[14U] = {16U};
        explicit_groups_[15U] = {12U};
        explicit_groups_[18U] = {18U};
        source_terrain_types_[34U] = 33U;
        source_terrain_types_[35U] = 34U;
        source_terrain_types_[28U] = 21U;
        source_terrain_types_[21U] = 27U;
        source_terrain_types_[27U] = 26U;
        source_terrain_types_[31U] = 30U;
        for (std::uint16_t group = 0U; group < 1024U; group += 2U) {
          explicit_groups_[22U].push_back(group);
        }
        compressed_type_map_ = {
            5,35,0, 35,5,2,20,27,28,34,22,0,
            2,34,35,20,27,28,22,0,
            34,2,3,20,21,27,28,35,22,0, 3,34,21,0,
            6,20,0, 20,6,2,35,34,27,28,22,0,
            14,27,31,0, 27,14,20,2,35,34,28,22,0,
            15,28,0, 28,15,2,34,35,20,27,22,0,
            7,21,0, 21,7,3,34,0, 18,31,0, 31,18,14,0,
            4,22,0, 22,4,2,34,35,20,27,28,0, 0};
        break;
      case 1U:  // Space platform
        initialize(22U);
        set(0U, 3U);
        set(2U, 1U, 0, 1U, "Space");
        set(3U, 2U, 3, 3U, "Platform");
        set(4U, 11U, 5, 4U, "Plating");
        set(5U, 4U, 7, 5U, "High Platform");
        set(6U, 12U, 8, 6U, "High Plating");
        set(7U, 8U, 6, 7U, "Solar Array");
        set(8U, 9U, 1, 8U, "Low Platform");
        set(9U, 10U, 2, 9U, "Rusty Pit");
        set(10U, 13U, 9, 10U, "Elevated Catwalk");
        set(11U, 14U, 4, 2U, "Dark Platform");
        set(13U, 136U); set(14U, 94U); set(15U, 108U);
        set(16U, 52U); set(17U, 66U); set(18U, 80U);
        set(19U, 122U); set(20U, 24U); set(21U, 38U);
        compressed_type_map_ = {
            2,20,0, 20,2,3,16,14,21,13,0,
            3,20,21,16,17,18,14,19,13,0,
            21,3,5,14,16,15,19,20,17,13,0, 5,21,15,0,
            7,16,0, 16,7,3,20,21,17,18,14,19,13,0,
            8,17,0, 17,8,3,16,14,21,13,0,
            9,18,0, 18,9,3,16,14,13,0,
            4,14,0, 14,4,3,20,21,16,17,18,19,13,0,
            6,15,0, 15,6,5,21,0,
            10,19,0, 19,10,3,16,14,21,13,0,
            11,13,0, 13,11,3,20,21,16,17,18,14,19,0, 0};
        break;
      case 2U:  // Installation
        initialize(16U);
        set(0U, 8U);
        set(2U, 1U, 0, 1U, "Substructure");
        set(3U, 2U, 1, 2U, "Floor");
        set(4U, 4U, 3, 4U, "Substructure Plating");
        set(5U, 5U, 4, 5U, "Plating");
        set(6U, 3U, 2, 3U, "Roof");
        set(7U, 7U, 6, 7U, "Bottomless Pit");
        set(8U, 6U, 5, 6U, "Substructure Panels");
        set(10U, 50U); set(11U, 64U); set(12U, 22U);
        set(13U, 36U); set(14U, 78U); set(15U, 92U);
        compressed_type_map_ = {
            2,12,10,14,15,0, 12,2,3,10,11,13,14,15,0,
            3,12,13,11,0, 13,6,3,11,12,0, 6,13,0,
            4,10,0, 10,4,2,12,14,15,0,
            5,11,0, 11,5,3,12,13,0,
            8,14,0, 14,8,2,12,10,15,0,
            7,15,0, 15,7,2,12,10,14,0, 0};
        break;
      case 3U:  // Ashworld
        initialize(18U);
        set(0U, 9U);
        set(2U, 2U, 1, 2U, "Dirt");
        set(3U, 3U, 2, 3U, "Lava");
        set(4U, 5U, 5, 5U, "High Dirt");
        set(5U, 6U, 6, 6U, "High Lava");
        set(6U, 4U, 3, 4U, "Shale");
        set(7U, 7U, 7, 7U, "High Shale");
        set(8U, 1U, 0, 1U, "Magma");
        set(9U, 8U, 4, 8U, "Broken Rock");
        set(11U, 55U); set(12U, 69U); set(13U, 83U);
        set(14U, 97U); set(15U, 111U); set(16U, 41U); set(17U, 27U);
        compressed_type_map_ = {
            8,17,0, 17,8,2,11,13,16,15,0,
            2,17,16,11,13,15,0, 3,11,0,
            11,3,2,17,16,13,15,0, 6,13,0,
            13,6,2,17,16,11,15,0, 9,15,0,
            15,9,13,2,17,16,11,0,
            16,2,4,11,13,12,14,17,15,0,
            4,16,12,14,0, 5,12,0, 12,5,4,16,14,0,
            7,14,0, 14,7,4,16,12,0, 0};
        break;
      case 4U:  // Jungle
        initialize(36U);
        set(0U, 14U);
        set(2U, 1U, 1, 1U, "Dirt");
        set(3U, 2U, 8, 2U, "High Dirt");
        set(4U, 13U, 2, 4U, "Mud");
        set(5U, 3U, 0, 3U, "Water");
        set(8U, 4U, 3, 8U, "Jungle");
        set(9U, 5U, 6, 11U, "Raised Jungle");
        set(10U, 9U, 9, 14U, "High Jungle");
        set(11U, 7U, 5, 12U, "Ruins");
        set(12U, 10U, 10, 15U, "High Ruins");
        set(13U, 11U, 11, 16U, "High Raised Jungle");
        set(15U, 6U, 4, 10U, "Rocky Ground");
        set(16U, 8U, 7, 13U, "Temple");
        set(17U, 12U, 12, 17U, "High Temple");
        set(22U, 171U); set(23U, 45U); set(24U, 115U);
        set(25U, 87U); set(26U, 129U); set(28U, 59U);
        set(29U, 73U); set(30U, 143U); set(32U, 101U);
        set(33U, 157U); set(34U, 17U); set(35U, 31U);
        explicit_groups_[2U] = {2U};
        explicit_groups_[3U] = {4U};
        explicit_groups_[4U] = {26U};
        explicit_groups_[5U] = {6U};
        explicit_groups_[8U] = {8U};
        explicit_groups_[9U] = {12U};
        explicit_groups_[10U] = {18U};
        explicit_groups_[11U] = {14U};
        explicit_groups_[12U] = {20U};
        explicit_groups_[13U] = {22U};
        explicit_groups_[15U] = {10U};
        explicit_groups_[16U] = {16U};
        explicit_groups_[17U] = {24U};
        source_terrain_types_[34U] = 33U;
        source_terrain_types_[35U] = 34U;
        source_terrain_types_[23U] = 22U;
        source_terrain_types_[28U] = 23U;
        source_terrain_types_[29U] = 27U;
        source_terrain_types_[25U] = 24U;
        source_terrain_types_[32U] = 25U;
        source_terrain_types_[24U] = 28U;
        source_terrain_types_[26U] = 29U;
        source_terrain_types_[30U] = 31U;
        source_terrain_types_[33U] = 32U;
        for (std::uint16_t group = 0U; group < 1024U; group += 2U) {
          explicit_groups_[22U].push_back(group);
        }
        compressed_type_map_ = {
            5,35,0, 35,5,2,23,28,34,22,0,
            2,34,35,23,28,22,0,
            34,2,3,24,23,28,35,22,0, 3,34,24,0,
            8,23,29,25,32,0, 4,22,0,
            22,4,2,34,35,23,28,0,
            23,8,2,35,34,28,25,29,22,0,
            15,28,0, 28,15,2,34,35,23,22,0,
            9,29,0, 29,9,8,25,32,23,0,
            11,25,0, 25,11,8,23,29,32,0,
            16,32,0, 32,16,8,25,29,0,
            10,24,26,30,33,0, 24,10,3,34,26,30,0,
            12,26,0, 26,12,10,24,30,33,0,
            13,30,0, 30,13,10,26,24,33,0,
            17,33,0, 33,17,10,26,30,0, 0};
        break;
      default:
        return false;
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool IsomCatalog::expand_terrain_type_map(
    const std::vector<std::uint16_t>& compressed) noexcept {
  const std::size_t count = terrain_types_.size();
  if (count == 0U || compressed.empty()) {
    return false;
  }
  try {
    std::vector<std::uint16_t> temporary(count * count,
                                         std::uint16_t{0});
    std::size_t cursor{};
    while (cursor < compressed.size() && compressed[cursor] != 0U) {
      const std::uint16_t source = compressed[cursor++];
      if (source >= count) {
        return false;
      }
      std::size_t destination = static_cast<std::size_t>(source) * count;
      while (cursor < compressed.size() && compressed[cursor] != 0U) {
        const std::uint16_t next = compressed[cursor++];
        if (next >= count || destination >= temporary.size()) {
          return false;
        }
        temporary[destination++] = next;
      }
      if (cursor >= compressed.size()) {
        return false;
      }
      ++cursor;
    }

    terrain_type_map_.assign(count * count, std::uint16_t{0});
    std::vector<std::uint16_t> row_data(count, std::uint16_t{0});
    for (std::size_t source = count; source-- > 0U;) {
      std::fill(row_data.begin(), row_data.end(), std::uint16_t{0});
      std::deque<std::uint16_t> pending{
          static_cast<std::uint16_t>(source)};
      terrain_type_map_[source * count + source] =
          static_cast<std::uint16_t>(source);
      while (!pending.empty()) {
        const std::uint16_t destination_row = pending.front();
        pending.pop_front();
        const std::size_t output_start = source * count;
        for (std::size_t input =
                 static_cast<std::size_t>(destination_row) * count;
             input < temporary.size() && temporary[input] != 0U; ++input) {
          const std::uint16_t path = temporary[input];
          if (terrain_type_map_[output_start + path] == 0U) {
            const std::uint16_t next = row_data[destination_row] == 0U
                                           ? path
                                           : row_data[destination_row];
            pending.push_back(path);
            terrain_type_map_[output_start + path] = next;
            row_data[path] = next;
          }
        }
      }
    }
    return true;
  } catch (...) {
    terrain_type_map_.clear();
    return false;
  }
}

bool IsomCatalog::generate_links(
    const starcraft::gds::TilesetData& tileset) noexcept {
  const std::size_t total_groups =
      (std::min)(tileset.group_count(), kMaximumTerrainGroups);
  if (total_groups < 2U || terrain_types_.size() < 4U) {
    diagnostic_code_ = 10U;
    return false;
  }
  std::vector<std::vector<std::uint16_t>> groups_by_type(
      terrain_types_.size());
  std::vector<starcraft::gds::Cv5TileGroup> groups(total_groups);
  for (std::size_t group_id = 0U; group_id < total_groups; ++group_id) {
    if (!tileset.tile_group(group_id, groups[group_id])) {
      return false;
    }
  }
  for (std::size_t type = 1U; type < groups_by_type.size(); ++type) {
    if (!explicit_groups_[type].empty()) {
      for (const std::uint16_t group_id : explicit_groups_[type]) {
        if (group_id >= total_groups || (group_id & 1U) != 0U) {
          diagnostic_code_ = 20U + static_cast<std::uint32_t>(type);
          return false;
        }
        groups_by_type[type].push_back(group_id);
      }
      continue;
    }
    const std::uint16_t source_type = source_terrain_types_[type];
    if (source_type == 0U) {
      continue;
    }
    for (std::size_t group_id = 0U; group_id < total_groups;
         group_id += 2U) {
      if (groups[group_id].terrain_type == source_type) {
        groups_by_type[type].push_back(
            static_cast<std::uint16_t>(group_id));
      }
    }
  }

  const std::size_t solid_end = terrain_types_.size() / 2U;
  std::size_t link_count = 1U;
  for (std::size_t type = 1U; type < terrain_types_.size(); ++type) {
    const std::uint16_t start = terrain_types_[type].isom_value;
    if (start != 0U) {
      link_count = (std::max)(link_count,
                              static_cast<std::size_t>(start) +
                                  (type <= solid_end ? 1U : kShapeCount));
    }
  }
  links_.assign(link_count, IsomShapeLinks{});

  for (std::size_t type = 1U; type <= solid_end; ++type) {
    const TerrainTypeInfo& info = terrain_types_[type];
    if (info.isom_value == 0U) {
      continue;
    }
    if (groups_by_type[type].empty() || info.isom_value >= links_.size()) {
      diagnostic_code_ = 100U + static_cast<std::uint32_t>(type);
      return false;
    }
    const auto& directional =
        groups[groups_by_type[type].front()].directional_links;
    IsomShapeLinks& shape = links_[info.isom_value];
    shape.terrain_type = static_cast<std::uint16_t>(type);
    shape.quadrants[0] = {directional[right], directional[bottom],
                          info.link_id};
    shape.quadrants[1] = {directional[left], directional[bottom],
                          info.link_id};
    shape.quadrants[2] = {directional[left], directional[top],
                          info.link_id};
    shape.quadrants[3] = {directional[top], directional[right],
                          info.link_id};
  }

  const auto patterns = shape_patterns();
  for (std::size_t type = solid_end + 1U;
       type < terrain_types_.size(); ++type) {
    const std::uint16_t start = terrain_types_[type].isom_value;
    if (start == 0U) {
      continue;
    }
    if (groups_by_type[type].empty() ||
        static_cast<std::size_t>(start) + kShapeCount > links_.size()) {
      diagnostic_code_ = 200U + static_cast<std::uint32_t>(type);
      return false;
    }
    std::array<std::array<std::size_t, kQuadrantCount>, kShapeCount>
        shape_groups{};
    for (auto& by_quadrant : shape_groups) {
      by_quadrant.fill(kNoGroup);
    }
    for (std::size_t shape = 0U; shape < kShapeCount; ++shape) {
      links_[static_cast<std::size_t>(start) + shape].terrain_type =
          static_cast<std::uint16_t>(type);
    }
    for (const std::uint16_t group_id : groups_by_type[type]) {
      const auto& group = groups[group_id];
      if (!is_shape_quadrant(group.directional_links)) {
        continue;
      }
      for (std::size_t shape = 0U; shape < kShapeCount; ++shape) {
        for (std::size_t quadrant = 0U; quadrant < kQuadrantCount;
             ++quadrant) {
          if (matches(patterns[shape].quadrants[quadrant], group)) {
            populate_quadrant(
                links_[static_cast<std::size_t>(start) + shape]
                    .quadrants[quadrant],
                static_cast<IsomQuadrant>(quadrant), group);
            shape_groups[shape][quadrant] = group_id;
          }
        }
      }
    }

    auto& shapes = links_;
    const auto shape_at = [&shapes, start](const std::size_t shape)
        -> IsomShapeLinks& {
      return shapes[static_cast<std::size_t>(start) + shape];
    };
    const auto group_at = [&groups, &shape_groups](
                              const std::size_t shape,
                              const std::size_t quadrant)
        -> const starcraft::gds::Cv5TileGroup* {
      const std::size_t id = shape_groups[shape][quadrant];
      return id < groups.size() ? &groups[id] : nullptr;
    };

    if (shape_at(jut_in_east).quadrants[1].first == 0U) {
      const auto* upper = group_at(edge_north_east, 3U);
      const auto* lower = group_at(edge_south_east, 0U);
      if (upper == nullptr || lower == nullptr) {
        diagnostic_code_ = 300U + static_cast<std::uint32_t>(type);
        return false;
      }
      shape_at(jut_in_east).quadrants[1].first =
          upper->directional_links[left];
      shape_at(jut_in_east).quadrants[1].second =
          upper->directional_links[bottom];
      shape_at(jut_in_east).quadrants[2].first =
          lower->directional_links[left];
      shape_at(jut_in_east).quadrants[2].second =
          lower->directional_links[top];
    }
    if (shape_at(jut_in_west).quadrants[0].first == 0U) {
      const auto* upper = group_at(edge_north_west, 2U);
      const auto* lower = group_at(edge_south_west, 1U);
      if (upper == nullptr || lower == nullptr) {
        diagnostic_code_ = 400U + static_cast<std::uint32_t>(type);
        return false;
      }
      shape_at(jut_in_west).quadrants[0].first =
          upper->directional_links[right];
      shape_at(jut_in_west).quadrants[0].second =
          upper->directional_links[bottom];
      shape_at(jut_in_west).quadrants[3].first =
          lower->directional_links[top];
      shape_at(jut_in_west).quadrants[3].second =
          lower->directional_links[right];
    }

    shape_at(edge_north_west).quadrants[0].first =
        shape_at(edge_north_west).quadrants[1].first;
    shape_at(edge_north_west).quadrants[0].second =
        shape_at(edge_north_west).quadrants[3].first;
    shape_at(edge_north_east).quadrants[1].first =
        shape_at(edge_north_east).quadrants[0].first;
    shape_at(edge_north_east).quadrants[1].second =
        shape_at(edge_north_east).quadrants[2].second;
    shape_at(edge_south_east).quadrants[2].first =
        shape_at(edge_south_east).quadrants[3].second;
    shape_at(edge_south_east).quadrants[2].second =
        shape_at(edge_south_east).quadrants[1].second;
    shape_at(edge_south_west).quadrants[3].first =
        shape_at(edge_south_west).quadrants[0].second;
    shape_at(edge_south_west).quadrants[3].second =
        shape_at(edge_south_west).quadrants[2].first;

    shape_at(jut_out_north).quadrants[0].second =
        shape_at(jut_out_north).quadrants[3].first;
    shape_at(jut_out_north).quadrants[0].first =
        shape_at(jut_out_north).quadrants[0].second;
    shape_at(jut_out_north).quadrants[1].second =
        shape_at(jut_out_north).quadrants[2].second;
    shape_at(jut_out_north).quadrants[1].first =
        shape_at(jut_out_north).quadrants[1].second;
    std::uint16_t fill = shape_at(jut_out_east).quadrants[0].first;
    shape_at(jut_out_east).quadrants[1].first = fill;
    shape_at(jut_out_east).quadrants[1].second = fill;
    shape_at(jut_out_east).quadrants[2].first = fill;
    shape_at(jut_out_east).quadrants[2].second = fill;
    shape_at(jut_out_south).quadrants[2].second =
        shape_at(jut_out_south).quadrants[1].second;
    shape_at(jut_out_south).quadrants[2].first =
        shape_at(jut_out_south).quadrants[2].second;
    shape_at(jut_out_south).quadrants[3].first =
        shape_at(jut_out_south).quadrants[0].second;
    shape_at(jut_out_south).quadrants[3].second =
        shape_at(jut_out_south).quadrants[3].first;
    fill = shape_at(jut_out_west).quadrants[1].first;
    shape_at(jut_out_west).quadrants[0].first = fill;
    shape_at(jut_out_west).quadrants[0].second = fill;
    shape_at(jut_out_west).quadrants[3].second = fill;
    shape_at(jut_out_west).quadrants[3].first = fill;

    for (std::size_t shape = 0U; shape < kShapeCount; ++shape) {
      for (std::size_t quadrant = 0U; quadrant < kQuadrantCount;
           ++quadrant) {
        const std::uint16_t hardcoded =
            patterns[shape].quadrants[quadrant].link_id;
        if (hardcoded >= kTrBlNorthWest) {
          shape_at(shape).quadrants[quadrant].link_id = hardcoded;
        }
      }
    }

    const auto* outer_group = group_at(edge_north_west, 1U);
    const auto* inner_group = group_at(edge_north_west, 2U);
    if (outer_group == nullptr || inner_group == nullptr) {
      diagnostic_code_ = 500U + static_cast<std::uint32_t>(type);
      return false;
    }
    const auto fill_outer = [&shape_at](const std::uint16_t link_id) {
      shape_at(edge_north_west).quadrants[0].link_id = link_id;
      shape_at(edge_north_east).quadrants[1].link_id = link_id;
      shape_at(edge_south_east).quadrants[2].link_id = link_id;
      shape_at(edge_south_west).quadrants[3].link_id = link_id;
      shape_at(jut_out_north).quadrants[0].link_id = link_id;
      shape_at(jut_out_north).quadrants[1].link_id = link_id;
      shape_at(jut_out_east).quadrants[1].link_id = link_id;
      shape_at(jut_out_east).quadrants[2].link_id = link_id;
      shape_at(jut_out_west).quadrants[0].link_id = link_id;
      shape_at(jut_out_west).quadrants[3].link_id = link_id;
      shape_at(jut_out_south).quadrants[2].link_id = link_id;
      shape_at(jut_out_south).quadrants[3].link_id = link_id;
    };
    const auto fill_inner = [&shape_at](const std::uint16_t link_id) {
      shape_at(edge_north_west).quadrants[2].link_id = link_id;
      shape_at(edge_north_east).quadrants[3].link_id = link_id;
      shape_at(edge_south_east).quadrants[0].link_id = link_id;
      shape_at(edge_south_west).quadrants[1].link_id = link_id;
      shape_at(jut_in_east).quadrants[1].link_id = link_id;
      shape_at(jut_in_east).quadrants[2].link_id = link_id;
      shape_at(jut_in_west).quadrants[0].link_id = link_id;
      shape_at(jut_in_west).quadrants[3].link_id = link_id;
      shape_at(jut_in_north).quadrants[2].link_id = link_id;
      shape_at(jut_in_north).quadrants[3].link_id = link_id;
      shape_at(jut_in_south).quadrants[0].link_id = link_id;
      shape_at(jut_in_south).quadrants[1].link_id = link_id;
    };
    for (std::size_t solid = 1U; solid <= solid_end; ++solid) {
      const std::uint16_t isom = terrain_types_[solid].isom_value;
      if (isom == 0U || isom >= links_.size()) {
        continue;
      }
      const IsomShapeLinks& brush = links_[isom];
      if (brush.quadrants[0].first ==
          outer_group->directional_links[left]) {
        fill_outer(brush.quadrants[0].link_id);
      }
      if (brush.quadrants[0].first ==
          inner_group->directional_links[right]) {
        fill_inner(brush.quadrants[0].link_id);
      }
    }
  }

  brushes_.clear();
  for (std::size_t type = 1U; type < terrain_types_.size(); ++type) {
    const TerrainTypeInfo& info = terrain_types_[type];
    if (info.brush_sort_order < 0 || info.isom_value == 0U ||
        groups_by_type[type].empty()) {
      continue;
    }
    const std::uint16_t preview = static_cast<std::uint16_t>(
        groups_by_type[type].front() << 4U);
    brushes_.push_back({static_cast<std::uint16_t>(type), info.isom_value,
                        preview, info.name});
  }
  std::sort(brushes_.begin(), brushes_.end(),
            [this](const IsomBrush& lhs, const IsomBrush& rhs) {
              return terrain_types_[lhs.terrain_type].brush_sort_order <
                     terrain_types_[rhs.terrain_type].brush_sort_order;
            });
  return !brushes_.empty();
}

}  // namespace staredit::terrain
