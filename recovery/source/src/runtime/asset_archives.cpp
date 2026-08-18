#include "starcraft/runtime/asset_archives.hpp"

#include "starcraft/runtime/storm.hpp"

#include <array>
#include <filesystem>

namespace starcraft::runtime {
namespace {

constexpr std::array<const wchar_t*, 2> kBaseNames{{
    L"StarDat.mpq", L"StarCraft.mpq"}};
constexpr std::array<const wchar_t*, 2> kExpansionNames{{
    L"BrooDat.mpq", L"BroodWar.mpq"}};

bool regular_file(const std::filesystem::path& path) noexcept {
  std::error_code error{};
  return std::filesystem::is_regular_file(path, error);
}

}  // namespace

bool AssetArchives::open(StormModule& storm,
                         const std::filesystem::path& root) noexcept {
  (void)close(storm);
  if (!storm.loaded()) {
    return false;
  }
  try {
    for (const wchar_t* const name : kBaseNames) {
      if (regular_file(root / name) &&
          storm.open_archive(root / name, &handles_[count_], 1000U)) {
        ++count_;
        break;
      }
    }
    if (count_ == 0U) {
      return false;
    }
    for (const wchar_t* const name : kExpansionNames) {
      if (regular_file(root / name) &&
          storm.open_archive(root / name, &handles_[count_], 2000U)) {
        ++count_;
        expansion_opened_ = true;
        break;
      }
    }
    const std::filesystem::path patch = root / L"patch_rt.mpq";
    if (regular_file(patch) &&
        storm.open_archive(patch, &handles_[count_], 3000U)) {
      ++count_;
    }
    return true;
  } catch (...) {
    (void)close(storm);
    return false;
  }
}

bool AssetArchives::close(StormModule& storm) noexcept {
  bool closed = true;
  while (count_ != 0U) {
    --count_;
    if (handles_[count_] != nullptr) {
      closed = storm.close_archive(handles_[count_]) && closed;
      handles_[count_] = nullptr;
    }
  }
  expansion_opened_ = false;
  return closed;
}

bool AssetArchives::opened() const noexcept { return count_ != 0U; }
bool AssetArchives::expansion_opened() const noexcept {
  return expansion_opened_;
}

bool has_supported_asset_archives(
    const std::filesystem::path& root) noexcept {
  try {
    for (const wchar_t* const name : kBaseNames) {
      if (regular_file(root / name)) {
        return true;
      }
    }
  } catch (...) {
  }
  return false;
}

}  // namespace starcraft::runtime
