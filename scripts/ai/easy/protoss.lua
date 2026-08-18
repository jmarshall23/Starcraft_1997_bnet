local function opening(ai)
  ai.ensure(Unit.Protoss.Probe, 8, 150)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Protoss.Pylon, 1, 235)
  end
  if not planned(ai, Unit.Protoss.Pylon, 1) then
    return true
  end

  ai.ensure(Unit.Protoss.Gateway, 1, 225)
  return not complete(ai, Unit.Protoss.Gateway, 1)
end

local function early_game(ai)
  ai.ensure(Unit.Protoss.Probe, 14, 155)
  ai.ensure(Unit.Protoss.Zealot, 6, 145)
  ai.ensure(Unit.Protoss.Assimilator, 1, 205)

  if complete(ai, Unit.Protoss.Assimilator, 1) then
    ai.ensure(Unit.Protoss.CyberneticsCore, 1, 195)
  end
  if complete(ai, Unit.Protoss.CyberneticsCore, 1) then
    ai.ensure(Unit.Protoss.Dragoon, 6, 140)
    ai.upgrade(Upgrade.DragoonRange, 1, 120)
  end
end

local function mid_game(ai)
  ai.ensure(Unit.Protoss.Probe, 20, 150)
  ai.ensure(Unit.Protoss.Gateway, 2, 180)
  ai.ensure(Unit.Protoss.Forge, 1, 165)
  ai.upgrade(Upgrade.ProtossGroundWeapons, 1, 120)
  if after(ai, 600) then
    ai.ensure(Unit.Protoss.Nexus, 2, 155)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 3)
  end
  if opening(ai) then
    return
  end

  early_game(ai)
  if after(ai, 300) then
    mid_game(ai)
  end
  attack_at(ai, 12)
end
