function string.split(input, delimiter)
	input = tostring(input)
	delimiter = tostring(delimiter)
	if (delimiter == '') then return false end
	local pos, arr = 0, {}
	-- for each divider found
	for st, sp in function() return string.find(input, delimiter, pos, true) end do
		table.insert(arr, string.sub(input, pos, st - 1))
		pos = sp + 1
	end
	table.insert(arr, string.sub(input, pos))
	return arr
end

function string.ltrim(input)
	return string.gsub(input, "^[ \t\n\r]+", "")
end

function string.rtrim(input)
	return string.gsub(input, "[ \t\n\r]+$", "")
end

function string.trim(input)
	input = string.gsub(input, "^[ \t\n\r]+", "")
	return string.gsub(input, "[ \t\n\r]+$", "")
end

function string.ucfirst(input)
	return string.upper(string.sub(input, 1, 1)) .. string.sub(input, 2)
end

function string.utf8len(input)
	local len = string.len(input)
	local left = len
	local cnt = 0
	local arr = {0, 192, 224, 240, 248, 252}
	while left ~= 0 do
		local tmp = string.byte(input, - left)
		local i = #arr
		while arr[i] do
			if tmp >= arr[i] then
				left = left - i
				break
			end
			i = i - 1
		end
		cnt = cnt + 1
	end
	return cnt
end

function string.endswith(input, substr)
	if input == nil or substr == nil then
		return nil, "the string or the sub-string parameter is nil"
	end
	local str_tmp = string.reverse(input)
	local substr_tmp = string.reverse(substr)
	if string.find(str_tmp, substr_tmp) ~= 1 then
		return false
	else
		return true
	end
end

function string.startsWith(input, substr)
	if string.find(input, substr) ~= 1 then
		return false
	else
		return true
	end
end

--目前只支持{0},{1},{2}这种简单的。还不支持内容还有其它的结构
function string.orderfmtCS(str, ...)
	local args = {...}
	return string.gsub(str, "{%d.-}", function(s)
		local n = tonumber(string.gmatch(s, "%d.-")())
		return tostring(args[n+1])
	end)
end


function string.shortStr(sName,nMaxCount,nShowCount)
	if sName == nil or nMaxCount == nil then
	  return;
	end
	local sStr = sName;
	local tCode = {};
	local tName = {};
	local nLenInByte = #sStr;
	local nWidth = 0;
	if nShowCount == nil then
	  nShowCount = nMaxCount;
	end
	for i=1,nLenInByte do
	  local curByte = string.byte(sStr,i);
	  local byteCount = 0;
	  if curByte > 0 and curByte <=127 then
		byteCount = 1;
	  elseif curByte>=192 and curByte<223 then
		byteCount = 2;
	  elseif curByte>=224 and curByte<239 then
		byteCount = 3;
	  elseif curByte>=240 and curByte<247 then
		byteCount = 4;
	  end
	  local char = nil;
	  if byteCount > 0 then
		char = string.sub(sStr, i, i + byteCount - 1);
		i = i + byteCount - 1;
	  end
	  if byteCount == 1 then
		nWidth = nWidth + 1;
		table.insert(tName,char);
		table.insert(tCode,1);
	  elseif byteCount > 1 then
		nWidth = nWidth + 1;
		table.insert(tName,char);
		table.insert(tCode,1);
	  end
	end
	if nWidth > nMaxCount then
	  local _sN = "";
	  local _len = 0;
	  for i=1,#tName do
		_sN = _sN .. tName[i];
		_len = _len + tCode[i];
		if _len >= nShowCount then
		  break;
		end
	  end
	sName = _sN;
	end
	return sName;
  end
