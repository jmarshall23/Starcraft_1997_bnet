#include "../platform/bootstrap_runtime.hpp"

#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

bool initialize_opengl(const HWND window, RecoveryWindowState &state) noexcept {
  state.device_context = GetDC(window);
  if (state.device_context == nullptr) {
    return false;
  }
  PIXELFORMATDESCRIPTOR descriptor{};
  descriptor.nSize = sizeof(descriptor);
  descriptor.nVersion = 1;
  descriptor.dwFlags =
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  descriptor.iPixelType = PFD_TYPE_RGBA;
  descriptor.cColorBits = 32;
  descriptor.cAlphaBits = 8;
  descriptor.iLayerType = PFD_MAIN_PLANE;
  const int format = ChoosePixelFormat(state.device_context, &descriptor);
  if (format == 0 ||
      !SetPixelFormat(state.device_context, format, &descriptor)) {
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
    return false;
  }
  state.rendering_context = wglCreateContext(state.device_context);
  if (state.rendering_context == nullptr ||
      !wglMakeCurrent(state.device_context, state.rendering_context)) {
    if (state.rendering_context != nullptr) {
      wglDeleteContext(state.rendering_context);
      state.rendering_context = nullptr;
    }
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
    return false;
  }
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  state.font_display_lists = glGenLists(96);
  const HFONT font =
      CreateFontA(-10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  NONANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, "Arial");
  if (state.font_display_lists == 0 || font == nullptr) {
    if (font != nullptr) {
      DeleteObject(font);
    }
    return false;
  }
  const HGDIOBJ previous = SelectObject(state.device_context, font);
  const bool font_ready = wglUseFontBitmapsA(state.device_context, 32, 96,
                                             state.font_display_lists) != FALSE;
  SelectObject(state.device_context, previous);
  DeleteObject(font);
  state.glue_font_display_lists = glGenLists(96);
  const HFONT glue_font = CreateFontA(
      -20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
      VARIABLE_PITCH | FF_DONTCARE, "Arial");
  if (!font_ready || state.glue_font_display_lists == 0U ||
      glue_font == nullptr) {
    if (glue_font != nullptr) {
      DeleteObject(glue_font);
    }
    return false;
  }
  const HGDIOBJ previous_glue = SelectObject(state.device_context, glue_font);
  const bool glue_font_ready =
      wglUseFontBitmapsA(state.device_context, 32, 96,
                         state.glue_font_display_lists) != FALSE;
  SelectObject(state.device_context, previous_glue);
  DeleteObject(glue_font);
  return glue_font_ready;
}

void shutdown_opengl(const HWND window, RecoveryWindowState &state) noexcept {
  if (state.rendering_context != nullptr) {
    if (state.font_display_lists != 0) {
      glDeleteLists(state.font_display_lists, 96);
      state.font_display_lists = 0;
    }
    if (state.glue_font_display_lists != 0) {
      glDeleteLists(state.glue_font_display_lists, 96);
      state.glue_font_display_lists = 0;
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(state.rendering_context);
    state.rendering_context = nullptr;
  }
  if (state.device_context != nullptr) {
    ReleaseDC(window, state.device_context);
    state.device_context = nullptr;
  }
}

} // namespace starcraft::recovery
