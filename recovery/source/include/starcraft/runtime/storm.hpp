#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace starcraft::runtime {

struct DecodedPcx {
  std::uint32_t width{};
  std::uint32_t height{};
  std::vector<std::uint8_t> pixels{};
  std::vector<std::uint8_t> palette{};
};

// Transitional typed access to the matching ordinal-only Storm DLL. As each
// prototype stabilizes this can be replaced by a generated import library.
class StormModule final {
 public:
  explicit StormModule(const std::filesystem::path& path) noexcept;
  ~StormModule();

  StormModule(const StormModule&) = delete;
  StormModule& operator=(const StormModule&) = delete;

  [[nodiscard]] bool loaded() const noexcept;

  [[nodiscard]] void* allocate(
      std::size_t bytes,
      const char* source_file,
      std::uint32_t source_line,
      std::uint32_t flags = 0) const noexcept;

  [[nodiscard]] bool free(
      void* allocation,
      const char* source_file,
      std::uint32_t source_line,
      std::uint32_t flags = 0) const noexcept;

  [[nodiscard]] bool open_archive(
      const std::filesystem::path& path,
      void** archive,
      std::uint32_t priority = 0,
      std::uint32_t flags = 0) const noexcept;

  [[nodiscard]] bool close_archive(void* archive) const noexcept;

  [[nodiscard]] bool open_file(
      void* archive,
      const char* archived_path,
      void** file,
      std::uint32_t search_scope = 0) const noexcept;

  [[nodiscard]] bool open_file_global(
      const char* archived_path,
      void** file) const noexcept;

  [[nodiscard]] std::uint32_t file_size(void* file) const noexcept;

  [[nodiscard]] bool read_file(
      void* file,
      void* destination,
      std::uint32_t bytes_to_read,
      std::uint32_t* bytes_read) const noexcept;

  [[nodiscard]] bool close_file(void* file) const noexcept;

  [[nodiscard]] bool load_file(
      const char* archived_path,
      std::vector<std::uint8_t>& bytes) const noexcept;

  [[nodiscard]] bool load_file_from_archive(
      void* archive,
      const char* archived_path,
      std::vector<std::uint8_t>& bytes,
      std::uint32_t search_scope = 0) const noexcept;

  [[nodiscard]] bool load_pcx(
      const char* archived_path,
      DecodedPcx& image) const noexcept;

 private:
  [[nodiscard]] void* resolve(std::uint16_t ordinal) const noexcept;

  void* module_{};
};

}  // namespace starcraft::runtime
