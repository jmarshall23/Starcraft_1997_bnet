local function opening(ai)
  ai.ensure(Unit.Terran.SCV, 8, 150)

  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Terran.SupplyDepot, 1, 235)
  end
  if not planned(ai, Unit.Terran.SupplyDepot, 1) then
    return true
  end

  ai.ensure(Unit.Terran.Barracks, 1, 225)
  return not complete(ai, Unit.Terran.Barracks, 1)
end

local function early_game(ai)
  ai.ensure(Unit.Terran.SCV, 14, 155)
  ai.ensure(Unit.Terran.Marine, 8, 145)
  ai.ensure(Unit.Terran.Refinery, 1, 205)

  if complete(ai, Unit.Terran.Refinery, 1) then
    ai.ensure(Unit.Terran.Academy, 1, 190)
    ai.ensure(Unit.Terran.Firebat, 2, 135)
  end
end

local function mid_game(ai)
  ai.ensure(Unit.Terran.SCV, 20, 150)
  ai.ensure(Unit.Terran.Barracks, 2, 185)
  ai.ensure(Unit.Terran.Marine, 16, 145)
  ai.ensure(Unit.Terran.EngineeringBay, 1, 165)
  ai.upgrade(Upgrade.TerranInfantryWeapons, 1, 120)

  if after(ai, 600) then
    ai.ensure(Unit.Terran.CommandCenter, 2, 155)
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
