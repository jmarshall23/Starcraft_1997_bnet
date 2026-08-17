#include "bootstrap_runtime.hpp"

#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_win32.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

namespace starcraft::recovery {

namespace {

constexpr std::uint32_t kFullResourceAmount = 999999U;

std::string normalized_command(const std::string_view command) {
  const auto first = std::find_if_not(
      command.begin(), command.end(),
      [](const unsigned char value) { return std::isspace(value) != 0; });
  const auto last = std::find_if_not(
                        command.rbegin(), command.rend(),
                        [](const unsigned char value) {
                          return std::isspace(value) != 0;
                        })
                        .base();
  if (first >= last) {
    return {};
  }
  std::string result(first, last);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](const unsigned char value) {
                   return static_cast<char>(std::tolower(value));
                 });
  return result;
}

void set_all_minerals(BootstrapStatus &status) noexcept {
  status.player_minerals = kFullResourceAmount;
  status.displayed_minerals = kFullResourceAmount;
  status.player_mineral_stock.fill(kFullResourceAmount);
}

void set_all_gas(BootstrapStatus &status) noexcept {
  status.player_gas = kFullResourceAmount;
  status.displayed_gas = kFullResourceAmount;
  status.player_gas_stock.fill(kFullResourceAmount);
}

bool set_boolean_cvar(const std::string_view value, bool &target) noexcept {
  if (value == "1" || value == "on" || value == "true") {
    target = true;
    return true;
  }
  if (value == "0" || value == "off" || value == "false") {
    target = false;
    return true;
  }
  return false;
}

} // namespace

bool initialize_debug_console(const HWND window,
                              RecoveryWindowState &state) noexcept {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowRounding = 0.0F;
  style.WindowBorderSize = 0.0F;
  style.WindowPadding = ImVec2(8.0F, 6.0F);
  if (!ImGui_ImplWin32_Init(window)) {
    ImGui::DestroyContext();
    return false;
  }
  if (!ImGui_ImplOpenGL2_Init()) {
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    return false;
  }
  state.imgui_ready = true;
  state.debug_console_lines.emplace_back(
      "StarCraft recovery console - type 'help' for commands");
  return true;
}

void shutdown_debug_console(RecoveryWindowState &state) noexcept {
  if (!state.imgui_ready) {
    return;
  }
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
  state.imgui_ready = false;
}

bool execute_debug_console_command(BootstrapStatus &status,
                                   const std::string_view command,
                                   std::string &result) noexcept {
  const std::string normalized = normalized_command(command);
  std::string_view cvar_command = normalized;
  if (cvar_command.rfind("set ", 0U) == 0U) {
    cvar_command.remove_prefix(4U);
  }
  const std::size_t separator = cvar_command.find(' ');
  const std::string_view cvar_name = cvar_command.substr(0U, separator);
  const std::string_view cvar_value =
      separator == std::string_view::npos
          ? std::string_view{}
          : cvar_command.substr(cvar_command.find_first_not_of(' ', separator));
  if (cvar_name == "fog_of_war" || cvar_name == "r_fog_of_war" ||
      cvar_name == "fow") {
    if (!cvar_value.empty() &&
        !set_boolean_cvar(cvar_value, status.fog_of_war_enabled)) {
      result = "fog_of_war expects 0/1, off/on, or false/true";
      return false;
    }
    if (status.fog_of_war_enabled) {
      // A disabled renderer never mutates the recovered current/explored
      // masks. Re-enabling can therefore use the live history directly.
      (void)rebuild_fog_render_surfaces(status);
      for (ScenarioUnitPreview &unit : status.units) {
        if (unit.selected && !fog_unit_visible(status, unit)) {
          unit.selected = false;
        }
      }
    }
    result = std::string{"fog_of_war = "} +
             (status.fog_of_war_enabled ? "1" : "0");
    return true;
  }
  if (normalized == "fullmoney") {
    set_all_minerals(status);
    set_all_gas(status);
    result = "minerals and gas set to 999999 for every player slot";
    return true;
  }
  if (normalized == "fullminerals") {
    set_all_minerals(status);
    result = "minerals set to 999999 for every player slot";
    return true;
  }
  if (normalized == "fullgas") {
    set_all_gas(status);
    result = "gas set to 999999 for every player slot";
    return true;
  }
  if (normalized == "help") {
    result = "commands: fog_of_war [0|1], fullmoney, fullminerals, "
             "fullgas, clear, help";
    return true;
  }
  result = normalized.empty() ? "empty command"
                              : "unknown command: " + normalized;
  return false;
}

void draw_debug_console(RecoveryWindowState &state,
                        const PresentationViewport &viewport) noexcept {
  if (!state.imgui_ready) {
    return;
  }
  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  if (state.debug_console_open) {
    ImGui::SetNextWindowPos(
        ImVec2(static_cast<float>(viewport.x), 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(static_cast<float>(viewport.width),
               static_cast<float>(viewport.height) * 0.45F),
        ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.90F);
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("RecoveryConsole", nullptr, flags)) {
      const float input_height = ImGui::GetFrameHeightWithSpacing();
      if (ImGui::BeginChild("ConsoleLog", ImVec2(0.0F, -input_height),
                            ImGuiChildFlags_Borders)) {
        for (const std::string &line : state.debug_console_lines) {
          ImGui::TextUnformatted(line.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0F) {
          ImGui::SetScrollHereY(1.0F);
        }
      }
      ImGui::EndChild();
      if (state.debug_console_focus) {
        ImGui::SetKeyboardFocusHere();
        state.debug_console_focus = false;
      }
      const bool entered = ImGui::InputTextWithHint(
          "##ConsoleInput", "] command", state.debug_console_input.data(),
          state.debug_console_input.size(), ImGuiInputTextFlags_EnterReturnsTrue);
      if (entered) {
        const std::string command = state.debug_console_input.data();
        state.debug_console_lines.emplace_back("] " + command);
        if (normalized_command(command) == "clear") {
          state.debug_console_lines.clear();
        } else if (state.status != nullptr) {
          std::string response;
          (void)execute_debug_console_command(*state.status, command, response);
          state.debug_console_lines.push_back(std::move(response));
        } else {
          state.debug_console_lines.emplace_back("no active game state");
        }
        state.debug_console_input.fill('\0');
        state.debug_console_focus = true;
      }
    }
    ImGui::End();
  }
  ImGui::Render();
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

bool debug_console_wants_mouse() noexcept {
  return ImGui::GetCurrentContext() != nullptr &&
         ImGui::GetIO().WantCaptureMouse;
}

bool debug_console_wants_keyboard() noexcept {
  return ImGui::GetCurrentContext() != nullptr &&
         ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace starcraft::recovery
