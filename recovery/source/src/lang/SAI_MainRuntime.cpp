#include "../platform/bootstrap_runtime.hpp"

#include "starcraft/lang/cunit_build.hpp"
#include "starcraft/lang/cunit_protoss.hpp"
#include "starcraft/lang/cunit_terran.hpp"
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
    if (!producer.construction_complete &&
        producer.construction_target_type == unit_type) {
      ++count;
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

ScenarioUnitPreview *available_builder(BootstrapStatus &status,
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

  // The strategy request often arrives after the opening workers have
  // already entered CUnit resource orders.  SAI_Build does not wait for a
  // miner to become naturally idle: it removes one eligible worker from its
  // resource group and assigns the construction order.  Reclaim a visible
  // empty-handed mineral worker here; cancel_unit_order also releases its
  // recovered HarvestQueue slot before construction takes ownership.
  for (ScenarioUnitPreview &unit : status.units) {
    if (!unit.alive || unit.owner != owner || unit.unit_type != types.worker ||
        unit.active_order != ActiveUnitOrder::gather || unit.sprite_hidden ||
        unit.cargo_minerals != 0U || unit.cargo_gas != 0U) {
      continue;
    }
    cancel_unit_order(status, unit);
    return &unit;
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
    if (request.kind == 1U && request.unit_type < status.upgrade_traits.size()) {
      const auto &traits = status.upgrade_traits[request.unit_type];
      const std::uint32_t level =
          status.player_upgrade_levels[ai.owner][request.unit_type];
      if (level < request.quantity && level < traits.maximum_level &&
          status.player_gas_stock[ai.owner] <
              traits.gas_cost + level * traits.gas_factor) {
        gas_needed = true;
      }
    }
    if (request.kind == 2U &&
        request.unit_type < status.technology_traits.size() &&
        !status.player_researched_technologies[ai.owner][request.unit_type] &&
        status.player_gas_stock[ai.owner] <
            status.technology_traits[request.unit_type].gas_cost) {
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
  const bool completed_refinery = std::any_of(
      status.units.begin(), status.units.end(), [&ai](const auto &unit) {
        return unit.alive && unit.owner == ai.owner &&
               unit.construction_complete && unit.resource_amount != 0U &&
               (unit.unit_type == 110U || unit.unit_type == 149U ||
                unit.unit_type == 157U);
      });
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
    // Do not strand an opening economy waiting for a gas structure that it
    // does not yet have the minerals to build. Until a completed refinery is
    // harvestable, every non-reserved worker must fall back to minerals.
    const bool seek_gas = gas_needed && completed_refinery && gas_workers < 3U;
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
    const ScenarioUnitPreview *base{};
    for (const ScenarioUnitPreview &unit : status.units) {
      if (unit.alive && unit.owner == owner &&
          (unit.unit_type == 106U || starcraft::lang::is_zerg_town_hall(unit.unit_type) ||
           unit.unit_type == 154U)) {
        base = &unit;
        break;
      }
    }
    const ScenarioUnitPreview *best{};
    std::uint64_t best_distance = UINT64_MAX;
    for (const ScenarioUnitPreview &geyser : status.units) {
      if (geyser.alive && geyser.unit_type == 188U &&
          placement_is_valid(status, buildable, geyser.x, geyser.y, owner)) {
        const std::int64_t dx = base == nullptr ? 0 :
            static_cast<int>(geyser.x) - base->x;
        const std::int64_t dy = base == nullptr ? 0 :
            static_cast<int>(geyser.y) - base->y;
        const std::uint64_t distance =
            static_cast<std::uint64_t>(dx * dx + dy * dy);
        if (best == nullptr || distance < best_distance) {
          best = &geyser;
          best_distance = distance;
        }
      }
    }
    if (best == nullptr) return false;
    output_x = best->x;
    output_y = best->y;
    return true;
  }
  const bool town_hall = buildable.unit_type == 106U ||
                         buildable.unit_type == 131U ||
                         buildable.unit_type == 154U;
  if (town_hall && owned_count(status, owner, buildable.unit_type) != 0U) {
    bool found{};
    std::uint64_t best_score = UINT64_MAX;
    const int half_width = buildable.placement_width / 2;
    const int half_height = buildable.placement_height / 2;
    for (const ScenarioUnitPreview &mineral : status.units) {
      if (!mineral.alive || mineral.unit_type < 176U ||
          mineral.unit_type > 178U || mineral.resource_amount == 0U) {
        continue;
      }
      bool serviced{};
      for (const ScenarioUnitPreview &base : status.units) {
        if (!base.alive || base.owner != owner ||
            (base.unit_type != 106U &&
             !starcraft::lang::is_zerg_town_hall(base.unit_type) &&
             base.unit_type != 154U)) {
          continue;
        }
        const std::int64_t dx = static_cast<int>(base.x) - mineral.x;
        const std::int64_t dy = static_cast<int>(base.y) - mineral.y;
        if (dx * dx + dy * dy < 384LL * 384LL) {
          serviced = true;
          break;
        }
      }
      if (serviced) continue;
      for (int radius = 4; radius <= 10; ++radius) {
        for (int edge = -radius; edge <= radius; ++edge) {
          const std::array<std::array<int, 2>, 4> points{{
              {{edge, -radius}}, {{radius, edge}},
              {{-edge, radius}}, {{-radius, -edge}},
          }};
          for (const auto &point : points) {
            const int left = ((static_cast<int>(mineral.x) + point[0] * 32 -
                               half_width + 16) / 32) * 32;
            const int top = ((static_cast<int>(mineral.y) + point[1] * 32 -
                              half_height + 16) / 32) * 32;
            const int center_x = left + half_width;
            const int center_y = top + half_height;
            if (center_x < 0 || center_y < 0 || center_x > UINT16_MAX ||
                center_y > UINT16_MAX ||
                !placement_is_valid(status, buildable,
                                    static_cast<std::uint16_t>(center_x),
                                    static_cast<std::uint16_t>(center_y),
                                    owner)) {
              continue;
            }
            const std::int64_t dx = center_x - mineral.x;
            const std::int64_t dy = center_y - mineral.y;
            const std::uint64_t score =
                static_cast<std::uint64_t>(dx * dx + dy * dy);
            if (score < best_score) {
              best_score = score;
              output_x = static_cast<std::uint16_t>(center_x);
              output_y = static_cast<std::uint16_t>(center_y);
              found = true;
            }
          }
        }
      }
    }
    if (found) return true;
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
  if ((buildable.simulation.dat_flags & 2U) != 0U) {
    for (ScenarioUnitPreview &candidate : status.units) {
      if (!candidate.alive || candidate.owner != ai.owner ||
          !candidate.construction_complete ||
          candidate.unit_type != buildable.addon_parent_type ||
          candidate.attached_addon_id != 0U) {
        continue;
      }
      const UnitRequirementResult requirements =
          unit_requirements_for(status, candidate, buildable.unit_type);
      std::uint16_t center_x{};
      std::uint16_t center_y{};
      if (!requirements.visible || !requirements.allowed ||
          !addon_center_for_parent(buildable, candidate, center_x, center_y) ||
          !placement_is_valid(status, buildable, center_x, center_y, ai.owner)) {
        continue;
      }
      auto &minerals = status.player_mineral_stock[ai.owner];
      auto &gas = status.player_gas_stock[ai.owner];
      if (minerals < buildable.simulation.mineral_cost ||
          gas < buildable.simulation.gas_cost) {
        return false;
      }
      ScenarioUnitPreview addon{};
      addon.unit_id = status.next_unit_id++;
      if (!configure_preview_type(status, addon, buildable.unit_type)) {
        return false;
      }
      addon.x = center_x;
      addon.y = center_y;
      addon.x_fixed = static_cast<std::int32_t>(center_x) << 8U;
      addon.y_fixed = static_cast<std::int32_t>(center_y) << 8U;
      addon.owner = ai.owner;
      addon.is_building = true;
      addon.construction_complete = false;
      addon.construction_ticks_total = static_cast<std::uint16_t>((std::max)(
          1U, static_cast<unsigned>(buildable.simulation.build_time) >> 1U));
      addon.construction_ticks_remaining = addon.construction_ticks_total;
      addon.hit_points = (std::max)(1U, addon.max_hit_points / 10U);
      addon.addon_parent_id = candidate.unit_id;
      addon.construction_animation_phase = 0U;
      if (buildable.construction_asset_index != SIZE_MAX &&
          !replace_preview_primary_image(status, addon,
                                         buildable.construction_asset_index)) {
        return false;
      }
      const std::uint32_t addon_id = addon.unit_id;
      const std::uint32_t parent_id = candidate.unit_id;
      try {
        status.units.push_back(std::move(addon));
      } catch (...) {
        return false;
      }
      if (ScenarioUnitPreview *const parent = find_unit_by_id(status, parent_id)) {
        parent->attached_addon_id = addon_id;
      }
      minerals -= buildable.simulation.mineral_cost;
      gas -= buildable.simulation.gas_cost;
      return true;
    }
    return false;
  }
  ScenarioUnitPreview *worker = available_builder(status, ai.owner);
  if (worker == nullptr) return false;
  const UnitRequirementResult requirements =
      unit_requirements_for(status, *worker, buildable.unit_type);
  if (!requirements.visible || !requirements.allowed) return false;
  std::uint16_t x{};
  std::uint16_t y{};
  if (!find_ai_build_site(status, buildable, ai.owner, x, y)) return false;
  if (ai.race == 2U) {
    return begin_protoss_build_order(status, *worker, buildable, x, y, true);
  }
  if (ai.race == 0U) {
    return begin_zerg_build_order(status, *worker, buildable, x, y, true);
  }
  // Terran AI uses the same sub_422DF0 move-then-create order as a player
  // SCV. This keeps its timing, construction image, and welding movement from
  // diverging into the former instant-building shortcut.
  return begin_terran_build_order(status, *worker, buildable, x, y, true);
}

bool start_ai_zerg_building_morph(BootstrapStatus &status,
                                  AiPlayerRuntime &ai,
                                  const std::uint16_t target_type) noexcept {
  std::uint16_t parent_type = 0xFFFFU;
  switch (target_type) {
    case 132U: parent_type = 131U; break;
    case 133U: parent_type = 132U; break;
    case 137U: parent_type = 141U; break;
    case 144U:
    case 146U: parent_type = 143U; break;
    default: return false;
  }
  if (target_type >= status.runtime_unit_types.size()) return false;
  const RuntimeUnitType &target = status.runtime_unit_types[target_type];
  if (!target.ready) return false;
  for (ScenarioUnitPreview &building : status.units) {
    if (!building.alive || building.owner != ai.owner ||
        building.unit_type != parent_type || !building.construction_complete) {
      continue;
    }
    const UnitRequirementResult requirements =
        unit_requirements_for(status, building, target_type);
    if (!requirements.visible || !requirements.allowed ||
        target.initialization.placement_width != building.selection_width ||
        target.initialization.placement_height != building.selection_height) {
      continue;
    }
    const auto &simulation = target.initialization.simulation;
    auto &minerals = status.player_mineral_stock[ai.owner];
    auto &gas = status.player_gas_stock[ai.owner];
    if (minerals < simulation.mineral_cost || gas < simulation.gas_cost) {
      return false;
    }
    minerals -= simulation.mineral_cost;
    gas -= simulation.gas_cost;
    building.construction_complete = false;
    building.construction_target_type = target_type;
    building.construction_ticks_total = static_cast<std::uint16_t>((std::max)(
        1U, static_cast<unsigned>(simulation.build_time) >> 1U));
    building.construction_ticks_remaining = building.construction_ticks_total;
    building.construction_animation_phase = 2U;
    const BuildableUnitVisual *const current =
        find_buildable_unit(status, building.unit_type);
    if (current != nullptr && current->construction_asset_index != SIZE_MAX) {
      (void)replace_preview_primary_image(status, building,
                                          current->construction_asset_index);
    }
    (void)restart_unit_animation(status, building, 13U);
    return true;
  }
  return false;
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
      if (ai.race == 0U &&
          starcraft::lang::is_zerg_building_morph_target(request.unit_type)) {
        if (start_ai_zerg_building_morph(status, ai, request.unit_type)) {
          return true;
        }
        continue;
      }
      const BuildableUnitVisual *const buildable =
          find_buildable_unit(status, request.unit_type);
      if (buildable == nullptr) {
        continue;
      }
      const auto &simulation = runtime.initialization.simulation;
      if (status.player_mineral_stock[ai.owner] < simulation.mineral_cost ||
          status.player_gas_stock[ai.owner] < simulation.gas_cost) {
        continue;
      }
      if (start_ai_building(status, ai, *buildable)) {
        return true;
      }
      continue;
    }
    for (ScenarioUnitPreview &producer : status.units) {
      if (!producer.alive || producer.owner != ai.owner ||
          !producer.construction_complete || producer.production_queue.full() ||
          !starcraft::lang::producer_builds_unit(producer.unit_type,
                                                request.unit_type)) {
        continue;
      }
      const UnitRequirementResult requirements =
          unit_requirements_for(status, producer, request.unit_type);
      if (!requirements.visible || !requirements.allowed) {
        continue;
      }
      const auto &simulation = runtime.initialization.simulation;
      std::uint32_t &minerals = status.player_mineral_stock[ai.owner];
      std::uint32_t &gas = status.player_gas_stock[ai.owner];
      const auto supply = player_supply(status, ai.owner, ai.race);
      const std::uint32_t required_supply =
          request.unit_type < status.unit_traits.size()
              ? status.unit_traits[request.unit_type].supply_required
              : 0U;
      if (minerals < simulation.mineral_cost || gas < simulation.gas_cost ||
          (required_supply != 0U && supply[0] + required_supply > supply[1]) ||
          !producer.production_queue.enqueue(request.unit_type)) {
        continue;
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
          (void)displace_units_for_zerg_egg(status, producer);
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
        (!attacker.has_ground_weapon && !attacker.has_air_weapon) ||
        attacker.active_order != ActiveUnitOrder::none) {
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
  const std::filesystem::path script_root =
      locate_input_root() / "scripts" / "ai";
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
    bool lua_ready{};
    try {
      ai.controller = std::make_shared<CAI>(ai.owner, ai.race,
                                            status.ai_difficulty, script_root);
      lua_ready = ai.controller->initialize();
    } catch (...) {
      ai.controller.reset();
    }
    ai.script_pc = find_script(status.ai_script_bytes, race_script_id(ai.race));
    ai.enabled = true;
    ai.script_active = lua_ready || ai.script_pc != 0U;
    ai.script_error =
        lua_ready ? std::string{}
                  : (ai.controller != nullptr ? ai.controller->last_error()
                                              : "Lua controller allocation failed");
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
    const bool lua_ready = ai.controller != nullptr && ai.controller->ready();
    if (!lua_ready) {
      changed = advance_ai_script(status, ai) || changed;
    }
    if (ai.macro_update_ticks != 0U) {
      --ai.macro_update_ticks;
    } else {
      if (lua_ready &&
          !ai.controller->update(status, ai, ai.update_counter)) {
        ai.script_active = false;
        ai.script_error = ai.controller->last_error();
      }
      switch (status.ai_difficulty) {
        case AiDifficulty::easy: ai.macro_update_ticks = 59U; break;
        case AiDifficulty::hard: ai.macro_update_ticks = 7U; break;
        case AiDifficulty::medium:
        default: ai.macro_update_ticks = 19U; break;
      }
      changed = assign_ai_harvest(status, ai) || changed;

      // SAI_Build.cpp::sub_486650 walks the complete priority request list
      // and can service independent towns/producers in one heavy pass. Keep
      // asking until no additional request can advance, bounded by the fixed
      // 64-entry recovered request table.
      for (std::size_t request = 0U;
           request < ai.build_requests.size() &&
           satisfy_ai_build_request(status, ai, now);
           ++request) {
        changed = true;
      }
      changed = satisfy_ai_research_request(status, ai) || changed;
      if (ai.attack_requested && issue_ai_attack(status, ai)) {
        ai.attack_requested = false;
        changed = true;
      }
    }
    ++ai.update_counter;
  }
  return changed;
}

}  // namespace starcraft::recovery
