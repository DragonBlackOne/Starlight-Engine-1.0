-- NewsGenerator.lua
News = {}
News.ActiveEvents = {}
News.Archive = {}

local eventTypes = {
    { title = "Tech Breakthrough", sector = "Tech", driftMod = 0.15, volMod = 0.05, duration = 10 },
    { title = "Energy Crisis", sector = "Energy", driftMod = -0.2, volMod = 0.3, duration = 15 },
    { title = "Market Bubble", sector = "All", driftMod = 0.1, volMod = 0.2, duration = 20 },
    { title = "Global Recession", sector = "All", driftMod = -0.15, volMod = 0.1, duration = 30 },
    { title = "FDA Approval", sector = "Health", driftMod = 0.25, volMod = 0.15, duration = 8 },
}

function News.Init()
    Engine.log("NewsGenerator: Initialized.")
end

function News.Update(dt)
    -- Chance to trigger a new event (very low per frame)
    if math.random() < 0.001 then
        News.TriggerRandomEvent()
    end
    
    -- Update active events
    for i = #News.ActiveEvents, 1, -1 do
        local ev = News.ActiveEvents[i]
        ev.duration = ev.duration - dt
        if ev.duration <= 0 then
            News.EndEvent(ev, i)
        end
    end
end

function News.TriggerRandomEvent()
    local template = eventTypes[math.random(#eventTypes)]
    local ev = {
        title = template.title,
        sector = template.sector,
        driftMod = template.driftMod,
        volMod = template.volMod,
        duration = template.duration,
        startTime = Engine.get_time()
    }
    
    table.insert(News.ActiveEvents, ev)
    Engine.log("NEWS ALERT: " .. ev.title .. " affecting " .. ev.sector)
    
    -- Apply modifiers to companies
    for _, comp in ipairs(Market.Companies) do
        if ev.sector == "All" or comp.sector == ev.sector then
            comp.drift = comp.drift + ev.driftMod
            comp.volatility = comp.volatility + ev.volMod
        end
    end
end

function News.EndEvent(ev, index)
    -- Revert modifiers
    for _, comp in ipairs(Market.Companies) do
        if ev.sector == "All" or comp.sector == ev.sector then
            comp.drift = comp.drift - ev.driftMod
            comp.volatility = comp.volatility - ev.volMod
        end
    end
    table.remove(News.ActiveEvents, index)
    table.insert(News.Archive, ev)
end
