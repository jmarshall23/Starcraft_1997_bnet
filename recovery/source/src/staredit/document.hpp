#pragma once

#include "formats/chk_document.hpp"
#include "formats/isom_section.hpp"
#include "formats/object_sections.hpp"
#include "formats/scenario_sections.hpp"
#include "editor_layer.hpp"
#include "object_art.hpp"
#include "starcraft/data/chk.hpp"
#include "starcraft/gds/tileset.hpp"
#include "terrain/isom_catalog.hpp"
#include "terrain/isom_topology.hpp"
#include "undo/undo_stack.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace staredit {

using ScenarioFormat = formats::ChkDialect;

struct UnitMarker {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t type{};
  std::uint8_t owner{};
};

struct UnitAttributes {
  std::size_t index{};
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t type{};
  std::uint8_t owner{};
  std::uint8_t hit_points{100U};
  std::uint8_t shield_points{100U};
  std::uint8_t energy{100U};
  std::uint32_t resource_amount{};
  std::uint16_t hangar_count{};
  std::uint16_t state_flags{};
  std::uint16_t valid_state_flags{};
  std::uint16_t valid_data_flags{};
  bool has_retail_attributes{};
};

struct DoodadMarker {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t type{};
  std::uint8_t owner{};
  bool enabled{true};
};

struct SpriteMarker {
  std::uint16_t x{};
  std::uint16_t y{};
  std::uint16_t type{};
  std::uint8_t owner{};
  std::uint16_t flags{};
};

struct LocationMarker {
  std::uint32_t left{};
  std::uint32_t top{};
  std::uint32_t right{};
  std::uint32_t bottom{};
  std::uint16_t string_id{};
  std::uint16_t flags{};
  std::uint16_t slot{};
};

struct ScenarioProperties {
  std::string name{};
  std::string description{};
};

struct ScenarioForces {
  std::array<std::uint8_t, formats::force_player_count> player_force{};
  std::array<std::string, formats::force_count> names{};
  std::array<std::uint8_t, formats::force_count> flags{};
  bool supports_flags{};
};

struct ScenarioSound {
  std::uint16_t slot{};
  std::uint32_t string_id{};
  std::string path{};

  [[nodiscard]] bool operator==(const ScenarioSound& other) const noexcept {
    return slot == other.slot && string_id == other.string_id &&
           path == other.path;
  }
};

struct ScenarioAiScript {
  std::uint32_t id{};
  std::uint16_t string_id{};
  std::string name{};
};

class EditorDocument final {
 public:
  static constexpr std::size_t tile_width = 32;
  static constexpr std::size_t tile_height = 32;
  using TilePixels = std::array<std::uint32_t, tile_width * tile_height>;

  [[nodiscard]] bool load(const std::filesystem::path& path,
                          const std::filesystem::path& data_root,
                          std::wstring& error) noexcept;
  [[nodiscard]] bool create_blank(
      const std::filesystem::path& scenario_template,
      const std::filesystem::path& data_root,
      std::uint16_t width,
      std::uint16_t height,
      std::uint16_t tileset_id,
      std::string_view terrain_name,
      std::wstring& error) noexcept;

  [[nodiscard]] const std::filesystem::path& path() const noexcept;
  [[nodiscard]] const std::wstring& title() const noexcept;
  [[nodiscard]] std::uint16_t width() const noexcept;
  [[nodiscard]] std::uint16_t height() const noexcept;
  [[nodiscard]] std::uint16_t tileset_id() const noexcept;
  [[nodiscard]] const std::string& tileset_name() const noexcept;
  [[nodiscard]] ScenarioFormat format() const noexcept;
  [[nodiscard]] bool source_is_archive() const noexcept;
  [[nodiscard]] std::size_t section_count() const noexcept;
  [[nodiscard]] std::size_t unit_count() const noexcept;
  [[nodiscard]] std::size_t doodad_count() const noexcept;
  [[nodiscard]] std::size_t sprite_count() const noexcept;
  [[nodiscard]] bool has_isom() const noexcept;
  [[nodiscard]] std::size_t isom_entry_count() const noexcept;
  [[nodiscard]] bool isom_topology_ready() const noexcept;
  [[nodiscard]] std::size_t isom_group_pair_count() const noexcept;
  [[nodiscard]] std::size_t isom_terrain_type_count() const noexcept;
  [[nodiscard]] std::uint64_t isom_checksum() const noexcept;
  [[nodiscard]] std::uint64_t editor_tile_checksum() const noexcept;
  [[nodiscard]] std::uint64_t game_tile_checksum() const noexcept;
  [[nodiscard]] bool chk_round_trip_matches_source() const noexcept;
  [[nodiscard]] bool editing_ready() const noexcept;
  [[nodiscard]] bool modified() const noexcept;
  [[nodiscard]] bool can_undo() const noexcept;
  [[nodiscard]] bool can_redo() const noexcept;
  [[nodiscard]] bool begin_tile_edit() noexcept;
  [[nodiscard]] bool paint_terrain(std::uint16_t x,
                                   std::uint16_t y,
                                   std::uint16_t brush_tile_id,
                                   std::uint16_t brush_extent) noexcept;
  [[nodiscard]] bool paint_tile(std::uint16_t x,
                                std::uint16_t y,
                                std::uint16_t tile_id) noexcept;
  [[nodiscard]] bool commit_tile_edit() noexcept;
  void cancel_tile_edit() noexcept;
  [[nodiscard]] bool undo() noexcept;
  [[nodiscard]] bool redo() noexcept;
  [[nodiscard]] bool export_raw_chk(const std::filesystem::path& path,
                                    std::wstring& error) const noexcept;
  [[nodiscard]] bool save_retail_archive(const std::filesystem::path& path,
                                         std::wstring& error) noexcept;
  [[nodiscard]] const std::vector<UnitMarker>& unit_markers() const noexcept;
  [[nodiscard]] bool unit_attributes(std::size_t index,
                                     UnitAttributes& attributes) const noexcept;
  [[nodiscard]] bool unit_index_at(std::uint16_t x,
                                   std::uint16_t y,
                                   std::size_t& index) const noexcept;
  [[nodiscard]] bool update_unit(
      std::size_t index,
      const UnitAttributes& attributes) noexcept;
  [[nodiscard]] bool move_unit(std::size_t index,
                               std::uint16_t x,
                               std::uint16_t y) noexcept;
  [[nodiscard]] bool erase_unit(std::size_t index) noexcept;
  [[nodiscard]] const std::vector<DoodadMarker>& doodad_markers() const noexcept;
  [[nodiscard]] const std::vector<SpriteMarker>& sprite_markers() const noexcept;
  [[nodiscard]] const std::vector<LocationMarker>& location_markers() const noexcept;
  [[nodiscard]] bool create_location(std::uint32_t left,
                                     std::uint32_t top,
                                     std::uint32_t right,
                                     std::uint32_t bottom) noexcept;
  [[nodiscard]] bool erase_location_at(std::uint16_t x,
                                       std::uint16_t y) noexcept;
  [[nodiscard]] bool begin_fog_edit() noexcept;
  [[nodiscard]] bool paint_fog(std::uint16_t x,
                               std::uint16_t y,
                               std::uint16_t brush_extent,
                               bool obscured) noexcept;
  [[nodiscard]] bool commit_fog_edit() noexcept;
  void cancel_fog_edit() noexcept;
  [[nodiscard]] bool fog_at(std::uint16_t x,
                            std::uint16_t y) const noexcept;
  [[nodiscard]] bool player_settings(
      std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& ownership,
      std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& races) const noexcept;
  [[nodiscard]] bool set_player_settings(
      const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& ownership,
      const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& races) noexcept;
  [[nodiscard]] bool scenario_properties(
      ScenarioProperties& properties) const noexcept;
  [[nodiscard]] bool set_scenario_properties(
      const ScenarioProperties& properties) noexcept;
  [[nodiscard]] bool scenario_forces(ScenarioForces& forces) const noexcept;
  [[nodiscard]] bool set_scenario_forces(
      const ScenarioForces& forces) noexcept;
  [[nodiscard]] bool scenario_sounds(
      std::vector<ScenarioSound>& sounds) const noexcept;
  [[nodiscard]] bool set_scenario_sounds(
      const std::vector<ScenarioSound>& sounds) noexcept;
  [[nodiscard]] bool scenario_triggers(
      std::vector<formats::TriggerRecord>& triggers) const noexcept;
  [[nodiscard]] bool set_scenario_triggers(
      const std::vector<formats::TriggerRecord>& triggers) noexcept;
  [[nodiscard]] bool scenario_briefing(
      std::vector<formats::TriggerRecord>& briefing) const noexcept;
  [[nodiscard]] bool set_scenario_briefing(
      const std::vector<formats::TriggerRecord>& briefing) noexcept;
  [[nodiscard]] bool trigger_ai_scripts(
      std::vector<ScenarioAiScript>& scripts) const noexcept;
  [[nodiscard]] const std::vector<std::uint16_t>& object_brushes(
      EditorLayer layer) const noexcept;
  [[nodiscard]] bool place_object(EditorLayer layer,
                                  std::uint16_t type,
                                  std::uint16_t x,
                                  std::uint16_t y,
                                  std::uint8_t owner = 0U) noexcept;
  [[nodiscard]] bool erase_object_at(EditorLayer layer,
                                     std::uint16_t x,
                                     std::uint16_t y) noexcept;
  [[nodiscard]] bool object_at(EditorLayer layer,
                               std::uint16_t x,
                               std::uint16_t y,
                               std::uint16_t& type) const noexcept;
  [[nodiscard]] const std::vector<std::uint16_t>& terrain_brushes() const noexcept;
  [[nodiscard]] std::string_view terrain_brush_name(
      std::uint16_t brush_tile_id) const noexcept;
  [[nodiscard]] bool logical_terrain_ready() const noexcept;
  [[nodiscard]] bool valid_terrain_tile(std::uint16_t tile_id) const noexcept;
  [[nodiscard]] bool terrain_group_id(std::uint16_t tile_id,
                                      std::uint16_t& group_id) const noexcept;
  [[nodiscard]] bool same_terrain_group(std::uint16_t left,
                                        std::uint16_t right) const noexcept;
  [[nodiscard]] bool terrain_brush_at(std::uint16_t x,
                                      std::uint16_t y,
                                      std::uint16_t& brush_tile_id) const noexcept;
  [[nodiscard]] bool terrain_variant(std::uint16_t brush_tile_id,
                                     std::uint16_t x,
                                     std::uint16_t y,
                                     std::uint16_t& tile_id) const noexcept;
  [[nodiscard]] bool tile_at(std::uint16_t x,
                             std::uint16_t y,
                             std::uint16_t& tile_id) const noexcept;
  [[nodiscard]] const TilePixels* tile_pixels(std::uint16_t tile_id) const noexcept;
  [[nodiscard]] const ObjectArtFrame* object_art(EditorLayer layer,
                                                  std::uint16_t type) const noexcept;
  [[nodiscard]] std::string_view object_brush_name(EditorLayer layer,
                                                    std::uint16_t type) const noexcept;
  [[nodiscard]] const std::vector<std::uint8_t>& art_palette() const noexcept;
  [[nodiscard]] const TeamColorTable& team_colors() const noexcept;

 private:
  void reset() noexcept;
  [[nodiscard]] bool parse_chk(std::wstring& error) noexcept;
  [[nodiscard]] bool parse_object_sections(std::wstring& error) noexcept;
  [[nodiscard]] bool parse_auxiliary_sections(std::wstring& error) noexcept;
  [[nodiscard]] bool build_object_brush_inventory() noexcept;
  [[nodiscard]] bool build_object_art_cache(
      starcraft::runtime::StormModule& storm) noexcept;
  [[nodiscard]] bool build_doodad_art_cache() noexcept;
  [[nodiscard]] bool normalize_object_prototypes() noexcept;
  [[nodiscard]] bool commit_object_edit(
      EditorLayer layer,
      std::vector<formats::PlacementRecord> records,
      std::vector<undo::TileChange> tile_changes) noexcept;
  [[nodiscard]] bool commit_section_edit(
      std::uint32_t tag,
      std::vector<std::uint8_t> after) noexcept;
  [[nodiscard]] bool commit_section_edits(
      std::vector<std::pair<std::uint32_t, std::vector<std::uint8_t>>>
          replacements) noexcept;
  [[nodiscard]] bool build_terrain_brush_inventory(std::wstring& error) noexcept;
  [[nodiscard]] bool build_active_command() noexcept;
  [[nodiscard]] bool update_chk_terrain_layers() noexcept;
  [[nodiscard]] static bool make_tile_payload(
      const std::vector<std::uint16_t>& tiles,
      std::vector<std::uint8_t>& payload) noexcept;

  std::filesystem::path path_{};
  std::filesystem::path data_root_{};
  std::wstring title_{};
  formats::ChkDocument chk_{};
  formats::IsomSection isom_{};
  std::vector<std::uint16_t> tiles_{};
  std::vector<std::uint16_t> editor_tiles_{};
  std::vector<std::uint16_t> terrain_brushes_{};
  std::vector<UnitMarker> unit_markers_{};
  std::vector<DoodadMarker> doodad_markers_{};
  std::vector<SpriteMarker> sprite_markers_{};
  std::vector<LocationMarker> location_markers_{};
  std::vector<std::uint8_t> location_payload_{};
  std::vector<std::uint8_t> fog_mask_{};
  std::vector<std::uint8_t> active_fog_before_{};
  std::vector<formats::PlacementRecord> unit_records_{};
  std::vector<formats::PlacementRecord> doodad_records_{};
  std::vector<formats::PlacementRecord> sprite_records_{};
  std::vector<std::uint16_t> doodad_brushes_{};
  std::vector<std::uint16_t> unit_brushes_{};
  std::vector<std::uint16_t> sprite_brushes_{};
  struct DoodadTemplateCell {
    std::int16_t pixel_offset_x{};
    std::int16_t pixel_offset_y{};
    std::uint16_t map_tile{};
  };
  struct DoodadTemplate {
    std::uint16_t type{};
    std::uint8_t x_alignment{};
    std::uint8_t y_alignment{};
    formats::PlacementRecord prototype{};
    std::vector<DoodadTemplateCell> cells{};
  };
  std::vector<DoodadTemplate> doodad_templates_{};
  std::vector<formats::PlacementRecord> unit_prototypes_{};
  std::vector<formats::PlacementRecord> sprite_prototypes_{};
  std::unordered_map<std::uint16_t, std::shared_ptr<ObjectArtFrame>>
      unit_art_{};
  std::unordered_map<std::uint16_t, std::shared_ptr<ObjectArtFrame>>
      sprite_art_{};
  std::unordered_map<std::uint16_t, std::shared_ptr<ObjectArtFrame>>
      doodad_art_{};
  TeamColorTable team_colors_{};
  std::vector<std::uint8_t> stat_text_{};
  starcraft::gds::TilesetData tileset_{};
  terrain::IsomCatalog isom_catalog_{};
  terrain::IsomTopology isom_topology_{};
  mutable std::unordered_map<std::uint16_t, TilePixels> tile_cache_{};
  undo::UndoStack undo_stack_{};
  undo::TileEditCommand active_tile_edit_{};
  std::vector<std::uint16_t> active_tiles_before_{};
  std::vector<std::uint16_t> active_editor_tiles_before_{};
  formats::IsomSection active_isom_before_{};
  std::string tileset_name_{};
  std::uint16_t width_{};
  std::uint16_t height_{};
  std::uint16_t tileset_id_{};
  std::size_t section_count_{};
  std::size_t unit_count_{};
  std::size_t doodad_count_{};
  std::size_t sprite_count_{};
  ScenarioFormat format_{ScenarioFormat::unknown_chk};
  bool source_is_archive_{};
  bool editing_ready_{};
  bool tile_edit_active_{};
  bool fog_edit_active_{};
  bool has_editor_tiles_{};
};

[[nodiscard]] const wchar_t* scenario_format_name(ScenarioFormat format) noexcept;

}  // namespace staredit
