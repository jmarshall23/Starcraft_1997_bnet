#pragma once

#include "starcraft/gds/tileset.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace staredit::terrain {

struct IsomGroupPair {
  std::uint16_t left_group{};
  std::uint16_t right_group{};
  std::uint16_t terrain_type{};
  std::array<std::uint16_t, 4> directional_links{};
  std::array<std::uint16_t, 4> stack_connections{};
  std::uint32_t hash{};
};

class IsomTopology final {
 public:
  [[nodiscard]] bool build(
      const starcraft::gds::TilesetData& tileset) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::size_t group_pair_count() const noexcept;
  [[nodiscard]] std::size_t terrain_type_count() const noexcept;
  [[nodiscard]] const std::vector<IsomGroupPair>& group_pairs() const noexcept;
  [[nodiscard]] const std::vector<std::uint16_t>* candidates(
      std::uint32_t hash) const noexcept;

  [[nodiscard]] static std::uint32_t group_pair_hash(
      const starcraft::gds::Cv5TileGroup& left_group) noexcept;

 private:
  std::vector<IsomGroupPair> group_pairs_{};
  std::unordered_map<std::uint32_t, std::vector<std::uint16_t>> candidates_{};
  std::size_t terrain_type_count_{};
  bool valid_{};
};

}  // namespace staredit::terrain
