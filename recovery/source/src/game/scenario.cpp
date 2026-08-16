#include "starcraft/game/scenario.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace starcraft::game {

bool MultiplayerScenario::load(const data::ChkView& chk) noexcept {
  valid_ = false;
  width_ = 0;
  height_ = 0;
  tileset_id_ = 0;
  players_ = {};
  tiles_.clear();
  units_.clear();
  start_locations_ = {};
  active_player_count_ = 0;

  std::uint16_t width{};
  std::uint16_t height{};
  std::uint16_t tileset{};
  if (!chk.valid() || !chk.dimensions(width, height) || !chk.tileset(tileset) ||
      width > std::numeric_limits<std::size_t>::max() / height) {
    return false;
  }

  try {
    std::array<ScenarioPlayer, data::chk_player_slot_count> players{};
    std::size_t active_players{};
    for (std::size_t player = 0; player < players.size(); ++player) {
      if (!chk.player_ownership(player, players[player].ownership) ||
          !chk.player_race(player, players[player].race)) {
        return false;
      }
      active_players += players[player].ownership != 0 ? 1U : 0U;
    }
    if (active_players < 2) {
      return false;
    }

    std::vector<std::uint16_t> tiles(static_cast<std::size_t>(width) * height);
    for (std::uint16_t y = 0; y < height; ++y) {
      for (std::uint16_t x = 0; x < width; ++x) {
        if (!chk.tile(x, y, tiles[static_cast<std::size_t>(y) * width + x])) {
          return false;
        }
      }
    }

    std::vector<ScenarioUnit> units;
    std::array<ScenarioStartLocation, data::chk_player_slot_count> start_locations{};
    units.reserve(chk.unit_count());
    for (std::size_t index = 0; index < chk.unit_count(); ++index) {
      data::BetaUnitPlacement placement{};
      if (!chk.unit(index, placement) || placement.owner >= players.size() ||
          (placement.owner != 11 && players[placement.owner].ownership == 0) ||
          placement.x >= width * 32U ||
          placement.y >= height * 32U) {
        return false;
      }
      // lang\maphdr.cpp::sub_46BC40 at 0x0046BC40 treats type 214 as a
      // start-location marker (lines 119-130), not as a runtime CUnit.
      if (placement.unit_type == 214) {
        start_locations[placement.owner] = {placement.x, placement.y, true};
        continue;
      }
      // The same recovered loop explicitly admits owner 11 before calling
      // CUnitInit.cpp::sub_42DEF0. Neutral resource/doodad units therefore do
      // not require an active OWNR[11] slot.
      units.push_back({placement.x, placement.y, placement.unit_type, placement.owner});
    }

    width_ = width;
    height_ = height;
    tileset_id_ = tileset;
    players_ = players;
    tiles_ = std::move(tiles);
    units_ = std::move(units);
    start_locations_ = start_locations;
    active_player_count_ = active_players;
    valid_ = true;
    return true;
  } catch (...) {
    return false;
  }
}

bool MultiplayerScenario::valid() const noexcept { return valid_; }
std::uint16_t MultiplayerScenario::width() const noexcept { return width_; }
std::uint16_t MultiplayerScenario::height() const noexcept { return height_; }
std::uint16_t MultiplayerScenario::tileset_id() const noexcept { return tileset_id_; }
std::size_t MultiplayerScenario::active_player_count() const noexcept {
  return active_player_count_;
}
const std::array<ScenarioPlayer, data::chk_player_slot_count>& MultiplayerScenario::players()
    const noexcept {
  return players_;
}
const std::vector<std::uint16_t>& MultiplayerScenario::tiles() const noexcept { return tiles_; }
const std::vector<ScenarioUnit>& MultiplayerScenario::units() const noexcept { return units_; }
const std::array<ScenarioStartLocation, data::chk_player_slot_count>&
MultiplayerScenario::start_locations() const noexcept {
  return start_locations_;
}

bool MultiplayerScenario::tile(
    const std::uint16_t x,
    const std::uint16_t y,
    std::uint16_t& tile_id) const noexcept {
  if (!valid_ || x >= width_ || y >= height_) {
    return false;
  }
  tile_id = tiles_[static_cast<std::size_t>(y) * width_ + x];
  return true;
}

bool MultiplayerScenario::configure_player(const std::size_t player,
                                            const std::uint8_t ownership,
                                            const std::uint8_t race) noexcept {
  if (!valid_ || player >= players_.size() || (ownership != 0U && race >= 3U)) {
    return false;
  }
  if (players_[player].ownership != 0U && ownership == 0U) {
    --active_player_count_;
  } else if (players_[player].ownership == 0U && ownership != 0U) {
    ++active_player_count_;
  }
  players_[player].ownership = ownership;
  players_[player].race = race;
  return true;
}

}  // namespace starcraft::game
