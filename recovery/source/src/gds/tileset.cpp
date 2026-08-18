#include "starcraft/gds/tileset.hpp"

#include "starcraft/runtime/storm.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::gds {
namespace {

constexpr std::array<std::string_view, 5> kBetaTilesetNames{{
    "badlands",
    "platform",
    "install",
    "AshWorld",
    "Jungle",
}};

constexpr std::size_t kCv5GroupBytes = 52;
constexpr std::size_t kCv5MegaTileOffset = 20;
constexpr std::size_t kMegaTilesPerGroup = 16;
constexpr std::size_t kMaximumTerrainGroups = 1024;
constexpr std::size_t kVx4MegaTileBytes = 32;
constexpr std::size_t kVr4MiniTileBytes = 64;
constexpr std::size_t kVf4MegaTileBytes = 32;
constexpr std::uint16_t kTerrainBlocksBuilding = 0x0800U;

bool load_asset(
    runtime::StormModule& storm,
    const std::string& path,
    std::vector<std::uint8_t>& output,
    std::string& failed_asset) {
  if (!storm.load_file(path.c_str(), output)) {
    failed_asset = path;
    return false;
  }
  return true;
}

}  // namespace

std::string_view beta_tileset_name(const std::uint16_t tileset_id) noexcept {
  return tileset_id < kBetaTilesetNames.size() ? kBetaTilesetNames[tileset_id]
                                               : std::string_view{};
}

bool TilesetData::load(
    runtime::StormModule& storm,
    const std::string_view tileset_name) noexcept {
  try {
    const std::string prefix = std::string{"tileset\\"} + std::string{tileset_name};
    std::vector<std::uint8_t> cv5;
    std::vector<std::uint8_t> vx4;
    std::vector<std::uint8_t> vr4;
    std::vector<std::uint8_t> vf4;
    std::vector<std::uint8_t> wpe;
    std::vector<std::uint8_t> creep_grp;
    failed_asset_.clear();
    if (!load_asset(storm, prefix + ".cv5", cv5, failed_asset_) ||
        !load_asset(storm, prefix + ".vx4", vx4, failed_asset_) ||
        !load_asset(storm, prefix + ".vr4", vr4, failed_asset_) ||
        !load_asset(storm, prefix + ".vf4", vf4, failed_asset_) ||
        !load_asset(storm, prefix + ".wpe", wpe, failed_asset_) ||
        !load_asset(storm, prefix + ".grp", creep_grp, failed_asset_)) {
      valid_ = false;
      return false;
    }
    return assign(
        std::move(cv5),
        std::move(vx4),
        std::move(vr4),
        std::move(vf4),
        std::move(wpe),
        std::move(creep_grp));
  } catch (...) {
    valid_ = false;
    return false;
  }
}

bool TilesetData::assign(
    std::vector<std::uint8_t> cv5,
    std::vector<std::uint8_t> vx4,
    std::vector<std::uint8_t> vr4,
    std::vector<std::uint8_t> vf4,
    std::vector<std::uint8_t> wpe,
    std::vector<std::uint8_t> creep_grp) noexcept {
  valid_ = false;
  if (cv5.empty() || cv5.size() % kCv5GroupBytes != 0 || vx4.empty() ||
      vx4.size() % kVx4MegaTileBytes != 0 || vr4.empty() ||
      vr4.size() % kVr4MiniTileBytes != 0 || vf4.empty() ||
      vf4.size() % kVf4MegaTileBytes != 0 ||
      vf4.size() / kVf4MegaTileBytes != vx4.size() / kVx4MegaTileBytes ||
      wpe.size() != 1024) {
    return false;
  }
  try {
    cv5_ = std::move(cv5);
    vx4_ = std::move(vx4);
    vr4_ = std::move(vr4);
    vf4_ = std::move(vf4);
    wpe_ = std::move(wpe);
    creep_grp_ = std::move(creep_grp);
    valid_ = true;
    return true;
  } catch (...) {
    return false;
  }
}

bool TilesetData::valid() const noexcept { return valid_; }

std::size_t TilesetData::group_count() const noexcept {
  return valid_ ? cv5_.size() / kCv5GroupBytes : 0;
}

std::size_t TilesetData::megatile_count() const noexcept {
  return valid_ ? vx4_.size() / kVx4MegaTileBytes : 0;
}

std::size_t TilesetData::minitile_count() const noexcept {
  return valid_ ? vr4_.size() / kVr4MiniTileBytes : 0;
}

const std::vector<std::uint8_t>& TilesetData::palette() const noexcept { return wpe_; }

const std::string& TilesetData::failed_asset() const noexcept { return failed_asset_; }

bool TilesetData::tile_group(const std::size_t group_id,
                             Cv5TileGroup& output) const noexcept {
  output = {};
  if (!valid_ || group_id >= group_count()) {
    return false;
  }
  const std::size_t offset = group_id * kCv5GroupBytes;
  output.terrain_type = read_u16(cv5_, offset);
  output.buildability = cv5_[offset + 2U];
  output.ground_height = cv5_[offset + 3U];
  for (std::size_t side = 0U; side < output.directional_links.size(); ++side) {
    output.directional_links[side] =
        read_u16(cv5_, offset + 4U + 2U * side);
    output.stack_connections[side] =
        read_u16(cv5_, offset + 12U + 2U * side);
  }
  output.doodad = group_id >= kMaximumTerrainGroups;
  return true;
}

bool TilesetData::terrain_tile_valid(
    const std::uint16_t map_tile_id) const noexcept {
  const std::size_t group = (map_tile_id >> 4U) & 0x7FFU;
  std::uint16_t megatile{};
  return group < kMaximumTerrainGroups && megatile_id(map_tile_id, megatile);
}

bool TilesetData::terrain_group_id(const std::uint16_t map_tile_id,
                                   std::uint16_t& group_id) const noexcept {
  if (!terrain_tile_valid(map_tile_id)) {
    return false;
  }
  group_id = static_cast<std::uint16_t>(
      (map_tile_id >> 4U) & static_cast<std::uint16_t>(0x7FFU));
  return true;
}

std::size_t TilesetData::terrain_group_members(
    const std::uint16_t group_id,
    std::array<std::uint16_t, 16>& map_tile_ids) const noexcept {
  TerrainGroupVariants variants{};
  if (!terrain_group_variants(group_id, variants)) {
    map_tile_ids.fill(0U);
    return 0U;
  }
  std::size_t count{};
  map_tile_ids.fill(0U);
  for (std::size_t index = 0U; index < variants.common_count; ++index) {
    map_tile_ids[count++] = variants.common[index];
  }
  for (std::size_t index = 0U; index < variants.rare_count; ++index) {
    map_tile_ids[count++] = variants.rare[index];
  }
  return count;
}

bool TilesetData::terrain_group_variants(
    const std::uint16_t group_id,
    TerrainGroupVariants& variants) const noexcept {
  variants = {};
  if (!valid_ || group_id >= kMaximumTerrainGroups ||
      group_id >= group_count()) {
    return false;
  }
  std::size_t member{};
  for (; member < kMegaTilesPerGroup; ++member) {
    const std::uint16_t map_tile_id = static_cast<std::uint16_t>(
        (group_id << 4U) | static_cast<std::uint16_t>(member));
    std::uint16_t megatile{};
    if (!megatile_id(map_tile_id, megatile) || megatile == 0U) {
      break;
    }
    variants.common[variants.common_count++] = map_tile_id;
  }
  if (member < kMegaTilesPerGroup) {
    ++member;  // A zero megatile separates the common and rare sequences.
  }
  for (; member < kMegaTilesPerGroup; ++member) {
    const std::uint16_t map_tile_id = static_cast<std::uint16_t>(
        (group_id << 4U) | static_cast<std::uint16_t>(member));
    std::uint16_t megatile{};
    if (!megatile_id(map_tile_id, megatile) || megatile == 0U) {
      break;
    }
    variants.rare[variants.rare_count++] = map_tile_id;
  }
  if (variants.common_count == 0U && variants.rare_count == 0U) {
    const std::uint16_t first =
        static_cast<std::uint16_t>(group_id << 4U);
    std::uint16_t megatile{};
    if (!megatile_id(first, megatile)) {
      return false;
    }
    variants.common[variants.common_count++] = first;
  }
  return true;
}

bool TilesetData::megatile_id(
    const std::uint16_t map_tile_id,
    std::uint16_t& output) const noexcept {
  if (!valid_) {
    return false;
  }
  const std::size_t group = (map_tile_id >> 4U) & 0x7FFU;
  const std::size_t member = map_tile_id & 0x0FU;
  if (group >= group_count()) {
    return false;
  }
  const std::size_t offset =
      group * kCv5GroupBytes + kCv5MegaTileOffset + 2U * member;
  output = read_u16(cv5_, offset);
  return output < megatile_count();
}

bool TilesetData::render(
    const std::uint16_t map_tile_id,
    IndexedMapTile& output) const noexcept {
  std::uint16_t megatile{};
  if (!megatile_id(map_tile_id, megatile)) {
    return false;
  }
  const std::size_t vx4_offset = static_cast<std::size_t>(megatile) * kVx4MegaTileBytes;
  for (std::size_t mini_y = 0; mini_y < 4; ++mini_y) {
    for (std::size_t mini_x = 0; mini_x < 4; ++mini_x) {
      const std::uint16_t reference =
          read_u16(vx4_, vx4_offset + 2U * (mini_y * 4U + mini_x));
      const std::size_t mini_tile = reference >> 1U;
      if (mini_tile >= minitile_count()) {
        return false;
      }
      const bool flipped = (reference & 1U) != 0;
      const std::size_t vr4_offset = mini_tile * kVr4MiniTileBytes;
      for (std::size_t row = 0; row < 8; ++row) {
        for (std::size_t column = 0; column < 8; ++column) {
          const std::size_t source_column = flipped ? 7U - column : column;
          output.pixels[(mini_y * 8U + row) * IndexedMapTile::width + mini_x * 8U + column] =
              vr4_[vr4_offset + row * 8U + source_column];
        }
      }
    }
  }
  return true;
}

bool TilesetData::terrain_flags(
    const std::uint16_t map_tile_id,
    std::uint16_t& group_flags,
    std::array<std::uint16_t, 16>& minitile_flags) const noexcept {
  std::uint16_t megatile{};
  if (!valid_ || !megatile_id(map_tile_id, megatile)) {
    return false;
  }
  const std::size_t group = (map_tile_id >> 4U) & 0x7FFU;
  group_flags = read_u16(cv5_, group * kCv5GroupBytes + 2U);
  const std::size_t vf4_offset = static_cast<std::size_t>(megatile) * kVf4MegaTileBytes;
  for (std::size_t index = 0; index < minitile_flags.size(); ++index) {
    minitile_flags[index] = read_u16(vf4_, vf4_offset + 2U * index);
  }
  return true;
}

bool TilesetData::buildable(const std::uint16_t map_tile_id) const noexcept {
  // gamemap.cpp::sub_4542D0 at 0x004542D0 places CV5 group flags in the
  // high word of each map collision cell. collide.cpp::sub_419260 at
  // 0x00419260 rejects building placement when cell bit 0x08000000 is set,
  // which is CV5 flag 0x0800 before that shift.
  std::uint16_t group_flags{};
  std::array<std::uint16_t, 16> minitile_flags{};
  return terrain_flags(map_tile_id, group_flags, minitile_flags) &&
         (group_flags & kTerrainBlocksBuilding) == 0;
}

bool TilesetData::creep_edge_frame(
    const std::uint8_t one_based_frame,
    DecodedGrpFrame& output) const noexcept {
  output = {};
  if (!valid_ || one_based_frame == 0U || creep_grp_.empty()) {
    return false;
  }
  // creep.cpp::sub_4D12F1 indexes descriptor (edge * 8 - 2), exactly the
  // standard six-byte GRP header followed by zero-based eight-byte frames.
  return decode_grp_frame(creep_grp_.data(), creep_grp_.size(),
                          static_cast<std::uint16_t>(one_based_frame - 1U),
                          output);
}

bool TilesetData::walkable(
    const std::uint16_t map_tile_id,
    const std::uint8_t minitile_x,
    const std::uint8_t minitile_y) const noexcept {
  if (minitile_x >= 4U || minitile_y >= 4U) {
    return false;
  }
  // gamemap.cpp::sub_4542D0 at 0x004542D0 counts VF4 bit 0 across each
  // megatile to derive the land-passability summary bits. Preserve the
  // original 8x8 minitile evidence here instead of routing on 32px MTXM
  // cells, which is essential around narrow building edges.
  std::uint16_t group_flags{};
  std::array<std::uint16_t, 16> minitile_flags{};
  return terrain_flags(map_tile_id, group_flags, minitile_flags) &&
         (minitile_flags[static_cast<std::size_t>(minitile_y) * 4U + minitile_x] & 1U) != 0;
}

std::uint16_t TilesetData::read_u16(
    const std::vector<std::uint8_t>& bytes,
    const std::size_t offset) noexcept {
  return static_cast<std::uint16_t>(bytes[offset]) |
         static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1]) << 8U);
}

}  // namespace starcraft::gds
