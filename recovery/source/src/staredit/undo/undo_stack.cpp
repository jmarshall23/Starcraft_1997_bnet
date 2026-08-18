#include "undo/undo_stack.hpp"

#include <algorithm>
#include <utility>

namespace staredit::undo {

bool UndoStack::initialize(const std::size_t maximum_commands) noexcept {
  clear();
  maximum_commands_ = maximum_commands;
  initialized_ = false;
  if (maximum_commands == 0U) {
    return false;
  }
  try {
    history_.reserve(maximum_commands);
    initialized_ = true;
    return true;
  } catch (...) {
    maximum_commands_ = 0U;
    return false;
  }
}

void UndoStack::clear() noexcept {
  history_.clear();
  cursor_ = 0U;
  origin_cursor_ = 0U;
  origin_reachable_ = true;
}

bool UndoStack::push(TileEditCommand&& command) noexcept {
  if (!initialized_ ||
      (command.changes.empty() && command.editor_changes.empty() &&
       command.isom_changes.empty() && command.section_changes.empty())) {
    return false;
  }
  // initialize() reserves the hard limit, so the operations below cannot
  // allocate. This lets EditorDocument commit a stroke atomically.
  if (cursor_ < history_.size()) {
    if (origin_reachable_ && origin_cursor_ > cursor_) {
      origin_reachable_ = false;
    }
    history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(cursor_),
                   history_.end());
  }
  if (history_.size() == maximum_commands_) {
    history_.erase(history_.begin());
    if (cursor_ != 0U) {
      --cursor_;
    }
    if (origin_reachable_) {
      if (origin_cursor_ == 0U) {
        origin_reachable_ = false;
      } else {
        --origin_cursor_;
      }
    }
  }
  history_.push_back(std::move(command));
  cursor_ = history_.size();
  return true;
}

bool UndoStack::undo(std::vector<std::uint16_t>& tiles) noexcept {
  if (!can_undo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_ - 1U];
  if (!indices_valid(command, tiles.size())) {
    return false;
  }
  for (auto change = command.changes.rbegin();
       change != command.changes.rend(); ++change) {
    tiles[change->tile_index] = change->before;
  }
  --cursor_;
  return true;
}

bool UndoStack::redo(std::vector<std::uint16_t>& tiles) noexcept {
  if (!can_redo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_];
  if (!indices_valid(command, tiles.size())) {
    return false;
  }
  for (const TileChange& change : command.changes) {
    tiles[change.tile_index] = change.after;
  }
  ++cursor_;
  return true;
}

bool UndoStack::undo(std::vector<std::uint16_t>& game_tiles,
                     std::vector<std::uint16_t>& editor_tiles,
                     formats::IsomSection& isom) noexcept {
  if (!can_undo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_ - 1U];
  if (!indices_valid(command, game_tiles.size(), editor_tiles.size(),
                     isom.entry_count())) {
    return false;
  }
  for (auto change = command.isom_changes.rbegin();
       change != command.isom_changes.rend(); ++change) {
    apply_isom_change(isom, *change, false);
  }
  for (auto change = command.editor_changes.rbegin();
       change != command.editor_changes.rend(); ++change) {
    editor_tiles[change->tile_index] = change->before;
  }
  for (auto change = command.changes.rbegin();
       change != command.changes.rend(); ++change) {
    game_tiles[change->tile_index] = change->before;
  }
  --cursor_;
  return true;
}

bool UndoStack::redo(std::vector<std::uint16_t>& game_tiles,
                     std::vector<std::uint16_t>& editor_tiles,
                     formats::IsomSection& isom) noexcept {
  if (!can_redo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_];
  if (!indices_valid(command, game_tiles.size(), editor_tiles.size(),
                     isom.entry_count())) {
    return false;
  }
  for (const TileChange& change : command.changes) {
    game_tiles[change.tile_index] = change.after;
  }
  for (const TileChange& change : command.editor_changes) {
    editor_tiles[change.tile_index] = change.after;
  }
  for (const TileEditCommand::IsomChange& change : command.isom_changes) {
    apply_isom_change(isom, change, true);
  }
  ++cursor_;
  return true;
}

bool UndoStack::undo(std::vector<std::uint16_t>& game_tiles,
                     std::vector<std::uint16_t>& editor_tiles,
                     formats::IsomSection& isom,
                     formats::ChkDocument& chk) noexcept {
  if (!can_undo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_ - 1U];
  if (!indices_valid(command, game_tiles.size(), editor_tiles.size(),
                     isom.entry_count()) ||
      !apply_section_changes(command, chk, false)) {
    return false;
  }
  for (auto change = command.isom_changes.rbegin();
       change != command.isom_changes.rend(); ++change) {
    apply_isom_change(isom, *change, false);
  }
  for (auto change = command.editor_changes.rbegin();
       change != command.editor_changes.rend(); ++change) {
    editor_tiles[change->tile_index] = change->before;
  }
  for (auto change = command.changes.rbegin();
       change != command.changes.rend(); ++change) {
    game_tiles[change->tile_index] = change->before;
  }
  --cursor_;
  return true;
}

bool UndoStack::redo(std::vector<std::uint16_t>& game_tiles,
                     std::vector<std::uint16_t>& editor_tiles,
                     formats::IsomSection& isom,
                     formats::ChkDocument& chk) noexcept {
  if (!can_redo()) {
    return false;
  }
  const TileEditCommand& command = history_[cursor_];
  if (!indices_valid(command, game_tiles.size(), editor_tiles.size(),
                     isom.entry_count()) ||
      !apply_section_changes(command, chk, true)) {
    return false;
  }
  for (const TileChange& change : command.changes) {
    game_tiles[change.tile_index] = change.after;
  }
  for (const TileChange& change : command.editor_changes) {
    editor_tiles[change.tile_index] = change.after;
  }
  for (const TileEditCommand::IsomChange& change : command.isom_changes) {
    apply_isom_change(isom, change, true);
  }
  ++cursor_;
  return true;
}

bool UndoStack::can_undo() const noexcept { return cursor_ != 0U; }
bool UndoStack::can_redo() const noexcept { return cursor_ < history_.size(); }
bool UndoStack::at_origin() const noexcept {
  return origin_reachable_ && cursor_ == origin_cursor_;
}
void UndoStack::mark_origin() noexcept {
  origin_cursor_ = cursor_;
  origin_reachable_ = true;
}
std::size_t UndoStack::undo_count() const noexcept { return cursor_; }
std::size_t UndoStack::redo_count() const noexcept {
  return history_.size() - cursor_;
}

bool UndoStack::indices_valid(const TileEditCommand& command,
                              const std::size_t tile_count) noexcept {
  return std::all_of(command.changes.begin(), command.changes.end(),
                     [tile_count](const TileChange& change) {
                       return change.tile_index < tile_count;
                     });
}

bool UndoStack::indices_valid(const TileEditCommand& command,
                              const std::size_t game_tile_count,
                              const std::size_t editor_tile_count,
                              const std::size_t isom_entry_count) noexcept {
  return std::all_of(command.changes.begin(), command.changes.end(),
                     [game_tile_count](const TileChange& change) {
                       return change.tile_index < game_tile_count;
                     }) &&
         std::all_of(command.editor_changes.begin(),
                     command.editor_changes.end(),
                     [editor_tile_count](const TileChange& change) {
                       return change.tile_index < editor_tile_count;
                     }) &&
         std::all_of(command.isom_changes.begin(), command.isom_changes.end(),
                     [isom_entry_count](
                         const TileEditCommand::IsomChange& change) {
                       return change.entry_index < isom_entry_count;
                     });
}

void UndoStack::apply_isom_change(
    formats::IsomSection& isom,
    const TileEditCommand::IsomChange& change,
    const bool after) noexcept {
  if (isom.column_count() == 0U) {
    return;
  }
  formats::IsomEntry* const entry = isom.mutable_entry(
      change.entry_index % isom.column_count(),
      change.entry_index / isom.column_count());
  if (entry != nullptr) {
    *entry = after ? change.after : change.before;
  }
}

bool UndoStack::apply_section_changes(const TileEditCommand& command,
                                      formats::ChkDocument& chk,
                                      const bool after) noexcept {
  if (command.section_changes.empty()) {
    return true;
  }
  try {
    formats::ChkDocument candidate = chk;
    const auto apply = [&candidate, after](
                           const TileEditCommand::SectionChange& change) {
      const bool exists = after ? change.after_exists : change.before_exists;
      const std::vector<std::uint8_t>& payload =
          after ? change.after : change.before;
      const bool current_exists =
          candidate.section(change.tag, change.occurrence) != nullptr;
      if (exists) {
        if (current_exists) {
          return candidate.replace_section(change.tag, change.occurrence,
                                           payload);
        }
        return change.occurrence == candidate.count(change.tag) &&
               candidate.append_section(change.tag, payload);
      }
      return !current_exists ||
             candidate.erase_section(change.tag, change.occurrence);
    };
    if (after) {
      for (const TileEditCommand::SectionChange& change :
           command.section_changes) {
        if (!apply(change)) {
          return false;
        }
      }
    } else {
      for (auto change = command.section_changes.rbegin();
           change != command.section_changes.rend(); ++change) {
        if (!apply(*change)) {
          return false;
        }
      }
    }
    chk = std::move(candidate);
    return true;
  } catch (...) {
    return false;
  }
}

}  // namespace staredit::undo
