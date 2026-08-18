#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace starcraft::recovery {

namespace {

bool queue_sound_event(BootstrapStatus &status,
                       const PendingGameSound &event) noexcept {
  if (event.sound_id == 0U || event.sound_id == 0xFFFFU) {
    return false;
  }
  if (status.pending_game_sound.sound_id == 0xFFFFU) {
    status.pending_game_sound = event;
    return true;
  }
  if (status.pending_game_sound_count >=
      status.pending_game_sound_backlog.size()) {
    return false;
  }
  status.pending_game_sound_backlog[status.pending_game_sound_count++] = event;
  return true;
}

} // namespace

bool queue_game_sound(BootstrapStatus &status,
                      const std::uint16_t sound_id) noexcept {
  return queue_sound_event(status, PendingGameSound{sound_id, 0U, 0U, false});
}

bool queue_positional_game_sound(BootstrapStatus &status,
                                 const std::uint16_t sound_id,
                                 const std::uint16_t world_x,
                                 const std::uint16_t world_y) noexcept {
  // gamesnd.cpp::sub_455370 tests the local player bit in the map-mask word
  // before allocating a positional channel. Sounds in unrevealed/fogged map
  // space do not leak enemy activity.
  if (!fog_world_position_visible(status, world_x, world_y,
                                  status.local_player)) {
    return false;
  }
  return queue_sound_event(
      status, PendingGameSound{sound_id, world_x, world_y, true});
}

bool play_pending_game_sound(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr ||
      state.status->pending_game_sound.sound_id == 0xFFFFU) {
    return false;
  }
  const PendingGameSound event = state.status->pending_game_sound;
  const std::uint16_t sound_id = event.sound_id;
  if (state.status->pending_game_sound_count != 0U) {
    state.status->pending_game_sound =
        state.status->pending_game_sound_backlog.front();
    for (std::size_t index = 1U;
         index < state.status->pending_game_sound_count; ++index) {
      state.status->pending_game_sound_backlog[index - 1U] =
          state.status->pending_game_sound_backlog[index];
    }
    --state.status->pending_game_sound_count;
  } else {
    state.status->pending_game_sound = {};
  }
  const auto sound = std::find_if(state.status->archived_sounds.begin(),
                                  state.status->archived_sounds.end(),
                                  [sound_id](const ArchivedSoundAsset &asset) {
                                    return asset.sound_id == sound_id;
                                  });
  if (sound == state.status->archived_sounds.end()) {
    return false;
  }
  const std::size_t index =
      static_cast<std::size_t>(sound - state.status->archived_sounds.begin());
  if (index >= state.archived_sound_buffers.size()) {
    return false;
  }
  if (!play_digital_sound_buffer(state, state.archived_sound_buffers[index],
                                 event)) {
    return false;
  }
  state.status->last_game_sound = sound_id;
  if (sound_id < state.status->game_sound_play_counts.size()) {
    ++state.status->game_sound_play_counts[sound_id];
  }
  return true;
}

std::uint16_t choose_unit_sound(BootstrapStatus &status,
                                const std::uint16_t first,
                                const std::uint16_t last) noexcept {
  if (first == 0U || last < first) {
    return 0U;
  }
  if (first == last) {
    status.last_game_sound = first;
    return first;
  }
  const std::uint32_t span = static_cast<std::uint32_t>(last - first) + 1U;
  std::uint16_t chosen = static_cast<std::uint16_t>(
      first + (++status.sound_choice_counter % span));
  if (chosen == status.last_game_sound && ++chosen > last) {
    chosen = first;
  }
  status.last_game_sound = chosen;
  return chosen;
}

bool queue_unit_response(BootstrapStatus &status,
                         const ScenarioUnitPreview &unit,
                         const bool order_acknowledgement) noexcept {
  if (!unit.alive || unit.owner != status.local_player ||
      unit.unit_type >= status.unit_sound_ranges.size()) {
    return false;
  }
  const UnitSoundRanges &ranges = status.unit_sound_ranges[unit.unit_type];
  std::uint16_t first = ranges.what_first;
  std::uint16_t last = ranges.what_last;
  if (order_acknowledgement) {
    first = ranges.yes_first;
    last = ranges.yes_last;
  } else if (status.last_voice_unit_id == unit.unit_id) {
    status.voice_repeat_count = static_cast<std::uint8_t>((
        std::min)(255U, static_cast<unsigned>(status.voice_repeat_count) + 1U));
    // gamesnd.cpp::sub_455860 enters the sequential annoyed range after the
    // fourth repeated selection response, then wraps back to What sounds.
    if (status.voice_repeat_count >= 4U && ranges.annoyed_first != 0U &&
        ranges.annoyed_last >= ranges.annoyed_first) {
      first = ranges.annoyed_first;
      last = ranges.annoyed_last;
      const std::uint32_t span = static_cast<std::uint32_t>(last - first) + 1U;
      first = last = static_cast<std::uint16_t>(
          first + (status.voice_repeat_count - 4U) % span);
    }
  } else {
    status.last_voice_unit_id = unit.unit_id;
    status.voice_repeat_count = 0U;
  }
  const std::uint16_t chosen = choose_unit_sound(status, first, last);
  if (chosen == 0U) {
    return false;
  }
  return queue_game_sound(status, chosen);
}

bool queue_unit_ready_sound(BootstrapStatus &status,
                            const ScenarioUnitPreview &unit) noexcept {
  // gamesnd.cpp::sub_455790 only announces completed local units in the
  // 106-entry Ready-sound table. Unit IDs 73 and 85 are explicitly excluded
  // by the original handler.
  if (!unit.alive || unit.owner != status.local_player ||
      unit.unit_type >= 106U ||
      unit.unit_type == 73U || unit.unit_type == 85U) {
    return false;
  }
  return queue_game_sound(status,
                          status.unit_sound_ranges[unit.unit_type].ready);
}

bool extract_unit_sound_ranges(const starcraft::data::CoreDataSet &data,
                               BootstrapStatus &status) noexcept {
  // units.dat descriptor fields 26..32 are the exact arrays used by
  // gamesnd.cpp::sub_455790, sub_455860, and sub_455640. The 106-entry
  // arrays apply only to unit IDs 0..105; What sounds cover all 228 types.
  const starcraft::data::DatField *const ready = data.units().field(26);
  const starcraft::data::DatField *const what_first = data.units().field(27);
  const starcraft::data::DatField *const what_last = data.units().field(28);
  const starcraft::data::DatField *const annoyed_first = data.units().field(29);
  const starcraft::data::DatField *const annoyed_last = data.units().field(30);
  const starcraft::data::DatField *const yes_first = data.units().field(31);
  const starcraft::data::DatField *const yes_last = data.units().field(32);
  if (ready == nullptr || what_first == nullptr || what_last == nullptr ||
      annoyed_first == nullptr || annoyed_last == nullptr ||
      yes_first == nullptr || yes_last == nullptr) {
    return false;
  }
  for (std::size_t type = 0; type < status.unit_sound_ranges.size(); ++type) {
    UnitSoundRanges &ranges = status.unit_sound_ranges[type];
    if (!what_first->value(type, ranges.what_first) ||
        !what_last->value(type, ranges.what_last)) {
      return false;
    }
    if (type < 106U && (!ready->value(type, ranges.ready) ||
                        !annoyed_first->value(type, ranges.annoyed_first) ||
                        !annoyed_last->value(type, ranges.annoyed_last) ||
                        !yes_first->value(type, ranges.yes_first) ||
                        !yes_last->value(type, ranges.yes_last))) {
      return false;
    }
  }
  return true;
}

bool cache_unit_sound_assets(
    starcraft::runtime::StormModule &storm,
    const std::vector<std::uint8_t> &sfx_data,
    const std::vector<std::uint8_t> &sfx_table,
    const std::array<bool, starcraft::lang::kUnitTypeCount> &wanted_types,
    BootstrapStatus &status) noexcept {
  constexpr std::size_t sound_count = 944U;
  if (sfx_data.size() != 8496U) {
    return false;
  }
  const starcraft::data::StringTableView paths{sfx_table.data(),
                                               sfx_table.size()};
  if (!paths.valid()) {
    return false;
  }
  const auto cache_sound = [&](const std::uint16_t sound_id) -> bool {
    if (sound_id == 0U) {
      return true;
    }
    if (sound_id >= sound_count) {
      return false;
    }
    if (std::any_of(status.archived_sounds.begin(),
                    status.archived_sounds.end(),
                    [sound_id](const ArchivedSoundAsset &asset) {
                      return asset.sound_id == sound_id;
                    })) {
      return true;
    }
    const std::uint32_t path_id = read_u32(sfx_data, sound_id * 4U);
    if (path_id == 0U || path_id > UINT16_MAX) {
      return false;
    }
    const std::string_view relative =
        paths.one_based(static_cast<std::uint16_t>(path_id));
    if (relative.empty()) {
      return false;
    }
    ArchivedSoundAsset asset{};
    asset.sound_id = sound_id;
    asset.path = R"(sound\)";
    asset.path.append(relative.data(), relative.size());
    if (!storm.load_file(asset.path.c_str(), asset.wave) ||
        asset.wave.empty()) {
      return false;
    }
    status.archived_sounds.push_back(std::move(asset));
    return true;
  };
  const auto cache_range = [&cache_sound,
                            sound_count](const std::uint16_t first,
                                         const std::uint16_t last) -> bool {
    if (first == 0U && last == 0U) {
      return true;
    }
    if (first == 0U || last < first || last >= sound_count) {
      return false;
    }
    for (std::uint32_t sound = first; sound <= last; ++sound) {
      if (!cache_sound(static_cast<std::uint16_t>(sound))) {
        return false;
      }
    }
    return true;
  };
  try {
    for (std::size_t type = 0; type < wanted_types.size(); ++type) {
      if (!wanted_types[type]) {
        continue;
      }
      const UnitSoundRanges &ranges = status.unit_sound_ranges[type];
      // Some unit records intentionally reference expansion/retail variants
      // absent from the currently mounted archive set. Preserve every sound
      // that is present without making one optional response invalidate the
      // complete renderer/audio bootstrap.
      (void)cache_sound(ranges.ready);
      (void)cache_range(ranges.what_first, ranges.what_last);
      (void)cache_range(ranges.annoyed_first, ranges.annoyed_last);
      (void)cache_range(ranges.yes_first, ranges.yes_last);
    }
    // CImage.cpp::sub_415210 opcodes 0x1B/0x1C/0x1D/0x1F feed the same
    // eight-slot digital mixer as unit voices. Walk every reachable action of
    // each cached image while Storm is open and preserve every sound ID (and
    // complete numeric range) that its IScript can emit. This includes the
    // Marine's licensed rifle sound without encoding a guessed unit/sound map.
    const starcraft::lang::IScriptProgramView program{
        status.iscript_bytes.data(), status.iscript_bytes.size()};
    constexpr std::array<std::uint32_t, 14> random_values{{
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 63U, 127U, 191U, 255U}};
    starcraft::lang::IScriptState parent{};
    parent.active = true;
    for (const UnitRenderAsset &asset : status.unit_assets) {
      for (std::uint8_t action = 0U; action < 28U; ++action) {
        for (const std::uint32_t random_value : random_values) {
          starcraft::lang::IScriptState script{};
          if (!program.start(asset.iscript_id, action, script)) {
            break;
          }
          for (std::size_t tick = 0U; tick < 256U; ++tick) {
            const std::uint32_t sound_events = script.sound_event_count;
            const auto result = program.tick(script, random_value, 256U,
                                             &parent, 0U);
            if (script.sound_event_count != sound_events) {
              (void)cache_range(script.sound_range_first,
                                script.sound_range_last);
            }
            if (result == starcraft::lang::IScriptTickResult::ended ||
                result ==
                    starcraft::lang::IScriptTickResult::unsupported_opcode ||
                result ==
                    starcraft::lang::IScriptTickResult::malformed_program ||
                result ==
                    starcraft::lang::IScriptTickResult::instruction_limit) {
              break;
            }
          }
        }
      }
    }
    // The SCV's explicit weapon-8 event creates the cutter projectile. Its
    // image-498 IScript plays one of SFX 23..27 (EDrRep00..04), the exact
    // working sound range used for harvesting and repair.
    // CUnitPBuild.cpp::sub_43BBF0/sub_43BDF0 explicitly play 245 when the
    // Probe materializes the footprint and 246 at the final warp handoff.
    // Probe weapon 42 creates image 493; ephFire's action-zero script plays
    // SFX 587 when the mineral beam reaches its impact frame.
    return cache_range(23U, 27U) && cache_sound(587U) &&
           cache_range(245U, 246U) &&
           !status.archived_sounds.empty();
  } catch (...) {
    status.archived_sounds.clear();
    return false;
  }
}

} // namespace starcraft::recovery
