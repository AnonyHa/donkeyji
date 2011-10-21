
local preload_files = {
	--first
	'script/base/class.lua',
	'script/base/import.lua',
	'script/base/const.lua',
	'script/base/ldb.lua',
	'script/base/traceback.lua',

	--last
	'script/base/global.lua',
}

for _, path in ipairs(preload_files) do
	print (path)
	dofile(path)
end

local logic_files = {
	'script/logic.lua'
}

for _, path in ipairs(logic_files) do
	dofile(path)
end
