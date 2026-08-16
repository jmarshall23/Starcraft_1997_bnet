#include "starcraft/runtime/invariant.hpp"

#include <atomic>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace starcraft::runtime {
namespace {

std::atomic<InvariantHandler> g_handler{nullptr};

}  // namespace

void set_invariant_handler(const InvariantHandler handler) noexcept {
  g_handler.store(handler, std::memory_order_release);
}

[[noreturn]] void fail_invariant(
    const char* const source_file,
    const int source_line,
    const char* const expression) noexcept {
  if (const auto handler = g_handler.load(std::memory_order_acquire); handler != nullptr) {
    handler(source_file, source_line, expression);
  }

  char message[512]{};
  std::snprintf(
      message,
      sizeof(message),
      "Recovered invariant failed: %s(%d): %s\n",
      source_file,
      source_line,
      expression);

#if defined(_WIN32)
  OutputDebugStringA(message);
#endif
  std::fputs(message, stderr);
  std::abort();
}

}  // namespace starcraft::runtime
