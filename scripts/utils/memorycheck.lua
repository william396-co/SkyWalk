local weakTbl = {}
local waitRemoveTick = 5 * 60 * 1000

setmetatable(weakTbl, {
    __mode = "v"
})

function add_mem_check(tbl)
    table.insert(weakTbl, tbl)
end

function print_weak()
    local account = {}
    local detailedAccount = {}
    local time = now(true)
    local snapshotCount = 5
    for k, v in pairs(weakTbl) do
        if v.__name__ then
            account[v.__name__] = (account[v.__name__] or 0) + 1
            if v.__istable__ then
                local size = 0
                for _, _ in pairs(v) do
                    size = size + 1
                end
                detailedAccount[v.__name__] = (detailedAccount[v.__name__] or 0) + size
            end
            if v.__removeTime and (v.__removeTime + waitRemoveTick < time) and snapshotCount > 0 then
                snapshotCount = snapshotCount - 1
                memorySingleSnapshot(v)
                weakTbl[k] = nil
            end
        end
    end
    
    local sortTbl = {}
    for k, v in pairs(account) do
        table.insert(sortTbl, {k = k, v = v})
    end
    table.sort(sortTbl, function(a, b)
        return a.v > b.v
    end)
    local str = "snapshot weak-->\n"
    for k, v in ipairs(sortTbl) do
        str = string.format("%ssnapshot weak-- %-40s count:%10s size:%s\n", str, v.k, v.v, detailedAccount[v.k])
    end
    DEBUG(str)
end
