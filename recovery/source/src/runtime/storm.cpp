#include "starcraft/runtime/storm.hpp"

#include "starcraft/runtime/dependency_abi.hpp"

#include <windows.h>

namespace starcraft::runtime {

StormModule::StormModule(const std::filesystem::path& path) noexcept
    : module_(LoadLibraryExW(path.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH)) {}

StormModule::~StormModule() {
  if (module_ != nullptr) {
    FreeLibrary(static_cast<HMODULE>(module_));
  }
}

bool StormModule::loaded() const noexcept {
  return module_ != nullptr;
}

void* StormModule::resolve(const std::uint16_t ordinal) const noexcept {
  if (module_ == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<void*>(
      GetProcAddress(static_cast<HMODULE>(module_), MAKEINTRESOURCEA(ordinal)));
}

void* StormModule::allocate(
    const std::size_t bytes,
    const char* const source_file,
    const std::uint32_t source_line,
    const std::uint32_t flags) const noexcept {
  using Function = void*(WINAPI*)(std::uint32_t, const char*, std::uint32_t, std::uint32_t);
  const auto function = reinterpret_cast<Function>(resolve(kStormMemoryAllocate));
  if (function == nullptr || bytes > UINT32_MAX) {
    return nullptr;
  }
  return function(static_cast<std::uint32_t>(bytes), source_file, source_line, flags);
}

bool StormModule::free(
    void* const allocation,
    const char* const source_file,
    const std::uint32_t source_line,
    const std::uint32_t flags) const noexcept {
  using Function = int(WINAPI*)(void*, const char*, std::uint32_t, std::uint32_t);
  const auto function = reinterpret_cast<Function>(resolve(kStormMemoryFree));
  return function != nullptr && function(allocation, source_file, source_line, flags) != 0;
}

bool StormModule::open_archive(
    const std::filesystem::path& path,
    void** const archive,
    const std::uint32_t priority,
    const std::uint32_t flags) const noexcept {
  using Function = int(WINAPI*)(const char*, std::uint32_t, std::uint32_t, void**);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileOpenArchive));
  if (function == nullptr || archive == nullptr) {
    return false;
  }
  *archive = nullptr;
  try {
    const std::string narrow_path = path.string();
    return function(narrow_path.c_str(), priority, flags, archive) != 0;
  } catch (...) {
    return false;
  }
}

bool StormModule::close_archive(void* const archive) const noexcept {
  using Function = int(WINAPI*)(void*);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileCloseArchive));
  return function != nullptr && function(archive) != 0;
}

bool StormModule::open_file(
    void* const archive,
    const char* const archived_path,
    void** const file,
    const std::uint32_t search_scope) const noexcept {
  using Function = int(WINAPI*)(void*, const char*, std::uint32_t, void**);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileOpenFileEx));
  if (function == nullptr || archived_path == nullptr || file == nullptr) {
    return false;
  }
  *file = nullptr;
  return function(archive, archived_path, search_scope, file) != 0;
}

bool StormModule::open_file_global(
    const char* const archived_path,
    void** const file) const noexcept {
  using Function = int(WINAPI*)(const char*, void**);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileOpenFile));
  if (function == nullptr || archived_path == nullptr || file == nullptr) {
    return false;
  }
  *file = nullptr;
  return function(archived_path, file) != 0;
}

std::uint32_t StormModule::file_size(void* const file) const noexcept {
  using Function = std::uint32_t(WINAPI*)(void*, std::uint32_t*);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileGetSize));
  return function == nullptr ? UINT32_MAX : function(file, nullptr);
}

bool StormModule::read_file(
    void* const file,
    void* const destination,
    const std::uint32_t bytes_to_read,
    std::uint32_t* const bytes_read) const noexcept {
  using Function = int(WINAPI*)(void*, void*, std::uint32_t, std::uint32_t*, void*);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileRead));
  return function != nullptr &&
         function(file, destination, bytes_to_read, bytes_read, nullptr) != 0;
}

bool StormModule::close_file(void* const file) const noexcept {
  using Function = int(WINAPI*)(void*);
  const auto function = reinterpret_cast<Function>(resolve(kStormFileCloseFile));
  return function != nullptr && function(file) != 0;
}

bool StormModule::load_file(
    const char* const archived_path,
    std::vector<std::uint8_t>& bytes) const noexcept {
  void* file{};
  if (!open_file_global(archived_path, &file)) {
    return false;
  }

  bool success{};
  try {
    const std::uint32_t size = file_size(file);
    if (size != UINT32_MAX && size <= 64U * 1024U * 1024U) {
      bytes.resize(size);
      std::uint32_t bytes_read{};
      success = size == 0 ||
                (read_file(file, bytes.data(), size, &bytes_read) && bytes_read == size);
      if (!success) {
        bytes.clear();
      }
    }
  } catch (...) {
    bytes.clear();
    success = false;
  }
  const bool closed = close_file(file);
  return success && closed;
}

bool StormModule::load_file_from_archive(
    void* const archive,
    const char* const archived_path,
    std::vector<std::uint8_t>& bytes,
    const std::uint32_t search_scope) const noexcept {
  void* file{};
  if (archive == nullptr || !open_file(archive, archived_path, &file, search_scope)) {
    return false;
  }

  bool success{};
  try {
    const std::uint32_t size = file_size(file);
    if (size != UINT32_MAX && size <= 64U * 1024U * 1024U) {
      bytes.resize(size);
      std::uint32_t bytes_read{};
      success = size == 0 ||
                (read_file(file, bytes.data(), size, &bytes_read) && bytes_read == size);
      if (!success) {
        bytes.clear();
      }
    }
  } catch (...) {
    bytes.clear();
    success = false;
  }
  const bool closed = close_file(file);
  return success && closed;
}

bool StormModule::load_pcx(
    const char* const archived_path,
    DecodedPcx& image) const noexcept {
  // Recovered from lang\gamedata.cpp::sub_453630 (0x00453630) and its
  // callers. A null destination queries dimensions; a second call decodes
  // 8-bit pixels and 256 four-byte palette entries.
  using Function = int(WINAPI*)(
      const char*, void*, void*, std::uint32_t, std::uint32_t*, std::uint32_t*, std::uint32_t);
  const auto function = reinterpret_cast<Function>(resolve(kStormPcxDecode));
  image = {};
  if (function == nullptr || archived_path == nullptr) {
    return false;
  }
  std::uint32_t width{};
  std::uint32_t height{};
  if (function(archived_path, nullptr, nullptr, 0, &width, &height, 0) == 0 ||
      width == 0 || height == 0 || width > 4096 || height > 4096 ||
      width > UINT32_MAX / height) {
    return false;
  }
  try {
    DecodedPcx decoded{};
    decoded.width = width;
    decoded.height = height;
    decoded.pixels.resize(static_cast<std::size_t>(width) * height);
    decoded.palette.resize(256U * 4U);
    if (function(
            archived_path,
            decoded.palette.data(),
            decoded.pixels.data(),
            static_cast<std::uint32_t>(decoded.pixels.size()),
            nullptr,
            nullptr,
            0) == 0) {
      return false;
    }
    image = std::move(decoded);
    return true;
  } catch (...) {
    return false;
  }
}

}  // namespace starcraft::runtime
