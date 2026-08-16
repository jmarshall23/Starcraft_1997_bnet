#pragma once

namespace starcraft::runtime {

using InvariantHandler = void (*)(const char* source_file, int source_line, const char* expression);

void set_invariant_handler(InvariantHandler handler) noexcept;

[[noreturn]] void fail_invariant(
    const char* source_file,
    int source_line,
    const char* expression) noexcept;

inline void require(
    const bool condition,
    const char* source_file,
    const int source_line,
    const char* expression) noexcept {
  if (!condition) {
    fail_invariant(source_file, source_line, expression);
  }
}

}  // namespace starcraft::runtime
