#include "starcraft/data/dat.hpp"

#include "starcraft/runtime/storm.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace starcraft::data {
namespace {

constexpr std::array<DatFieldShape, 48> kUnitShapes{{
    {1, 228}, {2, 228}, {2, 228}, {2, 98},  {4, 228}, {1, 228}, {1, 228},
    {2, 228}, {4, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {4, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {2, 106}, {2, 228},
    {2, 228}, {2, 106}, {2, 106}, {2, 106}, {2, 106}, {2, 456}, {2, 196},
    {2, 912}, {2, 228}, {2, 228}, {2, 228}, {2, 228}, {2, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228},
}};

// Retail 1.x/Brood War keeps the same field ordering through cargo capacity,
// then stores 16-bit build/destroy scores, the editor string id, the Brood
// War flag, and editor availability. Buildings also shrink from 98 beta rows
// to the retail 96-row range (unit ids 106..201).
constexpr std::array<DatFieldShape, 51> kRetailUnitShapes{{
    {1, 228}, {2, 228}, {2, 228}, {2, 96},  {4, 228}, {1, 228}, {1, 228},
    {2, 228}, {4, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {4, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {1, 228}, {2, 106}, {2, 228},
    {2, 228}, {2, 106}, {2, 106}, {2, 106}, {2, 106}, {2, 456}, {2, 192},
    {2, 912}, {2, 228}, {2, 228}, {2, 228}, {2, 228}, {2, 228}, {1, 228},
    {1, 228}, {1, 228}, {1, 228}, {1, 228}, {2, 228}, {2, 228}, {2, 228},
    {1, 228}, {2, 228},
}};

constexpr std::array<DatFieldShape, 24> kWeaponShapes{{
    {2, 66}, {4, 66}, {1, 66}, {2, 66}, {4, 66}, {4, 66}, {1, 66}, {1, 66},
    {1, 66}, {1, 66}, {1, 66}, {2, 66}, {2, 66}, {2, 66}, {2, 66}, {2, 66},
    {1, 66}, {1, 66}, {1, 66}, {1, 66}, {1, 66}, {1, 66}, {2, 66}, {2, 66},
}};

constexpr std::array<DatFieldShape, 24> kRetailWeaponShapes{{
    {2, 130}, {4, 130}, {1, 130}, {2, 130}, {4, 130}, {4, 130},
    {1, 130}, {1, 130}, {1, 130}, {1, 130}, {1, 130}, {2, 130},
    {2, 130}, {2, 130}, {2, 130}, {2, 130}, {1, 130}, {1, 130},
    {1, 130}, {1, 130}, {1, 130}, {1, 130}, {2, 130}, {2, 130},
}};

constexpr std::array<DatFieldShape, 6> kFlingyShapes{{
    {2, 175},
    {4, 175},
    {2, 175},
    {1, 175},
    {1, 175},
    {1, 175},
}};

constexpr std::array<DatFieldShape, 7> kRetailFlingyShapes{{
    {2, 209}, {4, 209}, {2, 209}, {4, 209},
    {1, 209}, {1, 209}, {1, 209},
}};

constexpr std::array<DatFieldShape, 5> kSpriteShapes{{
    {2, 267},
    {1, 164},
    {1, 267},
    {1, 164},
    {1, 164},
}};

constexpr std::array<DatFieldShape, 5> kRetailSpriteShapes{{
    {2, 517}, {1, 387}, {1, 517}, {1, 387}, {1, 387},
}};

constexpr std::array<DatFieldShape, 16> kImageShapes{{
    {4, 590},
    {1, 590},
    {1, 590},
    {1, 590},
    {1, 590},
    {1, 590},
    {1, 590},
    {1, 590},
    {4, 590},
    {4, 590},
    {4, 590},
    {4, 590},
    {4, 590},
    {4, 590},
    {4, 590},
    {1, 590},
}};

constexpr std::array<DatFieldShape, 16> kRetailImageShapes{{
    {4, 999}, {1, 999}, {1, 999}, {1, 999}, {1, 999}, {1, 999},
    {1, 999}, {1, 999}, {4, 999}, {4, 999}, {4, 999}, {4, 999},
    {4, 999}, {4, 999}, {4, 999}, {1, 999},
}};

// StarCraft.exe descriptor tables 0x00502000 and 0x00502A78. The research
// tables contain 28 technology rows and 46 upgrade rows respectively.
constexpr std::array<DatFieldShape, 9> kTechnologyShapes{{
    {2, 28}, {2, 28}, {2, 28}, {2, 28}, {2, 28},
    {2, 28}, {2, 28}, {2, 28}, {1, 28},
}};

constexpr std::array<DatFieldShape, 9> kRetailTechnologyShapes{{
    {2, 44}, {2, 44}, {2, 44}, {2, 44}, {4, 44},
    {2, 44}, {2, 44}, {1, 44}, {2, 44},
}};

// StarCraft.exe descriptor table 0x004FAA00: one word field, twelve byte
// fields, then two word fields. Fields 10/11/12 are byte_732988 (weapon),
// byte_732670 (technology), and byte_733058 (IScript action).
constexpr std::array<DatFieldShape, 15> kOrderShapes{{
    {2, 156}, {1, 156}, {1, 156}, {1, 156}, {1, 156},
    {1, 156}, {1, 156}, {1, 156}, {1, 156}, {1, 156},
    {1, 156}, {1, 156}, {1, 156}, {2, 156}, {2, 156},
}};

constexpr std::array<DatFieldShape, 19> kRetailOrderShapes{{
    {2, 189}, {1, 189}, {1, 189}, {1, 189}, {1, 189}, {1, 189},
    {1, 189}, {1, 189}, {1, 189}, {1, 189}, {1, 189}, {1, 189},
    {1, 189}, {1, 189}, {1, 189}, {1, 189}, {2, 189}, {2, 189},
    {1, 189},
}};

constexpr std::array<DatFieldShape, 11> kUpgradeShapes{{
    {2, 46}, {2, 46}, {2, 46}, {2, 46}, {2, 46}, {2, 46},
    {2, 46}, {2, 46}, {2, 46}, {1, 46}, {1, 46},
}};

constexpr std::array<DatFieldShape, 12> kRetailUpgradeShapes{{
    {2, 61}, {2, 61}, {2, 61}, {2, 61}, {2, 61}, {2, 61},
    {2, 61}, {2, 61}, {2, 61}, {1, 61}, {1, 61}, {1, 61},
}};

constexpr std::array<DatFieldShape, 1> kMapDataShapes{{
    {4, 13},
}};

constexpr std::array<DatFieldShape, 1> kRetailMapDataShapes{{
    {4, 65},
}};

// StarCraft.exe descriptor table 0x004FAE20 consumed by
// gamedata.cpp::sub_453360: two 87-entry string-ID arrays followed by four
// 87-entry probability arrays.
constexpr std::array<DatFieldShape, 6> kPortraitShapes{{
    {4, 87},
    {4, 87},
    {1, 87},
    {1, 87},
    {1, 87},
    {1, 87},
}};

constexpr std::array<DatFieldShape, 6> kRetailPortraitShapes{{
    {4, 110}, {4, 110}, {1, 110}, {1, 110}, {1, 110}, {1, 110},
}};

template <std::size_t Size>
constexpr std::size_t dat_payload_bytes(
    const std::array<DatFieldShape, Size>& shapes) noexcept {
  std::size_t bytes{};
  for (const DatFieldShape shape : shapes) {
    bytes += static_cast<std::size_t>(shape.element_width) *
             shape.element_count;
  }
  return bytes;
}

static_assert(dat_payload_bytes(kRetailUnitShapes) == 19192U);
static_assert(dat_payload_bytes(kRetailWeaponShapes) == 5460U);
static_assert(dat_payload_bytes(kRetailFlingyShapes) == 3135U);
static_assert(dat_payload_bytes(kRetailSpriteShapes) == 2712U);
static_assert(dat_payload_bytes(kRetailImageShapes) == 39960U);
static_assert(dat_payload_bytes(kRetailTechnologyShapes) == 836U);
static_assert(dat_payload_bytes(kRetailOrderShapes) == 4158U);
static_assert(dat_payload_bytes(kRetailUpgradeShapes) == 1281U);
static_assert(dat_payload_bytes(kRetailMapDataShapes) == 260U);
static_assert(dat_payload_bytes(kRetailPortraitShapes) == 1320U);

template <std::size_t Size>
bool load_dat(runtime::StormModule &storm, const char *const path,
              const std::array<DatFieldShape, Size> &shapes, DatTable &output,
              std::size_t &total, std::string &failed_asset) {
  std::vector<std::uint8_t> payload;
  if (!storm.load_file(path, payload) ||
      !output.unpack(payload, shapes.data(), shapes.size())) {
    failed_asset = path;
    return false;
  }
  total += payload.size();
  return true;
}

std::uint16_t read_u16(const std::uint8_t *const bytes) noexcept {
  return static_cast<std::uint16_t>(bytes[0]) |
         static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[1]) << 8U);
}

} // namespace

std::uint16_t DatField::element_width() const noexcept {
  return element_width_;
}

std::uint16_t DatField::element_count() const noexcept {
  return element_count_;
}

const std::vector<std::uint8_t> &DatField::bytes() const noexcept {
  return bytes_;
}

bool DatTable::unpack(const std::vector<std::uint8_t> &payload,
                      const DatFieldShape *const shapes,
                      const std::size_t shape_count) noexcept {
  if (shapes == nullptr && shape_count != 0) {
    return false;
  }
  try {
    std::size_t required{};
    for (std::size_t index = 0; index < shape_count; ++index) {
      const std::size_t field_size =
          static_cast<std::size_t>(shapes[index].element_width) *
          shapes[index].element_count;
      if (required > std::numeric_limits<std::size_t>::max() - field_size) {
        return false;
      }
      required += field_size;
    }
    if (required != payload.size()) {
      return false;
    }

    std::vector<DatField> unpacked;
    unpacked.reserve(shape_count);
    std::size_t cursor{};
    for (std::size_t index = 0; index < shape_count; ++index) {
      DatField field{};
      field.element_width_ = shapes[index].element_width;
      field.element_count_ = shapes[index].element_count;
      const std::size_t field_size =
          static_cast<std::size_t>(field.element_width_) * field.element_count_;
      field.bytes_.assign(payload.begin() + cursor,
                          payload.begin() + cursor + field_size);
      cursor += field_size;
      unpacked.push_back(std::move(field));
    }
    fields_ = std::move(unpacked);
    payload_size_ = payload.size();
    return true;
  } catch (...) {
    return false;
  }
}

const DatField *DatTable::field(const std::size_t index) const noexcept {
  return index < fields_.size() ? &fields_[index] : nullptr;
}

std::size_t DatTable::field_count() const noexcept { return fields_.size(); }

std::size_t DatTable::payload_size() const noexcept { return payload_size_; }

StringTableView::StringTableView(const std::uint8_t *const bytes,
                                 const std::size_t size) noexcept
    : bytes_(bytes), size_(size) {
  if (bytes == nullptr || size < sizeof(std::uint16_t)) {
    return;
  }
  count_ = read_u16(bytes);
  const std::size_t header_size =
      sizeof(std::uint16_t) +
      static_cast<std::size_t>(count_) * sizeof(std::uint16_t);
  if (header_size > size) {
    return;
  }
  for (std::uint16_t index = 0; index < count_; ++index) {
    const std::size_t offset = read_u16(bytes + 2 + 2 * index);
    if (offset >= size) {
      return;
    }
    std::size_t cursor = offset;
    while (cursor < size && bytes[cursor] != 0) {
      ++cursor;
    }
    if (cursor == size) {
      return;
    }
  }
  valid_ = true;
}

bool StringTableView::valid() const noexcept { return valid_; }

std::uint16_t StringTableView::count() const noexcept {
  return valid_ ? count_ : 0;
}

std::string_view
StringTableView::one_based(const std::uint16_t string_id) const noexcept {
  if (!valid_ || string_id == 0 || string_id > count_) {
    return {};
  }
  const std::size_t offset = read_u16(bytes_ + 2 + 2 * (string_id - 1));
  const char *const string = reinterpret_cast<const char *>(bytes_ + offset);
  std::size_t length{};
  while (offset + length < size_ && string[length] != '\0') {
    ++length;
  }
  return {string, length};
}

bool CoreDataSet::load(runtime::StormModule &storm) noexcept {
  failed_asset_.clear();
  total_payload_bytes_ = 0;
  retail_assets_ = false;

  std::vector<std::uint8_t> unit_payload{};
  if (!storm.load_file(R"(arr\units.dat)", unit_payload)) {
    failed_asset_ = R"(arr\units.dat)";
    return false;
  }
  if (!units_.unpack(unit_payload, kUnitShapes.data(), kUnitShapes.size())) {
    if (!units_.unpack(unit_payload, kRetailUnitShapes.data(),
                       kRetailUnitShapes.size())) {
      failed_asset_ = R"(arr\units.dat)";
      return false;
    }
    retail_assets_ = true;
  }
  total_payload_bytes_ += unit_payload.size();

  const bool tables_loaded =
      retail_assets_
          ? load_dat(storm, R"(arr\weapons.dat)", kRetailWeaponShapes,
                     weapons_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\flingy.dat)", kRetailFlingyShapes,
                         flingy_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\sprites.dat)", kRetailSpriteShapes,
                         sprites_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\images.dat)", kRetailImageShapes,
                         images_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\techdata.dat)",
                         kRetailTechnologyShapes, technologies_,
                         total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\orders.dat)", kRetailOrderShapes,
                         orders_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\upgrades.dat)", kRetailUpgradeShapes,
                         upgrades_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\mapdata.dat)", kRetailMapDataShapes,
                         mapdata_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\portdata.dat)",
                         kRetailPortraitShapes, portraits_,
                         total_payload_bytes_, failed_asset_)
          : load_dat(storm, R"(arr\weapons.dat)", kWeaponShapes, weapons_,
                     total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\flingy.dat)", kFlingyShapes, flingy_,
                         total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\sprites.dat)", kSpriteShapes,
                         sprites_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\images.dat)", kImageShapes, images_,
                         total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\techdata.dat)", kTechnologyShapes,
                         technologies_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\orders.dat)", kOrderShapes, orders_,
                         total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\upgrades.dat)", kUpgradeShapes,
                         upgrades_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\mapdata.dat)", kMapDataShapes,
                         mapdata_, total_payload_bytes_, failed_asset_) &&
                load_dat(storm, R"(arr\portdata.dat)", kPortraitShapes,
                         portraits_, total_payload_bytes_, failed_asset_);
  if (!tables_loaded) {
    return false;
  }
  if (!storm.load_file(R"(arr\images.tbl)", image_strings_)) {
    failed_asset_ = R"(arr\images.tbl)";
    return false;
  }
  const StringTableView strings{image_strings_.data(), image_strings_.size()};
  if (!strings.valid()) {
    failed_asset_ = R"(arr\images.tbl)";
    return false;
  }
  total_payload_bytes_ += image_strings_.size();
  if (!storm.load_file(R"(scripts\iscript.bin)", iscript_)) {
    failed_asset_ = R"(scripts\iscript.bin)";
    return false;
  }
  total_payload_bytes_ += iscript_.size();
  if (!storm.load_file(R"(arr\mapdata.tbl)", map_strings_)) {
    failed_asset_ = R"(arr\mapdata.tbl)";
    return false;
  }
  const StringTableView map_strings{map_strings_.data(), map_strings_.size()};
  if (!map_strings.valid()) {
    failed_asset_ = R"(arr\mapdata.tbl)";
    return false;
  }
  total_payload_bytes_ += map_strings_.size();
  if (!storm.load_file(R"(arr\portdata.tbl)", portrait_strings_)) {
    failed_asset_ = R"(arr\portdata.tbl)";
    return false;
  }
  const StringTableView portrait_strings{portrait_strings_.data(),
                                         portrait_strings_.size()};
  if (!portrait_strings.valid()) {
    failed_asset_ = R"(arr\portdata.tbl)";
    return false;
  }
  total_payload_bytes_ += portrait_strings_.size();
  return true;
}

const DatTable &CoreDataSet::units() const noexcept { return units_; }
const DatTable &CoreDataSet::weapons() const noexcept { return weapons_; }
const DatTable &CoreDataSet::flingy() const noexcept { return flingy_; }
const DatTable &CoreDataSet::sprites() const noexcept { return sprites_; }
const DatTable &CoreDataSet::images() const noexcept { return images_; }
const DatTable &CoreDataSet::technologies() const noexcept {
  return technologies_;
}
const DatTable &CoreDataSet::orders() const noexcept { return orders_; }
const DatTable &CoreDataSet::upgrades() const noexcept { return upgrades_; }
const DatTable &CoreDataSet::mapdata() const noexcept { return mapdata_; }
const DatTable &CoreDataSet::portraits() const noexcept { return portraits_; }
const std::vector<std::uint8_t> &CoreDataSet::image_strings() const noexcept {
  return image_strings_;
}
const std::vector<std::uint8_t> &CoreDataSet::iscript() const noexcept {
  return iscript_;
}
const std::vector<std::uint8_t> &CoreDataSet::map_strings() const noexcept {
  return map_strings_;
}
bool CoreDataSet::retail_assets() const noexcept { return retail_assets_; }
std::size_t CoreDataSet::total_payload_bytes() const noexcept {
  return total_payload_bytes_;
}
const std::string &CoreDataSet::failed_asset() const noexcept {
  return failed_asset_;
}

bool CoreDataSet::extract_unit_traits(
    starcraft::lang::UnitTraitsTable &traits) const noexcept {
  const DatField *const race = units_.field(41);
  const DatField *const provided = units_.field(42);
  const DatField *const required = units_.field(43);
  const DatField *const flags = units_.field(19);
  const DatField *const score = units_.field(46);
  if (race == nullptr || provided == nullptr || required == nullptr ||
      flags == nullptr || score == nullptr) {
    return false;
  }
  for (std::size_t type = 0; type < traits.size(); ++type) {
    if (!race->value(type, traits[type].race_flags) ||
        !provided->value(type, traits[type].supply_provided) ||
        !required->value(type, traits[type].supply_required) ||
        !flags->value(type, traits[type].dat_flags)) {
      return false;
    }
    if (retail_assets_) {
      std::uint16_t retail_score{};
      if (!score->value(type, retail_score)) {
        return false;
      }
      traits[type].score_value = static_cast<std::uint8_t>(
          (std::min)(retail_score, std::uint16_t{UINT8_MAX}));
    } else if (!score->value(type, traits[type].score_value)) {
      return false;
    }
  }
  return true;
}

std::string CoreDataSet::image_grp_path(const std::uint16_t image_id) const {
  const DatField *const names = images_.field(0);
  if (names == nullptr) {
    return {};
  }
  std::uint32_t string_id{};
  if (!names->value(image_id, string_id)) {
    return {};
  }
  const StringTableView strings{image_strings_.data(), image_strings_.size()};
  const std::string_view relative =
      strings.one_based(static_cast<std::uint16_t>(string_id));
  if (relative.empty()) {
    return {};
  }
  return std::string{"unit\\"} + std::string{relative};
}

std::string
CoreDataSet::image_damage_overlay_path(const std::uint16_t image_id) const {
  // images.dat field 11 is dword_729E18. CUnitGUI.cpp::sub_42BBA0 and
  // CSprite.cpp::sub_41CC30/sub_41CD30 use this LO table for the 22 possible
  // building damage attachment points.
  const DatField *const locations = images_.field(11U);
  std::uint32_t string_id{};
  if (locations == nullptr || !locations->value(image_id, string_id) ||
      string_id == 0U || string_id > 0xFFFFU) {
    return {};
  }
  const StringTableView strings{image_strings_.data(), image_strings_.size()};
  const std::string_view relative =
      strings.one_based(static_cast<std::uint16_t>(string_id));
  return relative.empty()
             ? std::string{}
             : std::string{"unit\\"} + std::string{relative};
}

std::string
CoreDataSet::image_special_overlay_path(const std::uint16_t image_id) const {
  // images.dat field 12 backs dword_55A918 in CImage.cpp. Opcode 0x40 uses
  // that per-image LO table to position resource-source plume images.
  const DatField *const locations = images_.field(12U);
  std::uint32_t string_id{};
  if (locations == nullptr || !locations->value(image_id, string_id) ||
      string_id == 0U || string_id > 0xFFFFU) {
    return {};
  }
  const StringTableView strings{image_strings_.data(), image_strings_.size()};
  const std::string_view relative =
      strings.one_based(static_cast<std::uint16_t>(string_id));
  return relative.empty()
             ? std::string{}
             : std::string{"unit\\"} + std::string{relative};
}

bool CoreDataSet::image_iscript_id(const std::uint16_t image_id,
                                   std::uint16_t &script_id) const noexcept {
  const DatField *const scripts = images_.field(8);
  std::uint32_t value{};
  if (scripts == nullptr || !scripts->value(image_id, value) ||
      value > 0xFFFFU) {
    return false;
  }
  script_id = static_cast<std::uint16_t>(value);
  return true;
}

bool CoreDataSet::image_render_traits(
    const std::uint16_t image_id, ImageRenderTraits &traits) const noexcept {
  // CImage construction at original VA 0x00410F60 sets image flag 0x08 from
  // byte_72D570, images.dat field 1.  CImage::sub_411510 (0x00411510) gates
  // directional-frame selection on that flag.  Field 2 (byte_728958) is the
  // separate banking capability consumed by sub_411CC0 (0x00411CC0).
  const DatField *const graphics_turns = images_.field(1);
  const DatField *const draw_function = images_.field(6);
  const DatField *const remapping = images_.field(7);
  std::uint8_t turns{};
  if (graphics_turns == nullptr || draw_function == nullptr ||
      remapping == nullptr || !graphics_turns->value(image_id, turns) ||
      !draw_function->value(image_id, traits.draw_function) ||
      !remapping->value(image_id, traits.remapping)) {
    return false;
  }
  traits.graphics_turns = turns != 0;
  return true;
}

bool CoreDataSet::unit_image_id(const std::uint16_t unit_type,
                                std::uint16_t &image_id) const noexcept {
  const DatField *const unit_graphics = units_.field(0);
  const DatField *const flingy_sprite = flingy_.field(0);
  const DatField *const sprite_image = sprites_.field(0);
  const DatField *const image_names = images_.field(0);
  std::uint8_t flingy_id{};
  std::uint16_t sprite_id{};
  return unit_graphics != nullptr && flingy_sprite != nullptr &&
         sprite_image != nullptr && image_names != nullptr &&
         unit_graphics->value(unit_type, flingy_id) &&
         flingy_sprite->value(flingy_id, sprite_id) &&
         sprite_image->value(sprite_id, image_id) &&
         image_id < image_names->element_count();
}

bool CoreDataSet::unit_construction_image_id(
    const std::uint16_t unit_type, std::uint16_t &image_id) const noexcept {
  // CUnitGUI.cpp::sub_42BA80 reads dword_8DE720[unit_type] when its second
  // argument is one. dword_8DE720 is units.dat field 4, the replacement
  // CImage used while a Zerg CUnit is under construction. A zero entry makes
  // the original routine fall back to the unit's normal sprite image.
  const DatField *const construction_images = units_.field(4U);
  const DatField *const image_names = images_.field(0U);
  std::uint32_t value{};
  if (construction_images == nullptr || image_names == nullptr ||
      !construction_images->value(unit_type, value) || value == 0U ||
      value >= image_names->element_count()) {
    return false;
  }
  image_id = static_cast<std::uint16_t>(value);
  return true;
}

bool CoreDataSet::unit_selection_circle(
    const std::uint16_t unit_type, std::uint16_t &image_id,
    std::int8_t &y_offset) const noexcept {
  // CUnitGUI.cpp::sub_42B340 calls CSprite::sub_41C550 when a unit enters
  // the selection set. CImage.cpp::sub_410260 follows
  // units.dat[0] -> flingy.dat[0] -> sprites.dat[3], adds image 532, and
  // applies sprites.dat[4] as the attached image's signed Y offset.
  const DatField *const unit_flingy = units_.field(0U);
  const DatField *const flingy_sprites = flingy_.field(0U);
  const DatField *const circle_images = sprites_.field(3U);
  const DatField *const circle_y_offsets = sprites_.field(4U);
  const DatField *const image_names = images_.field(0U);
  std::uint8_t flingy_id{};
  std::uint16_t sprite_id{};
  std::uint8_t circle_image_offset{};
  std::uint8_t raw_y_offset{};
  if (unit_flingy == nullptr || flingy_sprites == nullptr ||
      circle_images == nullptr || circle_y_offsets == nullptr ||
      image_names == nullptr ||
      !unit_flingy->value(unit_type, flingy_id) ||
      !flingy_sprites->value(flingy_id, sprite_id)) {
    return false;
  }
  const std::size_t circle_index =
      retail_assets_
          ? (sprite_id >= 130U ? static_cast<std::size_t>(sprite_id - 130U)
                               : SIZE_MAX)
          : sprite_id;
  if (!circle_images->value(circle_index, circle_image_offset) ||
      !circle_y_offsets->value(circle_index, raw_y_offset)) {
    return false;
  }
  image_id = static_cast<std::uint16_t>(
      (retail_assets_ ? 561U : 532U) + circle_image_offset);
  y_offset = static_cast<std::int8_t>(raw_y_offset);
  return image_id < image_names->element_count();
}

bool CoreDataSet::unit_sprite_elevation(
    const std::uint16_t unit_type, std::uint8_t &elevation) const noexcept {
  // CUnitInit.cpp::sub_42E400 copies byte_8E1368[unit_type] to
  // CSprite+0x11. byte_8E1368 is units.dat field 9: ground units in the
  // licensed beta use elevation 3 while airborne units such as the Overlord
  // use elevation 15.
  const DatField *const elevations = units_.field(9U);
  return elevations != nullptr && elevations->value(unit_type, elevation);
}

bool CoreDataSet::weapon_image_id(const std::uint16_t weapon_type,
                                  std::uint16_t &image_id) const noexcept {
  // weapons.dat field 1 backs dword_8E7F98, the flingy graphic created by
  // CUnitCombat.cpp::sub_426650 for IScript opcode 0x2B. Follow the same
  // flingy -> sprite -> image chain used by unit graphics.
  const DatField *const weapon_graphics = weapons_.field(1);
  const DatField *const flingy_sprite = flingy_.field(0);
  const DatField *const sprite_image = sprites_.field(0);
  const DatField *const image_names = images_.field(0);
  std::uint32_t flingy_id{};
  std::uint16_t sprite_id{};
  return weapon_graphics != nullptr && flingy_sprite != nullptr &&
         sprite_image != nullptr && image_names != nullptr &&
         weapon_graphics->value(weapon_type, flingy_id) &&
         flingy_id < flingy_sprite->element_count() &&
         flingy_id != 0U &&
         flingy_sprite->value(flingy_id, sprite_id) &&
         sprite_image->value(sprite_id, image_id) &&
         image_id < image_names->element_count();
}

bool CoreDataSet::unit_placement_size(const std::uint16_t unit_type,
                                      std::uint16_t &width,
                                      std::uint16_t &height) const noexcept {
  // units.dat field 33 backs dword_8DFFB0. CUnitGUI.cpp::sub_42B1D0 at
  // 0x0042B1D0 passes its low/high words as the unit's selection extent.
  const DatField *const placement = units_.field(33);
  return placement != nullptr &&
         placement->value(static_cast<std::size_t>(unit_type) * 2U, width) &&
         placement->value(static_cast<std::size_t>(unit_type) * 2U + 1U,
                          height) &&
         width != 0 && height != 0;
}

bool CoreDataSet::unit_collision_extents(const std::uint16_t unit_type,
                                         std::uint16_t &left,
                                         std::uint16_t &top,
                                         std::uint16_t &right,
                                         std::uint16_t &bottom) const noexcept {
  // units.dat field 35 is the four-word collision rectangle used by the
  // CUnit placement search. StarCraft.exe place_unit.cpp::sub_47FBF0 reads the
  // matching left/top and right/bottom arrays while settling freshly created
  // units.
  const DatField *const extents = units_.field(35);
  const std::size_t base = static_cast<std::size_t>(unit_type) * 4U;
  return extents != nullptr && extents->value(base, left) &&
         extents->value(base + 1U, top) && extents->value(base + 2U, right) &&
         extents->value(base + 3U, bottom);
}

bool CoreDataSet::unit_addon_position(const std::uint16_t unit_type,
                                      std::int16_t &x,
                                      std::int16_t &y) const noexcept {
  // units.dat field 34 backs dword_8DF730. The file stores its 98 packed
  // x/y pairs for building IDs 106..203; placebox.cpp::sub_482050 subtracts
  // this offset to recover the owning building's tile origin.
  constexpr std::uint16_t first_building = 106;
  const DatField *const positions = units_.field(34);
  if (positions == nullptr || unit_type < first_building) {
    return false;
  }
  const std::size_t index =
      static_cast<std::size_t>(unit_type - first_building) * 2U;
  return positions->value(index, x) && positions->value(index + 1U, y);
}

bool CoreDataSet::unit_build_time(const std::uint16_t unit_type,
                                  std::uint16_t &build_time) const noexcept {
  // units.dat field 39 backs word_8DEFF0. CUnitBuild.cpp::sub_423670 at
  // 0x00423670 derives total production time as word_8DEFF0[type] >> 1.
  const DatField *const build_times = units_.field(39);
  return build_times != nullptr && build_times->value(unit_type, build_time) &&
         build_time != 0;
}

bool CoreDataSet::unit_movement_traits(
    const std::uint16_t unit_type,
    FlingyMovementTraits &traits) const noexcept {
  // CFlingy.cpp::sub_405FB0 at 0x00405FB0 initializes the live CFlingy from
  // these five flingy.dat fields. The beta runtime doubles DAT top speed and
  // acceleration, doubles a turn value below 64, and clamps larger turns to
  // 127. Preserve those conversions for beta data; retail DAT values already
  // use the runtime scale.
  const DatField *const unit_graphics = units_.field(0);
  const DatField *const top_speeds = flingy_.field(1);
  const DatField *const accelerations = flingy_.field(2);
  const DatField *const turn_speeds = flingy_.field(retail_assets_ ? 4U : 3U);
  const DatField *const direction_thresholds =
      flingy_.field(retail_assets_ ? 5U : 4U);
  const DatField *const movement_controls =
      flingy_.field(retail_assets_ ? 6U : 5U);
  std::uint8_t flingy_id{};
  std::uint32_t raw_top_speed{};
  std::uint16_t raw_acceleration{};
  std::uint8_t raw_turn_speed{};
  if (unit_graphics == nullptr || top_speeds == nullptr ||
      accelerations == nullptr || turn_speeds == nullptr ||
      direction_thresholds == nullptr || movement_controls == nullptr ||
      !unit_graphics->value(unit_type, flingy_id) ||
      !top_speeds->value(flingy_id, raw_top_speed) ||
      !accelerations->value(flingy_id, raw_acceleration) ||
      !turn_speeds->value(flingy_id, raw_turn_speed) ||
      !direction_thresholds->value(flingy_id, traits.direction_threshold) ||
      !movement_controls->value(flingy_id, traits.movement_control) ||
      (!retail_assets_ && (raw_top_speed > UINT32_MAX / 2U ||
                           raw_acceleration > UINT16_MAX / 2U))) {
    return false;
  }
  traits.top_speed = retail_assets_ ? raw_top_speed : raw_top_speed * 2U;
  traits.acceleration =
      retail_assets_ ? raw_acceleration
                     : static_cast<std::uint16_t>(raw_acceleration * 2U);
  traits.turn_speed =
      retail_assets_ ? raw_turn_speed
                     : (raw_turn_speed < 64U
                            ? static_cast<std::uint8_t>(raw_turn_speed * 2U)
                            : static_cast<std::uint8_t>(127U));
  return traits.top_speed != 0 && traits.acceleration != 0 &&
         traits.turn_speed != 0;
}

bool CoreDataSet::unit_is_building(const std::uint16_t unit_type,
                                   bool &is_building) const noexcept {
  // units.dat field 19 is dword_8E1450. collide.cpp::sub_418900 and the unit
  // constructors consistently use bit 0 as the building/static footprint
  // discriminator.
  const DatField *const flags = units_.field(19);
  std::uint32_t value{};
  if (flags == nullptr || !flags->value(unit_type, value)) {
    return false;
  }
  is_building = (value & 1U) != 0;
  return true;
}

bool CoreDataSet::unit_simulation_traits(
    const std::uint16_t unit_type,
    UnitSimulationTraits &traits) const noexcept {
  // The field identities below are independently visible in the recovered
  // runtime: CUnitBuild.cpp::sub_422160 reads dword_8DD540 as maximum life;
  // CUnitCombat.cpp::sub_4267E0 reads word_8E8578 as weapon damage; and
  // sub_427870 reads dword_8E83A0/byte_8E7F50 as range/cooldown.
  const DatField *const maximum_life = units_.field(8);
  const DatField *const shields_enabled = units_.field(6);
  const DatField *const maximum_shields = units_.field(7);
  const DatField *const ground_weapons = units_.field(17);
  const DatField *const air_weapons = units_.field(18);
  const DatField *const flags = units_.field(19);
  const DatField *const seek_ranges = units_.field(20);
  const DatField *const sight_ranges = units_.field(21);
  const DatField *const armor_upgrades = units_.field(22);
  const DatField *const armor_classes = units_.field(23);
  const DatField *const armor = units_.field(24);
  const DatField *const cargo_required = units_.field(44);
  const DatField *const cargo_provided = units_.field(45);
  const DatField *const mineral_costs = units_.field(37);
  const DatField *const gas_costs = units_.field(38);
  const DatField *const build_times = units_.field(39);
  std::uint16_t raw_shields{};
  std::uint8_t shield_enable{};
  std::uint8_t raw_seek_range{};
  std::uint8_t sight_range{};
  if (maximum_life == nullptr || shields_enabled == nullptr ||
      maximum_shields == nullptr ||
      ground_weapons == nullptr ||
      air_weapons == nullptr ||
      flags == nullptr || seek_ranges == nullptr || sight_ranges == nullptr ||
      armor_upgrades == nullptr ||
      armor_classes == nullptr || armor == nullptr ||
      cargo_required == nullptr || cargo_provided == nullptr ||
      mineral_costs == nullptr || gas_costs == nullptr ||
      build_times == nullptr ||
      !maximum_life->value(unit_type, traits.max_hit_points) ||
      !shields_enabled->value(unit_type, shield_enable) ||
      !maximum_shields->value(unit_type, raw_shields) ||
      !ground_weapons->value(unit_type, traits.ground_weapon) ||
      !air_weapons->value(unit_type, traits.air_weapon) ||
      !flags->value(unit_type, traits.dat_flags) ||
      !seek_ranges->value(unit_type, raw_seek_range) ||
      !sight_ranges->value(unit_type, sight_range) ||
      !armor_upgrades->value(unit_type, traits.armor_upgrade) ||
      !armor_classes->value(unit_type, traits.armor_class) ||
      !armor->value(unit_type, traits.armor) ||
      !cargo_required->value(unit_type, traits.cargo_space_required) ||
      !cargo_provided->value(unit_type, traits.cargo_space_provided) ||
      !mineral_costs->value(unit_type, traits.mineral_cost) ||
      !gas_costs->value(unit_type, traits.gas_cost) ||
      !build_times->value(unit_type, traits.build_time) ||
      traits.max_hit_points == 0) {
    return false;
  }
  traits.max_shield_points =
      shield_enable != 0U ? static_cast<std::uint32_t>(raw_shields) << 8U : 0U;
  const DatField *const weapon_graphics = weapons_.field(1U);
  if (weapon_graphics == nullptr) {
    return false;
  }
  traits.has_ground_weapon =
      traits.ground_weapon < weapon_graphics->element_count();
  traits.has_air_weapon =
      traits.air_weapon < weapon_graphics->element_count();
  // CUnitInit.cpp::sub_42DD70 raises units.dat's acquisition range to each
  // weapon's range in tiles, then clamps it below the unit sight radius.
  traits.seek_range = static_cast<std::uint8_t>((std::min)(
      static_cast<unsigned>(raw_seek_range),
      sight_range == 0U ? 0U : static_cast<unsigned>(sight_range - 1U)));
  traits.sight_range = sight_range;
  if (!traits.has_ground_weapon && !traits.has_air_weapon) {
    return true;
  }
  const DatField *const maximum_ranges = weapons_.field(5);
  const DatField *const weapon_upgrades = weapons_.field(6);
  const DatField *const damage_classes = weapons_.field(7);
  const DatField *const damages = weapons_.field(14);
  const DatField *const damage_factors = weapons_.field(15);
  const DatField *const cooldowns = weapons_.field(16);
  if (maximum_ranges == nullptr || weapon_upgrades == nullptr ||
      damage_classes == nullptr || damages == nullptr ||
      damage_factors == nullptr || cooldowns == nullptr) {
    return false;
  }
  const auto decode_weapon = [&](const std::uint8_t weapon,
                                 std::uint32_t &range,
                                 std::uint8_t &upgrade,
                                 std::uint8_t &damage_class,
                                 std::uint16_t &damage,
                                 std::uint16_t &factor,
                                 std::uint8_t &cooldown) noexcept {
    return maximum_ranges->value(weapon, range) &&
           weapon_upgrades->value(weapon, upgrade) &&
           damage_classes->value(weapon, damage_class) &&
           damages->value(weapon, damage) &&
           damage_factors->value(weapon, factor) &&
           cooldowns->value(weapon, cooldown) && damage != 0U &&
           cooldown != 0U;
  };
  if (traits.has_ground_weapon &&
      !decode_weapon(traits.ground_weapon, traits.ground_weapon_range,
                     traits.ground_weapon_upgrade,
                     traits.ground_weapon_damage_class,
                     traits.ground_weapon_damage,
                     traits.ground_weapon_damage_factor,
                     traits.ground_weapon_cooldown)) {
    return false;
  }
  if (traits.has_air_weapon &&
      !decode_weapon(traits.air_weapon, traits.air_weapon_range,
                       traits.air_weapon_upgrade,
                       traits.air_weapon_damage_class,
                       traits.air_weapon_damage,
                       traits.air_weapon_damage_factor,
                       traits.air_weapon_cooldown)) {
    return false;
  }
  const std::uint32_t weapon_tiles = (std::max)(traits.ground_weapon_range,
                                                traits.air_weapon_range) >> 5U;
  traits.seek_range = static_cast<std::uint8_t>((std::min)(
      (std::max)(static_cast<unsigned>(traits.seek_range), weapon_tiles),
      sight_range == 0U ? 0U : static_cast<unsigned>(sight_range - 1U)));
  return true;
}

bool CoreDataSet::technology_research_traits(
    const std::uint16_t technology,
    TechnologyResearchTraits &traits) const noexcept {
  // CUnitUpgrade.cpp::sub_4467E0 reads word_8D9178/word_8D91E8 for cost and
  // stores word_8D9258[technology] >> 1 as the live research timer.
  const DatField *const minerals = technologies_.field(0);
  const DatField *const gas = technologies_.field(1);
  const DatField *const time = technologies_.field(2);
  const DatField *const energy = technologies_.field(3);
  return minerals != nullptr && gas != nullptr && time != nullptr &&
         energy != nullptr &&
         minerals->value(technology, traits.mineral_cost) &&
         gas->value(technology, traits.gas_cost) &&
         time->value(technology, traits.research_time) &&
         energy->value(technology, traits.energy_cost);
}

bool CoreDataSet::weapon_simulation_traits(
    const std::uint16_t weapon,
    WeaponSimulationTraits &traits) const noexcept {
  const DatField *const graphics = weapons_.field(1);
  const DatField *const range = weapons_.field(5);
  const DatField *const upgrade = weapons_.field(6);
  const DatField *const damage_class = weapons_.field(7);
  const DatField *const behavior = weapons_.field(8);
  const DatField *const removal_timer = weapons_.field(9);
  const DatField *const explosion_type = weapons_.field(10);
  const DatField *const inner = weapons_.field(11);
  const DatField *const median = weapons_.field(12);
  const DatField *const outer = weapons_.field(13);
  const DatField *const damage = weapons_.field(14);
  const DatField *const factor = weapons_.field(15);
  const DatField *const cooldown = weapons_.field(16);
  const DatField *const projectile_count = weapons_.field(17);
  const DatField *const forward_offset = weapons_.field(20);
  const DatField *const vertical_offset = weapons_.field(21);
  const DatField *const flingy_speeds = flingy_.field(1);
  std::uint32_t flingy_id{};
  std::uint32_t raw_top_speed{};
  const bool base_ready =
      graphics != nullptr && weapon < graphics->element_count() &&
      range != nullptr &&
      upgrade != nullptr &&
         damage_class != nullptr && inner != nullptr && median != nullptr &&
         outer != nullptr && damage != nullptr && factor != nullptr &&
         cooldown != nullptr && behavior != nullptr && removal_timer != nullptr &&
         explosion_type != nullptr && projectile_count != nullptr &&
         forward_offset != nullptr && vertical_offset != nullptr &&
         graphics->value(weapon, flingy_id) &&
         range->value(weapon, traits.maximum_range) &&
         upgrade->value(weapon, traits.upgrade) &&
         damage_class->value(weapon, traits.damage_class) &&
         behavior->value(weapon, traits.behavior) &&
         removal_timer->value(weapon, traits.removal_timer) &&
         explosion_type->value(weapon, traits.explosion_type) &&
         inner->value(weapon, traits.inner_splash_radius) &&
         median->value(weapon, traits.median_splash_radius) &&
         outer->value(weapon, traits.outer_splash_radius) &&
         damage->value(weapon, traits.damage) &&
         factor->value(weapon, traits.damage_factor) &&
         cooldown->value(weapon, traits.cooldown) &&
         projectile_count->value(weapon, traits.projectile_count) &&
         forward_offset->value(weapon, traits.forward_offset) &&
         vertical_offset->value(weapon, traits.vertical_offset);
  if (!base_ready || flingy_speeds == nullptr ||
      flingy_id >= flingy_speeds->element_count()) {
    return false;
  }
  // weapons.dat field 1 is the CFlingy type passed to sub_41D8B0 by
  // CBullet::sub_402940. Zero is the original no-projectile sentinel used by
  // melee weapons. Nonzero beta projectile flingies use the same doubled
  // fixed-speed conversion as ordinary unit flingies in sub_405FB0; retail
  // values are already stored at runtime scale.
  traits.has_projectile_graphic = flingy_id != 0U;
  if (traits.has_projectile_graphic) {
    if (!flingy_speeds->value(flingy_id, raw_top_speed) ||
        (!retail_assets_ && raw_top_speed > UINT32_MAX / 2U)) {
      return false;
    }
    traits.projectile_top_speed =
        retail_assets_ ? raw_top_speed : raw_top_speed * 2U;
  }
  return traits.projectile_count == 1U || traits.projectile_count == 2U;
}

bool CoreDataSet::weapon_display_traits(
    const std::uint16_t weapon, std::uint16_t &label_string_id,
    std::uint16_t &icon) const noexcept {
  const DatField *const labels = weapons_.field(0);
  const DatField *const icons = weapons_.field(23);
  return labels != nullptr && weapon < labels->element_count() &&
         icons != nullptr &&
         labels->value(weapon, label_string_id) &&
         icons->value(weapon, icon);
}

bool CoreDataSet::order_spell_traits(const std::uint16_t order,
                                     std::uint8_t &weapon,
                                     std::uint8_t &technology,
                                     std::uint8_t &animation) const noexcept {
  const std::size_t first = retail_assets_ ? 13U : 10U;
  const DatField *const weapons = orders_.field(first);
  const DatField *const technologies = orders_.field(first + 1U);
  const DatField *const animations = orders_.field(first + 2U);
  return weapons != nullptr && order < weapons->element_count() &&
         technologies != nullptr &&
         animations != nullptr && weapons->value(order, weapon) &&
         technologies->value(order, technology) &&
         animations->value(order, animation);
}

bool CoreDataSet::upgrade_research_traits(
    const std::uint16_t upgrade,
    UpgradeResearchTraits &traits) const noexcept {
  // CUnitUpgrade.cpp::sub_446740/sub_446790 and sub_4469E0 use these exact
  // base/factor arrays with the player's current byte_51791F level. The last
  // byte field, byte_8E1840, is the original maximum level.
  const DatField *const mineral_cost = upgrades_.field(0);
  const DatField *const mineral_factor = upgrades_.field(1);
  const DatField *const gas_cost = upgrades_.field(2);
  const DatField *const gas_factor = upgrades_.field(3);
  const DatField *const research_time = upgrades_.field(4);
  const DatField *const time_factor = upgrades_.field(5);
  const DatField *const maximum_level = upgrades_.field(10);
  return mineral_cost != nullptr && mineral_factor != nullptr &&
         gas_cost != nullptr && gas_factor != nullptr &&
         research_time != nullptr && time_factor != nullptr &&
         maximum_level != nullptr &&
         mineral_cost->value(upgrade, traits.mineral_cost) &&
         mineral_factor->value(upgrade, traits.mineral_factor) &&
         gas_cost->value(upgrade, traits.gas_cost) &&
         gas_factor->value(upgrade, traits.gas_factor) &&
         research_time->value(upgrade, traits.research_time) &&
         time_factor->value(upgrade, traits.time_factor) &&
         maximum_level->value(upgrade, traits.maximum_level);
}

bool CoreDataSet::upgrade_display_traits(
    const std::uint16_t upgrade, std::uint16_t &label_string_id,
    std::uint16_t &icon) const noexcept {
  const DatField *const icons = upgrades_.field(7);
  const DatField *const labels = upgrades_.field(8);
  return icons != nullptr && upgrade < icons->element_count() &&
         labels != nullptr &&
         icons->value(upgrade, icon) &&
         labels->value(upgrade, label_string_id);
}

std::string CoreDataSet::unit_portrait_path(const std::uint16_t unit_type,
                                            const std::uint8_t owner,
                                            const bool talking,
                                            const std::uint8_t variant) const {
  // statport.cpp::sub_4AA880 at 0x004AA880 reads units.dat field 36
  // (word_8DED60), maps portrait 79 to the owner's race variant, selects the
  // idle/talking prefix from portdata.dat fields 0/1, and appends 0..4/0..2.
  const DatField *const unit_portraits = units_.field(36);
  const DatField *const names = portraits_.field(talking ? 1U : 0U);
  std::uint16_t portrait{};
  std::uint32_t string_id{};
  if (unit_portraits == nullptr || names == nullptr ||
      !unit_portraits->value(unit_type, portrait)) {
    return {};
  }
  if (portrait == 79U) {
    portrait = static_cast<std::uint16_t>(portrait + owner);
  }
  if (!names->value(portrait, string_id) || string_id > UINT16_MAX) {
    return {};
  }
  const StringTableView strings{portrait_strings_.data(),
                                portrait_strings_.size()};
  const std::string_view prefix =
      strings.one_based(static_cast<std::uint16_t>(string_id));
  if (prefix.empty()) {
    return {};
  }
  return std::string{"portrait\\"} + std::string{prefix} +
         static_cast<char>('0' + variant) + ".smk";
}

std::string CoreDataSet::map_archive_path(const std::uint16_t map_id) const {
  const DatField *const names = mapdata_.field(0);
  std::uint32_t string_id{};
  if (names == nullptr || !names->value(map_id, string_id) ||
      string_id > 0xFFFFU) {
    return {};
  }
  const StringTableView strings{map_strings_.data(), map_strings_.size()};
  const std::string_view relative =
      strings.one_based(static_cast<std::uint16_t>(string_id));
  if (relative.empty()) {
    return {};
  }
  return std::string{"maps\\"} + std::string{relative};
}

} // namespace starcraft::data
