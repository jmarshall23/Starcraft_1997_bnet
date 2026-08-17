#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace starcraft::recovery {

bool parse_pcm_wave(const std::vector<std::uint8_t> &bytes,
                    PcmWaveView &wave) noexcept {
  // The archived advisor assets are ordinary RIFF PCM. Walk the chunks rather
  // than assuming a fixed 44-byte header so LIST/JUNK extensions remain valid.
  if (bytes.size() < 12U || read_u32(bytes, 0) != 0x46464952U ||
      read_u32(bytes, 8) != 0x45564157U) {
    return false;
  }
  std::uint16_t encoding{};
  std::uint16_t channels{};
  std::uint16_t bits{};
  std::uint32_t sample_rate{};
  const std::uint8_t *samples{};
  std::size_t sample_bytes{};
  for (std::size_t chunk = 12U; chunk + 8U <= bytes.size();) {
    const std::uint32_t id = read_u32(bytes, chunk);
    const std::size_t size = read_u32(bytes, chunk + 4U);
    const std::size_t payload = chunk + 8U;
    if (size > bytes.size() - payload) {
      return false;
    }
    if (id == 0x20746D66U && size >= 16U) { // "fmt "
      encoding = read_u16(bytes, payload);
      channels = read_u16(bytes, payload + 2U);
      sample_rate = read_u32(bytes, payload + 4U);
      bits = read_u16(bytes, payload + 14U);
    } else if (id == 0x61746164U) { // "data"
      samples = bytes.data() + payload;
      sample_bytes = size;
    }
    const std::size_t padded = size + (size & 1U);
    if (padded > bytes.size() - payload) {
      return false;
    }
    chunk = payload + padded;
  }
  if (encoding != 1U || samples == nullptr || sample_bytes == 0U ||
      sample_bytes > static_cast<std::size_t>(INT_MAX) || sample_rate == 0U ||
      sample_rate > static_cast<std::uint32_t>(INT_MAX)) {
    return false;
  }
  if (channels == 1U && bits == 8U) {
    wave.format = AL_FORMAT_MONO8;
  } else if (channels == 1U && bits == 16U) {
    wave.format = AL_FORMAT_MONO16;
  } else if (channels == 2U && bits == 8U) {
    wave.format = AL_FORMAT_STEREO8;
  } else if (channels == 2U && bits == 16U) {
    wave.format = AL_FORMAT_STEREO16;
  } else {
    return false;
  }
  wave.samples = samples;
  wave.sample_bytes = static_cast<ALsizei>(sample_bytes);
  wave.sample_rate = static_cast<ALsizei>(sample_rate);
  return true;
}

void shutdown_audio(RecoveryWindowState &state) noexcept {
  if (state.audio_context != nullptr) {
    (void)alcMakeContextCurrent(state.audio_context);
    if (state.music_source != 0U) {
      alSourceStop(state.music_source);
      alDeleteSources(1, &state.music_source);
      state.music_source = 0U;
    }
    for (const ALuint source : state.audio_sources) {
      if (source != 0U) {
        alSourceStop(source);
      }
    }
    if (std::any_of(state.audio_sources.begin(), state.audio_sources.end(),
                    [](const ALuint source) { return source != 0U; })) {
      alDeleteSources(static_cast<ALsizei>(state.audio_sources.size()),
                      state.audio_sources.data());
      state.audio_sources.fill(0U);
    }
    if (state.resource_error_buffers[0] != 0U ||
        state.resource_error_buffers[1] != 0U) {
      alDeleteBuffers(static_cast<ALsizei>(state.resource_error_buffers.size()),
                      state.resource_error_buffers.data());
      state.resource_error_buffers.fill(0U);
    }
    if (!state.archived_sound_buffers.empty()) {
      alDeleteBuffers(static_cast<ALsizei>(state.archived_sound_buffers.size()),
                      state.archived_sound_buffers.data());
      state.archived_sound_buffers.clear();
    }
    if (state.music_buffer != 0U) {
      alDeleteBuffers(1, &state.music_buffer);
      state.music_buffer = 0U;
    }
    (void)alcMakeContextCurrent(nullptr);
    alcDestroyContext(state.audio_context);
    state.audio_context = nullptr;
  }
  if (state.audio_device != nullptr) {
    (void)alcCloseDevice(state.audio_device);
    state.audio_device = nullptr;
  }
  state.audio_ready = false;
  state.music_playing = false;
  state.audio_source_cursor = 0U;
}

bool initialize_audio(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr) {
    return false;
  }
  std::array<PcmWaveView, 2> waves{};
  for (std::size_t index = 0; index < waves.size(); ++index) {
    if (!parse_pcm_wave(state.status->resource_error_waves[index],
                        waves[index])) {
      return false;
    }
  }
  std::vector<PcmWaveView> archived_waves(state.status->archived_sounds.size());
  for (std::size_t index = 0; index < archived_waves.size(); ++index) {
    if (!parse_pcm_wave(state.status->archived_sounds[index].wave,
                        archived_waves[index])) {
      return false;
    }
  }
  PcmWaveView music_wave{};
  if (!parse_pcm_wave(state.status->music_wave, music_wave)) {
    return false;
  }
  state.audio_device = alcOpenDevice(nullptr);
  if (state.audio_device == nullptr) {
    return false;
  }
  state.audio_context = alcCreateContext(state.audio_device, nullptr);
  if (state.audio_context == nullptr ||
      !alcMakeContextCurrent(state.audio_context)) {
    shutdown_audio(state);
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }
  alGenBuffers(static_cast<ALsizei>(state.resource_error_buffers.size()),
               state.resource_error_buffers.data());
  for (std::size_t index = 0; index < waves.size(); ++index) {
    alBufferData(state.resource_error_buffers[index], waves[index].format,
                 waves[index].samples, waves[index].sample_bytes,
                 waves[index].sample_rate);
  }
  state.archived_sound_buffers.resize(archived_waves.size());
  if (!state.archived_sound_buffers.empty()) {
    alGenBuffers(static_cast<ALsizei>(state.archived_sound_buffers.size()),
                 state.archived_sound_buffers.data());
  }
  for (std::size_t index = 0; index < archived_waves.size(); ++index) {
    alBufferData(state.archived_sound_buffers[index],
                 archived_waves[index].format, archived_waves[index].samples,
                 archived_waves[index].sample_bytes,
                 archived_waves[index].sample_rate);
  }
  alGenBuffers(1, &state.music_buffer);
  alBufferData(state.music_buffer, music_wave.format, music_wave.samples,
               music_wave.sample_bytes, music_wave.sample_rate);
  // The requested recovery runtime expands the original digital mixer to 128
  // simultaneous voices. Each source remains independently bound so a later
  // attack/explosion cannot truncate an earlier clip.
  alGenSources(static_cast<ALsizei>(state.audio_sources.size()),
               state.audio_sources.data());
  for (const ALuint source : state.audio_sources) {
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.0F);
    alSourcef(source, AL_GAIN, 1.0F);
  }
  alGenSources(1, &state.music_source);
  alSourcei(state.music_source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcef(state.music_source, AL_ROLLOFF_FACTOR, 0.0F);
  alSourcef(state.music_source, AL_GAIN, 0.45F);
  alSourcei(state.music_source, AL_LOOPING, AL_TRUE);
  alSourcei(state.music_source, AL_BUFFER,
            static_cast<ALint>(state.music_buffer));
  state.audio_ready =
                      alGetError() == AL_NO_ERROR &&
                      std::all_of(state.audio_sources.begin(),
                                  state.audio_sources.end(),
                                  [](const ALuint source) {
                                    return source != 0U;
                                  }) &&
                      state.resource_error_buffers[0] != 0U &&
                      state.resource_error_buffers[1] != 0U &&
                      !state.archived_sound_buffers.empty() &&
                      state.music_source != 0U && state.music_buffer != 0U;
  if (!state.audio_ready) {
    shutdown_audio(state);
  } else {
    alSourcePlay(state.music_source);
    ALint music_state{};
    alGetSourcei(state.music_source, AL_SOURCE_STATE, &music_state);
    state.music_playing =
        alGetError() == AL_NO_ERROR && music_state == AL_PLAYING;
  }
  return state.audio_ready;
}

bool play_digital_sound_buffer(RecoveryWindowState &state,
                               const ALuint buffer,
                               const PendingGameSound &event) noexcept {
  if (!state.audio_ready || buffer == 0U || state.audio_context == nullptr ||
      !alcMakeContextCurrent(state.audio_context)) {
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }

  float gain = 1.0F;
  if (event.positional && state.status != nullptr) {
    const int left = state.status->camera_x;
    const int top = state.status->camera_y;
    const int right = left + kMapViewportWidth;
    const int bottom = top + kMapViewportHeight;
    int outside{};
    if (event.world_x < left) {
      outside += left - event.world_x;
    } else if (event.world_x > right) {
      outside += event.world_x - right;
    }
    if (event.world_y < top) {
      outside += top - event.world_y;
    } else if (event.world_y > bottom) {
      outside += event.world_y - bottom;
    }
    // gamesnd.cpp::sub_4551F0 uses 99-(99*distance>>9) and rejects a
    // resulting volume of ten or less. This prevents every remote map event
    // from entering the mixer while retaining the original 512-pixel falloff.
    const int recovered_volume = (std::max)(0, 99 - ((99 * outside) >> 9));
    if (recovered_volume <= 10) {
      return false;
    }
    gain = static_cast<float>(recovered_volume) / 99.0F;
    alListener3f(AL_POSITION,
                 static_cast<ALfloat>(left + kMapViewportWidth / 2),
                 -static_cast<ALfloat>(top + kMapViewportHeight / 2), 0.0F);
  }

  std::size_t selected = state.audio_sources.size();
  for (std::size_t offset = 0; offset < state.audio_sources.size(); ++offset) {
    const std::size_t index =
        (static_cast<std::size_t>(state.audio_source_cursor) + offset) %
        state.audio_sources.size();
    ALint source_state{};
    alGetSourcei(state.audio_sources[index], AL_SOURCE_STATE, &source_state);
    if (source_state == AL_INITIAL || source_state == AL_STOPPED) {
      selected = index;
      break;
    }
  }
  if (selected == state.audio_sources.size()) {
    // This recovery target intentionally exposes 128 channels. Once all are
    // occupied, rotate fairly until the recovered priority metadata is wired
    // into the allocator.
    selected = state.audio_source_cursor % state.audio_sources.size();
    alSourceStop(state.audio_sources[selected]);
  }
  const ALuint source = state.audio_sources[selected];
  alSourcei(source, AL_SOURCE_RELATIVE, event.positional ? AL_FALSE : AL_TRUE);
  alSourcef(source, AL_ROLLOFF_FACTOR, 0.0F);
  alSourcef(source, AL_GAIN, gain);
  if (event.positional) {
    alSource3f(source, AL_POSITION, static_cast<ALfloat>(event.world_x),
               -static_cast<ALfloat>(event.world_y), 0.0F);
  } else {
    alSource3f(source, AL_POSITION, 0.0F, 0.0F, 0.0F);
  }
  alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    return false;
  }
  state.audio_source_cursor = static_cast<std::uint16_t>(
      (selected + 1U) % state.audio_sources.size());
  ++state.audio_play_count;
  return true;
}

bool play_pending_resource_error(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr || state.status->pending_resource_error_sound >=
                                     state.resource_error_buffers.size()) {
    return false;
  }
  const std::size_t sound = state.status->pending_resource_error_sound;
  state.status->pending_resource_error_sound = 0xFFU;
  return play_digital_sound_buffer(state,
                                   state.resource_error_buffers[sound]);
}

} // namespace starcraft::recovery
