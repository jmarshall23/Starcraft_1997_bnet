#include "starcraft/lang/iscript.hpp"

#include <cstddef>
#include <cstdint>

namespace starcraft::lang {

IScriptProgramView::IScriptProgramView(
    const std::uint8_t* const bytes,
    const std::size_t size) noexcept
    : bytes_(bytes), size_(size) {
  if (bytes == nullptr || size < sizeof(std::uint16_t)) {
    return;
  }

  std::size_t cursor{};
  while (cursor + sizeof(std::uint16_t) <= size) {
    std::uint16_t script_id{};
    if (!read_u16(cursor, script_id)) {
      return;
    }
    if (script_id == 0xFFFFU) {
      valid_ = true;
      return;
    }

    std::uint16_t header{};
    std::uint32_t magic{};
    if (!read_u16(cursor + 2, header) || !read_u32(header, magic) ||
        magic != iscript_header_magic) {
      return;
    }
    ++script_count_;
    cursor += 4;
  }
}

bool IScriptProgramView::valid() const noexcept { return valid_; }

std::size_t IScriptProgramView::script_count() const noexcept { return script_count_; }

bool IScriptProgramView::header_offset(
    const std::uint16_t script_id,
    std::uint16_t& output) const noexcept {
  if (!valid_) {
    return false;
  }
  std::size_t cursor{};
  while (cursor + 2 <= size_) {
    std::uint16_t candidate{};
    if (!read_u16(cursor, candidate) || candidate == 0xFFFFU) {
      return false;
    }
    std::uint16_t header{};
    if (!read_u16(cursor + 2, header)) {
      return false;
    }
    if (candidate == script_id) {
      output = header;
      return true;
    }
    cursor += 4;
  }
  return false;
}

bool IScriptProgramView::animation_offset(
    const std::uint16_t script_id,
    const std::uint8_t action,
    std::uint16_t& output) const noexcept {
  std::uint16_t header{};
  std::uint32_t magic{};
  std::uint16_t maximum_action{};
  if (!header_offset(script_id, header) || !read_u32(header, magic) ||
      magic != iscript_header_magic || !read_u16(static_cast<std::size_t>(header) + 4, maximum_action) ||
      action > maximum_action) {
    return false;
  }
  const std::size_t action_entry = static_cast<std::size_t>(header) + 8 + 2U * action;
  if (!read_u16(action_entry, output) || output == 0 || output >= size_) {
    return false;
  }
  return true;
}

bool IScriptProgramView::start(
    const std::uint16_t script_id,
    const std::uint8_t action,
    IScriptState& state) const noexcept {
  std::uint16_t program{};
  if (!animation_offset(script_id, action, program)) {
    return false;
  }
  state = {};
  state.program_counter = program;
  state.active = true;
  return true;
}

IScriptTickResult IScriptProgramView::tick(
    IScriptState& state,
    const std::uint32_t random_value,
    const std::size_t instruction_budget,
    const IScriptState* const parent,
    const std::uint16_t tileset_frame_offset) const noexcept {
  if (!valid_) {
    return IScriptTickResult::malformed_program;
  }
  if (!state.active) {
    return IScriptTickResult::ended;
  }
  if (state.sleep_ticks != 0) {
    --state.sleep_ticks;
    return IScriptTickResult::sleeping;
  }

  for (std::size_t instruction = 0; instruction < instruction_budget; ++instruction) {
    std::uint8_t opcode{};
    if (!read_u8(state.program_counter, opcode)) {
      state.active = false;
      return IScriptTickResult::malformed_program;
    }
    ++state.program_counter;

    std::uint8_t byte{};
    std::uint16_t word{};
    switch (opcode) {
      case 0x00:  // set frame
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2);
        state.frame = word;
        break;
      case 0x02:  // set frame relative to the current tileset
        // CImage.cpp's dispatcher (sub_415210, case 2) consumes a u16 and
        // adds the current map tileset global at 0x0050A5C0 before assigning
        // CImage+0x20. Neutral geyser script 187 uses operand zero here.
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2U);
        state.frame = static_cast<std::uint16_t>(tileset_frame_offset + word);
        break;
      case 0x03:  // set horizontal image offset
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.x_offset = static_cast<std::int8_t>(byte);
        break;
      case 0x04:  // set vertical image offset
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.y_offset = static_cast<std::int8_t>(byte);
        break;
      case 0x05:  // set both image offsets
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.x_offset = static_cast<std::int8_t>(byte);
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.y_offset = static_cast<std::int8_t>(byte);
        break;
      case 0x06:  // wait
        if (!read_u8(state.program_counter++, byte) || byte == 0) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.sleep_ticks = static_cast<std::uint8_t>(byte - 1);
        return IScriptTickResult::yielded;
      case 0x07: {  // wait random inclusive range
        std::uint8_t maximum{};
        if (!read_u8(state.program_counter++, byte) ||
            !read_u8(state.program_counter++, maximum) || byte == 0 || byte > maximum) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        const std::uint32_t range = static_cast<std::uint32_t>(maximum - byte) + 1U;
        state.sleep_ticks = static_cast<std::uint8_t>(byte + random_value % range - 1U);
        return IScriptTickResult::yielded;
      }
      case 0x08:  // jump
        if (!read_u16(state.program_counter, word) || word >= size_) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = word;
        break;
      case 0x09:  // create image overlay above the current image
      case 0x0A:  // create image overlay below the current image
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2);
        state.overlay_image = word;
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.overlay_x_offset = static_cast<std::int8_t>(byte);
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.overlay_y_offset = static_cast<std::int8_t>(byte);
        state.overlay_above = opcode == 0x09;
        ++state.overlay_event_count;
        break;
      case 0x11:  // create a sprite below the current sprite
        // CImage.cpp::sub_415210 case 0x11 consumes a u16 sprite ID and a
        // signed Y offset, creates the CSprite at the owning sprite/image
        // position, forces CSprite+17 (elevation) to zero, then initializes
        // its head image. Building death scripts use this for the separately
        // owned explosion sprite after attaching their first blast image.
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter =
            static_cast<std::uint16_t>(state.program_counter + 2U);
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.sprite_id = word;
        state.sprite_y_offset = static_cast<std::int8_t>(byte);
        state.sprite_elevation = 0U;
        ++state.sprite_event_count;
        break;
      case 0x19:  // end/delete image
        state.active = false;
        return IScriptTickResult::ended;
      case 0x1A:  // mirror
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.mirrored = byte != 0;
        break;
      case 0x1B:  // play one sound
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2U);
        state.sound_event = word;
        state.sound_range_first = word;
        state.sound_range_last = word;
        ++state.sound_event_count;
        break;
      case 0x1C:
      case 0x1F: {  // play one random sound; 0x1F also fires the unit weapon
        // CImage.cpp::sub_415210 reads a byte count (at most ten), chooses
        // one of the following u16 sound IDs, and advances past the complete
        // list. Opcode 0x1F invokes CUnitCombat.cpp::sub_4267E0 before using
        // the identical list decoder.
        std::uint8_t count{};
        if (!read_u8(state.program_counter++, count) || count == 0U ||
            count > 10U) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        const std::size_t choices = state.program_counter;
        const std::size_t bytes = static_cast<std::size_t>(count) * 2U;
        if (choices + bytes > size_) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        const std::size_t selected = random_value % count;
        if (!read_u16(choices + selected * 2U, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter =
            static_cast<std::uint16_t>(state.program_counter + bytes);
        state.sound_event = word;
        state.sound_range_first = word;
        state.sound_range_last = word;
        ++state.sound_event_count;
        if (opcode == 0x1FU) {
          state.weapon_event = 0xFFU;
          ++state.weapon_event_count;
        }
        break;
      }
      case 0x1D: {  // play one sound from an inclusive numeric range
        std::uint16_t last{};
        if (!read_u16(state.program_counter, word) ||
            !read_u16(static_cast<std::size_t>(state.program_counter) + 2U, last) ||
            last < word) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 4U);
        const std::uint32_t range = static_cast<std::uint32_t>(last - word) + 1U;
        state.sound_event = static_cast<std::uint16_t>(word + random_value % range);
        state.sound_range_first = word;
        state.sound_range_last = last;
        ++state.sound_event_count;
        break;
      }
      case 0x1E:  // apply the bullet's impact behavior
        // CImage.cpp delegates this operand-less event to CBullet. The image
        // timeline continues even when the bootstrap does not materialize a
        // separate projectile object (for example the SCV mining cutter).
        break;
      case 0x20:  // synchronize attached image with its parent image
        if (parent == nullptr) {
          state.unsupported_opcode = opcode;
          return IScriptTickResult::unsupported_opcode;
        }
        state.frame = parent->frame;
        state.mirrored = parent->mirrored;
        break;
      case 0x22: {  // random conditional jump (inclusive byte threshold)
        std::uint8_t threshold{};
        if (!read_u8(state.program_counter++, threshold) ||
            !read_u16(state.program_counter, word) || word >= size_) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2);
        if (static_cast<std::uint8_t>(random_value) <= threshold) {
          state.program_counter = word;
        }
        break;
      }
      case 0x26:  // invoke one of the unit's two action variants
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.unit_event = byte;
        state.alternate_unit_event = (random_value & 3U) == 1U;
        ++state.unit_event_count;
        break;
      case 0x27:  // OR flags into the image target
        // CImage.cpp consumes one flag byte and ORs it into the target
        // object's byte +91. CUnitPBuild/CUnitZBuild use bits 1 and 4 from
        // this exact event to advance construction state machines.
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.image_target_flags |= byte;
        break;
      case 0x28:  // attack with the encoded weapon/variant
        // CImage.cpp::sub_415210 consumes one byte and passes it to
        // CUnitCombat.cpp::sub_4266F0. Preserve the launch boundary so the
        // unit's attack timeline continues through its damage frame.
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.weapon_event = byte;
        ++state.weapon_event_count;
        break;
      case 0x29:  // attack the current air target with variant two
        state.weapon_event = 2U;
        ++state.weapon_event_count;
        break;
      case 0x2A:  // attack with the current order's weapon
        state.weapon_event = 0xFFU;
        ++state.weapon_event_count;
        break;
      case 0x2B:  // launch the explicitly named weapon at the unit target
        // CImage.cpp's opcode dispatcher at 0x00412100 consumes one weapon
        // byte and calls CUnitCombat.cpp::sub_426650. Preserve the event at
        // the interpreter boundary so simulations can decide whether to
        // launch a projectile, play its feedback, or only animate it.
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.weapon_event = byte;
        ++state.weapon_event_count;
        break;
      case 0x2C:  // set the owning flingy's current velocity
        // CImage.cpp::sub_415210 case 0x2C consumes a byte, shifts it left by
        // eight, applies CUnit.cpp::sub_434480's status modifier, and writes
        // the result to CFlingy+0x48 through sub_4063C0. The interpreter
        // records the unmodified value; the recovery has no stim/ensnare
        // status fields yet.
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.flingy_velocity = static_cast<std::uint16_t>(byte) << 8U;
        ++state.flingy_velocity_event_count;
        break;
      case 0x2D:  // clear the owning CUnit's attacking flag
        // The recovered simulation owns that flag at the order layer. The
        // opcode is operand-less and must not terminate the image timeline.
        break;
      case 0x33:  // begin an uninterruptible image sequence
        // CImage.cpp::sub_415210 sets CUnit+216 bit 0x80 and CSprite+18 bit
        // 0x20. Marine and other weapon scripts bracket their launch frames
        // with 0x33/0x34; treating this as unsupported stopped the script on
        // its third tick and made every cooldown restart visibly flicker.
        state.uninterruptible = true;
        break;
      case 0x34:  // finish an uninterruptible image sequence
        // The original calls CUnitOrder.cpp::sub_4349D0, which clears the
        // same CUnit and CSprite bits before resuming normal order handling.
        state.uninterruptible = false;
        break;
      case 0x35:  // wait while the owning CUnit still has an attack target
        // The original rewinds the image PC to this opcode and sleeps for ten
        // image ticks while CUnit+100 is non-null. The interpreter has no
        // CUnit pointer, so expose the wait boundary to gameloop.cpp, which
        // performs the recovered target test before the next tick.
        --state.program_counter;
        state.sleep_ticks = 9U;
        state.waiting_for_attack_target = true;
        return IScriptTickResult::yielded;
      case 0x37:  // hide
        state.hidden = true;
        break;
      case 0x38:  // show
        state.hidden = false;
        break;
      case 0x3F:  // set the owning flingy's current top speed
        // CImage.cpp::sub_415210 case 0x3F consumes a u16 and stores it at
        // CUnit/CFlingy +0x40. Walking units have a DAT top-speed sentinel of
        // two fixed-point units; their walking animation supplies the real
        // speed through this opcode.
        if (!read_u16(state.program_counter, word)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2U);
        state.flingy_speed = word;
        ++state.flingy_speed_event_count;
        break;
      case 0x40:  // create a resource-source plume at an LO-table point
        // CImage.cpp::sub_415210 case 0x40 consumes a point index, resolves it
        // through dword_55A918[image][frame], and creates
        // image 402+point for a nonempty resource (407+point when depleted).
        if (!read_u8(state.program_counter++, byte)) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.resource_overlay_point = byte;
        ++state.resource_overlay_event_count;
        break;
      case 0x41:  // loop while this is not the owning sprite's head image
        // Inventory overlays are inserted as the sprite's head image by
        // CUnitInv.cpp::sub_430FB0, so the branch is not taken for the cargo
        // image represented by this state. Still consume its word target.
        if (!read_u16(state.program_counter, word) || word >= size_) {
          state.active = false;
          return IScriptTickResult::malformed_program;
        }
        state.program_counter = static_cast<std::uint16_t>(state.program_counter + 2U);
        break;
      default:
        state.unsupported_opcode = opcode;
        return IScriptTickResult::unsupported_opcode;
    }
  }
  return IScriptTickResult::instruction_limit;
}

bool IScriptProgramView::read_u8(
    const std::size_t offset,
    std::uint8_t& output) const noexcept {
  if (offset >= size_) {
    return false;
  }
  output = bytes_[offset];
  return true;
}

bool IScriptProgramView::read_u16(
    const std::size_t offset,
    std::uint16_t& output) const noexcept {
  if (offset > size_ || size_ - offset < 2) {
    return false;
  }
  output = static_cast<std::uint16_t>(bytes_[offset]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes_[offset + 1]) << 8U);
  return true;
}

bool IScriptProgramView::read_u32(
    const std::size_t offset,
    std::uint32_t& output) const noexcept {
  if (offset > size_ || size_ - offset < 4) {
    return false;
  }
  output = static_cast<std::uint32_t>(bytes_[offset]) |
           (static_cast<std::uint32_t>(bytes_[offset + 1]) << 8U) |
           (static_cast<std::uint32_t>(bytes_[offset + 2]) << 16U) |
           (static_cast<std::uint32_t>(bytes_[offset + 3]) << 24U);
  return true;
}

}  // namespace starcraft::lang
