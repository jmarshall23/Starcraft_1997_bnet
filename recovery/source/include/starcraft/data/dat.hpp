#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "starcraft/lang/count.hpp"

namespace starcraft::runtime {
class StormModule;
}

namespace starcraft::data {

struct DatFieldShape {
  std::uint16_t element_width{};
  std::uint16_t element_count{};
};

class DatField final {
 public:
  [[nodiscard]] std::uint16_t element_width() const noexcept;
  [[nodiscard]] std::uint16_t element_count() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;

  template <typename T>
  [[nodiscard]] bool value(const std::size_t index, T& output) const noexcept {
    if (sizeof(T) != element_width_ || index >= element_count_) {
      return false;
    }
    const auto* source = bytes_.data() + index * element_width_;
    std::uint8_t* destination = reinterpret_cast<std::uint8_t*>(&output);
    for (std::size_t byte = 0; byte < sizeof(T); ++byte) {
      destination[byte] = source[byte];
    }
    return true;
  }

 private:
  friend class DatTable;
  std::uint16_t element_width_{};
  std::uint16_t element_count_{};
  std::vector<std::uint8_t> bytes_{};
};

class DatTable final {
 public:
  [[nodiscard]] bool unpack(
      const std::vector<std::uint8_t>& payload,
      const DatFieldShape* shapes,
      std::size_t shape_count) noexcept;

  [[nodiscard]] const DatField* field(std::size_t index) const noexcept;
  [[nodiscard]] std::size_t field_count() const noexcept;
  [[nodiscard]] std::size_t payload_size() const noexcept;

 private:
  std::vector<DatField> fields_{};
  std::size_t payload_size_{};
};

class StringTableView final {
 public:
  StringTableView(const std::uint8_t* bytes, std::size_t size) noexcept;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] std::uint16_t count() const noexcept;
  [[nodiscard]] std::string_view one_based(std::uint16_t string_id) const noexcept;

 private:
  const std::uint8_t* bytes_{};
  std::size_t size_{};
  std::uint16_t count_{};
  bool valid_{};
};

struct ImageRenderTraits {
  std::uint8_t draw_function{};
  std::uint8_t remapping{};
  bool graphics_turns{};
};

struct FlingyMovementTraits {
  std::uint32_t top_speed{};
  std::uint16_t acceleration{};
  std::uint8_t turn_speed{};
  std::uint8_t direction_threshold{};
  std::uint8_t movement_control{};
};

struct UnitSimulationTraits {
  std::uint32_t max_hit_points{};
  std::uint32_t max_shield_points{};
  std::uint32_t dat_flags{};
  std::uint32_t ground_weapon_range{};
  std::uint32_t air_weapon_range{};
  std::uint16_t ground_weapon_damage{};
  std::uint16_t ground_weapon_damage_factor{};
  std::uint16_t air_weapon_damage{};
  std::uint16_t air_weapon_damage_factor{};
  std::uint16_t mineral_cost{};
  std::uint16_t gas_cost{};
  std::uint16_t build_time{};
  std::uint8_t armor{};
  std::uint8_t armor_class{};
  std::uint8_t armor_upgrade{46U};
  std::uint8_t ground_weapon{};
  std::uint8_t air_weapon{};
  std::uint8_t ground_weapon_upgrade{46U};
  std::uint8_t air_weapon_upgrade{46U};
  std::uint8_t ground_weapon_damage_class{};
  std::uint8_t ground_weapon_cooldown{};
  std::uint8_t air_weapon_damage_class{};
  std::uint8_t air_weapon_cooldown{};
  std::uint8_t seek_range{};
  std::uint8_t sight_range{};
  std::uint8_t cargo_space_required{};
  std::uint8_t cargo_space_provided{};
  bool has_ground_weapon{};
  bool has_air_weapon{};
};

struct WeaponSimulationTraits {
  std::uint32_t maximum_range{};
  std::uint32_t projectile_top_speed{};
  std::uint16_t inner_splash_radius{};
  std::uint16_t median_splash_radius{};
  std::uint16_t outer_splash_radius{};
  std::uint16_t damage{};
  std::uint16_t damage_factor{};
  std::uint8_t upgrade{46U};
  std::uint8_t damage_class{};
  std::uint8_t cooldown{};
  std::uint8_t behavior{};
  std::uint8_t removal_timer{};
  std::uint8_t explosion_type{};
  std::uint8_t projectile_count{1U};
  std::uint8_t forward_offset{};
  std::uint8_t vertical_offset{};
  bool has_projectile_graphic{};
};

struct TechnologyResearchTraits {
  std::uint16_t mineral_cost{};
  std::uint16_t gas_cost{};
  std::uint16_t research_time{};
  std::uint16_t energy_cost{};
};

struct UpgradeResearchTraits {
  std::uint16_t mineral_cost{};
  std::uint16_t mineral_factor{};
  std::uint16_t gas_cost{};
  std::uint16_t gas_factor{};
  std::uint16_t research_time{};
  std::uint16_t time_factor{};
  std::uint8_t maximum_level{};
};

class CoreDataSet final {
 public:
  [[nodiscard]] bool load(runtime::StormModule& storm) noexcept;

  [[nodiscard]] const DatTable& units() const noexcept;
  [[nodiscard]] const DatTable& weapons() const noexcept;
  [[nodiscard]] const DatTable& flingy() const noexcept;
  [[nodiscard]] const DatTable& sprites() const noexcept;
  [[nodiscard]] const DatTable& images() const noexcept;
  [[nodiscard]] const DatTable& technologies() const noexcept;
  [[nodiscard]] const DatTable& orders() const noexcept;
  [[nodiscard]] const DatTable& upgrades() const noexcept;
  [[nodiscard]] const DatTable& mapdata() const noexcept;
  [[nodiscard]] const DatTable& portraits() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& image_strings() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& iscript() const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& map_strings() const noexcept;
  [[nodiscard]] std::size_t total_payload_bytes() const noexcept;
  [[nodiscard]] const std::string& failed_asset() const noexcept;

  [[nodiscard]] bool extract_unit_traits(starcraft::lang::UnitTraitsTable& traits) const noexcept;
  [[nodiscard]] std::string image_grp_path(std::uint16_t image_id) const;
  [[nodiscard]] std::string
  image_special_overlay_path(std::uint16_t image_id) const;
  [[nodiscard]] bool image_iscript_id(
      std::uint16_t image_id,
      std::uint16_t& script_id) const noexcept;
  [[nodiscard]] bool image_render_traits(
      std::uint16_t image_id,
      ImageRenderTraits& traits) const noexcept;
  [[nodiscard]] bool unit_image_id(
      std::uint16_t unit_type,
      std::uint16_t& image_id) const noexcept;
  [[nodiscard]] bool unit_construction_image_id(
      std::uint16_t unit_type,
      std::uint16_t& image_id) const noexcept;
  [[nodiscard]] bool unit_selection_circle(
      std::uint16_t unit_type, std::uint16_t& image_id,
      std::int8_t& y_offset) const noexcept;
  [[nodiscard]] bool unit_sprite_elevation(
      std::uint16_t unit_type,
      std::uint8_t& elevation) const noexcept;
  [[nodiscard]] bool weapon_image_id(
      std::uint16_t weapon_type,
      std::uint16_t& image_id) const noexcept;
  [[nodiscard]] bool unit_placement_size(
      std::uint16_t unit_type,
      std::uint16_t& width,
      std::uint16_t& height) const noexcept;
  [[nodiscard]] bool unit_collision_extents(
      std::uint16_t unit_type,
      std::uint16_t& left,
      std::uint16_t& top,
      std::uint16_t& right,
      std::uint16_t& bottom) const noexcept;
  [[nodiscard]] bool unit_addon_position(
      std::uint16_t unit_type,
      std::int16_t& x,
      std::int16_t& y) const noexcept;
  [[nodiscard]] bool unit_build_time(
      std::uint16_t unit_type,
      std::uint16_t& build_time) const noexcept;
  [[nodiscard]] bool unit_movement_traits(
      std::uint16_t unit_type,
      FlingyMovementTraits& traits) const noexcept;
  [[nodiscard]] bool unit_is_building(
      std::uint16_t unit_type,
      bool& is_building) const noexcept;
  [[nodiscard]] bool unit_simulation_traits(
      std::uint16_t unit_type,
      UnitSimulationTraits& traits) const noexcept;
  [[nodiscard]] bool technology_research_traits(
      std::uint16_t technology,
      TechnologyResearchTraits& traits) const noexcept;
  [[nodiscard]] bool weapon_simulation_traits(
      std::uint16_t weapon,
      WeaponSimulationTraits& traits) const noexcept;
  [[nodiscard]] bool order_spell_traits(
      std::uint16_t order, std::uint8_t& weapon, std::uint8_t& technology,
      std::uint8_t& animation) const noexcept;
  [[nodiscard]] bool upgrade_research_traits(
      std::uint16_t upgrade,
      UpgradeResearchTraits& traits) const noexcept;
  [[nodiscard]] std::string unit_portrait_path(
      std::uint16_t unit_type,
      std::uint8_t owner,
      bool talking,
      std::uint8_t variant) const;
  [[nodiscard]] std::string map_archive_path(std::uint16_t map_id) const;

 private:
  DatTable units_{};
  DatTable weapons_{};
  DatTable flingy_{};
  DatTable sprites_{};
  DatTable images_{};
  DatTable technologies_{};
  DatTable orders_{};
  DatTable upgrades_{};
  DatTable mapdata_{};
  DatTable portraits_{};
  std::vector<std::uint8_t> image_strings_{};
  std::vector<std::uint8_t> iscript_{};
  std::vector<std::uint8_t> map_strings_{};
  std::vector<std::uint8_t> portrait_strings_{};
  std::size_t total_payload_bytes_{};
  std::string failed_asset_{};
};

}  // namespace starcraft::data
