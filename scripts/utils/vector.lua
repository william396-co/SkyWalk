
Vector = {}

Vector.__index = function( t, k )
	local ele = t.data_[ k ]
	if ele ~= nil then
		return ele
	else
		return Vector[ k ]
	end
end

function Vector:new()
	local meta =
	{
		size_ = 0,
		data_ = {}
	}
	setmetatable( meta, self )
	return meta
end

function Vector:delete()
	self = nil
end

function Vector:size()
	return self.size_
end

function Vector:append( element )
	self.size_ = self.size_ + 1
	table.insert( self.data_, element )
end

function Vector:remove( element )
	for i,v in pairs( self.data_ ) do
		if v == element then
			self[i] = nil
			self.size_ = self.size_ - 1
			return true
		end
	end
	return false
end

function Vector:remove_by_key( key )
	if self[ key ] ~= nil then
		self[ key ] = nil
		self.size_ = self.size_ - 1
		return true
	end
	return false
end

function Vector:data()
	return self.data_
end

function Vector:dump()
	for k,v in pairs( self.data_ ) do
		print( k, v )
	end
end
