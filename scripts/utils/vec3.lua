
-- namespace
local vec3 = {}

-- meta and prototype
vec3.meta = {}
vec3.prototype = {x = 0, y = 0, z = 0}

--
-- vec3.meta
--
vec3.meta.__index = vec3
--vec3.meta.__index = vec3.prototype

vec3.meta.__newindex = function( self, key, value )
	if vec3.prototype[ key ] ~= nil then
		rawset( self, key, value )
	end
end

vec3.meta.__add = function( a, b )
	local res = vec3.new()
	res.x = a.x + b.x
	res.y = a.y + b.y
	res.z = a.z + b.z
	return res
end

vec3.meta.__mul = function( a, b )
	local res = vec3.new()
	res.x = a.x * b
	res.y = a.y * b
	res.z = a.z * b
	return res
end

vec3.meta.__sub = function( a, b )
	local res = vec3.new()
	res.x = a.x - b.x
	res.y = a.y - b.y
	res.z = a.z - b.z
	return res
end

vec3.meta.__div = function( a, b )
	local inva = 1.0 / b
	local res = vec3.new()
	res.x = a.x * inva
	res.y = a.y * inva
	res.z = a.z * inva
	return res
end

vec3.meta.__eq = function( a, b )
	return a.x == b.x and a.y == b.y and a.z == b.z
end

vec3.meta.__tostring = function( v )
	local a = "vec3 = {"
	a = a .. v.x .. ", "
	a = a .. v.y .. ", "
	a = a .. v.z .. "}"
	return a
end

--
-- vec3
--

function vec3.new( o )
	o = o or vec3.prototype
	return setmetatable( o, vec3.meta )
end

function vec3.length( a )
	return math.sqrt( a.x * a.x + a.y * a.y + a.z * a.z )
	--return 0
end

function vec3.distance( a, b )
	return math.sqrt( (a.x - b.x)^2 + (a.y - b.y)^2 + (a.z - b.z)^2 )

end

function vec3.compare( a, b, epsilon )
	if math.abs( a.x - b.x ) > epsilon then
		return false
	end
	if math.abs( a.y - b.y ) > epsilon then
		return false
	end
	if math.abs( a.z - b.z ) > epsilon then
		return false
	end
	return true
end

return vec3
