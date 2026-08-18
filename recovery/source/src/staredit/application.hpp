#pragma once

#include <windows.h>

namespace staredit {

[[nodiscard]] int run_editor(HINSTANCE instance,
                             int show_command) noexcept;

}  // namespace staredit
