#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace staredit::formats {

struct IsomLink {
  std::uint16_t raw{};

  [[nodiscard]] constexpr std::uint16_t terrain_type() const noexcept {
    return static_cast<std::uint16_t>((raw >> 4U) & 0x07FFU);
  }
  [[nodiscard]] constexpr std::uint8_t edge_flags() const noexcept {
    return static_cast<std::uint8_t>(raw & 0x000EU);
  }
  [[nodiscard]] constexpr bool edited() const noexcept {
    return (raw & 0x0001U) != 0U;
  }
  [[nodiscard]] constexpr bool skipped() const noexcept {
    return (raw & 0x8000U) != 0U;
  }
};

struct IsomEntry {
  std::array<IsomLink, 4> links{};
};

class IsomSection final {
 public:
  [[nodiscard]] bool initialize(std::uint16_t map_width,
                                std::uint16_t map_height) noexcept;
  [[nodiscard]] bool parse(const std::uint8_t* payload,
                           std::size_t payload_size,
                           std::uint16_t map_width,
                           std::uint16_t map_height,
                           std::string& error) noexcept;
  [[nodiscard]] bool serialize(std::vector<std::uint8_t>& payload) const noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::size_t column_count() const noexcept;
  [[nodiscard]] std::size_t row_count() const noexcept;
  [[nodiscard]] std::size_t entry_count() const noexcept;
  [[nodiscard]] const IsomEntry* entry(std::size_t column,
                                       std::size_t row) const noexcept;
  [[nodiscard]] IsomEntry* mutable_entry(std::size_t column,
                                         std::size_t row) noexcept;

 private:
  void reset() noexcept;

  std::vector<IsomEntry> entries_{};
  std::size_t columns_{};
  std::size_t rows_{};
  bool valid_{};
};

}  // namespace staredit::formats
