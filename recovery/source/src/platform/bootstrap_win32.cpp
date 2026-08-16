#include "bootstrap_runtime.hpp"

#include <cstring>
#include <string>

namespace starcraft::recovery {

constexpr char kWindowClass[] = "StarcraftBetaRecovered";
constexpr char kWindowTitle[] = "Starcraft Beta - Source Recovery Bootstrap";

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
  WNDCLASSA window_class{};
  window_class.style = CS_DBLCLKS | CS_OWNDC;
  window_class.lpfnWndProc = recovery_window_proc;
  window_class.hInstance = instance;
  window_class.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
  window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
  window_class.hbrBackground = nullptr;
  window_class.lpszClassName = kWindowClass;

  if (RegisterClassA(&window_class) == 0) {
    return 1;
  }

  std::string window_title = kWindowTitle;
  if (!status.detail.empty()) {
    window_title += " | ";
    window_title += status.detail;
  }
  RECT requested_window{0, 0, 960, 600};
  AdjustWindowRect(&requested_window, WS_OVERLAPPEDWINDOW, FALSE);
  const HWND window = CreateWindowExA(
      0, kWindowClass, window_title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
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
  SetTimer(window, 1, kSimulationTickMilliseconds, nullptr);

  MSG message{};
  while (GetMessageA(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageA(&message);
  }
  return static_cast<int>(message.wParam);
}
