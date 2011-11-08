#-*- coding: GB2312 -*-
# $Id: cgi_fcgi.py 13521 2005-12-08 03:21:40Z romeo $

import traceback, os, sys, time, urllib
import jon.cgi as cgi
import jon.fcgi as fcgi
import cgiext, cgiext_util
import cgiext_jon, cgi_error, cgi_session, cgi_log
import _mysql_exceptions as exc


class HandlerWithLogging(cgi.Handler):
	def __init__(self):
		super(HandlerWithLogging, self).__init__()
		self._err_filefd = None
		self._err_filename = None
		self._req = None
		self._request = None
		self._response = None
		#self._log = None
		self._log = cgi_log.LogBase()
		self.regist_logs()


	#simple process sample.
	def real_process(self):
		self._response.FinishHeader()
		self._request.Write("Hello, how are you!\n")

	def exception_handle(self):
		'''
		if not self.init_errfile():
			print >>sys.stderr, "Cannot open error log file: "+ self._err_filename
			self._err_filefd = sys.stderr

		print >>self._err_filefd, "================================================"
		print >>self._err_filefd, "DateTime="+time.ctime()
		traceback.print_exc(limit=None, file=self._err_filefd)
		'''
		from cStringIO import StringIO
		cstr = StringIO()
		traceback.print_exc(None, cstr)

		log_msg = "================================================\n" + \
				"DateTime=" + time.ctime() + "\n" + cstr.getvalue()

		cstr.close()

		self._log.log( "error_log", log_msg )

		return


	def regist_logs(self):
		self._log.regist_new_log( "error_log", os.getenv('CGI_ERROR_LOG_FILE') )


	def process(self, req):
		try:
			self._req = req
			self._request = cgiext_jon.JonUniRequest(self._req)
			self._response = cgiext_jon.JonUniResponse(self._req) 
			self.real_process()
			self._req = None

		except SystemExit:
			raise
		except:
			self.exception_handle()
			

class CgiHandlerBase(HandlerWithLogging):
	def __init__(self):
		super(CgiHandlerBase, self).__init__()
		self._output_finished = False

	def exception_handle(self):
		(t, v, tr) = sys.exc_info()
		if t == None:
			return 
		#else
		if isinstance(v, cgi_error.SessionError):
			if isinstance(v, cgi_error.SessionInternalError):
				self.general_error_page("非法访问！")
				super(CgiHandlerBase, self).exception_handle()
				return
			if isinstance(v, cgi_error.SessionTimeoutError):
				self.general_error_page("登陆超时！")
				return 
			if isinstance(v, cgi_error.SessionUserTypeError):
				self.general_error_page("非法访问！")
				return 
			
			self.general_error_page("登陆超时！")
			return 

		if isinstance(v, exc.MySQLError):
			if isinstance(v, exc.DatabaseError):
				self.general_exception_page("数据库错误")
				super(CgiHandlerBase, self).exception_handle()
				return
			if isinstance(v, exc.InterfaceError):
				self.general_exception_page("系统错误，请稍后再试！")
				super(CgiHandlerBase, self).exception_handle()
				return

			self.general_exception_page("MySQLError")
			super(CgiHandlerBase, self).exception_handle()
			return
			
		if isinstance(v, urllib.socket.error):
			self.general_exception_page("网络连接超时, 请稍后再试!")
			return
				
		self.general_exception_page("未知错误!")
		# pass it back to super to handle
		return super(CgiHandlerBase, self).exception_handle()

	def get_cext(self, fullpath_filename):
		#dir = ???
		#CgiHandlerBase.cext_pool = cgiext_jon.CextPool(TEMPLATEDIR)
		#CgiHandlerBase.cext_pool.get_obj(filename)
		cext = cgiext.MYCGIEXT_New() 
		if cext.load( fullpath_filename) < 0 :
			self._response.HtmlHeader()
			self._response.Write("load pat failed, please try again.")
			self._response.Err("load pat failed: " + fullpath_filename)
			self._output_finished = True
			return None
		return cext

	def output_cgiext(self, cext):
		if self._request.gzip_isClientPrefer():
			self._response.AddGzipEncodingHeader()
			self._response.FinishHeader()
			go = cgiext_util.GzipOutput(self._req)
			cext.output(go)
			go.write("<!--gzipped-->")
		else:
			self._response.FinishHeader()
			cext.output(self._req)
		self._output_finished = True
	

	def prepare(self):
		return True

	#for user 
	def general_info_page(self, msg):
		if self._output_finished:
			return
		self._response.HtmlHeader()
		self._response.Write(msg)
		self._output_finished = True

	def general_error_page(self, msg):
		self.general_info_page("出错了:  " + msg)

	def general_exception_page(self, msg):
		self.general_info_page("system error: " + msg)

	def authenticate(self, session_dir=cgi_session.SessionDir):
		return True

	def authorize(self, session_dir=cgi_session.SessionDir):
		return True
	
	def get_cgi_common_var(self):
		return True
	#check and get input.
	def get_cgi_var(self):
		return True


	# this function should return a CGIEXT object or None
	#if error , you can call general_error_page function, and ruturn None.
	def handle_request(self):
		return None
	
	
	def set_cext_common_var(self, cext):
		cext.setVal("ScriptName", self._request.GetScriptName())
		cext.setVal("ServerName", self._request.GetServerName())

	def set_cext_var(self, cext):
		return
	
	def real_process(self):
		if not self.get_cgi_common_var():
			self.general_error_page("获取输入错误!")
			return

		if not self.prepare():
			self.general_error_page("CGI初始化错误!")
			return

		if not self.authenticate():
			self.general_error_page("登陆超时!")
			return
		
		if not self.authorize():
			self.general_error_page("权限不足!")
			return
		
		if not self.get_cgi_var():
			self.general_error_page("获取输入错误!")
			return

		cext = self.handle_request()
		if cext == None:
			return

		self.set_cext_common_var(cext)
		self.set_cext_var(cext)
		self.output_cgiext(cext)
	
	

_fcgi_server = None

def AcceptAndHandle(handler):
	global _fcgi_server
	_fcgi_server = fcgi.Server({fcgi.FCGI_RESPONDER: handler}, threading_level=0)
	_fcgi_server.run()


def ExitFcgi():
	global _fcgi_server
	_fcgi_server.exit()

