#pragma once

#include "formats/isom_section.hpp"
#include "formats/chk_document.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace staredit::undo {

struct TileChange {
  std::size_t tile_index{};
  std::uint16_t before{};
  std::uint16_t after{};
};

struct TileEditCommand {
  std::vector<TileChange> changes{};
  std::vector<TileChange> editor_changes{};
  struct IsomChange {
    std::size_t entry_index{};
    formats::IsomEntry before{};
    formats::IsomEntry after{};
  };
  std::vector<IsomChange> isom_changes{};
  struct SectionChange {
    std::uint32_t tag{};
    std::size_t occurrence{};
    bool before_exists{};
    bool after_exists{};
    std::vector<std::uint8_t> before{};
    std::vector<std::uint8_t> after{};
  };
  std::vector<SectionChange> section_changes{};
};

class UndoStack final {
 public:
  [[nodiscard]] bool initialize(std::size_t maximum_commands) noexcept;
  void clear() noexcept;

  [[nodiscard]] bool push(TileEditCommand&& command) noexcept;
  [[nodiscard]] bool undo(std::vector<std::uint16_t>& tiles) noexcept;
  [[nodiscard]] bool redo(std::vector<std::uint16_t>& tiles) noexcept;
  [[nodiscard]] bool undo(std::vector<std::uint16_t>& game_tiles,
                          std::vector<std::uint16_t>& editor_tiles,
                          formats::IsomSection& isom) noexcept;
  [[nodiscard]] bool redo(std::vector<std::uint16_t>& game_tiles,
                          std::vector<std::uint16_t>& editor_tiles,
                          formats::IsomSection& isom) noexcept;
  [[nodiscard]] bool undo(std::vector<std::uint16_t>& game_tiles,
                          std::vector<std::uint16_t>& editor_tiles,
                          formats::IsomSection& isom,
                          formats::ChkDocument& chk) noexcept;
  [[nodiscard]] bool redo(std::vector<std::uint16_t>& game_tiles,
                          std::vector<std::uint16_t>& editor_tiles,
                          formats::IsomSection& isom,
                          formats::ChkDocument& chk) noexcept;

  [[nodiscard]] bool can_undo() const noexcept;
  [[nodiscard]] bool can_redo() const noexcept;
  [[nodiscard]] bool at_origin() const noexcept;
  void mark_origin() noexcept;
  [[nodiscard]] std::size_t undo_count() const noexcept;
  [[nodiscard]] std::size_t redo_count() const noexcept;

 private:
  [[nodiscard]] static bool indices_valid(
      const TileEditCommand& command,
      std::size_t tile_count) noexcept;
  [[nodiscard]] static bool indices_valid(
      const TileEditCommand& command,
      std::size_t game_tile_count,
      std::size_t editor_tile_count,
      std::size_t isom_entry_count) noexcept;
  static void apply_isom_change(formats::IsomSection& isom,
                                const TileEditCommand::IsomChange& change,
                                bool after) noexcept;
  [[nodiscard]] static bool apply_section_changes(
      const TileEditCommand& command,
      formats::ChkDocument& chk,
      bool after) noexcept;

  std::vector<TileEditCommand> history_{};
  std::size_t cursor_{};
  std::size_t origin_cursor_{};
  std::size_t maximum_commands_{};
  bool origin_reachable_{true};
  bool initialized_{};
};

}  // namespace staredit::undo
