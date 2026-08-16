#include "starcraft/net/lockstep.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace starcraft::net {

std::uint32_t synchronization_checksum(
    const std::uint32_t* const words,
    const std::size_t word_count,
    const std::uint32_t excluded_bits) noexcept {
  if (words == nullptr && word_count != 0) {
    return 0;
  }
  std::uint32_t checksum{};
  for (std::size_t index = 0; index < word_count; ++index) {
    const std::uint32_t value = (words[index] & ~excluded_bits) ^ checksum;
    checksum = (value << 3U) | (value >> 29U);
  }
  return checksum;
}

TurnCommandBuffer::TurnCommandBuffer(const std::size_t capacity) noexcept
    : capacity_(capacity) {
  try {
    bytes_.reserve(capacity);
  } catch (...) {
    capacity_ = 0;
  }
}

bool TurnCommandBuffer::append(const void* const bytes, const std::size_t size) noexcept {
  if (sealed_ || (bytes == nullptr && size != 0) || size > capacity_ - bytes_.size()) {
    return false;
  }
  try {
    const auto* const source = static_cast<const std::uint8_t*>(bytes);
    bytes_.insert(bytes_.end(), source, source + size);
    return true;
  } catch (...) {
    return false;
  }
}

bool TurnCommandBuffer::append_synchronization(
    const std::uint8_t sequence,
    const std::uint8_t category,
    const std::uint32_t checksum) noexcept {
  const std::uint8_t command[7]{
      synchronization_command,
      sequence,
      category,
      static_cast<std::uint8_t>(checksum),
      static_cast<std::uint8_t>(checksum >> 8U),
      static_cast<std::uint8_t>(checksum >> 16U),
      static_cast<std::uint8_t>(checksum >> 24U),
  };
  return append(command, sizeof(command));
}

bool TurnCommandBuffer::seal() noexcept {
  if (sealed_) {
    return false;
  }
  if (bytes_.empty() && !append(&empty_turn_command, 1)) {
    return false;
  }
  sealed_ = true;
  return true;
}

void TurnCommandBuffer::clear() noexcept {
  bytes_.clear();
  sealed_ = false;
}

bool TurnCommandBuffer::sealed() const noexcept { return sealed_; }
const std::vector<std::uint8_t>& TurnCommandBuffer::bytes() const noexcept { return bytes_; }

bool LockstepSession::configure(
    const std::array<bool, data::chk_player_slot_count>& active_players) noexcept {
  active_players_ = active_players;
  submitted_ = {};
  for (auto& commands : commands_) {
    commands.clear();
  }
  current_turn_ = 0;
  active_player_count_ = 0;
  for (const bool active : active_players_) {
    active_player_count_ += active ? 1U : 0U;
  }
  configured_ = active_player_count_ >= 2;
  return configured_;
}

bool LockstepSession::submit(
    const std::uint8_t player,
    const std::uint32_t turn,
    const std::vector<std::uint8_t>& commands) noexcept {
  if (!configured_ || player >= active_players_.size() || !active_players_[player] ||
      submitted_[player] || turn != current_turn_ || commands.empty()) {
    return false;
  }
  try {
    commands_[player] = commands;
    submitted_[player] = true;
    return true;
  } catch (...) {
    return false;
  }
}

bool LockstepSession::ready() const noexcept {
  if (!configured_) {
    return false;
  }
  for (std::size_t player = 0; player < active_players_.size(); ++player) {
    if (active_players_[player] && !submitted_[player]) {
      return false;
    }
  }
  return true;
}

bool LockstepSession::consume(std::vector<PlayerTurn>& turns) noexcept {
  if (!ready()) {
    return false;
  }
  try {
    std::vector<PlayerTurn> completed;
    completed.reserve(active_player_count_);
    for (std::size_t player = 0; player < active_players_.size(); ++player) {
      if (active_players_[player]) {
        completed.push_back(
            {static_cast<std::uint8_t>(player), std::move(commands_[player])});
        submitted_[player] = false;
      }
    }
    turns = std::move(completed);
    ++current_turn_;
    return true;
  } catch (...) {
    return false;
  }
}

std::uint32_t LockstepSession::current_turn() const noexcept { return current_turn_; }
std::size_t LockstepSession::active_player_count() const noexcept { return active_player_count_; }

}  // namespace starcraft::net
