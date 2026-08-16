#include "../platform/bootstrap_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace starcraft::recovery {

std::filesystem::path executable_directory() {
  std::array<wchar_t, 32768> path{};
  const DWORD length =
      GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
  if (length == 0 || length >= path.size()) {
    return {};
  }
  return std::filesystem::path{path.data(), path.data() + length}.parent_path();
}

std::filesystem::path locate_input_root() {
  try {
    const auto executable = executable_directory();
    std::array<std::filesystem::path, 3> candidates{{
        std::filesystem::current_path(),
        executable,
        executable.parent_path().parent_path().parent_path(),
    }};
    for (const auto &candidate : candidates) {
      if (std::filesystem::is_regular_file(candidate / L"storm.dll") &&
          std::filesystem::is_regular_file(candidate / L"StarDat.mpq")) {
        return candidate;
      }
    }
  } catch (...) {
  }
  return {};
}

bool read_loose_file(const std::filesystem::path &path,
                     std::vector<std::uint8_t> &output) noexcept {
  output.clear();
  const HANDLE file = CreateFileW(
      path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  LARGE_INTEGER size{};
  bool okay = GetFileSizeEx(file, &size) != FALSE && size.QuadPart > 0 &&
              static_cast<unsigned long long>(size.QuadPart) <= SIZE_MAX;
  try {
    if (okay) {
      output.resize(static_cast<std::size_t>(size.QuadPart));
      std::size_t cursor{};
      while (cursor < output.size()) {
        const DWORD request =
            static_cast<DWORD>((std::min)(output.size() - cursor,
                                          static_cast<std::size_t>(1U << 20U)));
        DWORD received{};
        if (!ReadFile(file, output.data() + cursor, request, &received,
                      nullptr) ||
            received == 0U) {
          okay = false;
          break;
        }
        cursor += received;
      }
    }
  } catch (...) {
    okay = false;
  }
  CloseHandle(file);
  if (!okay) {
    output.clear();
  }
  return okay;
}

} // namespace starcraft::recovery
