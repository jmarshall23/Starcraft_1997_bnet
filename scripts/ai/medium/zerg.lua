local function safe_pool_opening(ai)
  ai.ensure(Unit.Zerg.Drone, 9, 155)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Zerg.Overlord, 2, 245)
  end
  if not planned(ai, Unit.Zerg.Overlord, 2) then
    return true
  end

  ai.ensure(Unit.Zerg.SpawningPool, 1, 235)
  return not complete(ai, Unit.Zerg.SpawningPool, 1)
end

local function establish_second_hatchery(ai)
  ai.ensure(Unit.Zerg.Zergling, 8, 155)
  ai.ensure(Unit.Zerg.Drone, 14, 160)
  ai.ensure(Unit.Zerg.Hatchery, 2, 210)
  ai.ensure(Unit.Zerg.Extractor, 1, 200)
  return not complete(ai, Unit.Zerg.Hatchery, 2) or
         not complete(ai, Unit.Zerg.Extractor, 1)
end

local function mutalisk_branch(ai)
  ai.ensure(Unit.Zerg.Lair, 1, 195)
  if not complete(ai, Unit.Zerg.Lair, 1) then
    return
  end

  ai.ensure(Unit.Zerg.Spire, 1, 185)
  if complete(ai, Unit.Zerg.Spire, 1) then
    ai.ensure(Unit.Zerg.Mutalisk, 12, 155)
  end
end

local function hydralisk_branch(ai)
  ai.ensure(Unit.Zerg.HydraliskDen, 1, 195)
  if not complete(ai, Unit.Zerg.HydraliskDen, 1) then
    return
  end

  ai.ensure(Unit.Zerg.Hydralisk, 18, 155)
  if after(ai, 480) then
    ai.ensure(Unit.Zerg.EvolutionChamber, 1, 170)
    ai.upgrade(Upgrade.ZergMissile, 1, 125)
  end
end

local function macro_branch(ai)
  ai.ensure(Unit.Zerg.Drone, 22, 160)
  if after(ai, 540) then
    ai.ensure(Unit.Zerg.Hatchery, 3, 180)
  end
  if complete(ai, Unit.Zerg.Hatchery, 3) then
    ai.ensure(Unit.Zerg.Drone, 32, 155)
    ai.ensure(Unit.Zerg.Extractor, 2, 175)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 4)
  end
  if safe_pool_opening(ai) then
    return
  end
  if establish_second_hatchery(ai) then
    return
  end

  local enemy = ai.enemy_race()
  if enemy == Race.Terran or enemy == Race.Zerg then
    mutalisk_branch(ai)
  else
    hydralisk_branch(ai)
  end
  macro_branch(ai)
  attack_at(ai, 14)
end
