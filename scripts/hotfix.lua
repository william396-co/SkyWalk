local isdebug = false
local realenv = _ENV
local debug = debug
local string = string
local print = print
local getupvalue = debug.getupvalue
local upvaluejoin = debug.upvaluejoin

local function DEBUG(str, ...)
	if isdebug then
		print("[DEBUG] " .. string.format(str, ...))
	end
end

local function WARN(str, ...)
	print("[WARN ] " .. string.format(str, ...))
end

local function ERROR(str, ...)
	print("[ERROR] " .. string.format(str, ...))
end

-- only supports lua 5.3
local function getfenv(f)
	if not f then
		return nil
	end

	local i = 1
	while true do
		local name, value = getupvalue(f, i)
		if name == "_ENV" then
			return value
		elseif not name then
			break
		end
		i = i + 1
	end
end

local function setfenv(f, env)
	if not f then
		return nil
	end

	local i = 1
	while true do
		local name, value = getupvalue(f, i)
		if name == "_ENV" then
			upvaluejoin(f, i, (function()
				return env
			end), 1)
			break
		elseif not name then
			break
		end
		i = i + 1
	end
end

local hotfix = {}
local fakeenv = {}

function hotfix.require(modname)
	DEBUG("require: %s", modname)
	local loader, path = hotfix:find_loader(modname)
	if string.find(path,".so") ~= nil then
	  return realenv["require"](modname)
	else
	  if loader == nil then
		  -- pcall can catch it
		  error(string.format("require:%s failed", modname))
		  return
	  end
	  setfenv(loader, hotfix.fakeenv)
	  return loader()
	end
end

hotfix.inner_func = {
	require = hotfix.require,
}

function fakeenv:new()
	local fake_env = {}
	setmetatable(fake_env, {__index = function(t, k)
		if hotfix.inner_func[k] then
			return hotfix.inner_func[k]
		else
			return realenv[k]
		end
	end})

	return fake_env
end

-- clone table without function
function hotfix:clone_table(tb, name)
	local res = {}
	for k, v in pairs(tb) do
		local key_type = type(k)
		local value_type = type(v)

		if key_type == "function" or value_type == "function" then
			goto continue
		end

		if value_type == "table" then
			assert(key_type == "string")
			local cbmt = hotfix:clone_table(v, k)
			local cb = setmetatable({}, cbmt)
			res[k] = cb
			goto continue
		end

		res[k] = v
		::continue::
	end

	res.name = name

	res.__newindex = function(tb, k, v)
		if not name then
			name = "_RELOADENV"
		end
		if not hotfix.reload_map[name] then
			hotfix.reload_map[name] = {}
		end
		table.insert(hotfix.reload_map[name], {k, v})
	end

	res.__index = function(tb, k)
		if res[k] then
			return res[k]
		end
	end

	return res
end

local function _update_value(oldtb, newtb)
	for _, v in ipairs(newtb) do
		local name = v[1]
		local value = v[2]

		DEBUG("_update_value %s:%s", name, value)
		rawset(oldtb, name, value)
		::continue::
	end
end

function hotfix:reload_value(reload_map)
	for name, obj in pairs(reload_map) do
		if name == "_RELOADENV" then
			_update_value(realenv, obj)
		else
			if not realenv[name] then
				ERROR("can not find %s in _ENV", name)
				goto continue
			end
			_update_value(realenv[name], obj)
		end
		::continue::
	end

end

-- check chunk type and update chunk
-- module can return a table or function
-- other types? (boolean)
function hotfix:check_chunk_type(oldchunk, chunk)
	local chunkey_type = type(chunk)
	local oldchunkey_type = type(oldchunk)

	if oldchunkey_type ~= "boolean" and oldchunk == chunk then
		--ERROR("oldchunk == chunk and not return")
		--return false
	end

	if oldchunk ~= nil and oldchunkey_type ~= chunkey_type then
		ERROR("oldchunkey_type:%s != chunkey_type:%s", oldchunkey_type, chunkey_type)
		return false
	end

	DEBUG("chunkey_type is %s", chunkey_type)

	-- 1. get a new object
	-- 2. compare two objects
	-- maybe a gloabl variable or a function
	for name, newobj in pairs(hotfix.fakeenv) do
		hotfix.new_object_map[name] = newobj

		local newobj_type = type(newobj)
		local oldobj = rawget(realenv, name)
		local oldobj_type = type(oldobj)

		if newobj_type == "function" then
			hotfix:reload_function(name, oldobj, newobj)
		elseif newobj_type == "table" then
			if not oldobj then
				oldobj = rawset(realenv, name, {})
			end
			hotfix:reload_table(oldobj, newobj)
		elseif valuetype == "userdata" or valuetype == "thread" then
			WARN("not support %s, %s", name, valuetype)
		else
			if not oldobj then
				DEBUG("automatic updates, objType:%s, name:%s, obj:%s",
					newobj_type, name, tostring(newobj))
				rawset(realenv, name, newobj)
			else
				WARN("check no automatic updates, objType:%s, name:%s, oldobj:%s, newobj:%s",
					newobj_type, name, tostring(oldobj), tostring(newobj))
			end
		end
	end

	-- 3. call __RELOAD function if it exists
	local __reload = hotfix.fakeenv["__RELOAD"]
	local __reloadType = type(__reload)
	if __reloadType == "nil" then
		return
	elseif __reloadType ~= "function" then
		ERROR("__RELOAD not a function")
		return
	end

	local reloadEnvMT = hotfix:clone_table(hotfix.fakeenv)
	setmetatable(reloadEnvMT, {__index = hotfix.fakeenv})

	local reloadEnv = setmetatable({}, reloadEnvMT)
	setfenv(__reload, reloadEnv)

	local succ = pcall(__reload,chunk)
	-- __reload()

	if not succ then
		ERROR("call __RELOAD failed")
	end

	hotfix:reload_value(hotfix.reload_map)
end

-- if find succ, return function and module path
function hotfix:find_loader(name)
	local errMsg = {}
	for _, loader in ipairs(package.searchers) do
		local succ, ret, path = pcall(loader, name)
		if not succ then
			ERROR("find_loader:%s", ret)
			return nil
		end

		local retType = type(ret)

		if retType == "function" then
			return ret, path
		elseif retType == "string" then
			table.insert(errMsg, ret)
		end
	end
	ERROR("module:%s not found:%s", name, table.concat(errMsg))
	return nil
end

function hotfix:reload(modname)
	if package.loaded[modname] == nil then
		ERROR("module:%s is not exists", modname)
		return
	end
	local loader, path = hotfix:find_loader(modname)
	if loader == nil then
		ERROR("module:%s can not find path", modname)
		return
	end
	hotfix:reload_chunk(modname, path)
end

-- modname: package.loaded[?]
-- src: file path
function hotfix:reload_chunk(modname, src)
	hotfix:reset()

	local oldchunk = package.loaded[modname]
	if oldchunk == nil then
		ERROR("oldchunk is nil")
		return false
	end

	local isnt_ret_flag = false
	if type(oldchunk) == "boolean" then
		isnt_ret_flag = true
	end

	-- must be a chunk or true...
	local ckOrBool = hotfix:build_chunk(src, isnt_ret_flag)

	if not ckOrBool then
		ERROR("reload_chunk failed")
		return false
	end

	-- ckOrBool is a chunk or boolean
	hotfix:reload_local(oldchunk,ckOrBool)
	hotfix:check_chunk_type(oldchunk, ckOrBool)
	hotfix:clear()
	return ckOrBool
end

function hotfix:copy_table(oldtable,newtable,reloaddata_flag)
	for key,value in pairs(newtable) do
		local newobj_type = type(value)
		if newobj_type ~= 'table' then
		  if newobj_type == 'function' then
			oldtable[key] = value
		  elseif newobj_type == "userdata" or newobj_type == "thread" then
			WARN("not support %s, %s", key, newobj_type)
		  else
			if reloaddata_flag ~= nil then
			  oldtable[key] = value
			end
		  end
		else
		  local tag = string.format("%s %s", tostring(oldtable), tostring(newtable))
		  if hotfix.table_mark[tag] then
			  DEBUG("reload_table is same %s", tag)
			  return
		  end
		  hotfix.table_mark[tag] = true
		  hotfix:copy_table(oldtable[key],value,reloaddata_flag)
		end
	end
end

function hotfix:reload_local(oldchunk,newchunk)
	local chunkey_type = type(newchunk)
	local oldchunkey_type = type(oldchunk)
	if oldchunk ~= nil and oldchunkey_type ~= chunkey_type then
		ERROR("oldchunkey_type:%s != chunkey_type:%s", oldchunkey_type, chunkey_type)
		return false
	end

	local metatable = getmetatable(newchunk)
	if metatable ~= nil then
		setmetatable(oldchunk, metatable)
	end

	local reloaddata_flag = nil
	if chunkey_type == 'table' then
		reloaddata_flag = rawget(newchunk, 'reloaddata') --是否更新数据,默认nil,不更新数据
	end

	if chunkey_type == 'table' then
		for name,newobj in pairs(newchunk) do
			local newobj_type = type(newobj)
			if newobj_type == "table" then
				hotfix:copy_table(oldchunk[name],newobj,reloaddata_flag)
			elseif newobj_type == "userdata" or newobj_type == "thread" then
				WARN("not support %s, %s", name, newobj_type)
			else
				if newobj_type == 'function' then
					oldchunk[name] = newobj
				else
					if newobj_type ~= nil then
						oldchunk[name] = newobj
					end
				end
			end
		end
	end
end

function hotfix:reload_function(name, oldfunc, newfunc)
	if hotfix.func_mark[name] then
		return
	end
	hotfix.func_mark[name] = true

	DEBUG("--------------- reload_function ----- -----------------")
	DEBUG("reload_function name: %s", name)
	hotfix:reload_upvalues(oldfunc, newfunc)
	local env = getfenv(oldfunc) or realenv
	setfenv(newfunc, env)
	DEBUG("--------------- reload_function END -----------------")

end

function hotfix:reload_upvalues(oldfunc, newfunc)
	if not oldfunc then
		DEBUG("not OldFunc")
		return
	end
	DEBUG("--------------- reload_upvalues -----------------")
	-- k: name, v: {index, value}
	local upvaluemap = {}
	-- k: name, v: is exists(boolean)
	local namemap = {}

	local i = 1
	while true do
		local name, value = getupvalue(oldfunc, i)
		if not name then
			break
		end

		DEBUG("old upvalue, %s, %s", name, value)

		upvaluemap[name] = {
			index = i,
			value = value
		}

		namemap[name] = true
		i = i + 1
	end

	i = 1
	while true do
		local name, value = getupvalue(newfunc, i)
		if not name then
			break
		end

		-- upvalue exists in the oldchunk
		if namemap[name] then
			local oldvalue = upvaluemap[name].value
			local oldvalue_index = upvaluemap[name].index
			local oldvalue_type = type(oldvalue)

			local valuetype = type(value)

			DEBUG("new upvalue, name:%s, oldvalue_type:%s, valuetype:%s", name, oldvalue_type, valuetype)

			if oldvalue_type ~= valuetype then
				ERROR("reload_upvalues oldvalue_type:%s != valuetype:%s",
					oldvalue_type, valuetype)
				goto continue
			end

			DEBUG("%s is a %s", name, valuetype)

			if valuetype == "table" then
				hotfix:reload_table(oldvalue, value)
				upvaluejoin(newfunc, i, oldfunc, oldvalue_index)
			elseif valuetype == "function" then
				hotfix:reload_function(name, oldvalue, value)
				DEBUG("reload_function ", name, value)
			elseif valuetype == "userdata" or valuetype == "thread" then
				WARN("not support %s, %s", name, valuetype)
			else
				local tag = string.format("%s %s", tostring(newfunc), tostring(name))
				if hotfix.fix_upvalue_map[tag] == true then
					goto continue
				end
				DEBUG("upvaluejoin %s, oldvalue_index:%d, type:%s", name, oldvalue_index, oldvalue_type)
				upvaluejoin(newfunc, i, oldfunc, oldvalue_index)
			end
		else
			DEBUG("set new upvalue, name:%s, value:%s, idx:%d", name, value, i)
			debug.setupvalue(newfunc, i, value)
			local tag = string.format("%s %s", tostring(newfunc), tostring(name))
			hotfix.fix_upvalue_map[tag] = true
		end

		::continue::
		i = i + 1
	end
	DEBUG("---------------- reload_upvalues end-----------------")

end

function hotfix:reload_table(oldtable, newtable)
	if oldtable == newtable then
		return
	end

	local tag = string.format("%s %s", tostring(oldtable), tostring(newtable))
	if hotfix.table_mark[tag] then
		DEBUG("reload_table is same %s", tag)
		return
	end
	hotfix.table_mark[tag] = true

	for name, value in pairs(newtable) do
		if oldtable == nil then
			goto continue
		end
		local oldvalue = rawget(oldtable, name)
		local oldvalue_type = type(oldvalue)

		local valuetype = type(value)

		DEBUG("reload_table, name:%s, oldvalue_type:%s newvalue_type:%s",
			name, oldvalue_type, valuetype)

		if valuetype == "function" then
			hotfix:reload_function(name, oldvalue, value)
			DEBUG("set oldtable[%s] = %s", name, tostring(value))
			rawset(oldtable, name, value)
		elseif valuetype == "table" then
			hotfix:reload_table(oldvalue, value)
		else
			if oldvalue == value then
				goto continue
			end
			rawset(oldtable, name, value)
			--[[
			if not oldvalue then
				DEBUG("set new variable, objType:%s, name:%s, newobj:%s",
					valuetype, name, value)
				rawset(oldtable, name, value)
			else
				WARN("table no automatic updates, objType:%s, name:%s, oldobj:%s, newobj:%s",
					valuetype, name, oldvalue, value)
			end
			--]]
		end

		::continue::
	end

	if newtable == hotfix.fakeenv then
		return
	end

	local oldmetatable = getmetatable(oldtable)
	local metatable = getmetatable(newtable)
	local oldmetatable_type = type(oldmetatable)
	local metatableType = type(metatable)
	if oldmetatable_type == metatableType and metatableType == "table" then
		hotfix:reload_table(oldmetatable, metatable)
	end
end

-- return chunk
-- when isnt_ret_flag == true, return true
function hotfix:build_chunk(src, isnt_ret_flag)
	hotfix.fakeenv = fakeenv:new()

	local chunk, err = loadfile(src, "bt", hotfix.fakeenv)
	if not chunk and err then
		ERROR("build_chunk failed, err: %s", err)
		return nil
	end

	local succ, ck = pcall(function() return chunk() end)
	if not succ then
		ERROR("build_chunk failed ck: %s", ck)
		return nil
	end

	if ck and not isnt_ret_flag then
		return ck
	elseif isnt_ret_flag then
		return true
	else
		return nil
	end
end

function hotfix:clear()
	hotfix.new_object_map = nil
	hotfix.table_mark = nil
	hotfix.require_map = nil
	hotfix.reload_map = nil
	hotfix.fakeenv = nil
	hotfix.fix_upvalue_map = nil
	hotfix.func_mark = nil
end

function hotfix:reset()
	hotfix.new_object_map = {}
	hotfix.table_mark = {}
	hotfix.require_map = {}
	hotfix.reload_map = {}
	hotfix.fix_upvalue_map = {}
	hotfix.func_mark = {}
end

return hotfix
