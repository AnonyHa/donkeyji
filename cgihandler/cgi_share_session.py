#-*- coding: GB2312 -*-
#$Id: cgi_share_session.py 28239 2008-03-03 02:45:39Z jorwen $

import ss_api
from cgihandler import cgi_error

SessionDir = None
SessionCookieName = 'cgi_ck'
SessionKeyName = "sid" 
SessionTimeout = 3600
SessionKeyLen = 32

SID_IN_COOKIE = False
CGICK_IN_COOKIE = True


def session_ok(ss_ret):
	return ss_ret["ret"] == "ok"

def _get_sid(full_sid):
	return full_sid[:SessionKeyLen]

def _get_cookie(full_sid):
	return full_sid[SessionKeyLen:]

def _get_full_sid(unireq, sid_name=SessionKeyName, cgick_name=SessionCookieName, sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE):

	if sid_in_cookie:
		sid = unireq.GetCookie(sid_name)
		if sid:
			sid = sid.value
	else:
		sid = unireq.GetValStrip(sid_name, "")
			# no sid was set
	if not sid:
		raise cgi_error.SessionTimeoutError

	# first get from cookie
	if cgick_in_cookie:
		cookie = unireq.GetCookie(cgick_name)
		if cookie:
			cookie = cookie.value
	else:
		# get from html variable
		cookie = unireq.GetValStrip(cgick_name, "")

	# no cookie set
	if not cookie:
		raise cgi_error.SessionTimeoutError

	return  sid + cookie


# create a session for registration
def CreateSession(uniresponse, server_config, productid, userid, var_map, sid_name=SessionKeyName, \
		cgick_name=SessionCookieName, sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE, sess_domain="", \
		sess_path="", sess_expires=""):

	var_str = str(var_map)
	var_len = len(var_str)
	result  = {}
	ret = ss_api.CreateSession(server_config, productid, userid, var_str, var_len, result)

	if not ret:
		raise cgi_error.SessionInternalError, result

	full_sid = result['sid']
	sid = _get_sid(full_sid) 
	cookie = _get_cookie(full_sid) 

	if sid_in_cookie:
		uniresponse.SetCookie(sid_name, sid, domain=sess_domain, path=sess_path, expires=sess_expires)

	if cgick_in_cookie:
		uniresponse.SetCookie(cgick_name, cookie, domain=sess_domain, path=sess_path, expires=sess_expires)

	return sid 
	

# general check session
def CheckSession(unireq, server_config, productid, sid_name=SessionKeyName, cgick_name=SessionCookieName, \
		sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE):

	sid = _get_full_sid(unireq, sid_name, cgick_name, sid_in_cookie, cgick_in_cookie)
	result = {}
	ret = ss_api.CheckSession(server_config, productid, sid, result)

	if not ret:
		raise cgi_error.SessionTimeoutError

	if int(result["time_delta"]) > SessionTimeout:
		ss_api.RemoveSession(server_config, productid, sid, result)
		raise cgi_error.SessionTimeoutError

	sess_str = result["sess_datas"]
	if len(sess_str) == 0:
		result["sess_datas"] = None
	else:
		result["sess_datas"] = eval(sess_str)

	return (result, _get_sid(sid), None)


def UpdateSession(unireq, server_config, productid, sess_data, sid_name=SessionKeyName, cgick_name=SessionCookieName, \
		sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE):

	sid = _get_full_sid(unireq, sid_name, cgick_name, sid_in_cookie, cgick_in_cookie)
	var_str = str(sess_data)
	var_len = len(var_str)
	result = {}
	ret = ss_api.UpdateSession(server_config, productid, sid, var_str, var_len, result)
	if not ret:
		raise cgi_error.SessionInternalError, result

	return True


def RemoveSession(unireq, server_config, productid, sid_name=SessionKeyName, cgick_name=SessionCookieName, \
		sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE):

	sid = _get_full_sid(unireq, sid_name, cgick_name, sid_in_cookie, cgick_in_cookie)
	result = {}
	ret = ss_api.RemoveSession(server_config, productid, sid, result)
	if not ret:
		raise cgi_error.SessionInternalError

	return True


class SessionCreater(object):
	def __init__(self, response, server_config, productid, userid, sid_name=SessionKeyName, cgick_name=SessionCookieName, \
			sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE, throw=True):

		self.__response = response
		self.__config = server_config
		self.__userid = userid
		self.__productid = productid
		self.__var_map = {}
		self.__sid = None
		self.__throw = throw
		self.__exception_description = None
		self.__sid_name = sid_name
		self.__cgick_name = cgick_name
		self.__sid_in_cookie = sid_in_cookie
		self.__cgick_in_cookie = cgick_in_cookie

	def AppendSessionVar(self, dict_key, dict_var):
		self.__var_map[dict_key] = dict_var
		return

	def RemoveSessionVar(self, dict_key):
		self.__var_map.pop(dict_key)
		return

	def DoCreate(self):
		try:
			self.__sid = CreateSession(self.__response, self.__config, self.__productid, self.__userid, \
					self.__var_map, self.__sid_name, self.__cgick_name, self.__sid_in_cookie, self.__cgick_in_cookie)

		except cgi_error.SessionInternalError, self.__exception_description:
			if self.__throw:
				raise

		if self.__sid :
			return True
		else:
			return False

	def GetSkey(self):
		return self.__sid

	def GetExceptionInfo(self):
		return self.__exception_description


class SessionChecker(object):
	def __init__(self, request, server_config, productid, sid_name=SessionKeyName, cgick_name=SessionCookieName, \
			sid_in_cookie=SID_IN_COOKIE, cgick_in_cookie=CGICK_IN_COOKIE, throw=True):

		self.__sess_ret = None
		self.__sess_info = None
		self.__productid = productid
		self.__sid = None 
		self.__session_filename = None
		self.__throw = throw
		self.__authorized = False
		self.__exception_description = ''
		self.__sid_name = sid_name
		self.__cgick_name = cgick_name

		try:
			(self.__sess_ret, self.__sid, self.__session_filename) = CheckSession(request, server_config, \
					self.__productid, self.__sid_name, self.__cgick_name, sid_in_cookie, cgick_in_cookie)

			self.__sess_info = self.__sess_ret['sess_datas']
			self.__authorized = True
		except (cgi_error.SessionTimeoutError, cgi_error.SessionInternalError), self.__exception_description:
			self.__authorized = False
			if self.__throw:
				raise

	def GetKey(self):
		return self.__sid

	def GetSessData(self):
		return self.__sess_info

	def GetSessionFileName(self):
		return self.__session_filename

	def GetSinfo(self):
		return self.__sess_info

	def Logined(self):
		return self.__authorized

	def GetGeneralSessionInfo(self):
		return (self.__sess_info, self.__sid, self.__session_filename)

	def GetUserId(self):
		return self.__sess_ret['userid']

	def GetTimeDelta(self):
		return self.__sess_ret['time_delta']

	def GetSessionVar(self, key_str, default_value = None):
		if not self.Logined():
			return default_value

		if self.__sess_info:
			return self.__sess_info.get(key_str, default_value)
		else:
			return None

	def GetExceptionInfo(self):
		return self.__exception_description
