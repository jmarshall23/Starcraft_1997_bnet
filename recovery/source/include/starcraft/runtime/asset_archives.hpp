#pragma once

#include <array>
#include <cstddef>
#include <filesystem>

namespace starcraft::runtime {

class StormModule;

// Opens either the recovered beta MPQ set or a retail StarCraft/Brood War MPQ
// set into Storm's priority chain. Patch data overrides expansion and base.
class AssetArchives final {
 public:
  [[nodiscard]] bool open(StormModule& storm,
                          const std::filesystem::path& root) noexcept;
  [[nodiscard]] bool close(StormModule& storm) noexcept;
  [[nodiscard]] bool opened() const noexcept;
  [[nodiscard]] bool expansion_opened() const noexcept;

 private:
  std::array<void*, 3> handles_{};
  std::size_t count_{};
  bool expansion_opened_{};
};

[[nodiscard]] bool has_supported_asset_archives(
    const std::filesystem::path& root) noexcept;

}  // namespace starcraft::runtime
