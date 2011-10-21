--[[
function huge(a)
end

function trace(msg)
	--print('*********', msg) 
end

hujilib.test('huji fc')
--]]

print('---path', package.path)
print('---cpath', package.cpath)

require('json')
for k,v in pairs(_G) do
	print(k, v)
end
print('huji') 

math.