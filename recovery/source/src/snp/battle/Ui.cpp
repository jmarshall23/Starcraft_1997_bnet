#include "Battle.hpp"

#include "../../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <string_view>

namespace starcraft::recovery::battle {
namespace {

// Battle.snp's dialog resources use a 256x202 dialog-unit canvas over the
// 640x480 artwork supplied by lang/gluBN.cpp.
struct UiRect {
  int left{};
  int top{};
  int right{};
  int bottom{};
};

constexpr int dlu_x(const int value) noexcept {
  return (value * 640 + 128) / 256;
}

constexpr int dlu_y(const int value) noexcept {
  return (value * 480 + 101) / 202;
}

constexpr UiRect dialog_rect(const int x, const int y, const int width,
                             const int height,
                             const int parent_y = 0) noexcept {
  return {dlu_x(x), dlu_y(parent_y + y), dlu_x(x + width),
          dlu_y(parent_y + y + height)};
}

constexpr int kChildDialogY = 57;
constexpr std::int16_t kConnectCancel = 2;
constexpr std::int16_t kLogonName = 1020;
constexpr std::int16_t kLogonPassword = 1011;
constexpr std::int16_t kDialogOkay = 1;
constexpr std::int16_t kDialogCancel = 2;
constexpr std::int16_t kNewAccount = 1055;
constexpr std::int16_t kNewName = 1064;
constexpr std::int16_t kNewPassword = 1023;
constexpr std::int16_t kRepeatPassword = 1024;
constexpr std::int16_t kChannelName = 1010;
constexpr std::int16_t kChannelList = 1039;
constexpr std::int16_t kChatInput = 1006;
constexpr std::int16_t kChatChannel = 1012;
constexpr std::int16_t kChatCreate = 1008;
constexpr std::int16_t kChatJoin = 1007;
constexpr std::int16_t kChatLadder = 1009;
constexpr std::int16_t kChatQuit = 1045;
constexpr std::int16_t kChatSend = 1;
constexpr std::int16_t kChatWhisper = 1035;
constexpr std::int16_t kJoinName = 1010;
constexpr std::int16_t kJoinPassword = 1011;
constexpr std::int16_t kJoinList = 1027;
constexpr std::int16_t kCreateName = 1000;
constexpr std::int16_t kCreatePassword = 1001;
constexpr std::int16_t kCreateMap = 1009;
constexpr std::int16_t kCreateBrowse = 1008;
constexpr std::int16_t kCreateGameType = 1201;
constexpr std::int16_t kCreateSubtype = 1005;
constexpr std::int16_t kBrowsePath = 1016;
constexpr std::int16_t kBrowseList = 1200;
constexpr std::int16_t kLadderSearchEdit = 1010;
constexpr std::int16_t kLadderSearchButton = 1085;
constexpr std::int16_t kLadderPrevious = 1070;
constexpr std::int16_t kLadderNext = 1071;
constexpr std::int16_t kLadderClose = 2;
constexpr std::int16_t kChannelRowBase = 2000;
constexpr std::int16_t kGameRowBase = 2200;
constexpr std::int16_t kMapRowBase = 2400;
constexpr std::int16_t kUserRowBase = 2600;

constexpr UiRect kConnectCancelRect{263, 259, 373, 288};

constexpr UiRect kLogonNameRect = dialog_rect(22, 87, 88, 14);
constexpr UiRect kLogonPasswordRect = dialog_rect(23, 114, 85, 12);
constexpr UiRect kLogonOkayRect = dialog_rect(179, 185, 34, 15);
constexpr UiRect kLogonCancelRect = dialog_rect(214, 185, 34, 15);
constexpr UiRect kNewAccountRect = dialog_rect(33, 157, 66, 15);

constexpr UiRect kNewNameRect =
    dialog_rect(23, 31, 85, 12, kChildDialogY);
constexpr UiRect kNewPasswordRect =
    dialog_rect(23, 57, 85, 12, kChildDialogY);
constexpr UiRect kRepeatPasswordRect =
    dialog_rect(23, 82, 85, 12, kChildDialogY);
constexpr UiRect kNewOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kNewCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

constexpr UiRect kChannelNameRect =
    dialog_rect(168, 50, 76, 12, kChildDialogY);
constexpr UiRect kChannelListRect =
    dialog_rect(10, 23, 97, 117, kChildDialogY);
constexpr UiRect kChannelOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kChannelCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

constexpr UiRect kChatInputRect =
    dialog_rect(46, 127, 122, 12, kChildDialogY);
constexpr UiRect kChatChannelRect =
    dialog_rect(4, 2, 34, 23, kChildDialogY);
constexpr UiRect kChatCreateRect =
    dialog_rect(4, 31, 34, 23, kChildDialogY);
constexpr UiRect kChatJoinRect =
    dialog_rect(4, 60, 34, 23, kChildDialogY);
constexpr UiRect kChatLadderRect =
    dialog_rect(4, 89, 34, 23, kChildDialogY);
constexpr UiRect kChatQuitRect =
    dialog_rect(4, 118, 34, 23, kChildDialogY);
constexpr UiRect kChatMessagesRect =
    dialog_rect(46, 10, 115, 108, kChildDialogY);
constexpr UiRect kChatUsersRect =
    dialog_rect(184, 27, 57, 92, kChildDialogY);
constexpr UiRect kChatSendRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kChatWhisperRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

constexpr UiRect kJoinNameRect =
    dialog_rect(168, 50, 76, 12, kChildDialogY);
constexpr UiRect kJoinPasswordRect =
    dialog_rect(168, 69, 76, 12, kChildDialogY);
constexpr UiRect kJoinListRect =
    dialog_rect(10, 23, 97, 117, kChildDialogY);
constexpr UiRect kJoinOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kJoinCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

constexpr UiRect kCreateNameRect =
    dialog_rect(21, 33, 64, 12, kChildDialogY);
constexpr UiRect kCreatePasswordRect =
    dialog_rect(95, 33, 64, 12, kChildDialogY);
constexpr UiRect kCreateMapRect =
    dialog_rect(21, 64, 96, 14, kChildDialogY);
constexpr UiRect kCreateBrowseRect =
    dialog_rect(119, 64, 44, 15, kChildDialogY);
constexpr UiRect kCreateGameTypeRect =
    dialog_rect(21, 86, 88, 14, kChildDialogY);
constexpr UiRect kCreateSubtypeRect =
    dialog_rect(109, 86, 56, 14, kChildDialogY);
constexpr UiRect kCreateOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kCreateCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);
constexpr UiRect kBrowsePathRect =
    dialog_rect(11, 29, 108, 12, kChildDialogY);
constexpr UiRect kBrowseListRect =
    dialog_rect(11, 56, 105, 56, kChildDialogY);
constexpr UiRect kBrowseOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kBrowseCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

constexpr UiRect kLadderCloseRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);
constexpr UiRect kLadderSearchRect =
    dialog_rect(127, 26, 63, 12, kChildDialogY);
constexpr UiRect kLadderSearchButtonRect =
    dialog_rect(194, 26, 44, 12, kChildDialogY);
constexpr UiRect kLadderPreviousRect =
    dialog_rect(69, 25, 13, 15, kChildDialogY);
constexpr UiRect kLadderNextRect =
    dialog_rect(111, 25, 13, 15, kChildDialogY);
constexpr UiRect kProfileOkayRect =
    dialog_rect(179, 128, 34, 15, kChildDialogY);
constexpr UiRect kProfileCancelRect =
    dialog_rect(214, 128, 34, 15, kChildDialogY);

bool contains(const UiRect &rect, const int x, const int y) noexcept {
  return x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
}

float scaled_y(const int value) noexcept {
  return static_cast<float>(value) * hud_vertical_scale();
}

void draw_frame(const SpritePreviewFrame &frame, const float x,
                const float logical_y, const float width,
                const float logical_height) noexcept {
  draw_preview_frame_gl(frame, x, logical_y * hud_vertical_scale(), width,
                        logical_height * hud_vertical_scale());
}

void draw_full_background(const SpritePreviewFrame &frame) noexcept {
  draw_frame(frame, 0.0F, 0.0F, 640.0F, 480.0F);
}

bool load_art(starcraft::runtime::StormModule &storm, const char *path,
              const bool transparent, SpritePreviewFrame &output) {
  starcraft::runtime::DecodedPcx image{};
  return storm.load_pcx(path, image) &&
         decode_pcx_frame(image, transparent, output);
}

bool split_five_state_sheet(const SpritePreviewFrame &sheet,
                            std::array<SpritePreviewFrame, 5> &frames) {
  if (sheet.width == 0U || sheet.height == 0U ||
      (sheet.height % frames.size() != 0U &&
       sheet.height % frames.size() != frames.size() - 1U) ||
      sheet.bgra.empty()) {
    return false;
  }
  // but_110x35.pcx retains a one-row separator between its five states
  // (184 = 5*36 + 4); the other retail sheets are tightly stacked.
  const bool separated = sheet.height % frames.size() == frames.size() - 1U;
  const std::size_t frame_height =
      separated ? (sheet.height - (frames.size() - 1U)) / frames.size()
                : sheet.height / frames.size();
  const std::size_t stride = frame_height + (separated ? 1U : 0U);
  try {
    for (std::size_t state = 0U; state < frames.size(); ++state) {
      SpritePreviewFrame &frame = frames[state];
      frame.width = sheet.width;
      frame.height = static_cast<std::uint16_t>(frame_height);
      const std::size_t first = state * stride * sheet.width;
      const std::size_t count = frame_height * sheet.width;
      frame.bgra.assign(sheet.bgra.begin() + static_cast<std::ptrdiff_t>(first),
                        sheet.bgra.begin() +
                            static_cast<std::ptrdiff_t>(first + count));
    }
  } catch (...) {
    frames = {};
    return false;
  }
  return true;
}

bool load_button_sheet(starcraft::runtime::StormModule &storm,
                       const char *path,
                       std::array<SpritePreviewFrame, 5> &frames) {
  SpritePreviewFrame sheet{};
  return load_art(storm, path, true, sheet) &&
         split_five_state_sheet(sheet, frames);
}

void draw_label(const RecoveryWindowState &state, const std::string_view text,
                const int x, const int y, const bool highlighted = false,
                const bool large = false) noexcept {
  draw_glue_text_gl(state, text, static_cast<float>(x), static_cast<float>(y),
                    highlighted ? 255U : 218U,
                    highlighted ? 208U : 218U,
                    highlighted ? 88U : 218U, large);
}

float label_width(const RecoveryWindowState &state,
                  const std::string_view text) noexcept {
  float width{};
  for (const unsigned char character : text) {
    const std::size_t index =
        character >= 32U && character < 128U
            ? character - 32U
            : static_cast<std::size_t>('?' - 32);
    width += state.font_advances[index] * state.font_outline_scale;
  }
  return width;
}

void draw_centered_label(const RecoveryWindowState &state,
                         const std::string_view text, const UiRect &rect,
                         const bool highlighted = false) noexcept {
  const int x = static_cast<int>((rect.left + rect.right -
                                  label_width(state, text)) /
                                 2.0F);
  const int y = (rect.top + rect.bottom + 9) / 2;
  draw_label(state, text, x, y, highlighted);
}

void draw_button(const RecoveryWindowState &state,
                 const BattleRuntime &runtime, const std::int16_t identifier,
                 const UiRect &rect, const std::string_view text,
                 const std::array<SpritePreviewFrame, 5> &frames,
                 const bool enabled = true) noexcept {
  std::size_t image_state{};
  if (!enabled) {
    image_state = 4U;
  } else if (runtime.pressed_control == identifier) {
    image_state = 3U;
  } else if (runtime.hovered_control == identifier) {
    image_state = 2U;
  }
  const SpritePreviewFrame &frame = frames[image_state];
  const float width = static_cast<float>(frame.width);
  const float logical_height = static_cast<float>(frame.height);
  const float x = (static_cast<float>(rect.left + rect.right) - width) / 2.0F;
  const float y = (static_cast<float>(rect.top + rect.bottom) -
                   logical_height) /
                  2.0F;
  draw_frame(frame, x, y, width, logical_height);
  draw_centered_label(state, text, rect, enabled && image_state >= 2U);
}

void draw_edit_surface(const UiRect &rect) noexcept {
  glDisable(GL_TEXTURE_2D);
  glColor4ub(0U, 0U, 0U, 255U);
  glBegin(GL_QUADS);
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.bottom));
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.bottom));
  glEnd();
  glColor4ub(108U, 112U, 112U, 255U);
  glBegin(GL_LINE_LOOP);
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.bottom));
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.bottom));
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
}

void draw_square_button(const RecoveryWindowState &state,
                        const BattleRuntime &runtime,
                        const std::int16_t identifier, const UiRect &rect,
                        const std::string_view text) noexcept {
  draw_edit_surface(rect);
  draw_centered_label(state, text, rect,
                      runtime.hovered_control == identifier ||
                          runtime.pressed_control == identifier);
}

void draw_edit(const RecoveryWindowState &state, const BattleRuntime &runtime,
               const EditControl control, const UiRect &rect,
               const std::string_view value,
               const bool password = false) noexcept {
  std::string display = password ? std::string(value.size(), '*')
                                 : std::string(value);
  if (runtime.edit_control == control && (GetTickCount() / 500U) % 2U == 0U) {
    display.push_back('|');
  }
  draw_label(state, display, rect.left + 5, rect.bottom - 3,
             runtime.edit_control == control);
}

void draw_selection(const UiRect &rect) noexcept {
  glDisable(GL_TEXTURE_2D);
  glColor4ub(28U, 58U, 92U, 210U);
  glBegin(GL_QUADS);
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.top));
  glVertex2f(static_cast<float>(rect.right), scaled_y(rect.bottom));
  glVertex2f(static_cast<float>(rect.left), scaled_y(rect.bottom));
  glEnd();
  glColor4ub(255U, 255U, 255U, 255U);
  glEnable(GL_TEXTURE_2D);
}

void draw_shell(const RecoveryWindowState &state,
                const SpritePreviewFrame &child) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_full_background(art.shell_background);
  draw_frame(art.welcome_ad, static_cast<float>(dlu_x(37)),
             static_cast<float>(dlu_y(29)),
             static_cast<float>(art.welcome_ad.width),
             static_cast<float>(art.welcome_ad.height));
  draw_frame(child, 0.0F, static_cast<float>(dlu_y(kChildDialogY)), 640.0F,
             static_cast<float>(child.height));
}

void draw_connecting(const RecoveryWindowState &state,
                     const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_full_background(art.connect_background);
  const float popup_x = (640.0F - art.small_popup.width) / 2.0F;
  const float popup_y = (480.0F - art.small_popup.height) / 2.0F;
  draw_frame(art.small_popup, popup_x, popup_y,
             static_cast<float>(art.small_popup.width),
             static_cast<float>(art.small_popup.height));
  draw_label(state, "Searching for the fastest", 217, 205, true);
  draw_label(state, "Battle.net server...", 240, 225, true);
  draw_button(state, runtime, kConnectCancel, kConnectCancelRect, "Cancel",
              art.small_buttons);
}

void draw_logon(const RecoveryWindowState &state,
                const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_full_background(art.logon_background);
  draw_centered_label(state, "Battle.net Login", dialog_rect(2, 63, 254, 12),
                      true);
  draw_label(state, "Name:", dlu_x(23), dlu_y(87));
  draw_label(state, "Password:", dlu_x(23), dlu_y(114));
  draw_label(state, "Profile:", dlu_x(134), dlu_y(84));
  draw_edit(state, runtime, EditControl::account_name, kLogonNameRect,
            runtime.account_name);
  draw_edit(state, runtime, EditControl::password, kLogonPasswordRect,
            runtime.password, true);
  draw_button(state, runtime, kDialogOkay, kLogonOkayRect, "OK",
              art.extra_small_buttons);
  draw_button(state, runtime, kDialogCancel, kLogonCancelRect, "Cancel",
              art.extra_small_buttons);
  draw_button(state, runtime, kNewAccount, kNewAccountRect, "New Account",
              art.medium_buttons);
}

void draw_new_account(const RecoveryWindowState &state,
                      const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  // newaccount.pcx is absent from the licensed retail archive. Account.cpp's
  // recovered LoadArtwork deliberately ignores that failure, leaving the
  // parent bnlogin.pcx surface visible under DIALOG_NEW_ACCOUNT.
  draw_full_background(art.logon_background);
  draw_centered_label(state, "New Account",
                      dialog_rect(10, 6, 234, 10, kChildDialogY), true);
  draw_label(state, "Name:", dlu_x(23), dlu_y(kChildDialogY + 31));
  draw_label(state, "Password:", dlu_x(23), dlu_y(kChildDialogY + 57));
  draw_label(state, "Repeat Password:", dlu_x(23),
             dlu_y(kChildDialogY + 82));
  // Even when its optional child artwork fails, the Storm edit control owns
  // and draws its black client surface independently.
  draw_edit_surface(kRepeatPasswordRect);
  draw_edit(state, runtime, EditControl::account_name, kNewNameRect,
            runtime.account_name);
  draw_edit(state, runtime, EditControl::password, kNewPasswordRect,
            runtime.password, true);
  draw_edit(state, runtime, EditControl::confirm_password,
            kRepeatPasswordRect, runtime.confirm_password, true);
  draw_button(state, runtime, kDialogOkay, kNewOkayRect, "OK",
              art.extra_small_buttons,
              !runtime.account_name.empty() && !runtime.password.empty() &&
                  !runtime.confirm_password.empty());
  draw_button(state, runtime, kDialogCancel, kNewCancelRect, "Cancel",
              art.extra_small_buttons);
}

void draw_channels(const RecoveryWindowState &state,
                   const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.channel_background);
  draw_centered_label(state, "Channels",
                      dialog_rect(10, 8, 104, 9, kChildDialogY), true);
  draw_centered_label(state, "Select Channel",
                      dialog_rect(130, 8, 114, 9, kChildDialogY), true);
  draw_label(state, "To join or create a Private Channel,", dlu_x(130),
             dlu_y(kChildDialogY + 29));
  draw_label(state, "enter a name below.", dlu_x(130),
             dlu_y(kChildDialogY + 37));
  draw_label(state, "Channel:", dlu_x(130), dlu_y(kChildDialogY + 59));
  draw_edit(state, runtime, EditControl::channel_name, kChannelNameRect,
            runtime.channel_name);
  constexpr int row_height = 22;
  for (std::size_t row = 0U; row < runtime.channels.size() && row < 11U;
       ++row) {
    const UiRect row_rect{kChannelListRect.left + 4,
                          kChannelListRect.top + 4 +
                              static_cast<int>(row) * row_height,
                          kChannelListRect.right - 4,
                          kChannelListRect.top + 4 +
                                  static_cast<int>(row + 1U) * row_height -
                              1};
    if (row == runtime.selected_channel) {
      draw_selection(row_rect);
    }
    draw_label(state, runtime.channels[row].name, row_rect.left + 4,
               row_rect.bottom - 4, row == runtime.selected_channel);
  }
  draw_button(state, runtime, kDialogOkay, kChannelOkayRect, "OK",
              art.extra_small_buttons);
  draw_button(state, runtime, kDialogCancel, kChannelCancelRect, "Cancel",
              art.extra_small_buttons);
}

void draw_chat(const RecoveryWindowState &state,
               const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.chat_background);
  draw_centered_label(state, runtime.current_channel,
                      dialog_rect(181, 8, 66, 9, kChildDialogY), true);
  constexpr std::size_t visible_lines = 13U;
  const std::size_t first = runtime.chat_lines.size() > visible_lines
                                ? runtime.chat_lines.size() - visible_lines
                                : 0U;
  for (std::size_t index = first; index < runtime.chat_lines.size(); ++index) {
    draw_label(state, runtime.chat_lines[index], kChatMessagesRect.left + 4,
               kChatMessagesRect.top + 15 +
                   static_cast<int>(index - first) * 16);
  }
  for (std::size_t index = 0U; index < runtime.users.size() && index < 11U;
       ++index) {
    draw_label(state, runtime.users[index], kChatUsersRect.left + 4,
               kChatUsersRect.top + 15 + static_cast<int>(index) * 17,
               runtime.users[index] == runtime.account_name);
  }
  draw_edit(state, runtime, EditControl::chat_input, kChatInputRect,
            runtime.chat_input);
  draw_button(state, runtime, kChatChannel, kChatChannelRect, "Channel",
              art.battle_buttons);
  draw_button(state, runtime, kChatCreate, kChatCreateRect, "Create",
              art.battle_buttons);
  draw_button(state, runtime, kChatJoin, kChatJoinRect, "Join",
              art.battle_buttons);
  draw_button(state, runtime, kChatLadder, kChatLadderRect, "Ladder",
              art.battle_buttons);
  draw_button(state, runtime, kChatQuit, kChatQuitRect, "Quit",
              art.battle_buttons);
  draw_button(state, runtime, kChatSend, kChatSendRect, "Send",
              art.extra_small_buttons);
  draw_button(state, runtime, kChatWhisper, kChatWhisperRect, "Whisper",
              art.extra_small_buttons);
}

void draw_join_game(const RecoveryWindowState &state,
                    const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.join_background);
  draw_centered_label(state, "Matching Public Games",
                      dialog_rect(10, 8, 104, 9, kChildDialogY), true);
  draw_centered_label(state, "Join Game",
                      dialog_rect(130, 8, 114, 9, kChildDialogY), true);
  draw_label(state, "To Join a game, enter the game", dlu_x(130),
             dlu_y(kChildDialogY + 29));
  draw_label(state, "information below.", dlu_x(130),
             dlu_y(kChildDialogY + 37));
  draw_label(state, "Name:", dlu_x(130), dlu_y(kChildDialogY + 59));
  draw_label(state, "Password:", dlu_x(130), dlu_y(kChildDialogY + 78));
  draw_edit(state, runtime, EditControl::game_name, kJoinNameRect,
            runtime.game_name);
  draw_edit(state, runtime, EditControl::game_password, kJoinPasswordRect,
            runtime.game_password, true);
  constexpr int row_height = 25;
  for (std::size_t row = 0U; row < runtime.games.size() && row < 9U; ++row) {
    const UiRect row_rect{kJoinListRect.left + 4,
                          kJoinListRect.top + 4 +
                              static_cast<int>(row) * row_height,
                          kJoinListRect.right - 4,
                          kJoinListRect.top + 4 +
                                  static_cast<int>(row + 1U) * row_height -
                              1};
    if (row == runtime.selected_game) {
      draw_selection(row_rect);
    }
    draw_label(state, runtime.games[row].name, row_rect.left + 4,
               row_rect.bottom - 4, row == runtime.selected_game);
  }
  if (runtime.selected_game < runtime.games.size()) {
    const GameEntry &game = runtime.games[runtime.selected_game];
    draw_label(state, game.host + " - " + game.map, dlu_x(130),
               dlu_y(kChildDialogY + 98));
    draw_label(state,
               std::to_string(game.players) + "/" +
                   std::to_string(game.maximum_players) + " players",
               dlu_x(130), dlu_y(kChildDialogY + 108));
  }
  draw_button(state, runtime, kDialogOkay, kJoinOkayRect, "OK",
              art.extra_small_buttons);
  draw_button(state, runtime, kDialogCancel, kJoinCancelRect, "Cancel",
              art.extra_small_buttons);
}

void draw_create_game(const RecoveryWindowState &state,
                      const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.create_background);
  draw_centered_label(state, "Create Game",
                      dialog_rect(10, 6, 234, 12, kChildDialogY), true);
  draw_label(state, "Game Name:", dlu_x(21),
             dlu_y(kChildDialogY + 32));
  draw_label(state, "Game Password:", dlu_x(93),
             dlu_y(kChildDialogY + 32));
  draw_label(state, "Map:", dlu_x(21), dlu_y(kChildDialogY + 64));
  draw_label(state, "Game type:", dlu_x(21),
             dlu_y(kChildDialogY + 86));
  draw_label(state, "Subtype:", dlu_x(93),
             dlu_y(kChildDialogY + 88));
  draw_label(state, "Game Speed:", dlu_x(21),
             dlu_y(kChildDialogY + 111));
  draw_edit(state, runtime, EditControl::game_name, kCreateNameRect,
            runtime.game_name);
  draw_edit(state, runtime, EditControl::game_password, kCreatePasswordRect,
            runtime.game_password, true);
  draw_label(state, runtime.selected_map_name, kCreateMapRect.left + 4,
             kCreateMapRect.bottom - 4, true);
  draw_label(state, "Melee", kCreateGameTypeRect.left + 4,
             kCreateGameTypeRect.bottom - 4, true);
  draw_label(state, "Normal", kCreateSubtypeRect.left + 4,
             kCreateSubtypeRect.bottom - 4);
  draw_label(state, "Fast", dlu_x(82), dlu_y(kChildDialogY + 111), true);
  if (runtime.selected_map < runtime.available_maps.size()) {
    const BattleMapEntry &map = runtime.available_maps[runtime.selected_map];
    draw_label(state, map.name, dlu_x(181),
               dlu_y(kChildDialogY + 47), true);
    draw_label(state, "Players:", dlu_x(181),
               dlu_y(kChildDialogY + 91));
    draw_label(state, std::to_string(map.players), dlu_x(207),
               dlu_y(kChildDialogY + 91), true);
    draw_label(state, "Map Size:", dlu_x(181),
               dlu_y(kChildDialogY + 100));
    draw_label(state,
               std::to_string(map.width) + "x" + std::to_string(map.height),
               dlu_x(207), dlu_y(kChildDialogY + 100), true);
  }
  draw_button(state, runtime, kCreateBrowse, kCreateBrowseRect, "Browse...",
              art.browse_buttons);
  draw_button(state, runtime, kDialogOkay, kCreateOkayRect, "OK",
              art.extra_small_buttons,
              !runtime.game_name.empty() &&
                  !runtime.selected_map_name.empty());
  draw_button(state, runtime, kDialogCancel, kCreateCancelRect, "Cancel",
              art.extra_small_buttons);
}

void draw_create_browse(const RecoveryWindowState &state,
                        const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.browse_background);
  draw_centered_label(state, "Browse Scenarios and Campaigns",
                      dialog_rect(10, 6, 242, 10, kChildDialogY), true);
  draw_label(state, "Path:", dlu_x(10), dlu_y(kChildDialogY + 25));
  draw_label(state, runtime.selected_map_name, kBrowsePathRect.left + 4,
             kBrowsePathRect.bottom - 4, true);
  draw_label(state, "Directory View:", dlu_x(10),
             dlu_y(kChildDialogY + 52));
  draw_label(state, "Game Type:", dlu_x(139),
             dlu_y(kChildDialogY + 38));
  constexpr int row_height = 22;
  for (std::size_t row = 0U; row < runtime.available_maps.size() && row < 6U;
       ++row) {
    const UiRect row_rect{kBrowseListRect.left + 3,
                          kBrowseListRect.top + 3 +
                              static_cast<int>(row) * row_height,
                          kBrowseListRect.right - 3,
                          kBrowseListRect.top + 3 +
                                  static_cast<int>(row + 1U) * row_height -
                              1};
    if (row == runtime.selected_map) {
      draw_selection(row_rect);
    }
    draw_label(state, runtime.available_maps[row].name, row_rect.left + 4,
               row_rect.bottom - 4, row == runtime.selected_map);
  }
  if (runtime.selected_map < runtime.available_maps.size()) {
    const BattleMapEntry &map = runtime.available_maps[runtime.selected_map];
    draw_label(state, "Melee", dlu_x(183),
               dlu_y(kChildDialogY + 49), true);
    draw_label(state, map.name, dlu_x(139),
               dlu_y(kChildDialogY + 66), true);
    draw_label(state, "Players:", dlu_x(139),
               dlu_y(kChildDialogY + 96));
    draw_label(state, std::to_string(map.players), dlu_x(183),
               dlu_y(kChildDialogY + 96), true);
    draw_label(state, "Map Size:", dlu_x(139),
               dlu_y(kChildDialogY + 108));
    draw_label(state,
               std::to_string(map.width) + "x" + std::to_string(map.height),
               dlu_x(183), dlu_y(kChildDialogY + 108), true);
  }
  draw_button(state, runtime, kDialogOkay, kBrowseOkayRect, "OK",
              art.extra_small_buttons);
  draw_button(state, runtime, kDialogCancel, kBrowseCancelRect, "Cancel",
              art.extra_small_buttons);
}

void draw_ladder(const RecoveryWindowState &state,
                 const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.ladder_background);
  draw_centered_label(state, "Ladder Rankings",
                      dialog_rect(10, 6, 234, 9, kChildDialogY), true);
  draw_label(state, "Ladder Type", dlu_x(10), dlu_y(kChildDialogY + 27));
  draw_label(state, "Find a player:", dlu_x(126),
             dlu_y(kChildDialogY + 27));
  draw_edit(state, runtime, EditControl::ladder_search, kLadderSearchRect,
            runtime.ladder_search);
  draw_button(state, runtime, kLadderSearchButton, kLadderSearchButtonRect,
              "Search", art.small_buttons);
  draw_square_button(state, runtime, kLadderPrevious, kLadderPreviousRect,
                     "<");
  draw_centered_label(state, "1-25",
                      dialog_rect(83, 28, 24, 10, kChildDialogY), true);
  draw_square_button(state, runtime, kLadderNext, kLadderNextRect, ">");
  draw_label(state, "Rank     Name             Rating       Record       Games Played    Win %",
             dlu_x(10), dlu_y(kChildDialogY + 48), true);
  draw_label(state,
             "Ladder rankings are updated every night at midnight Pacific Time.",
             dlu_x(10), dlu_y(kChildDialogY + 130));
  draw_button(state, runtime, kLadderClose, kLadderCloseRect, "Close",
              art.extra_small_buttons);
}

void draw_profile(const RecoveryWindowState &state,
                  const BattleRuntime &runtime) noexcept {
  const BattleArtwork &art = state.glue.battle_artwork;
  draw_shell(state, art.profile_background);
  draw_centered_label(state, "Player Profile",
                      dialog_rect(12, 6, 234, 12, kChildDialogY), true);
  draw_label(state, "Name", dlu_x(12), dlu_y(kChildDialogY + 33));
  draw_label(state,
             runtime.profile_name.empty() ? std::string_view{"No Name"}
                                          : std::string_view{runtime.profile_name},
             dlu_x(15), dlu_y(kChildDialogY + 46), true);
  draw_label(state, "Sex", dlu_x(74), dlu_y(kChildDialogY + 32));
  draw_label(state, "Age", dlu_x(101), dlu_y(kChildDialogY + 32));
  draw_label(state, "Location", dlu_x(15), dlu_y(kChildDialogY + 59));
  draw_label(state, "Favorite Game Type", dlu_x(15),
             dlu_y(kChildDialogY + 85));
  draw_label(state, "Clan", dlu_x(85), dlu_y(kChildDialogY + 85));
  draw_label(state, "Homepage", dlu_x(15), dlu_y(kChildDialogY + 111));
  draw_button(state, runtime, kDialogOkay, kProfileOkayRect, "OK",
              art.extra_small_buttons);
  draw_button(state, runtime, kDialogCancel, kProfileCancelRect, "Cancel",
              art.extra_small_buttons);
}

std::string *edited_string(BattleRuntime &runtime) noexcept {
  switch (runtime.edit_control) {
  case EditControl::account_name:
    return &runtime.account_name;
  case EditControl::password:
    return &runtime.password;
  case EditControl::confirm_password:
    return &runtime.confirm_password;
  case EditControl::channel_name:
    return &runtime.channel_name;
  case EditControl::chat_input:
    return &runtime.chat_input;
  case EditControl::game_name:
    return &runtime.game_name;
  case EditControl::game_password:
    return &runtime.game_password;
  case EditControl::ladder_search:
    return &runtime.ladder_search;
  default:
    return nullptr;
  }
}

BattleUiAction activate_control(BattleRuntime &runtime,
                                const std::int16_t control,
                                const std::string_view map_name) noexcept {
  switch (runtime.screen) {
  case BattleScreen::connecting:
    if (control == kConnectCancel) {
      UiEndConnect(runtime);
      return BattleUiAction::leave_battle_net;
    }
    break;
  case BattleScreen::logon:
    if (control == kLogonName) {
      runtime.edit_control = EditControl::account_name;
    } else if (control == kLogonPassword) {
      runtime.edit_control = EditControl::password;
    } else if (control == kDialogOkay) {
      (void)LogonAttempt(runtime);
    } else if (control == kNewAccount) {
      (void)NewAccount(runtime);
    } else if (control == kDialogCancel) {
      UiEndConnect(runtime);
      return BattleUiAction::leave_battle_net;
    }
    break;
  case BattleScreen::new_account:
    if (control == kNewName) {
      runtime.edit_control = EditControl::account_name;
    } else if (control == kNewPassword) {
      runtime.edit_control = EditControl::password;
    } else if (control == kRepeatPassword) {
      runtime.edit_control = EditControl::confirm_password;
    } else if (control == kDialogOkay && !runtime.account_name.empty() &&
               !runtime.password.empty() &&
               !runtime.confirm_password.empty()) {
      (void)NewAccount(runtime);
    } else if (control == kDialogCancel) {
      (void)UiLogon(runtime);
    }
    break;
  case BattleScreen::channel_select:
    if (control == kChannelName) {
      runtime.edit_control = EditControl::channel_name;
    } else if (control >= kChannelRowBase &&
               static_cast<std::size_t>(control - kChannelRowBase) <
                   runtime.channels.size()) {
      runtime.selected_channel =
          static_cast<std::size_t>(control - kChannelRowBase);
      runtime.channel_name = runtime.channels[runtime.selected_channel].name;
    } else if (control == kDialogOkay) {
      const std::string_view channel =
          !runtime.channel_name.empty()
              ? std::string_view{runtime.channel_name}
              : runtime.selected_channel < runtime.channels.size()
                    ? std::string_view{
                          runtime.channels[runtime.selected_channel].name}
                    : std::string_view{};
      (void)ChatSelectChannel(runtime, channel);
    } else if (control == kDialogCancel) {
      UiEndConnect(runtime);
      return BattleUiAction::leave_battle_net;
    }
    break;
  case BattleScreen::chat_room:
    if (control == kChatInput) {
      runtime.edit_control = EditControl::chat_input;
    } else if (control == kChatSend) {
      (void)SendChatMsg(runtime);
    } else if (control == kChatWhisper) {
      runtime.status = "Select a user to whisper.";
    } else if (control == kChatChannel) {
      (void)SrvBeginChat(runtime);
    } else if (control == kChatJoin) {
      (void)DoJoinGame(runtime);
    } else if (control == kChatCreate) {
      (void)DoCreateGame(runtime, map_name);
    } else if (control == kChatLadder) {
      runtime.screen = BattleScreen::ladder;
      runtime.edit_control = EditControl::none;
    } else if (control == kChatQuit) {
      UiEndConnect(runtime);
      return BattleUiAction::leave_battle_net;
    } else if (control >= kUserRowBase &&
               static_cast<std::size_t>(control - kUserRowBase) <
                   runtime.users.size()) {
      runtime.profile_name =
          runtime.users[static_cast<std::size_t>(control - kUserRowBase)];
      runtime.screen = BattleScreen::profile;
      runtime.edit_control = EditControl::none;
    }
    break;
  case BattleScreen::join_game:
    if (control == kJoinName) {
      runtime.edit_control = EditControl::game_name;
    } else if (control == kJoinPassword) {
      runtime.edit_control = EditControl::game_password;
    } else if (control >= kGameRowBase &&
               static_cast<std::size_t>(control - kGameRowBase) <
                   runtime.games.size()) {
      runtime.selected_game = static_cast<std::size_t>(control - kGameRowBase);
      runtime.game_name = runtime.games[runtime.selected_game].name;
    } else if (control == kDialogOkay) {
      (void)DoJoin(runtime);
    } else if (control == kDialogCancel) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
    }
    break;
  case BattleScreen::create_game:
    if (control == kCreateName) {
      runtime.edit_control = EditControl::game_name;
    } else if (control == kCreatePassword) {
      runtime.edit_control = EditControl::game_password;
    } else if (control == kCreateBrowse) {
      runtime.screen = BattleScreen::create_browse;
      runtime.edit_control = EditControl::none;
    } else if (control == kDialogOkay && !runtime.game_name.empty() &&
               !runtime.selected_map_name.empty()) {
      (void)CreateGameAttempt(runtime);
    } else if (control == kDialogCancel) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
    }
    break;
  case BattleScreen::create_browse:
    if (control >= kMapRowBase &&
        static_cast<std::size_t>(control - kMapRowBase) <
            runtime.available_maps.size()) {
      runtime.selected_map = static_cast<std::size_t>(control - kMapRowBase);
      runtime.selected_map_name = runtime.available_maps[runtime.selected_map].name;
    } else if (control == kDialogOkay || control == kDialogCancel) {
      runtime.screen = BattleScreen::create_game;
      runtime.edit_control = EditControl::game_name;
    }
    break;
  case BattleScreen::ladder:
    if (control == kLadderSearchEdit) {
      runtime.edit_control = EditControl::ladder_search;
    } else if (control == kLadderSearchButton) {
      runtime.status = "Ladder lookup will use the recovery service.";
    } else if (control == kLadderClose) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
    }
    break;
  case BattleScreen::profile:
    if (control == kDialogOkay || control == kDialogCancel) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
    }
    break;
  }
  return BattleUiAction::redraw;
}

} // namespace

bool UiLoadArtwork(starcraft::runtime::StormModule &storm,
                   BattleArtwork &artwork) noexcept {
  artwork = {};
  const bool loaded =
      load_art(storm, R"(glue\battle.net\Backgrounds\bnconnect.pcx)",
               false, artwork.connect_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\bnlogin.pcx)", false,
               artwork.logon_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\bn_bkg.pcx)", false,
               artwork.shell_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\bnselchn.pcx)", false,
               artwork.channel_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\bnjoinbg.pcx)", false,
               artwork.join_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\chat_bkg.pcx)", false,
               artwork.chat_background) &&
      load_art(storm, R"(glue\battle.net\Backgrounds\bnladder.pcx)", false,
               artwork.ladder_background) &&
      load_art(storm, R"(glue\battle.net\backgrounds\creat_bg.pcx)", false,
               artwork.create_background) &&
      load_art(storm, R"(glue\battle.net\backgrounds\bnfile.pcx)", false,
               artwork.browse_background) &&
      load_art(storm,
               R"(glue\battle.net\backgrounds\bnviewprofile.pcx)", false,
               artwork.profile_background) &&
      load_art(storm, R"(glue\battle.net\popups\welcome.pcx)", false,
               artwork.welcome_ad) &&
      load_art(storm, R"(glue\battle.net\generic\popups\spopup.pcx)", true,
               artwork.small_popup) &&
      load_art(storm, R"(glue\battle.net\generic\popups\lepopup.pcx)", true,
               artwork.large_popup) &&
      load_button_sheet(storm,
                        R"(glue\battle.net\icons\bnbuttns.pcx)",
                        artwork.battle_buttons) &&
      load_button_sheet(storm,
                        R"(glue\battle.net\generic\icons\but_xsm.pcx)",
                        artwork.extra_small_buttons) &&
      load_button_sheet(storm,
                        R"(glue\battle.net\generic\icons\but_sml.pcx)",
                        artwork.small_buttons) &&
      load_button_sheet(storm,
                        R"(glue\battle.net\generic\icons\but_med.pcx)",
                        artwork.medium_buttons) &&
      load_button_sheet(
          storm, R"(glue\battle.net\generic\icons\but_110x35.pcx)",
          artwork.browse_buttons);
  artwork.ready = loaded;
  return loaded;
}

bool UiBeginConnect(BattleRuntime &runtime) noexcept {
  if (!UiInitialize(runtime)) {
    return false;
  }
  runtime.screen = BattleScreen::connecting;
  runtime.edit_control = EditControl::none;
  runtime.status = "Searching for the fastest Battle.net server...";
  runtime.connect_pending = true;
  runtime.connect_artwork_presented = false;
  return true;
}

void UiEndConnect(BattleRuntime &runtime) noexcept {
  SrvDisconnect(runtime);
  runtime.screen = BattleScreen::connecting;
  runtime.edit_control = EditControl::none;
  runtime.connect_pending = false;
  runtime.connect_artwork_presented = false;
  runtime.channels.clear();
  runtime.users.clear();
  runtime.chat_lines.clear();
  runtime.games.clear();
}

BattleUiAction UiNotification(BattleRuntime &runtime) noexcept {
  if (runtime.connect_pending && runtime.connect_artwork_presented) {
    runtime.connect_pending = false;
    (void)ConnectDialogProc(runtime);
    return BattleUiAction::redraw;
  }
  const bool changed = runtime.connected && SrvProcessClientReq(runtime);
  if (runtime.pending_game_lobby) {
    runtime.pending_game_lobby = false;
    return BattleUiAction::enter_game_lobby;
  }
  return changed ? BattleUiAction::redraw : BattleUiAction::none;
}

std::int16_t BattleNetControlAt(const BattleRuntime &runtime, const int x,
                                const int y) noexcept {
  switch (runtime.screen) {
  case BattleScreen::connecting:
    return contains(kConnectCancelRect, x, y) ? kConnectCancel : -1;
  case BattleScreen::logon:
    if (contains(kLogonNameRect, x, y)) return kLogonName;
    if (contains(kLogonPasswordRect, x, y)) return kLogonPassword;
    if (contains(kLogonOkayRect, x, y)) return kDialogOkay;
    if (contains(kLogonCancelRect, x, y)) return kDialogCancel;
    if (contains(kNewAccountRect, x, y)) return kNewAccount;
    break;
  case BattleScreen::new_account:
    if (contains(kNewNameRect, x, y)) return kNewName;
    if (contains(kNewPasswordRect, x, y)) return kNewPassword;
    if (contains(kRepeatPasswordRect, x, y)) return kRepeatPassword;
    if (contains(kNewOkayRect, x, y) && !runtime.account_name.empty() &&
        !runtime.password.empty() && !runtime.confirm_password.empty()) {
      return kDialogOkay;
    }
    if (contains(kNewCancelRect, x, y)) return kDialogCancel;
    break;
  case BattleScreen::channel_select:
    if (contains(kChannelNameRect, x, y)) return kChannelName;
    if (contains(kChannelListRect, x, y)) {
      const int row = (y - kChannelListRect.top - 4) / 22;
      if (row >= 0 && static_cast<std::size_t>(row) < runtime.channels.size()) {
        return static_cast<std::int16_t>(kChannelRowBase + row);
      }
    }
    if (contains(kChannelOkayRect, x, y)) return kDialogOkay;
    if (contains(kChannelCancelRect, x, y)) return kDialogCancel;
    break;
  case BattleScreen::chat_room:
    if (contains(kChatInputRect, x, y)) return kChatInput;
    if (contains(kChatChannelRect, x, y)) return kChatChannel;
    if (contains(kChatCreateRect, x, y)) return kChatCreate;
    if (contains(kChatJoinRect, x, y)) return kChatJoin;
    if (contains(kChatLadderRect, x, y)) return kChatLadder;
    if (contains(kChatQuitRect, x, y)) return kChatQuit;
    if (contains(kChatSendRect, x, y)) return kChatSend;
    if (contains(kChatWhisperRect, x, y)) return kChatWhisper;
    if (contains(kChatUsersRect, x, y)) {
      const int row = (y - kChatUsersRect.top) / 17;
      if (row >= 0 && static_cast<std::size_t>(row) < runtime.users.size()) {
        return static_cast<std::int16_t>(kUserRowBase + row);
      }
    }
    break;
  case BattleScreen::join_game:
    if (contains(kJoinNameRect, x, y)) return kJoinName;
    if (contains(kJoinPasswordRect, x, y)) return kJoinPassword;
    if (contains(kJoinListRect, x, y)) {
      const int row = (y - kJoinListRect.top - 4) / 25;
      if (row >= 0 && static_cast<std::size_t>(row) < runtime.games.size()) {
        return static_cast<std::int16_t>(kGameRowBase + row);
      }
    }
    if (contains(kJoinOkayRect, x, y)) return kDialogOkay;
    if (contains(kJoinCancelRect, x, y)) return kDialogCancel;
    break;
  case BattleScreen::create_game:
    if (contains(kCreateNameRect, x, y)) return kCreateName;
    if (contains(kCreatePasswordRect, x, y)) return kCreatePassword;
    if (contains(kCreateBrowseRect, x, y)) return kCreateBrowse;
    if (contains(kCreateOkayRect, x, y) && !runtime.game_name.empty() &&
        !runtime.selected_map_name.empty()) {
      return kDialogOkay;
    }
    if (contains(kCreateCancelRect, x, y)) return kDialogCancel;
    break;
  case BattleScreen::create_browse:
    if (contains(kBrowseListRect, x, y)) {
      const int row = (y - kBrowseListRect.top - 3) / 22;
      if (row >= 0 &&
          static_cast<std::size_t>(row) < runtime.available_maps.size()) {
        return static_cast<std::int16_t>(kMapRowBase + row);
      }
    }
    if (contains(kBrowseOkayRect, x, y)) return kDialogOkay;
    if (contains(kBrowseCancelRect, x, y)) return kDialogCancel;
    break;
  case BattleScreen::ladder:
    if (contains(kLadderSearchRect, x, y)) return kLadderSearchEdit;
    if (contains(kLadderSearchButtonRect, x, y)) return kLadderSearchButton;
    if (contains(kLadderPreviousRect, x, y)) return kLadderPrevious;
    if (contains(kLadderNextRect, x, y)) return kLadderNext;
    if (contains(kLadderCloseRect, x, y)) return kLadderClose;
    break;
  case BattleScreen::profile:
    if (contains(kProfileOkayRect, x, y)) return kDialogOkay;
    if (contains(kProfileCancelRect, x, y)) return kDialogCancel;
    break;
  }
  return -1;
}

BattleUiAction BattleNetMouseMove(BattleRuntime &runtime, const int x,
                                  const int y) noexcept {
  const std::int16_t previous = runtime.hovered_control;
  runtime.hovered_control = BattleNetControlAt(runtime, x, y);
  return previous == runtime.hovered_control ? BattleUiAction::none
                                             : BattleUiAction::redraw;
}

BattleUiAction BattleNetLeftDown(BattleRuntime &runtime, const int x,
                                 const int y) noexcept {
  runtime.pressed_control = BattleNetControlAt(runtime, x, y);
  return runtime.pressed_control == -1 ? BattleUiAction::none
                                       : BattleUiAction::redraw;
}

BattleUiAction BattleNetLeftUp(BattleRuntime &runtime, const int x,
                               const int y,
                               const std::string_view map_name) noexcept {
  const std::int16_t released = BattleNetControlAt(runtime, x, y);
  const std::int16_t pressed = runtime.pressed_control;
  runtime.pressed_control = -1;
  if (pressed == -1 || pressed != released) {
    return BattleUiAction::redraw;
  }
  return activate_control(runtime, released, map_name);
}

BattleUiAction BattleNetKeyDown(BattleRuntime &runtime,
                                const std::uintptr_t key,
                                const std::string_view map_name) noexcept {
  if (key == VK_ESCAPE) {
    if (runtime.screen == BattleScreen::new_account) {
      (void)UiLogon(runtime);
      return BattleUiAction::redraw;
    }
    if (runtime.screen == BattleScreen::join_game ||
        runtime.screen == BattleScreen::ladder ||
        runtime.screen == BattleScreen::profile) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
      return BattleUiAction::redraw;
    }
    if (runtime.screen == BattleScreen::create_browse) {
      runtime.screen = BattleScreen::create_game;
      runtime.edit_control = EditControl::game_name;
      return BattleUiAction::redraw;
    }
    if (runtime.screen == BattleScreen::create_game) {
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
      return BattleUiAction::redraw;
    }
    UiEndConnect(runtime);
    return BattleUiAction::leave_battle_net;
  }
  if (key == VK_TAB) {
    if (runtime.screen == BattleScreen::logon) {
      runtime.edit_control = runtime.edit_control == EditControl::account_name
                                 ? EditControl::password
                                 : EditControl::account_name;
    } else if (runtime.screen == BattleScreen::new_account) {
      runtime.edit_control =
          runtime.edit_control == EditControl::account_name
              ? EditControl::password
              : runtime.edit_control == EditControl::password
                    ? EditControl::confirm_password
                    : EditControl::account_name;
    } else if (runtime.screen == BattleScreen::join_game ||
               runtime.screen == BattleScreen::create_game) {
      runtime.edit_control = runtime.edit_control == EditControl::game_name
                                 ? EditControl::game_password
                                 : EditControl::game_name;
    }
    return BattleUiAction::redraw;
  }
  if (key == VK_UP) {
    if (runtime.screen == BattleScreen::channel_select &&
        runtime.selected_channel > 0U) {
      --runtime.selected_channel;
      runtime.channel_name = runtime.channels[runtime.selected_channel].name;
      return BattleUiAction::redraw;
    }
    if (runtime.screen == BattleScreen::join_game &&
        runtime.selected_game > 0U) {
      --runtime.selected_game;
      runtime.game_name = runtime.games[runtime.selected_game].name;
      return BattleUiAction::redraw;
    }
  }
  if (key == VK_DOWN) {
    if (runtime.screen == BattleScreen::channel_select &&
        runtime.selected_channel + 1U < runtime.channels.size()) {
      ++runtime.selected_channel;
      runtime.channel_name = runtime.channels[runtime.selected_channel].name;
      return BattleUiAction::redraw;
    }
    if (runtime.screen == BattleScreen::join_game &&
        runtime.selected_game + 1U < runtime.games.size()) {
      ++runtime.selected_game;
      runtime.game_name = runtime.games[runtime.selected_game].name;
      return BattleUiAction::redraw;
    }
  }
  if (key == VK_RETURN) {
    switch (runtime.screen) {
    case BattleScreen::connecting:
      return BattleUiAction::none;
    case BattleScreen::logon:
    case BattleScreen::new_account:
    case BattleScreen::channel_select:
    case BattleScreen::join_game:
    case BattleScreen::create_game:
    case BattleScreen::create_browse:
      return activate_control(runtime, kDialogOkay, map_name);
    case BattleScreen::chat_room:
      return activate_control(runtime, kChatSend, map_name);
    case BattleScreen::ladder:
    case BattleScreen::profile:
      return BattleUiAction::none;
    }
  }
  return BattleUiAction::none;
}

BattleUiAction BattleNetCharacter(BattleRuntime &runtime,
                                  const char character) noexcept {
  std::string *const target = edited_string(runtime);
  if (target == nullptr || character == '\r' || character == '\n') {
    return BattleUiAction::none;
  }
  if (character == '\b') {
    if (!target->empty()) {
      target->pop_back();
      return BattleUiAction::redraw;
    }
    return BattleUiAction::none;
  }
  std::size_t maximum = 240U;
  if (runtime.edit_control == EditControl::account_name) {
    maximum = 15U;
  } else if (runtime.edit_control == EditControl::password ||
             runtime.edit_control == EditControl::confirm_password ||
             runtime.edit_control == EditControl::game_password) {
    maximum = 64U;
  } else if (runtime.edit_control == EditControl::channel_name) {
    maximum = 31U;
  } else if (runtime.edit_control == EditControl::game_name) {
    maximum = 31U;
  }
  if (static_cast<unsigned char>(character) >= 32U &&
      static_cast<unsigned char>(character) < 127U && target->size() < maximum) {
    target->push_back(character);
    return BattleUiAction::redraw;
  }
  return BattleUiAction::none;
}

void DrawBattleNet(const RecoveryWindowState &state) noexcept {
  const BattleRuntime &runtime = state.glue.battle_net;
  if (!state.glue.battle_artwork.ready) {
    draw_full_background(state.glue.connection_background);
    return;
  }
  switch (runtime.screen) {
  case BattleScreen::connecting:
    draw_connecting(state, runtime);
    // This assignment is intentionally after every connect-dialog draw call.
    // The next UiNotification tick may now enter ConnectDialogProc.
    runtime.connect_artwork_presented = true;
    break;
  case BattleScreen::logon:
    draw_logon(state, runtime);
    break;
  case BattleScreen::new_account:
    draw_new_account(state, runtime);
    break;
  case BattleScreen::channel_select:
    draw_channels(state, runtime);
    break;
  case BattleScreen::chat_room:
    draw_chat(state, runtime);
    break;
  case BattleScreen::join_game:
    draw_join_game(state, runtime);
    break;
  case BattleScreen::create_game:
    draw_create_game(state, runtime);
    break;
  case BattleScreen::create_browse:
    draw_create_browse(state, runtime);
    break;
  case BattleScreen::ladder:
    draw_ladder(state, runtime);
    break;
  case BattleScreen::profile:
    draw_profile(state, runtime);
    break;
  }
}

} // namespace starcraft::recovery::battle
