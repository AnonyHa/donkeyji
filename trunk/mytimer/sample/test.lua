local function f()
	print('----------test in lua---------')
	func(a)
end

local g = {} 

timer.multi_call(f, 1.5)
