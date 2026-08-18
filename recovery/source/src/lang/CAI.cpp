#include "starcraft/lang/CAI.h"

#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/count.hpp"
#include "starcraft/lang/cunit_init.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <new>
#include <string>
#include <utility>

namespace starcraft::recovery {
namespace {

constexpr std::size_t kLuaMemoryLimit = 4U * 1024U * 1024U;
constexpr int kLuaInstructionLimit = 100000;
constexpr int kLuaInstructionHookStep = 1000;
constexpr std::size_t kMaximumScriptBytes = 256U * 1024U;

struct LuaMemoryBudget {
  std::size_t used{};
  std::size_t limit{kLuaMemoryLimit};
};

struct LuaInstructionBudget {
  int remaining{kLuaInstructionLimit};
};

struct PendingAiGoals {
  std::array<AiBuildRequest, 64> requests{};
  std::size_t count{};
  bool attack{};
};

struct LuaTickContext {
  BootstrapStatus *status{};
  AiPlayerRuntime *player{};
  PendingAiGoals *goals{};
  std::uint32_t simulation_tick{};
};

void *lua_budget_allocator(void *const user_data, void *const pointer,
                           const std::size_t old_size,
                           const std::size_t new_size) noexcept {
  auto &budget = *static_cast<LuaMemoryBudget *>(user_data);
  if (new_size == 0U) {
    std::free(pointer);
    budget.used -= (std::min)(budget.used, old_size);
    return nullptr;
  }
  if (new_size > old_size &&
      new_size - old_size > budget.limit - (std::min)(budget.used, budget.limit)) {
    return nullptr;
  }
  void *const replacement = std::realloc(pointer, new_size);
  if (replacement == nullptr) {
    return nullptr;
  }
  if (new_size >= old_size) {
    budget.used += new_size - old_size;
  } else {
    budget.used -= (std::min)(budget.used, old_size - new_size);
  }
  return replacement;
}

void lua_instruction_hook(lua_State *const state, lua_Debug *) {
  auto *const budget = *static_cast<LuaInstructionBudget **>(
      lua_getextraspace(state));
  if (budget == nullptr) {
    return;
  }
  budget->remaining -= kLuaInstructionHookStep;
  if (budget->remaining <= 0) {
    luaL_error(state, "AI script instruction limit exceeded");
  }
}

LuaTickContext *tick_context(lua_State *const state) noexcept {
  return static_cast<LuaTickContext *>(lua_touserdata(state, lua_upvalueindex(1)));
}

std::size_t owned_count(const BootstrapStatus &status, const std::uint8_t owner,
                        const std::uint16_t unit_type,
                        const bool completed_only) noexcept {
  return static_cast<std::size_t>(std::count_if(
      status.units.begin(), status.units.end(),
      [=](const ScenarioUnitPreview &unit) {
        return unit.alive && unit.owner == owner && unit.unit_type == unit_type &&
               (!completed_only || unit.construction_complete);
      }));
}

std::size_t planned_count(const BootstrapStatus &status,
                          const std::uint8_t owner,
                          const std::uint16_t unit_type) noexcept {
  std::size_t count = owned_count(status, owner, unit_type, false);
  for (const ScenarioUnitPreview &producer : status.units) {
    if (!producer.alive || producer.owner != owner) {
      continue;
    }
    if (!producer.construction_complete &&
        producer.construction_target_type == unit_type) {
      ++count;
    }
    for (std::size_t index = 0; index < producer.production_queue.count();
         ++index) {
      count += producer.production_queue.at(index) == unit_type ? 1U : 0U;
    }
  }
  return count;
}

void emit_goal(PendingAiGoals &goals, const std::uint8_t kind,
               const std::uint16_t type, const std::uint8_t quantity,
               const std::uint8_t priority) noexcept {
  for (std::size_t index = 0; index < goals.count; ++index) {
    AiBuildRequest &request = goals.requests[index];
    if (request.kind == kind && request.unit_type == type) {
      request.quantity = (std::max)(request.quantity, quantity);
      request.priority = (std::max)(request.priority, priority);
      return;
    }
  }
  if (goals.count < goals.requests.size()) {
    goals.requests[goals.count++] = {type, quantity, priority, kind};
  }
}

int lua_ai_ensure(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  if (context == nullptr || context->goals == nullptr) {
    return luaL_error(state, "AI tick context is unavailable");
  }
  const lua_Integer type = luaL_checkinteger(state, 1);
  const lua_Integer quantity = luaL_checkinteger(state, 2);
  const lua_Integer priority = luaL_optinteger(state, 3, 50);
  if (type < 0 || type >= static_cast<lua_Integer>(starcraft::lang::kUnitTypeCount) ||
      quantity < 0 || quantity > 255 || priority < 0 || priority > 255) {
    return luaL_error(state, "ensure expects a valid unit, quantity, and priority");
  }
  emit_goal(*context->goals, 0U, static_cast<std::uint16_t>(type),
            static_cast<std::uint8_t>(quantity),
            static_cast<std::uint8_t>(priority));
  return 0;
}

int lua_ai_upgrade(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer type = luaL_checkinteger(state, 1);
  const lua_Integer quantity = luaL_optinteger(state, 2, 1);
  const lua_Integer priority = luaL_optinteger(state, 3, 35);
  if (context == nullptr || context->goals == nullptr || type < 0 || type >= 46 ||
      quantity < 0 || quantity > 255 || priority < 0 || priority > 255) {
    return luaL_error(state, "upgrade expects a valid upgrade, level, and priority");
  }
  emit_goal(*context->goals, 1U, static_cast<std::uint16_t>(type),
            static_cast<std::uint8_t>(quantity),
            static_cast<std::uint8_t>(priority));
  return 0;
}

int lua_ai_research(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer type = luaL_checkinteger(state, 1);
  const lua_Integer priority = luaL_optinteger(state, 2, 35);
  if (context == nullptr || context->goals == nullptr || type < 0 || type >= 28 ||
      priority < 0 || priority > 255) {
    return luaL_error(state, "research expects a valid technology and priority");
  }
  emit_goal(*context->goals, 2U, static_cast<std::uint16_t>(type), 1U,
            static_cast<std::uint8_t>(priority));
  return 0;
}

int lua_ai_attack(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  if (context == nullptr || context->goals == nullptr) {
    return luaL_error(state, "AI tick context is unavailable");
  }
  context->goals->attack = true;
  return 0;
}

int lua_ai_count(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer type = luaL_checkinteger(state, 1);
  if (context == nullptr || context->status == nullptr || context->player == nullptr ||
      type < 0 || type >= static_cast<lua_Integer>(starcraft::lang::kUnitTypeCount)) {
    lua_pushinteger(state, 0);
    return 1;
  }
  lua_pushinteger(state, static_cast<lua_Integer>(planned_count(
                             *context->status, context->player->owner,
                             static_cast<std::uint16_t>(type))));
  return 1;
}

int lua_ai_completed(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer type = luaL_checkinteger(state, 1);
  if (context == nullptr || context->status == nullptr || context->player == nullptr ||
      type < 0 || type >= static_cast<lua_Integer>(starcraft::lang::kUnitTypeCount)) {
    lua_pushinteger(state, 0);
    return 1;
  }
  lua_pushinteger(state, static_cast<lua_Integer>(owned_count(
                             *context->status, context->player->owner,
                             static_cast<std::uint16_t>(type), true)));
  return 1;
}

int lua_ai_minerals(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const std::uint32_t value =
      context != nullptr && context->status != nullptr && context->player != nullptr
          ? context->status->player_mineral_stock[context->player->owner]
          : 0U;
  lua_pushinteger(state, value);
  return 1;
}

int lua_ai_gas(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const std::uint32_t value =
      context != nullptr && context->status != nullptr && context->player != nullptr
          ? context->status->player_gas_stock[context->player->owner]
          : 0U;
  lua_pushinteger(state, value);
  return 1;
}

int lua_ai_supply_used(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const auto supply = context != nullptr && context->status != nullptr &&
                              context->player != nullptr
                          ? player_supply(*context->status,
                                          context->player->owner,
                                          context->player->race)
                          : std::array<std::uint32_t, 2>{{0U, 0U}};
  lua_pushinteger(state, (supply[0] + 1U) >> 1U);
  return 1;
}

int lua_ai_supply_cap(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const auto supply = context != nullptr && context->status != nullptr &&
                              context->player != nullptr
                          ? player_supply(*context->status,
                                          context->player->owner,
                                          context->player->race)
                          : std::array<std::uint32_t, 2>{{0U, 0U}};
  lua_pushinteger(state, (supply[1] + 1U) >> 1U);
  return 1;
}

int lua_ai_ensure_supply(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer display_buffer = luaL_optinteger(state, 1, 4);
  const lua_Integer priority = luaL_optinteger(state, 2, 125);
  if (context == nullptr || context->status == nullptr || context->player == nullptr ||
      context->goals == nullptr || display_buffer < 0 || display_buffer > 200 ||
      priority < 0 || priority > 255) {
    return luaL_error(state, "ensure_supply expects a valid buffer and priority");
  }
  constexpr std::array<std::uint16_t, 3> supply_types{{42U, 109U, 156U}};
  if (context->player->race >= supply_types.size()) {
    return 0;
  }
  const std::uint16_t supply_type = supply_types[context->player->race];
  const auto supply = player_supply(*context->status, context->player->owner,
                                    context->player->race);
  const std::uint32_t wanted =
      supply[0] + static_cast<std::uint32_t>(display_buffer) * 2U;
  if (wanted <= supply[1]) {
    return 0;
  }
  const std::uint32_t provider_supply =
      supply_type < context->status->unit_traits.size()
          ? context->status->unit_traits[supply_type].supply_provided
          : 0U;
  if (provider_supply == 0U) {
    return 0;
  }
  const std::size_t current = planned_count(*context->status,
                                            context->player->owner,
                                            supply_type);
  const std::size_t completed = owned_count(
      *context->status, context->player->owner, supply_type, true);
  const std::size_t pending = current > completed ? current - completed : 0U;
  const std::uint64_t future_cap =
      static_cast<std::uint64_t>(supply[1]) +
      static_cast<std::uint64_t>(pending) * provider_supply;
  if (static_cast<std::uint64_t>(wanted) <= future_cap) {
    return 0;
  }
  const std::uint32_t missing = static_cast<std::uint32_t>(
      static_cast<std::uint64_t>(wanted) - future_cap);
  const std::size_t additional =
      (missing + provider_supply - 1U) / provider_supply;
  emit_goal(*context->goals, 0U, supply_type,
            static_cast<std::uint8_t>((std::min)(
                current + additional, static_cast<std::size_t>(255U))),
            static_cast<std::uint8_t>(priority));
  return 0;
}

int lua_ai_combat_count(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  std::size_t count{};
  if (context != nullptr && context->status != nullptr &&
      context->player != nullptr) {
    count = static_cast<std::size_t>(std::count_if(
        context->status->units.begin(), context->status->units.end(),
        [context](const ScenarioUnitPreview &unit) {
          return unit.alive && unit.owner == context->player->owner &&
                 !unit.is_building &&
                 (unit.has_ground_weapon || unit.has_air_weapon) &&
                 (unit.dat_flags & 0x08U) == 0U;
        }));
  }
  lua_pushinteger(state, static_cast<lua_Integer>(count));
  return 1;
}

int lua_ai_visible_enemy_count(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  std::size_t count{};
  if (context != nullptr && context->status != nullptr &&
      context->player != nullptr) {
    count = static_cast<std::size_t>(std::count_if(
        context->status->units.begin(), context->status->units.end(),
        [context](const ScenarioUnitPreview &unit) {
          return unit.alive && unit.owner < 8U &&
                 unit.owner != context->player->owner &&
                 fog_unit_visible(*context->status, unit,
                                  context->player->owner);
        }));
  }
  lua_pushinteger(state, static_cast<lua_Integer>(count));
  return 1;
}

int lua_ai_enemy_race(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  lua_Integer result = -1;
  if (context != nullptr && context->status != nullptr &&
      context->player != nullptr) {
    for (std::size_t owner = 0; owner < 8U; ++owner) {
      if (owner != context->player->owner &&
          owner < context->status->active_players.size() &&
          context->status->active_players[owner]) {
        result = context->status->scenario.players()[owner].race;
        break;
      }
    }
  }
  lua_pushinteger(state, result);
  return 1;
}

int lua_ai_time(lua_State *const state) {
  const LuaTickContext *const context = tick_context(state);
  lua_pushinteger(state, context != nullptr ? context->simulation_tick : 0U);
  return 1;
}

int lua_ai_random(lua_State *const state) {
  LuaTickContext *const context = tick_context(state);
  const lua_Integer upper = luaL_checkinteger(state, 1);
  if (context == nullptr || context->status == nullptr || upper <= 0 ||
      static_cast<unsigned long long>(upper) > UINT32_MAX) {
    return luaL_error(state, "random expects a positive upper bound");
  }
  context->status->synchronized_random_state =
      22695477U * context->status->synchronized_random_state + 1U;
  const std::uint32_t value =
      (context->status->synchronized_random_state >> 16U) & 0x7FFFU;
  lua_pushinteger(state, value % static_cast<std::uint32_t>(upper));
  return 1;
}

void set_integer_field(lua_State *const state, const char *const name,
                       const lua_Integer value) {
  lua_pushinteger(state, value);
  lua_setfield(state, -2, name);
}

void set_named_integer_table(
    lua_State *const state, const char *const name,
    const std::initializer_list<std::pair<const char *, lua_Integer>> values) {
  lua_createtable(state, 0, static_cast<int>(values.size()));
  for (const auto &[entry_name, value] : values) {
    set_integer_field(state, entry_name, value);
  }
  lua_setfield(state, -2, name);
}

void install_constants(lua_State *const state) {
  lua_createtable(state, 0, 3);
  set_named_integer_table(state, "Terran",
      {
          {"Marine", 0U}, {"Ghost", 1U}, {"Vulture", 2U},
          {"Goliath", 3U}, {"SiegeTank", 5U}, {"SCV", 7U}, {"Wraith", 8U},
          {"ScienceVessel", 9U}, {"Dropship", 11U}, {"Battlecruiser", 12U},
          {"Firebat", 32U}, {"CommandCenter", 106U}, {"ComsatStation", 107U},
          {"NuclearSilo", 108U}, {"SupplyDepot", 109U}, {"Refinery", 110U},
          {"Barracks", 111U}, {"Academy", 112U}, {"Factory", 113U},
          {"Starport", 114U}, {"ControlTower", 115U}, {"ScienceFacility", 116U},
          {"CovertOps", 117U}, {"PhysicsLab", 118U}, {"MachineShop", 120U},
          {"EngineeringBay", 122U}, {"Armory", 123U}, {"MissileTurret", 124U},
          {"Bunker", 125U}});
  set_named_integer_table(state, "Zerg",
      {
          {"Larva", 35U}, {"Zergling", 37U},
          {"Hydralisk", 38U}, {"Ultralisk", 39U}, {"Drone", 41U},
          {"Overlord", 42U}, {"Mutalisk", 43U}, {"Guardian", 44U},
          {"Queen", 45U}, {"Defiler", 46U}, {"Scourge", 47U},
          {"Hatchery", 131U}, {"Lair", 132U}, {"Hive", 133U},
          {"NydusCanal", 134U}, {"HydraliskDen", 135U}, {"DefilerMound", 136U},
          {"GreaterSpire", 137U}, {"QueensNest", 138U},
          {"EvolutionChamber", 139U}, {"UltraliskCavern", 140U},
          {"Spire", 141U}, {"SpawningPool", 142U}, {"CreepColony", 143U},
          {"SporeColony", 144U}, {"SunkenColony", 146U}, {"Extractor", 149U}});
  set_named_integer_table(state, "Protoss",
      {
          {"Probe", 64U}, {"Zealot", 65U},
          {"Dragoon", 66U}, {"HighTemplar", 67U}, {"Archon", 68U},
          {"Shuttle", 69U}, {"Scout", 70U}, {"Arbiter", 71U},
          {"Carrier", 72U}, {"Reaver", 83U}, {"Observer", 84U},
          {"Nexus", 154U}, {"RoboticsFacility", 155U}, {"Pylon", 156U},
          {"Assimilator", 157U}, {"Observatory", 159U}, {"Gateway", 160U},
          {"ShieldBattery", 162U}, {"CitadelOfAdun", 163U},
          {"CyberneticsCore", 164U}, {"TemplarArchives", 165U}, {"Forge", 166U},
          {"Stargate", 167U}, {"FleetBeacon", 169U}, {"ArbiterTribunal", 170U},
          {"RoboticsSupportBay", 171U}, {"ShieldBatteryAlt", 172U}});
  lua_setglobal(state, "Unit");

  lua_createtable(state, 0, 16);
  set_integer_field(state, "StimPacks", 0);
  set_integer_field(state, "CloakingField", 1);
  set_integer_field(state, "EMPShockwave", 2);
  set_integer_field(state, "YamatoGun", 3);
  set_integer_field(state, "Irradiate", 6);
  set_integer_field(state, "Burrowing", 11);
  set_integer_field(state, "DarkSwarm", 15);
  set_integer_field(state, "Plague", 16);
  set_integer_field(state, "Consume", 17);
  set_integer_field(state, "Ensnare", 18);
  set_integer_field(state, "Parasite", 19);
  set_integer_field(state, "PsionicStorm", 22);
  set_integer_field(state, "Hallucination", 23);
  set_integer_field(state, "StasisField", 24);
  set_integer_field(state, "Recall", 25);
  set_integer_field(state, "ArchonWarp", 27);
  lua_setglobal(state, "Tech");

  lua_createtable(state, 0, 32);
  set_integer_field(state, "TerranInfantryArmor", 0);
  set_integer_field(state, "TerranVehiclePlating", 1);
  set_integer_field(state, "TerranShipPlating", 2);
  set_integer_field(state, "ZergCarapace", 3);
  set_integer_field(state, "ZergFlyerCarapace", 4);
  set_integer_field(state, "ProtossArmor", 5);
  set_integer_field(state, "ProtossPlating", 6);
  set_integer_field(state, "TerranInfantryWeapons", 7);
  set_integer_field(state, "TerranVehicleWeapons", 8);
  set_integer_field(state, "TerranShipWeapons", 9);
  set_integer_field(state, "ZergMelee", 10);
  set_integer_field(state, "ZergMissile", 11);
  set_integer_field(state, "ZergFlyer", 12);
  set_integer_field(state, "ProtossGroundWeapons", 13);
  set_integer_field(state, "ProtossAirWeapons", 14);
  set_integer_field(state, "ProtossPlasmaShields", 15);
  set_integer_field(state, "U238Shells", 16);
  set_integer_field(state, "MachineShopPrimary", 18);
  set_integer_field(state, "ZerglingSpeed", 27);
  set_integer_field(state, "ZerglingAttack", 28);
  set_integer_field(state, "HydraliskRange", 29);
  set_integer_field(state, "HydraliskSpeed", 30);
  set_integer_field(state, "DragoonRange", 33);
  set_integer_field(state, "ZealotSpeed", 34);
  set_integer_field(state, "ScarabDamage", 35);
  set_integer_field(state, "ReaverCapacity", 36);
  set_integer_field(state, "ShuttleSpeed", 37);
  set_integer_field(state, "ObserverSight", 38);
  set_integer_field(state, "ObserverSpeed", 39);
  set_integer_field(state, "TemplarEnergy", 40);
  set_integer_field(state, "CarrierCapacity", 43);
  set_integer_field(state, "ArbiterEnergy", 44);
  lua_setglobal(state, "Upgrade");
}

void install_ai_function(lua_State *const state, LuaTickContext *const context,
                         const char *const name, lua_CFunction function) {
  lua_pushlightuserdata(state, context);
  lua_pushcclosure(state, function, 1);
  lua_setfield(state, -2, name);
}

void push_ai_api(lua_State *const state, LuaTickContext &context) {
  lua_createtable(state, 0, 18);
  install_ai_function(state, &context, "ensure", lua_ai_ensure);
  install_ai_function(state, &context, "upgrade", lua_ai_upgrade);
  install_ai_function(state, &context, "research", lua_ai_research);
  install_ai_function(state, &context, "attack", lua_ai_attack);
  install_ai_function(state, &context, "count", lua_ai_count);
  install_ai_function(state, &context, "completed", lua_ai_completed);
  install_ai_function(state, &context, "minerals", lua_ai_minerals);
  install_ai_function(state, &context, "gas", lua_ai_gas);
  install_ai_function(state, &context, "supply_used", lua_ai_supply_used);
  install_ai_function(state, &context, "supply_cap", lua_ai_supply_cap);
  install_ai_function(state, &context, "ensure_supply", lua_ai_ensure_supply);
  install_ai_function(state, &context, "combat_count", lua_ai_combat_count);
  install_ai_function(state, &context, "visible_enemy_count",
                      lua_ai_visible_enemy_count);
  install_ai_function(state, &context, "enemy_race", lua_ai_enemy_race);
  install_ai_function(state, &context, "time", lua_ai_time);
  install_ai_function(state, &context, "random", lua_ai_random);
}

const char *race_script_name(const std::uint8_t race) noexcept {
  switch (race) {
  case 0U:
    return "zerg.lua";
  case 1U:
    return "terran.lua";
  case 2U:
    return "protoss.lua";
  default:
    return nullptr;
  }
}

bool read_script(const std::filesystem::path &path, std::string &output,
                 std::string &error) {
  std::ifstream input(path, std::ios::binary | std::ios::ate);
  if (!input) {
    error = "could not open " + path.generic_string();
    return false;
  }
  const std::streamoff length = input.tellg();
  if (length <= 0 || static_cast<std::uint64_t>(length) > kMaximumScriptBytes) {
    error = "AI script is empty, invalid, or too large: " + path.generic_string();
    return false;
  }
  output.resize(static_cast<std::size_t>(length));
  input.seekg(0, std::ios::beg);
  if (!output.empty()) {
    input.read(output.data(), static_cast<std::streamsize>(output.size()));
  }
  if (!input) {
    error = "could not read " + path.generic_string();
    return false;
  }
  return true;
}

} // namespace

struct CAI::Impl {
  std::uint8_t owner{};
  std::uint8_t race{};
  AiDifficulty difficulty{AiDifficulty::medium};
  std::filesystem::path script_root{};
  LuaMemoryBudget memory{};
  LuaInstructionBudget instructions{};
  LuaTickContext tick_context{};
  lua_State *state{};
  std::string error{};
  bool initialized{};

  ~Impl() {
    if (state != nullptr) {
      lua_close(state);
    }
  }

  bool run_file(const std::filesystem::path &path) {
    std::string source{};
    if (!read_script(path, source, error)) {
      return false;
    }
    if (luaL_loadbufferx(state, source.data(), source.size(),
                         path.generic_string().c_str(), "t") != LUA_OK) {
      error = lua_tostring(state, -1) != nullptr ? lua_tostring(state, -1)
                                                  : "Lua compile error";
      lua_pop(state, 1);
      return false;
    }
    instructions.remaining = kLuaInstructionLimit;
    lua_sethook(state, lua_instruction_hook, LUA_MASKCOUNT,
                kLuaInstructionHookStep);
    const int result = lua_pcall(state, 0, 0, 0);
    lua_sethook(state, nullptr, 0, 0);
    if (result != LUA_OK) {
      error = lua_tostring(state, -1) != nullptr ? lua_tostring(state, -1)
                                                  : "Lua initialization error";
      lua_pop(state, 1);
      return false;
    }
    return true;
  }
};

CAI::CAI(const std::uint8_t owner, const std::uint8_t race,
         const AiDifficulty difficulty, std::filesystem::path script_root)
    : impl_(std::make_unique<Impl>()) {
  impl_->owner = owner;
  impl_->race = race;
  impl_->difficulty = difficulty;
  impl_->script_root = std::move(script_root);
}

CAI::~CAI() = default;
CAI::CAI(CAI &&) noexcept = default;
CAI &CAI::operator=(CAI &&) noexcept = default;

bool CAI::initialize() noexcept {
  try {
    if (impl_ == nullptr) {
      return false;
    }
    impl_->error.clear();
    impl_->initialized = false;
    if (impl_->state != nullptr) {
      lua_close(impl_->state);
      impl_->state = nullptr;
      impl_->memory.used = 0U;
    }
    impl_->state = lua_newstate(lua_budget_allocator, &impl_->memory,
                                0x53434149U);
    if (impl_->state == nullptr) {
      impl_->error = "Lua state allocation failed";
      return false;
    }
    *static_cast<LuaInstructionBudget **>(lua_getextraspace(impl_->state)) =
        &impl_->instructions;
    luaL_openselectedlibs(impl_->state,
                          LUA_GLIBK | LUA_MATHLIBK | LUA_STRLIBK | LUA_TABLIBK,
                          0);
    for (const char *const unsafe : {"dofile", "loadfile", "load",
                                     "collectgarbage"}) {
      lua_pushnil(impl_->state);
      lua_setglobal(impl_->state, unsafe);
    }
    lua_getglobal(impl_->state, "math");
    if (lua_istable(impl_->state, -1)) {
      lua_pushnil(impl_->state);
      lua_setfield(impl_->state, -2, "random");
      lua_pushnil(impl_->state);
      lua_setfield(impl_->state, -2, "randomseed");
    }
    lua_pop(impl_->state, 1);
    lua_getglobal(impl_->state, "string");
    if (lua_istable(impl_->state, -1)) {
      lua_pushnil(impl_->state);
      lua_setfield(impl_->state, -2, "dump");
    }
    lua_pop(impl_->state, 1);
    install_constants(impl_->state);

    const std::filesystem::path common = impl_->script_root / "common.lua";
    const char *const race_file = race_script_name(impl_->race);
    if (race_file == nullptr || !impl_->run_file(common) ||
        !impl_->run_file(impl_->script_root /
                         ai_difficulty_name(impl_->difficulty) / race_file)) {
      return false;
    }
    lua_getglobal(impl_->state, "tick");
    const bool has_tick = lua_isfunction(impl_->state, -1) != 0;
    lua_pop(impl_->state, 1);
    if (!has_tick) {
      impl_->error = "AI script did not define tick(ai)";
      return false;
    }
    impl_->initialized = true;
    return true;
  } catch (const std::exception &exception) {
    if (impl_ != nullptr) {
      impl_->error = exception.what();
    }
    return false;
  } catch (...) {
    if (impl_ != nullptr) {
      impl_->error = "unknown Lua initialization failure";
    }
    return false;
  }
}

bool CAI::update(BootstrapStatus &status, AiPlayerRuntime &player,
                 const std::uint32_t simulation_tick) noexcept {
  if (impl_ == nullptr || !impl_->initialized || impl_->state == nullptr) {
    return false;
  }
  PendingAiGoals goals{};
  impl_->tick_context = {&status, &player, &goals, simulation_tick};
  lua_getglobal(impl_->state, "tick");
  if (!lua_isfunction(impl_->state, -1)) {
    lua_pop(impl_->state, 1);
    impl_->error = "tick(ai) is no longer callable";
    impl_->initialized = false;
    return false;
  }
  push_ai_api(impl_->state, impl_->tick_context);
  impl_->instructions.remaining = kLuaInstructionLimit;
  lua_sethook(impl_->state, lua_instruction_hook, LUA_MASKCOUNT,
              kLuaInstructionHookStep);
  const int result = lua_pcall(impl_->state, 1, 0, 0);
  lua_sethook(impl_->state, nullptr, 0, 0);
  if (result != LUA_OK) {
    impl_->error = lua_tostring(impl_->state, -1) != nullptr
                       ? lua_tostring(impl_->state, -1)
                       : "Lua tick failed";
    lua_pop(impl_->state, 1);
    impl_->initialized = false;
    impl_->tick_context = {};
    return false;
  }
  player.build_requests = goals.requests;
  player.build_request_count = goals.count;
  player.attack_requested = goals.attack;
  player.script_error.clear();
  impl_->tick_context = {};
  return true;
}

bool CAI::ready() const noexcept {
  return impl_ != nullptr && impl_->initialized;
}

const std::string &CAI::last_error() const noexcept {
  static const std::string empty{};
  return impl_ != nullptr ? impl_->error : empty;
}

AiDifficulty CAI::difficulty() const noexcept {
  return impl_ != nullptr ? impl_->difficulty : AiDifficulty::medium;
}

const char *ai_difficulty_name(const AiDifficulty value) noexcept {
  switch (value) {
  case AiDifficulty::easy:
    return "easy";
  case AiDifficulty::medium:
    return "medium";
  case AiDifficulty::hard:
    return "hard";
  default:
    return "medium";
  }
}

bool set_ai_difficulty(BootstrapStatus &status,
                       const AiDifficulty value) noexcept {
  status.ai_difficulty = value;
  const std::filesystem::path scripts = locate_input_root() / "scripts" / "ai";
  bool ready = true;
  for (AiPlayerRuntime &player : status.ai_players) {
    if (!player.enabled) {
      continue;
    }
    try {
      player.controller =
          std::make_shared<CAI>(player.owner, player.race, value, scripts);
      player.script_active = player.controller->initialize();
      player.script_error = player.script_active
                                ? std::string{}
                                : player.controller->last_error();
    } catch (...) {
      player.controller.reset();
      player.script_active = false;
      player.script_error = "Lua controller allocation failed";
    }
    player.macro_update_ticks = 0U;
    ready = ready && player.script_active;
  }
  return ready;
}

} // namespace starcraft::recovery
