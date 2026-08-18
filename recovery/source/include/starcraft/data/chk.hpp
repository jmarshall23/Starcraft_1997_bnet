#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace starcraft::data {

constexpr std::uint32_t chk_fourcc(
    const char a,
    const char b,
    const char c,
    const char d) noexcept {
  return static_cast<std::uint32_t>(static_cast<std::uint8_t>(a)) |
         (static_cast<std::uint32_t>(static_cast<std::uint8_t>(b)) << 8U) |
         (static_cast<std::uint32_t>(static_cast<std::uint8_t>(c)) << 16U) |
         (static_cast<std::uint32_t>(static_cast<std::uint8_t>(d)) << 24U);
}

constexpr std::uint32_t chk_section_era = chk_fourcc('E', 'R', 'A', ' ');
constexpr std::uint32_t chk_section_dimensions = chk_fourcc('D', 'I', 'M', ' ');
constexpr std::uint32_t chk_section_tiles = chk_fourcc('M', 'T', 'X', 'M');
constexpr std::uint32_t chk_section_editor_tiles = chk_fourcc('T', 'I', 'L', 'E');
constexpr std::uint32_t chk_section_isom = chk_fourcc('I', 'S', 'O', 'M');
constexpr std::uint32_t chk_section_ownership = chk_fourcc('O', 'W', 'N', 'R');
constexpr std::uint32_t chk_section_races = chk_fourcc('S', 'I', 'D', 'E');
// Scenario metadata is indirect: SPRP and FORC store one-based ids into STR.
// Later retail maps may use the 32-bit-offset STRx variant instead.
constexpr std::uint32_t chk_section_strings = chk_fourcc('S', 'T', 'R', ' ');
constexpr std::uint32_t chk_section_extended_strings =
    chk_fourcc('S', 'T', 'R', 'x');
constexpr std::uint32_t chk_section_scenario_properties =
    chk_fourcc('S', 'P', 'R', 'P');
constexpr std::uint32_t chk_section_forces = chk_fourcc('F', 'O', 'R', 'C');
constexpr std::uint32_t chk_section_sounds = chk_fourcc('W', 'A', 'V', ' ');
constexpr std::uint32_t chk_section_triggers = chk_fourcc('T', 'R', 'I', 'G');
constexpr std::uint32_t chk_section_briefing = chk_fourcc('M', 'B', 'R', 'F');
constexpr std::uint32_t chk_section_units = chk_fourcc('U', 'N', 'I', 'T');
constexpr std::uint32_t chk_section_doodads = chk_fourcc('D', 'O', 'O', 'D');
constexpr std::uint32_t chk_section_doodads_retail =
    chk_fourcc('D', 'D', '2', ' ');
// maphdr.cpp::sub_46C070 copies one MASK byte per MTXM tile into both the
// current-mask and black-mask bytes of the live map word.
constexpr std::uint32_t chk_section_fog_mask = chk_fourcc('M', 'A', 'S', 'K');
// MRGN contains the 64 editor location slots. Each slot is a 20-byte record:
// left/top/right/bottom pixel bounds followed by a string id and flags.
constexpr std::uint32_t chk_section_locations = chk_fourcc('M', 'R', 'G', 'N');
constexpr std::size_t chk_location_slot_count = 64;
constexpr std::size_t chk_location_record_bytes = 20;
// StarCraft.exe's beta CHK dispatch table names this section THGY. Retail
// StarCraft/StarEdit uses the wider THG2 records.
constexpr std::uint32_t chk_section_sprites = chk_fourcc('T', 'H', 'G', 'Y');
constexpr std::uint32_t chk_section_sprites_retail =
    chk_fourcc('T', 'H', 'G', '2');
constexpr std::size_t chk_player_slot_count = 12;

#pragma pack(push, 1)
struct BetaUnitPlacement {
  std::uint16_t x;
  std::uint16_t y;
  std::uint16_t unit_type;
  std::uint8_t reserved_06[6];
  std::uint8_t owner;
  std::uint8_t reserved_13;
};

struct BetaSpritePlacement {
  std::uint16_t sprite_type;
  std::uint16_t x;
  std::uint16_t y;
};

struct BetaDoodadPlacement {
  std::uint16_t doodad_type;
  std::uint16_t x;
  std::uint16_t y;
};
#pragma pack(pop)

struct UnitPlacement {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t unit_type{};
  std::uint8_t owner{};
};

static_assert(std::is_standard_layout_v<BetaUnitPlacement>);
static_assert(sizeof(BetaUnitPlacement) == 14);
static_assert(offsetof(BetaUnitPlacement, owner) == 12);
static_assert(sizeof(BetaSpritePlacement) == 6);
static_assert(sizeof(BetaDoodadPlacement) == 6);

struct ChkSection {
  std::uint32_t tag{};
  const std::uint8_t* bytes{};
  std::uint32_t size{};
};

class ChkView final {
 public:
  ChkView(const std::uint8_t* bytes, std::size_t size) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::size_t section_count() const noexcept;
  [[nodiscard]] bool section_at(std::size_t index, ChkSection& output) const noexcept;
  [[nodiscard]] bool section(
      std::uint32_t tag,
      std::size_t occurrence,
      ChkSection& output) const noexcept;
  [[nodiscard]] std::size_t count(std::uint32_t tag) const noexcept;

  [[nodiscard]] bool dimensions(std::uint16_t& width, std::uint16_t& height) const noexcept;
  [[nodiscard]] bool tileset(std::uint16_t& tileset_id) const noexcept;
  [[nodiscard]] bool player_ownership(
      std::size_t player,
      std::uint8_t& ownership) const noexcept;
  [[nodiscard]] bool player_race(std::size_t player, std::uint8_t& race) const noexcept;
  [[nodiscard]] std::size_t unit_count() const noexcept;
  [[nodiscard]] bool unit(std::size_t index, BetaUnitPlacement& output) const noexcept;
  [[nodiscard]] bool unit(std::size_t index, UnitPlacement& output) const noexcept;
  [[nodiscard]] std::size_t sprite_count() const noexcept;
  [[nodiscard]] bool sprite(std::size_t index, BetaSpritePlacement& output) const noexcept;
  [[nodiscard]] bool tile(
      std::uint16_t x,
      std::uint16_t y,
      std::uint16_t& tile_id) const noexcept;

 private:
  [[nodiscard]] bool read_u16(std::size_t offset, std::uint16_t& output) const noexcept;
  [[nodiscard]] bool read_u32(std::size_t offset, std::uint32_t& output) const noexcept;
  [[nodiscard]] std::size_t unit_record_bytes() const noexcept;

  const std::uint8_t* bytes_{};
  std::size_t size_{};
  std::size_t section_count_{};
  bool valid_{};
};

}  // namespace starcraft::data
