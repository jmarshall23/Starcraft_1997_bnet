local function opening(ai)
  ai.ensure(Unit.Terran.SCV, 9, 155)

  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Terran.SupplyDepot, 1, 240)
  end
  if not planned(ai, Unit.Terran.SupplyDepot, 1) then
    return true
  end

  ai.ensure(Unit.Terran.Barracks, 1, 230)
  if not planned(ai, Unit.Terran.Barracks, 1) then
    return true
  end

  ai.ensure(Unit.Terran.Refinery, 1, 215)
  return not complete(ai, Unit.Terran.Barracks, 1)
end

local function bio_against_zerg(ai)
  ai.ensure(Unit.Terran.SCV, 18, 160)
  ai.ensure(Unit.Terran.Barracks, 2, 200)
  ai.ensure(Unit.Terran.Marine, 20, 150)

  if complete(ai, Unit.Terran.Refinery, 1) then
    ai.ensure(Unit.Terran.Academy, 1, 195)
  end
  if complete(ai, Unit.Terran.Academy, 1) then
    ai.ensure(Unit.Terran.Firebat, 4, 145)
    ai.research(Tech.StimPacks, 130)
  end
  if after(ai, 420) then
    ai.ensure(Unit.Terran.EngineeringBay, 1, 170)
    ai.upgrade(Upgrade.TerranInfantryWeapons, 1, 125)
  end
end

local function mech_against_terran_or_protoss(ai)
  ai.ensure(Unit.Terran.SCV, 18, 160)
  ai.ensure(Unit.Terran.Marine, 6, 145)

  if complete(ai, Unit.Terran.Refinery, 1) then
    ai.ensure(Unit.Terran.Factory, 1, 205)
  end
  if complete(ai, Unit.Terran.Factory, 1) then
    ai.ensure(Unit.Terran.MachineShop, 1, 195)
  end
  if complete(ai, Unit.Terran.MachineShop, 1) then
    ai.ensure(Unit.Terran.SiegeTank, after(ai, 540) and 8 or 4, 150)
  end
end

local function expansion(ai)
  if not after(ai, 420) then
    return
  end
  ai.ensure(Unit.Terran.CommandCenter, 2, 175)
  if complete(ai, Unit.Terran.CommandCenter, 2) then
    ai.ensure(Unit.Terran.SCV, 30, 155)
  end
end

function tick(ai)
  if ai.supply_used() >= 7 then
    keep_supply(ai, 4)
  end
  if opening(ai) then
    return
  end

  local enemy = ai.enemy_race()
  if enemy == Race.Zerg then
    bio_against_zerg(ai)
  else
    mech_against_terran_or_protoss(ai)
  end
  expansion(ai)
  attack_at(ai, enemy == Race.Zerg and 16 or 12)
end
