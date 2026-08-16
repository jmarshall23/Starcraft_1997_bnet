#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

std::uint16_t read_u16(const std::vector<std::uint8_t> &bytes,
                       const std::size_t offset) noexcept {
  return offset + 2U <= bytes.size()
             ? static_cast<std::uint16_t>(bytes[offset]) |
                   static_cast<std::uint16_t>(
                       static_cast<std::uint16_t>(bytes[offset + 1U]) << 8U)
             : 0;
}

std::uint32_t read_u32(const std::vector<std::uint8_t> &bytes,
                       const std::size_t offset) noexcept {
  return offset + 4U <= bytes.size()
             ? static_cast<std::uint32_t>(bytes[offset]) |
                   (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U) |
                   (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U) |
                   (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U)
             : 0;
}

} // namespace starcraft::recovery
