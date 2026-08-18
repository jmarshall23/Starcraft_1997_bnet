#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

namespace starcraft::recovery {

struct AiPlayerRuntime;
struct BootstrapStatus;

enum class AiDifficulty : std::uint8_t {
  easy,
  medium,
  hard,
};

// The new strategy boundary intentionally exposes no Lua types.  Lua decides
// goals, while the recovered C++ runtime remains authoritative for resources,
// prerequisites, construction, production, movement, and combat.
class CAI final {
public:
  CAI(std::uint8_t owner, std::uint8_t race, AiDifficulty difficulty,
      std::filesystem::path script_root);
  ~CAI();

  CAI(const CAI &) = delete;
  CAI &operator=(const CAI &) = delete;
  CAI(CAI &&) noexcept;
  CAI &operator=(CAI &&) noexcept;

  [[nodiscard]] bool initialize() noexcept;
  [[nodiscard]] bool update(BootstrapStatus &status, AiPlayerRuntime &player,
                            std::uint32_t simulation_tick) noexcept;
  [[nodiscard]] bool ready() const noexcept;
  [[nodiscard]] const std::string &last_error() const noexcept;
  [[nodiscard]] AiDifficulty difficulty() const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

[[nodiscard]] const char *ai_difficulty_name(AiDifficulty value) noexcept;
[[nodiscard]] bool set_ai_difficulty(BootstrapStatus &status,
                                     AiDifficulty value) noexcept;

} // namespace starcraft::recovery
