--a table to save session for user
stbl = {
--[[ the fromation is just as below:
	[uid] = {
		['time'] = os.time(),
		['skey'] = 'asdgasdfasd',
	}
--]]
}
local db = nil
local con = nil

--connect to the mysql db
function db_init(db_name, user_name, pwd, host, port)
	db = luasql.mysql()
	assert(db)
	con = db:connect(db_name, user_name, pwd, host, port)
	assert(con)
end

function add_session(uid, skey)
	stbl[uid] = skey
end

function del_session(uid)
	stbl[uid] = nil
end

function get_session(uid)
	if not stbl[uid] then
		return ''
	end
	if os.time() - stbl[uid]['time'] > 600 then
		return ''
	end
	return stbl[uid]['skey']
end

function check_user(urs, pwd)
	local info = db.query()
	if not info then
		return -1
	end
	local uid = info['uid']
	local skey = gen_session()
	stbl[uid] = {
		['time'] = os.time(),
		['skey'] = skey,
	}
	return 0
end

function gen_session()
	return 'adsbasd'
end
