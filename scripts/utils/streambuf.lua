
local streambuf = {}

-- meta and prototype
streambuf.meta = { __index = streambuf }
streambuf.prototype = {
	endian_ = 1,
	buffer_ = "",
    size_ = 0,
	offset_ = 1
}

function streambuf.new( offset, endian )
	local obj = {
		endian_ = endian,
		buffer_ = "",
        size_ = 0,
		offset_ = offset
	}
    print('encode')
	return setmetatable( obj, streambuf.meta )
end

function streambuf.new( buf, len, endian )
	local obj = {
		endian_ = endian,
		buffer_ = buf,
        size_ = len,
		offset_ = 1
	}
    print('decode')
	return setmetatable( obj, streambuf.meta )
end

function streambuf.reset( self )
    self.offset_ = 1
end

function streambuf.offset( self )
    return self.offset_
end

function streambuf.data( self )
    return self.buffer_, self.size_
end

function streambuf.encode( self, len, value )
    local str = ''
	if self.endian_ == 1 then
	    str = string.pack( string.format( ">I%d", len ), value )
	else
		str = string.pack( string.format( "<I%d", len ), value )
	end
    self.buffer_ = self.buffer_ .. str
    self.size_ = self.size_ + len
	self.offset_ = self.offset_ + len
end

function streambuf.decode( self, len )
	local d = 0
	if self.endian_ == 1 then
		d,_ = string.unpack( string.format( ">I%d", len ), self.buffer_, self.offset_ )
	else
		d,_ = string.unpack( string.format( "<I%d", len ), self.buffer_, self.offset_ )
	end
	self.offset_ = self.offset_ + len
	return d
end

return streambuf
