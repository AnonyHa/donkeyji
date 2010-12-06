function f()
	local a = {}
	print 'test trace'
	if a > nil then
	end
end

function print_tbl(tbl)
	for k,v in pairs(tbl) do
		print(k, v)
	end
end
