#pragma once

#include <windows.h>

namespace staredit {

class EditorDocument;

[[nodiscard]] bool show_player_settings_dialog(
    HWND parent,
    HINSTANCE instance,
    EditorDocument& document) noexcept;

[[nodiscard]] bool show_scenario_properties_dialog(
    HWND parent,
    HINSTANCE instance,
    EditorDocument& document) noexcept;

[[nodiscard]] bool show_forces_dialog(HWND parent,
                                      HINSTANCE instance,
                                      EditorDocument& document) noexcept;

[[nodiscard]] bool show_sounds_dialog(HWND parent,
                                      HINSTANCE instance,
                                      EditorDocument& document) noexcept;

[[nodiscard]] bool show_triggers_dialog(HWND parent,
                                         HINSTANCE instance,
                                         EditorDocument& document) noexcept;

[[nodiscard]] bool show_mission_briefing_dialog(
    HWND parent,
    HINSTANCE instance,
    EditorDocument& document) noexcept;

}  // namespace staredit
