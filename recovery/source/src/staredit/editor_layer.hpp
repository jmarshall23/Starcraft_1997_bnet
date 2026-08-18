#pragma once

#include <cstdint>

namespace staredit {

enum class EditorLayer : std::uint8_t {
  terrain,
  doodads,
  units,
  locations,
  sprites,
  fog,
};

[[nodiscard]] constexpr const wchar_t* editor_layer_name(
    const EditorLayer layer) noexcept {
  switch (layer) {
    case EditorLayer::terrain:
      return L"Terrain";
    case EditorLayer::doodads:
      return L"Doodads";
    case EditorLayer::units:
      return L"Units";
    case EditorLayer::locations:
      return L"Locations";
    case EditorLayer::sprites:
      return L"Sprites";
    case EditorLayer::fog:
      return L"Fog of War";
  }
  return L"Terrain";
}

}  // namespace staredit
