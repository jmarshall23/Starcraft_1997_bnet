#include "starcraft/lang/cunit_build.hpp"

#include <algorithm>

namespace starcraft::lang {
namespace {

// These are the action-0x0047E920 records in the executable command-card
// table rooted at 0x00500188. The fields are producer/card, button position,
// icon, and action argument (the produced CUnit type).
constexpr std::array<UnitProductionButton, 1> kCommandCenter{{{106, 1, 7, 7}}};
constexpr std::array<UnitProductionButton, 1> kNuclearSilo{{{108, 1, 14, 14}}};
constexpr std::array<UnitProductionButton, 3> kBarracks{{
    {111, 1, 0, 0}, {111, 2, 32, 32}, {111, 3, 1, 1},
}};
constexpr std::array<UnitProductionButton, 3> kFactory{{
    {113, 1, 2, 2}, {113, 2, 5, 5}, {113, 3, 3, 3},
}};
constexpr std::array<UnitProductionButton, 4> kStarport{{
    {114, 1, 8, 8}, {114, 2, 11, 11}, {114, 3, 9, 9}, {114, 4, 12, 12},
}};
constexpr std::array<UnitProductionButton, 1> kInfestedCommandCenter{{{130, 1, 50, 50}}};
constexpr std::array<UnitProductionButton, 1> kNexus{{{154, 1, 64, 64}}};
constexpr std::array<UnitProductionButton, 3> kRoboticsFacility{{
    {155, 1, 83, 83}, {155, 2, 69, 69}, {155, 3, 84, 84},
}};
constexpr std::array<UnitProductionButton, 3> kGateway{{
    {160, 1, 65, 65}, {160, 2, 66, 66}, {160, 3, 67, 67},
}};
constexpr std::array<UnitProductionButton, 3> kStargate{{
    {167, 1, 70, 70}, {167, 2, 72, 72}, {167, 3, 71, 71},
}};

// CUnitCarrier.cpp::sub_424010 uses the ordinary five-entry production queue
// but attaches completed type-73 Interceptors/type-85 Scarabs to CUnit+192
// instead of placing them on the map.
constexpr std::array<UnitProductionButton, 1> kCarrier{{
    {72, 7, 73, 73, UnitProductionKind::carrier_hangar},
}};
constexpr std::array<UnitProductionButton, 1> kGantrithor{{
    {82, 7, 73, 73, UnitProductionKind::carrier_hangar},
}};
constexpr std::array<UnitProductionButton, 1> kReaver{{
    {83, 7, 85, 85, UnitProductionKind::carrier_hangar},
}};

// Larva card 35 uses action 0x0047F690, recovered in statbtn.cpp, and morphs
// through CUnitZBuild.cpp::sub_447820 instead of a building production queue.
constexpr std::array<UnitProductionButton, 9> kLarva{{
    {35, 1, 41, 41, UnitProductionKind::zerg_larva_morph},
    {35, 2, 37, 37, UnitProductionKind::zerg_larva_morph},
    {35, 3, 42, 42, UnitProductionKind::zerg_larva_morph},
    {35, 4, 38, 38, UnitProductionKind::zerg_larva_morph},
    {35, 5, 39, 39, UnitProductionKind::zerg_larva_morph},
    {35, 6, 43, 43, UnitProductionKind::zerg_larva_morph},
    {35, 7, 45, 45, UnitProductionKind::zerg_larva_morph},
    {35, 8, 46, 46, UnitProductionKind::zerg_larva_morph},
    {35, 9, 47, 47, UnitProductionKind::zerg_larva_morph},
}};

constexpr std::array<std::uint16_t, 14> kProductionProducerTypes{{
    35, 72, 82, 83, 106, 108, 111, 113, 114, 130, 154, 155, 160, 167,
}};

template <std::size_t Size>
constexpr UnitProductionButtonView view(
    const std::array<UnitProductionButton, Size>& buttons) noexcept {
  return {buttons.data(), buttons.size()};
}

}  // namespace

UnitProductionQueue::UnitProductionQueue() noexcept { clear(); }

bool UnitProductionQueue::enqueue(const std::uint16_t unit_type) noexcept {
  for (std::size_t offset = 0; offset < types_.size(); ++offset) {
    const std::size_t index = (head_ + offset) % types_.size();
    if (types_[index] == cunit_production_empty) {
      types_[index] = unit_type;
      return true;
    }
  }
  return false;
}

std::uint16_t UnitProductionQueue::remove(const std::size_t relative_index) noexcept {
  if (relative_index >= types_.size()) {
    return cunit_production_empty;
  }
  const std::size_t index = (head_ + relative_index) % types_.size();
  const std::uint16_t removed = types_[index];
  if (removed == cunit_production_empty) {
    return removed;
  }
  for (std::size_t offset = relative_index; offset + 1U < types_.size(); ++offset) {
    types_[(head_ + offset) % types_.size()] =
        types_[(head_ + offset + 1U) % types_.size()];
  }
  types_[(head_ + types_.size() - 1U) % types_.size()] = cunit_production_empty;
  return removed;
}

std::uint16_t UnitProductionQueue::front() const noexcept { return at(0); }

std::uint16_t UnitProductionQueue::at(const std::size_t relative_index) const noexcept {
  return relative_index < types_.size()
             ? types_[(head_ + relative_index) % types_.size()]
             : cunit_production_empty;
}

std::size_t UnitProductionQueue::count() const noexcept {
  return static_cast<std::size_t>(std::count_if(
      types_.begin(), types_.end(), [](const std::uint16_t type) {
        return type != cunit_production_empty;
      }));
}

bool UnitProductionQueue::empty() const noexcept { return count() == 0U; }
bool UnitProductionQueue::full() const noexcept { return count() == types_.size(); }

void UnitProductionQueue::advance() noexcept {
  if (types_[head_] == cunit_production_empty) {
    return;
  }
  types_[head_] = cunit_production_empty;
  head_ = static_cast<std::uint8_t>((head_ + 1U) % types_.size());
}

void UnitProductionQueue::clear() noexcept {
  types_.fill(cunit_production_empty);
  head_ = 0;
}

UnitProductionButtonView production_buttons_for(
    const std::uint16_t producer_type) noexcept {
  switch (producer_type) {
    case 35: return view(kLarva);
    case 72: return view(kCarrier);
    case 82: return view(kGantrithor);
    case 83: return view(kReaver);
    case 106: return view(kCommandCenter);
    case 108: return view(kNuclearSilo);
    case 111: return view(kBarracks);
    case 113: return view(kFactory);
    case 114: return view(kStarport);
    case 130: return view(kInfestedCommandCenter);
    case 154: return view(kNexus);
    case 155: return view(kRoboticsFacility);
    case 160: return view(kGateway);
    case 167: return view(kStargate);
    default: return {};
  }
}

UnitProductionProducerView production_producer_types() noexcept {
  return {kProductionProducerTypes.data(), kProductionProducerTypes.size()};
}

bool producer_builds_unit(
    const std::uint16_t producer_type,
    const std::uint16_t product_type) noexcept {
  const UnitProductionButtonView buttons = production_buttons_for(producer_type);
  for (std::size_t index = 0; index < buttons.count; ++index) {
    if (buttons.buttons[index].product_type == product_type) {
      return true;
    }
  }
  return false;
}

std::uint16_t production_total_ticks(const std::uint16_t dat_build_time) noexcept {
  return static_cast<std::uint16_t>(dat_build_time >> 1U);
}

std::uint8_t production_percent(
    const std::uint16_t dat_build_time,
    const std::uint16_t remaining_ticks) noexcept {
  const std::uint16_t total = production_total_ticks(dat_build_time);
  if (total == 0U) {
    return 0U;
  }
  const std::uint16_t bounded = (std::min)(remaining_ticks, total);
  return static_cast<std::uint8_t>(100U * (total - bounded) / total);
}

}  // namespace starcraft::lang
