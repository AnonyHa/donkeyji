#-*- coding: GB2312 -*-
#$Id: cgiext_util.py 9291 2005-06-02 07:41:33Z romeo $

import zlib,struct
import cgiext

def write32(output, value):
	output.write(struct.pack("<l", value))


def write32u(output, value):
	if value < 0:
		value = value + 0x100000000L
	output.write(struct.pack("<L", value))


def GetCookieValue(headers_in, key):
	if headers_in.has_key('Cookie'):
		import Cookie
		C = Cookie.SmartCookie(headers_in['Cookie'])
		if C.has_key(key) :
			return C[key].value


def GetPostValueTable(req):
	ls = req.headers_in.get('Content-Length')
	if ls:
		from mod_python import util
		post_qs = req.read(int(ls))
		return util.parse_qs(post_qs, 1)
	else:
		return {}


def gzip_isClientPrefered(headers_in):
	h = headers_in.get('Accept-Encoding')
	if h:
		return cgiext.gzip_isClientPrefered(h)


def RedirectAfterPost(req, uri):
	from mod_python import apache
	req.headers_out['Location'] = uri
	return apache.HTTP_SEE_OTHER


def CheckRequestMethod(req, method):
	from mod_python import apache
	if req.method != method:
		req.status = apache.HTTP_METHOD_NOT_ALLOWED
		return None
	return 1


class GzipOutput:

	def __init__ (self, fo):
		if not hasattr(fo, 'write'):
			raise TypeError, "output does not support 'write' method"

		self._fileobj = fo
		self._zipobj = zlib.compressobj(1, zlib.DEFLATED, -zlib.MAX_WBITS, 9, 0)
		self._size = 0
		self._crc = 0
		self.write_gzip_header()

	def write_gzip_header(self):
		gzip_header = "\x1f\x8b\x08\x00\x00\x00\x00\x00\x00\x03"
		self._fileobj.write(gzip_header)


	def write(self, data):
		if len(data)>0:
			self._size += len(data)
			self._crc = zlib.crc32(data, self._crc)
			self._fileobj.write( self._zipobj.compress(data) )


	def close(self):
		if self._fileobj == None:
			return
		
		self._fileobj.write(self._zipobj.flush())
		write32(self._fileobj, self._crc)
		write32(self._fileobj, self._size)
		self._fileobj = None
		self._zipobj = None


	def __del__(self):
		self.close()


def gzip_output(req, istr):
	go = GzipOutput(req)
	go.write(istr)


def gzip_AddEncodingHeader(headers_out):
	headers_out['Content-Encoding'] = 'gzip'
