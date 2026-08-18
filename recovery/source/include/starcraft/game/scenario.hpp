#pragma once

#include "starcraft/data/chk.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::game {

struct ScenarioPlayer {
  std::uint8_t ownership{};
  std::uint8_t race{};
};

struct ScenarioUnit {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t unit_type{};
  std::uint8_t owner{};
};

struct ScenarioStartLocation {
  std::uint16_t x{};
  std::uint16_t y{};
  bool present{};
};

class MultiplayerScenario final {
 public:
  [[nodiscard]] bool load(const data::ChkView& chk) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::uint16_t width() const noexcept;
  [[nodiscard]] std::uint16_t height() const noexcept;
  [[nodiscard]] std::uint16_t tileset_id() const noexcept;
  [[nodiscard]] std::size_t active_player_count() const noexcept;
  [[nodiscard]] const std::array<ScenarioPlayer, data::chk_player_slot_count>& players()
      const noexcept;
  [[nodiscard]] const std::vector<std::uint16_t>& tiles() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& fog_mask() const noexcept;
  [[nodiscard]] const std::vector<ScenarioUnit>& units() const noexcept;
  [[nodiscard]] const std::array<ScenarioStartLocation, data::chk_player_slot_count>&
  start_locations() const noexcept;
  [[nodiscard]] bool tile(
      std::uint16_t x,
      std::uint16_t y,
      std::uint16_t& tile_id) const noexcept;
  [[nodiscard]] bool configure_player(std::size_t player,
                                      std::uint8_t ownership,
                                      std::uint8_t race) noexcept;
  [[nodiscard]] bool randomize_melee_start_locations(
      std::uint32_t seed) noexcept;

 private:
  std::uint16_t width_{};
  std::uint16_t height_{};
  std::uint16_t tileset_id_{};
  std::array<ScenarioPlayer, data::chk_player_slot_count> players_{};
  std::vector<std::uint16_t> tiles_{};
  std::vector<std::uint8_t> fog_mask_{};
  std::vector<ScenarioUnit> units_{};
  std::array<ScenarioStartLocation, data::chk_player_slot_count> start_locations_{};
  std::size_t active_player_count_{};
  bool valid_{};
};

}  // namespace starcraft::game
