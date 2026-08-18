local function opening(ai)
  ai.ensure(Unit.Protoss.Probe, 9, 165)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Protoss.Pylon, 1, 245)
  end
  if not planned(ai, Unit.Protoss.Pylon, 1) then
    return true
  end

  ai.ensure(Unit.Protoss.Gateway, 1, 235)
  ai.ensure(Unit.Protoss.Assimilator, 1, 225)
  if complete(ai, Unit.Protoss.Gateway, 1) then
    ai.ensure(Unit.Protoss.Zealot, 2, 160)
  end
  if complete(ai, Unit.Protoss.Assimilator, 1) then
    ai.ensure(Unit.Protoss.CyberneticsCore, 1, 215)
  end
  return not complete(ai, Unit.Protoss.CyberneticsCore, 1)
end

local function two_gateway_pressure(ai)
  ai.ensure(Unit.Protoss.Probe, 20, 170)
  ai.ensure(Unit.Protoss.Gateway, 2, 210)
  ai.ensure(Unit.Protoss.Zealot, 16, 165)
  ai.ensure(Unit.Protoss.Dragoon, 10, 155)
  ai.upgrade(Upgrade.DragoonRange, 1, 140)
end

local function dragoon_reaver(ai)
  ai.ensure(Unit.Protoss.Probe, 20, 170)
  ai.ensure(Unit.Protoss.Gateway, 3, 205)
  ai.ensure(Unit.Protoss.Dragoon, 20, 165)
  ai.upgrade(Upgrade.DragoonRange, 1, 145)
  ai.ensure(Unit.Protoss.RoboticsFacility, 1, 195)

  if complete(ai, Unit.Protoss.RoboticsFacility, 1) then
    ai.ensure(Unit.Protoss.RoboticsSupportBay, 1, 180)
    ai.ensure(Unit.Protoss.Reaver, 4, 155)
  end
end

local function macro_branch(ai)
  if after(ai, 360) then
    ai.ensure(Unit.Protoss.Nexus, 2, 195)
  end
  if complete(ai, Unit.Protoss.Nexus, 2) then
    ai.ensure(Unit.Protoss.Probe, after(ai, 720) and 46 or 32, 165)
    ai.ensure(Unit.Protoss.Assimilator, 2, 185)
    ai.ensure(Unit.Protoss.Forge, 1, 180)
    ai.upgrade(Upgrade.ProtossGroundWeapons, after(ai, 720) and 2 or 1, 135)
    ai.upgrade(Upgrade.ProtossArmor, after(ai, 780) and 2 or 1, 130)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 5)
  end
  if opening(ai) then
    return
  end

  local enemy = ai.enemy_race()
  if enemy == Race.Zerg then
    two_gateway_pressure(ai)
  else
    dragoon_reaver(ai)
  end
  macro_branch(ai)
  attack_at(ai, enemy == Race.Zerg and 12 or 10)
end
