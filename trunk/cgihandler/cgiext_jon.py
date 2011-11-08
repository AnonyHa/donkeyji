#-*- coding: GB2312 -*-
#$Id: cgiext_jon.py 32326 2008-08-11 02:52:19Z zhuang@NETEASE.COM $

import sys
import cgiext
import cgi_util, cgi_error

#
# an Adapter class for cgi request
#
class JonUniRequest:
	# param 'req' is a request in cgi.Request
	def __init__(self, req):
		self._req = req

	def GetVal(self, name, default_value = None):
		return self._req.params.get(name, default_value)
	def GetValStrip(self, name, default_value = None):
		return cgi_util.safe_strip(self.GetVal(name, default_value))

	def GetValMulti(self, name, default_value = None):
		if not name.endswith("*"):
			name += "*"
		return self._req.params.get(name, default_value) 
	def GetValStripMulti(self, name, default_value = None):
		vlist = self.GetValMulti(name, default_value)
		if vlist == None:
			return default_value
		for i in xrange(len(vlist)):
			vlist[i] = cgi_util.safe_strip(vlist[i])
		return vlist

	def GetUploadEntity(self, name):
		if not name.endswith("!"):
			name += "!"
		e = self._req.params.get(name)
		return e

	def GetUploadFile(self, name):
		e = self.GetUploadEntity(name)
		if e != None:
			return e.body
		return None
		
	#example as: "C:\sample.txt"
	def GetUploadFileName(self, name):
		e = self.GetUploadEntity(name)
		if e != None:
			return e.content_disposition[1].get("filename")
		return None

	def GetCookie(self, name, default_value = None):
		return self._req.cookies.get(name, default_value)

	#environment
	def GetRemoteAddr(self):
		return self._req.environ.get('REMOTE_ADDR')
		
	def GetClientIp(self):
		return self._req.environ.get('HTTP_X_FORWARDED_FOR') or self._req.environ.get('REMOTE_ADDR')

	def GetRefer(self):
		return self._req.environ.get('HTTP_REFERER')

	def GetServerPort(self):
		return self._req.environ.get('SERVER_PORT')

	def GetServerName(self):
		return self._req.environ.get('SERVER_NAME')

	def GetRequestMethod(self):
		return self._req.environ.get('REQUEST_METHOD')

	def GetScriptName(self):
		return self._req.environ.get('SCRIPT_NAME')

	def GetQueryString(self):
		return self._req.environ.get('QUERY_STRING')

	def IsUsingHTTPS(self):
		return self._req.environ.get('HTTPS').upper() == 'ON' 

	def gzip_isClientPrefer(self):
		h = self._req.environ.get('HTTP_ACCEPT_ENCODING')
		if h:
			return cgiext.gzip_isClientPrefered(h)
		else:
			return False

#
# an Adapter class for cgi respond
#
class JonUniResponse:
	def __init__(self, req):
		self._req = req

		self._charset = None
		self._content_type = None
		self._cookie_list = []
		self._header_finished = False

	def Write(self, s):
		self._req.write(s)
	
	#print msg to std error.
	def Err(self, msg):
		print >> sys.stderr, msg

	def AddHeader(self, name, value):
		if not self._header_finished :
			self._req.add_header(name, value)


	def SetHeader(self, name, value):
		if not self._header_finished :
			self._req.set_header(name, value)

		
	def SetStatus(self, status_code, status_msg):
		self.SetHeader("Status", "%d %s" % (status_code, status_msg))
		
	def SetCharset(self, charset):
		self._charset = charset
	
	def SetContentLength(self, length):
		self.SetHeader("Content-Length", "%d" % (length))
	
	def SetContentType(self, content_type):
		self._content_type = content_type
		if self._charset != None:
			self._content_type = self._content_type + "; charset=" + self._charset
		
	#If need more than one cookie, call this function more 
	def SetCookie(self, name, value, expires="", path="", domain=""):
		cookieval = name+"="+value
		if expires:
			cookieval += "; expires="+expires
		if path:
			cookieval += "; path="+path
		if domain:
			cookieval += "; domain="+domain
		self._cookie_list.append(cookieval)
		
	def AddGzipEncodingHeader(self):
		self.AddHeader("Content-Encoding", "gzip")	

	def FinishHeader(self):
		if self._header_finished:
			return False 
			
		if self._content_type != None:
			self.SetHeader("Content-Type", self._content_type)
		else:
			self.SetHeader("Content-Type", "text/html")

		for cookieval in self._cookie_list:
			self.AddHeader("Set-Cookie", cookieval)
		
		self._req.output_headers()
		self._header_finished = True
		return True

	def HtmlHeader(self):
		#self._content_type = "text/html"
		self.FinishHeader()
		
	def JpegHeader(self):
		self._content_type = "image/jpeg"
		self.FinishHeader()
	

	def Redirect(self, url, permanent):
		self._req.clear_headers()
		if permanent:
			self.SetHeader("Status","%d %s" %(301, "Move Permanently"))
		else:
			self.SetHeader("Status", "%d %s" %(307, "Move Temporarily"))
		
		self.AddHeader("Location", url)
		self.FinishHeader()

	def RedirectAfterPost(self, url):
		self._req.clear_headers()
		self.SetHeader("Status", "303 see others")
		self.AddHeader("Location",url)
		self.FinishHeader()
		

class CextPool(object):
	def __init__(self, basedir_list):
		self._pool = {}
		self._basedir_list = basedir_list
	
	def get_obj(self, filename):
		cext = self._pool.get(filename)
		if cext != None:
			return cext
		
		cext = cgiext.MYCGIEXT_New()
		if cext == None:
			raise MemoryError

		for bdir in self._basedir_list:
			pfile = bdir + "/" + filename
			if cext.load(pfile) < 0:
				continue
			
			self._pool[filename] = cext
			return cext
			
		raise cgi_error.TmplFileLoadError, pfile
