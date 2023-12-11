local tconcat = table.concat
local tinsert = table.insert
local call_stack = 0

local function _traceback()
    return "\n" .. debug.traceback("lua traceback", 3)
end

local function _dump(t, out, head)
    call_stack = call_stack + 1
    if t == nil or call_stack > 10 then
        call_stack = call_stack - 1
        if call_stack >= 10 then
            return "dump overflow."
        else
            return
        end
    end

    local tt = type(t)
    if tt == "table" then
        if head ~= nil then
            tinsert(out, tostring(head) .. ":{")
        else
            tinsert(out, "{")
        end
        for k, v in pairs(t) do
            _dump(v, out, k)
        end
        tinsert(out, "}")
    else
        if tt == "string" and (tt == "" or t == " ") then
            return
        end
        if head then
            tinsert(out, tostring(head) .. ":" .. tostring(t) .. " ")
        else
            tinsert(out, tostring(t) .. " ")
        end
    end
    call_stack = call_stack - 1
end

--declare_global("log_dump")
function log_dump(head, content,t)
    -- 不需要二次判断print
    -- if get_global('print') == false then
    --   return
    -- end

    -- local out = {}
    -- call_stack = 0
    -- _dump(t, out)
    -- print(tostring(head) .. '\t' ..tconcat(out) .. _traceback())

    local value = t
    local desciption = "DEBUG INFO"

    local lookupTable = {}
    local result = {}

    local function dump_value_(v)
        if type(v) == "string" then
            v = '"' .. v .. '"'
        end
        return tostring(v) or "nil"
    end

    local function dump_(value, desciption, indent, nest, keylen)
        local spc = ""
        if type(keylen) == "number" then
            spc = string.rep(" ", keylen - string.len(dump_value_(desciption)))
        end
        if type(value) ~= "table" then
            result[#result + 1] =
                string.format("%s[%s]%s = %s,", indent, dump_value_(desciption), spc, dump_value_(value))
        elseif lookupTable[tostring(value)] then
            result[#result + 1] = string.format("%s[%s]%s = '*REF*',", indent, dump_value_(desciption), spc)
        else
            lookupTable[tostring(value)] = true
            result[#result + 1] = string.format("%s[%s] = {", indent, dump_value_(desciption))
            local indent2 = indent .. "    "
            local keys = {}
            local keylen = 0
            local values = {}
            for k, v in pairs(value) do
                keys[#keys + 1] = k
                local vk = dump_value_(k)
                local vkl = string.len(vk)
                if vkl > keylen then
                    keylen = vkl
                end
                values[k] = v
            end
            table.sort(keys,function(a, b)
              if type(a) == "number" and type(b) == "number" then
                return a < b
              else
                return tostring(a) < tostring(b)
              end
            end)
            for k, v in ipairs(keys) do
                dump_(values[v], v, indent2, nest + 1, keylen)
            end
            result[#result + 1] = string.format("%s},", indent)
        end
    end
    dump_(value, desciption, "", 1)
    content = content..head..'\n'.. table.concat(result, "\n")
    print(content)
end

--declare_global("log_info")
function log_info(head, t, ...)

    if get_global("print") == nil or get_global("print") == false then
        return
    end

    if t == nil then
      t = head
      head = ''
    end

    if type(t) == "table" then
      local traceback = string.split(debug.traceback("", 2), "\n")
      local content = ''
      content = content..(string.trim(traceback[3]).."\n")
      log_dump(head,content,t)
    else
        local content = {t, ...}
        for k = 1, #content do
            content[k] = tostring(content[k])
        end
        return print("[DEBUG INFO]" .. tostring(head) .. "\t" .. tconcat(content, " ") .. _traceback())
    end
end

local isdebug = false

--declare_global("DEBUG")
function DEBUG(str, ...)
    -- logger(6, string.format(str, ...) .. '\n')
    local traceback = string.split(debug.traceback("", 3), "\n")
    logger(6, string.format(str, ...) .. '\n')
    if traceback[4] then
        logger(6, traceback[4] .. '\n')
    end
end

--declare_global("WARN")
function WARN(str, ...)
    print("[WARN] " .. string.format(str, ...))
end

--declare_global("ERROR")
function ERROR(str, ...)
    local traceback = string.split(debug.traceback("", 3), "\n")
    logger(2, string.format(str, ...) .. '\n')
    if traceback[4] then
        logger(2, traceback[4] .. '\n')
    end
end

--declare_global("log_error")
function log_error(...)
    if get_global("__erorr_print") == nil then
        return
    end

    local c = {...}
    for k = 1, #c do
        c[k] = tostring(c[k])
    end
    if #c > 0 then
        __erorr_print(tconcat(c, "|") .. _traceback())
    end
end

--declare_global("EDITOR_DEBUG")
function EDITOR_DEBUG(str, ...)
    if get_global("__editor_debug") == nil then
        return
    end
    __editor_debug(string.format(str, ...))
end
