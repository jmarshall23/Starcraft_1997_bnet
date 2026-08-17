#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "starcraft/gds/grp.hpp"

namespace starcraft::runtime {
class StormModule;
}

namespace starcraft::gds {

// Recovered from StarCraft.exe's pointer table at 0x004F2708, consumed by
// lang\gamemap.cpp::sub_454730 at 0x00454730.
[[nodiscard]] std::string_view beta_tileset_name(std::uint16_t tileset_id) noexcept;

struct IndexedMapTile {
  static constexpr std::size_t width = 32;
  static constexpr std::size_t height = 32;
  std::array<std::uint8_t, width * height> pixels{};
};

class TilesetData final {
 public:
  [[nodiscard]] bool load(
      runtime::StormModule& storm,
      std::string_view tileset_name) noexcept;
  [[nodiscard]] bool assign(
      std::vector<std::uint8_t> cv5,
      std::vector<std::uint8_t> vx4,
      std::vector<std::uint8_t> vr4,
      std::vector<std::uint8_t> vf4,
      std::vector<std::uint8_t> wpe,
      std::vector<std::uint8_t> creep_grp = {}) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::size_t group_count() const noexcept;
  [[nodiscard]] std::size_t megatile_count() const noexcept;
  [[nodiscard]] std::size_t minitile_count() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& palette() const noexcept;
  [[nodiscard]] const std::string& failed_asset() const noexcept;

  [[nodiscard]] bool megatile_id(
      std::uint16_t map_tile_id,
      std::uint16_t& output) const noexcept;
  [[nodiscard]] bool render(
      std::uint16_t map_tile_id,
      IndexedMapTile& output) const noexcept;
  [[nodiscard]] bool terrain_flags(
      std::uint16_t map_tile_id,
      std::uint16_t& group_flags,
      std::array<std::uint16_t, 16>& minitile_flags) const noexcept;
  [[nodiscard]] bool walkable(
      std::uint16_t map_tile_id,
      std::uint8_t minitile_x,
      std::uint8_t minitile_y) const noexcept;
  [[nodiscard]] bool buildable(std::uint16_t map_tile_id) const noexcept;
  [[nodiscard]] bool creep_edge_frame(
      std::uint8_t one_based_frame,
      DecodedGrpFrame& output) const noexcept;

 private:
  [[nodiscard]] static std::uint16_t read_u16(
      const std::vector<std::uint8_t>& bytes,
      std::size_t offset) noexcept;

  std::vector<std::uint8_t> cv5_{};
  std::vector<std::uint8_t> vx4_{};
  std::vector<std::uint8_t> vr4_{};
  std::vector<std::uint8_t> vf4_{};
  std::vector<std::uint8_t> wpe_{};
  std::vector<std::uint8_t> creep_grp_{};
  std::string failed_asset_{};
  bool valid_{};
};

}  // namespace starcraft::gds
