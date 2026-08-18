#include "bootstrap_runtime.hpp"
#include "resource.h"

#include <charconv>
#include <cctype>
#include <cstring>
#include <string>
#include <string_view>

namespace starcraft::recovery {

constexpr char kWindowClass[] = "StarcraftBattleNetBeta";
constexpr char kWindowTitle[] = "Starcraft - Battle.Net Beta";

std::string command_line_option(const char *const command_line,
                                const std::string_view option) {
  if (command_line == nullptr) {
    return {};
  }
  const char *const found = std::strstr(command_line, option.data());
  if (found == nullptr) {
    return {};
  }
  const char *const begin = found + option.size();
  const char *end = begin;
  while (*end != '\0' && std::isspace(static_cast<unsigned char>(*end)) == 0) {
    ++end;
  }
  return {begin, end};
}

void configure_battle_server(GlueRuntime &glue,
                             const char *const command_line) {
  const std::string host =
      command_line_option(command_line, "--battle-host=");
  if (!host.empty()) {
    glue.battle_net.server_host = host;
  }
  const std::string port_text =
      command_line_option(command_line, "--battle-port=");
  unsigned int port{};
  if (!port_text.empty()) {
    const auto parsed =
        std::from_chars(port_text.data(), port_text.data() + port_text.size(),
                        port);
    if (parsed.ec == std::errc{} && parsed.ptr == port_text.data() +
                                                    port_text.size() &&
        port > 0U && port <= 65535U) {
      glue.battle_net.server_port = static_cast<std::uint16_t>(port);
    }
  }
}

} // namespace starcraft::recovery

using namespace starcraft::recovery;

int WINAPI WinMain(const HINSTANCE instance, HINSTANCE, LPSTR command_line,
                   const int show_command) {
  BootstrapStatus status = probe_assets();
  if (command_line != nullptr &&
      std::strstr(command_line, "--probe-assets") != nullptr) {
    return status.assets_ready ? 0
                               : (status.failed_runtime_unit_type == 0xFFFFU
                                      ? 10
                                      : 1000 + status.failed_runtime_unit_type);
  }

  RecoveryWindowState window_state{&status};
  (void)initialize_glue_assets(window_state.glue);
  configure_battle_server(window_state.glue, command_line);
  (void)initialize_game_dialog_assets(window_state.game_dialog, status);
  WNDCLASSEXA window_class{};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_DBLCLKS | CS_OWNDC;
  window_class.lpfnWndProc = recovery_window_proc;
  window_class.hInstance = instance;
  window_class.hIcon = static_cast<HICON>(LoadImageA(
      instance, MAKEINTRESOURCEA(IDI_STARCRAFT_ICON), IMAGE_ICON,
      GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0));
  window_class.hIconSm = static_cast<HICON>(LoadImageA(
      instance, MAKEINTRESOURCEA(IDI_STARCRAFT_ICON), IMAGE_ICON,
      GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0));
  if (window_class.hIcon == nullptr) {
    window_class.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
  }
  if (window_class.hIconSm == nullptr) {
    window_class.hIconSm = window_class.hIcon;
  }
  window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
  window_class.hbrBackground = nullptr;
  window_class.lpszClassName = kWindowClass;

  if (RegisterClassExA(&window_class) == 0) {
    return 1;
  }

  RECT requested_window{0, 0, 960, 600};
  AdjustWindowRect(&requested_window, WS_OVERLAPPEDWINDOW, FALSE);
  const HWND window = CreateWindowExA(
      0, kWindowClass, kWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
      CW_USEDEFAULT, requested_window.right - requested_window.left,
      requested_window.bottom - requested_window.top, nullptr, nullptr,
      instance, &window_state);
  if (window == nullptr) {
    return 2;
  }

  bool probe_handled{};
  const int probe_result = run_bootstrap_probes(
      command_line, window, window_state, status, probe_handled);
  if (probe_handled) {
    return probe_result;
  }

  ShowWindow(window, show_command);
  UpdateWindow(window);
  return run_game_loop(window, window_state);
}
