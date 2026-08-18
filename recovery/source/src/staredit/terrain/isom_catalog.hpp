#pragma once

#include "starcraft/gds/tileset.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace staredit::terrain {

enum class IsomQuadrant : std::uint8_t {
  top_left,
  top_right,
  bottom_right,
  bottom_left,
};

struct IsomQuadrantLinks {
  std::uint16_t first{};
  std::uint16_t second{};
  std::uint16_t link_id{};
};

struct IsomShapeLinks {
  std::uint16_t terrain_type{};
  std::array<IsomQuadrantLinks, 4> quadrants{};

  [[nodiscard]] std::uint16_t link_id(IsomQuadrant quadrant) const noexcept;
  [[nodiscard]] std::uint16_t edge_link(std::uint16_t raw_isom) const noexcept;
};

struct IsomBrush {
  std::uint16_t terrain_type{};
  std::uint16_t isom_value{};
  std::uint16_t preview_tile{};
  std::string_view name{};
};

class IsomCatalog final {
 public:
  [[nodiscard]] bool build(
      std::uint16_t tileset_id,
      const starcraft::gds::TilesetData& tileset) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::uint16_t tileset_id() const noexcept;
  [[nodiscard]] std::uint32_t diagnostic_code() const noexcept;
  [[nodiscard]] std::size_t terrain_type_count() const noexcept;
  [[nodiscard]] const std::vector<IsomShapeLinks>& links() const noexcept;
  [[nodiscard]] const std::vector<std::uint16_t>& terrain_type_map()
      const noexcept;
  [[nodiscard]] const std::vector<IsomBrush>& brushes() const noexcept;
  [[nodiscard]] std::uint16_t terrain_type_isom_value(
      std::uint16_t terrain_type) const noexcept;
  [[nodiscard]] const IsomBrush* brush_for_preview_tile(
      std::uint16_t tile_id) const noexcept;
  [[nodiscard]] const IsomBrush* brush_for_terrain_type(
      std::uint16_t terrain_type) const noexcept;
  [[nodiscard]] const IsomBrush* brush_for_transition_type(
      std::uint16_t terrain_type) const noexcept;
  [[nodiscard]] bool isom_rect_hash(
      const std::array<std::uint16_t, 4>& raw_sides,
      std::uint32_t& hash) const noexcept;

 private:
  struct TerrainTypeInfo {
    std::uint16_t isom_value{};
    std::int16_t brush_sort_order{-1};
    std::uint16_t link_id{};
    std::string_view name{};
  };

  void reset() noexcept;
  [[nodiscard]] bool load_definition(std::uint16_t tileset_id) noexcept;
  [[nodiscard]] bool expand_terrain_type_map(
      const std::vector<std::uint16_t>& compressed) noexcept;
  [[nodiscard]] bool generate_links(
      const starcraft::gds::TilesetData& tileset) noexcept;

  std::vector<TerrainTypeInfo> terrain_types_{};
  std::vector<std::uint16_t> source_terrain_types_{};
  std::vector<std::vector<std::uint16_t>> explicit_groups_{};
  std::vector<std::uint16_t> compressed_type_map_{};
  std::vector<std::uint16_t> terrain_type_map_{};
  std::vector<IsomShapeLinks> links_{};
  std::vector<IsomBrush> brushes_{};
  std::uint16_t tileset_id_{};
  std::uint32_t diagnostic_code_{};
  bool valid_{};
};

}  // namespace staredit::terrain
