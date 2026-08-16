#include "starcraft/lang/cunit_harvest.hpp"

#include <algorithm>

namespace starcraft::lang {

HarvestAdmission UnitHarvestQueue::request(const std::uint32_t worker_id) {
  if (worker_id == 0U || active_worker_ == worker_id || is_queued(worker_id)) {
    return HarvestAdmission::already_present;
  }
  // sub_42C520 reserves the resource's single active slot. sub_42C210 puts
  // additional workers at the head of its intrusive waiting list.
  if (active_worker_ == 0U) {
    active_worker_ = worker_id;
    return HarvestAdmission::active;
  }
  waiting_head_to_tail_.insert(waiting_head_to_tail_.begin(), worker_id);
  return HarvestAdmission::queued;
}

std::uint32_t UnitHarvestQueue::release(const std::uint32_t worker_id) noexcept {
  if (worker_id == 0U) {
    return active_worker_;
  }
  if (active_worker_ == worker_id) {
    active_worker_ = 0U;
    // sub_42C190 walks the head-linked list and retains the last eligible
    // worker; sub_42C610 promotes that tail worker when harvesting ends.
    if (!waiting_head_to_tail_.empty()) {
      active_worker_ = waiting_head_to_tail_.back();
      waiting_head_to_tail_.pop_back();
    }
    return active_worker_;
  }
  const auto queued = std::find(
      waiting_head_to_tail_.begin(), waiting_head_to_tail_.end(), worker_id);
  if (queued != waiting_head_to_tail_.end()) {
    waiting_head_to_tail_.erase(queued);
  }
  return active_worker_;
}

bool UnitHarvestQueue::is_active(const std::uint32_t worker_id) const noexcept {
  return worker_id != 0U && active_worker_ == worker_id;
}

bool UnitHarvestQueue::is_queued(const std::uint32_t worker_id) const noexcept {
  return std::find(
             waiting_head_to_tail_.begin(), waiting_head_to_tail_.end(), worker_id) !=
         waiting_head_to_tail_.end();
}

std::uint32_t UnitHarvestQueue::active_worker() const noexcept {
  return active_worker_;
}

std::size_t UnitHarvestQueue::queued_count() const noexcept {
  return waiting_head_to_tail_.size();
}

void UnitHarvestQueue::clear() noexcept {
  active_worker_ = 0U;
  waiting_head_to_tail_.clear();
}

}  // namespace starcraft::lang
