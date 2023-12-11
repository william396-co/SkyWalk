
-- api
local random = {}

-- meta and prototype
random.meta = { __index = random }
random.prototype = { seed_ = 0, rawseed_ = 0 }

--
-- random
--

function random.new( seed )
	local obj =
	{
		seed_ = seed,
		rawseed_ = seed
	}
	return setmetatable( obj, random.meta )
end

function random.rand( self )
	local product = 0
	local A = 16807
	local M = 2147483647
	product = self.seed_ * A
	self.seed_ = (product >> 31) + ( product & M )
	if ( self.seed_ > M ) then
		self.seed_ = self.seed_ - M
	end
	return self.seed_
end

function random.uniform( self, n )
	return self:rand() % n
end

function random.seed( self )
	return self.seed_
end

function random.rawseed( self )
	return self.rawseed_
end

return random
