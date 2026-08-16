#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace starcraft::game {

struct ImageFacingFrame {
  std::uint8_t frame{};
  bool mirrored{};
};

// Recovered from CImage::sub_411510 at original VA 0x00411510.  Flingy uses
// 256 angular steps while turning GRPs store 17 facings and mirror the other
// 15.  Keep the direction=252..255 case faithful: it selects frame zero with
// the mirrored draw path.
[[nodiscard]] constexpr ImageFacingFrame image_facing_frame(
    const std::uint8_t flingy_direction) noexcept {
  std::uint8_t frame = static_cast<std::uint8_t>(
      (static_cast<unsigned>(flingy_direction) + 4U) >> 3U);
  const bool mirrored = frame > 16U;
  if (mirrored) {
    frame = static_cast<std::uint8_t>(32U - frame);
  }
  return {frame, mirrored};
}

// Address fields remain 32-bit values so the recovered in-memory contract is
// explicit and does not silently change if host-side inspection tools are x64.
#pragma pack(push, 1)
struct ImageLayout32 {
  std::uint32_t previous;             // 0x00
  std::uint32_t next;                 // 0x04
  std::uint32_t reserved_08;          // 0x08
  std::uint16_t image_id;             // 0x0C
  std::uint8_t draw_function;         // 0x0E
  std::uint8_t direction;             // 0x0F
  std::uint16_t flags;                // 0x10
  std::int8_t x_offset;               // 0x12
  std::int8_t y_offset;               // 0x13
  std::uint8_t direction_offset;      // 0x14
  std::uint8_t reserved_15;           // 0x15
  std::uint16_t reserved_16;          // 0x16
  std::uint16_t script_header_offset; // 0x18
  std::uint16_t script_pc;            // 0x1A
  std::uint16_t script_return;        // 0x1C
  std::uint8_t animation;             // 0x1E
  std::uint8_t sleep_ticks;           // 0x1F
  std::uint16_t base_frame;           // 0x20
  std::uint16_t frame;                // 0x22
  std::int16_t screen_x;              // 0x24
  std::int16_t screen_y;              // 0x26
  std::int16_t draw_x;                // 0x28
  std::int16_t draw_y;                // 0x2A
  std::int16_t bounds_left;           // 0x2C
  std::int16_t bounds_top;            // 0x2E
  std::int16_t bounds_right;          // 0x30
  std::int16_t bounds_bottom;         // 0x32
  std::uint32_t group;                // 0x34
  std::uint32_t render_data;          // 0x38
  std::uint32_t draw_callback;        // 0x3C
  std::uint32_t update_callback;      // 0x40
  std::uint32_t sprite_owner;         // 0x44
};

struct SpriteLayout32 {
  std::uint32_t previous;         // 0x00
  std::uint32_t next;             // 0x04
  std::uint32_t reserved_08;      // 0x08
  std::uint16_t sprite_id;        // 0x0C
  std::uint8_t player;            // 0x0E
  std::uint8_t selection_index;   // 0x0F
  std::uint8_t visibility_mask;   // 0x10
  std::uint8_t elevation;         // 0x11
  std::uint8_t flags;             // 0x12
  std::uint8_t reserved_13;       // 0x13
  std::uint16_t reserved_14;      // 0x14
  std::uint8_t canvas_width;      // 0x16
  std::uint8_t canvas_height;     // 0x17
  std::int16_t x;                 // 0x18
  std::int16_t y;                 // 0x1A
  std::uint32_t main_image;       // 0x1C
  std::uint32_t image_head;       // 0x20
  std::uint32_t image_tail;       // 0x24
  std::uint16_t selection_timer;  // 0x28
  std::uint16_t reserved_2A;      // 0x2A
};
#pragma pack(pop)

static_assert(std::is_standard_layout_v<ImageLayout32>);
static_assert(sizeof(ImageLayout32) == 72);
static_assert(offsetof(ImageLayout32, image_id) == 12);
static_assert(offsetof(ImageLayout32, script_header_offset) == 24);
static_assert(offsetof(ImageLayout32, script_pc) == 26);
static_assert(offsetof(ImageLayout32, sleep_ticks) == 31);
static_assert(offsetof(ImageLayout32, base_frame) == 32);
static_assert(offsetof(ImageLayout32, group) == 52);
static_assert(offsetof(ImageLayout32, sprite_owner) == 68);

static_assert(std::is_standard_layout_v<SpriteLayout32>);
static_assert(sizeof(SpriteLayout32) == 44);
static_assert(offsetof(SpriteLayout32, sprite_id) == 12);
static_assert(offsetof(SpriteLayout32, x) == 24);
static_assert(offsetof(SpriteLayout32, main_image) == 28);
static_assert(offsetof(SpriteLayout32, image_head) == 32);
static_assert(offsetof(SpriteLayout32, selection_timer) == 40);

}  // namespace starcraft::game
