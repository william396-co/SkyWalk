-- --
-- -- 禁止隐式声明、访问全局变量
-- --

local function _traceback()
  return '\n' .. debug.traceback('lua traceback', 3)
end

local function _mt_index(_, n)
	error( "Attempt to read undeclared global variable: " .. n .. _traceback() )
end

function declare_global( name, init_value )
	if rawget(_G, name) then
		return
	end
	rawset( _G, name, init_value or false )
end

function del_global(name)
  rawset( _G, name, nil )
end

function get_global( name )
	return rawget( _G, name )
end

function readonly_table(t)
	assert(type(t) == 'table', 'invalid type')
	setmetatable( t, {
		__newindex = function ( _, n )
			error( "Attempt to write to undeclared global variable: " .. n .. _traceback() )
		end,
		__index = _mt_index,
	} )

end

function declare_enum(t)
	readonly_table(t)
end

setmetatable(_G, {
	__newindex = function(tb, key, value)
		error(string.format("can not define global value：%s in local space", key))
	end
})

--readonly_table(_G)

