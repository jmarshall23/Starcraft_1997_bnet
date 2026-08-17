#pragma once

#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

constexpr std::uint32_t iscript_header_magic = 0x45504353U;

enum class IScriptTickResult {
  yielded,
  sleeping,
  ended,
  unsupported_opcode,
  malformed_program,
  instruction_limit,
};

struct IScriptState {
  std::uint16_t program_counter{};
  std::uint16_t frame{};
  std::int8_t x_offset{};
  std::int8_t y_offset{};
  std::uint8_t sleep_ticks{};
  std::uint8_t unsupported_opcode{};
  std::uint16_t overlay_image{};
  std::int8_t overlay_x_offset{};
  std::int8_t overlay_y_offset{};
  std::uint32_t overlay_event_count{};
  std::uint32_t unit_event_count{};
  std::uint8_t unit_event{};
  std::uint32_t weapon_event_count{};
  std::uint8_t weapon_event{};
  std::uint32_t sound_event_count{};
  std::uint16_t sound_event{};
  std::uint16_t sound_range_first{};
  std::uint16_t sound_range_last{};
  std::uint32_t flingy_velocity_event_count{};
  std::uint16_t flingy_velocity{};
  std::uint32_t flingy_speed_event_count{};
  std::uint16_t flingy_speed{};
  std::uint32_t resource_overlay_event_count{};
  std::uint8_t resource_overlay_point{};
  std::uint32_t sprite_event_count{};
  std::uint16_t sprite_id{};
  std::int8_t sprite_y_offset{};
  std::uint8_t sprite_elevation{};
  std::uint8_t image_target_flags{};
  bool overlay_above{};
  bool alternate_unit_event{};
  bool mirrored{};
  bool hidden{};
  // CImage opcode 0x33 marks the owning unit/sprite as being inside an
  // uninterruptible image sequence; opcode 0x34 clears both flags again.
  bool uninterruptible{};
  // Opcode 0x35 holds an attack image at the end of its repeat sequence
  // while CUnit+100 still names a target. The CUnit runtime clears this and
  // dispatches the attack-to-idle animation after the target disappears.
  bool waiting_for_attack_target{};
  bool active{};
};

class IScriptProgramView final {
 public:
  IScriptProgramView(const std::uint8_t* bytes, std::size_t size) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::size_t script_count() const noexcept;
  [[nodiscard]] bool header_offset(
      std::uint16_t script_id,
      std::uint16_t& output) const noexcept;
  [[nodiscard]] bool animation_offset(
      std::uint16_t script_id,
      std::uint8_t action,
      std::uint16_t& output) const noexcept;
  [[nodiscard]] bool start(
      std::uint16_t script_id,
      std::uint8_t action,
      IScriptState& state) const noexcept;

  // Advances one original image-update tick. The random value is injected so
  // random-wait behavior is deterministic in tests and replayable simulations.
  [[nodiscard]] IScriptTickResult tick(
      IScriptState& state,
      std::uint32_t random_value = 0,
      std::size_t instruction_budget = 256,
      const IScriptState* parent = nullptr,
      std::uint16_t tileset_frame_offset = 0) const noexcept;

 private:
  [[nodiscard]] bool read_u8(std::size_t offset, std::uint8_t& output) const noexcept;
  [[nodiscard]] bool read_u16(std::size_t offset, std::uint16_t& output) const noexcept;
  [[nodiscard]] bool read_u32(std::size_t offset, std::uint32_t& output) const noexcept;

  const std::uint8_t* bytes_{};
  std::size_t size_{};
  std::size_t script_count_{};
  bool valid_{};
};

}  // namespace starcraft::lang
