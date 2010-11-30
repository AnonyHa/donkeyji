function g() 
	print('gggg') 
end

function f()
	debug.traceback('mmmm', 2)
	local ret = debug.getinfo(2)
	for k,v in pairs(ret) do
		print(k, v)
	end
end

f()
