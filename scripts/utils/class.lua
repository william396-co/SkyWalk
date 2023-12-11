-- class.lua
class = {
    __name__ = "class",
}
class.__class__ = class

local function rawtostring(t)
    local metatable = getmetatable(t)
    if metatable then
        if metatable.__tostring then
            local tmp = metatable.__tostring
            metatable.__tostring = nil
            local ret = tostring(t)
            metatable.__tostring = tmp
            return ret
        end
    end
    return tostring(t)
end

local MetaClass = { }

MetaClass.__tostring = function (obj)
    return string.format( "<class \"%s\">", obj.__name__)
end

MetaClass.__call = function (_, name, base)
    local class_type = {
        __class__ = class,
        __name__ = name,
        super = base,
        isObj = true,
    }
    local class_object_meta = {
        __index = class_type,
        __tostring = function (obj)
            return string.format( "<%s Object>", obj.__class__.__name__)
        end
    }
    local class_type_meta = {
        __tostring = function (cls)
            return string.format( "<class \"%s\">", cls.__name__)
        end,
        __call = function (cls, ...)
            local object = {
                __class__ = class_type
            }
            setmetatable(object, class_object_meta)
            if object.ctor then
                object.ctor(object, ...)
            end
            if analysisConfig.bMemorySnapshot then
                add_mem_check(object)
            end
            return object
        end
    }

    if class_type.super then
        class_type_meta.__index = class_type.super
    end

    setmetatable(class_type, class_type_meta)
    return class_type
end

setmetatable(class, MetaClass)

class.super = function (class)
    return class.super
end
