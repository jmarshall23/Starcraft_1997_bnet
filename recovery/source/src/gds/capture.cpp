#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

bool capture_opengl_bmp(const HWND window, RecoveryWindowState &state,
                        const char *const path) noexcept {
  if (path == nullptr || *path == '\0' || state.device_context == nullptr ||
      state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    return false;
  }
  RECT client{};
  if (!GetClientRect(window, &client) || client.right <= 0 ||
      client.bottom <= 0) {
    return false;
  }
  try {
    const std::size_t byte_count =
        static_cast<std::size_t>(client.right) * client.bottom * 4U;
    if (byte_count > UINT32_MAX) {
      return false;
    }
    std::vector<std::uint8_t> pixels(byte_count);
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, client.right, client.bottom, kGlBgra, GL_UNSIGNED_BYTE,
                 pixels.data());
    if (glGetError() != GL_NO_ERROR) {
      return false;
    }

    BITMAPFILEHEADER file_header{};
    BITMAPINFOHEADER info_header{};
    file_header.bfType = 0x4D42;
    file_header.bfOffBits = sizeof(file_header) + sizeof(info_header);
    file_header.bfSize = file_header.bfOffBits + static_cast<DWORD>(byte_count);
    info_header.biSize = sizeof(info_header);
    info_header.biWidth = client.right;
    info_header.biHeight = client.bottom;
    info_header.biPlanes = 1;
    info_header.biBitCount = 32;
    info_header.biCompression = BI_RGB;
    info_header.biSizeImage = static_cast<DWORD>(byte_count);

    const HANDLE file =
        CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
      return false;
    }
    DWORD written{};
    const bool okay =
        WriteFile(file, &file_header, sizeof(file_header), &written, nullptr) !=
            FALSE &&
        written == sizeof(file_header) &&
        WriteFile(file, &info_header, sizeof(info_header), &written, nullptr) !=
            FALSE &&
        written == sizeof(info_header) &&
        WriteFile(file, pixels.data(), static_cast<DWORD>(pixels.size()),
                  &written, nullptr) != FALSE &&
        written == pixels.size();
    CloseHandle(file);
    return okay;
  } catch (...) {
    return false;
  }
}

} // namespace starcraft::recovery
