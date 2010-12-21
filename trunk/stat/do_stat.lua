local config = nil
local all_host_info = nil 
local all_attr_info = nil
local all_skill_info = nil
local all_obj_count = nil
local all_mini_game_info = nil 
local all_role_info = nil

local SPLIT_STR = '		'--2 tabs

--------------------------------------------------------------
local function _normalize(value)
	local retval = ''
	if type(value) == 'function' then
		retval = '<' .. tostring(value) .. '>'
	elseif type(value) == 'table' then
		retval = '<' .. tostring(value) .. '>'
	elseif type(value) == 'string' then
		retval = string.format('%q',value)
	else
		retval = tostring(value)
	end
	return retval
end

local function dodump (value, c)
	local retval = ''
	if type(value) == 'table' then
		c = (c or 0) + 1
		if c >= 100 then error("sys.dump too deep:"..retval) end

		retval = retval .. '{'
		for k, v in pairs(value) do
			retval = retval .. '[' .. dodump(k, c) .. '] = ' ..dodump(v, c) .. ', '
		end
		retval = retval .. '}'
		return retval 
	else
		retval = _normalize(value)
	end
	return retval
end

function do_dump (value)
	local ni, ret = pcall(dodump, value)
	return ret
end


function table_size(tbl)
	if tbl then
		local ret = 0
		for _,_ in pairs(tbl) do
			ret = ret + 1
		end
		return ret
	else
		return 0
	end
end
------------------------------------------------------------

function do_debug(fmt, ...)
	local str = string.format(fmt, ...)
end

function do_init()
	config = require('config') 
	all_host_info = config.get_host_info()
	all_attr_info = config.get_attr_info() 
	all_obj_count = config.get_obj_count()
	all_skill_info = config.get_skill_info()
	all_mini_game_info = config.get_mini_game_info()
	all_role_info = config.get_role_info()
end

---------------
--rsync db data
---------------
function do_rsync(ip, dir)
end

function do_tree_db(dir, ftbl)
	dir = dir or './db'----postion of db
	local files = posix.dir(dir)

	for i, f in pairs(files) do
		if string.sub(f, 1, 1) ~= '.' then---filter svn file'.svn' and '..' and '.'
			local path_name = string.format('%s/%s', dir, f)
			local s = assert(posix.stat(path_name))
			if s.type == 'directory' then
				--do_debug('---dir---		%s', f)
				do_tree_db(path_name, ftbl)
			elseif s.type == 'regular' then
				--do_debug('---file---	%s', f)
				table.insert(ftbl, path_name)
			else
				do_debug('skip invalid directory: %s, type: %s', path_name, s.type)
			end
		end
	end
end

---------------------------------
local CHECKSUM_SYMBOL = "\n@\n"
--unserialize
function do_unserialize_db_file(file)
	local fh, err = io.open(file)
	if not fh then
		return
	end
	local data = fh:read("*a")
	fh:close()

	local _begin, _end = string.find(data, CHECKSUM_SYMBOL)
	if _begin and _end then
		local Checksum = string.sub(data, 1, _begin-1)
		data = string.sub(data, _end+1)
	else
		return
	end

	local data_tbl = assert(loadstring('return '..data))()

	return data_tbl
end

function do_stat_one_db_file(file)
	local data = do_unserialize_db_file(file)

	local stat_data = {}

	---统计基本属性
	for _, key in ipairs(all_attr_info) do
		local val = data['user'][key] or ''
		if key == 'OfflineTime' then
			if val ~= '' then
				val = os.date('%x', val)
			end
		elseif key == 'CurRoleId' then
			val = all_role_info[tonumber(val)] or val
		end
		table.insert(stat_data, tostring(val))
	end

	---统计obj数量: 好友，物品
	for _, v in ipairs(all_obj_count) do
		local field = v['field']
		local key = v['key']
		local cnt
		if key == '' then
			cnt = table_size(data[field])
		else
			cnt = table_size(data[field][key])
		end
		table.insert(stat_data, tostring(cnt))
	end

	---统计小游戏
	local game_stat = ''
	local game_data = data['user']['GameTbl'] or {}
	for _, v in ipairs(all_mini_game_info) do
		local tmp = game_data[v['id']] or {}
		local time = tostring(tmp['time'] or 0)
		local level = tostring(tmp['level'] or 0)
		local str = '['..v['name']..':'..time..'(time)'..level..'(level)'..']'
		game_stat = game_stat..str
	end
	table.insert(stat_data, game_stat)

	---统计skill
	local skill_stat = {}
	local skill_data = data['NewSkillTbl'] or {}
	for _, v in ipairs(all_skill_info) do
	end

	--return data['user']['Id'] or 0, stat_data
	return data['user']['URS'] or data['user']['Id'], stat_data
end

---------------
--stat db file
---------------
function do_scan_all_db_files(db_files)
	db_files = db_files or {}
	local all_user_stat = {}
	for id, path_name in pairs(db_files) do
		--local uid, one_user_stat = do_stat_one_db_file(path_name)
		local URS, one_user_stat = do_stat_one_db_file(path_name)
		--all_user_stat[uid] = one_user_stat
		all_user_stat[URS] = one_user_stat
	end
	return all_user_stat
end

function do_write_stat_result(stat_data, file)
	file = file or 'result.txt'
	local fh, err = io.open(file, 'w')
	if not fh then
		return
	end

	--local attr_str = 'uid'
	local attr_str = 'URS'
	for _, attr_name in ipairs(all_attr_info) do
		attr_str = attr_str..SPLIT_STR..attr_name
	end

	for _, val in ipairs(all_obj_count) do
		if val['key'] ~= '' then
			attr_str = attr_str..SPLIT_STR..val['key']
		else
			attr_str = attr_str..SPLIT_STR..val['field']
		end
	end
	fh:write(attr_str..'\n')

	--for uid, info in pairs(stat_data) do
	for URS, info in pairs(stat_data) do
		--local line_str = string.format('%d', uid)
		local line_str = string.format('%s', URS)
		for _, val in ipairs(info) do
			line_str = line_str..SPLIT_STR..tostring(val)
		end
		fh:write(line_str..'\n')
	end

	fh:close()
end

function do_stat()
	do_init()
	for srv, conf in pairs(all_host_info) do
		local ip = conf['IP']
		local db_dir = conf['db_dir']
		local result_file = conf['result_file']

		local db_dir = do_rsync(ip, db_dir)

		local all_db_files = {}
		do_tree_db(db_dir, all_db_files)
		print('-----total----', table_size(all_db_files))

		local all_user_stat_info = do_scan_all_db_files(all_db_files)

		do_write_stat_result(all_user_stat_info, result_file)
	end
end

------------
--main
------------
function main()
	do_stat()
end

main()
