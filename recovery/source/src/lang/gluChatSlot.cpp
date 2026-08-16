#include "../platform/bootstrap_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace starcraft::recovery {
namespace {

constexpr std::int16_t kSlotNameBase = 100;
constexpr std::int16_t kSlotRaceBase = 200;

const GlueControl *control_with_id(const std::vector<GlueControl> &controls,
                                   const std::int16_t identifier) noexcept {
  for (const GlueControl &control : controls) {
    if (control.identifier == identifier) {
      return &control;
    }
  }
  return nullptr;
}

bool point_in_control(const GlueControl &control, const int x,
                      const int y) noexcept {
  return x >= control.left && x <= control.right && y >= control.top &&
         y <= control.bottom;
}

std::string_view race_name(const std::uint8_t race) noexcept {
  constexpr std::array<std::string_view, 3> names{{"Zerg", "Terran",
                                                   "Protoss"}};
  return race < names.size() ? names[race] : std::string_view{""};
}

} // namespace

std::int16_t lobby_control_at(const GlueRuntime &glue, const int x,
                              const int y) noexcept {
  for (const std::int16_t identifier :
       {std::int16_t{6}, std::int16_t{7}, std::int16_t{15}}) {
    const GlueControl *const control =
        control_with_id(glue.lobby_controls, identifier);
    if (control != nullptr && point_in_control(*control, x, y)) {
      return identifier;
    }
  }
  for (std::size_t row = 0; row < glue.lobby_slots.size(); ++row) {
    const std::int16_t first = static_cast<std::int16_t>(25 + row * 4U);
    const GlueControl *const name =
        control_with_id(glue.lobby_controls,
                        static_cast<std::int16_t>(first + 1));
    const GlueControl *const race =
        control_with_id(glue.lobby_controls,
                        static_cast<std::int16_t>(first + 2));
    if (name != nullptr && point_in_control(*name, x, y)) {
      return static_cast<std::int16_t>(kSlotNameBase + row);
    }
    if (race != nullptr && point_in_control(*race, x, y)) {
      return static_cast<std::int16_t>(kSlotRaceBase + row);
    }
  }
  return -1;
}

void draw_lobby_slots_gl(const RecoveryWindowState &state) noexcept {
  for (std::size_t row = 0; row < state.glue.lobby_slots.size(); ++row) {
    const GlueLobbySlot &slot = state.glue.lobby_slots[row];
    const std::int16_t first = static_cast<std::int16_t>(25 + row * 4U);
    const GlueControl *const number =
        control_with_id(state.glue.lobby_controls, first);
    const GlueControl *const name = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 1));
    const GlueControl *const race = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 2));
    const GlueControl *const status = control_with_id(
        state.glue.lobby_controls, static_cast<std::int16_t>(first + 3));
    if (number == nullptr || name == nullptr || race == nullptr ||
        status == nullptr) {
      continue;
    }
    const bool active = slot.ownership != 0U;
    const std::int16_t name_id =
        static_cast<std::int16_t>(kSlotNameBase + row);
    const std::int16_t race_id =
        static_cast<std::int16_t>(kSlotRaceBase + row);
    const bool selected = state.glue.hovered_control == name_id ||
                          state.glue.hovered_control == race_id;
    const std::uint8_t red = selected ? 255U : active ? 215U : 120U;
    const std::uint8_t green = selected ? 224U : active ? 215U : 120U;
    const std::uint8_t blue = selected ? 96U : active ? 215U : 120U;
    draw_glue_text_gl(state, std::to_string(row + 1U),
                      static_cast<float>(number->left),
                      static_cast<float>(number->bottom - 2), red, green, blue,
                      false);
    draw_glue_text_gl(state, active ? slot.name : "Closed",
                      static_cast<float>(name->left),
                      static_cast<float>(name->bottom - 2), red, green, blue,
                      false);
    draw_glue_text_gl(state, active ? race_name(slot.race) : "---",
                      static_cast<float>(race->left),
                      static_cast<float>(race->bottom - 2), red, green, blue,
                      false);
    draw_glue_text_gl(state,
                      active ? slot.local ? "OK" : "CPU" : "",
                      static_cast<float>(status->left),
                      static_cast<float>(status->bottom - 2), red, green, blue,
                      false);
  }
}

} // namespace starcraft::recovery
