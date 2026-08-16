#pragma once

#include <array>
#include <cstdint>

namespace starcraft::runtime {

inline constexpr std::array<std::uint16_t, 102> kRequiredStormOrdinals{{
    102, 105, 106, 107, 109, 113, 114, 115, 117, 118, 119, 120, 121, 122, 123,
    125, 127, 128, 130, 132, 133, 134, 135, 136, 138, 139, 206, 209, 211, 216,
    221, 222, 252, 253, 255, 256, 257, 258, 260, 261, 263, 265, 266, 267, 268,
    269, 271, 274, 276, 313, 314, 321, 323, 325, 331, 332, 334, 346, 350, 351,
    354, 356, 357, 383, 385, 386, 389, 390, 393, 401, 403, 421, 422, 423, 424,
    425, 426, 431, 432, 434, 436, 437, 438, 440, 442, 443, 445, 451, 452, 453,
    454, 457, 458, 462, 463, 465, 468, 501, 523, 524, 525, 529,
}};

inline constexpr std::array<std::uint16_t, 10> kRequiredVoxwareOrdinals{{
    100, 101, 200, 519, 1100, 1110, 1120, 2100, 2110, 2120,
}};

// Names proven directly by the matching storm.dll implementation.
inline constexpr std::uint16_t kStormMemoryAllocate = 401;  // SMemAlloc
inline constexpr std::uint16_t kStormMemoryFree = 403;      // SMemFree
inline constexpr std::uint16_t kStormStringCopy = 501;      // SStrCopy
inline constexpr std::uint16_t kStormFileCloseArchive = 252;  // SFileCloseArchive
inline constexpr std::uint16_t kStormFileCloseFile = 253;     // SFileCloseFile
inline constexpr std::uint16_t kStormFileGetSize = 265;       // SFileGetFileSize
inline constexpr std::uint16_t kStormFileOpenArchive = 266;   // SFileOpenArchive
inline constexpr std::uint16_t kStormFileOpenFile = 267;      // SFileOpenFile
inline constexpr std::uint16_t kStormFileOpenFileEx = 268;    // SFileOpenFileEx
inline constexpr std::uint16_t kStormFileRead = 269;          // SFileReadFile
// Called by lang\gamedata.cpp::sub_453630 at 0x00453630.
inline constexpr std::uint16_t kStormPcxDecode = 323;

}  // namespace starcraft::runtime
