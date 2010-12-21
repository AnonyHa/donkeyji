module(..., package.seeall)

--all host
local ALL_HOST_TO_STAT = {
	['internal'] = {
		['IP'] = '',
		['db_dir'] = './db',---dir of db files
		['result_file'] = 'result.txt'
	},
}
--------------------------------------------------------------------------

--all attribute
local ALL_USER_ATTRS_TO_STAT = {---in ['user']
	[1] = 'CurRoleId',
	[2] = 'GameExp',
	[3] = 'Cash',
	[4] = 'Fund',
	[5] = 'GameTime',
	[6] = 'OfflineTime',
}

--obj cnt
local ALL_OBJ_COUNT = {
	[1] = {['key'] = 'FriendInfoTbl', ['field'] = 'user'},--统计好友个数
	[2] = {['key'] = '', ['field'] = 'item'}--统计item个数
}

--all skill
local ALL_SKILL_TO_STAT = {
	[1] = {['id'] = 1001},
}

--all mini game
local ALL_MINI_GAME_TO_STAT = {
	[1] = {['id'] = 1001, ['name'] = '知识问答'},---知识问答
	[2] = {['id'] = 1015, ['name'] = '对对碰'},--对对碰
	[3] = {['id'] = 1016, ['name'] = '找茬'},--找茬
	[4] = {['id'] = 56, ['name'] = '连连看'},--连连看
	[5] = {['id'] = 61, ['name'] = '麻将'},--麻将
	[6] = {['id'] = 60, ['name'] = '拼拼乐'},--拼拼乐
	[7] = {['id'] = 55, ['name'] = '德州'},--德州
}

local ROLE_INFO = {
	[100101]  =	'测试1',
	[100102]  =	'我是大坏蛋',
	[100103]  =	'3测试',
	[100104]  =	'优秀学员',
	[100105]  =	'市民',
	[100106]  =	'幸运小猪',
	[100107]  =	'未来梦想家',
	[100108]  =	'不稳定辐射源',
	[100109]  =	'健康人群',
	[100110]  =	'生化专家',
	[100111]  =	'病毒潜伏者',
	[100112]  =	'感染体',
	[100113]  =	'匿星病毒母体',
	[100114]  =	'沙星病毒母体',
	[100115]  =	'虫蛹',
	[100116]  =	'有识之士',
	[102100]  =	'大学一年生',
	[102101]  =	'大学二年生',
	[102102]  =	'大学三年生',
	[102103]  =	'大学四年生',
	[102104]  =	'大学毕业生',
	[102105]  =	'硕士',
	[102106]  =	'博士',
	[102107]  =	'博士后',
	[102000]  =	'预备公民',
	[102001]  =	'正式公民',
	[102002]  =	'优秀公民',
	[102003]  =	'荣誉市民',
	[102004]  =	'城市达人',
	[102200]  =	'电玩初学者',
	[102201]  =	'电玩爱好者',
	[102202]  =	'电玩狂热者',
	[102203]  =	'电玩专家',
	[102204]  =	'电玩大师',
	[102205]  =	'电玩达人',
	[105000]  =	'智者',
	[105001]  =	'幸运金星',
	[105002]  =	'幸运银星',
	[105003]  =	'幸运铜星',
}

--house info
local HOUSE_INFO = {
}

function get_host_info()
	return ALL_HOST_TO_STAT
end

function get_attr_info()
	return ALL_USER_ATTRS_TO_STAT
end

function get_obj_count()
	return ALL_OBJ_COUNT
end

function get_skill_info()
	return ALL_SKILL_TO_STAT
end

function get_mini_game_info()
	return ALL_MINI_GAME_TO_STAT
end

function get_role_info()
	return ROLE_INFO
end
