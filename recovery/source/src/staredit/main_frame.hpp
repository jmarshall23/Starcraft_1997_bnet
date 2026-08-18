#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>

#include <windows.h>

#include "editor_layer.hpp"
#include "new_map_dialog.hpp"

namespace staredit {

class EditorDocument;

class MainFrame final {
 public:
  explicit MainFrame(std::filesystem::path data_root) noexcept;

  [[nodiscard]] static bool register_class(HINSTANCE instance) noexcept;
  [[nodiscard]] bool create(HINSTANCE instance, int show_command) noexcept;
  // File/New: shows the recovered New Scenario dialog.
  [[nodiscard]] bool new_document() noexcept;
  // Startup/probe path: creates the requested 96x96 Jungle Dirt map silently.
  [[nodiscard]] bool new_default_document() noexcept;
  [[nodiscard]] bool new_document(const NewMapSettings& settings) noexcept;
  [[nodiscard]] bool open_document(const std::filesystem::path& path) noexcept;
  [[nodiscard]] HWND window() const noexcept;
  [[nodiscard]] HWND mdi_client() const noexcept;
  [[nodiscard]] HWND brush_palette() const noexcept;
  [[nodiscard]] HWND minimap() const noexcept;
  [[nodiscard]] HACCEL accelerators() const noexcept;

 private:
  [[nodiscard]] bool on_create() noexcept;
  void on_size() noexcept;
  void on_command(UINT command) noexcept;
  [[nodiscard]] bool attach_document(
      std::unique_ptr<EditorDocument> document) noexcept;
  void choose_and_open_document() noexcept;
  void export_active_chk() noexcept;
  void close_active_document() noexcept;
  [[nodiscard]] bool close_all_documents() noexcept;
  void show_scenario_summary() const noexcept;
  void update_status() noexcept;

  static LRESULT CALLBACK window_proc(HWND window,
                                      UINT message,
                                      WPARAM wparam,
                                      LPARAM lparam) noexcept;

  std::filesystem::path data_root_{};
  HINSTANCE instance_{};
  HWND window_{};
  HWND mdi_client_{};
  HWND brush_palette_{};
  HWND minimap_{};
  HWND status_bar_{};
  HMENU menu_{};
  HMENU window_menu_{};
  HACCEL accelerators_{};
  bool grid_visible_{};
  bool units_visible_{true};
  bool brush_palette_visible_{true};
  std::uint8_t brush_size_{1U};
  EditorLayer active_layer_{EditorLayer::terrain};
  NewMapSettings new_map_defaults_{};
};

}  // namespace staredit
