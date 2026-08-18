-- Deterministic helpers shared by every recovered-AI replacement strategy.
-- The host owns all simulation state; these functions only submit goals.

if io ~= nil or os ~= nil or package ~= nil or debug ~= nil or
   require ~= nil or dofile ~= nil or loadfile ~= nil or load ~= nil or
   collectgarbage ~= nil or math.random ~= nil or string.dump ~= nil then
  error("unsafe Lua library exposed to AI strategy")
end

TICKS_PER_SECOND = 24

Race = {
  Zerg = 0,
  Terran = 1,
  Protoss = 2
}

function after(ai, seconds)
  return ai.time() >= seconds * TICKS_PER_SECOND
end

function keep_supply(ai, buffer)
  ai.ensure_supply(buffer, 220)
end

function opening_supply_ready(ai, used)
  -- The resource clause lets a recovered or custom map with a rich start
  -- catch up immediately without flattening the normal melee opening.
  return ai.supply_used() >= used or ai.minerals() >= 400
end

function planned(ai, unit_type, quantity)
  return ai.count(unit_type) >= quantity
end

function complete(ai, unit_type, quantity)
  return ai.completed(unit_type) >= quantity
end

function attack_at(ai, army_size)
  if ai.combat_count() >= army_size then
    ai.attack()
  end
end
