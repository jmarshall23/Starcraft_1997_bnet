#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace staredit::formats {

// Writes a classic version-1 StarEdit MPQ containing one uncompressed file.
// That is the complete container needed by SCM/SCX scenarios
// (staredit\scenario.chk).
[[nodiscard]] bool write_single_file_mpq(
    const std::filesystem::path& path,
    std::string_view archived_path,
    const std::vector<std::uint8_t>& bytes,
    std::string& error) noexcept;

}  // namespace staredit::formats
