#-*- coding: GB2312 -*-
#$Id$


import cgiext
import cgi_error, cgi_util


SessionDir = "/tmp/session"
SessionCookieName = 'cgi_ck'
SessionKeyName = 'sid'
SessionCookieLen = 20
SessionKeyLen = 12
SessionFileHashDepth = 2

# time out = 0 means never time out
SessionTimeout = 3600

SessionReg_Key_SessionKey = '_sess_key'


# create a session for registration
def CreateSession(uniresponse, var_map, SessionDir="/tmp/session"):
	sess_manager = cgiext.SessionManager_File_New(
		SessionDir, SessionKeyLen, SessionFileHashDepth)
	if not sess_manager:
		raise cgi_error.SessionInternalError

	sess_info = cgiext.SessionInfo_File_New()
	if not sess_info:
		raise cgi_error.SessionInternalError
	
	session_filename = sess_manager.NewSession(sess_info)
	if not session_filename :
		raise cgi_error.SessionInternalError

	if SessionCookieLen > 0:
		key_str = cgi_util.GenerateRandomStr(SessionCookieLen)
		if not key_str :
			raise cgi_error.SessionInternalError
	else:
		key_str = ''

	if not sess_info.RegStr(SessionReg_Key_SessionKey, key_str):
		raise cgi_error.SessionInternalError
		
	for k, v in var_map.items():
		if not sess_info.RegStr(k, str(v)):
			raise cgi_error.SessionInternalError
	
	if not sess_manager.SaveInfo(session_filename, sess_info):
		raise cgi_error.SessionInternalError
	
	uniresponse.SetCookie(SessionCookieName, session_filename)
	return key_str
	


# general check session
def CheckSession(unireq, SessionDir="/tmp/session"):
	skey = unireq.GetVal(SessionKeyName)
	session_filename = unireq.GetCookie(SessionCookieName)
	if skey == None:
		skey = ''

	if session_filename == None or session_filename == '':
		raise cgi_error.SessionTimeoutError
	
	sess_manager = cgiext.SessionManager_File_New(
		SessionDir, SessionKeyLen, SessionFileHashDepth)
	if sess_manager == None:
		raise cgi_error.SessionInternalError

	sess_info = sess_manager.GetInfo(session_filename.value, SessionTimeout)
	if sess_info == None :
		raise cgi_error.SessionTimeoutError

	sess_key = sess_info.GetStr(SessionReg_Key_SessionKey)
	if sess_key != skey:
		raise cgi_error.SessionTimeoutError
	
	return (sess_info, skey, session_filename)


def RemoveSession(skey, SessionDir="/tmp/session"):
	sess_manager = cgiext.SessionManager_File_New(
		SessionDir, SessionKeyLen, SessionFileHashDepth)
	if not sess_manager:
		raise cgi_error.SessionInternalError
	
	if not sess_manager.RemoveSession(skey) :
		return False
	
	return True


class SessionCreater(object):
	def __init__(self, response, session_dir=SessionDir, throw=False):
		self.__response = response
		self.__var_map = {}
		self.__session_dir = session_dir
		self.__skey = None
		self.__throw = throw
		self.__exception_description = None


	def AppendSessionVar(self, dict_key, dict_var):
		self.__var_map[dict_key] = dict_var
		return

	def RemoveSessionVar(self, dict_key):
		self.__var_map.pop(dict_key)
		return

	def DoCreate(self):
		try:
			self.__skey = CreateSession( self.__response, self.__var_map, self.__session_dir)
		except (cgi_error.SessionTimeoutError, cgi_error.SessionInternalError), self.__exception_description:
			if self.__throw:
				raise

		if self.__skey == None:
			return True
		else:
			return False

	def GetSkey(self):
		return self.__skey

	def GetExceptionInfo(self):
		return self.__exception_description



class SessionChecker(object):
	def __init__(self, request, session_dir=SessionDir, throw=False):
		self.__sess_info = None
		self.__sid = None
		self.__session_filename = None
		self.__throw = throw
		self.__authorized = False
		self.__exception_description = ''
		try:
			(self.__sess_info, self.__sid, self.__session_filename) = CheckSession( request, session_dir )
			self.__authorized = True
		except (cgi_error.SessionTimeoutError, cgi_error.SessionInternalError), self.__exception_description:
			self.__authorized = False
			if self.__throw:
				raise


	def GetKey(self):
		return self.__sid

	def GetSessionFileName(self):
		return self.__session_filename

	def GetSinfo(self):
		return self.__sess_info

	def Logined(self):
		return self.__authorized

	def GetGeneralSessionInfo(self):
		return (self.__sess_info, self.__sid, self.__session_filename)

	def GetSessionVar(self, key_str, default_value = None):
		if not self.Logined():
			return default_value

		temp_str = self.__sess_info.GetStr( key_str )
		if temp_str == None:
			return default_value
		else:
			return temp_str

	def GetExceptionInfo(self):
		return self.__exception_description


