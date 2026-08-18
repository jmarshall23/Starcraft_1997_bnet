#include "../platform/bootstrap_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace starcraft::recovery {
namespace {

// TechTree.cpp::word_502078, recovered byte-for-byte from StarCraft.exe.
// Record zero is unused. Each following record is
//   unit type, condition stream, 0xffff
// and 0xff01 joins the adjacent conditions into an OR group.
constexpr std::array<std::uint16_t, 492> kUnitRequirementTree{{
  0x0000U, 0x0000U, 0xFF02U, 0x006FU, 0xFF07U, 0xFF05U, 0xFFFFU, 0x0001U, 0xFF02U, 0x006FU, 0xFF07U, 0xFF05U,
  0x0075U, 0x0070U, 0xFFFFU, 0x0002U, 0xFF02U, 0x0071U, 0xFF05U, 0xFF07U, 0xFFFFU, 0x0003U, 0xFF02U, 0x0071U,
  0xFF05U, 0xFF07U, 0x007BU, 0xFFFFU, 0x0005U, 0xFF02U, 0x0071U, 0xFF05U, 0xFF07U, 0xFF04U, 0x0078U, 0xFFFFU,
  0x0007U, 0xFF02U, 0x006AU, 0xFF07U, 0xFF05U, 0xFFFFU, 0x0008U, 0xFF02U, 0x0072U, 0xFF07U, 0xFF05U, 0xFFFFU,
  0x0009U, 0xFF02U, 0x0072U, 0xFF07U, 0xFF05U, 0xFF04U, 0x0073U, 0x0074U, 0xFFFFU, 0x000BU, 0xFF02U, 0x0072U,
  0xFF07U, 0xFF05U, 0xFF04U, 0x0073U, 0xFFFFU, 0x000CU, 0xFF02U, 0x0072U, 0xFF07U, 0xFF05U, 0xFF04U, 0x0073U,
  0x0076U, 0xFFFFU, 0x0020U, 0xFF02U, 0x006FU, 0xFF07U, 0xFF05U, 0x0070U, 0xFFFFU, 0x000EU, 0xFF02U, 0x006CU,
  0xFF0CU, 0xFF06U, 0xFFFFU, 0x0025U, 0xFF02U, 0x0023U, 0x008EU, 0xFFFFU, 0x0026U, 0xFF02U, 0x0023U, 0x0087U,
  0xFFFFU, 0x0027U, 0xFF02U, 0x0023U, 0x008CU, 0xFFFFU, 0x0029U, 0xFF02U, 0x0023U, 0xFFFFU, 0x002AU, 0xFF02U,
  0x0023U, 0xFFFFU, 0x002BU, 0xFF02U, 0x0023U, 0x008DU, 0xFF01U, 0xFF03U, 0x0089U, 0xFFFFU, 0x002DU, 0xFF02U,
  0x0023U, 0x008AU, 0xFFFFU, 0x002EU, 0xFF02U, 0x0023U, 0x0088U, 0xFFFFU, 0x002FU, 0xFF02U, 0x0023U, 0x0089U,
  0xFFFFU, 0x0032U, 0xFF02U, 0x0082U, 0xFF05U, 0xFFFFU, 0x0040U, 0xFF02U, 0x009AU, 0xFF07U, 0xFFFFU, 0x0041U,
  0xFF02U, 0x00A0U, 0xFFFFU, 0x0042U, 0xFF02U, 0x00A0U, 0x00A4U, 0xFFFFU, 0x0043U, 0xFF02U, 0x00A0U, 0x00A5U,
  0xFFFFU, 0x0045U, 0xFF02U, 0x009BU, 0x00ABU, 0xFFFFU, 0x0046U, 0xFF02U, 0x00A7U, 0xFFFFU, 0x0047U, 0xFF02U,
  0x00A7U, 0x00AAU, 0xFFFFU, 0x0048U, 0xFF02U, 0x00A7U, 0x00A9U, 0xFFFFU, 0x0049U, 0xFF02U, 0x0048U, 0xFF01U,
  0xFF02U, 0x0052U, 0xFF0AU, 0xFFFFU, 0x0053U, 0xFF02U, 0x009BU, 0xFFFFU, 0x0055U, 0xFF02U, 0x0053U, 0xFF0AU,
  0xFFFFU, 0x0054U, 0xFF02U, 0x009BU, 0x009FU, 0xFFFFU, 0x0058U, 0xFF02U, 0x00B5U, 0xFFFFU, 0x005CU, 0xFF02U,
  0x00B5U, 0xFFFFU, 0x005DU, 0xFF02U, 0x00B8U, 0xFFFFU, 0x006AU, 0xFF02U, 0x0007U, 0xFFFFU, 0x006BU, 0xFF02U,
  0x006AU, 0xFF08U, 0xFF06U, 0x0070U, 0xFFFFU, 0x006CU, 0xFF02U, 0x006AU, 0xFF08U, 0xFF06U, 0x0075U, 0xFFFFU,
  0x006DU, 0xFF02U, 0x0007U, 0xFFFFU, 0x006EU, 0xFF02U, 0x0007U, 0xFFFFU, 0x006FU, 0xFF02U, 0x0007U, 0x006AU,
  0xFFFFU, 0x0070U, 0xFF02U, 0x0007U, 0x006FU, 0xFFFFU, 0x0071U, 0xFF02U, 0x0007U, 0x006FU, 0xFFFFU, 0x0072U,
  0xFF02U, 0x0007U, 0x0071U, 0xFFFFU, 0x0073U, 0xFF02U, 0x0072U, 0xFF08U, 0xFF06U, 0xFFFFU, 0x0074U, 0xFF02U,
  0x0007U, 0x0072U, 0xFFFFU, 0x0075U, 0xFF02U, 0x0074U, 0xFF08U, 0xFF06U, 0xFFFFU, 0x0076U, 0xFF02U, 0x0074U,
  0xFF08U, 0xFF06U, 0xFFFFU, 0x0078U, 0xFF02U, 0x0071U, 0xFF08U, 0xFF06U, 0xFFFFU, 0x007AU, 0xFF02U, 0x0007U,
  0x006AU, 0xFFFFU, 0x007BU, 0xFF02U, 0x0007U, 0x0071U, 0xFFFFU, 0x007CU, 0xFF02U, 0x0007U, 0x007AU, 0xFFFFU,
  0x007DU, 0xFF02U, 0x0007U, 0x006FU, 0xFFFFU, 0x0083U, 0xFF02U, 0x0029U, 0xFF0DU, 0xFFFFU, 0x0084U, 0xFF02U,
  0x0083U, 0x008EU, 0xFF06U, 0xFFFFU, 0x0085U, 0xFF02U, 0x0084U, 0x008AU, 0xFF06U, 0xFFFFU, 0x008EU, 0xFF02U,
  0x0029U, 0xFF0DU, 0x0083U, 0xFF01U, 0xFF03U, 0x0084U, 0xFF01U, 0xFF03U, 0x0085U, 0xFFFFU, 0x0087U, 0xFF02U,
  0x0029U, 0xFF0DU, 0x008EU, 0xFFFFU, 0x0088U, 0xFF02U, 0x0029U, 0xFF0DU, 0x0085U, 0xFFFFU, 0x0089U, 0xFF02U,
  0x008DU, 0xFF06U, 0x0084U, 0xFF01U, 0xFF03U, 0x0085U, 0xFFFFU, 0x008AU, 0xFF02U, 0x0029U, 0xFF0DU, 0x0084U,
  0xFF01U, 0xFF03U, 0x0085U, 0xFFFFU, 0x008BU, 0xFF02U, 0x0029U, 0xFF0DU, 0x0083U, 0xFF01U, 0xFF03U, 0x0084U,
  0xFF01U, 0xFF03U, 0x0085U, 0xFFFFU, 0x008CU, 0xFF02U, 0x0029U, 0xFF0DU, 0x0085U, 0xFFFFU, 0x008DU, 0xFF02U,
  0x0029U, 0xFF0DU, 0x008BU, 0xFFFFU, 0x0086U, 0xFF02U, 0x0029U, 0xFF0DU, 0x0087U, 0xFFFFU, 0x008FU, 0xFF02U,
  0x0029U, 0xFF0DU, 0xFFFFU, 0x0090U, 0xFF02U, 0x008FU, 0x008BU, 0xFFFFU, 0x0092U, 0xFF02U, 0x008FU, 0x008EU,
  0xFFFFU, 0x0095U, 0xFF02U, 0x0029U, 0xFFFFU, 0x009AU, 0xFF02U, 0x0040U, 0xFFFFU, 0x009BU, 0xFF02U, 0x0040U,
  0x00A4U, 0xFFFFU, 0x009CU, 0xFF02U, 0x0040U, 0xFFFFU, 0x009DU, 0xFF02U, 0x0040U, 0xFFFFU, 0x009FU, 0xFF02U,
  0x0040U, 0x009BU, 0xFFFFU, 0x00A0U, 0xFF02U, 0x0040U, 0x009AU, 0xFFFFU, 0x00A2U, 0xFF02U, 0x0040U, 0x00A6U,
  0xFFFFU, 0x00A3U, 0xFF02U, 0x0040U, 0x00A4U, 0xFFFFU, 0x00A4U, 0xFF02U, 0x0040U, 0x00A0U, 0xFFFFU, 0x00A5U,
  0xFF02U, 0x0040U, 0x00A3U, 0xFFFFU, 0x00A6U, 0xFF02U, 0x0040U, 0x009AU, 0xFFFFU, 0x00A7U, 0xFF02U, 0x0040U,
  0x00A4U, 0xFFFFU, 0x00A9U, 0xFF02U, 0x0040U, 0x00A7U, 0xFFFFU, 0x00AAU, 0xFF02U, 0x0040U, 0x00A7U, 0x00A5U,
  0xFFFFU, 0x00ABU, 0xFF02U, 0x0040U, 0x009BU, 0xFFFFU, 0x00ACU, 0xFF02U, 0x0040U, 0x00A0U, 0xFFFFU, 0xFFFFU,
}};

struct RequirementCondition {
  bool met{};
  bool visible{true};
  bool names_unit{};
  std::uint16_t unit_type{0xFFFFU};
  std::size_t next{};
};

bool owns_unit(const BootstrapStatus &status, const std::uint8_t owner,
               const std::uint16_t unit_type,
               const bool include_incomplete) noexcept {
  return std::any_of(status.units.begin(), status.units.end(),
                     [=](const ScenarioUnitPreview &unit) {
    return unit.alive && unit.owner == owner && unit.unit_type == unit_type &&
           (include_incomplete || unit.construction_complete);
  });
}

RequirementCondition evaluate_condition(const BootstrapStatus &status,
                                        const ScenarioUnitPreview &producer,
                                        const std::size_t index) noexcept {
  RequirementCondition result{};
  result.next = index + 1U;
  const std::uint16_t opcode = kUnitRequirementTree[index];
  switch (opcode) {
    case 0xFF02U:
    case 0xFF03U:
    case 0xFF04U: {
      if (result.next >= kUnitRequirementTree.size()) {
        return result;
      }
      result.unit_type = kUnitRequirementTree[result.next++];
      result.names_unit = true;
      if (opcode == 0xFF02U) {
        result.met = result.unit_type == producer.unit_type ||
                     owns_unit(status, producer.owner, result.unit_type, false);
      } else if (opcode == 0xFF03U) {
        result.met = owns_unit(status, producer.owner, result.unit_type, true);
      } else {
        const ScenarioUnitPreview *const addon =
            find_unit_by_id(status, producer.attached_addon_id);
        result.met = addon != nullptr && addon->alive &&
                     addon->unit_type == result.unit_type;
      }
      return result;
    }
    case 0xFF05U:
      result.met = producer.construction_complete;
      return result;
    case 0xFF06U:
      result.met = !producer.production_active &&
                   producer.production_queue.empty() &&
                   producer.active_technology == 28U &&
                   producer.active_upgrade == 46U;
      return result;
    case 0xFF07U:
      result.met = producer.construction_complete && !producer.dying;
      return result;
    case 0xFF08U:
      result.met = producer.attached_addon_id == 0U;
      return result;
    case 0xFF0AU: {
      const std::size_t occupied = static_cast<std::size_t>(std::count_if(
          producer.hangar_unit_ids.begin(), producer.hangar_unit_ids.end(),
          [](const std::uint32_t id) { return id != 0U; }));
      std::size_t capacity =
          producer.unit_type == 83U || producer.unit_type == 72U ||
                  producer.unit_type == 82U
              ? 5U
              : 0U;
      if (producer.owner < status.player_upgrade_levels.size() &&
          ((producer.unit_type == 83U &&
            status.player_upgrade_levels[producer.owner][36U] != 0U) ||
           ((producer.unit_type == 72U || producer.unit_type == 82U) &&
            status.player_upgrade_levels[producer.owner][43U] != 0U))) {
        capacity = 10U;
      }
      result.met = occupied + producer.production_queue.count() < capacity;
      return result;
    }
    case 0xFF0CU:
      result.met = producer.unit_type == 108U &&
                   producer.production_queue.empty();
      return result;
    case 0xFF0DU:
      result.met = !is_airborne(producer);
      return result;
    case 0xFF18U:
      result.met = false;
      return result;
    case 0xFF19U:
      result.met = false;
      result.visible = false;
      return result;
    default:
      result.names_unit = opcode < 228U;
      result.unit_type = opcode;
      result.met = result.names_unit &&
                   owns_unit(status, producer.owner, opcode, false);
      return result;
  }
}

void append_unique(std::array<std::uint16_t, 12> &values,
                   std::size_t &count, const std::uint16_t value) noexcept {
  if (value >= 228U || count >= values.size() ||
      std::find(values.begin(), values.begin() + count, value) !=
          values.begin() + count) {
    return;
  }
  values[count++] = value;
}

std::size_t requirement_record(const std::uint16_t product_type) noexcept {
  std::size_t index = 1U;
  while (index < kUnitRequirementTree.size()) {
    const std::uint16_t unit_type = kUnitRequirementTree[index++];
    if (unit_type == 0xFFFFU) {
      break;
    }
    if (unit_type == product_type) {
      return index;
    }
    while (index < kUnitRequirementTree.size() &&
           kUnitRequirementTree[index] != 0xFFFFU) {
      ++index;
    }
    ++index;
  }
  return 0U;
}

bool button_product(const CommandButtonVisual &button) noexcept {
  using Action = CommandButtonVisual::Action;
  return button.action == Action::train_unit ||
         button.action == Action::begin_building_placement ||
         button.action == Action::build_addon ||
         button.action == Action::morph_building;
}

}  // namespace

UnitRequirementResult unit_requirements_for(
    const BootstrapStatus &status, const ScenarioUnitPreview &producer,
    const std::uint16_t product_type) noexcept {
  UnitRequirementResult result{};
  std::size_t index = requirement_record(product_type);
  if (index == 0U) {
    return result;
  }
  while (index < kUnitRequirementTree.size() &&
         kUnitRequirementTree[index] != 0xFFFFU) {
    bool group_met{};
    bool group_visible{true};
    std::array<std::uint16_t, 4> group_units{};
    std::size_t group_unit_count{};
    do {
      const RequirementCondition condition =
          evaluate_condition(status, producer, index);
      index = condition.next;
      group_met = group_met || condition.met;
      group_visible = group_visible && condition.visible;
      if (condition.names_unit && condition.unit_type != producer.unit_type &&
          group_unit_count < group_units.size()) {
        group_units[group_unit_count++] = condition.unit_type;
      }
      if (index >= kUnitRequirementTree.size() ||
          kUnitRequirementTree[index] != 0xFF01U) {
        break;
      }
      ++index;
    } while (index < kUnitRequirementTree.size());

    result.visible = result.visible && group_visible;
    result.allowed = result.allowed && group_met;
    for (std::size_t unit_index = 0U; unit_index < group_unit_count;
         ++unit_index) {
      append_unique(result.required_units, result.required_count,
                    group_units[unit_index]);
      if (!group_met) {
        append_unique(result.missing_units, result.missing_count,
                      group_units[unit_index]);
      }
    }
  }
  return result;
}

bool command_button_enabled(const BootstrapStatus &status,
                            const ScenarioUnitPreview &producer,
                            const CommandButtonVisual &button) noexcept {
  using Action = CommandButtonVisual::Action;
  if (button.action == Action::upgrade_technology) {
    if (button.argument >= status.upgrade_traits.size() ||
        producer.active_technology != 28U || producer.active_upgrade != 46U) {
      return false;
    }
    const auto &traits = status.upgrade_traits[button.argument];
    const std::uint32_t level =
        producer.owner < status.player_upgrade_levels.size()
            ? status.player_upgrade_levels[producer.owner][button.argument]
            : status.upgrade_levels[button.argument];
    return level < traits.maximum_level &&
           player_minerals_for(status, producer.owner) >=
               traits.mineral_cost + level * traits.mineral_factor &&
           player_gas_for(status, producer.owner) >=
               traits.gas_cost + level * traits.gas_factor;
  }
  if (button.action == Action::research_technology) {
    return button.argument < status.technology_traits.size() &&
           producer.owner < status.player_researched_technologies.size() &&
           !status.player_researched_technologies[producer.owner]
                                                   [button.argument] &&
           producer.active_technology == 28U &&
           producer.active_upgrade == 46U &&
           player_minerals_for(status, producer.owner) >=
               status.technology_traits[button.argument].mineral_cost &&
           player_gas_for(status, producer.owner) >=
               status.technology_traits[button.argument].gas_cost;
  }
  if (!button_product(button)) {
    return button.action != Action::none;
  }
  const UnitRequirementResult requirements =
      unit_requirements_for(status, producer, button.argument);
  if (!requirements.visible || !requirements.allowed) {
    return false;
  }
  std::uint32_t minerals{};
  std::uint32_t gas{};
  if (button.argument < status.runtime_unit_types.size() &&
      status.runtime_unit_types[button.argument].ready) {
    const auto &simulation =
        status.runtime_unit_types[button.argument].initialization.simulation;
    minerals = simulation.mineral_cost;
    gas = simulation.gas_cost;
  } else if (const BuildableUnitVisual *const buildable =
                 find_buildable_unit(status, button.argument)) {
    minerals = buildable->simulation.mineral_cost;
    gas = buildable->simulation.gas_cost;
  } else {
    return false;
  }
  if (player_minerals_for(status, producer.owner) < minerals ||
      player_gas_for(status, producer.owner) < gas) {
    return false;
  }
  if (button.action == Action::train_unit &&
      producer.production_queue.full()) {
    return false;
  }
  if (button.action == Action::build_addon &&
      producer.attached_addon_id != 0U) {
    return false;
  }
  return true;
}

}  // namespace starcraft::recovery
