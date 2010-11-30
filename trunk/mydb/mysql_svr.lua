module(..., package.seeall)

local env = nil
local con = nil

--invoked in C
function mysql_db_init(db, user, pwd, host, port)
	local _env = luasql.mysql()
	assert(_env)
	local _con = _env:connect(db, user, pwd, host, port)
	assert(_con)
	env = _env
	con = _con
end
