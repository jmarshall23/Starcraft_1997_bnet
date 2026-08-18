#include "application.hpp"

#include "brush_palette.hpp"
#include "document.hpp"
#include "main_frame.hpp"
#include "map_view.hpp"
#include "minimap.hpp"
#include "resource.h"

#include "starcraft/game/scenario.hpp"
#include "starcraft/runtime/storm.hpp"

#include <algorithm>
#include <array>
#include <commctrl.h>
#include <cstdint>
#include <filesystem>
#include <shellapi.h>
#include <string>
#include <string_view>
#include <vector>

namespace staredit {
namespace {

std::filesystem::path executable_directory() noexcept {
  std::array<wchar_t, 32768> path{};
  const DWORD length =
      GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
  if (length == 0U || length >= path.size()) {
    return {};
  }
  try {
    return std::filesystem::path{path.data(), path.data() + length}.parent_path();
  } catch (...) {
    return {};
  }
}

std::vector<std::wstring> command_line_arguments() {
  int count{};
  LPWSTR* const values = CommandLineToArgvW(GetCommandLineW(), &count);
  if (values == nullptr) {
    return {};
  }
  std::vector<std::wstring> arguments{};
  try {
    arguments.reserve(static_cast<std::size_t>(count));
    for (int index = 0; index < count; ++index) {
      arguments.emplace_back(values[index]);
    }
  } catch (...) {
    arguments.clear();
  }
  LocalFree(values);
  return arguments;
}

int run_probe(const std::filesystem::path& map,
              const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error)) {
    return 2;
  }
  if (document.width() == 0U || document.height() == 0U ||
      document.section_count() == 0U) {
    return 3;
  }
  std::uint16_t first_tile{};
  if (!document.tile_at(0U, 0U, first_tile) ||
      document.tile_pixels(first_tile) == nullptr) {
    return 3;
  }
  return 0;
}

int run_round_trip_probe(const std::filesystem::path& map,
                         const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error)) {
    return 2;
  }
  return document.chk_round_trip_matches_source() ? 0 : 4;
}

int run_isom_probe(const std::filesystem::path& map,
                   const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error)) {
    return 2;
  }
  const std::size_t expected =
      (static_cast<std::size_t>(document.width()) / 2U + 1U) *
      (static_cast<std::size_t>(document.height()) + 1U);
  if (!document.has_isom() || document.isom_entry_count() != expected) {
    return 3;
  }
  return document.chk_round_trip_matches_source() ? 0 : 4;
}

int run_isom_topology_probe(const std::filesystem::path& map,
                            const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error)) {
    return 2;
  }
  return document.isom_topology_ready() &&
                 document.isom_group_pair_count() != 0U &&
                 document.isom_terrain_type_count() != 0U
             ? 0
             : 3;
}

int run_isom_edit_probe(const std::filesystem::path& map,
                        const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error) ||
      !document.logical_terrain_ready() ||
      document.terrain_brushes().size() < 2U) {
    return 2;
  }
  const std::uint16_t x = document.width() / 2U;
  const std::uint16_t y = document.height() / 2U;
  std::uint16_t original_tile{};
  if (!document.tile_at(x, y, original_tile)) {
    return 3;
  }
  const auto replacement = std::find_if(
      document.terrain_brushes().begin(), document.terrain_brushes().end(),
      [&document, original_tile](const std::uint16_t brush) {
        return !document.same_terrain_group(brush, original_tile);
      });
  if (replacement == document.terrain_brushes().end()) {
    return 3;
  }
  const std::uint64_t before_isom = document.isom_checksum();
  const std::uint64_t before_editor = document.editor_tile_checksum();
  const std::uint64_t before_game = document.game_tile_checksum();
  if (!document.begin_tile_edit() ||
      !document.paint_terrain(x, y, *replacement, 3U) ||
      !document.commit_tile_edit()) {
    document.cancel_tile_edit();
    return 4;
  }
  const std::uint64_t after_isom = document.isom_checksum();
  const std::uint64_t after_editor = document.editor_tile_checksum();
  const std::uint64_t after_game = document.game_tile_checksum();
  if (after_isom == before_isom || after_editor == before_editor ||
      after_game == before_game || !document.modified() ||
      !document.can_undo()) {
    return 5;
  }
  if (!document.undo() || document.isom_checksum() != before_isom ||
      document.editor_tile_checksum() != before_editor ||
      document.game_tile_checksum() != before_game || document.modified()) {
    return 6;
  }
  if (!document.redo() || document.isom_checksum() != after_isom ||
      document.editor_tile_checksum() != after_editor ||
      document.game_tile_checksum() != after_game) {
    return 7;
  }

  std::array<wchar_t, MAX_PATH> temporary_directory{};
  std::array<wchar_t, MAX_PATH> seed_path{};
  const DWORD directory_length = GetTempPathW(
      static_cast<DWORD>(temporary_directory.size()), temporary_directory.data());
  if (directory_length == 0U ||
      directory_length >= temporary_directory.size() ||
      GetTempFileNameW(temporary_directory.data(), L"sei", 0U,
                       seed_path.data()) == 0U) {
    return 8;
  }
  const std::filesystem::path seed{seed_path.data()};
  std::filesystem::path exported = seed;
  exported += L".chk";
  (void)DeleteFileW(seed.c_str());
  int result = 9;
  if (document.export_raw_chk(exported, error)) {
    EditorDocument reopened{};
    if (reopened.load(exported, data_root, error) &&
        reopened.isom_checksum() == after_isom &&
        reopened.editor_tile_checksum() == after_editor &&
        reopened.game_tile_checksum() == after_game &&
        reopened.chk_round_trip_matches_source()) {
      result = 0;
    }
  }
  (void)DeleteFileW(exported.c_str());
  return result;
}

bool find_replacement_tile(const EditorDocument& document,
                           const std::uint16_t original,
                           std::uint16_t& replacement) noexcept {
  for (std::uint16_t y = 0U; y < document.height(); ++y) {
    for (std::uint16_t x = 0U; x < document.width(); ++x) {
      std::uint16_t candidate{};
      if (document.tile_at(x, y, candidate) && candidate != original &&
          document.valid_terrain_tile(candidate) &&
          !document.same_terrain_group(candidate, original)) {
        replacement = candidate;
        return true;
      }
    }
  }
  return false;
}

int run_terrain_edit_probe(const std::filesystem::path& map,
                           const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  std::uint16_t original{};
  std::uint16_t replacement{};
  if (!document.load(map, data_root, error) || !document.editing_ready() ||
      !document.tile_at(0U, 0U, original) ||
      !find_replacement_tile(document, original, replacement)) {
    return 2;
  }
  if (!document.begin_tile_edit() ||
      !document.paint_tile(0U, 0U, replacement) ||
      !document.commit_tile_edit()) {
    return 3;
  }
  std::uint16_t current{};
  if (!document.tile_at(0U, 0U, current) || current != replacement ||
      !document.modified() || !document.can_undo() ||
      document.chk_round_trip_matches_source()) {
    return 4;
  }
  if (!document.undo() || !document.tile_at(0U, 0U, current) ||
      current != original || document.modified() ||
      !document.chk_round_trip_matches_source() || !document.can_redo()) {
    return 5;
  }
  if (!document.redo() || !document.tile_at(0U, 0U, current) ||
      current != replacement || !document.modified() || !document.undo()) {
    return 6;
  }
  return document.chk_round_trip_matches_source() ? 0 : 7;
}

int run_raw_export_probe(const std::filesystem::path& map,
                         const std::filesystem::path& data_root) noexcept {
  EditorDocument source{};
  std::wstring error{};
  std::uint16_t original{};
  std::uint16_t replacement{};
  if (!source.load(map, data_root, error) ||
      !source.tile_at(0U, 0U, original) ||
      !find_replacement_tile(source, original, replacement) ||
      !source.begin_tile_edit() || !source.paint_tile(0U, 0U, replacement) ||
      !source.commit_tile_edit()) {
    return 2;
  }

  std::array<wchar_t, MAX_PATH> temporary_directory{};
  std::array<wchar_t, MAX_PATH> seed_path{};
  const DWORD directory_length = GetTempPathW(
      static_cast<DWORD>(temporary_directory.size()), temporary_directory.data());
  if (directory_length == 0U || directory_length >= temporary_directory.size() ||
      GetTempFileNameW(temporary_directory.data(), L"sed", 0U,
                       seed_path.data()) == 0U) {
    return 3;
  }
  const std::filesystem::path seed{seed_path.data()};
  std::filesystem::path exported = seed;
  exported += L".chk";
  (void)DeleteFileW(seed.c_str());
  int result = 4;
  if (source.export_raw_chk(exported, error)) {
    EditorDocument reopened{};
    std::uint16_t current{};
    if (reopened.load(exported, data_root, error) &&
        reopened.tile_at(0U, 0U, current) && current == replacement &&
        reopened.chk_round_trip_matches_source()) {
      result = 0;
    }
  }
  (void)DeleteFileW(exported.c_str());
  return result;
}

int run_ui_edit_probe(MainFrame& frame,
                      const std::filesystem::path& map) noexcept {
  if (!frame.open_document(map)) {
    return 2;
  }
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(frame.mdi_client(), WM_MDIGETACTIVE, 0, 0));
  EditorDocument* const document = active_editor_document(frame.mdi_client());
  std::uint16_t original{};
  const std::uint16_t target_x = document == nullptr ? 0U : document->width() / 2U;
  const std::uint16_t target_y = document == nullptr ? 0U : document->height() / 2U;
  if (child == nullptr || document == nullptr ||
      !document->tile_at(target_x, target_y, original)) {
    return 3;
  }
  const auto replacement = std::find_if(
      document->terrain_brushes().begin(), document->terrain_brushes().end(),
      [document, original](const std::uint16_t brush) {
        return !document->same_terrain_group(brush, original);
      });
  if (replacement == document->terrain_brushes().end()) {
    return 4;
  }
  const std::uint64_t before = document->game_tile_checksum();
  const LPARAM target_position = MAKELPARAM(
      static_cast<WORD>(target_x * EditorDocument::tile_width + 16U),
      static_cast<WORD>(target_y * EditorDocument::tile_height + 16U));
  set_active_brush(frame.mdi_client(), *replacement);
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, target_position);
  SendMessageW(child, WM_LBUTTONUP, 0, target_position);
  if (document->game_tile_checksum() == before ||
      !document->modified() || !document->can_undo()) {
    return 5;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  std::uint16_t current{};
  if (!document->tile_at(target_x, target_y, current) ||
      current != original || document->game_tile_checksum() != before ||
      document->modified()) {
    return 6;
  }
  return 0;
}

int run_palette_probe(MainFrame& frame,
                      const std::filesystem::path& map) noexcept {
  if (!frame.open_document(map)) {
    return 2;
  }
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(frame.mdi_client(), WM_MDIGETACTIVE, 0, 0));
  const HWND palette = frame.brush_palette();
  EditorDocument* const document = active_editor_document(frame.mdi_client());
  std::array<wchar_t, 64> class_name{};
  std::uint16_t original{};
  if (child == nullptr || palette == nullptr || document == nullptr ||
      GetClassNameW(palette, class_name.data(),
                    static_cast<int>(class_name.size())) == 0 ||
      std::wstring_view{class_name.data()} != L"StarEditBrushPalette" ||
      !document->tile_at(0U, 0U, original)) {
    return 3;
  }

  const auto& brushes = document->terrain_brushes();
  const auto replacement_at =
      std::find_if(brushes.begin(), brushes.end(),
                   [document, original](const std::uint16_t tile) {
                     return !document->same_terrain_group(tile, original);
                   });
  if (replacement_at == brushes.end()) {
    return 4;
  }
  const std::uint16_t replacement = *replacement_at;

  const LPARAM target_position = MAKELPARAM(16U, 16U);
  SendMessageW(child, WM_RBUTTONDOWN, MK_RBUTTON, target_position);
  std::uint16_t selected{};
  if (get_brush_palette_selection(palette, selected)) {
    return 5;
  }

  RECT palette_client{};
  GetClientRect(palette, &palette_client);
  constexpr int cell_width = 54;
  constexpr int cell_height = 58;
  constexpr int header_height = 30;
  const int columns = (std::max)(1L, palette_client.right / cell_width);
  const std::size_t index =
      static_cast<std::size_t>(replacement_at - brushes.begin());
  const int column = static_cast<int>(index % columns);
  const int row = static_cast<int>(index / columns);
  const LPARAM palette_position = MAKELPARAM(
      static_cast<WORD>(column * cell_width + cell_width / 2),
      static_cast<WORD>(header_height + row * cell_height + cell_height / 2));
  SendMessageW(palette, WM_LBUTTONDOWN, MK_LBUTTON, palette_position);
  if (!get_brush_palette_selection(palette, selected) ||
      selected != replacement) {
    return 6;
  }

  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, target_position);
  SendMessageW(child, WM_LBUTTONUP, 0, target_position);
  std::uint16_t current{};
  if (!document->tile_at(0U, 0U, current) ||
      !document->same_terrain_group(current, replacement) ||
      !document->modified() || !document->can_undo()) {
    return 7;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  if (!document->tile_at(0U, 0U, current) || current != original ||
      document->modified()) {
    return 8;
  }
  if (!frame.open_document(map)) {
    return 9;
  }
  const HWND second_child = reinterpret_cast<HWND>(
      SendMessageW(frame.mdi_client(), WM_MDIGETACTIVE, 0, 0));
  if (second_child == nullptr || second_child == child ||
      get_brush_palette_selection(palette, selected)) {
    return 10;
  }
  SendMessageW(frame.mdi_client(), WM_MDIACTIVATE,
               reinterpret_cast<WPARAM>(child), 0);
  if (!get_brush_palette_selection(palette, selected) ||
      selected != replacement) {
    return 11;
  }
  return 0;
}

int run_brush_footprint_probe(MainFrame& frame,
                              const std::filesystem::path& map) noexcept {
  if (!frame.open_document(map)) {
    return 2;
  }
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(frame.mdi_client(), WM_MDIGETACTIVE, 0, 0));
  EditorDocument* const document = active_editor_document(frame.mdi_client());
  if (child == nullptr || document == nullptr || document->width() < 5U ||
      document->height() < 5U) {
    return 3;
  }

  if (!document->begin_tile_edit() ||
      document->paint_tile(2U, 2U, 0xFFFFU)) {
    document->cancel_tile_edit();
    return 4;
  }
  document->cancel_tile_edit();

  const std::uint16_t target_x = document->width() / 2U;
  const std::uint16_t target_y = document->height() / 2U;
  std::uint16_t center{};
  if (!document->tile_at(target_x, target_y, center)) {
    return 5;
  }
  const auto replacement = std::find_if(
      document->terrain_brushes().begin(), document->terrain_brushes().end(),
      [document, center](const std::uint16_t brush) {
        return !document->same_terrain_group(brush, center);
      });
  if (replacement == document->terrain_brushes().end()) {
    return 5;
  }
  std::array<std::uint16_t, 9> original{};
  std::size_t original_index{};
  for (std::uint16_t y = target_y - 1U; y <= target_y + 1U; ++y) {
    for (std::uint16_t x = target_x - 1U; x <= target_x + 1U; ++x) {
      if (!document->tile_at(x, y, original[original_index++])) {
        return 6;
      }
    }
  }
  std::uint16_t outside{};
  if (!document->tile_at(0U, 0U, outside)) {
    return 6;
  }

  const std::uint64_t before = document->game_tile_checksum();
  set_active_brush(frame.mdi_client(), *replacement);
  SendMessageW(frame.window(), WM_COMMAND, ID_BRUSH_SIZE_3, 0);
  const LPARAM target = MAKELPARAM(
      static_cast<WORD>(target_x * EditorDocument::tile_width + 16U),
      static_cast<WORD>(target_y * EditorDocument::tile_height + 16U));
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, target);
  SendMessageW(child, WM_LBUTTONUP, 0, target);

  std::uint16_t current{};
  std::size_t changed{};
  original_index = 0U;
  for (std::uint16_t y = target_y - 1U; y <= target_y + 1U; ++y) {
    for (std::uint16_t x = target_x - 1U; x <= target_x + 1U; ++x) {
      if (!document->tile_at(x, y, current)) {
        return 7;
      }
      changed += current != original[original_index++] ? 1U : 0U;
    }
  }
  if (changed == 0U || document->game_tile_checksum() == before ||
      !document->tile_at(0U, 0U, current) || current != outside ||
      !document->modified() || !document->can_undo()) {
    return 8;
  }

  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  original_index = 0U;
  for (std::uint16_t y = target_y - 1U; y <= target_y + 1U; ++y) {
    for (std::uint16_t x = target_x - 1U; x <= target_x + 1U; ++x) {
      if (!document->tile_at(x, y, current) ||
          current != original[original_index++]) {
        return 9;
      }
    }
  }
  return document->modified() || document->game_tile_checksum() != before
             ? 10
             : 0;
}

int run_layout_probe(MainFrame& frame,
                     const std::filesystem::path& map) noexcept {
  if (!frame.open_document(map)) {
    return 2;
  }
  BOOL maximized{};
  const HWND child = reinterpret_cast<HWND>(SendMessageW(
      frame.mdi_client(), WM_MDIGETACTIVE, 0,
      reinterpret_cast<LPARAM>(&maximized)));
  const HWND minimap = frame.minimap();
  const HWND palette = frame.brush_palette();
  if (child == nullptr || minimap == nullptr || palette == nullptr ||
      maximized == FALSE) {
    return 3;
  }
  std::array<wchar_t, 64> class_name{};
  if (GetClassNameW(minimap, class_name.data(),
                    static_cast<int>(class_name.size())) == 0 ||
      std::wstring_view{class_name.data()} != L"StarEditMiniMap") {
    return 4;
  }
  RECT mini{};
  RECT brushes{};
  RECT mdi{};
  if (GetWindowRect(minimap, &mini) == FALSE ||
      GetWindowRect(palette, &brushes) == FALSE ||
      GetWindowRect(frame.mdi_client(), &mdi) == FALSE) {
    return 5;
  }
  if (mini.left != brushes.left || mini.top >= brushes.top ||
      mini.bottom > brushes.top || mini.right > mdi.left) {
    return 6;
  }
  return 0;
}

int run_new_map_probe(MainFrame& frame) noexcept {
  if (!frame.new_default_document()) {
    return 2;
  }
  EditorDocument* const document = active_editor_document(frame.mdi_client());
  BOOL maximized{};
  const HWND child = reinterpret_cast<HWND>(SendMessageW(
      frame.mdi_client(), WM_MDIGETACTIVE, 0,
      reinterpret_cast<LPARAM>(&maximized)));
  if (document == nullptr || child == nullptr || maximized == FALSE ||
      document->title() != L"Untitled" || !document->path().empty() ||
      document->width() != 96U || document->height() != 96U ||
      document->tileset_id() != 4U || document->tileset_name() != "Jungle" ||
      document->unit_count() != 0U || document->doodad_count() != 0U ||
      document->sprite_count() != 0U || !document->editing_ready() ||
      !document->logical_terrain_ready() || document->modified()) {
    return 3;
  }
  if (document->object_brushes(EditorLayer::units).size() != 228U ||
      document->object_brushes(EditorLayer::sprites).size() != 267U ||
      document->object_brushes(EditorLayer::doodads).empty()) {
    return 4;
  }
  std::uint16_t dirt{};
  if (!document->terrain_brush_at(0U, 0U, dirt) ||
      document->terrain_brush_name(dirt) != "Dirt") {
    return 5;
  }
  for (std::uint16_t y = 0U; y < document->height(); ++y) {
    for (std::uint16_t x = 0U; x < document->width(); ++x) {
      std::uint16_t brush{};
      if (!document->terrain_brush_at(x, y, brush) ||
          document->terrain_brush_name(brush) != "Dirt") {
        return 6;
      }
    }
  }
  const std::uint16_t center_x = static_cast<std::uint16_t>(
      document->width() * EditorDocument::tile_width / 2U);
  const std::uint16_t center_y = static_cast<std::uint16_t>(
      document->height() * EditorDocument::tile_height / 2U);
  if (!document->place_object(EditorLayer::units, 106U, center_x, center_y) ||
      document->unit_count() != 1U || !document->undo() ||
      document->unit_count() != 0U || document->modified()) {
    return 7;
  }
  const std::uint16_t doodad_type =
      document->object_brushes(EditorLayer::doodads).front();
  if (!document->place_object(EditorLayer::doodads, doodad_type, center_x,
                              center_y) ||
      document->doodad_count() != 1U || !document->undo() ||
      document->doodad_count() != 0U || document->modified()) {
    return 8;
  }

  center_active_view(frame.mdi_client(), center_x, center_y);
  MapViewport before{};
  RECT client{};
  if (!query_active_viewport(frame.mdi_client(), before) ||
      before.zoom_percent != 100 || GetClientRect(child, &client) == FALSE) {
    return 9;
  }
  POINT pointer{client.right / 2, client.bottom / 2};
  const int anchor_x =
      before.world_x + pointer.x * 100 / before.zoom_percent;
  const int anchor_y =
      before.world_y + pointer.y * 100 / before.zoom_percent;
  POINT screen = pointer;
  if (ClientToScreen(child, &screen) == FALSE) {
    return 10;
  }
  SendMessageW(child, WM_MOUSEWHEEL,
               MAKEWPARAM(0, static_cast<WORD>(WHEEL_DELTA)),
               MAKELPARAM(static_cast<WORD>(screen.x),
                          static_cast<WORD>(screen.y)));
  MapViewport zoomed{};
  if (!query_active_viewport(frame.mdi_client(), zoomed) ||
      zoomed.zoom_percent != 200 ||
      zoomed.client_width >= before.client_width ||
      std::abs(zoomed.world_x + pointer.x * 100 / zoomed.zoom_percent -
               anchor_x) > 1 ||
      std::abs(zoomed.world_y + pointer.y * 100 / zoomed.zoom_percent -
               anchor_y) > 1) {
    return 11;
  }
  SendMessageW(child, WM_MOUSEWHEEL,
               MAKEWPARAM(0, static_cast<WORD>(-WHEEL_DELTA)),
               MAKELPARAM(static_cast<WORD>(screen.x),
                          static_cast<WORD>(screen.y)));
  MapViewport restored{};
  if (!query_active_viewport(frame.mdi_client(), restored) ||
      restored.zoom_percent != 100) {
    return 12;
  }

  SendMessageW(frame.window(), WM_COMMAND, ID_LAYER_LOCATIONS, 0);
  const int location_left = client.right / 2 - 64;
  const int location_top = client.bottom / 2 - 48;
  const int location_right = client.right / 2 + 64;
  const int location_bottom = client.bottom / 2 + 48;
  const LPARAM location_start =
      MAKELPARAM(static_cast<WORD>(location_left),
                 static_cast<WORD>(location_top));
  const LPARAM location_end =
      MAKELPARAM(static_cast<WORD>(location_right),
                 static_cast<WORD>(location_bottom));
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, location_start);
  SendMessageW(child, WM_MOUSEMOVE, MK_LBUTTON, location_end);
  SendMessageW(child, WM_LBUTTONUP, 0, location_end);
  if (document->location_markers().size() != 1U || !document->undo() ||
      !document->location_markers().empty()) {
    return 13;
  }

  SendMessageW(frame.window(), WM_COMMAND, ID_LAYER_FOG, 0);
  const int fog_client_x = client.right / 2;
  const int fog_client_y = client.bottom / 2;
  const std::uint16_t fog_x = static_cast<std::uint16_t>(
      (restored.world_x + fog_client_x) / EditorDocument::tile_width);
  const std::uint16_t fog_y = static_cast<std::uint16_t>(
      (restored.world_y + fog_client_y) / EditorDocument::tile_height);
  const LPARAM fog_position =
      MAKELPARAM(static_cast<WORD>(fog_client_x),
                 static_cast<WORD>(fog_client_y));
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, fog_position);
  SendMessageW(child, WM_LBUTTONUP, 0, fog_position);
  if (!document->fog_at(fog_x, fog_y)) {
    return 14;
  }
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, fog_position);
  SendMessageW(child, WM_LBUTTONUP, 0, fog_position);
  if (document->fog_at(fog_x, fog_y) || !document->undo() ||
      !document->fog_at(fog_x, fog_y) || !document->undo() ||
      document->fog_at(fog_x, fog_y) || document->modified()) {
    return 15;
  }
  return 0;
}

int run_new_map_variants_probe(
    const std::filesystem::path& data_root) noexcept {
  constexpr std::array<NewMapSettings, 5> settings{{
      {64U, 96U, 0U, 0U},
      {96U, 64U, 1U, 3U},
      {128U, 192U, 2U, 1U},
      {192U, 128U, 3U, 1U},
      {256U, 256U, 4U, 3U},
  }};
  for (const NewMapTileset& tileset : new_map_tilesets()) {
    for (std::size_t terrain = 0U; terrain < tileset.terrain.size(); ++terrain) {
      if (!tileset.terrain[terrain].empty() &&
          !valid_new_map_settings(
              {64U, 64U, tileset.id, static_cast<std::uint16_t>(terrain)})) {
        return 2;
      }
    }
  }
  for (const NewMapSettings& setting : settings) {
    std::filesystem::path scenario_template{};
    switch (setting.tileset_id) {
      case 1U:
        scenario_template = data_root / L"maps" / L"96x96_space4.scm";
        break;
      case 3U:
        scenario_template = data_root / L"maps" / L"96x96_ash4.scm";
        break;
      default:
        scenario_template =
            data_root / L"maps" / L"96x96_wasteland4.scm";
        break;
    }
    EditorDocument document{};
    std::wstring error{};
    const std::string_view terrain = new_map_terrain_name(setting);
    if (!document.create_blank(scenario_template, data_root, setting.width,
                               setting.height, setting.tileset_id, terrain,
                               error) ||
        document.width() != setting.width ||
        document.height() != setting.height ||
        document.tileset_id() != setting.tileset_id ||
        document.unit_count() != 0U || document.doodad_count() != 0U ||
        document.sprite_count() != 0U || !document.editing_ready() ||
        document.modified()) {
      return 3 + setting.tileset_id;
    }
    const std::array<std::pair<std::uint16_t, std::uint16_t>, 5> points{{
        {std::uint16_t{0}, std::uint16_t{0}},
        {static_cast<std::uint16_t>(setting.width - 1U), std::uint16_t{0}},
        {std::uint16_t{0},
         static_cast<std::uint16_t>(setting.height - 1U)},
        {static_cast<std::uint16_t>(setting.width - 1U),
         static_cast<std::uint16_t>(setting.height - 1U)},
        {static_cast<std::uint16_t>(setting.width / 2U),
         static_cast<std::uint16_t>(setting.height / 2U)},
    }};
    for (const auto point : points) {
      std::uint16_t brush{};
      if (!document.terrain_brush_at(point.first, point.second, brush) ||
          document.terrain_brush_name(brush) != terrain) {
        return 8 + setting.tileset_id;
      }
    }
  }
  return 0;
}

int run_editor_layers_probe(const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.create_blank(data_root / L"maps" / L"96x96_wasteland4.scm",
                             data_root, 96U, 96U, 4U, "Dirt", error) ||
      !document.location_markers().empty()) {
    return 2;
  }
  if (!document.create_location(64U, 96U, 320U, 352U) ||
      document.location_markers().size() != 1U || !document.modified() ||
      !document.undo() || !document.location_markers().empty() ||
      document.modified() || !document.redo() ||
      document.location_markers().size() != 1U || !document.undo()) {
    return 3;
  }
  if (document.fog_at(10U, 10U) || !document.begin_fog_edit() ||
      !document.paint_fog(10U, 10U, 3U, true) ||
      !document.commit_fog_edit() || !document.fog_at(10U, 10U) ||
      !document.undo() || document.fog_at(10U, 10U) ||
      !document.redo() || !document.fog_at(10U, 10U) || !document.undo()) {
    return 4;
  }
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> races{};
  if (!document.player_settings(ownership, races)) {
    return 5;
  }
  const std::uint8_t original_owner = ownership[0];
  ownership[0] = original_owner == 6U ? 5U : 6U;
  races[0] = static_cast<std::uint8_t>((races[0] + 1U) % 3U);
  if (!document.set_player_settings(ownership, races) || !document.modified()) {
    return 6;
  }
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> current_ownership{};
  std::array<std::uint8_t, starcraft::data::chk_player_slot_count> current_races{};
  if (!document.player_settings(current_ownership, current_races) ||
      current_ownership[0] != ownership[0] || current_races[0] != races[0] ||
      !document.undo() || document.modified() ||
      !document.player_settings(current_ownership, current_races) ||
      current_ownership[0] != original_owner) {
    return 7;
  }
  ScenarioProperties original_properties{};
  if (!document.scenario_properties(original_properties)) {
    return 8;
  }
  ScenarioProperties changed_properties{
      "Recovered Scenario", "Scenario metadata round-trip probe."};
  ScenarioProperties current_properties{};
  if (!document.set_scenario_properties(changed_properties) ||
      !document.modified() ||
      !document.scenario_properties(current_properties) ||
      current_properties.name != changed_properties.name ||
      current_properties.description != changed_properties.description ||
      !document.undo() || document.modified() ||
      !document.scenario_properties(current_properties) ||
      current_properties.name != original_properties.name ||
      current_properties.description != original_properties.description) {
    return 9;
  }
  ScenarioForces original_forces{};
  if (!document.scenario_forces(original_forces)) {
    return 10;
  }
  ScenarioForces changed_forces = original_forces;
  changed_forces.player_force[0] =
      static_cast<std::uint8_t>((changed_forces.player_force[0] + 1U) % 4U);
  changed_forces.names[0] = "Alpha Team";
  if (changed_forces.supports_flags) {
    changed_forces.flags[0] ^= formats::force_flag_allies;
  }
  ScenarioForces current_forces{};
  if (!document.set_scenario_forces(changed_forces) || !document.modified() ||
      !document.scenario_forces(current_forces) ||
      current_forces.player_force != changed_forces.player_force ||
      current_forces.names != changed_forces.names ||
      (changed_forces.supports_flags &&
       current_forces.flags != changed_forces.flags) ||
      !document.undo() || document.modified() ||
      !document.scenario_forces(current_forces) ||
      current_forces.player_force != original_forces.player_force ||
      current_forces.names != original_forces.names) {
    return 11;
  }
  return 0;
}

int run_retail_save_probe(const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.create_blank(data_root / L"maps" / L"96x96_wasteland4.scm",
                             data_root, 96U, 96U, 4U, "Dirt", error) ||
      document.format() != ScenarioFormat::retail_chk) {
    return 2;
  }
  const ScenarioProperties properties{"Retail Save Probe",
                                      "Retail STR/SPRP archive validation."};
  constexpr std::uint16_t unit_type = 106U;
  constexpr std::uint16_t unit_x = 512U;
  constexpr std::uint16_t unit_y = 640U;
  if (!document.set_scenario_properties(properties) ||
      !document.place_object(EditorLayer::units, unit_type, unit_x, unit_y)) {
    return 3;
  }

  std::array<wchar_t, MAX_PATH> temporary_directory{};
  std::array<wchar_t, MAX_PATH> seed_path{};
  const DWORD directory_length = GetTempPathW(
      static_cast<DWORD>(temporary_directory.size()), temporary_directory.data());
  if (directory_length == 0U ||
      directory_length >= temporary_directory.size() ||
      GetTempFileNameW(temporary_directory.data(), L"ser", 0U,
                       seed_path.data()) == 0U) {
    return 4;
  }
  const std::filesystem::path seed{seed_path.data()};
  std::filesystem::path archive_path = seed;
  archive_path += L".scx";
  (void)DeleteFileW(seed.c_str());

  int result = 5;
  if (document.save_retail_archive(archive_path, error)) {
    result = 6;
    if (!document.modified() && document.source_is_archive() &&
        document.format() == ScenarioFormat::retail_chk) {
      result = 7;
      EditorDocument reopened{};
      ScenarioProperties reopened_properties{};
      if (reopened.load(archive_path, data_root, error)) {
        result = 8;
        if (reopened.format() == ScenarioFormat::retail_chk &&
            reopened.unit_count() == 1U &&
            reopened.scenario_properties(reopened_properties) &&
            reopened_properties.name == properties.name &&
            reopened_properties.description == properties.description) {
          result = 9;
          starcraft::runtime::StormModule storm{data_root / L"storm.dll"};
          void* archive{};
          std::vector<std::uint8_t> chk_bytes{};
          const bool loaded =
              storm.loaded() &&
              storm.open_archive(archive_path, &archive, 4000U) &&
              storm.load_file_from_archive(
                  archive, R"(staredit\scenario.chk)", chk_bytes);
          const bool closed =
              archive == nullptr || storm.close_archive(archive);
          const starcraft::data::ChkView chk{chk_bytes.data(),
                                             chk_bytes.size()};
          starcraft::data::UnitPlacement unit{};
          starcraft::game::MultiplayerScenario scenario{};
          if (loaded && closed && chk.valid() && chk.unit_count() == 1U &&
              chk.unit(0U, unit) && unit.unit_type == unit_type &&
              unit.x == unit_x && unit.y == unit_y && scenario.load(chk)) {
            result = 0;
          }
        }
      }
    }
  } else if (error.find(L"could not be opened by Storm") !=
             std::wstring::npos) {
    result = 12;
  } else if (error.find(L"could not read staredit") != std::wstring::npos) {
    result = 13;
  } else if (error.find(L"different scenario.chk") != std::wstring::npos) {
    result = 14;
  } else if (error.find(L"invalid CHK") != std::wstring::npos) {
    result = 15;
  } else if (error.find(L"did not contain a retail") != std::wstring::npos) {
    result = 16;
  }
  (void)DeleteFileW(archive_path.c_str());
  return result;
}

int run_new_map_dialog_resource_probe(const HINSTANCE instance) noexcept {
  const HRSRC resource = FindResourceW(
      instance, MAKEINTRESOURCEW(IDD_NEW_SCENARIO), RT_DIALOG);
  if (resource == nullptr || SizeofResource(instance, resource) != 640U) {
    return 2;
  }
  const HGLOBAL loaded = LoadResource(instance, resource);
  const auto* const bytes =
      static_cast<const std::uint8_t*>(LockResource(loaded));
  if (bytes == nullptr) {
    return 3;
  }
  std::uint64_t hash = 1469598103934665603ULL;
  for (std::size_t index = 0U; index < 640U; ++index) {
    hash ^= bytes[index];
    hash *= 1099511628211ULL;
  }
  // FNV-1a of EditLocal.dll RT_DIALOG:1200 from the licensed 1998 editor.
  if (hash != 0x1AD9C6504571BC30ULL) {
    return 4;
  }
  const NewMapSettings original_defaults{};
  if (original_defaults.width != 128U ||
      original_defaults.height != 128U ||
      original_defaults.tileset_id != 0U ||
      original_defaults.terrain_index != 0U ||
      new_map_tilesets().size() != 5U) {
    return 5;
  }
  return 0;
}

int run_object_ui_probe(MainFrame& frame,
                        const std::filesystem::path& map) noexcept {
  if (!frame.open_document(map)) {
    return 2;
  }
  EditorDocument* const document = active_editor_document(frame.mdi_client());
  const HWND child = reinterpret_cast<HWND>(
      SendMessageW(frame.mdi_client(), WM_MDIGETACTIVE, 0, 0));
  const HWND palette = frame.brush_palette();
  if (document == nullptr || child == nullptr || palette == nullptr ||
      document->doodad_markers().empty() ||
      document->object_brushes(EditorLayer::doodads).empty()) {
    return 3;
  }
  const DoodadMarker target = document->doodad_markers().front();
  const auto& brushes = document->object_brushes(EditorLayer::doodads);
  const auto brush_at = std::find(brushes.begin(), brushes.end(), target.type);
  if (brush_at == brushes.end()) {
    return 4;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_LAYER_DOODADS, 0);
  RECT palette_client{};
  GetClientRect(palette, &palette_client);
  constexpr int cell_width = 54;
  constexpr int cell_height = 58;
  constexpr int header_height = 30;
  const int columns = (std::max)(1L, palette_client.right / cell_width);
  const std::size_t brush_index =
      static_cast<std::size_t>(brush_at - brushes.begin());
  const LPARAM palette_position = MAKELPARAM(
      static_cast<WORD>((brush_index % columns) * cell_width + cell_width / 2),
      static_cast<WORD>(header_height + (brush_index / columns) * cell_height +
                        cell_height / 2));
  SendMessageW(palette, WM_LBUTTONDOWN, MK_LBUTTON, palette_position);
  std::uint16_t selected{};
  if (!get_brush_palette_selection(palette, selected) ||
      selected != target.type) {
    return 5;
  }
  center_active_view(frame.mdi_client(), target.x, target.y);
  MapViewport viewport{};
  if (!query_active_viewport(frame.mdi_client(), viewport)) {
    return 6;
  }
  const LPARAM map_position = MAKELPARAM(
      static_cast<WORD>(target.x - viewport.world_x),
      static_cast<WORD>(target.y - viewport.world_y));
  const std::size_t before = document->doodad_count();
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, map_position);
  SendMessageW(child, WM_LBUTTONUP, 0, map_position);
  if (document->doodad_count() != before + 1U || !document->modified()) {
    return 7;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  if (document->doodad_count() != before || document->modified()) {
    return 8;
  }

  // Unit type 106 is the Terran Command Center. Exercise the same map-view
  // placement path used by a palette-selected building, not only the model API.
  constexpr std::uint16_t building_type = 106U;
  const auto& unit_brushes = document->object_brushes(EditorLayer::units);
  if (std::find(unit_brushes.begin(), unit_brushes.end(), building_type) ==
      unit_brushes.end()) {
    return 9;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_LAYER_UNITS, 0);
  const HWND search = FindWindowExW(palette, nullptr, L"Edit", nullptr);
  if (search == nullptr) {
    return 10;
  }
  if ((GetWindowLongPtrW(search, GWL_STYLE) & WS_VISIBLE) == 0) {
    return 15;
  }
  SetWindowTextW(search, L"Command Center");
  SendMessageW(palette, WM_LBUTTONDOWN, MK_LBUTTON,
               MAKELPARAM(54U, 58U + 52U));
  if (!get_brush_palette_selection(palette, selected) ||
      selected != building_type) {
    return 11;
  }
  SetWindowTextW(search, L"");
  set_brush_palette_selection(palette, building_type);
  set_active_brush(frame.mdi_client(), building_type);
  const int building_x = static_cast<int>(document->width()) *
                         static_cast<int>(EditorDocument::tile_width) / 2;
  const int building_y = static_cast<int>(document->height()) *
                         static_cast<int>(EditorDocument::tile_height) / 2;
  center_active_view(frame.mdi_client(), building_x, building_y);
  if (!query_active_viewport(frame.mdi_client(), viewport)) {
    return 12;
  }
  const LPARAM building_position = MAKELPARAM(
      static_cast<WORD>(building_x - viewport.world_x),
      static_cast<WORD>(building_y - viewport.world_y));
  const std::size_t units_before = document->unit_count();
  SendMessageW(child, WM_MOUSEMOVE, 0, building_position);
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, building_position);
  SendMessageW(child, WM_LBUTTONUP, 0, building_position);
  if (document->unit_count() != units_before + 1U ||
      document->unit_markers().back().type != building_type) {
    return 13;
  }
  SendMessageW(child, WM_RBUTTONDOWN, MK_RBUTTON, building_position);
  if (get_brush_palette_selection(palette, selected)) {
    return 14;
  }
  SendMessageW(child, WM_LBUTTONDOWN, MK_LBUTTON, building_position);
  const LPARAM moved_building_position = MAKELPARAM(
      static_cast<WORD>(building_x - viewport.world_x + 32),
      static_cast<WORD>(building_y - viewport.world_y + 16));
  SendMessageW(child, WM_MOUSEMOVE, MK_LBUTTON, moved_building_position);
  SendMessageW(child, WM_LBUTTONUP, 0, moved_building_position);
  if (document->unit_count() != units_before + 1U ||
      document->unit_markers().back().x != building_x + 32 ||
      document->unit_markers().back().y != building_y + 16) {
    return 16;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  if (document->unit_markers().back().x != building_x ||
      document->unit_markers().back().y != building_y) {
    return 17;
  }
  SendMessageW(frame.window(), WM_COMMAND, ID_EDIT_UNDO, 0);
  return document->unit_count() == units_before && !document->modified() ? 0
                                                                         : 18;
}

int run_object_art_probe(const std::filesystem::path& map,
                         const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error)) {
    return 2;
  }
  const auto count_art = [&document](const EditorLayer layer) {
    std::size_t count{};
    for (const std::uint16_t type : document.object_brushes(layer)) {
      if (document.object_art(layer, type) != nullptr) {
        ++count;
      }
    }
    return count;
  };
  const auto& unit_brushes = document.object_brushes(EditorLayer::units);
  constexpr std::uint16_t building_type = 106U;
  if (unit_brushes.size() != 228U ||
      count_art(EditorLayer::units) != unit_brushes.size() ||
      document.object_art(EditorLayer::units, building_type) == nullptr ||
      document.object_brush_name(EditorLayer::units, 0U).empty()) {
    return 3;
  }
  if (document.object_brushes(EditorLayer::sprites).size() != 267U ||
      count_art(EditorLayer::sprites) !=
          document.object_brushes(EditorLayer::sprites).size()) {
    return 4;
  }
  if (document.object_brushes(EditorLayer::doodads).empty() ||
      count_art(EditorLayer::doodads) !=
          document.object_brushes(EditorLayer::doodads).size()) {
    return 5;
  }
  const ObjectArtFrame* const building =
      document.object_art(EditorLayer::units, building_type);
  constexpr std::uint32_t background = 0xFF123456U;
  std::vector<std::uint32_t> preview{};
  if (building == nullptr ||
      !make_object_art_preview(*building, document.art_palette(),
                               document.team_colors(), 0U, background, 40, 40,
                               preview) ||
      std::all_of(preview.begin(), preview.end(), [](const std::uint32_t pixel) {
        return pixel == background;
      })) {
    return 6;
  }
  std::vector<std::uint32_t> map_pixels(160U * 160U, background);
  composite_object_art(*building, document.art_palette(),
                       document.team_colors(), 0U, map_pixels, 160, 160, 80,
                       80);
  if (std::all_of(map_pixels.begin(), map_pixels.end(),
                  [](const std::uint32_t pixel) {
                    return pixel == background;
                  })) {
    return 7;
  }
  const std::size_t units_before = document.unit_count();
  const std::uint16_t x = static_cast<std::uint16_t>(
      static_cast<std::uint32_t>(document.width()) *
      EditorDocument::tile_width / 2U);
  const std::uint16_t y = static_cast<std::uint16_t>(
      static_cast<std::uint32_t>(document.height()) *
      EditorDocument::tile_height / 2U);
  if (!document.place_object(EditorLayer::units, building_type, x, y, 0U) ||
      document.unit_count() != units_before + 1U ||
      document.unit_markers().back().type != building_type ||
      !document.undo() || document.unit_count() != units_before) {
    return 8;
  }
  return 0;
}

int run_object_edit_probe(const std::filesystem::path& map,
                          const std::filesystem::path& data_root) noexcept {
  EditorDocument document{};
  std::wstring error{};
  if (!document.load(map, data_root, error) ||
      document.doodad_markers().empty() || document.unit_markers().empty() ||
      document.sprite_markers().empty()) {
    return 2;
  }
  const std::size_t doodads = document.doodad_count();
  const std::size_t units = document.unit_count();
  const std::size_t sprites = document.sprite_count();
  const std::uint64_t terrain = document.game_tile_checksum();
  UnitAttributes original_attributes{};
  if (!document.unit_attributes(0U, original_attributes)) {
    return 20;
  }
  const std::uint32_t map_right =
      static_cast<std::uint32_t>(document.width()) *
          EditorDocument::tile_width -
      1U;
  const std::uint16_t moved_x = static_cast<std::uint16_t>(
      static_cast<std::uint32_t>(original_attributes.x) + 24U <= map_right
          ? original_attributes.x + 24U
          : original_attributes.x - 24U);
  if (!document.move_unit(0U, moved_x, original_attributes.y)) {
    return 21;
  }
  UnitAttributes changed_attributes{};
  if (!document.unit_attributes(0U, changed_attributes) ||
      changed_attributes.x != moved_x || !document.undo() ||
      !document.unit_attributes(0U, changed_attributes) ||
      changed_attributes.x != original_attributes.x || document.modified()) {
    return 22;
  }
  changed_attributes = original_attributes;
  changed_attributes.owner = static_cast<std::uint8_t>(
      (changed_attributes.owner + 1U) %
      starcraft::data::chk_player_slot_count);
  if (!document.update_unit(0U, changed_attributes) ||
      !document.unit_attributes(0U, changed_attributes) ||
      changed_attributes.owner == original_attributes.owner ||
      !document.undo() || document.modified()) {
    return 23;
  }
  const DoodadMarker doodad = document.doodad_markers().front();
  if (!document.erase_object_at(EditorLayer::doodads, doodad.x, doodad.y) ||
      document.doodad_count() != doodads - 1U || !document.undo() ||
      document.doodad_count() != doodads ||
      document.game_tile_checksum() != terrain || document.modified()) {
    return 3;
  }
  if (!document.place_object(EditorLayer::doodads, doodad.type, doodad.x,
                             doodad.y) ||
      document.doodad_count() != doodads + 1U || !document.can_undo() ||
      !document.undo() || document.doodad_count() != doodads ||
      document.game_tile_checksum() != terrain || document.modified() ||
      !document.redo() || document.doodad_count() != doodads + 1U) {
    return 4;
  }
  const UnitMarker unit = document.unit_markers().front();
  if (!document.erase_object_at(EditorLayer::units, unit.x, unit.y) ||
      document.unit_count() != units - 1U || !document.undo() ||
      document.unit_count() != units) {
    return 5;
  }
  if (!document.place_object(EditorLayer::units, unit.type, unit.x, unit.y,
                             unit.owner) ||
      document.unit_count() != units + 1U) {
    return 6;
  }
  const SpriteMarker sprite = document.sprite_markers().front();
  if (!document.erase_object_at(EditorLayer::sprites, sprite.x, sprite.y) ||
      document.sprite_count() != sprites - 1U || !document.undo() ||
      document.sprite_count() != sprites) {
    return 7;
  }
  if (!document.place_object(EditorLayer::sprites, sprite.type, sprite.x,
                             sprite.y, sprite.owner) ||
      document.sprite_count() != sprites + 1U) {
    return 8;
  }

  std::array<wchar_t, MAX_PATH> temporary_directory{};
  std::array<wchar_t, MAX_PATH> seed_path{};
  const DWORD directory_length = GetTempPathW(
      static_cast<DWORD>(temporary_directory.size()), temporary_directory.data());
  if (directory_length == 0U ||
      directory_length >= temporary_directory.size() ||
      GetTempFileNameW(temporary_directory.data(), L"seo", 0U,
                       seed_path.data()) == 0U) {
    return 9;
  }
  const std::filesystem::path seed{seed_path.data()};
  std::filesystem::path exported = seed;
  exported += L".chk";
  (void)DeleteFileW(seed.c_str());
  int result = 10;
  if (document.export_raw_chk(exported, error)) {
    EditorDocument reopened{};
    if (reopened.load(exported, data_root, error) &&
        reopened.doodad_count() == doodads + 1U &&
        reopened.unit_count() == units + 1U &&
        reopened.sprite_count() == sprites + 1U &&
        reopened.chk_round_trip_matches_source()) {
      result = 0;
    }
  }
  (void)DeleteFileW(exported.c_str());
  return result;
}

}  // namespace

int run_editor(const HINSTANCE instance, const int show_command) noexcept {
  SetProcessDPIAware();
  INITCOMMONCONTROLSEX controls{};
  controls.dwSize = sizeof(controls);
  controls.dwICC = ICC_BAR_CLASSES | ICC_WIN95_CLASSES;
  if (InitCommonControlsEx(&controls) == FALSE) {
    return 1;
  }

  const std::filesystem::path data_root = executable_directory();
  if (data_root.empty()) {
    return 1;
  }

  std::vector<std::wstring> arguments{};
  try {
    arguments = command_line_arguments();
  } catch (...) {
    return 1;
  }
  constexpr std::wstring_view probe_prefix = L"--probe-map=";
  constexpr std::wstring_view round_trip_prefix = L"--probe-roundtrip=";
  constexpr std::wstring_view isom_probe_prefix = L"--probe-isom=";
  constexpr std::wstring_view isom_topology_probe_prefix =
      L"--probe-isom-topology=";
  constexpr std::wstring_view isom_edit_probe_prefix =
      L"--probe-isom-edit=";
  constexpr std::wstring_view terrain_edit_prefix = L"--probe-terrain-edit=";
  constexpr std::wstring_view raw_export_prefix = L"--probe-raw-export=";
  constexpr std::wstring_view ui_probe_prefix = L"--probe-ui=";
  constexpr std::wstring_view ui_edit_probe_prefix = L"--probe-ui-edit=";
  constexpr std::wstring_view palette_probe_prefix = L"--probe-palette=";
  constexpr std::wstring_view brush_footprint_probe_prefix =
      L"--probe-brush-footprint=";
  constexpr std::wstring_view layout_probe_prefix = L"--probe-layout=";
  constexpr std::wstring_view object_edit_probe_prefix =
      L"--probe-object-edit=";
  constexpr std::wstring_view object_art_probe_prefix =
      L"--probe-object-art=";
  constexpr std::wstring_view object_ui_probe_prefix = L"--probe-object-ui=";
  constexpr std::wstring_view new_map_probe_argument = L"--probe-new-map";
  constexpr std::wstring_view new_map_variants_probe_argument =
      L"--probe-new-map-variants";
  constexpr std::wstring_view new_map_dialog_probe_argument =
      L"--probe-new-map-dialog-resource";
  constexpr std::wstring_view editor_layers_probe_argument =
      L"--probe-editor-layers";
  constexpr std::wstring_view retail_save_probe_argument =
      L"--probe-retail-save";
  std::filesystem::path ui_probe_map{};
  std::filesystem::path ui_edit_probe_map{};
  std::filesystem::path palette_probe_map{};
  std::filesystem::path brush_footprint_probe_map{};
  std::filesystem::path layout_probe_map{};
  std::filesystem::path object_ui_probe_map{};
  bool new_map_probe{};
  for (std::size_t index = 1U; index < arguments.size(); ++index) {
    const std::wstring_view argument{arguments[index]};
    if (argument == new_map_probe_argument) {
      new_map_probe = true;
    }
    if (argument == new_map_variants_probe_argument) {
      return run_new_map_variants_probe(data_root);
    }
    if (argument == new_map_dialog_probe_argument) {
      return run_new_map_dialog_resource_probe(instance);
    }
    if (argument == editor_layers_probe_argument) {
      return run_editor_layers_probe(data_root);
    }
    if (argument == retail_save_probe_argument) {
      return run_retail_save_probe(data_root);
    }
    if (argument.rfind(probe_prefix, 0U) == 0U) {
      return run_probe(std::filesystem::path{argument.substr(probe_prefix.size())},
                       data_root);
    }
    if (argument.rfind(round_trip_prefix, 0U) == 0U) {
      return run_round_trip_probe(
          std::filesystem::path{argument.substr(round_trip_prefix.size())},
          data_root);
    }
    if (argument.rfind(isom_probe_prefix, 0U) == 0U) {
      return run_isom_probe(
          std::filesystem::path{argument.substr(isom_probe_prefix.size())},
          data_root);
    }
    if (argument.rfind(isom_topology_probe_prefix, 0U) == 0U) {
      return run_isom_topology_probe(
          std::filesystem::path{
              argument.substr(isom_topology_probe_prefix.size())},
          data_root);
    }
    if (argument.rfind(isom_edit_probe_prefix, 0U) == 0U) {
      return run_isom_edit_probe(
          std::filesystem::path{
              argument.substr(isom_edit_probe_prefix.size())},
          data_root);
    }
    if (argument.rfind(terrain_edit_prefix, 0U) == 0U) {
      return run_terrain_edit_probe(
          std::filesystem::path{argument.substr(terrain_edit_prefix.size())},
          data_root);
    }
    if (argument.rfind(raw_export_prefix, 0U) == 0U) {
      return run_raw_export_probe(
          std::filesystem::path{argument.substr(raw_export_prefix.size())},
          data_root);
    }
    if (argument.rfind(object_edit_probe_prefix, 0U) == 0U) {
      return run_object_edit_probe(
          std::filesystem::path{
              argument.substr(object_edit_probe_prefix.size())},
          data_root);
    }
    if (argument.rfind(object_art_probe_prefix, 0U) == 0U) {
      return run_object_art_probe(
          std::filesystem::path{
              argument.substr(object_art_probe_prefix.size())},
          data_root);
    }
    if (argument.rfind(ui_probe_prefix, 0U) == 0U) {
      ui_probe_map = std::filesystem::path{
          argument.substr(ui_probe_prefix.size())};
    }
    if (argument.rfind(ui_edit_probe_prefix, 0U) == 0U) {
      ui_edit_probe_map = std::filesystem::path{
          argument.substr(ui_edit_probe_prefix.size())};
    }
    if (argument.rfind(palette_probe_prefix, 0U) == 0U) {
      palette_probe_map = std::filesystem::path{
          argument.substr(palette_probe_prefix.size())};
    }
    if (argument.rfind(brush_footprint_probe_prefix, 0U) == 0U) {
      brush_footprint_probe_map = std::filesystem::path{
          argument.substr(brush_footprint_probe_prefix.size())};
    }
    if (argument.rfind(layout_probe_prefix, 0U) == 0U) {
      layout_probe_map = std::filesystem::path{
          argument.substr(layout_probe_prefix.size())};
    }
    if (argument.rfind(object_ui_probe_prefix, 0U) == 0U) {
      object_ui_probe_map = std::filesystem::path{
          argument.substr(object_ui_probe_prefix.size())};
    }
  }

  if (!register_map_view_class(instance) ||
      !register_brush_palette_class(instance) ||
      !register_minimap_class(instance) ||
      !MainFrame::register_class(instance)) {
    return 1;
  }
  MainFrame frame{data_root};
  const bool hidden_probe = !ui_probe_map.empty() || !ui_edit_probe_map.empty() ||
                            !palette_probe_map.empty() ||
                            !brush_footprint_probe_map.empty() ||
                            !layout_probe_map.empty() ||
                            !object_ui_probe_map.empty() || new_map_probe;
  if (!frame.create(instance, hidden_probe ? SW_HIDE : show_command)) {
    return 1;
  }

  if (!ui_probe_map.empty()) {
    const bool loaded = frame.open_document(ui_probe_map);
    const bool initialized = loaded && frame.window() != nullptr &&
                             frame.mdi_client() != nullptr &&
                             active_document(frame.mdi_client()) != nullptr;
    DestroyWindow(frame.window());
    return initialized ? 0 : 4;
  }
  if (!ui_edit_probe_map.empty()) {
    const int result = run_ui_edit_probe(frame, ui_edit_probe_map);
    DestroyWindow(frame.window());
    return result;
  }
  if (!palette_probe_map.empty()) {
    const int result = run_palette_probe(frame, palette_probe_map);
    DestroyWindow(frame.window());
    return result;
  }
  if (!brush_footprint_probe_map.empty()) {
    const int result =
        run_brush_footprint_probe(frame, brush_footprint_probe_map);
    DestroyWindow(frame.window());
    return result;
  }
  if (!layout_probe_map.empty()) {
    const int result = run_layout_probe(frame, layout_probe_map);
    DestroyWindow(frame.window());
    return result;
  }
  if (!object_ui_probe_map.empty()) {
    const int result = run_object_ui_probe(frame, object_ui_probe_map);
    DestroyWindow(frame.window());
    return result;
  }
  if (new_map_probe) {
    const int result = run_new_map_probe(frame);
    DestroyWindow(frame.window());
    return result;
  }

  bool opened_document{};
  for (std::size_t index = 1U; index < arguments.size(); ++index) {
    if (!arguments[index].empty() && arguments[index][0] != L'-') {
      opened_document = frame.open_document(arguments[index]) || opened_document;
    }
  }
  if (!opened_document && !frame.new_default_document()) {
    DestroyWindow(frame.window());
    return 1;
  }

  MSG message{};
  while (true) {
    const BOOL result = GetMessageW(&message, nullptr, 0, 0);
    if (result == 0) {
      return static_cast<int>(message.wParam);
    }
    if (result < 0) {
      return 1;
    }
    if (TranslateMDISysAccel(frame.mdi_client(), &message) == FALSE &&
        TranslateAcceleratorW(frame.window(), frame.accelerators(), &message) ==
            0) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
  }
}

}  // namespace staredit

int WINAPI wWinMain(const HINSTANCE instance,
                    HINSTANCE,
                    PWSTR,
                    const int show_command) {
  return staredit::run_editor(instance, show_command);
}
