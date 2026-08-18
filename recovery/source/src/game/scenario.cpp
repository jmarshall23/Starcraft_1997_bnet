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
  fog_mask_.clear();
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

    // StarCraft.exe's maphdr.cpp::sub_46C070 requires MASK to contain one
    // byte per map tile. Each bit is a player's initial shroud state. Keep
    // the fully-masked initialization from mask.cpp::sub_46CB50 when an old
    // beta map omits the optional section.
    const std::size_t tile_count = static_cast<std::size_t>(width) * height;
    std::vector<std::uint8_t> fog_mask(tile_count, 0xFFU);
    data::ChkSection mask{};
    if (chk.section(data::chk_section_fog_mask, 0U, mask)) {
      if (mask.size != tile_count) {
        return false;
      }
      fog_mask.assign(mask.bytes, mask.bytes + mask.size);
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
    fog_mask_ = std::move(fog_mask);
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
const std::vector<std::uint8_t>& MultiplayerScenario::fog_mask() const noexcept {
  return fog_mask_;
}
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

bool MultiplayerScenario::randomize_melee_start_locations(
    std::uint32_t seed) noexcept {
  if (!valid_) {
    return false;
  }

  std::array<std::size_t, 8> active_players{};
  std::array<ScenarioStartLocation, 8> available_starts{};
  std::size_t active_count{};
  std::size_t start_count{};
  for (std::size_t player = 0; player < 8U; ++player) {
    if (players_[player].ownership != 0U) {
      active_players[active_count++] = player;
    }
    if (start_locations_[player].present) {
      available_starts[start_count++] = start_locations_[player];
    }
  }
  if (active_count == 0U || start_count < active_count) {
    return false;
  }

  // net_misc.cpp::sub_4797B0 supplies StarCraft's synchronized 15-bit LCG.
  // Shuffle every authored melee marker, including currently closed slots, so
  // a two-player match on a four-player map can begin at any two locations.
  for (std::size_t remaining = start_count; remaining > 1U; --remaining) {
    seed = 22695477U * seed + 1U;
    const std::uint32_t random = (seed >> 16U) & 0x7FFFU;
    const std::size_t other = random % remaining;
    std::swap(available_starts[remaining - 1U], available_starts[other]);
  }
  for (std::size_t index = 0; index < active_count; ++index) {
    start_locations_[active_players[index]] = available_starts[index];
  }
  return true;
}

}  // namespace starcraft::game
