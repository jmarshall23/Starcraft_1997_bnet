#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include <windows.h>

namespace staredit {

struct NewMapSettings {
  std::uint16_t width{128U};
  std::uint16_t height{128U};
  std::uint16_t tileset_id{};
  std::uint16_t terrain_index{};
};

struct NewMapTileset {
  std::uint16_t id{};
  std::wstring_view name{};
  std::array<std::string_view, 13> terrain{};
  std::uint16_t default_terrain{};
};

inline constexpr std::array<std::uint16_t, 5> new_map_dimensions{
    64U, 96U, 128U, 192U, 256U};

[[nodiscard]] const std::array<NewMapTileset, 5>&
new_map_tilesets() noexcept;
[[nodiscard]] const NewMapTileset* new_map_tileset(
    std::uint16_t id) noexcept;
[[nodiscard]] std::string_view new_map_terrain_name(
    const NewMapSettings& settings) noexcept;
[[nodiscard]] bool valid_new_map_settings(
    const NewMapSettings& settings) noexcept;

// Returns true only when the user accepts the recovered New Scenario dialog.
[[nodiscard]] bool show_new_map_dialog(HWND owner,
                                       HINSTANCE instance,
                                       NewMapSettings& settings) noexcept;

}  // namespace staredit
