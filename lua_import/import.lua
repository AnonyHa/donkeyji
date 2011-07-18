--维护一个已经import的模块
local imported_module = {}

function import(file_path)
	if imported_module[file_path] then
		return imported_module[file_path]
	end

	--just compile the mod file
	local func, err = loadfile(file_path)

	local M = {}
	--保证file_path里能访问到_G
	setmetatable(M, {__index=_G})

	--给func设置环境，保证func()之后，将func里的变量都存入M，而不是_G
	setfenv(func, M)

	--call mod
	func()

	--call __init__
	if M.__init__ and type(M.__init__) == 'function' then
		M.__init__()
	end

	--save mod
	imported_module[file_path] = M

	--return mod
	return M
end
