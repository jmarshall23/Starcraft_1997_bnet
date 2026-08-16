#include "../platform/bootstrap_runtime.hpp"

namespace starcraft::recovery {

void draw_title_gl(const RecoveryWindowState &state) noexcept {
  // title.cpp::sub_4B3710 loads glue\title\title-beta.pcx into the complete
  // 640x480 glue surface. The original keeps it visible while initialization
  // runs and then asks the title task to close when gluMain is ready.
  draw_preview_frame_gl(state.glue.title_background, 0.0F, 0.0F,
                        static_cast<float>(kMapViewportWidth),
                        static_cast<float>(kMapViewportHeight));
}

} // namespace starcraft::recovery
