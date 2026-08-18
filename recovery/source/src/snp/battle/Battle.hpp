#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::runtime {
class StormModule;
}

namespace starcraft::recovery {

struct BattleArtwork;
struct RecoveryWindowState;

namespace battle {

// The archived Battle.snp exposes these screens as nested modal dialogs.
// The recovery keeps the same flow in one render surface while preserving the
// verified CodeView function names at their original source-unit boundaries.
enum class BattleScreen : std::uint8_t {
  connecting,
  logon,
  new_account,
  channel_select,
  chat_room,
  join_game,
  create_game,
  create_browse,
  ladder,
  profile,
};

enum class BattleUiAction : std::uint8_t {
  none,
  redraw,
  leave_battle_net,
  enter_game_lobby,
  leave_game_lobby,
  start_game,
};

enum class EditControl : std::uint8_t {
  none,
  account_name,
  password,
  confirm_password,
  channel_name,
  chat_input,
  game_name,
  game_password,
  ladder_search,
};

struct ChannelEntry {
  std::string name{};
  std::uint32_t users{};
};

struct GameEntry {
  std::uint32_t identifier{};
  std::string name{};
  std::string host{};
  std::string map{};
  std::uint32_t players{};
  std::uint32_t maximum_players{};
};

struct BattleMapEntry {
  std::string path{};
  std::string name{};
  std::uint16_t width{};
  std::uint16_t height{};
  std::uint32_t players{};
};

struct LobbyPlayer {
  std::uint8_t slot{};
  std::string name{};
};

enum class LobbySlotKind : std::uint8_t {
  closed,
  open,
  computer,
  human,
};

struct LobbySlotEntry {
  std::uint8_t slot{};
  LobbySlotKind kind{LobbySlotKind::closed};
  std::uint8_t race{1U};
};

struct PlayerTurnPayload {
  std::uint8_t slot{};
  std::vector<std::uint8_t> payload{};
};

struct CommittedTurn {
  std::uint32_t turn{};
  std::vector<PlayerTurnPayload> players{};
};

struct PendingOutgoingTurn {
  std::uint32_t turn{};
  std::vector<std::uint8_t> payload{};
};

struct BattleRuntime {
  BattleScreen screen{BattleScreen::connecting};
  EditControl edit_control{EditControl::account_name};
  std::uintptr_t socket_handle{static_cast<std::uintptr_t>(-1)};
  std::string server_host{"127.0.0.1"};
  std::uint16_t server_port{6112U};
  std::string account_name{"Player"};
  std::string password{};
  std::string confirm_password{};
  std::string channel_name{};
  std::string chat_input{};
  std::string game_name{"Recovery Game"};
  std::string game_password{};
  std::string selected_map_name{};
  std::string ladder_search{};
  std::string profile_name{};
  std::string current_channel{};
  std::string status{};
  std::string receive_buffer{};
  std::vector<ChannelEntry> channels{};
  std::vector<std::string> users{};
  std::vector<std::string> chat_lines{};
  std::vector<GameEntry> games{};
  std::vector<BattleMapEntry> available_maps{};
  std::vector<LobbyPlayer> lobby_players{};
  std::vector<LobbySlotEntry> lobby_slots{};
  std::vector<CommittedTurn> committed_turns{};
  std::vector<PendingOutgoingTurn> outgoing_turns{};
  std::uint32_t game_identifier{};
  std::uint32_t game_seed{1U};
  std::uint32_t simulation_turn{};
  std::uint32_t next_turn_to_submit{};
  std::uint32_t account_wins{};
  std::uint32_t account_losses{};
  std::uint8_t local_player_slot{};
  std::uint8_t game_player_count{};
  std::uint8_t winning_player_slot{};
  std::size_t selected_channel{};
  std::size_t selected_game{};
  std::size_t selected_map{};
  std::uint8_t game_speed{3U};
  std::int16_t hovered_control{-1};
  std::int16_t pressed_control{-1};
  std::uint32_t last_ping_tick{};
  bool winsock_ready{};
  bool connected{};
  bool authenticated{};
  bool pending_game_lobby{};
  bool pending_game_lobby_exit{};
  bool pending_game_start{};
  bool game_host{};
  bool game_started{};
  bool game_aborted{};
  bool game_result_reported{};
  bool game_finished{};
  bool connect_pending{};
  bool local_server_launch_attempted{};
  // DrawBattleNet marks this only after the recovered connect dialog artwork
  // has reached the OpenGL surface. UiNotification is then allowed to enter
  // COnnect.cpp and begin socket discovery on the following loop iteration.
  mutable bool connect_artwork_presented{};
};

struct SnpDescriptor {
  std::uint32_t identifier{};
  const char *description{};
  const char *requirements{};
  std::uint32_t capabilities{};
};

// Battle.cpp / Ui.cpp
[[nodiscard]] bool SnpQuery(std::uint32_t index,
                            SnpDescriptor &descriptor) noexcept;
[[nodiscard]] bool SnpBind(std::uint32_t index) noexcept;
[[nodiscard]] bool UiInitialize(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool UiLoadArtwork(starcraft::runtime::StormModule &storm,
                                 BattleArtwork &artwork) noexcept;
void UiDestroy(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool UiBeginConnect(BattleRuntime &runtime) noexcept;
void UiEndConnect(BattleRuntime &runtime) noexcept;
[[nodiscard]] BattleUiAction UiNotification(BattleRuntime &runtime) noexcept;
[[nodiscard]] std::int16_t BattleNetControlAt(const BattleRuntime &runtime,
                                              int x, int y) noexcept;
[[nodiscard]] BattleUiAction BattleNetMouseMove(BattleRuntime &runtime,
                                                int x, int y) noexcept;
[[nodiscard]] BattleUiAction BattleNetLeftDown(BattleRuntime &runtime,
                                               int x, int y) noexcept;
[[nodiscard]] BattleUiAction BattleNetLeftUp(BattleRuntime &runtime, int x,
                                             int y,
                                             std::string_view map_name) noexcept;
[[nodiscard]] BattleUiAction BattleNetKeyDown(BattleRuntime &runtime,
                                              std::uintptr_t key,
                                              std::string_view map_name) noexcept;
[[nodiscard]] BattleUiAction BattleNetCharacter(BattleRuntime &runtime,
                                                char character) noexcept;
void DrawBattleNet(const RecoveryWindowState &state) noexcept;

// COnnect.cpp / Logon.cpp / Account.cpp
[[nodiscard]] bool ConnectDialogProc(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool UiLogon(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool LogonAttempt(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool NewAccount(BattleRuntime &runtime) noexcept;

// Chatchnl.cpp / Chatroom.cpp / Joingame.cpp
[[nodiscard]] bool ChatSelectChannel(BattleRuntime &runtime,
                                     std::string_view channel) noexcept;
void ChatChannelJoined(BattleRuntime &runtime,
                       std::string_view channel) noexcept;
void ChatAddUser(BattleRuntime &runtime, std::string_view user) noexcept;
void ChatDeleteUser(BattleRuntime &runtime, std::string_view user) noexcept;
void ChatReceiveMsg(BattleRuntime &runtime, std::string_view user,
                    std::string_view message) noexcept;
[[nodiscard]] bool SendChatMsg(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool DoJoinGame(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool DoCreateGame(BattleRuntime &runtime,
                                std::string_view map_name) noexcept;
[[nodiscard]] bool CreateGameAttempt(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool UpdateGameList(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool DoJoin(BattleRuntime &runtime) noexcept;

// Srv.cpp. This intentionally retains the original service boundary while
// using the new RECOVERY-BNET/1 protocol documented by the server project.
[[nodiscard]] bool SrvInitialize(BattleRuntime &runtime) noexcept;
void SrvDestroy(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvConnectToServer(BattleRuntime &runtime) noexcept;
void SrvDisconnect(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvIsConnected(const BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvCreateAccount(BattleRuntime &runtime,
                                    std::string_view account,
                                    std::string_view password) noexcept;
[[nodiscard]] bool SrvLogon(BattleRuntime &runtime, std::string_view account,
                            std::string_view password) noexcept;
[[nodiscard]] bool SrvBeginChat(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvJoinChannel(BattleRuntime &runtime,
                                  std::string_view channel) noexcept;
[[nodiscard]] bool SrvSendChatString(BattleRuntime &runtime,
                                     std::string_view message) noexcept;
[[nodiscard]] bool SrvGetGameList(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvStartAdvertisingGame(BattleRuntime &runtime,
                                           std::string_view name,
                                           std::string_view map,
                                           std::uint32_t maximum_players) noexcept;
[[nodiscard]] bool SrvNotifyJoin(BattleRuntime &runtime,
                                 std::uint32_t game_identifier) noexcept;
[[nodiscard]] bool SrvSetLobbySlot(BattleRuntime &runtime, std::uint8_t slot,
                                   LobbySlotKind kind,
                                   std::uint8_t race) noexcept;
[[nodiscard]] bool SrvLeaveGame(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvStartGame(BattleRuntime &runtime) noexcept;
[[nodiscard]] bool SrvSubmitTurn(BattleRuntime &runtime, std::uint32_t turn,
                                 const std::vector<std::uint8_t> &payload) noexcept;
[[nodiscard]] bool SrvReportGameResult(BattleRuntime &runtime,
                                       std::uint8_t winner_slot) noexcept;
[[nodiscard]] bool SrvTakeCommittedTurn(BattleRuntime &runtime,
                                        std::uint32_t turn,
                                        CommittedTurn &commit) noexcept;
[[nodiscard]] bool SrvProcessClientReq(BattleRuntime &runtime) noexcept;

} // namespace battle
} // namespace starcraft::recovery
