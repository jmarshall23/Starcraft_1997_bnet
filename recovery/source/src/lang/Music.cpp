#include "../platform/bootstrap_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

namespace starcraft::recovery {
namespace {

bool read_wave(const std::filesystem::path &path,
               std::vector<std::uint8_t> &bytes) {
  std::ifstream input{path, std::ios::binary};
  if (!input) {
    return false;
  }
  bytes.assign(std::istreambuf_iterator<char>{input},
               std::istreambuf_iterator<char>{});
  return !bytes.empty();
}

bool play_music_bytes(RecoveryWindowState &state,
                      const std::vector<std::uint8_t> &bytes,
                      const bool looping,
                      const std::string &display_path) noexcept {
  if (!state.audio_ready || state.audio_context == nullptr ||
      state.music_source == 0U || state.music_buffer == 0U || bytes.empty()) {
    return false;
  }
  PcmWaveView wave{};
  if (!parse_pcm_wave(bytes, wave) ||
      !alcMakeContextCurrent(state.audio_context)) {
    return false;
  }
  while (alGetError() != AL_NO_ERROR) {
  }
  alSourceStop(state.music_source);
  alSourcei(state.music_source, AL_BUFFER, 0);
  alBufferData(state.music_buffer, wave.format, wave.samples,
               wave.sample_bytes, wave.sample_rate);
  alSourcei(state.music_source, AL_BUFFER,
            static_cast<ALint>(state.music_buffer));
  alSourcei(state.music_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
  alSourcef(state.music_source, AL_GAIN,
            0.60F * static_cast<float>(state.game_dialog.music_volume) /
                100.0F);
  alSourcePlay(state.music_source);
  ALint source_state{};
  alGetSourcei(state.music_source, AL_SOURCE_STATE, &source_state);
  state.music_playing =
      alGetError() == AL_NO_ERROR && source_state == AL_PLAYING;
  if (state.music_playing) {
    state.active_music_path = display_path;
  }
  return state.music_playing;
}

bool play_loose_music(RecoveryWindowState &state,
                      const std::filesystem::path &relative_path,
                      const bool looping) noexcept {
  std::vector<std::uint8_t> bytes;
  const std::filesystem::path root = locate_input_root();
  if (root.empty() || !read_wave(root / relative_path, bytes)) {
    return false;
  }
  return play_music_bytes(state, bytes, looping, relative_path.string());
}

} // namespace

bool play_result_music(RecoveryWindowState &state,
                       const MatchOutcome outcome) noexcept {
  if (!state.audio_ready || state.status == nullptr ||
      state.status->local_race >= 3U || outcome == MatchOutcome::none ||
      state.audio_context == nullptr || state.music_source == 0U ||
      state.music_buffer == 0U) {
    return false;
  }
  // The sgSongs table at 0x004F8D60 maps indices 13..15 to the three defeat
  // WAVs and 16..18 to the victory WAVs.  Its loop byte is zero for all six.
  constexpr std::array<const wchar_t *, 3> defeat{{
      L"music\\zdefeat.wav", L"music\\tdefeat.wav",
      L"music\\pdefeat.wav",
  }};
  constexpr std::array<const wchar_t *, 3> victory{{
      L"music\\zvict.wav", L"music\\tvict.wav", L"music\\pvict.wav",
  }};
  const auto &paths =
      outcome == MatchOutcome::victory ? victory : defeat;
  return play_loose_music(state, paths[state.status->local_race], false);
}

bool play_title_music(RecoveryWindowState &state) noexcept {
  // sgSongs entry 22 is music\title.wav with its loop byte set.
  return play_loose_music(state, L"music\\title.wav", true);
}

bool play_gameplay_music(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr || state.status->music_path.empty() ||
      state.status->music_wave.empty()) {
    return false;
  }
  return play_music_bytes(state, state.status->music_wave, true,
                          state.status->music_path);
}

bool ensure_gameplay_music(RecoveryWindowState &state) noexcept {
  if (state.status == nullptr || !state.audio_ready ||
      state.active_music_path != state.status->music_path) {
    return play_gameplay_music(state);
  }
  ALint source_state{};
  alGetSourcei(state.music_source, AL_SOURCE_STATE, &source_state);
  if (alGetError() == AL_NO_ERROR && source_state == AL_PLAYING) {
    state.music_playing = true;
    return true;
  }
  return play_gameplay_music(state);
}

} // namespace starcraft::recovery
