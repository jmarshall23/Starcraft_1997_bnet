#include "document.hpp"

#include "formats/mpq_writer.hpp"
#include "formats/retail_chk.hpp"

#include "starcraft/data/chk.hpp"
#include "starcraft/data/dat.hpp"
#include "starcraft/gds/grp.hpp"
#include "starcraft/lang/iscript.hpp"
#include "starcraft/runtime/storm.hpp"
#include "starcraft/runtime/asset_archives.hpp"
#include "terrain/isom_terrain_editor.hpp"

#include <algorithm>
#include <cwctype>
#include <fstream>
#include <limits>
#include <queue>
#include <set>
#include <string_view>
#include <utility>

#include <windows.h>

namespace staredit {
namespace {

constexpr std::size_t kMaximumChkBytes = 64U * 1024U * 1024U;

std::wstring widen_ascii(const std::string& value) {
  return std::wstring{value.begin(), value.end()};
}

struct ScenarioStringBinding {
  std::uint32_t tag{starcraft::data::chk_section_strings};
  bool exists{};
  formats::ChkStringTable table{};
};

bool load_scenario_strings(const formats::ChkDocument& chk,
                           ScenarioStringBinding& binding) noexcept {
  binding = {};
  const formats::ChkSection* section =
      chk.section(starcraft::data::chk_section_extended_strings);
  formats::ChkStringFormat format = formats::ChkStringFormat::extended_u32;
  if (section != nullptr) {
    binding.tag = starcraft::data::chk_section_extended_strings;
  } else {
    section = chk.section(starcraft::data::chk_section_strings);
    format = formats::ChkStringFormat::classic_u16;
    binding.tag = starcraft::data::chk_section_strings;
  }
  binding.exists = section != nullptr;
  if (section == nullptr) {
    return true;
  }
  std::string error{};
  return binding.table.parse(section->payload, format, error);
}

std::wstring lowercase_extension(const std::filesystem::path& path) {
  std::wstring extension = path.extension().wstring();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](const wchar_t value) {
                   return static_cast<wchar_t>(std::towlower(value));
                 });
  return extension;
}

bool read_loose_chk(const std::filesystem::path& path,
                    std::vector<std::uint8_t>& bytes) {
  bytes.clear();
  std::error_code error{};
  const std::uintmax_t size = std::filesystem::file_size(path, error);
  if (error || size == 0 || size > kMaximumChkBytes ||
      size > (std::numeric_limits<std::size_t>::max)()) {
    return false;
  }
  try {
    std::ifstream input{path, std::ios::binary};
    if (!input) {
      return false;
    }
    bytes.resize(static_cast<std::size_t>(size));
    input.read(reinterpret_cast<char*>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    if (!input || static_cast<std::size_t>(input.gcount()) != bytes.size()) {
      bytes.clear();
      return false;
    }
    return true;
  } catch (...) {
    bytes.clear();
    return false;
  }
}

bool write_transactional_file(const std::filesystem::path& target,
                              const std::vector<std::uint8_t>& bytes,
                              std::wstring& error) noexcept {
  error.clear();
  std::filesystem::path temporary{};
  HANDLE file = INVALID_HANDLE_VALUE;
  try {
    const std::filesystem::path absolute = std::filesystem::absolute(target);
    const std::filesystem::path directory = absolute.parent_path();
    if (directory.empty()) {
      error = L"The export destination has no parent directory.";
      return false;
    }
    std::error_code directory_error{};
    if (!std::filesystem::is_directory(directory, directory_error)) {
      error = L"The export destination directory does not exist.";
      return false;
    }
    const std::wstring stem = absolute.filename().wstring() +
                              L".staredit-" +
                              std::to_wstring(GetCurrentProcessId()) + L"-";
    for (std::uint32_t attempt = 0U; attempt < 100U; ++attempt) {
      temporary = directory / (stem + std::to_wstring(attempt) + L".tmp");
      file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr,
                         CREATE_NEW,
                         FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
                         nullptr);
      if (file != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS) {
        break;
      }
    }
    if (file == INVALID_HANDLE_VALUE) {
      error = L"A temporary export file could not be created (Windows error " +
              std::to_wstring(GetLastError()) + L").";
      return false;
    }

    bool written = true;
    std::size_t cursor{};
    while (cursor < bytes.size()) {
      const DWORD request = static_cast<DWORD>((std::min)(
          bytes.size() - cursor, static_cast<std::size_t>(1U << 20U)));
      DWORD completed{};
      if (WriteFile(file, bytes.data() + cursor, request, &completed, nullptr) ==
              FALSE ||
          completed != request) {
        written = false;
        break;
      }
      cursor += completed;
    }
    written = written && FlushFileBuffers(file) != FALSE;
    const DWORD write_error = written ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);
    file = INVALID_HANDLE_VALUE;
    if (!written) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary export file could not be written (Windows error " +
              std::to_wstring(write_error) + L").";
      return false;
    }

    std::vector<std::uint8_t> verification{};
    formats::ChkDocument parsed{};
    std::string parse_error{};
    if (!read_loose_chk(temporary, verification) || verification != bytes ||
        !parsed.parse(std::move(verification), parse_error) ||
        !parsed.matches_original()) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary export failed CHK reopen validation.";
      return false;
    }
    if (MoveFileExW(temporary.c_str(), absolute.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == FALSE) {
      const DWORD move_error = GetLastError();
      (void)DeleteFileW(temporary.c_str());
      error = L"The validated export could not replace the destination "
              L"(Windows error " +
              std::to_wstring(move_error) + L").";
      return false;
    }
    return true;
  } catch (...) {
    if (file != INVALID_HANDLE_VALUE) {
      CloseHandle(file);
    }
    if (!temporary.empty()) {
      (void)DeleteFileW(temporary.c_str());
    }
    error = L"The raw CHK export could not be completed.";
    return false;
  }
}

bool write_transactional_scenario_archive(
    const std::filesystem::path& target,
    const std::filesystem::path& storm_path,
    const std::vector<std::uint8_t>& chk_bytes,
    std::wstring& error) noexcept {
  error.clear();
  std::filesystem::path temporary{};
  try {
    const std::filesystem::path absolute = std::filesystem::absolute(target);
    const std::filesystem::path directory = absolute.parent_path();
    std::error_code directory_error{};
    if (directory.empty() ||
        !std::filesystem::is_directory(directory, directory_error)) {
      error = L"The scenario destination directory does not exist.";
      return false;
    }
    const std::wstring stem = absolute.filename().wstring() +
                              L".staredit-" +
                              std::to_wstring(GetCurrentProcessId()) + L"-";
    for (std::uint32_t attempt = 0U; attempt < 100U; ++attempt) {
      const std::filesystem::path candidate =
          directory / (stem + std::to_wstring(attempt) + L".tmp");
      HANDLE reservation = CreateFileW(
          candidate.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
          FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
      if (reservation != INVALID_HANDLE_VALUE) {
        CloseHandle(reservation);
        temporary = candidate;
        break;
      }
      if (GetLastError() != ERROR_FILE_EXISTS) {
        break;
      }
    }
    if (temporary.empty()) {
      error = L"A temporary scenario archive could not be reserved.";
      return false;
    }

    std::string mpq_error{};
    if (!formats::write_single_file_mpq(
            temporary, R"(staredit\scenario.chk)", chk_bytes, mpq_error)) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX archive could not be written: " +
              widen_ascii(mpq_error);
      return false;
    }
    starcraft::runtime::StormModule storm{storm_path};
    void* archive{};
    std::vector<std::uint8_t> verification{};
    const bool opened =
        storm.loaded() && storm.open_archive(temporary, &archive, 4000U);
    const bool loaded = opened && storm.load_file_from_archive(
                                      archive, R"(staredit\scenario.chk)",
                                      verification);
    const bool closed = archive == nullptr || storm.close_archive(archive);
    formats::ChkDocument parsed{};
    std::string parse_error{};
    if (!storm.loaded()) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX could not load storm.dll.";
      return false;
    }
    if (!opened) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX could not be opened by Storm.";
      return false;
    }
    if (!loaded) {
      (void)DeleteFileW(temporary.c_str());
      error = L"Storm could not read staredit\\scenario.chk from the "
              L"temporary SCX.";
      return false;
    }
    if (!closed) {
      (void)DeleteFileW(temporary.c_str());
      error = L"Storm could not close the temporary SCX.";
      return false;
    }
    if (verification != chk_bytes) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX returned different scenario.chk bytes.";
      return false;
    }
    if (!parsed.parse(std::move(verification), parse_error)) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX returned an invalid CHK: " +
              widen_ascii(parse_error);
      return false;
    }
    if (parsed.dialect().dialect != formats::ChkDialect::retail_chk) {
      (void)DeleteFileW(temporary.c_str());
      error = L"The temporary SCX did not contain a retail CHK.";
      return false;
    }
    if (MoveFileExW(temporary.c_str(), absolute.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) ==
        FALSE) {
      const DWORD move_error = GetLastError();
      (void)DeleteFileW(temporary.c_str());
      error = L"The validated SCX could not replace the destination "
              L"(Windows error " +
              std::to_wstring(move_error) + L").";
      return false;
    }
    return true;
  } catch (...) {
    if (!temporary.empty()) {
      (void)DeleteFileW(temporary.c_str());
    }
    error = L"The retail scenario archive could not be completed.";
    return false;
  }
}

std::uint16_t read_u16(const std::uint8_t* const bytes,
                       const std::size_t offset) noexcept {
  return static_cast<std::uint16_t>(bytes[offset]) |
         static_cast<std::uint16_t>(
             static_cast<std::uint16_t>(bytes[offset + 1U]) << 8U);
}

std::uint32_t read_u32(const std::uint8_t* const bytes,
                       const std::size_t offset) noexcept {
  return static_cast<std::uint32_t>(bytes[offset]) |
         (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U) |
         (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U) |
         (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

void write_u16(std::vector<std::uint8_t>& bytes,
               const std::size_t offset,
               const std::uint16_t value) noexcept {
  bytes[offset] = static_cast<std::uint8_t>(value);
  bytes[offset + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

void write_u32(std::vector<std::uint8_t>& bytes,
               const std::size_t offset,
               const std::uint32_t value) noexcept {
  bytes[offset] = static_cast<std::uint8_t>(value);
  bytes[offset + 1U] = static_cast<std::uint8_t>(value >> 8U);
  bytes[offset + 2U] = static_cast<std::uint8_t>(value >> 16U);
  bytes[offset + 3U] = static_cast<std::uint8_t>(value >> 24U);
}

formats::PlacementRecordLayout unit_placement_layout(
    const formats::ChkDialectInfo& dialect) noexcept {
  const formats::UnitRecordLayout& unit = dialect.units;
  return {starcraft::data::chk_section_units,
          unit.record_bytes,
          unit.type_offset,
          unit.x_offset,
          unit.y_offset,
          unit.owner_offset};
}

std::uint32_t squared_distance(const std::uint16_t left_x,
                               const std::uint16_t left_y,
                               const std::uint16_t right_x,
                               const std::uint16_t right_y) noexcept {
  const std::int32_t dx = static_cast<std::int32_t>(left_x) - right_x;
  const std::int32_t dy = static_cast<std::int32_t>(left_y) - right_y;
  return static_cast<std::uint32_t>(dx * dx + dy * dy);
}

}  // namespace

bool EditorDocument::load(const std::filesystem::path& path,
                          const std::filesystem::path& data_root,
                          std::wstring& error) noexcept {
  reset();
  error.clear();
  data_root_ = data_root;
  path_ = path;
  title_ = path.filename().wstring();
  source_is_archive_ = lowercase_extension(path) != L".chk";

  starcraft::runtime::StormModule storm{data_root / L"storm.dll"};
  if (!storm.loaded()) {
    error = L"The recovered storm.dll could not be loaded from:\n" +
            (data_root / L"storm.dll").wstring();
    return false;
  }

  starcraft::runtime::AssetArchives asset_archives{};
  void* map_archive{};
  std::vector<std::uint8_t> chk_bytes{};
  const auto close_archives = [&]() noexcept {
    if (map_archive != nullptr) {
      (void)storm.close_archive(map_archive);
      map_archive = nullptr;
    }
    (void)asset_archives.close(storm);
  };

  if (!asset_archives.open(storm, data_root)) {
    error = L"No supported beta or retail StarCraft MPQ set could be opened "
            L"from:\n" + data_root.wstring();
    close_archives();
    return false;
  }

  bool chk_loaded{};
  if (source_is_archive_) {
    chk_loaded = storm.open_archive(path, &map_archive, 4000U) &&
                 storm.load_file_from_archive(
                     map_archive, R"(staredit\scenario.chk)", chk_bytes);
    if (map_archive != nullptr) {
      (void)storm.close_archive(map_archive);
      map_archive = nullptr;
    }
  } else {
    chk_loaded = read_loose_chk(path, chk_bytes);
  }
  if (!chk_loaded) {
    error = source_is_archive_
                ? L"The file is not a readable SCM/SCX scenario archive."
                : L"The file is not a readable CHK scenario.";
    close_archives();
    return false;
  }
  std::string parse_error{};
  if (!chk_.parse(std::move(chk_bytes), parse_error)) {
    error = L"The scenario.chk stream is invalid: " + widen_ascii(parse_error);
    close_archives();
    return false;
  }
  if (!parse_chk(error)) {
    close_archives();
    return false;
  }
  editing_ready_ = undo_stack_.initialize(64U);

  const std::string_view recovered_name =
      starcraft::gds::tileset_name(tileset_id_);
  tileset_name_.assign(recovered_name);
  if (tileset_name_.empty()) {
    error = L"The scenario uses an unsupported tileset id: " +
            std::to_wstring(tileset_id_);
    close_archives();
    return false;
  }
  if (!tileset_.load(storm, tileset_name_)) {
    error = L"The terrain art could not be loaded: " +
            widen_ascii(tileset_.failed_asset());
    close_archives();
    return false;
  }
  if (!isom_topology_.build(tileset_)) {
    error = L"The tileset CV5 topology could not be constructed.";
    close_archives();
    return false;
  }
  // The recovered logical ISOM definitions cover the five tilesets present in
  // the older editor. Retail expansion maps (Desert, Ice, and Twilight) still
  // load and use CV5 group brushes; their low-level terrain painter remains
  // available until those transition catalogs are recovered.
  (void)isom_catalog_.build(tileset_id_, tileset_);
  if (!build_terrain_brush_inventory(error)) {
    close_archives();
    return false;
  }
  if (!build_object_brush_inventory()) {
    error = L"The placed-object brush inventory could not be constructed.";
    close_archives();
    return false;
  }
  (void)build_object_art_cache(storm);

  close_archives();
  return true;
}

bool EditorDocument::create_blank(
    const std::filesystem::path& scenario_template,
    const std::filesystem::path& data_root,
    const std::uint16_t width,
    const std::uint16_t height,
    const std::uint16_t tileset_id,
    const std::string_view terrain_name,
    std::wstring& error) noexcept {
  if (!load(scenario_template, data_root, error)) {
    return false;
  }
  try {
    if (width == 0U || height == 0U || tileset_id > 4U ||
        terrain_name.empty()) {
      error = L"The new scenario settings are invalid.";
      reset();
      return false;
    }

    const std::uint16_t source_tileset = tileset_id_;
    width_ = width;
    height_ = height;
    tileset_id_ = tileset_id;
    const std::size_t tile_count = static_cast<std::size_t>(width_) * height_;
    tiles_.assign(tile_count, 0U);
    editor_tiles_.assign(tile_count, 0U);
    tile_cache_.clear();
    if (!isom_.initialize(width_, height_)) {
      error = L"The blank ISOM grid could not be allocated.";
      reset();
      return false;
    }

    std::vector<std::uint8_t> dimensions{
        static_cast<std::uint8_t>(width_),
        static_cast<std::uint8_t>(width_ >> 8U),
        static_cast<std::uint8_t>(height_),
        static_cast<std::uint8_t>(height_ >> 8U)};
    std::vector<std::uint8_t> era{static_cast<std::uint8_t>(tileset_id_),
                                  static_cast<std::uint8_t>(tileset_id_ >> 8U)};
    if (!chk_.replace_section(starcraft::data::chk_section_dimensions, 0U,
                              std::move(dimensions)) ||
        !chk_.replace_section(starcraft::data::chk_section_era, 0U,
                              std::move(era))) {
      error = L"The blank scenario dimensions or tileset could not be serialized.";
      reset();
      return false;
    }

    starcraft::runtime::StormModule storm{data_root / L"storm.dll"};
    starcraft::runtime::AssetArchives asset_archives{};
    if (!storm.loaded() || !asset_archives.open(storm, data_root)) {
      error = L"The terrain assets for the new scenario could not be opened.";
      reset();
      return false;
    }
    tileset_ = {};
    isom_catalog_ = {};
    isom_topology_ = {};
    tileset_name_.assign(starcraft::gds::tileset_name(tileset_id_));
    const bool terrain_loaded = !tileset_name_.empty() &&
                                tileset_.load(storm, tileset_name_) &&
                                isom_topology_.build(tileset_) &&
                                isom_catalog_.build(tileset_id_, tileset_) &&
                                build_terrain_brush_inventory(error);
    if (!terrain_loaded) {
      (void)asset_archives.close(storm);
      if (error.empty()) {
        error = L"The selected tileset could not be initialized.";
      }
      reset();
      return false;
    }

    // The sample maps carry reconstructed doodad tile templates. They are
    // usable only when the source and requested tilesets match; unit and
    // sprite inventories are independent of terrain and remain complete.
    if (source_tileset != tileset_id_) {
      doodad_brushes_.clear();
      doodad_templates_.clear();
      doodad_art_.clear();
    }
    (void)build_object_art_cache(storm);
    (void)asset_archives.close(storm);

    const auto terrain = std::find_if(
        terrain_brushes_.begin(), terrain_brushes_.end(),
        [this, terrain_name](const std::uint16_t brush) {
          return terrain_brush_name(brush) == terrain_name;
        });
    if (terrain == terrain_brushes_.end() || !begin_tile_edit() ||
        !paint_terrain(width_ / 2U, height_ / 2U, *terrain,
                       (std::max)(width_, height_)) ||
        !commit_tile_edit()) {
      error = L"The selected default terrain could not fill the new scenario.";
      reset();
      return false;
    }

    // ISOM regeneration intentionally preserves placed doodad overlays during
    // ordinary editing. A new map owns no doodads, so its game layer begins as
    // the freshly flattened editor layer.
    tiles_ = editor_tiles_;
    if (!update_chk_terrain_layers()) {
      error = L"The blank terrain layers could not be serialized.";
      reset();
      return false;
    }

    const formats::ChkDialectInfo& dialect = chk_.dialect();
    const formats::PlacementRecordLayout units =
        unit_placement_layout(dialect);
    const auto clear_placement = [this](
                                     const formats::PlacementRecordLayout& layout) {
      if (layout.record_bytes == 0U) {
        return false;
      }
      return chk_.count(layout.section_tag) != 0U
                 ? chk_.replace_section(layout.section_tag, 0U, {})
                 : chk_.append_section(layout.section_tag, {});
    };
    if (!clear_placement(units) || !clear_placement(dialect.doodads) ||
        !clear_placement(dialect.sprites)) {
      error = L"The blank placed-object sections could not be initialized.";
      reset();
      return false;
    }

    constexpr std::uint32_t trigger_tag =
        starcraft::data::chk_fourcc('T', 'R', 'I', 'G');
    constexpr std::uint32_t briefing_tag =
        starcraft::data::chk_fourcc('M', 'B', 'R', 'F');
    if (chk_.count(trigger_tag) != 0U &&
        !chk_.replace_section(trigger_tag, 0U, {})) {
      error = L"The blank trigger section could not be initialized.";
      reset();
      return false;
    }
    if (chk_.count(briefing_tag) != 0U &&
        !chk_.replace_section(briefing_tag, 0U, {})) {
      error = L"The blank briefing section could not be initialized.";
      reset();
      return false;
    }
    if (chk_.count(starcraft::data::chk_section_fog_mask) != 0U &&
        !chk_.replace_section(starcraft::data::chk_section_fog_mask, 0U,
                              std::vector<std::uint8_t>(tiles_.size(), 0U))) {
      error = L"The blank fog section could not be initialized.";
      reset();
      return false;
    }

    const std::vector<std::uint8_t> empty_locations(
        starcraft::data::chk_location_slot_count *
            starcraft::data::chk_location_record_bytes,
        0U);
    const bool locations_written =
        chk_.count(starcraft::data::chk_section_locations) != 0U
            ? chk_.replace_section(starcraft::data::chk_section_locations, 0U,
                                   empty_locations)
            : chk_.append_section(starcraft::data::chk_section_locations,
                                  empty_locations);
    if (!locations_written) {
      error = L"The blank location section could not be initialized.";
      reset();
      return false;
    }

    std::string retail_error{};
    if (!formats::convert_to_retail_chk(chk_, retail_error) ||
        !normalize_object_prototypes()) {
      error = L"The blank scenario could not be converted to retail CHK: " +
              widen_ascii(retail_error);
      reset();
      return false;
    }
    format_ = chk_.dialect().dialect;

    if (!parse_object_sections(error) || !parse_auxiliary_sections(error) ||
        !undo_stack_.initialize(64U)) {
      if (error.empty()) {
        error = L"The blank document undo history could not be initialized.";
      }
      reset();
      return false;
    }
    section_count_ = chk_.section_count();
    path_.clear();
    title_ = L"Untitled";
    source_is_archive_ = false;
    editing_ready_ = true;
    return true;
  } catch (...) {
    error = L"There was not enough memory to create a blank map.";
    reset();
    return false;
  }
}

const std::filesystem::path& EditorDocument::path() const noexcept { return path_; }
const std::wstring& EditorDocument::title() const noexcept { return title_; }
std::uint16_t EditorDocument::width() const noexcept { return width_; }
std::uint16_t EditorDocument::height() const noexcept { return height_; }
std::uint16_t EditorDocument::tileset_id() const noexcept { return tileset_id_; }
const std::string& EditorDocument::tileset_name() const noexcept { return tileset_name_; }
ScenarioFormat EditorDocument::format() const noexcept { return format_; }
bool EditorDocument::source_is_archive() const noexcept { return source_is_archive_; }
std::size_t EditorDocument::section_count() const noexcept { return section_count_; }
std::size_t EditorDocument::unit_count() const noexcept { return unit_count_; }
std::size_t EditorDocument::doodad_count() const noexcept {
  return doodad_count_;
}
std::size_t EditorDocument::sprite_count() const noexcept { return sprite_count_; }
bool EditorDocument::has_isom() const noexcept { return isom_.valid(); }
std::size_t EditorDocument::isom_entry_count() const noexcept {
  return isom_.entry_count();
}
bool EditorDocument::isom_topology_ready() const noexcept {
  return isom_topology_.valid();
}
std::size_t EditorDocument::isom_group_pair_count() const noexcept {
  return isom_topology_.group_pair_count();
}
std::size_t EditorDocument::isom_terrain_type_count() const noexcept {
  return isom_topology_.terrain_type_count();
}
std::uint64_t EditorDocument::isom_checksum() const noexcept {
  std::uint64_t hash = 1469598103934665603ULL;
  if (!isom_.valid()) {
    return 0U;
  }
  for (std::size_t row = 0U; row < isom_.row_count(); ++row) {
    for (std::size_t column = 0U; column < isom_.column_count(); ++column) {
      const formats::IsomEntry* const entry = isom_.entry(column, row);
      if (entry == nullptr) {
        return 0U;
      }
      for (const formats::IsomLink link : entry->links) {
        hash ^= link.raw;
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}
std::uint64_t EditorDocument::editor_tile_checksum() const noexcept {
  std::uint64_t hash = 1469598103934665603ULL;
  for (const std::uint16_t tile : editor_tiles_) {
    hash ^= tile;
    hash *= 1099511628211ULL;
  }
  return hash;
}
std::uint64_t EditorDocument::game_tile_checksum() const noexcept {
  std::uint64_t hash = 1469598103934665603ULL;
  for (const std::uint16_t tile : tiles_) {
    hash ^= tile;
    hash *= 1099511628211ULL;
  }
  return hash;
}
bool EditorDocument::chk_round_trip_matches_source() const noexcept {
  return chk_.matches_original();
}
bool EditorDocument::editing_ready() const noexcept { return editing_ready_; }
bool EditorDocument::modified() const noexcept {
  return editing_ready_ && !undo_stack_.at_origin();
}
bool EditorDocument::can_undo() const noexcept {
  return editing_ready_ && undo_stack_.can_undo();
}
bool EditorDocument::can_redo() const noexcept {
  return editing_ready_ && undo_stack_.can_redo();
}

bool EditorDocument::begin_tile_edit() noexcept {
  if (!editing_ready_ || tile_edit_active_) {
    return false;
  }
  try {
    active_tile_edit_ = {};
    active_tiles_before_ = tiles_;
    active_editor_tiles_before_ = editor_tiles_;
    active_isom_before_ = isom_;
    tile_edit_active_ = true;
    return true;
  } catch (...) {
    active_tiles_before_.clear();
    active_editor_tiles_before_.clear();
    active_isom_before_ = {};
    return false;
  }
}

bool EditorDocument::paint_terrain(const std::uint16_t x,
                                   const std::uint16_t y,
                                   const std::uint16_t brush_tile_id,
                                   const std::uint16_t brush_extent) noexcept {
  if (!tile_edit_active_ || !logical_terrain_ready()) {
    return false;
  }
  const terrain::IsomBrush* const brush =
      isom_catalog_.brush_for_preview_tile(brush_tile_id);
  return brush != nullptr && terrain::IsomTerrainEditor::paint(
                                 isom_, editor_tiles_, tiles_, width_, height_,
                                 x, y, brush->terrain_type, brush_extent,
                                 isom_catalog_, isom_topology_, tileset_);
}

bool EditorDocument::paint_tile(const std::uint16_t x,
                                const std::uint16_t y,
                                const std::uint16_t tile_id) noexcept {
  if (!tile_edit_active_ || x >= width_ || y >= height_ ||
      !valid_terrain_tile(tile_id)) {
    return false;
  }
  const std::size_t index = static_cast<std::size_t>(y) * width_ + x;
  tiles_[index] = tile_id;
  editor_tiles_[index] = tile_id;
  return true;
}

bool EditorDocument::commit_tile_edit() noexcept {
  if (!tile_edit_active_) {
    return false;
  }
  tile_edit_active_ = false;
  if (!build_active_command()) {
    tiles_ = std::move(active_tiles_before_);
    editor_tiles_ = std::move(active_editor_tiles_before_);
    isom_ = std::move(active_isom_before_);
    return false;
  }
  if (active_tile_edit_.changes.empty() &&
      active_tile_edit_.editor_changes.empty() &&
      active_tile_edit_.isom_changes.empty()) {
    active_tiles_before_.clear();
    active_editor_tiles_before_.clear();
    active_isom_before_ = {};
    return true;
  }
  const bool had_editor_tiles = has_editor_tiles_;
  if (!update_chk_terrain_layers()) {
    tiles_ = std::move(active_tiles_before_);
    editor_tiles_ = std::move(active_editor_tiles_before_);
    isom_ = std::move(active_isom_before_);
    active_tile_edit_ = {};
    return false;
  }
  if (!undo_stack_.push(std::move(active_tile_edit_))) {
    tiles_ = std::move(active_tiles_before_);
    editor_tiles_ = std::move(active_editor_tiles_before_);
    isom_ = std::move(active_isom_before_);
    has_editor_tiles_ = had_editor_tiles;
    (void)update_chk_terrain_layers();
    active_tile_edit_ = {};
    return false;
  }
  active_tile_edit_ = {};
  active_tiles_before_.clear();
  active_editor_tiles_before_.clear();
  active_isom_before_ = {};
  return true;
}

void EditorDocument::cancel_tile_edit() noexcept {
  if (!tile_edit_active_) {
    return;
  }
  tiles_ = std::move(active_tiles_before_);
  editor_tiles_ = std::move(active_editor_tiles_before_);
  isom_ = std::move(active_isom_before_);
  active_tile_edit_ = {};
  tile_edit_active_ = false;
}

bool EditorDocument::undo() noexcept {
  if (!can_undo()) {
    return false;
  }
  if (!undo_stack_.undo(tiles_, editor_tiles_, isom_, chk_)) {
    return false;
  }
  if (!update_chk_terrain_layers()) {
    (void)undo_stack_.redo(tiles_, editor_tiles_, isom_, chk_);
    return false;
  }
  std::wstring object_error{};
  return parse_object_sections(object_error) &&
         parse_auxiliary_sections(object_error);
}

bool EditorDocument::redo() noexcept {
  if (!can_redo()) {
    return false;
  }
  if (!undo_stack_.redo(tiles_, editor_tiles_, isom_, chk_)) {
    return false;
  }
  if (!update_chk_terrain_layers()) {
    (void)undo_stack_.undo(tiles_, editor_tiles_, isom_, chk_);
    return false;
  }
  std::wstring object_error{};
  return parse_object_sections(object_error) &&
         parse_auxiliary_sections(object_error);
}

bool EditorDocument::export_raw_chk(const std::filesystem::path& path,
                                    std::wstring& error) const noexcept {
  std::vector<std::uint8_t> serialized{};
  std::string serialization_error{};
  if (!chk_.serialize(serialized, serialization_error)) {
    error = L"The CHK could not be serialized: " +
            widen_ascii(serialization_error);
    return false;
  }
  return write_transactional_file(path, serialized, error);
}

bool EditorDocument::save_retail_archive(
    const std::filesystem::path& path,
    std::wstring& error) noexcept {
  error.clear();
  if (!editing_ready_ || path.empty() || data_root_.empty()) {
    error = L"The scenario is not ready to save.";
    return false;
  }
  try {
    formats::ChkDocument retail = chk_;
    std::string conversion_error{};
    if (!formats::convert_to_retail_chk(retail, conversion_error)) {
      error = L"The scenario could not be converted to retail CHK: " +
              widen_ascii(conversion_error);
      return false;
    }
    std::vector<std::uint8_t> serialized{};
    std::string serialization_error{};
    if (!retail.serialize(serialized, serialization_error)) {
      error = L"The retail CHK could not be serialized: " +
              widen_ascii(serialization_error);
      return false;
    }
    if (!write_transactional_scenario_archive(
            path, data_root_ / L"storm.dll", serialized, error)) {
      return false;
    }

    const bool converted =
        chk_.dialect().dialect != formats::ChkDialect::retail_chk;
    chk_ = std::move(retail);
    format_ = chk_.dialect().dialect;
    section_count_ = chk_.section_count();
    std::wstring parse_error{};
    if (!parse_object_sections(parse_error) ||
        !parse_auxiliary_sections(parse_error) ||
        !normalize_object_prototypes()) {
      error = L"The saved retail CHK could not be activated: " + parse_error;
      return false;
    }
    if (converted) {
      if (!undo_stack_.initialize(64U)) {
        error = L"The undo history could not be reset after retail conversion.";
        return false;
      }
    } else {
      undo_stack_.mark_origin();
    }
    path_ = path;
    title_ = path.filename().wstring();
    source_is_archive_ = true;
    return true;
  } catch (...) {
    error = L"There was not enough memory to save the retail scenario.";
    return false;
  }
}
const std::vector<UnitMarker>& EditorDocument::unit_markers() const noexcept {
  return unit_markers_;
}

bool EditorDocument::unit_attributes(
    const std::size_t index,
    UnitAttributes& attributes) const noexcept {
  attributes = {};
  if (index >= unit_records_.size()) {
    return false;
  }
  const formats::PlacementRecord& record = unit_records_[index];
  attributes.index = index;
  attributes.x = record.x;
  attributes.y = record.y;
  attributes.type = record.type;
  attributes.owner = record.owner;
  attributes.hit_points = 100U;
  attributes.shield_points = 100U;
  attributes.energy = 100U;
  if (chk_.dialect().dialect == formats::ChkDialect::retail_chk &&
      record.raw.size() >= 36U) {
    attributes.valid_state_flags = read_u16(record.raw.data(), 12U);
    attributes.valid_data_flags = read_u16(record.raw.data(), 14U);
    attributes.hit_points = record.raw[17U];
    attributes.shield_points = record.raw[18U];
    attributes.energy = record.raw[19U];
    attributes.resource_amount = read_u32(record.raw.data(), 20U);
    attributes.hangar_count = read_u16(record.raw.data(), 24U);
    attributes.state_flags = read_u16(record.raw.data(), 26U);
    attributes.has_retail_attributes = true;
  }
  return true;
}

bool EditorDocument::unit_index_at(const std::uint16_t x,
                                   const std::uint16_t y,
                                   std::size_t& index) const noexcept {
  for (std::size_t remaining = unit_markers_.size(); remaining != 0U;
       --remaining) {
    const std::size_t candidate = remaining - 1U;
    const UnitMarker& marker = unit_markers_[candidate];
    int half_width = 10;
    int half_height = 10;
    if (const ObjectArtFrame* const art =
            object_art(EditorLayer::units, marker.type)) {
      half_width = (std::max)(half_width,
                              static_cast<int>(art->canvas_width) / 2);
      half_height = (std::max)(half_height,
                               static_cast<int>(art->canvas_height) / 2);
    }
    if (std::abs(static_cast<int>(x) - marker.x) <= half_width &&
        std::abs(static_cast<int>(y) - marker.y) <= half_height) {
      index = candidate;
      return true;
    }
  }
  return false;
}

bool EditorDocument::update_unit(const std::size_t index,
                                 const UnitAttributes& attributes) noexcept {
  const std::uint32_t map_width = static_cast<std::uint32_t>(width_) * tile_width;
  const std::uint32_t map_height =
      static_cast<std::uint32_t>(height_) * tile_height;
  if (!editing_ready_ || tile_edit_active_ || index >= unit_records_.size() ||
      attributes.x >= map_width || attributes.y >= map_height ||
      attributes.owner >= starcraft::data::chk_player_slot_count ||
      std::find(unit_brushes_.begin(), unit_brushes_.end(), attributes.type) ==
          unit_brushes_.end() ||
      (attributes.has_retail_attributes &&
       (attributes.hit_points > 100U || attributes.shield_points > 100U ||
        attributes.energy > 100U))) {
    return false;
  }
  try {
    std::vector<formats::PlacementRecord> records = unit_records_;
    formats::PlacementRecord& record = records[index];
    const formats::PlacementRecord before = record;
    record.x = attributes.x;
    record.y = attributes.y;
    record.type = attributes.type;
    record.owner = attributes.owner;
    if (attributes.has_retail_attributes &&
        chk_.dialect().dialect == formats::ChkDialect::retail_chk &&
        record.raw.size() >= 36U) {
      write_u16(record.raw, 12U, attributes.valid_state_flags);
      write_u16(record.raw, 14U, attributes.valid_data_flags);
      record.raw[17U] = attributes.hit_points;
      record.raw[18U] = attributes.shield_points;
      record.raw[19U] = attributes.energy;
      write_u32(record.raw, 20U, attributes.resource_amount);
      write_u16(record.raw, 24U, attributes.hangar_count);
      write_u16(record.raw, 26U, attributes.state_flags);
    }
    if (record.x == before.x && record.y == before.y &&
        record.type == before.type && record.owner == before.owner &&
        record.raw == before.raw) {
      return true;
    }
    return commit_object_edit(EditorLayer::units, std::move(records), {});
  } catch (...) {
    return false;
  }
}

bool EditorDocument::move_unit(const std::size_t index,
                               const std::uint16_t x,
                               const std::uint16_t y) noexcept {
  UnitAttributes attributes{};
  if (!unit_attributes(index, attributes)) {
    return false;
  }
  attributes.x = x;
  attributes.y = y;
  return update_unit(index, attributes);
}

bool EditorDocument::erase_unit(const std::size_t index) noexcept {
  if (!editing_ready_ || tile_edit_active_ || index >= unit_records_.size()) {
    return false;
  }
  try {
    std::vector<formats::PlacementRecord> records = unit_records_;
    records.erase(records.begin() + static_cast<std::ptrdiff_t>(index));
    return commit_object_edit(EditorLayer::units, std::move(records), {});
  } catch (...) {
    return false;
  }
}
const std::vector<DoodadMarker>& EditorDocument::doodad_markers() const noexcept {
  return doodad_markers_;
}
const std::vector<SpriteMarker>& EditorDocument::sprite_markers() const noexcept {
  return sprite_markers_;
}
const std::vector<LocationMarker>&
EditorDocument::location_markers() const noexcept {
  return location_markers_;
}

bool EditorDocument::create_location(const std::uint32_t left,
                                     const std::uint32_t top,
                                     const std::uint32_t right,
                                     const std::uint32_t bottom) noexcept {
  if (!editing_ready_) {
    return false;
  }
  const std::uint32_t map_right =
      static_cast<std::uint32_t>(width_) * tile_width;
  const std::uint32_t map_bottom =
      static_cast<std::uint32_t>(height_) * tile_height;
  const std::uint32_t normalized_left = (std::min)(left, right);
  const std::uint32_t normalized_top = (std::min)(top, bottom);
  const std::uint32_t normalized_right = (std::min)(
      map_right, (std::max)(left, right));
  const std::uint32_t normalized_bottom = (std::min)(
      map_bottom, (std::max)(top, bottom));
  if (normalized_left >= normalized_right ||
      normalized_top >= normalized_bottom) {
    return false;
  }
  try {
    std::vector<std::uint8_t> payload = location_payload_;
    payload.resize(starcraft::data::chk_location_slot_count *
                       starcraft::data::chk_location_record_bytes,
                   0U);
    std::size_t selected = starcraft::data::chk_location_slot_count;
    for (std::size_t slot = 0U;
         slot < starcraft::data::chk_location_slot_count; ++slot) {
      const std::size_t offset =
          slot * starcraft::data::chk_location_record_bytes;
      if (read_u32(payload.data(), offset + 8U) <=
              read_u32(payload.data(), offset) ||
          read_u32(payload.data(), offset + 12U) <=
              read_u32(payload.data(), offset + 4U)) {
        selected = slot;
        break;
      }
    }
    if (selected == starcraft::data::chk_location_slot_count) {
      return false;
    }
    const std::size_t offset =
        selected * starcraft::data::chk_location_record_bytes;
    write_u32(payload, offset, normalized_left);
    write_u32(payload, offset + 4U, normalized_top);
    write_u32(payload, offset + 8U, normalized_right);
    write_u32(payload, offset + 12U, normalized_bottom);
    write_u16(payload, offset + 16U, 0U);
    write_u16(payload, offset + 18U, 0U);
    return commit_section_edit(starcraft::data::chk_section_locations,
                               std::move(payload));
  } catch (...) {
    return false;
  }
}

bool EditorDocument::erase_location_at(const std::uint16_t x,
                                       const std::uint16_t y) noexcept {
  const auto selected = std::find_if(
      location_markers_.rbegin(), location_markers_.rend(),
      [x, y](const LocationMarker& marker) {
        return x >= marker.left && x < marker.right && y >= marker.top &&
               y < marker.bottom;
      });
  if (selected == location_markers_.rend()) {
    return false;
  }
  try {
    std::vector<std::uint8_t> payload = location_payload_;
    const std::size_t offset =
        static_cast<std::size_t>(selected->slot) *
        starcraft::data::chk_location_record_bytes;
    if (offset + starcraft::data::chk_location_record_bytes > payload.size()) {
      return false;
    }
    std::fill_n(payload.begin() + static_cast<std::ptrdiff_t>(offset),
                starcraft::data::chk_location_record_bytes,
                static_cast<std::uint8_t>(0U));
    return commit_section_edit(starcraft::data::chk_section_locations,
                               std::move(payload));
  } catch (...) {
    return false;
  }
}

bool EditorDocument::begin_fog_edit() noexcept {
  if (!editing_ready_ || fog_edit_active_ || fog_mask_.size() != tiles_.size()) {
    return false;
  }
  try {
    active_fog_before_ = fog_mask_;
    fog_edit_active_ = true;
    return true;
  } catch (...) {
    active_fog_before_.clear();
    return false;
  }
}

bool EditorDocument::paint_fog(const std::uint16_t x,
                               const std::uint16_t y,
                               const std::uint16_t brush_extent,
                               const bool obscured) noexcept {
  if (!fog_edit_active_ || x >= width_ || y >= height_ ||
      brush_extent == 0U) {
    return false;
  }
  const int radius = static_cast<int>(brush_extent) / 2;
  for (int offset_y = -radius; offset_y <= radius; ++offset_y) {
    const int tile_y = static_cast<int>(y) + offset_y;
    if (tile_y < 0 || tile_y >= height_) {
      continue;
    }
    for (int offset_x = -radius; offset_x <= radius; ++offset_x) {
      const int tile_x = static_cast<int>(x) + offset_x;
      if (tile_x < 0 || tile_x >= width_) {
        continue;
      }
      fog_mask_[static_cast<std::size_t>(tile_y) * width_ + tile_x] =
          obscured ? 0xFFU : 0U;
    }
  }
  return true;
}

bool EditorDocument::commit_fog_edit() noexcept {
  if (!fog_edit_active_) {
    return false;
  }
  fog_edit_active_ = false;
  if (fog_mask_ == active_fog_before_) {
    active_fog_before_.clear();
    return true;
  }
  std::vector<std::uint8_t> before = std::move(active_fog_before_);
  if (commit_section_edit(starcraft::data::chk_section_fog_mask, fog_mask_)) {
    return true;
  }
  fog_mask_ = std::move(before);
  return false;
}

void EditorDocument::cancel_fog_edit() noexcept {
  if (!fog_edit_active_) {
    return;
  }
  fog_mask_ = std::move(active_fog_before_);
  fog_edit_active_ = false;
}

bool EditorDocument::fog_at(const std::uint16_t x,
                            const std::uint16_t y) const noexcept {
  return x < width_ && y < height_ &&
         static_cast<std::size_t>(y) * width_ + x < fog_mask_.size() &&
         fog_mask_[static_cast<std::size_t>(y) * width_ + x] != 0U;
}

bool EditorDocument::player_settings(
    std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& ownership,
    std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& races) const noexcept {
  ownership = {};
  races = {};
  const formats::ChkSection* const ownership_section =
      chk_.section(starcraft::data::chk_section_ownership);
  const formats::ChkSection* const race_section =
      chk_.section(starcraft::data::chk_section_races);
  if (ownership_section == nullptr || race_section == nullptr ||
      ownership_section->payload.size() < ownership.size() ||
      race_section->payload.size() < races.size()) {
    return false;
  }
  std::copy_n(ownership_section->payload.begin(), ownership.size(),
              ownership.begin());
  std::copy_n(race_section->payload.begin(), races.size(), races.begin());
  return true;
}

bool EditorDocument::set_player_settings(
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& ownership,
    const std::array<std::uint8_t, starcraft::data::chk_player_slot_count>& races) noexcept {
  if (!editing_ready_) {
    return false;
  }
  try {
    const formats::ChkSection* const old_ownership =
        chk_.section(starcraft::data::chk_section_ownership);
    const formats::ChkSection* const old_races =
        chk_.section(starcraft::data::chk_section_races);
    std::vector<std::uint8_t> ownership_payload =
        old_ownership == nullptr ? std::vector<std::uint8_t>{}
                                 : old_ownership->payload;
    std::vector<std::uint8_t> race_payload =
        old_races == nullptr ? std::vector<std::uint8_t>{} : old_races->payload;
    ownership_payload.resize((std::max)(ownership_payload.size(),
                                        ownership.size()), 0U);
    race_payload.resize((std::max)(race_payload.size(), races.size()), 0U);
    std::copy(ownership.begin(), ownership.end(), ownership_payload.begin());
    std::copy(races.begin(), races.end(), race_payload.begin());
    if (old_ownership != nullptr && old_races != nullptr &&
        old_ownership->payload == ownership_payload &&
        old_races->payload == race_payload) {
      return true;
    }
    formats::ChkDocument candidate = chk_;
    const auto write = [&candidate](const std::uint32_t tag,
                                    const bool exists,
                                    const std::vector<std::uint8_t>& payload) {
      return exists ? candidate.replace_section(tag, 0U, payload)
                    : candidate.append_section(tag, payload);
    };
    if (!write(starcraft::data::chk_section_ownership,
               old_ownership != nullptr, ownership_payload) ||
        !write(starcraft::data::chk_section_races, old_races != nullptr,
               race_payload)) {
      return false;
    }
    undo::TileEditCommand command{};
    command.section_changes.push_back(
        {starcraft::data::chk_section_ownership, 0U, old_ownership != nullptr,
         true,
         old_ownership == nullptr ? std::vector<std::uint8_t>{}
                                  : old_ownership->payload,
         ownership_payload});
    command.section_changes.push_back(
        {starcraft::data::chk_section_races, 0U, old_races != nullptr, true,
         old_races == nullptr ? std::vector<std::uint8_t>{}
                              : old_races->payload,
         race_payload});
    formats::ChkDocument previous = chk_;
    chk_ = std::move(candidate);
    if (!undo_stack_.push(std::move(command))) {
      chk_ = std::move(previous);
      return false;
    }
    section_count_ = chk_.section_count();
    return true;
  } catch (...) {
    return false;
  }
}

bool EditorDocument::scenario_properties(
    ScenarioProperties& properties) const noexcept {
  properties = {};
  try {
    formats::ScenarioPropertyReferences references{};
    const formats::ChkSection* const section =
        chk_.section(starcraft::data::chk_section_scenario_properties);
    if (section != nullptr &&
        !formats::parse_scenario_property_references(section->payload,
                                                      references)) {
      return false;
    }
    ScenarioStringBinding strings{};
    if (!load_scenario_strings(chk_, strings)) {
      return false;
    }
    properties.name = strings.table.value(references.name_string_id);
    properties.description =
        strings.table.value(references.description_string_id);
    return true;
  } catch (...) {
    properties = {};
    return false;
  }
}

bool EditorDocument::set_scenario_properties(
    const ScenarioProperties& properties) noexcept {
  if (!editing_ready_ || properties.name.find('\0') != std::string::npos ||
      properties.description.find('\0') != std::string::npos) {
    return false;
  }
  ScenarioProperties current{};
  if (!scenario_properties(current)) {
    return false;
  }
  if (current.name == properties.name &&
      current.description == properties.description) {
    return true;
  }
  try {
    ScenarioStringBinding strings{};
    if (!load_scenario_strings(chk_, strings)) {
      return false;
    }
    const std::size_t old_string_count = strings.table.size();
    formats::ScenarioPropertyReferences references{};
    if (!strings.table.find_or_append(properties.name,
                                      references.name_string_id) ||
        !strings.table.find_or_append(properties.description,
                                      references.description_string_id)) {
      return false;
    }

    const formats::ChkSection* const existing =
        chk_.section(starcraft::data::chk_section_scenario_properties);
    std::vector<std::uint8_t> property_payload =
        existing == nullptr ? std::vector<std::uint8_t>{}
                            : existing->payload;
    if (!formats::write_scenario_property_references(references,
                                                      property_payload)) {
      return false;
    }
    std::vector<std::pair<std::uint32_t, std::vector<std::uint8_t>>>
        replacements{};
    if (strings.table.size() != old_string_count) {
      std::vector<std::uint8_t> string_payload{};
      std::string error{};
      if (!strings.table.serialize(string_payload, error)) {
        return false;
      }
      replacements.emplace_back(strings.tag, std::move(string_payload));
    }
    replacements.emplace_back(
        starcraft::data::chk_section_scenario_properties,
        std::move(property_payload));
    return commit_section_edits(std::move(replacements));
  } catch (...) {
    return false;
  }
}

bool EditorDocument::scenario_forces(ScenarioForces& forces) const noexcept {
  forces = {};
  try {
    formats::ForceSectionData data{};
    const formats::ChkSection* const section =
        chk_.section(starcraft::data::chk_section_forces);
    if (section != nullptr) {
      if (!formats::parse_force_section(section->payload, data)) {
        return false;
      }
    } else {
      data.supports_flags =
          chk_.dialect().dialect == formats::ChkDialect::retail_chk;
    }
    ScenarioStringBinding strings{};
    if (!load_scenario_strings(chk_, strings)) {
      return false;
    }
    forces.player_force = data.player_force;
    forces.flags = data.flags;
    forces.supports_flags = data.supports_flags;
    for (std::size_t force = 0U; force < formats::force_count; ++force) {
      forces.names[force] = strings.table.value(data.name_string_ids[force]);
    }
    return true;
  } catch (...) {
    forces = {};
    return false;
  }
}

bool EditorDocument::set_scenario_forces(
    const ScenarioForces& forces) noexcept {
  if (!editing_ready_ ||
      std::any_of(forces.player_force.begin(), forces.player_force.end(),
                  [](const std::uint8_t force) {
                    return force >= formats::force_count;
                  }) ||
      std::any_of(forces.names.begin(), forces.names.end(),
                  [](const std::string& name) {
                    return name.find('\0') != std::string::npos;
                  })) {
    return false;
  }
  ScenarioForces current{};
  if (!scenario_forces(current)) {
    return false;
  }
  if (current.player_force == forces.player_force &&
      current.names == forces.names &&
      (!current.supports_flags || current.flags == forces.flags)) {
    return true;
  }
  try {
    const formats::ChkSection* const existing =
        chk_.section(starcraft::data::chk_section_forces);
    formats::ForceSectionData data{};
    if (existing != nullptr) {
      if (!formats::parse_force_section(existing->payload, data)) {
        return false;
      }
    } else {
      data.supports_flags =
          chk_.dialect().dialect == formats::ChkDialect::retail_chk;
    }
    data.player_force = forces.player_force;
    if (data.supports_flags) {
      data.flags = forces.flags;
    }

    ScenarioStringBinding strings{};
    if (!load_scenario_strings(chk_, strings)) {
      return false;
    }
    const std::size_t old_string_count = strings.table.size();
    for (std::size_t force = 0U; force < formats::force_count; ++force) {
      if (!strings.table.find_or_append(forces.names[force],
                                        data.name_string_ids[force])) {
        return false;
      }
    }

    std::vector<std::uint8_t> force_payload =
        existing == nullptr ? std::vector<std::uint8_t>{}
                            : existing->payload;
    if (!formats::write_force_section(data, force_payload)) {
      return false;
    }
    std::vector<std::pair<std::uint32_t, std::vector<std::uint8_t>>>
        replacements{};
    if (strings.table.size() != old_string_count) {
      std::vector<std::uint8_t> string_payload{};
      std::string error{};
      if (!strings.table.serialize(string_payload, error)) {
        return false;
      }
      replacements.emplace_back(strings.tag, std::move(string_payload));
    }
    replacements.emplace_back(starcraft::data::chk_section_forces,
                              std::move(force_payload));
    return commit_section_edits(std::move(replacements));
  } catch (...) {
    return false;
  }
}

const std::vector<std::uint16_t>& EditorDocument::object_brushes(
    const EditorLayer layer) const noexcept {
  switch (layer) {
    case EditorLayer::doodads:
      return doodad_brushes_;
    case EditorLayer::units:
      return unit_brushes_;
    case EditorLayer::sprites:
      return sprite_brushes_;
    case EditorLayer::locations:
    case EditorLayer::fog: {
      static const std::vector<std::uint16_t> empty{};
      return empty;
    }
    case EditorLayer::terrain:
      return terrain_brushes_;
  }
  return terrain_brushes_;
}
const std::vector<std::uint16_t>& EditorDocument::terrain_brushes() const noexcept {
  return terrain_brushes_;
}

std::string_view EditorDocument::terrain_brush_name(
    const std::uint16_t brush_tile_id) const noexcept {
  const terrain::IsomBrush* const brush =
      isom_catalog_.brush_for_preview_tile(brush_tile_id);
  return brush == nullptr ? std::string_view{} : brush->name;
}

bool EditorDocument::logical_terrain_ready() const noexcept {
  return isom_.valid() && isom_catalog_.valid() && isom_topology_.valid();
}

bool EditorDocument::valid_terrain_tile(
    const std::uint16_t tile_id) const noexcept {
  return tileset_.terrain_tile_valid(tile_id);
}

bool EditorDocument::terrain_group_id(const std::uint16_t tile_id,
                                      std::uint16_t& group_id) const noexcept {
  return tileset_.terrain_group_id(tile_id, group_id);
}

bool EditorDocument::same_terrain_group(const std::uint16_t left,
                                        const std::uint16_t right) const noexcept {
  std::uint16_t left_group{};
  std::uint16_t right_group{};
  return terrain_group_id(left, left_group) &&
         terrain_group_id(right, right_group) && left_group == right_group;
}

bool EditorDocument::terrain_brush_at(
    const std::uint16_t x,
    const std::uint16_t y,
    std::uint16_t& brush_tile_id) const noexcept {
  std::uint16_t tile{};
  if (!tile_at(x, y, tile)) {
    return false;
  }
  for (const std::uint16_t brush : terrain_brushes_) {
    if (same_terrain_group(brush, tile)) {
      brush_tile_id = brush;
      return true;
    }
  }
  if (!logical_terrain_ready()) {
    return false;
  }
  const formats::IsomEntry* const rect = isom_.entry(x / 2U, y);
  if (rect == nullptr) {
    return false;
  }
  for (const formats::IsomLink link : rect->links) {
    const std::size_t isom_value =
        static_cast<std::size_t>((link.raw & 0x7FFEU) >> 4U);
    if (isom_value >= isom_catalog_.links().size()) {
      continue;
    }
    const terrain::IsomBrush* const brush =
        isom_catalog_.brush_for_transition_type(
            isom_catalog_.links()[isom_value].terrain_type);
    if (brush != nullptr) {
      brush_tile_id = brush->preview_tile;
      return true;
    }
  }
  return false;
}

bool EditorDocument::terrain_variant(const std::uint16_t brush_tile_id,
                                     const std::uint16_t x,
                                     const std::uint16_t y,
                                     std::uint16_t& tile_id) const noexcept {
  std::uint16_t group_id{};
  if (!terrain_group_id(brush_tile_id, group_id)) {
    return false;
  }
  starcraft::gds::TerrainGroupVariants variants{};
  if (!tileset_.terrain_group_variants(group_id, variants)) {
    return false;
  }
  const std::uint32_t hash =
      static_cast<std::uint32_t>(x) * 73856093U ^
      static_cast<std::uint32_t>(y) * 19349663U ^
      static_cast<std::uint32_t>(group_id) * 83492791U;
  if (variants.rare_count != 0U && hash % 20U == 0U) {
    tile_id = variants.rare[(hash / 20U) % variants.rare_count];
  } else if (variants.common_count != 0U) {
    tile_id = variants.common[hash % variants.common_count];
  } else {
    tile_id = variants.rare[hash % variants.rare_count];
  }
  return true;
}

bool EditorDocument::tile_at(const std::uint16_t x,
                             const std::uint16_t y,
                             std::uint16_t& tile_id) const noexcept {
  if (x >= width_ || y >= height_) {
    return false;
  }
  tile_id = tiles_[static_cast<std::size_t>(y) * width_ + x];
  return true;
}

const EditorDocument::TilePixels* EditorDocument::tile_pixels(
    const std::uint16_t tile_id) const noexcept {
  const auto existing = tile_cache_.find(tile_id);
  if (existing != tile_cache_.end()) {
    return &existing->second;
  }

  starcraft::gds::IndexedMapTile indexed{};
  if (!tileset_.render(tile_id, indexed)) {
    return nullptr;
  }
  try {
    TilePixels pixels{};
    const auto& palette = tileset_.palette();
    if (palette.size() != 1024U) {
      return nullptr;
    }
    for (std::size_t pixel = 0; pixel < pixels.size(); ++pixel) {
      const std::size_t color = 4U * indexed.pixels[pixel];
      pixels[pixel] =
          0xFF000000U | static_cast<std::uint32_t>(palette[color + 2U]) |
          (static_cast<std::uint32_t>(palette[color + 1U]) << 8U) |
          (static_cast<std::uint32_t>(palette[color]) << 16U);
    }
    const auto inserted = tile_cache_.emplace(tile_id, std::move(pixels));
    return &inserted.first->second;
  } catch (...) {
    return nullptr;
  }
}

const ObjectArtFrame* EditorDocument::object_art(
    const EditorLayer layer,
    const std::uint16_t type) const noexcept {
  const auto* cache = &unit_art_;
  if (layer == EditorLayer::doodads) {
    cache = &doodad_art_;
  } else if (layer == EditorLayer::sprites) {
    cache = &sprite_art_;
  } else if (layer != EditorLayer::units) {
    return nullptr;
  }
  const auto found = cache->find(type);
  return found == cache->end() ? nullptr : found->second.get();
}

std::string_view EditorDocument::object_brush_name(
    const EditorLayer layer,
    const std::uint16_t type) const noexcept {
  if (layer != EditorLayer::units || stat_text_.empty()) {
    return {};
  }
  const starcraft::data::StringTableView strings{stat_text_.data(),
                                                 stat_text_.size()};
  return strings.valid() ? strings.one_based(static_cast<std::uint16_t>(type + 1U))
                         : std::string_view{};
}

const std::vector<std::uint8_t>& EditorDocument::art_palette() const noexcept {
  return tileset_.palette();
}

const TeamColorTable& EditorDocument::team_colors() const noexcept {
  return team_colors_;
}

void EditorDocument::reset() noexcept {
  path_.clear();
  data_root_.clear();
  title_.clear();
  chk_ = {};
  isom_ = {};
  tiles_.clear();
  editor_tiles_.clear();
  terrain_brushes_.clear();
  unit_markers_.clear();
  doodad_markers_.clear();
  sprite_markers_.clear();
  location_markers_.clear();
  location_payload_.clear();
  fog_mask_.clear();
  active_fog_before_.clear();
  unit_records_.clear();
  doodad_records_.clear();
  sprite_records_.clear();
  doodad_brushes_.clear();
  unit_brushes_.clear();
  sprite_brushes_.clear();
  doodad_templates_.clear();
  unit_prototypes_.clear();
  sprite_prototypes_.clear();
  unit_art_.clear();
  sprite_art_.clear();
  doodad_art_.clear();
  team_colors_ = {};
  stat_text_.clear();
  tileset_ = {};
  isom_catalog_ = {};
  isom_topology_ = {};
  tile_cache_.clear();
  undo_stack_.clear();
  active_tile_edit_.changes.clear();
  active_tile_edit_.editor_changes.clear();
  active_tile_edit_.isom_changes.clear();
  active_tile_edit_.section_changes.clear();
  active_tiles_before_.clear();
  active_editor_tiles_before_.clear();
  active_isom_before_ = {};
  tileset_name_.clear();
  width_ = 0;
  height_ = 0;
  tileset_id_ = 0;
  section_count_ = 0;
  unit_count_ = 0;
  doodad_count_ = 0;
  sprite_count_ = 0;
  format_ = ScenarioFormat::unknown_chk;
  source_is_archive_ = false;
  editing_ready_ = false;
  tile_edit_active_ = false;
  fog_edit_active_ = false;
  has_editor_tiles_ = false;
}

bool EditorDocument::build_active_command() noexcept {
  if (active_tiles_before_.size() != tiles_.size() ||
      active_editor_tiles_before_.size() != editor_tiles_.size() ||
      active_isom_before_.valid() != isom_.valid() ||
      (isom_.valid() &&
       active_isom_before_.entry_count() != isom_.entry_count())) {
    return false;
  }
  try {
    undo::TileEditCommand command{};
    for (std::size_t index = 0U; index < tiles_.size(); ++index) {
      if (active_tiles_before_[index] != tiles_[index]) {
        command.changes.push_back(
            {index, active_tiles_before_[index], tiles_[index]});
      }
      if (active_editor_tiles_before_[index] != editor_tiles_[index]) {
        command.editor_changes.push_back(
            {index, active_editor_tiles_before_[index], editor_tiles_[index]});
      }
    }
    if (isom_.valid()) {
      const std::size_t columns = isom_.column_count();
      for (std::size_t index = 0U; index < isom_.entry_count(); ++index) {
        const formats::IsomEntry* const before = active_isom_before_.entry(
            index % columns, index / columns);
        const formats::IsomEntry* const after = isom_.entry(
            index % columns, index / columns);
        bool different = before == nullptr || after == nullptr;
        if (!different) {
          for (std::size_t link = 0U; link < before->links.size(); ++link) {
            different = different ||
                        before->links[link].raw != after->links[link].raw;
          }
        }
        if (different) {
          if (before == nullptr || after == nullptr) {
            return false;
          }
          command.isom_changes.push_back({index, *before, *after});
        }
      }
    }
    active_tile_edit_ = std::move(command);
    return true;
  } catch (...) {
    active_tile_edit_ = {};
    return false;
  }
}

bool EditorDocument::make_tile_payload(
    const std::vector<std::uint16_t>& tiles,
    std::vector<std::uint8_t>& payload) noexcept {
  try {
    payload.resize(tiles.size() * sizeof(std::uint16_t));
    for (std::size_t index = 0U; index < tiles.size(); ++index) {
      payload[2U * index] = static_cast<std::uint8_t>(tiles[index]);
      payload[2U * index + 1U] =
          static_cast<std::uint8_t>(tiles[index] >> 8U);
    }
    return true;
  } catch (...) {
    payload.clear();
    return false;
  }
}

bool EditorDocument::update_chk_terrain_layers() noexcept {
  try {
    std::vector<std::uint8_t> game_payload{};
    std::vector<std::uint8_t> editor_payload{};
    std::vector<std::uint8_t> isom_payload{};
    const bool write_editor_tiles = has_editor_tiles_ || isom_.valid();
    if (!make_tile_payload(tiles_, game_payload) ||
        (write_editor_tiles &&
         !make_tile_payload(editor_tiles_, editor_payload)) ||
        (isom_.valid() && !isom_.serialize(isom_payload))) {
      return false;
    }
    formats::ChkDocument candidate = chk_;
    if (!candidate.replace_section(starcraft::data::chk_section_tiles, 0U,
                                   std::move(game_payload))) {
      return false;
    }
    if (write_editor_tiles) {
      const bool editor_written =
          candidate.count(starcraft::data::chk_section_editor_tiles) != 0U
              ? candidate.replace_section(
                    starcraft::data::chk_section_editor_tiles, 0U,
                    std::move(editor_payload))
              : candidate.append_section(
                    starcraft::data::chk_section_editor_tiles,
                    std::move(editor_payload));
      if (!editor_written) {
        return false;
      }
    }
    if (isom_.valid() &&
        !candidate.replace_section(starcraft::data::chk_section_isom, 0U,
                                   std::move(isom_payload))) {
      return false;
    }
    chk_ = std::move(candidate);
    has_editor_tiles_ = write_editor_tiles;
    return true;
  } catch (...) {
    return false;
  }
}

bool EditorDocument::parse_chk(std::wstring& error) noexcept {
  const formats::ChkSection* const dimensions =
      chk_.section(starcraft::data::chk_section_dimensions);
  const formats::ChkSection* const era =
      chk_.section(starcraft::data::chk_section_era);
  if (!chk_.valid() || dimensions == nullptr ||
      dimensions->payload.size() != 4U || era == nullptr ||
      era->payload.size() < 2U) {
    error = L"The scenario.chk section stream is invalid or incomplete.";
    return false;
  }
  width_ = read_u16(dimensions->payload.data(), 0U);
  height_ = read_u16(dimensions->payload.data(), 2U);
  tileset_id_ = read_u16(era->payload.data(), 0U);
  if (width_ == 0U || height_ == 0U) {
    error = L"The scenario dimensions are empty.";
    return false;
  }
  section_count_ = chk_.section_count();

  const formats::ChkSection* const isom_section =
      chk_.section(starcraft::data::chk_section_isom);
  if (isom_section != nullptr) {
    std::string isom_error{};
    if (!isom_.parse(isom_section->payload.data(),
                     isom_section->payload.size(), width_, height_,
                     isom_error)) {
      error = L"The scenario ISOM section is invalid: " +
              widen_ascii(isom_error);
      return false;
    }
  }

  const std::size_t tile_count = static_cast<std::size_t>(width_) * height_;
  const formats::ChkSection* const tile_section =
      chk_.section(starcraft::data::chk_section_tiles);
  if (tile_section == nullptr ||
      tile_section->payload.size() != tile_count * sizeof(std::uint16_t)) {
    error = L"The scenario MTXM section does not match its dimensions.";
    return false;
  }
  try {
    tiles_.resize(tile_count);
    for (std::size_t index = 0; index < tile_count; ++index) {
      tiles_[index] = read_u16(tile_section->payload.data(), 2U * index);
    }
    const formats::ChkSection* const editor_tile_section =
        chk_.section(starcraft::data::chk_section_editor_tiles);
    has_editor_tiles_ = editor_tile_section != nullptr;
    if (editor_tile_section != nullptr) {
      if (editor_tile_section->payload.size() !=
          tile_count * sizeof(std::uint16_t)) {
        error = L"The scenario TILE section does not match its dimensions.";
        return false;
      }
      editor_tiles_.resize(tile_count);
      for (std::size_t index = 0U; index < tile_count; ++index) {
        editor_tiles_[index] =
            read_u16(editor_tile_section->payload.data(), 2U * index);
      }
    } else {
      editor_tiles_ = tiles_;
    }
  } catch (...) {
    error = L"There was not enough memory to load the scenario terrain.";
    return false;
  }

  format_ = chk_.dialect().dialect;
  return parse_object_sections(error) && parse_auxiliary_sections(error);
}

bool EditorDocument::parse_auxiliary_sections(std::wstring& error) noexcept {
  error.clear();
  location_markers_.clear();
  location_payload_.clear();
  fog_mask_.clear();
  try {
    const formats::ChkSection* const locations =
        chk_.section(starcraft::data::chk_section_locations);
    if (locations != nullptr) {
      if (locations->payload.size() %
              starcraft::data::chk_location_record_bytes !=
          0U) {
        error = L"The MRGN location section has an invalid record width.";
        return false;
      }
      location_payload_ = locations->payload;
      const std::size_t slots = (std::min)(
          starcraft::data::chk_location_slot_count,
          location_payload_.size() /
              starcraft::data::chk_location_record_bytes);
      for (std::size_t slot = 0U; slot < slots; ++slot) {
        const std::size_t offset =
            slot * starcraft::data::chk_location_record_bytes;
        LocationMarker marker{read_u32(location_payload_.data(), offset),
                              read_u32(location_payload_.data(), offset + 4U),
                              read_u32(location_payload_.data(), offset + 8U),
                              read_u32(location_payload_.data(), offset + 12U),
                              read_u16(location_payload_.data(), offset + 16U),
                              read_u16(location_payload_.data(), offset + 18U),
                              static_cast<std::uint16_t>(slot)};
        if (marker.right > marker.left && marker.bottom > marker.top) {
          location_markers_.push_back(marker);
        }
      }
    }
    const formats::ChkSection* const fog =
        chk_.section(starcraft::data::chk_section_fog_mask);
    if (fog != nullptr && fog->payload.size() == tiles_.size()) {
      fog_mask_ = fog->payload;
    } else if (fog == nullptr) {
      fog_mask_.assign(tiles_.size(), 0U);
    } else {
      error = L"The MASK fog section does not match the map dimensions.";
      return false;
    }
    return true;
  } catch (...) {
    error = L"There was not enough memory to load locations and fog.";
    location_markers_.clear();
    location_payload_.clear();
    fog_mask_.clear();
    return false;
  }
}

bool EditorDocument::parse_object_sections(std::wstring& error) noexcept {
  error.clear();
  unit_records_.clear();
  doodad_records_.clear();
  sprite_records_.clear();
  unit_markers_.clear();
  doodad_markers_.clear();
  sprite_markers_.clear();
  const formats::ChkDialectInfo& dialect = chk_.dialect();
  const auto parse = [this, &error](
                         const formats::PlacementRecordLayout& layout,
                         std::vector<formats::PlacementRecord>& records,
                         const wchar_t* const label) {
    if (layout.record_bytes == 0U) {
      return true;
    }
    const formats::ChkSection* const section = chk_.section(layout.section_tag);
    if (section == nullptr) {
      return true;
    }
    if (formats::parse_placement_records(section->payload, layout, records)) {
      return true;
    }
    error = L"The ";
    error += label;
    error += L" placement section has an invalid record width.";
    return false;
  };
  if (!parse(unit_placement_layout(dialect), unit_records_, L"unit") ||
      !parse(dialect.doodads, doodad_records_, L"doodad") ||
      !parse(dialect.sprites, sprite_records_, L"sprite")) {
    return false;
  }
  try {
    unit_markers_.reserve(unit_records_.size());
    for (const formats::PlacementRecord& record : unit_records_) {
      unit_markers_.push_back(
          {record.x, record.y, record.type, record.owner});
    }
    doodad_markers_.reserve(doodad_records_.size());
    for (const formats::PlacementRecord& record : doodad_records_) {
      doodad_markers_.push_back(
          {record.x, record.y, record.type, record.owner, record.enabled});
    }
    sprite_markers_.reserve(sprite_records_.size());
    for (const formats::PlacementRecord& record : sprite_records_) {
      sprite_markers_.push_back(
          {record.x, record.y, record.type, record.owner, record.flags});
    }
  } catch (...) {
    error = L"There was not enough memory to load placed objects.";
    return false;
  }
  unit_count_ = unit_records_.size();
  doodad_count_ = doodad_records_.size();
  sprite_count_ = sprite_records_.size();
  return true;
}

bool EditorDocument::build_object_brush_inventory() noexcept {
  doodad_brushes_.clear();
  unit_brushes_.clear();
  sprite_brushes_.clear();
  doodad_templates_.clear();
  unit_prototypes_.clear();
  sprite_prototypes_.clear();
  try {
    const auto collect = [](const std::vector<formats::PlacementRecord>& records,
                            std::vector<std::uint16_t>& brushes,
                            std::vector<formats::PlacementRecord>& prototypes) {
      std::set<std::uint16_t> seen{};
      for (const formats::PlacementRecord& record : records) {
        if (seen.insert(record.type).second) {
          brushes.push_back(record.type);
          prototypes.push_back(record);
        }
      }
      std::sort(brushes.begin(), brushes.end());
    };
    collect(unit_records_, unit_brushes_, unit_prototypes_);
    collect(sprite_records_, sprite_brushes_, sprite_prototypes_);
    const formats::PlacementRecordLayout unit_layout =
        unit_placement_layout(chk_.dialect());
    for (std::uint16_t type = 0U; type < 228U; ++type) {
      if (std::find(unit_brushes_.begin(), unit_brushes_.end(), type) ==
          unit_brushes_.end()) {
        unit_brushes_.push_back(type);
        unit_prototypes_.push_back(
            formats::make_placement_record(unit_layout, type, 0U, 0U));
      }
    }
    for (std::uint16_t type = 0U; type < 267U; ++type) {
      if (std::find(sprite_brushes_.begin(), sprite_brushes_.end(), type) ==
          sprite_brushes_.end()) {
        sprite_brushes_.push_back(type);
        sprite_prototypes_.push_back(formats::make_placement_record(
            chk_.dialect().sprites, type, 0U, 0U));
      }
    }
    std::sort(unit_brushes_.begin(), unit_brushes_.end());
    std::sort(sprite_brushes_.begin(), sprite_brushes_.end());

    std::set<std::uint16_t> seen_doodads{};
    for (const formats::PlacementRecord& record : doodad_records_) {
      if (!seen_doodads.insert(record.type).second) {
        continue;
      }
      doodad_brushes_.push_back(record.type);
      DoodadTemplate brush{};
      brush.type = record.type;
      brush.x_alignment = static_cast<std::uint8_t>(record.x % tile_width);
      brush.y_alignment = static_cast<std::uint8_t>(record.y % tile_height);
      brush.prototype = record;

      const int center_x = record.x / static_cast<int>(tile_width);
      const int center_y = record.y / static_cast<int>(tile_height);
      int start_x{-1};
      int start_y{-1};
      int best_distance = 999;
      for (int dy = -3; dy <= 3; ++dy) {
        for (int dx = -3; dx <= 3; ++dx) {
          const int x = center_x + dx;
          const int y = center_y + dy;
          if (x < 0 || y < 0 || x >= width_ || y >= height_) {
            continue;
          }
          const std::uint16_t tile =
              tiles_[static_cast<std::size_t>(y) * width_ + x];
          const std::uint16_t group = static_cast<std::uint16_t>(tile >> 4U);
          const int distance = dx * dx + dy * dy;
          if (group >= 1024U && distance < best_distance) {
            start_x = x;
            start_y = y;
            best_distance = distance;
          }
        }
      }
      if (start_x >= 0) {
        std::queue<std::pair<int, int>> pending{};
        std::set<std::pair<int, int>> visited{};
        pending.emplace(start_x, start_y);
        while (!pending.empty() && visited.size() < 256U) {
          const auto cell = pending.front();
          pending.pop();
          if (!visited.insert(cell).second) {
            continue;
          }
          const int x = cell.first;
          const int y = cell.second;
          if (x < 0 || y < 0 || x >= width_ || y >= height_ ||
              std::abs(x - center_x) > 8 || std::abs(y - center_y) > 8) {
            continue;
          }
          const std::uint16_t tile =
              tiles_[static_cast<std::size_t>(y) * width_ + x];
          if ((tile >> 4U) < 1024U) {
            continue;
          }
          brush.cells.push_back(
              {static_cast<std::int16_t>(
                   x * static_cast<int>(tile_width) - record.x),
               static_cast<std::int16_t>(
                   y * static_cast<int>(tile_height) - record.y),
               tile});
          pending.emplace(x - 1, y);
          pending.emplace(x + 1, y);
          pending.emplace(x, y - 1);
          pending.emplace(x, y + 1);
        }
      }
      doodad_templates_.push_back(std::move(brush));
    }
    std::sort(doodad_brushes_.begin(), doodad_brushes_.end());
    return true;
  } catch (...) {
    doodad_brushes_.clear();
    unit_brushes_.clear();
    sprite_brushes_.clear();
    doodad_templates_.clear();
    unit_prototypes_.clear();
    sprite_prototypes_.clear();
    return false;
  }
}

bool EditorDocument::normalize_object_prototypes() noexcept {
  try {
    const formats::PlacementRecordLayout units =
        unit_placement_layout(chk_.dialect());
    const auto normalize = [](formats::PlacementRecord& record,
                              const formats::PlacementRecordLayout& layout) {
      if (record.raw.size() == layout.record_bytes) {
        return;
      }
      formats::PlacementRecord converted = formats::make_placement_record(
          layout, record.type, record.x, record.y, record.owner);
      converted.enabled = record.enabled;
      converted.flags = record.flags;
      record = std::move(converted);
    };
    for (formats::PlacementRecord& prototype : unit_prototypes_) {
      normalize(prototype, units);
    }
    for (formats::PlacementRecord& prototype : sprite_prototypes_) {
      normalize(prototype, chk_.dialect().sprites);
    }
    for (DoodadTemplate& doodad : doodad_templates_) {
      normalize(doodad.prototype, chk_.dialect().doodads);
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool EditorDocument::build_object_art_cache(
    starcraft::runtime::StormModule& storm) noexcept {
  unit_art_.clear();
  sprite_art_.clear();
  doodad_art_.clear();
  stat_text_.clear();
  team_colors_ = {};
  try {
    for (auto& owner_colors : team_colors_) {
      for (std::size_t color = 0U; color < owner_colors.size(); ++color) {
        owner_colors[color] = static_cast<std::uint8_t>(8U + color);
      }
    }
    starcraft::data::CoreDataSet data{};
    if (!data.load(storm)) {
      return false;
    }
    (void)storm.load_file(R"(rez\stat_txt.tbl)", stat_text_);
    starcraft::runtime::DecodedPcx unit_colors{};
    if (storm.load_pcx(R"(game\tunit.pcx)", unit_colors) &&
        unit_colors.width == 128U && unit_colors.height == 1U &&
        unit_colors.pixels.size() >= 96U) {
      for (std::size_t owner = 0U; owner < team_colors_.size(); ++owner) {
        std::copy_n(unit_colors.pixels.begin() + owner * 8U, 8U,
                    team_colors_[owner].begin());
      }
    }
    const std::vector<std::uint8_t>& palette = tileset_.palette();
    if (palette.size() != 1024U) {
      return false;
    }
    std::unordered_map<std::uint16_t, std::shared_ptr<ObjectArtFrame>> images{};
    std::set<std::uint16_t> failed_images{};
    const starcraft::lang::IScriptProgramView scripts{
        data.iscript().data(), data.iscript().size()};
    const auto load_image = [&](const std::uint16_t image_id)
        -> std::shared_ptr<ObjectArtFrame> {
      const auto cached = images.find(image_id);
      if (cached != images.end()) {
        return cached->second;
      }
      if (failed_images.find(image_id) != failed_images.end()) {
        return {};
      }
      const std::string path = data.image_grp_path(image_id);
      std::vector<std::uint8_t> group{};
      if (path.empty() || !storm.load_file(path.c_str(), group)) {
        failed_images.insert(image_id);
        return {};
      }
      std::uint16_t frame_index{};
      std::uint16_t script_id{};
      starcraft::lang::IScriptState state{};
      if (data.image_iscript_id(image_id, script_id) &&
          scripts.start(script_id, 0U, state)) {
        const auto result = scripts.tick(state, 0U, 256U);
        if (result == starcraft::lang::IScriptTickResult::yielded ||
            result == starcraft::lang::IScriptTickResult::sleeping) {
          frame_index = state.frame;
        }
      }
      starcraft::gds::DecodedGrpFrame decoded{};
      if (!starcraft::gds::decode_grp_frame(group.data(), group.size(),
                                            frame_index, decoded) &&
          (frame_index == 0U ||
           !starcraft::gds::decode_grp_frame(group.data(), group.size(), 0U,
                                             decoded))) {
        failed_images.insert(image_id);
        return {};
      }
      auto art = std::make_shared<ObjectArtFrame>();
      art->width = decoded.width;
      art->height = decoded.height;
      art->canvas_width = decoded.canvas_width;
      art->canvas_height = decoded.canvas_height;
      art->x_offset = decoded.x_offset;
      art->y_offset = decoded.y_offset;
      art->palette_indices = std::move(decoded.pixels);
      art->opacity = std::move(decoded.opacity);
      art->bgra.resize(art->palette_indices.size());
      for (std::size_t pixel = 0U; pixel < art->bgra.size(); ++pixel) {
        if (pixel >= art->opacity.size() || art->opacity[pixel] == 0U) {
          art->bgra[pixel] = 0U;
          continue;
        }
        const std::size_t color = 4U * art->palette_indices[pixel];
        art->bgra[pixel] =
            0xFF000000U | static_cast<std::uint32_t>(palette[color + 2U]) |
            (static_cast<std::uint32_t>(palette[color + 1U]) << 8U) |
            (static_cast<std::uint32_t>(palette[color]) << 16U);
      }
      images.emplace(image_id, art);
      return art;
    };

    const starcraft::data::DatField* const sprite_images =
        data.sprites().field(0U);
    if (sprite_images != nullptr) {
      const std::size_t sprite_count = sprite_images->element_count();
      for (std::size_t type = 0U; type < sprite_count; ++type) {
        const std::uint16_t sprite_type = static_cast<std::uint16_t>(type);
        if (std::find(sprite_brushes_.begin(), sprite_brushes_.end(),
                      sprite_type) == sprite_brushes_.end()) {
          sprite_brushes_.push_back(sprite_type);
          sprite_prototypes_.push_back(formats::make_placement_record(
              chk_.dialect().sprites, sprite_type, 0U, 0U));
        }
      }
      std::sort(sprite_brushes_.begin(), sprite_brushes_.end());
    }

    for (const std::uint16_t type : unit_brushes_) {
      std::uint16_t image_id{};
      if (data.unit_image_id(type, image_id)) {
        const std::shared_ptr<ObjectArtFrame> art = load_image(image_id);
        if (art != nullptr) {
          unit_art_.emplace(type, art);
        }
      }
    }
    for (const std::uint16_t type : sprite_brushes_) {
      std::uint16_t image_id{};
      if (sprite_images != nullptr && sprite_images->value(type, image_id)) {
        const std::shared_ptr<ObjectArtFrame> art = load_image(image_id);
        if (art != nullptr) {
          sprite_art_.emplace(type, art);
        }
      }
    }
    return build_doodad_art_cache() && !unit_art_.empty() &&
           !sprite_art_.empty();
  } catch (...) {
    unit_art_.clear();
    sprite_art_.clear();
    doodad_art_.clear();
    stat_text_.clear();
    team_colors_ = {};
    return false;
  }
}

bool EditorDocument::build_doodad_art_cache() noexcept {
  doodad_art_.clear();
  try {
    for (const DoodadTemplate& brush : doodad_templates_) {
      if (brush.cells.empty()) {
        continue;
      }
      int minimum_x{};
      int minimum_y{};
      int maximum_x{};
      int maximum_y{};
      bool first = true;
      for (const DoodadTemplateCell& cell : brush.cells) {
        const int left = cell.pixel_offset_x;
        const int top = cell.pixel_offset_y;
        const int right = left + static_cast<int>(tile_width);
        const int bottom = top + static_cast<int>(tile_height);
        if (first) {
          minimum_x = left;
          minimum_y = top;
          maximum_x = right;
          maximum_y = bottom;
          first = false;
        } else {
          minimum_x = (std::min)(minimum_x, left);
          minimum_y = (std::min)(minimum_y, top);
          maximum_x = (std::max)(maximum_x, right);
          maximum_y = (std::max)(maximum_y, bottom);
        }
      }
      const int half_width = (std::max)(-minimum_x, maximum_x);
      const int half_height = (std::max)(-minimum_y, maximum_y);
      const int canvas_width = 2 * half_width;
      const int canvas_height = 2 * half_height;
      if (canvas_width <= 0 || canvas_height <= 0 || canvas_width > 1024 ||
          canvas_height > 1024) {
        continue;
      }
      auto art = std::make_shared<ObjectArtFrame>();
      art->width = static_cast<std::uint16_t>(canvas_width);
      art->height = static_cast<std::uint16_t>(canvas_height);
      art->canvas_width = art->width;
      art->canvas_height = art->height;
      art->bgra.resize(static_cast<std::size_t>(canvas_width) * canvas_height);
      art->opacity.resize(art->bgra.size());
      for (const DoodadTemplateCell& cell : brush.cells) {
        const TilePixels* const tile = tile_pixels(cell.map_tile);
        if (tile == nullptr) {
          continue;
        }
        const int destination_x = half_width + cell.pixel_offset_x;
        const int destination_y = half_height + cell.pixel_offset_y;
        for (std::size_t y = 0U; y < tile_height; ++y) {
          for (std::size_t x = 0U; x < tile_width; ++x) {
            const std::size_t destination =
                static_cast<std::size_t>(destination_y + static_cast<int>(y)) *
                    canvas_width +
                static_cast<std::size_t>(destination_x +
                                         static_cast<int>(x));
            art->bgra[destination] = (*tile)[y * tile_width + x];
            art->opacity[destination] = 0xFFU;
          }
        }
      }
      doodad_art_.emplace(brush.type, std::move(art));
    }
    return true;
  } catch (...) {
    doodad_art_.clear();
    return false;
  }
}

bool EditorDocument::object_at(const EditorLayer layer,
                               const std::uint16_t x,
                               const std::uint16_t y,
                               std::uint16_t& type) const noexcept {
  std::uint32_t best = layer == EditorLayer::doodads ? 64U * 64U : 20U * 20U;
  bool found{};
  const auto consider = [&best, &found, &type, x, y](const auto& marker) {
    const std::uint32_t distance =
        squared_distance(x, y, marker.x, marker.y);
    if (distance <= best) {
      best = distance;
      type = marker.type;
      found = true;
    }
  };
  switch (layer) {
    case EditorLayer::doodads:
      for (const DoodadMarker& marker : doodad_markers_) {
        consider(marker);
      }
      break;
    case EditorLayer::units:
      for (const UnitMarker& marker : unit_markers_) {
        consider(marker);
      }
      break;
    case EditorLayer::sprites:
      for (const SpriteMarker& marker : sprite_markers_) {
        consider(marker);
      }
      break;
    case EditorLayer::terrain:
    case EditorLayer::locations:
    case EditorLayer::fog:
      return false;
  }
  return found;
}

bool EditorDocument::place_object(const EditorLayer layer,
                                  const std::uint16_t type,
                                  std::uint16_t x,
                                  std::uint16_t y,
                                  const std::uint8_t owner) noexcept {
  if (!editing_ready_ || tile_edit_active_ ||
      (layer != EditorLayer::doodads && layer != EditorLayer::units &&
       layer != EditorLayer::sprites)) {
    return false;
  }
  try {
    std::vector<formats::PlacementRecord> records{};
    std::vector<undo::TileChange> tile_changes{};
    formats::PlacementRecord placement{};
    if (layer == EditorLayer::doodads) {
      const auto brush = std::find_if(
          doodad_templates_.begin(), doodad_templates_.end(),
          [type](const DoodadTemplate& value) { return value.type == type; });
      if (brush == doodad_templates_.end()) {
        return false;
      }
      const auto align = [](const std::uint16_t coordinate,
                            const std::uint8_t remainder) {
        int result = static_cast<int>(coordinate / 32U) * 32 + remainder;
        if (result - coordinate > 16) {
          result -= 32;
        } else if (coordinate - result > 16) {
          result += 32;
        }
        return static_cast<std::uint16_t>((std::max)(0, result));
      };
      x = align(x, brush->x_alignment);
      y = align(y, brush->y_alignment);
      placement = brush->prototype;
      records = doodad_records_;
      for (const DoodadTemplateCell& cell : brush->cells) {
        const int pixel_x = static_cast<int>(x) + cell.pixel_offset_x;
        const int pixel_y = static_cast<int>(y) + cell.pixel_offset_y;
        if (pixel_x < 0 || pixel_y < 0 ||
            pixel_x % static_cast<int>(tile_width) != 0 ||
            pixel_y % static_cast<int>(tile_height) != 0) {
          return false;
        }
        const int tile_x = pixel_x / static_cast<int>(tile_width);
        const int tile_y = pixel_y / static_cast<int>(tile_height);
        if (tile_x >= width_ || tile_y >= height_) {
          return false;
        }
        const std::size_t index =
            static_cast<std::size_t>(tile_y) * width_ + tile_x;
        if (tiles_[index] != cell.map_tile) {
          tile_changes.push_back({index, tiles_[index], cell.map_tile});
        }
      }
    } else if (layer == EditorLayer::units) {
      const auto prototype = std::find_if(
          unit_prototypes_.begin(), unit_prototypes_.end(),
          [type](const formats::PlacementRecord& value) {
            return value.type == type;
          });
      if (prototype == unit_prototypes_.end()) {
        return false;
      }
      placement = *prototype;
      records = unit_records_;
    } else {
      const auto prototype = std::find_if(
          sprite_prototypes_.begin(), sprite_prototypes_.end(),
          [type](const formats::PlacementRecord& value) {
            return value.type == type;
          });
      if (prototype == sprite_prototypes_.end()) {
        return false;
      }
      placement = *prototype;
      records = sprite_records_;
    }
    if (x >= static_cast<std::uint32_t>(width_) * tile_width ||
        y >= static_cast<std::uint32_t>(height_) * tile_height) {
      return false;
    }
    placement.type = type;
    placement.x = x;
    placement.y = y;
    placement.owner = owner;
    placement.enabled = true;
    records.push_back(std::move(placement));
    return commit_object_edit(layer, std::move(records),
                              std::move(tile_changes));
  } catch (...) {
    return false;
  }
}

bool EditorDocument::erase_object_at(const EditorLayer layer,
                                     const std::uint16_t x,
                                     const std::uint16_t y) noexcept {
  if (!editing_ready_ || tile_edit_active_ ||
      (layer != EditorLayer::doodads && layer != EditorLayer::units &&
       layer != EditorLayer::sprites)) {
    return false;
  }
  try {
    std::vector<formats::PlacementRecord> records =
        layer == EditorLayer::doodads
            ? doodad_records_
            : (layer == EditorLayer::units ? unit_records_ : sprite_records_);
    if (records.empty()) {
      return false;
    }
    const std::uint32_t maximum =
        layer == EditorLayer::doodads ? 64U * 64U : 20U * 20U;
    std::uint32_t best = maximum;
    std::size_t selected = records.size();
    for (std::size_t index = 0U; index < records.size(); ++index) {
      const std::uint32_t distance = squared_distance(
          x, y, records[index].x, records[index].y);
      if (distance <= best) {
        best = distance;
        selected = index;
      }
    }
    if (selected == records.size()) {
      return false;
    }
    std::vector<undo::TileChange> tile_changes{};
    if (layer == EditorLayer::doodads) {
      const formats::PlacementRecord& placement = records[selected];
      const auto brush = std::find_if(
          doodad_templates_.begin(), doodad_templates_.end(),
          [&placement](const DoodadTemplate& value) {
            return value.type == placement.type;
          });
      if (brush != doodad_templates_.end()) {
        for (const DoodadTemplateCell& cell : brush->cells) {
          const int pixel_x = static_cast<int>(placement.x) +
                              cell.pixel_offset_x;
          const int pixel_y = static_cast<int>(placement.y) +
                              cell.pixel_offset_y;
          if (pixel_x < 0 || pixel_y < 0 ||
              pixel_x % static_cast<int>(tile_width) != 0 ||
              pixel_y % static_cast<int>(tile_height) != 0) {
            continue;
          }
          const int tile_x = pixel_x / static_cast<int>(tile_width);
          const int tile_y = pixel_y / static_cast<int>(tile_height);
          if (tile_x >= width_ || tile_y >= height_) {
            continue;
          }
          const std::size_t index =
              static_cast<std::size_t>(tile_y) * width_ + tile_x;
          std::uint16_t recomposed = editor_tiles_[index];
          for (std::size_t remaining = 0U; remaining < records.size();
               ++remaining) {
            if (remaining == selected) {
              continue;
            }
            const formats::PlacementRecord& other = records[remaining];
            const auto other_brush = std::find_if(
                doodad_templates_.begin(), doodad_templates_.end(),
                [&other](const DoodadTemplate& value) {
                  return value.type == other.type;
                });
            if (other_brush == doodad_templates_.end()) {
              continue;
            }
            for (const DoodadTemplateCell& other_cell : other_brush->cells) {
              const int other_x = static_cast<int>(other.x) +
                                  other_cell.pixel_offset_x;
              const int other_y = static_cast<int>(other.y) +
                                  other_cell.pixel_offset_y;
              if (other_x >= 0 && other_y >= 0 &&
                  other_x % static_cast<int>(tile_width) == 0 &&
                  other_y % static_cast<int>(tile_height) == 0 &&
                  static_cast<std::size_t>(other_y /
                                           static_cast<int>(tile_height)) *
                              width_ +
                          static_cast<std::size_t>(other_x /
                                                   static_cast<int>(tile_width)) ==
                      index) {
                recomposed = other_cell.map_tile;
              }
            }
          }
          if (tiles_[index] != recomposed) {
            tile_changes.push_back({index, tiles_[index], recomposed});
          }
        }
      }
    }
    records.erase(records.begin() + static_cast<std::ptrdiff_t>(selected));
    return commit_object_edit(layer, std::move(records),
                              std::move(tile_changes));
  } catch (...) {
    return false;
  }
}

bool EditorDocument::commit_object_edit(
    const EditorLayer layer,
    std::vector<formats::PlacementRecord> records,
    std::vector<undo::TileChange> tile_changes) noexcept {
  const formats::ChkDialectInfo& dialect = chk_.dialect();
  const formats::PlacementRecordLayout layout =
      layer == EditorLayer::doodads
          ? dialect.doodads
          : (layer == EditorLayer::units ? unit_placement_layout(dialect)
                                         : dialect.sprites);
  if (layout.record_bytes == 0U) {
    return false;
  }
  try {
    std::vector<std::uint8_t> after{};
    if (!formats::serialize_placement_records(records, layout, after)) {
      return false;
    }
    const formats::ChkSection* const existing =
        chk_.section(layout.section_tag, 0U);
    undo::TileEditCommand command{};
    command.changes = std::move(tile_changes);
    command.section_changes.push_back(
        {layout.section_tag, 0U, existing != nullptr, true,
         existing == nullptr ? std::vector<std::uint8_t>{} : existing->payload,
         after});
    formats::ChkDocument previous_chk = chk_;
    const std::vector<std::uint16_t> previous_tiles = tiles_;
    formats::ChkDocument candidate = chk_;
    const bool section_written = existing != nullptr
                                     ? candidate.replace_section(
                                           layout.section_tag, 0U, after)
                                     : candidate.append_section(
                                           layout.section_tag, after);
    if (!section_written) {
      return false;
    }
    for (const undo::TileChange& change : command.changes) {
      if (change.tile_index >= tiles_.size()) {
        return false;
      }
      tiles_[change.tile_index] = change.after;
    }
    chk_ = std::move(candidate);
    if (!update_chk_terrain_layers() ||
        !undo_stack_.push(std::move(command))) {
      tiles_ = previous_tiles;
      chk_ = std::move(previous_chk);
      return false;
    }
    std::wstring parse_error{};
    return parse_object_sections(parse_error);
  } catch (...) {
    return false;
  }
}

bool EditorDocument::commit_section_edit(
    const std::uint32_t tag,
    std::vector<std::uint8_t> after) noexcept {
  try {
    std::vector<std::pair<std::uint32_t, std::vector<std::uint8_t>>>
        replacements{};
    replacements.emplace_back(tag, std::move(after));
    if (!commit_section_edits(std::move(replacements))) {
      return false;
    }
    std::wstring parse_error{};
    return parse_auxiliary_sections(parse_error);
  } catch (...) {
    return false;
  }
}

bool EditorDocument::commit_section_edits(
    std::vector<std::pair<std::uint32_t, std::vector<std::uint8_t>>>
        replacements) noexcept {
  if (!editing_ready_ || replacements.empty()) {
    return false;
  }
  try {
    formats::ChkDocument candidate = chk_;
    undo::TileEditCommand command{};
    for (std::size_t index = 0U; index < replacements.size(); ++index) {
      const std::uint32_t tag = replacements[index].first;
      if (std::any_of(replacements.begin(), replacements.begin() + index,
                      [tag](const auto& replacement) {
                        return replacement.first == tag;
                      })) {
        return false;
      }
      const std::vector<std::uint8_t>& after = replacements[index].second;
      const formats::ChkSection* const existing = chk_.section(tag, 0U);
      if (existing != nullptr && existing->payload == after) {
        continue;
      }
      const bool written =
          existing != nullptr
              ? candidate.replace_section(tag, 0U, after)
              : candidate.append_section(tag, after);
      if (!written) {
        return false;
      }
      command.section_changes.push_back(
          {tag, 0U, existing != nullptr, true,
           existing == nullptr ? std::vector<std::uint8_t>{}
                               : existing->payload,
           after});
    }
    if (command.section_changes.empty()) {
      return true;
    }
    formats::ChkDocument previous = chk_;
    chk_ = std::move(candidate);
    if (!undo_stack_.push(std::move(command))) {
      chk_ = std::move(previous);
      return false;
    }
    section_count_ = chk_.section_count();
    return true;
  } catch (...) {
    return false;
  }
}

bool EditorDocument::build_terrain_brush_inventory(
    std::wstring& error) noexcept {
  terrain_brushes_.clear();
  try {
    if (isom_catalog_.valid()) {
      terrain_brushes_.reserve(isom_catalog_.brushes().size());
      for (const terrain::IsomBrush& brush : isom_catalog_.brushes()) {
        if (!valid_terrain_tile(brush.preview_tile)) {
          error = L"A logical terrain brush references an invalid CV5 group.";
          terrain_brushes_.clear();
          return false;
        }
        terrain_brushes_.push_back(brush.preview_tile);
      }
      return !terrain_brushes_.empty();
    }
    terrain_brushes_.reserve(
        (std::min)(tiles_.size(), static_cast<std::size_t>(1024U)));
    std::array<bool, 1024> seen_groups{};
    for (const std::uint16_t tile_id : tiles_) {
      std::uint16_t group_id{};
      if (terrain_group_id(tile_id, group_id) && !seen_groups[group_id]) {
        seen_groups[group_id] = true;
        terrain_brushes_.push_back(tile_id);
      }
    }
    std::sort(terrain_brushes_.begin(), terrain_brushes_.end(),
              [](const std::uint16_t left, const std::uint16_t right) {
                return (left >> 4U) < (right >> 4U);
              });
    if (terrain_brushes_.empty()) {
      error = L"The map does not reference any valid terrain groups.";
      return false;
    }
    return true;
  } catch (...) {
    terrain_brushes_.clear();
    error = L"There was not enough memory to inventory terrain brushes.";
    return false;
  }
}

const wchar_t* scenario_format_name(const ScenarioFormat format) noexcept {
  return formats::chk_dialect_name(format);
}

}  // namespace staredit
