
-- 字符串转时间戳
-- 1970-01-01 08:00:00
function totimestamp( str )
	local year, month, day, hour, minute, second = string.match( str, "(%d+)-(%d+)-(%d+)%s(%d+):(%d+):(%d+)" )

	if ( year and month and day and hour and minute and second ) then
		local d = { year=year, month=month, day=day, hour=hour, min=minute, sec=second, isdst=false }
		return os.time( d )
	end

	return nil
end

-- 是否隔天
-- dailytime - "00:00:00"
function is_over_time( timestamp, dailytime, now )
    -- 0 直接隔天
	if timestamp == 0 then
		return true
	end
	local oneday = 3600 * 24
	local hour,minute,second = string.match( dailytime, "(%d+):(%d+):(%d+)" )
	local today = os.date("*t", now)
	local refreshpoint = os.time( {day=today.day, month=today.month,year=today.year,hour=hour,minute=minute,second=second} )
	if refreshpoint <= 0 then
		return false
	end
    -- 今后的时间也是隔天
	if timestamp > refreshpoint + oneday then
		return true
	end
	if now >= refreshpoint then
		if timestamp < refreshpoint then
			return true
		end
	else
		if timestamp < refreshpoint - oneday then
			return true
		end
	end
	return false
end

-- Trim
-- 去除字符串首尾的空格
function trim( str )
	return ( string.gsub(str, "^%s*(.-)%s*$", "%1") )
end

-- json叶子节点
local function json2leaf( str, from, to )
	local element = string.sub( str, from, to )

	-- 优先转换成number
	local number = tonumber( element )
	if number then
		return number
	end

	-- 其次转换成时间戳
	local timestamp = totimestamp( element )
	if timestamp then
		return timestamp
	end

	-- 最后才是字符串
	return trim( element )
end

-- json转换成Table
function json2table( str, from, to )
    local result = {}

    from = from or 1
    local pos = 1
    local i = from+1
    local to = to or string.len(str)

    while ( i <= to ) do
        local c = string.sub( str, i, i )

        if c == ',' or c == ']' then
            result[ #result+1 ] = json2leaf( str, pos+1, i-1 )
            pos = i
        elseif c == '[' then
			result[ #result+1 ] = json2table( str, pos+1, i-1 )
			pos = i
        end

        i = i + 1
    end

    return result
end
