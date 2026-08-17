#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {

PresentationViewport presentation_viewport(const int client_width,
                                           const int client_height) noexcept {
  if (client_width <= 0 || client_height <= 0) {
    return {};
  }
  // The recovered renderer's complete logical surface is 640x400. Preserve
  // that ratio, consume the full client height, and center the resulting
  // presentation horizontally. A narrow client intentionally crops equal
  // amounts from both sides rather than distorting the game surface.
  const int width = static_cast<int>(
      (static_cast<long long>(client_height) * kMapViewportWidth +
       kMapViewportHeight / 2) /
      kMapViewportHeight);
  return {(client_width - width) / 2, 0, width, client_height};
}

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
  // CImage's cloak draw function modulates the image instead of replacing
  // the incoming fragment color. GL_REPLACE discarded the per-CUnit alpha,
  // making Wraiths, Ghosts, and the permanently cloaked Observer fully
  // opaque even though CImage.cpp selected the recovered cloak state.
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
  std::array<GLYPHMETRICSFLOAT, 96> font_metrics{};
  const bool font_ready =
      wglUseFontOutlinesA(state.device_context, 32, 96,
                          state.font_display_lists, 0.0F, 0.0F,
                          WGL_FONT_POLYGONS, font_metrics.data()) != FALSE;
  for (std::size_t index = 0; index < font_metrics.size(); ++index) {
    state.font_advances[index] = font_metrics[index].gmfCellIncX;
  }
  float font_height{};
  for (const GLYPHMETRICSFLOAT &metric : font_metrics) {
    font_height = (std::max)(font_height, metric.gmfBlackBoxY);
  }
  if (font_height > 0.0F) {
    state.font_outline_scale = 10.0F / font_height;
  }
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
  std::array<GLYPHMETRICSFLOAT, 96> glue_font_metrics{};
  const bool glue_font_ready =
      wglUseFontOutlinesA(state.device_context, 32, 96,
                          state.glue_font_display_lists, 0.0F, 0.0F,
                          WGL_FONT_POLYGONS,
                          glue_font_metrics.data()) != FALSE;
  for (std::size_t index = 0; index < glue_font_metrics.size(); ++index) {
    state.glue_font_advances[index] = glue_font_metrics[index].gmfCellIncX;
  }
  float glue_font_height{};
  for (const GLYPHMETRICSFLOAT &metric : glue_font_metrics) {
    glue_font_height = (std::max)(glue_font_height, metric.gmfBlackBoxY);
  }
  if (glue_font_height > 0.0F) {
    state.glue_font_outline_scale = 20.0F / glue_font_height;
  }
  SelectObject(state.device_context, previous_glue);
  DeleteObject(glue_font);
  return glue_font_ready && initialize_debug_console(window, state);
}

void shutdown_opengl(const HWND window, RecoveryWindowState &state) noexcept {
  if (state.rendering_context != nullptr) {
    shutdown_debug_console(state);
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
