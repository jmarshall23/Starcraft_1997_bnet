local function one_gateway_core_opening(ai)
  ai.ensure(Unit.Protoss.Probe, 9, 155)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Protoss.Pylon, 1, 240)
  end
  if not planned(ai, Unit.Protoss.Pylon, 1) then
    return true
  end

  ai.ensure(Unit.Protoss.Gateway, 1, 230)
  if not planned(ai, Unit.Protoss.Gateway, 1) then
    return true
  end

  ai.ensure(Unit.Protoss.Assimilator, 1, 215)
  if complete(ai, Unit.Protoss.Gateway, 1) then
    ai.ensure(Unit.Protoss.Zealot, 2, 150)
  end
  if complete(ai, Unit.Protoss.Assimilator, 1) then
    ai.ensure(Unit.Protoss.CyberneticsCore, 1, 205)
  end
  return not complete(ai, Unit.Protoss.CyberneticsCore, 1)
end

local function pressure_zerg(ai)
  ai.ensure(Unit.Protoss.Probe, 18, 160)
  ai.ensure(Unit.Protoss.Gateway, 2, 200)
  ai.ensure(Unit.Protoss.Zealot, 12, 155)
  ai.ensure(Unit.Protoss.Dragoon, 6, 145)
end

local function dragoon_tech(ai)
  ai.ensure(Unit.Protoss.Probe, 18, 160)
  ai.ensure(Unit.Protoss.Gateway, 2, 195)
  ai.ensure(Unit.Protoss.Dragoon, 14, 155)
  ai.upgrade(Upgrade.DragoonRange, 1, 135)

  if after(ai, 420) then
    ai.ensure(Unit.Protoss.RoboticsFacility, 1, 180)
  end
  if complete(ai, Unit.Protoss.RoboticsFacility, 1) then
    ai.ensure(Unit.Protoss.RoboticsSupportBay, 1, 165)
    ai.ensure(Unit.Protoss.Reaver, 2, 145)
  end
end

local function expansion(ai)
  if not after(ai, 420) then
    return
  end
  ai.ensure(Unit.Protoss.Nexus, 2, 175)
  if complete(ai, Unit.Protoss.Nexus, 2) then
    ai.ensure(Unit.Protoss.Probe, 30, 155)
    ai.ensure(Unit.Protoss.Forge, 1, 165)
    ai.upgrade(Upgrade.ProtossGroundWeapons, 1, 125)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 4)
  end
  if one_gateway_core_opening(ai) then
    return
  end

  local enemy = ai.enemy_race()
  if enemy == Race.Zerg then
    pressure_zerg(ai)
  else
    dragoon_tech(ai)
  end
  expansion(ai)
  attack_at(ai, enemy == Race.Zerg and 14 or 12)
end
