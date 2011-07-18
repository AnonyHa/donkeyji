local preload_files = {
	'logic/base/import.lua',
	'logic/base/global.lua',
}

for _, path in ipairs(preload_files) do
	print (path)
	dofile(path)
end

local logic_files = {
	'logic/logic.lua'
}

for _, path in ipairs(logic_files) do
	dofile(path)
end
