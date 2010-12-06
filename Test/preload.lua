function f()
	local a = {}
	print 'test trace'
	debug.debug()
	print_tbl(t)
end

function print_tbl(tbl)
	for k,v in pairs(tbl) do
		print(k, v)
	end
end
