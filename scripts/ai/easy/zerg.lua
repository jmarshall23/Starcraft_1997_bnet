local function pool_first_opening(ai)
  ai.ensure(Unit.Zerg.Drone, 8, 150)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Zerg.Overlord, 2, 240)
  end
  if not planned(ai, Unit.Zerg.Overlord, 2) then
    return true
  end

  ai.ensure(Unit.Zerg.SpawningPool, 1, 230)
  return not complete(ai, Unit.Zerg.SpawningPool, 1)
end

local function early_game(ai)
  ai.ensure(Unit.Zerg.Drone, 12, 155)
  ai.ensure(Unit.Zerg.Zergling, 8, 145)
  ai.ensure(Unit.Zerg.Hatchery, 2, 195)
  ai.ensure(Unit.Zerg.Extractor, 1, 185)
end

local function hydralisk_mid_game(ai)
  if not complete(ai, Unit.Zerg.Hatchery, 2) or
     not complete(ai, Unit.Zerg.Extractor, 1) then
    return
  end

  ai.ensure(Unit.Zerg.Drone, 18, 150)
  ai.ensure(Unit.Zerg.HydraliskDen, 1, 175)
  if complete(ai, Unit.Zerg.HydraliskDen, 1) then
    ai.ensure(Unit.Zerg.Hydralisk, 10, 145)
  end
  if after(ai, 480) then
    ai.ensure(Unit.Zerg.EvolutionChamber, 1, 155)
    ai.upgrade(Upgrade.ZergMissile, 1, 120)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 3)
  end
  if pool_first_opening(ai) then
    return
  end

  early_game(ai)
  if after(ai, 300) then
    hydralisk_mid_game(ai)
  end
  attack_at(ai, 14)
end
