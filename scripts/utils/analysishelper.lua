local lua_profiler = require "utils.lua_profiler"
local mri = require "utils.memoryreferenceinfo"

local profileName = os.date("%Y%m%d-%H%M%S", os.time())
local snapshotCount = 0

analysisConfig = {
    bProCheck = false,    
    bMemorySnapshot = false,
    bCheckTimer = false,
}

function startProCheck()
    -- local overrides = {
    --     fW = 50,
    --     fnW = 30,
    --     tW = 12,
    -- }
    -- lua_profiler.configuration(overrides)
    -- lua_profiler.start()
    collectgarbage("stop")
    analysisConfig.bProCheck = true
end

function stopProCheck()
    -- lua_profiler.stop()
    -- lua_profiler.report("debug/lua_" .. profileName .. ".pro")
    analysisConfig.bProCheck = false
    gWorldMgr:print()
    da_report()
end

function memorySnapshot()
    collectgarbage("collect")
    snapshotCount = snapshotCount + 1
    mri.m_cMethods.DumpMemorySnapshot("debug/", string.format("lua_mri_%s_%s", profileName, snapshotCount), -1)
end

function memorySingleSnapshot(object, name)
    collectgarbage("collect")
    local filename = ""
    if object.id then
        filename = filename .. "_" .. object.id
    end
    if name then
        filename = filename .. "_" .. name
    end
    mri.m_cMethods.DumpMemorySnapshotSingleObject("debug/", filename, -1, "object", object)
end

function memoryObjectsSnapshot(objects, name)
    collectgarbage("collect")
    local filename = "all_objects"
    if name then
        filename = filename .. "_" .. name
    end
    mri.m_cMethods.DumpMemorySnapshotObjects("debug/", filename, -1, "objects", objects)
end

function startMemorySnapshot()
    analysisConfig.bMemorySnapshot = true
end

function stopMemorySnapshot()
    analysisConfig.bMemorySnapshot = false
end
