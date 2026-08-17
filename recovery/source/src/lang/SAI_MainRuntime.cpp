#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_zerg.hpp"
#include "starcraft/lang/place_unit.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace starcraft::recovery {
namespace {

std::uint16_t ai_u16(const std::vector<std::uint8_t> &bytes,
                     const std::size_t offset) noexcept {
  return offset + 2U <= bytes.size()
             ? static_cast<std::uint16_t>(bytes[offset]) |
                   static_cast<std::uint16_t>(bytes[offset + 1U] << 8U)
             : 0U;
}

std::uint32_t ai_u32(const std::vector<std::uint8_t> &bytes,
                     const std::size_t offset) noexcept {
  return offset + 4U <= bytes.size()
             ? static_cast<std::uint32_t>(bytes[offset]) |
                   (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U) |
                   (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U) |
                   (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U)
             : 0U;
}

std::uint32_t race_script_id(const std::uint8_t race) noexcept {
  // The supplied beta scripts/aiscript.bin headers are Pro1, Ter1 and Zer1.
  // SAI_Scripts.cpp::sub_49A010 compares these four bytes as a dword.
  constexpr std::array<std::uint32_t, 3> ids{{
      0x3172655AU,  // "Zer1"
      0x31726554U,  // "Ter1"
      0x316F7250U,  // "Pro1"
  }};
  return race < ids.size() ? ids[race] : 0U;
}

std::uint32_t find_script(const std::vector<std::uint8_t> &bytes,
                          const std::uint32_t id) noexcept {
  // SAI_Scripts.cpp::sub_49A010 walks sixteen-byte headers until ID zero and
  // takes the program offset from header +4.
  for (std::size_t offset = 0; offset + 16U <= bytes.size(); offset += 16U) {
    const std::uint32_t candidate = ai_u32(bytes, offset);
    if (candidate == 0U) {
      break;
    }
    if (candidate == id) {
      const std::uint32_t program = ai_u32(bytes, offset + 4U);
      return program < bytes.size() ? program : 0U;
    }
  }
  return 0U;
}

std::size_t owned_count(const BootstrapStatus &status, const std::uint8_t owner,
                        const std::uint16_t unit_type) noexcept {
  return static_cast<std::size_t>(std::count_if(
      status.units.begin(), status.units.end(),
      [owner, unit_type](const ScenarioUnitPreview &unit) {
        return unit.alive && unit.owner == owner && unit.unit_type == unit_type;
      }));
}

std::size_t planned_count(const BootstrapStatus &status,
                          const std::uint8_t owner,
                          const std::uint16_t unit_type) noexcept {
  std::size_t count = owned_count(status, owner, unit_type);
  for (const ScenarioUnitPreview &producer : status.units) {
    if (!producer.alive || producer.owner != owner) {
      continue;
    }
    for (std::size_t queued = 0; queued < producer.production_queue.count();
         ++queued) {
      count += producer.production_queue.at(queued) == unit_type ? 1U : 0U;
    }
  }
  return count;
}

std::uint16_t race_equivalent(const std::uint8_t race,
                              const std::uint16_t terran_type) noexcept {
  if (race == 1U) {
    return terran_type;
  }
  // The supplied beta aiscript headers Pro1/Ter1/Zer1 all point at the same
  // Ter1 program. The native SAI build machinery is race-generic, so route
  // that aliased program through the exact race unit IDs recovered from the
  // command cards and SAI_Build's race branches instead of silently dropping
  // every non-Terran request.
  switch (terran_type) {
    case 106U: return race == 0U ? 131U : 154U;  // town hall
    case 7U: return race == 0U ? 41U : 64U;      // worker
    case 109U: return race == 0U ? 42U : 156U;   // supply provider
    case 110U: return race == 0U ? 149U : 157U;  // gas structure
    case 111U: return race == 0U ? 142U : 160U;  // first tech/production
    case 112U: return race == 0U ? 143U : 162U;  // basic defense
    case 113U: return race == 0U ? 135U : 155U;  // second production
    case 114U: return race == 0U ? 141U : 167U;  // air production
    case 122U: return race == 0U ? 139U : 166U;  // weapon upgrades
    case 0U: return race == 0U ? 37U : 65U;      // first combat unit
    case 32U: return race == 0U ? 38U : 66U;     // second combat unit
    case 8U: return race == 0U ? 43U : 70U;      // first air unit
    default: return terran_type;
  }
}

bool unit_belongs_to_race(const std::uint16_t unit_type,
                          const std::uint8_t race) noexcept {
  switch (race) {
    case 0U:
      return (unit_type >= 35U && unit_type <= 63U) ||
             (unit_type >= 131U && unit_type <= 153U);
    case 1U:
      return unit_type <= 34U ||
             (unit_type >= 106U && unit_type <= 130U);
    case 2U:
      return (unit_type >= 64U && unit_type <= 105U) ||
             (unit_type >= 154U && unit_type <= 175U);
    default:
      return false;
  }
}

void remember_build_request(AiPlayerRuntime &ai, const std::uint8_t kind,
                            const std::uint8_t quantity,
                            const std::uint16_t unit_type,
                            const std::uint8_t priority) noexcept {
  const std::uint16_t normalized_type =
      kind == 0U ? race_equivalent(ai.race, unit_type) : unit_type;
  for (std::size_t index = 0; index < ai.build_request_count; ++index) {
    AiBuildRequest &request = ai.build_requests[index];
    if (request.kind == kind && request.unit_type == normalized_type) {
      request.quantity = (std::max)(request.quantity, quantity);
      request.priority = priority;
      return;
    }
  }
  if (ai.build_request_count < ai.build_requests.size()) {
    ai.build_requests[ai.build_request_count++] =
        {normalized_type, quantity, priority, kind};
  }
}

bool advance_ai_script(BootstrapStatus &status, AiPlayerRuntime &ai) noexcept {
  if (!ai.script_active || status.ai_script_bytes.empty()) {
    return false;
  }
  if (ai.sleep_ticks != 0U) {
    --ai.sleep_ticks;
    return false;
  }
  const auto &bytes = status.ai_script_bytes;
  bool changed{};
  for (std::size_t instruction = 0; instruction < 128U; ++instruction) {
    if (ai.script_pc >= bytes.size()) {
      ai.script_active = false;
      return changed;
    }
    const std::uint32_t opcode_pc = ai.script_pc;
    const std::uint8_t opcode = bytes[ai.script_pc++];
    switch (opcode) {
      case 0: {  // goto
        const std::uint16_t target = ai_u16(bytes, ai.script_pc);
        if (target >= bytes.size()) ai.script_active = false;
        else ai.script_pc = target;
        break;
      }
      case 1: {  // notowns_jump
        if (ai.script_pc + 4U > bytes.size()) {
          ai.script_active = false;
          break;
        }
        const std::uint16_t unit_type = ai_u16(bytes, ai.script_pc);
        const std::uint16_t target = ai_u16(bytes, ai.script_pc + 2U);
        ai.script_pc += 4U;
        if (owned_count(status, ai.owner, unit_type) == 0U) {
          ai.script_pc = target;
        }
        break;
      }
      case 2:  // wait
        ai.sleep_ticks = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        return true;
      case 3:  // start_town
        changed = true;
        break;
      case 4:  // start_areatown, u16 search selector
        ai.script_pc += 2U;
        changed = true;
        break;
      case 5: {  // build qty, unit, priority
        if (ai.script_pc + 4U > bytes.size()) {
          ai.script_active = false;
          break;
        }
        const std::uint8_t quantity = bytes[ai.script_pc++];
        const std::uint16_t unit_type = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        const std::uint8_t priority = bytes[ai.script_pc++];
        remember_build_request(ai, 0U, quantity, unit_type, priority);
        changed = true;
        break;
      }
      case 6: {  // upgrade qty, upgrade
        const std::uint8_t quantity = bytes[ai.script_pc++];
        const std::uint16_t upgrade = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        remember_build_request(ai, 1U, quantity, upgrade, 30U);
        changed = true;
        break;
      }
      case 7: {  // technology
        const std::uint16_t technology = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        remember_build_request(ai, 2U, 1U, technology, 30U);
        changed = true;
        break;
      }
      case 8: {  // wait_build
        const std::uint8_t quantity = bytes[ai.script_pc++];
        const std::uint16_t unit_type = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        if (owned_count(status, ai.owner, unit_type) < quantity) {
          ai.sleep_ticks = 50U;
          ai.script_pc = opcode_pc;
          return changed;
        }
        break;
      }
      case 10: {  // attack_add
        const std::uint8_t quantity = bytes[ai.script_pc++];
        const std::uint16_t unit_type = ai_u16(bytes, ai.script_pc);
        ai.script_pc += 2U;
        remember_build_request(ai, 0U, quantity, unit_type, 60U);
        changed = true;
        break;
      }
      case 17: case 18: case 19: case 20:
      case 21: case 22: case 23: case 24:
        // The eight defense opcodes each consume byte quantity/u16 type and
        // feed the original defense tables at sub_484480..sub_484960.
        ai.script_pc += 3U;
        break;
      case 9:  // attack_clear
        ai.attack_requested = false;
        break;
      case 14:  // attack_do
      case 15:  // suicide
        ai.attack_requested = true;
        changed = true;
        break;
      case 11: case 12: case 13: case 16:
      case 25: case 26: case 27: case 28:
        break;
      default:
        ai.script_active = false;
        return changed;
    }
  }
  return changed;
}

ScenarioUnitPreview *idle_worker(BootstrapStatus &status,
                                 const std::uint8_t owner) noexcept {
  starcraft::lang::MeleeUnitTypes types{};
  if (!starcraft::lang::melee_unit_types(status.ai_players[owner].race, types)) {
    return nullptr;
  }
  for (ScenarioUnitPreview &unit : status.units) {
    if (unit.alive && unit.owner == owner && unit.unit_type == types.worker &&
        unit.active_order == ActiveUnitOrder::none && !unit.moving) {
      return &unit;
    }
  }
  return nullptr;
}

bool assign_ai_harvest(BootstrapStatus &status, AiPlayerRuntime &ai) noexcept {
  bool changed{};
  bool reserve_builder{};
  bool gas_needed{};
  for (std::size_t index = 0; index < ai.build_request_count; ++index) {
    const AiBuildRequest &request = ai.build_requests[index];
    if (request.kind == 0U && unit_belongs_to_race(request.unit_type, ai.race) &&
        request.unit_type < status.runtime_unit_types.size() &&
        status.runtime_unit_types[request.unit_type].ready &&
        status.runtime_unit_types[request.unit_type].initialization.is_building &&
        owned_count(status, ai.owner, request.unit_type) < request.quantity) {
      reserve_builder = true;
    }
    if (request.kind == 0U && request.unit_type < status.runtime_unit_types.size() &&
        status.runtime_unit_types[request.unit_type].ready &&
        status.player_gas_stock[ai.owner] <
            status.runtime_unit_types[request.unit_type]
                .initialization.simulation.gas_cost) {
      gas_needed = true;
    }
  }
  std::size_t gas_workers = static_cast<std::size_t>(std::count_if(
      status.units.begin(), status.units.end(), [&status, &ai](const auto &unit) {
        if (!unit.alive || unit.owner != ai.owner ||
            unit.active_order != ActiveUnitOrder::gather) {
          return false;
        }
        const ScenarioUnitPreview *const source =
            find_unit_by_id(status, unit.harvest_source_id);
        return source != nullptr &&
               (source->unit_type == 110U || source->unit_type == 149U ||
                source->unit_type == 157U);
      }));
  for (ScenarioUnitPreview &worker : status.units) {
    starcraft::lang::MeleeUnitTypes types{};
    if (!starcraft::lang::melee_unit_types(ai.race, types) || !worker.alive ||
        worker.owner != ai.owner || worker.unit_type != types.worker ||
        worker.active_order != ActiveUnitOrder::none || worker.moving) {
      continue;
    }
    if (reserve_builder) {
      reserve_builder = false;
      continue;
    }
    const ScenarioUnitPreview *nearest{};
    std::uint64_t nearest_distance = UINT64_MAX;
    const bool seek_gas = gas_needed && gas_workers < 3U;
    for (const ScenarioUnitPreview &resource : status.units) {
      const bool mineral =
          resource.unit_type >= 176U && resource.unit_type <= 178U;
      const bool refinery = resource.owner == ai.owner &&
                            (resource.unit_type == 110U ||
                             resource.unit_type == 149U ||
                             resource.unit_type == 157U);
      if (!resource.alive || resource.resource_amount == 0U ||
          (seek_gas ? !refinery : !mineral)) {
        continue;
      }
      const std::int64_t dx = static_cast<int>(resource.x) - worker.x;
      const std::int64_t dy = static_cast<int>(resource.y) - worker.y;
      const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
      if (distance < nearest_distance) {
        nearest_distance = distance;
        nearest = &resource;
      }
    }
    if (nearest != nullptr && begin_scv_interaction(
                                  status, worker, *nearest,
                                  ActiveUnitOrder::gather)) {
      gas_workers += seek_gas ? 1U : 0U;
      changed = true;
    }
  }
  return changed;
}

bool find_ai_build_site(const BootstrapStatus &status,
                        const BuildableUnitVisual &buildable,
                        const std::uint8_t owner, std::uint16_t &output_x,
                        std::uint16_t &output_y) noexcept {
  if (buildable.unit_type == 110U || buildable.unit_type == 149U ||
      buildable.unit_type == 157U) {
    for (const ScenarioUnitPreview &geyser : status.units) {
      if (geyser.alive && geyser.unit_type == 188U &&
          placement_is_valid(status, buildable, geyser.x, geyser.y, owner)) {
        output_x = geyser.x;
        output_y = geyser.y;
        return true;
      }
    }
    return false;
  }
  const ScenarioUnitPreview *anchor{};
  for (const ScenarioUnitPreview &unit : status.units) {
    if (unit.alive && unit.owner == owner && unit.is_building) {
      anchor = &unit;
      break;
    }
  }
  if (anchor == nullptr) return false;
  const int half_width = buildable.placement_width / 2;
  const int half_height = buildable.placement_height / 2;
  for (int radius = 3; radius <= 15; ++radius) {
    for (int edge = -radius; edge <= radius; ++edge) {
      const std::array<std::array<int, 2>, 4> points{{
          {{edge, -radius}}, {{radius, edge}}, {{-edge, radius}}, {{-radius, -edge}},
      }};
      for (const auto &point : points) {
        const int left = ((static_cast<int>(anchor->x) + point[0] * 32 -
                           half_width + 16) /
                          32) *
                         32;
        const int top = ((static_cast<int>(anchor->y) + point[1] * 32 -
                          half_height + 16) /
                         32) *
                        32;
        const int center_x = left + half_width;
        const int center_y = top + half_height;
        if (center_x < 0 || center_y < 0 || center_x > UINT16_MAX ||
            center_y > UINT16_MAX) {
          continue;
        }
        if (placement_is_valid(status, buildable,
                               static_cast<std::uint16_t>(center_x),
                               static_cast<std::uint16_t>(center_y), owner)) {
          output_x = static_cast<std::uint16_t>(center_x);
          output_y = static_cast<std::uint16_t>(center_y);
          return true;
        }
      }
    }
  }
  return false;
}

bool start_ai_building(BootstrapStatus &status, AiPlayerRuntime &ai,
                       const BuildableUnitVisual &buildable) noexcept {
  ScenarioUnitPreview *worker = idle_worker(status, ai.owner);
  if (worker == nullptr) return false;
  std::uint16_t x{};
  std::uint16_t y{};
  if (!find_ai_build_site(status, buildable, ai.owner, x, y)) return false;
  if (ai.race == 2U) {
    return begin_protoss_build_order(status, *worker, buildable, x, y, true);
  }
  std::uint32_t &minerals = status.player_mineral_stock[ai.owner];
  std::uint32_t &gas = status.player_gas_stock[ai.owner];
  if (minerals < buildable.simulation.mineral_cost ||
      gas < buildable.simulation.gas_cost) {
    return false;
  }
  if (ai.race == 0U) {
    cancel_unit_order(status, *worker);
    if (!configure_preview_type(status, *worker, buildable.unit_type)) return false;
    minerals -= buildable.simulation.mineral_cost;
    gas -= buildable.simulation.gas_cost;
    if (buildable.unit_type == 149U) {
      for (ScenarioUnitPreview &geyser : status.units) {
        if (geyser.alive && geyser.unit_type == 188U &&
            std::abs(static_cast<int>(geyser.x) - x) <= 16 &&
            std::abs(static_cast<int>(geyser.y) - y) <= 16) {
          worker->resource_amount = geyser.resource_amount;
          geyser.alive = false;
          geyser.selected = false;
          break;
        }
      }
    }
    worker->x = x;
    worker->y = y;
    worker->x_fixed = static_cast<std::int32_t>(x) << 8U;
    worker->y_fixed = static_cast<std::int32_t>(y) << 8U;
    worker->construction_complete = false;
    worker->construction_ticks_total = static_cast<std::uint16_t>((
        std::max)(1U, static_cast<unsigned>(buildable.simulation.build_time) >> 1U));
    worker->construction_ticks_remaining = worker->construction_ticks_total;
    worker->hit_points = (std::max)(1U, worker->max_hit_points / 10U);
    worker->construction_animation_phase = 0U;
    return true;
  }
  const std::uint32_t worker_id = worker->unit_id;
  ScenarioUnitPreview building{};
  building.unit_id = status.next_unit_id++;
  building.owner = ai.owner;
  if (!configure_preview_type(status, building, buildable.unit_type)) return false;
  minerals -= buildable.simulation.mineral_cost;
  gas -= buildable.simulation.gas_cost;
  building.x = x;
  building.y = y;
  building.x_fixed = static_cast<std::int32_t>(x) << 8U;
  building.y_fixed = static_cast<std::int32_t>(y) << 8U;
  building.construction_complete = false;
  building.construction_ticks_total = static_cast<std::uint16_t>((
      std::max)(1U, static_cast<unsigned>(buildable.simulation.build_time) >> 1U));
  building.construction_ticks_remaining = building.construction_ticks_total;
  building.hit_points = (std::max)(1U, building.max_hit_points / 10U);
  building.construction_builder_id = worker_id;
  if (buildable.unit_type == 110U) {
    for (ScenarioUnitPreview &geyser : status.units) {
      if (geyser.alive && geyser.unit_type == 188U &&
          std::abs(static_cast<int>(geyser.x) - x) <= 16 &&
          std::abs(static_cast<int>(geyser.y) - y) <= 16) {
        building.resource_amount = geyser.resource_amount;
        geyser.alive = false;
        geyser.selected = false;
        break;
      }
    }
  }
  const std::uint32_t building_id = building.unit_id;
  status.units.push_back(std::move(building));
  worker = find_unit_by_id(status, worker_id);
  ScenarioUnitPreview *created = find_unit_by_id(status, building_id);
  return worker != nullptr && created != nullptr &&
         begin_scv_interaction(status, *worker, *created,
                               ActiveUnitOrder::construct);
}

bool satisfy_ai_build_request(BootstrapStatus &status, AiPlayerRuntime &ai,
                              const std::uint32_t now) noexcept {
  std::array<std::size_t, 64> request_order{};
  for (std::size_t index = 0; index < request_order.size(); ++index) {
    request_order[index] = index;
  }
  std::stable_sort(request_order.begin(),
                   request_order.begin() + ai.build_request_count,
                   [&ai](const std::size_t left, const std::size_t right) {
                     return ai.build_requests[left].priority >
                            ai.build_requests[right].priority;
                   });
  for (std::size_t order = 0; order < ai.build_request_count; ++order) {
    const AiBuildRequest &request = ai.build_requests[request_order[order]];
    if (request.kind != 0U || !unit_belongs_to_race(request.unit_type, ai.race) ||
        request.unit_type >= status.runtime_unit_types.size() ||
        planned_count(status, ai.owner, request.unit_type) >= request.quantity) {
      continue;
    }
    const RuntimeUnitType &runtime = status.runtime_unit_types[request.unit_type];
    if (!runtime.ready) continue;
    if (runtime.initialization.is_building) {
      const BuildableUnitVisual *const buildable =
          find_buildable_unit(status, request.unit_type);
      if (buildable == nullptr) {
        continue;
      }
      const auto &simulation = runtime.initialization.simulation;
      if (status.player_mineral_stock[ai.owner] < simulation.mineral_cost ||
          status.player_gas_stock[ai.owner] < simulation.gas_cost) {
        return false;
      }
      return start_ai_building(status, ai, *buildable);
    }
    for (ScenarioUnitPreview &producer : status.units) {
      if (!producer.alive || producer.owner != ai.owner ||
          !producer.construction_complete || producer.production_queue.full() ||
          !starcraft::lang::producer_builds_unit(producer.unit_type,
                                                request.unit_type)) {
        continue;
      }
      const auto &simulation = runtime.initialization.simulation;
      std::uint32_t &minerals = status.player_mineral_stock[ai.owner];
      std::uint32_t &gas = status.player_gas_stock[ai.owner];
      if (minerals < simulation.mineral_cost || gas < simulation.gas_cost ||
          !producer.production_queue.enqueue(request.unit_type)) {
        return false;
      }
      minerals -= simulation.mineral_cost;
      gas -= simulation.gas_cost;
      if (!producer.production_active) {
        producer.production_active = true;
        producer.production_started = now;
        producer.production_kind =
            producer.unit_type == starcraft::lang::zerg_larva_type
                ? starcraft::lang::UnitProductionKind::zerg_larva_morph
                : starcraft::lang::UnitProductionKind::train;
        if (producer.production_kind ==
            starcraft::lang::UnitProductionKind::zerg_larva_morph) {
          (void)configure_preview_type(status, producer,
                                       starcraft::lang::zerg_egg_type);
        } else {
          (void)restart_unit_animation(status, producer, 19U);
        }
      }
      return true;
    }
  }
  return false;
}

bool satisfy_ai_research_request(BootstrapStatus &status,
                                 AiPlayerRuntime &ai) noexcept {
  for (std::size_t index = 0; index < ai.build_request_count; ++index) {
    const AiBuildRequest &request = ai.build_requests[index];
    if (request.kind == 1U) {
      if (request.unit_type >= status.upgrade_traits.size() ||
          ai.owner >= status.player_upgrade_levels.size()) {
        continue;
      }
      const std::size_t upgrade = request.unit_type;
      const starcraft::data::UpgradeResearchTraits &traits =
          status.upgrade_traits[upgrade];
      const std::uint32_t level =
          status.player_upgrade_levels[ai.owner][upgrade];
      if (level >= request.quantity || level >= traits.maximum_level) {
        continue;
      }
      for (ScenarioUnitPreview &building : status.units) {
        if (!building.alive || building.owner != ai.owner ||
            !building.is_building || !building.construction_complete ||
            building.active_technology != 28U ||
            building.active_upgrade != 46U) {
          continue;
        }
        const CommandCardView card = recovered_building_card(building.unit_type);
        const bool supported =
            card.count != 0U &&
            std::any_of(card.buttons, card.buttons + card.count,
                        [upgrade](const CommandButtonVisual &button) {
                          return button.action ==
                                     CommandButtonVisual::Action::
                                         upgrade_technology &&
                                 button.argument == upgrade;
                        });
        if (!supported) continue;
        const std::uint32_t minerals =
            traits.mineral_cost + level * traits.mineral_factor;
        const std::uint32_t gas = traits.gas_cost + level * traits.gas_factor;
        if (status.player_mineral_stock[ai.owner] < minerals ||
            status.player_gas_stock[ai.owner] < gas) {
          return false;
        }
        status.player_mineral_stock[ai.owner] -= minerals;
        status.player_gas_stock[ai.owner] -= gas;
        building.active_upgrade = static_cast<std::uint8_t>(upgrade);
        building.technology_ticks_total = static_cast<std::uint16_t>((
            std::max)(1U, (traits.research_time +
                           level * traits.time_factor) >>
                              1U));
        building.technology_ticks_remaining = building.technology_ticks_total;
        (void)restart_unit_animation(status, building, 19U);
        return true;
      }
    } else if (request.kind == 2U) {
      if (request.unit_type >= status.technology_traits.size() ||
          ai.owner >= status.player_researched_technologies.size() ||
          status.player_researched_technologies[ai.owner]
                                                 [request.unit_type]) {
        continue;
      }
      const std::size_t technology = request.unit_type;
      const starcraft::data::TechnologyResearchTraits &traits =
          status.technology_traits[technology];
      for (ScenarioUnitPreview &building : status.units) {
        if (!building.alive || building.owner != ai.owner ||
            !building.is_building || !building.construction_complete ||
            building.active_technology != 28U ||
            building.active_upgrade != 46U) {
          continue;
        }
        const CommandCardView card = recovered_building_card(building.unit_type);
        const bool supported =
            card.count != 0U &&
            std::any_of(card.buttons, card.buttons + card.count,
                        [technology](const CommandButtonVisual &button) {
                          return button.action ==
                                     CommandButtonVisual::Action::
                                         research_technology &&
                                 button.argument == technology;
                        });
        if (!supported) continue;
        if (status.player_mineral_stock[ai.owner] < traits.mineral_cost ||
            status.player_gas_stock[ai.owner] < traits.gas_cost) {
          return false;
        }
        status.player_mineral_stock[ai.owner] -= traits.mineral_cost;
        status.player_gas_stock[ai.owner] -= traits.gas_cost;
        building.active_technology =
            static_cast<std::uint8_t>(technology);
        building.technology_ticks_total = static_cast<std::uint16_t>((
            std::max)(1U,
                      static_cast<unsigned>(traits.research_time) >> 1U));
        building.technology_ticks_remaining = building.technology_ticks_total;
        (void)restart_unit_animation(status, building, 19U);
        return true;
      }
    }
  }
  return false;
}

bool issue_ai_attack(BootstrapStatus &status, AiPlayerRuntime &ai) noexcept {
  bool changed{};
  for (std::size_t index = 0; index < status.units.size(); ++index) {
    ScenarioUnitPreview &attacker = status.units[index];
    if (!attacker.alive || attacker.owner != ai.owner || attacker.is_building ||
        !attacker.has_ground_weapon || attacker.active_order != ActiveUnitOrder::none) {
      continue;
    }
    const ScenarioUnitPreview *nearest{};
    std::uint64_t nearest_distance = UINT64_MAX;
    for (const ScenarioUnitPreview &target : status.units) {
      if (!target.alive || target.owner == ai.owner || target.owner >= 8U ||
          target.unit_id == attacker.unit_id) continue;
      const std::int64_t dx = static_cast<int>(target.x) - attacker.x;
      const std::int64_t dy = static_cast<int>(target.y) - attacker.y;
      const std::uint64_t distance = static_cast<std::uint64_t>(dx * dx + dy * dy);
      if (distance < nearest_distance) {
        nearest_distance = distance;
        nearest = &target;
      }
    }
    if (nearest != nullptr && begin_scv_interaction(
                                  status, attacker, *nearest,
                                  ActiveUnitOrder::attack)) {
      changed = true;
    }
  }
  return changed;
}

}  // namespace

bool initialize_ai_players(BootstrapStatus &status) noexcept {
  bool initialized{};
  for (std::size_t owner = 0; owner < status.ai_players.size(); ++owner) {
    AiPlayerRuntime &ai = status.ai_players[owner];
    ai = {};
    if (owner == 0U || owner >= status.active_players.size() ||
        !status.active_players[owner] ||
        status.scenario.players()[owner].ownership != 5U) {
      continue;
    }
    ai.owner = static_cast<std::uint8_t>(owner);
    ai.race = status.scenario.players()[owner].race;
    ai.script_pc = find_script(status.ai_script_bytes, race_script_id(ai.race));
    ai.enabled = true;
    ai.script_active = ai.script_pc != 0U;
    // SAI_Build.cpp::sub_486650 automatically requests the race supply unit
    // while available supply is below thirteen: Overlord 42, Depot 109, or
    // Pylon 156. Besides supply, the Pylon is the power prerequisite for the
    // Protoss production requests that follow.
    constexpr std::array<std::uint16_t, 3> supply_types{{42U, 109U, 156U}};
    if (ai.race < supply_types.size()) {
      remember_build_request(ai, 0U, 1U, supply_types[ai.race], 120U);
    }
    status.player_mineral_stock[owner] = 50U;
    status.player_gas_stock[owner] = 0U;
    initialized = true;
  }
  return initialized;
}

bool advance_ai_players(BootstrapStatus &status,
                        const std::uint32_t now) noexcept {
  bool changed{};
  for (AiPlayerRuntime &ai : status.ai_players) {
    if (!ai.enabled) continue;
    changed = advance_ai_script(status, ai) || changed;
    ++ai.update_counter;
    if ((ai.update_counter & 15U) == 0U) {
      changed = assign_ai_harvest(status, ai) || changed;
    }
    if ((ai.update_counter & 31U) == 0U) {
      changed = satisfy_ai_build_request(status, ai, now) || changed;
      changed = satisfy_ai_research_request(status, ai) || changed;
    }
    if (ai.attack_requested && (ai.update_counter & 31U) == 0U &&
        issue_ai_attack(status, ai)) {
      ai.attack_requested = false;
      changed = true;
    }
  }
  return changed;
}

}  // namespace starcraft::recovery
