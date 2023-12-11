-- Gamed入口脚本
--
-- ROOT: 生产环境的相对根目录
-- ENV: 游戏逻辑环境
--
-- logger : 日志记录
-- traverse : 遍历目录
--

require 'utils.stringex'
local factory = require 'protocol/factory'

--
-- DEBUG
--
local key = ""
function print_table(arg, level)
	level = level or 1
	local indent = ""
	for i = 1, level do
		indent = indent.."  "
	end

	if key ~= "" then
    	print(indent..key.." ".."=".." ".."{")
	else
		print(indent .. "{")
	end

	key = ""
	for k, v in pairs(arg) do
		if type( v ) ~= "table" then
			local content = string.format("%s%s = %s", indent .. "  ",tostring(k), tostring(v))
			print(content)
		else
			key = k
			print_table( v, level + 1 )
		end
	end
	print( indent .. "}")
end

--
-- 日志宏定义
--
function LOG_FATAL( msg ) logger( 1, msg ) end
function LOG_ERROR( msg ) logger( 2, msg ) end
function LOG_WARN( msg )  logger( 3, msg ) end
function LOG_INFO( msg )  logger( 4, msg ) end
function LOG_TRACE( msg ) logger( 5, msg ) end
function LOG_DEBUG( msg ) logger( 6, msg ) end

--
-- AppStart启动
-- 根据不同的服务器加载
--
function appstart( app )
	if app == 'scene' then
		-- 场景服务器
		require 'scene.routine'
	elseif app == 'world' then
		-- 世界服务器
		require 'world.routine'
	else
		-- 其他服务
	end
end

--
-- AppReload重启
--
function appreload()
end

--
-- AppStop
--
function appstop()
end

--
-- AppHooks
--
function apphooks()
	return factory:hooks()
end
