#pragma once

#include "formats/chk_dialect.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace staredit::formats {

struct ChkSection {
  std::uint32_t tag{};
  std::vector<std::uint8_t> payload{};
};

class ChkDocument final {
 public:
  [[nodiscard]] bool parse(const std::uint8_t* bytes,
                           std::size_t size,
                           std::string& error) noexcept;
  [[nodiscard]] bool parse(std::vector<std::uint8_t> bytes,
                           std::string& error) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] bool dirty() const noexcept;
  [[nodiscard]] std::size_t original_size() const noexcept;
  [[nodiscard]] std::size_t section_count() const noexcept;
  [[nodiscard]] std::size_t count(std::uint32_t tag) const noexcept;
  [[nodiscard]] const std::vector<ChkSection>& sections() const noexcept;
  [[nodiscard]] const ChkSection* section(std::uint32_t tag,
                                          std::size_t occurrence = 0U) const noexcept;
  [[nodiscard]] const ChkDialectInfo& dialect() const noexcept;

  [[nodiscard]] bool replace_section(std::uint32_t tag,
                                     std::size_t occurrence,
                                     std::vector<std::uint8_t> payload) noexcept;
  [[nodiscard]] bool append_section(std::uint32_t tag,
                                    std::vector<std::uint8_t> payload) noexcept;
  [[nodiscard]] std::size_t erase_sections(std::uint32_t tag) noexcept;
  [[nodiscard]] bool erase_section(std::uint32_t tag,
                                   std::size_t occurrence) noexcept;

  [[nodiscard]] bool serialize(std::vector<std::uint8_t>& output,
                               std::string& error) const noexcept;
  [[nodiscard]] bool matches_original() const noexcept;

 private:
  void reset() noexcept;
  void update_dialect() noexcept;
  [[nodiscard]] ChkSection* find_section(std::uint32_t tag,
                                         std::size_t occurrence) noexcept;

  std::vector<ChkSection> sections_{};
  std::vector<std::uint8_t> original_bytes_{};
  ChkDialectInfo dialect_{};
  bool valid_{};
  bool dirty_{};
};

}  // namespace staredit::formats
