import urllib
import urllib2

URL = 'http://192.168.10.121:8078/simpleapi/game/role'

# ------------------------------------------------
# to do: how to check the validity of the argument
# ------------------------------------------------

def handle_friend():
	data = {
		'game' : 'itown',
		#'regionid' : 1,
		#'serverid' : 2,
		#'roleid' : 124,
		'rolename' : 'donitz',
		#'friend_regionid' : 1,
		#'friend_serverid' : 2,
		#'friend_roleid' : 125,
		'token' : '4eelZS24yHUJG9UXmLw5Yklrluh1A3FhHcQwkBZV',
		#'sinceid' : 1,
		'content' : 'i am donitz',
		#'comment_thread' : 2,
	}
	#init_url = '%s/friendship/create' % URL
	#init_url = '%s/action/timeline' % URL
	#init_url = '%s/action/homeline' % URL
	init_url = '%s/record/create' % URL
	#init_url = '%s/comment/list' % URL
	print init_url
	arg = urllib.urlencode(data)
	req = urllib2.Request(init_url, arg)
	rsp = urllib2.urlopen(req)
	code = rsp.code
	print 'code = ', code
	if code == 200:
		return {'code' : 0}
	else:
		return {'code' : -1}

#-----------------------------------------------------
#-----------------------------------------------------


if __name__ == '__main__':
	ret = handle_friend()
	print ret
