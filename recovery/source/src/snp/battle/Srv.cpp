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
             fields.size() >= 2U) {
    runtime.pending_game_lobby = true;
    set_status(runtime, command == "GAME_CREATED" ? "Game created."
                                                   : "Game joined.");
  } else if (command == "ERROR") {
    set_status(runtime, fields.size() >= 3U ? decode_field(fields[2])
                                           : "Server request failed.");
  }
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
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  char port[16]{};
  std::snprintf(port, sizeof(port), "%u",
                static_cast<unsigned>(runtime.server_port));
  addrinfo *addresses{};
  if (getaddrinfo(runtime.server_host.c_str(), port, &hints, &addresses) != 0) {
    set_status(runtime, "The recovery server address could not be resolved.");
    return false;
  }
  SOCKET connected = INVALID_SOCKET;
  for (addrinfo *address = addresses; address != nullptr;
       address = address->ai_next) {
    SOCKET candidate = socket(address->ai_family, address->ai_socktype,
                              address->ai_protocol);
    if (candidate == INVALID_SOCKET) {
      continue;
    }
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
      timeval timeout{1, 500000};
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
