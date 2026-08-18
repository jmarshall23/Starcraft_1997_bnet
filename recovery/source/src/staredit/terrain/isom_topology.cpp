#include "terrain/isom_topology.hpp"

#include <algorithm>
#include <array>
#include <utility>

namespace staredit::terrain {
namespace {

constexpr std::size_t kMaximumTerrainGroups = 1024U;
constexpr std::uint16_t kHardLinkThreshold = 48U;

}  // namespace

bool IsomTopology::build(
    const starcraft::gds::TilesetData& tileset) noexcept {
  group_pairs_.clear();
  candidates_.clear();
  terrain_type_count_ = 0U;
  valid_ = false;
  if (!tileset.valid()) {
    return false;
  }
  try {
    const std::size_t groups =
        (std::min)(tileset.group_count(), kMaximumTerrainGroups);
    group_pairs_.reserve(groups / 2U);
    std::array<bool, 65536> terrain_types{};
    for (std::size_t left_id = 0U; left_id + 1U < groups; left_id += 2U) {
      starcraft::gds::Cv5TileGroup left{};
      starcraft::gds::Cv5TileGroup right{};
      if (!tileset.tile_group(left_id, left) ||
          !tileset.tile_group(left_id + 1U, right)) {
        group_pairs_.clear();
        candidates_.clear();
        return false;
      }
      const std::uint32_t hash = group_pair_hash(left);
      const auto left_group = static_cast<std::uint16_t>(left_id);
      group_pairs_.push_back(
          {left_group, static_cast<std::uint16_t>(left_id + 1U),
           left.terrain_type, left.directional_links,
           left.stack_connections, hash});
      candidates_[hash].push_back(left_group);
      // Older/beta CV5 archives use different numeric terrain-type IDs for
      // several otherwise identical transition groups.  ISOM edge links are
      // stable across those layouts, so retain an edge-only lookup alongside
      // the exact retail-style hash.  Exact matches remain preferred.
      const std::uint32_t edge_hash = hash & ~std::uint32_t{0x3FU};
      if (edge_hash != hash) {
        candidates_[edge_hash].push_back(left_group);
      }
      if (left.terrain_type != 0U && !terrain_types[left.terrain_type]) {
        terrain_types[left.terrain_type] = true;
        ++terrain_type_count_;
      }
    }
    valid_ = !group_pairs_.empty();
    return valid_;
  } catch (...) {
    group_pairs_.clear();
    candidates_.clear();
    terrain_type_count_ = 0U;
    return false;
  }
}

bool IsomTopology::valid() const noexcept { return valid_; }
std::size_t IsomTopology::group_pair_count() const noexcept {
  return valid_ ? group_pairs_.size() : 0U;
}
std::size_t IsomTopology::terrain_type_count() const noexcept {
  return valid_ ? terrain_type_count_ : 0U;
}
const std::vector<IsomGroupPair>& IsomTopology::group_pairs() const noexcept {
  return group_pairs_;
}

const std::vector<std::uint16_t>* IsomTopology::candidates(
    const std::uint32_t hash) const noexcept {
  const auto found = candidates_.find(hash);
  if (valid_ && found != candidates_.end()) {
    return &found->second;
  }
  const auto edge_only = candidates_.find(hash & ~std::uint32_t{0x3FU});
  return valid_ && edge_only != candidates_.end() ? &edge_only->second
                                                  : nullptr;
}

std::uint32_t IsomTopology::group_pair_hash(
    const starcraft::gds::Cv5TileGroup& left_group) noexcept {
  std::uint32_t hash{};
  bool has_hard_link{};
  for (const std::uint16_t link : left_group.directional_links) {
    hash = (hash | link) << 6U;
    has_hard_link = has_hard_link || link >= kHardLinkThreshold;
  }
  return has_hard_link ? hash | left_group.terrain_type : hash;
}

}  // namespace staredit::terrain
