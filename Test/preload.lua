ga = 'one'

function f()
	print ('fffff')
	print(ga)
end

local a = {
	['zero'] = 10,
	['three'] = 3, 
	['one']=1, 
	['two']=2,
}
--hujilib.travel(a)
--local ret = hujilib.new_t()
--for k,v in pairs(ret) do
	--print(k, v)
--end

local b = {1, 3, 4}
--hujilib.obj_len(b)
hujilib.new_env()
