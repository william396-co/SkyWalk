
local pb = require 'pb'

local msgfactory = {
	hooks_ = {0x1704, --[[0x1301, 0x1304, 0x1306--]]},
	namescope_ = {}
}

function msgfactory:loads( path )
	local files = traverse( path, "desc", true )
	for i = 1, #files do
		pb.loadfile( files[i] )
	end
	for k,v in pb.fields("msg.MsgID") do
		self.namescope_[k] = v
	end
	for name,basename,type in pb.types() do
		if type == "enum" then
			local namescope = string.split( name, "." )
			if namescope[3] and namescope[4]
				and string.startsWith( namescope[4], "Errorcode" ) then
				for k,v in pb.fields( name ) do
                    if self.namescope_[ namescope[3] ] == nil then
                        self.namescope_[ namescope[3] ] = {}
                    end
					self.namescope_[namescope[3]][k] = v
				end
			end
		end
	end
end

function msgfactory:hooks()
	return self.hooks_
end

function msgfactory:namescope()
	return self.namescope_
end

msgfactory:loads( 'scripts/protocol' )

return msgfactory
