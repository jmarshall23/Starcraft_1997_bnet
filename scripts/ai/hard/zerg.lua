local function prepare_overlord(ai)
  ai.ensure(Unit.Zerg.Drone, 9, 165)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Zerg.Overlord, 2, 250)
  end
  return planned(ai, Unit.Zerg.Overlord, 2)
end

local function pool_first_opening(ai)
  if not prepare_overlord(ai) then
    return true
  end
  ai.ensure(Unit.Zerg.SpawningPool, 1, 240)
  if planned(ai, Unit.Zerg.SpawningPool, 1) then
    ai.ensure(Unit.Zerg.Hatchery, 2, 225)
  end
  return not complete(ai, Unit.Zerg.SpawningPool, 1)
end

local function twelve_hatch_opening(ai)
  if not prepare_overlord(ai) then
    return true
  end

  ai.ensure(Unit.Zerg.Drone, 11, 170)
  if ai.count(Unit.Zerg.Drone) >= 10 or ai.minerals() >= 500 then
    ai.ensure(Unit.Zerg.Hatchery, 2, 240)
  end
  if planned(ai, Unit.Zerg.Hatchery, 2) then
    ai.ensure(Unit.Zerg.SpawningPool, 1, 230)
  end
  return not complete(ai, Unit.Zerg.SpawningPool, 1)
end

local function establish_gas(ai)
  ai.ensure(Unit.Zerg.Zergling, 10, 170)
  ai.ensure(Unit.Zerg.Drone, 16, 175)
  ai.ensure(Unit.Zerg.Hatchery, 2, 220)
  ai.ensure(Unit.Zerg.Extractor, 1, 210)
  return not complete(ai, Unit.Zerg.Hatchery, 2) or
         not complete(ai, Unit.Zerg.Extractor, 1)
end

local function mutalisk_branch(ai)
  ai.ensure(Unit.Zerg.Lair, 1, 205)
  if not complete(ai, Unit.Zerg.Lair, 1) then
    return
  end
  ai.ensure(Unit.Zerg.Spire, 1, 195)
  if complete(ai, Unit.Zerg.Spire, 1) then
    ai.ensure(Unit.Zerg.Mutalisk, 16, 170)
  end
end

local function hydralisk_branch(ai)
  ai.ensure(Unit.Zerg.HydraliskDen, 1, 205)
  if not complete(ai, Unit.Zerg.HydraliskDen, 1) then
    return
  end
  ai.ensure(Unit.Zerg.Hydralisk, 24, 170)
  ai.ensure(Unit.Zerg.EvolutionChamber, 1, 185)
  ai.upgrade(Upgrade.ZergMissile, after(ai, 720) and 2 or 1, 140)
  ai.upgrade(Upgrade.ZergCarapace, after(ai, 780) and 2 or 1, 135)
end

local function macro_branch(ai)
  ai.ensure(Unit.Zerg.Drone, 24, 175)
  if after(ai, 480) then
    ai.ensure(Unit.Zerg.Hatchery, 3, 195)
  end
  if complete(ai, Unit.Zerg.Hatchery, 3) then
    ai.ensure(Unit.Zerg.Drone, after(ai, 720) and 44 or 32, 170)
    ai.ensure(Unit.Zerg.Extractor, 2, 185)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 5)
  end

  local enemy = ai.enemy_race()
  local opening_active
  if enemy == Race.Zerg then
    opening_active = pool_first_opening(ai)
  else
    opening_active = twelve_hatch_opening(ai)
  end
  if opening_active then
    return
  end
  if establish_gas(ai) then
    return
  end

  if enemy == Race.Protoss then
    hydralisk_branch(ai)
  else
    mutalisk_branch(ai)
  end
  macro_branch(ai)
  attack_at(ai, 10)
end
