local function opening(ai)
  ai.ensure(Unit.Terran.SCV, 9, 165)
  if opening_supply_ready(ai, 7) then
    ai.ensure(Unit.Terran.SupplyDepot, 1, 245)
  end
  if not planned(ai, Unit.Terran.SupplyDepot, 1) then
    return true
  end

  ai.ensure(Unit.Terran.Barracks, 1, 235)
  ai.ensure(Unit.Terran.Refinery, 1, 225)
  return not complete(ai, Unit.Terran.Barracks, 1) or
         not complete(ai, Unit.Terran.Refinery, 1)
end

local function bio_branch(ai)
  ai.ensure(Unit.Terran.SCV, 20, 170)
  ai.ensure(Unit.Terran.Barracks, 3, 215)
  ai.ensure(Unit.Terran.Academy, 1, 205)
  ai.ensure(Unit.Terran.Marine, 30, 165)

  if complete(ai, Unit.Terran.Academy, 1) then
    ai.ensure(Unit.Terran.Firebat, 6, 155)
    ai.research(Tech.StimPacks, 145)
  end
  ai.ensure(Unit.Terran.EngineeringBay, 1, 180)
  ai.upgrade(Upgrade.TerranInfantryWeapons, after(ai, 720) and 2 or 1, 135)
  ai.upgrade(Upgrade.TerranInfantryArmor, after(ai, 780) and 2 or 1, 130)
end

local function mech_branch(ai)
  ai.ensure(Unit.Terran.SCV, 20, 170)
  ai.ensure(Unit.Terran.Marine, 6, 155)
  ai.ensure(Unit.Terran.Factory, 1, 215)

  if complete(ai, Unit.Terran.Factory, 1) then
    ai.ensure(Unit.Terran.MachineShop, 1, 205)
  end
  if complete(ai, Unit.Terran.MachineShop, 1) then
    ai.ensure(Unit.Terran.SiegeTank, 8, 170)
    ai.ensure(Unit.Terran.Factory, 2, 195)
  end
  ai.upgrade(Upgrade.TerranVehicleWeapons, 1, 130)
end

local function macro_branch(ai)
  if after(ai, 360) then
    ai.ensure(Unit.Terran.CommandCenter, 2, 195)
  end
  if complete(ai, Unit.Terran.CommandCenter, 2) then
    ai.ensure(Unit.Terran.SCV, after(ai, 720) and 46 or 32, 165)
    ai.ensure(Unit.Terran.Refinery, 2, 185)
  end
  if after(ai, 720) and complete(ai, Unit.Terran.Factory, 1) then
    ai.ensure(Unit.Terran.Starport, 2, 175)
    ai.ensure(Unit.Terran.ControlTower, 2, 165)
    ai.ensure(Unit.Terran.Wraith, 8, 150)
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
    bio_branch(ai)
  else
    mech_branch(ai)
  end
  macro_branch(ai)
  attack_at(ai, enemy == Race.Zerg and 12 or 10)
end
