#include "../platform/bootstrap_runtime.hpp"

#include <cstdint>
#include <string>

namespace starcraft::recovery {

void draw_ready_gl(const RecoveryWindowState &state,
                   const std::uint32_t now) noexcept {
  draw_lobby_gl(state);
  const std::int32_t remaining =
      static_cast<std::int32_t>(state.glue.ready_deadline - now);
  const std::uint32_t seconds =
      remaining <= 0 ? 0U : (static_cast<std::uint32_t>(remaining) + 999U) /
                                    1000U;
  glDisable(GL_TEXTURE_2D);
  glColor4ub(0U, 0U, 0U, 205U);
  glBegin(GL_QUADS);
  glVertex2f(380.0F, 267.0F * hud_vertical_scale());
  glVertex2f(628.0F, 267.0F * hud_vertical_scale());
  glVertex2f(628.0F, 302.0F * hud_vertical_scale());
  glVertex2f(380.0F, 302.0F * hud_vertical_scale());
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
  draw_glue_text_gl(state, "Starting in: " + std::to_string(seconds), 398.0F,
                    291.0F, 255U, 220U, 96U, true);
}

} // namespace starcraft::recovery
