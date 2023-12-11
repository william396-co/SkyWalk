function math.count1Bits(x)
	local c = 0
	while x >0 do
		c = c + 1
	 	x = x &(x-1)	
	end
	return c
end
