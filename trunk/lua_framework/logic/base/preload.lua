local preload_files = {
	--first
	'logic/base/import.lua',
	'logic/base/const.lua',

	--last
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
