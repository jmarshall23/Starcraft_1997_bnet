// New service transport for the recovered Battle.net flow. The archived
// Battle.snp Srv.cpp names are retained, but no legacy Battle.net packets,
// authentication hashes, opcodes, or server addresses are used here.
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Battle.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace starcraft::recovery::battle {
namespace {

constexpr std::uintptr_t kInvalidSocket = static_cast<std::uintptr_t>(-1);
constexpr std::size_t kMaximumBufferedBytes = 64U * 1024U;

SOCKET socket_from(const BattleRuntime &runtime) noexcept {
  return static_cast<SOCKET>(runtime.socket_handle);
}

bool safe_character(const unsigned char character) noexcept {
  return std::isalnum(character) != 0 || character == '-' ||
         character == '_' || character == '.' || character == ' ';
}

std::string encode_field(const std::string_view field) {
  constexpr char hex[] = "0123456789ABCDEF";
  std::string output;
  output.reserve(field.size());
  for (const unsigned char character : field) {
    if (safe_character(character) && character != '%') {
      output.push_back(static_cast<char>(character));
    } else {
      output.push_back('%');
      output.push_back(hex[character >> 4U]);
      output.push_back(hex[character & 0x0FU]);
    }
  }
  return output;
}

int hex_value(const char character) noexcept {
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }
  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }
  return -1;
}

std::string decode_field(const std::string_view field) {
  std::string output;
  output.reserve(field.size());
  for (std::size_t index = 0U; index < field.size(); ++index) {
    if (field[index] == '%' && index + 2U < field.size()) {
      const int high = hex_value(field[index + 1U]);
      const int low = hex_value(field[index + 2U]);
      if (high >= 0 && low >= 0) {
        output.push_back(static_cast<char>((high << 4) | low));
        index += 2U;
        continue;
      }
    }
    output.push_back(field[index]);
  }
  return output;
}

std::string encode_hex(const std::vector<std::uint8_t> &bytes) {
  constexpr char hex[] = "0123456789ABCDEF";
  if (bytes.empty()) {
    return "-";
  }
  std::string output;
  output.reserve(bytes.size() * 2U);
  for (const std::uint8_t byte : bytes) {
    output.push_back(hex[byte >> 4U]);
    output.push_back(hex[byte & 0x0FU]);
  }
  return output;
}

bool decode_hex(const std::string_view text,
                std::vector<std::uint8_t> &output) {
  output.clear();
  if (text == "-") {
    return true;
  }
  if ((text.size() & 1U) != 0U) {
    return false;
  }
  try {
    output.reserve(text.size() / 2U);
    for (std::size_t index = 0U; index < text.size(); index += 2U) {
      const int high = hex_value(text[index]);
      const int low = hex_value(text[index + 1U]);
      if (high < 0 || low < 0) {
        output.clear();
        return false;
      }
      output.push_back(static_cast<std::uint8_t>((high << 4) | low));
    }
  } catch (...) {
    output.clear();
    return false;
  }
  return true;
}

bool parse_lobby_slot_kind(const std::string_view text,
                           LobbySlotKind &kind) noexcept {
  if (text == "OPEN") {
    kind = LobbySlotKind::open;
  } else if (text == "COMPUTER") {
    kind = LobbySlotKind::computer;
  } else if (text == "HUMAN") {
    kind = LobbySlotKind::human;
  } else if (text == "CLOSED") {
    kind = LobbySlotKind::closed;
  } else {
    return false;
  }
  return true;
}

std::string_view lobby_slot_kind_name(const LobbySlotKind kind) noexcept {
  switch (kind) {
  case LobbySlotKind::open: return "OPEN";
  case LobbySlotKind::computer: return "COMPUTER";
  case LobbySlotKind::closed: return "CLOSED";
  case LobbySlotKind::human:
  default: return "HUMAN";
  }
}

std::vector<std::string_view> split_fields(const std::string_view line) {
  std::vector<std::string_view> fields;
  std::size_t begin{};
  while (begin <= line.size()) {
    const std::size_t separator = line.find('|', begin);
    if (separator == std::string_view::npos) {
      fields.push_back(line.substr(begin));
      break;
    }
    fields.push_back(line.substr(begin, separator - begin));
    begin = separator + 1U;
  }
  return fields;
}

template <typename Integer>
bool parse_integer(const std::string_view text, Integer &output) noexcept {
  const char *const begin = text.data();
  const char *const end = begin + text.size();
  const auto result = std::from_chars(begin, end, output);
  return result.ec == std::errc{} && result.ptr == end;
}

bool send_line(BattleRuntime &runtime, const std::string_view command,
               const std::initializer_list<std::string_view> fields = {}) {
  if (!runtime.connected || runtime.socket_handle == kInvalidSocket) {
    runtime.status = "Not connected to the recovery server.";
    return false;
  }
  std::string line{command};
  for (const std::string_view field : fields) {
    line.push_back('|');
    line += encode_field(field);
  }
  line.push_back('\n');
  std::size_t sent{};
  while (sent < line.size()) {
    const int result = send(socket_from(runtime), line.data() + sent,
                            static_cast<int>(line.size() - sent), 0);
    if (result > 0) {
      sent += static_cast<std::size_t>(result);
      continue;
    }
    const int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK) {
      fd_set writable{};
      FD_ZERO(&writable);
      FD_SET(socket_from(runtime), &writable);
      timeval timeout{0, 200000};
      if (select(0, nullptr, &writable, nullptr, &timeout) > 0) {
        continue;
      }
    }
    runtime.status = "The recovery server connection was lost.";
    SrvDisconnect(runtime);
    return false;
  }
  return true;
}

void set_status(BattleRuntime &runtime, const std::string_view text) {
  try {
    runtime.status.assign(text);
  } catch (...) {
    runtime.status.clear();
  }
}

void process_server_line(BattleRuntime &runtime,
                         const std::string_view line) {
  const std::vector<std::string_view> fields = split_fields(line);
  if (fields.empty()) {
    return;
  }
  const std::string_view command = fields[0];
  if (command == "WELCOME") {
    set_status(runtime, "Connected. Enter an account name and password.");
    (void)UiLogon(runtime);
  } else if (command == "ACCOUNT_CREATED") {
    set_status(runtime, "Account created. You can now log on.");
    runtime.confirm_password.clear();
    (void)UiLogon(runtime);
  } else if (command == "LOGON_OK" && fields.size() >= 2U) {
    runtime.account_name = decode_field(fields[1]);
    runtime.authenticated = true;
    runtime.password.clear();
    set_status(runtime, "Logon accepted. Requesting channels...");
    (void)SrvBeginChat(runtime);
  } else if (command == "CHANNEL_LIST_BEGIN") {
    runtime.channels.clear();
  } else if (command == "CHANNEL" && fields.size() >= 3U) {
    ChannelEntry entry{};
    entry.name = decode_field(fields[1]);
    (void)parse_integer(fields[2], entry.users);
    runtime.channels.push_back(std::move(entry));
  } else if (command == "CHANNEL_LIST_END") {
    runtime.selected_channel =
        (std::min)(runtime.selected_channel,
                   runtime.channels.empty() ? 0U
                                            : runtime.channels.size() - 1U);
    runtime.screen = BattleScreen::channel_select;
    runtime.edit_control = EditControl::none;
    set_status(runtime, "Select a channel.");
  } else if (command == "JOINED_CHANNEL" && fields.size() >= 2U) {
    ChatChannelJoined(runtime, decode_field(fields[1]));
  } else if (command == "USER_LIST_BEGIN") {
    runtime.users.clear();
  } else if (command == "USER" && fields.size() >= 2U) {
    ChatAddUser(runtime, decode_field(fields[1]));
  } else if (command == "USER_JOIN" && fields.size() >= 2U) {
    ChatAddUser(runtime, decode_field(fields[1]));
  } else if (command == "USER_LEAVE" && fields.size() >= 2U) {
    ChatDeleteUser(runtime, decode_field(fields[1]));
  } else if (command == "CHAT" && fields.size() >= 3U) {
    ChatReceiveMsg(runtime, decode_field(fields[1]), decode_field(fields[2]));
  } else if (command == "SYSTEM" && fields.size() >= 2U) {
    ChatReceiveMsg(runtime, "Battle.net", decode_field(fields[1]));
  } else if (command == "GAME_LIST_BEGIN") {
    runtime.games.clear();
  } else if (command == "GAME" && fields.size() >= 7U) {
    GameEntry game{};
    (void)parse_integer(fields[1], game.identifier);
    game.name = decode_field(fields[2]);
    game.host = decode_field(fields[3]);
    game.map = decode_field(fields[4]);
    (void)parse_integer(fields[5], game.players);
    (void)parse_integer(fields[6], game.maximum_players);
    runtime.games.push_back(std::move(game));
  } else if (command == "GAME_LIST_END") {
    runtime.selected_game =
        (std::min)(runtime.selected_game,
                   runtime.games.empty() ? 0U : runtime.games.size() - 1U);
    set_status(runtime, runtime.games.empty() ? "No advertised games."
                                             : "Select a game to join.");
  } else if ((command == "GAME_CREATED" || command == "JOINED_GAME") &&
             fields.size() >= 5U) {
    (void)parse_integer(fields[1], runtime.game_identifier);
    runtime.selected_map_name = decode_field(fields[3]);
    (void)parse_integer(fields[4], runtime.local_player_slot);
    runtime.game_host = command == "GAME_CREATED";
    runtime.game_started = false;
    runtime.game_aborted = false;
    runtime.lobby_players.clear();
    runtime.lobby_slots.clear();
    runtime.committed_turns.clear();
    runtime.outgoing_turns.clear();
    runtime.pending_game_lobby = true;
    runtime.pending_game_lobby_exit = false;
    set_status(runtime, command == "GAME_CREATED" ? "Game created."
                                                   : "Game joined.");
  } else if (command == "LOBBY_CLEAR") {
    runtime.lobby_players.clear();
    runtime.lobby_slots.clear();
  } else if (command == "LOBBY_SLOT" && fields.size() >= 4U) {
    LobbySlotEntry slot{};
    if (!parse_integer(fields[1], slot.slot) ||
        !parse_lobby_slot_kind(fields[2], slot.kind) ||
        !parse_integer(fields[3], slot.race) || slot.race >= 3U) {
      return;
    }
    const auto found = std::find_if(
        runtime.lobby_slots.begin(), runtime.lobby_slots.end(),
        [&slot](const LobbySlotEntry &candidate) {
          return candidate.slot == slot.slot;
        });
    if (found == runtime.lobby_slots.end()) {
      runtime.lobby_slots.push_back(slot);
    } else {
      *found = slot;
    }
  } else if (command == "LOBBY_PLAYER" && fields.size() >= 3U) {
    LobbyPlayer player{};
    (void)parse_integer(fields[1], player.slot);
    player.name = decode_field(fields[2]);
    const auto found = std::find_if(
        runtime.lobby_players.begin(), runtime.lobby_players.end(),
        [&player](const LobbyPlayer &candidate) {
          return candidate.slot == player.slot;
        });
    if (found == runtime.lobby_players.end()) {
      runtime.lobby_players.push_back(std::move(player));
    } else {
      *found = std::move(player);
    }
  } else if (command == "LOBBY_PLAYER_LEFT" && fields.size() >= 2U) {
    std::uint8_t slot{};
    if (parse_integer(fields[1], slot)) {
      runtime.lobby_players.erase(
          std::remove_if(runtime.lobby_players.begin(),
                         runtime.lobby_players.end(),
                         [slot](const LobbyPlayer &player) {
                           return player.slot == slot;
                         }),
          runtime.lobby_players.end());
    }
  } else if (command == "GAME_START" && fields.size() >= 5U) {
    (void)parse_integer(fields[1], runtime.game_identifier);
    runtime.selected_map_name = decode_field(fields[2]);
    (void)parse_integer(fields[3], runtime.game_seed);
    (void)parse_integer(fields[4], runtime.game_player_count);
    runtime.simulation_turn = 0U;
    runtime.next_turn_to_submit = 0U;
    runtime.committed_turns.clear();
    runtime.outgoing_turns.clear();
    runtime.game_started = true;
    runtime.game_aborted = false;
    runtime.pending_game_start = true;
    set_status(runtime, "The synchronized game is starting.");
  } else if (command == "TURN_COMMIT" && fields.size() >= 4U &&
             (fields.size() & 1U) == 0U) {
    CommittedTurn commit{};
    if (!parse_integer(fields[1], commit.turn)) {
      return;
    }
    try {
      for (std::size_t index = 2U; index + 1U < fields.size(); index += 2U) {
        PlayerTurnPayload player{};
        if (!parse_integer(fields[index], player.slot) ||
            !decode_hex(fields[index + 1U], player.payload)) {
          return;
        }
        commit.players.push_back(std::move(player));
      }
      runtime.committed_turns.push_back(std::move(commit));
    } catch (...) {
      set_status(runtime, "A synchronized turn could not be buffered.");
    }
  } else if (command == "LEFT_GAME") {
    runtime.game_identifier = 0U;
    runtime.game_host = false;
    runtime.game_started = false;
    runtime.game_aborted = false;
    runtime.pending_game_lobby = false;
    runtime.pending_game_start = false;
    runtime.lobby_players.clear();
    runtime.lobby_slots.clear();
    runtime.committed_turns.clear();
    runtime.outgoing_turns.clear();
    runtime.screen = BattleScreen::chat_room;
    runtime.edit_control = EditControl::chat_input;
    runtime.pending_game_lobby_exit = true;
    set_status(runtime, "Returned to the Battle.net channel.");
  } else if (command == "GAME_ABORTED") {
    const bool game_was_started = runtime.game_started;
    runtime.game_aborted = game_was_started;
    runtime.pending_game_start = false;
    if (!game_was_started) {
      runtime.game_identifier = 0U;
      runtime.game_host = false;
      runtime.game_started = false;
      runtime.pending_game_lobby = false;
      runtime.lobby_players.clear();
      runtime.lobby_slots.clear();
      runtime.committed_turns.clear();
      runtime.outgoing_turns.clear();
      runtime.screen = BattleScreen::chat_room;
      runtime.edit_control = EditControl::chat_input;
      runtime.pending_game_lobby_exit = true;
    }
    set_status(runtime, fields.size() >= 2U ? decode_field(fields[1])
                                           : "The network game ended.");
  } else if (command == "ERROR") {
    set_status(runtime, fields.size() >= 3U ? decode_field(fields[2])
                                           : "Server request failed.");
  }
}

bool local_server_endpoint(const BattleRuntime &runtime) noexcept {
  return runtime.server_host == "127.0.0.1" ||
         runtime.server_host == "localhost" || runtime.server_host == "::1";
}

SOCKET connect_endpoint(const BattleRuntime &runtime,
                        const long timeout_microseconds) noexcept {
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  char port[16]{};
  std::snprintf(port, sizeof(port), "%u",
                static_cast<unsigned>(runtime.server_port));
  addrinfo *addresses{};
  if (getaddrinfo(runtime.server_host.c_str(), port, &hints, &addresses) != 0) {
    return INVALID_SOCKET;
  }
  SOCKET connected = INVALID_SOCKET;
  for (addrinfo *address = addresses; address != nullptr;
       address = address->ai_next) {
    SOCKET candidate = socket(address->ai_family, address->ai_socktype,
                              address->ai_protocol);
    if (candidate == INVALID_SOCKET) continue;
    u_long nonblocking = 1U;
    (void)ioctlsocket(candidate, FIONBIO, &nonblocking);
    const int result = connect(candidate, address->ai_addr,
                               static_cast<int>(address->ai_addrlen));
    if (result == 0) {
      connected = candidate;
      break;
    }
    if (WSAGetLastError() == WSAEWOULDBLOCK) {
      fd_set writable{};
      FD_ZERO(&writable);
      FD_SET(candidate, &writable);
      timeval timeout{timeout_microseconds / 1000000L,
                      timeout_microseconds % 1000000L};
      if (select(0, nullptr, &writable, nullptr, &timeout) > 0) {
        int error{};
        int size = sizeof(error);
        if (getsockopt(candidate, SOL_SOCKET, SO_ERROR,
                       reinterpret_cast<char *>(&error), &size) == 0 &&
            error == 0) {
          connected = candidate;
          break;
        }
      }
    }
    closesocket(candidate);
  }
  freeaddrinfo(addresses);
  return connected;
}

bool launch_local_server() noexcept {
  std::array<wchar_t, 32768> module{};
  const DWORD length = GetModuleFileNameW(
      nullptr, module.data(), static_cast<DWORD>(module.size()));
  if (length == 0U || length >= module.size()) return false;
  std::wstring directory{module.data(), length};
  const std::size_t separator = directory.find_last_of(L"\\/");
  if (separator == std::wstring::npos) return false;
  directory.resize(separator);
  const std::wstring server =
      directory + L"\\StarCraftRecoveryServer.exe";
  if (GetFileAttributesW(server.c_str()) == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  std::wstring command = L"\"" + server + L"\"";
  STARTUPINFOW startup{};
  startup.cb = sizeof(startup);
  PROCESS_INFORMATION process{};
  if (CreateProcessW(server.c_str(), command.data(), nullptr, nullptr, FALSE,
                     CREATE_NO_WINDOW, nullptr, directory.c_str(), &startup,
                     &process) == FALSE) {
    return false;
  }
  CloseHandle(process.hThread);
  CloseHandle(process.hProcess);
  return true;
}

} // namespace

bool SrvInitialize(BattleRuntime &runtime) noexcept {
  if (runtime.winsock_ready) {
    return true;
  }
  WSADATA data{};
  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    set_status(runtime, "Windows sockets could not be initialized.");
    return false;
  }
  runtime.winsock_ready = true;
  return true;
}

void SrvDestroy(BattleRuntime &runtime) noexcept {
  SrvDisconnect(runtime);
  if (runtime.winsock_ready) {
    WSACleanup();
    runtime.winsock_ready = false;
  }
}

bool SrvConnectToServer(BattleRuntime &runtime) noexcept {
  if (runtime.connected) {
    return true;
  }
  if (!SrvInitialize(runtime)) {
    return false;
  }
  SOCKET connected = connect_endpoint(runtime, 1500000L);
  if (connected == INVALID_SOCKET && local_server_endpoint(runtime) &&
      !runtime.local_server_launch_attempted) {
    runtime.local_server_launch_attempted = true;
    if (launch_local_server()) {
      for (int attempt = 0; attempt < 20 && connected == INVALID_SOCKET;
           ++attempt) {
        Sleep(50U);
        connected = connect_endpoint(runtime, 100000L);
      }
    }
  }
  if (connected == INVALID_SOCKET) {
    set_status(runtime, "Could not connect to the recovery server at " +
                            runtime.server_host + ":" +
                            std::to_string(runtime.server_port) + ".");
    return false;
  }
  runtime.socket_handle = static_cast<std::uintptr_t>(connected);
  runtime.connected = true;
  runtime.authenticated = false;
  runtime.receive_buffer.clear();
  if (!send_line(runtime, "HELLO", {"RECOVERY-BNET/1", "StarCraftRecovery"})) {
    return false;
  }
  set_status(runtime, "Connected. Waiting for the recovery server...");
  return true;
}

void SrvDisconnect(BattleRuntime &runtime) noexcept {
  if (runtime.socket_handle != kInvalidSocket) {
    shutdown(socket_from(runtime), SD_BOTH);
    closesocket(socket_from(runtime));
  }
  runtime.socket_handle = kInvalidSocket;
  runtime.connected = false;
  runtime.authenticated = false;
  runtime.pending_game_lobby = false;
  runtime.pending_game_lobby_exit = false;
  runtime.pending_game_start = false;
  runtime.game_identifier = 0U;
  runtime.game_host = false;
  runtime.game_started = false;
  runtime.game_aborted = false;
  runtime.lobby_players.clear();
  runtime.lobby_slots.clear();
  runtime.committed_turns.clear();
  runtime.outgoing_turns.clear();
  runtime.receive_buffer.clear();
}

bool SrvIsConnected(const BattleRuntime &runtime) noexcept {
  return runtime.connected;
}

bool SrvCreateAccount(BattleRuntime &runtime, const std::string_view account,
                      const std::string_view password) noexcept {
  return send_line(runtime, "CREATE_ACCOUNT", {account, password});
}

bool SrvLogon(BattleRuntime &runtime, const std::string_view account,
              const std::string_view password) noexcept {
  return send_line(runtime, "LOGON", {account, password});
}

bool SrvBeginChat(BattleRuntime &runtime) noexcept {
  return runtime.authenticated && send_line(runtime, "LIST_CHANNELS");
}

bool SrvJoinChannel(BattleRuntime &runtime,
                    const std::string_view channel) noexcept {
  return runtime.authenticated && send_line(runtime, "JOIN_CHANNEL", {channel});
}

bool SrvSendChatString(BattleRuntime &runtime,
                       const std::string_view message) noexcept {
  return runtime.authenticated && send_line(runtime, "CHAT", {message});
}

bool SrvGetGameList(BattleRuntime &runtime) noexcept {
  return runtime.authenticated && send_line(runtime, "LIST_GAMES");
}

bool SrvStartAdvertisingGame(BattleRuntime &runtime,
                             const std::string_view name,
                             const std::string_view map,
                             const std::uint32_t maximum_players) noexcept {
  const std::string players = std::to_string(maximum_players);
  return runtime.authenticated &&
         send_line(runtime, "CREATE_GAME", {name, map, players});
}

bool SrvNotifyJoin(BattleRuntime &runtime,
                   const std::uint32_t game_identifier) noexcept {
  const std::string identifier = std::to_string(game_identifier);
  return runtime.authenticated && send_line(runtime, "JOIN_GAME", {identifier});
}

bool SrvSetLobbySlot(BattleRuntime &runtime, const std::uint8_t slot,
                     const LobbySlotKind kind,
                     const std::uint8_t race) noexcept {
  if (!runtime.authenticated || !runtime.game_host ||
      kind == LobbySlotKind::human || race >= 3U) {
    return false;
  }
  const std::string slot_text = std::to_string(slot);
  const std::string race_text = std::to_string(race);
  return send_line(runtime, "SET_SLOT",
                   {slot_text, lobby_slot_kind_name(kind), race_text});
}

bool SrvLeaveGame(BattleRuntime &runtime) noexcept {
  if (!runtime.authenticated || runtime.game_identifier == 0U) {
    return false;
  }
  if (!send_line(runtime, "LEAVE_GAME")) {
    return false;
  }
  set_status(runtime, "Leaving the game lobby...");
  return true;
}

bool SrvStartGame(BattleRuntime &runtime) noexcept {
  return runtime.authenticated && runtime.game_host &&
         send_line(runtime, "START_GAME");
}

bool SrvSubmitTurn(BattleRuntime &runtime, const std::uint32_t turn,
                   const std::vector<std::uint8_t> &payload) noexcept {
  if (!runtime.authenticated || !runtime.game_started ||
      payload.size() > 2048U) {
    return false;
  }
  try {
    const std::string turn_text = std::to_string(turn);
    const std::string payload_text = encode_hex(payload);
    return send_line(runtime, "TURN", {turn_text, payload_text});
  } catch (...) {
    set_status(runtime, "A synchronized turn could not be encoded.");
    return false;
  }
}

bool SrvTakeCommittedTurn(BattleRuntime &runtime, const std::uint32_t turn,
                          CommittedTurn &commit) noexcept {
  const auto found = std::find_if(
      runtime.committed_turns.begin(), runtime.committed_turns.end(),
      [turn](const CommittedTurn &candidate) {
        return candidate.turn == turn;
      });
  if (found == runtime.committed_turns.end()) {
    return false;
  }
  commit = std::move(*found);
  runtime.committed_turns.erase(found);
  return true;
}

bool SrvProcessClientReq(BattleRuntime &runtime) noexcept {
  if (!runtime.connected || runtime.socket_handle == kInvalidSocket) {
    return false;
  }
  bool changed{};
  std::array<char, 4096> buffer{};
  for (;;) {
    const int received = recv(socket_from(runtime), buffer.data(),
                              static_cast<int>(buffer.size()), 0);
    if (received > 0) {
      runtime.receive_buffer.append(buffer.data(),
                                    static_cast<std::size_t>(received));
      changed = true;
      if (runtime.receive_buffer.size() > kMaximumBufferedBytes) {
        set_status(runtime, "The recovery server sent an oversized response.");
        SrvDisconnect(runtime);
        return true;
      }
      continue;
    }
    if (received == 0) {
      set_status(runtime, "The recovery server closed the connection.");
      SrvDisconnect(runtime);
      return true;
    }
    const int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK) {
      set_status(runtime, "The recovery server connection was lost.");
      SrvDisconnect(runtime);
      return true;
    }
    break;
  }
  std::size_t newline{};
  while ((newline = runtime.receive_buffer.find('\n')) != std::string::npos) {
    std::string line = runtime.receive_buffer.substr(0U, newline);
    runtime.receive_buffer.erase(0U, newline + 1U);
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    process_server_line(runtime, line);
    changed = true;
  }
  return changed;
}

} // namespace starcraft::recovery::battle
