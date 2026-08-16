#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::lang {

enum class HarvestAdmission : std::uint8_t {
  active,
  queued,
  already_present,
};

// CUnitHarvest.cpp keeps one worker inside a resource source and links the
// remaining workers through CUnit +0xD0/+0xD4. New waiters are inserted at
// the head and the oldest eligible waiter is promoted from the tail.
class UnitHarvestQueue final {
 public:
  [[nodiscard]] HarvestAdmission request(std::uint32_t worker_id);
  [[nodiscard]] std::uint32_t release(std::uint32_t worker_id) noexcept;
  [[nodiscard]] bool is_active(std::uint32_t worker_id) const noexcept;
  [[nodiscard]] bool is_queued(std::uint32_t worker_id) const noexcept;
  [[nodiscard]] std::uint32_t active_worker() const noexcept;
  [[nodiscard]] std::size_t queued_count() const noexcept;
  void clear() noexcept;

 private:
  std::uint32_t active_worker_{};
  std::vector<std::uint32_t> waiting_head_to_tail_{};
};

}  // namespace starcraft::lang
