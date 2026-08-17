#include "../platform/bootstrap_runtime.hpp"

namespace starcraft::recovery {

void draw_title_gl(const RecoveryWindowState &state) noexcept {
  // title.cpp::sub_4B3710 loads glue\title\title-beta.pcx into the complete
  // 640x480 glue surface. The original keeps it visible while initialization
  // runs and then asks the title task to close when gluMain is ready.
  draw_preview_frame_gl(state.glue.title_background, 0.0F, 0.0F,
                         static_cast<float>(kMapViewportWidth),
                         static_cast<float>(kMapViewportHeight));
  // 0x004B36CD passes rez\titledlg.bin to DLG::sub_4D9C40. Its -10 control
  // is (220,400)-(419,419), rather than the entire title surface; the other
  // type-10 controls are the retail copyright lines at the bottom.
  for (const GlueControl &control : state.glue.title_controls) {
    if (control.type != 10U || control.text.empty() ||
        (control.identifier == -10 &&
         !title_loading_visible(state.glue, state.glue.clock_tick))) {
      continue;
    }
    draw_glue_centered_styled_text_gl(
        state, control.text, control, glue_control_font_style(control),
        (control.flags & 0x4800U) != 0U);
  }
}

} // namespace starcraft::recovery
