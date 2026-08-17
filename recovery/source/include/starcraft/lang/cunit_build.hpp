#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

inline constexpr std::uint16_t cunit_production_empty = 228U;
inline constexpr std::size_t cunit_production_capacity = 5U;

class UnitProductionQueue final {
 public:
  UnitProductionQueue() noexcept;

  [[nodiscard]] bool enqueue(std::uint16_t unit_type) noexcept;  // VA 0x00421B30
  [[nodiscard]] std::uint16_t remove(std::size_t relative_index) noexcept;
  [[nodiscard]] std::uint16_t front() const noexcept;
  [[nodiscard]] std::uint16_t at(std::size_t relative_index) const noexcept;
  [[nodiscard]] std::size_t count() const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] bool full() const noexcept;
  void advance() noexcept;
  void clear() noexcept;

 private:
  std::array<std::uint16_t, cunit_production_capacity> types_{};
  std::uint8_t head_{};
};

enum class UnitProductionKind : std::uint8_t {
  train,
  zerg_larva_morph,
  carrier_hangar,
};

struct UnitProductionButton {
  std::uint16_t producer_type{};
  std::uint16_t position{};
  std::uint16_t icon{};
  std::uint16_t product_type{};
  UnitProductionKind kind{UnitProductionKind::train};
};

struct UnitProductionButtonView {
  const UnitProductionButton* buttons{};
  std::size_t count{};
};

struct UnitProductionProducerView {
  const std::uint16_t* unit_types{};
  std::size_t count{};
};

[[nodiscard]] UnitProductionButtonView production_buttons_for(
    std::uint16_t producer_type) noexcept;
[[nodiscard]] UnitProductionProducerView production_producer_types() noexcept;
[[nodiscard]] bool producer_builds_unit(
    std::uint16_t producer_type,
    std::uint16_t product_type) noexcept;
[[nodiscard]] std::uint16_t production_total_ticks(
    std::uint16_t dat_build_time) noexcept;  // VA 0x00423670
[[nodiscard]] std::uint8_t production_percent(
    std::uint16_t dat_build_time,
    std::uint16_t remaining_ticks) noexcept;

}  // namespace starcraft::lang
