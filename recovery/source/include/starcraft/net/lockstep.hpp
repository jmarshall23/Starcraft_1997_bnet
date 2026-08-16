#pragma once

#include "starcraft/data/chk.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::net {

constexpr std::uint8_t empty_turn_command = 5;
constexpr std::uint8_t synchronization_command = 59;

[[nodiscard]] std::uint32_t synchronization_checksum(
    const std::uint32_t* words,
    std::size_t word_count,
    std::uint32_t excluded_bits = 0) noexcept;

class TurnCommandBuffer final {
 public:
  explicit TurnCommandBuffer(std::size_t capacity = 512) noexcept;

  [[nodiscard]] bool append(const void* bytes, std::size_t size) noexcept;
  [[nodiscard]] bool append_synchronization(
      std::uint8_t sequence,
      std::uint8_t category,
      std::uint32_t checksum) noexcept;
  [[nodiscard]] bool seal() noexcept;
  void clear() noexcept;

  [[nodiscard]] bool sealed() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;

 private:
  std::size_t capacity_{};
  std::vector<std::uint8_t> bytes_{};
  bool sealed_{};
};

struct PlayerTurn {
  std::uint8_t player{};
  std::vector<std::uint8_t> commands{};
};

class LockstepSession final {
 public:
  [[nodiscard]] bool configure(
      const std::array<bool, data::chk_player_slot_count>& active_players) noexcept;
  [[nodiscard]] bool submit(
      std::uint8_t player,
      std::uint32_t turn,
      const std::vector<std::uint8_t>& commands) noexcept;
  [[nodiscard]] bool ready() const noexcept;
  [[nodiscard]] bool consume(std::vector<PlayerTurn>& turns) noexcept;

  [[nodiscard]] std::uint32_t current_turn() const noexcept;
  [[nodiscard]] std::size_t active_player_count() const noexcept;

 private:
  std::array<bool, data::chk_player_slot_count> active_players_{};
  std::array<bool, data::chk_player_slot_count> submitted_{};
  std::array<std::vector<std::uint8_t>, data::chk_player_slot_count> commands_{};
  std::uint32_t current_turn_{};
  std::size_t active_player_count_{};
  bool configured_{};
};

}  // namespace starcraft::net
