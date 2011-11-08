#!/usr/local/bin/python
#-*- coding: GB2312 -*-
#$Id: cgi_image_validater.py 20448 2007-03-30 06:49:01Z zhuang $


import gd
import random
import cgi_util, cgi_error, cgi_fcgi, cgi_session
import cgiext

from cStringIO import StringIO


ValidaterCookieName = 'image_vaildater'
SessionDir = cgi_session.SessionDir
ImageKeyName = 'image_key'
ImageValueField = 'image_value'

KEY_LEN = 4

# ===================== generate image class =====================================
ORIGIN_SIZE = ( 20, 20 )
ORIGIN_POSITION_RANGE = ( 5, 5 )
ORIGIN_POSITION = ( 0, 0 )
DISTURB_LINE_NUM = 0

SHALLOW_COLOR_RED = 200
SHALLOW_COLOR_GREEN = 200
SHALLOW_COLOR_BLUE = 200 

DEEP_COLOR_RED = 100
DEEP_COLOR_GREEN = 100
DEEP_COLOR_BLUE = 100

QUALITY = 100


def fast_rand( rand_range_begin, rand_range_end ):
	return random.randint( rand_range_begin, rand_range_end )


def get_rand_line( gd_size ):
	return ( random.randint(0, gd_size[0]), random.randint(0, gd_size[1]) )


def get_rand_origin_position( origin_position_range ):
	return ( fast_rand(0, origin_position_range[0]), fast_rand(0, origin_position_range[1]) )


def get_rand_shallow_color( shallow_color_red, shallow_color_green, shallow_color_blue ):
	return ( fast_rand(shallow_color_red, 255), fast_rand(shallow_color_green, 255), fast_rand(shallow_color_blue, 255) )


def get_rand_deep_color( deep_color_red, deep_color_green, deep_color_blue ):
	return ( fast_rand(0, deep_color_red), fast_rand(0, deep_color_green), fast_rand(0, deep_color_blue) )


class SetImageArgu(object):
	def __init__(self):
		self._origin_size_x, self._origin_size_y = ORIGIN_SIZE
		self._origin_position_range_x, self._origin_position_range_y = ORIGIN_POSITION_RANGE
		self._origin_position_x, self._origin_position_y = ORIGIN_POSITION
		self._disturb_line_num = DISTURB_LINE_NUM
		
		self._shallow_color_red = SHALLOW_COLOR_RED
		self._shallow_color_green = SHALLOW_COLOR_GREEN
		self._shallow_color_blue = SHALLOW_COLOR_BLUE
		
		self._deep_color_red = DEEP_COLOR_RED
		self._deep_color_green = DEEP_COLOR_GREEN
		self._deep_color_blue = DEEP_COLOR_BLUE

		self._quality = QUALITY



	def set_origin_size(self, origin_size_x, origin_size_y):
		self._origin_size_x = origin_size_x
		self._origin_size_y = origin_size_y

	def set_origin_position_range(self, origin_position_range_x, origin_position_range_y):
		self._origin_position_range_x = origin_position_range_x
		self._origin_position_range_y = origin_position_range_y

	def set_origin_position(self, origin_position_x, origin_position_y):
		self._origin_position_x = origin_position_x
		self._origin_position_y = origin_position_y

	def set_disturb_line_num(self, line_num):
		self._disturb_line_num = line_num

	def set_shallow_color_red(self, shallow_color_red):
		self._shallow_color_red = shallow_color_red

	def set_shallow_color_green(self, shallow_color_green):
		self._shallow_color_green = shallow_color_green

	def set_shallow_color_blue(self, shallow_color_blue):
		self._shallow_color_blue = shallow_color_blue

	def set_deep_color_red(self, deep_color_red):
		self._deep_color_red = deep_color_red

	def set_deep_color_green(self, deep_color_green):
		self._deep_color_green = deep_color_green

	def set_deep_color_blue(self, deep_color_blue):
		self._deep_color_blue = deep_color_blue

	def set_quality(self, quality):
		self._quality = quality

		

	def get_origin_size(self):
		return ( self._origin_size_x, self._origin_size_y )

	def get_origin_position_range(self):
		return ( self._origin_position_range_x, self._origin_position_range_y )

	def get_origin_position(self):
		return ( self._origin_position_x, self._origin_position_y )

	def get_disturb_line_num(self):
		return self._disturb_line_num

	def get_shallow_color_red(self):
		return self._shallow_color_red

	def get_shallow_color_green(self):
		return self._shallow_color_green

	def get_shallow_color_blue(self):
		return self._shallow_color_blue

	def get_shallow_color(self):
		return self._shallow_color_red, self._shallow_color_green, self._shallow_color_blue

	def get_deep_color_red(self):
		return self._deep_color_red

	def get_deep_color_green(self):
		return self._deep_color_green

	def get_deep_color_blue(self):
		return self._deep_color_blue

	def get_deep_color(self):
		return self._deep_color_red, self._deep_color_green, self._deep_color_blue

	def get_quality(self):
		return self._quality

	

class ImageBase(object):
	def __init__(self):
		self._deep = None
	def set_deep_color(self):
		return


class GenerateImageBase(object):
	def __init__(self):
		super(GenerateImageBase, self).__init__()
		self._image_arg = SetImageArgu()
		self._deep_color_red, self._deep_color_green, self._deep_color_blue = self._image_arg.get_deep_color()
		self._shallow_color_red, self._shallow_color_green, self._shallow_color_blue = self._image_arg.get_shallow_color()


	# generate a origin picture like 'a', 'w'
	def gen_base_image( self, res_str ):
		origin_gd = gd.image( self._image_arg.get_origin_size() )

		self.set_random_black_ground( origin_gd )

		origin_gd.string(gd.gdFontGiant, get_rand_origin_position( self._image_arg.get_origin_position_range() ), \
				str( res_str ), origin_gd.colorAllocate(get_rand_deep_color(self._deep_color_red, self._deep_color_green, \
				self._deep_color_blue)) )

		self.set_rand_line( origin_gd )

		return origin_gd


	# set gd object black ground . Warning : any color in this object would be recover!!!
	def set_random_black_ground( self, gd_obj ):
		# get gd object size
		gd_size = gd_obj.size()

		# get random black ground color
		t_color = gd_obj.colorAllocate( get_rand_shallow_color(self._shallow_color_red, self._shallow_color_green, self._shallow_color_blue) )

		# set black ground color
		gd_obj.rectangle( (0, 0), gd_size, t_color )


	def set_rand_line( self, gd_obj ):
		gd_size = gd_obj.size()

		for i in xrange(self._image_arg.get_disturb_line_num()):
			line_color =  gd_obj.colorAllocate( get_rand_deep_color(self._deep_color_red, self._deep_color_green, self._deep_color_blue) )
			gd_obj.line( get_rand_line(gd_size), get_rand_line( gd_size ), line_color )


	def get_authentication_picture( self, str_array, result_size=None ):
		tmp_gd = gd.image( [self._image_arg.get_origin_size()[0] * len(str_array), self._image_arg.get_origin_size()[1]] )
		
		for i in xrange(len(str_array)):
			tmp = self.gen_base_image( str_array[i] )
			tmp.copyResizedTo( tmp_gd, ( self._image_arg.get_origin_position()[0] + self._image_arg.get_origin_size()[0] * i, \
					self._image_arg.get_origin_position()[1] ), self._image_arg.get_origin_position(), \
					self._image_arg.get_origin_size(), self._image_arg.get_origin_size() )
			tmp = None

		if result_size != None:
			result_gd = gd.image( result_size )
			tmp_gd.copyResizeTo( result_gd, self._image_arg.get_origin_position(), self._image_arg.get_origin_position(), \
					tmp_gd.size(), result_size )
		else:
			result_gd = tmp_gd

		ios = StringIO()
		result_gd.writeJpeg(ios , self._image_arg.get_quality())
		tmp = ios.getvalue()
		ios.close()

		return tmp


# ===================== image page generater =====================================

class ImageGenerater(object):
	def __init__(self):
		super(ImageGenerater, self).__init__()
		self._im = GenerateImageBase()


	# two result for get image key, now is get random 0-9
	# remark for get random a-z, A-Z, 0-9 and some other symbol
	def get_image_key(self, keylen):
		'''
		# This generate for all letter
		tmp_key = cgi_util.GenerateRandomStr(keylen)

		# this replace only for replaced "-" and  "_" symbol
		tmp_key = tmp_key.replace('_','g').replace('-','f')
		'''
		tmp_key = ''
		for i in xrange(keylen):
			tmp_key += str( random.randint(0, 9) )

		return tmp_key

		
	# image size = None means use origin size
	def get_image(self, image_key, image_size=None):
		image_str = self._im.get_authentication_picture( image_key, image_size )

		return image_str


	def run(self, keylen=KEY_LEN, image_size=None):
		image_key = self.get_image_key( keylen )
		image_str = self.get_image( image_key, image_size )
		return image_key, image_str


# ================ cgi info creater =====================
# creater return two vaules
# the first is server session file anme : sess_file_name
# second is jpge image type string : image_str

# VirtualResponse used to define a virtual response,
# in order to session function can set a virtual response.
class VirtualResponse(object):
	def __init__(self):
		self._session_cookie_name = None
		self._session_file_name = None


	def SetCookie( self, sname, sfname ):
		self._session_cookie_name = sname
		self._session_file_name = sfname


	def GetCookieName( self ):
		return self._session_cookie_name


	def GetFileName(self):
		return self._session_file_name



class ImageValidaterCreater(object):
	def __init__(self, sess_dir=SessionDir):
		self._sess_dir = sess_dir
		self._ig = ImageGenerater()


	def get_image_info(self):
		image_key, image_str = self._ig.run()

		return image_key, image_str
	

	def create_vaildater_session(self, image_key, sess_dir=SessionDir):
		response = VirtualResponse()
		sc = cgi_session.SessionCreater(response, sess_dir, throw=True)
		sc.AppendSessionVar(ImageKeyName, image_key)
		sc.DoCreate()
		#sid = sc.GetSkey()
		return response.GetFileName()


	def run(self):
		# get image key and image jpge type string
		image_key, image_str = self.get_image_info()

		# create session file and get file name
		sess_file_name = self.create_vaildater_session( image_key, self._sess_dir )

		return sess_file_name, image_str


# ====================== image checker ===========================

# this session checker similar with cgi_session check session
# yes just alter compare skey and return values :)
# because I have no idea except define this function
def CheckImageSession(unireq, skey, sess_dir=SessionDir):
	#skey = unireq.GetVal(SessionKeyName)
	session_filename = unireq.GetCookie( ValidaterCookieName )
	
	if skey == None:
		skey = ''

	if session_filename == None or session_filename == '':
		raise cgi_error.SessionTimeoutError

	# Warning: session dir modified here !
	sess_manager = cgiext.SessionManager_File_New(
		sess_dir, cgi_session.SessionKeyLen, cgi_session.SessionFileHashDepth)

	if sess_manager == None:
		raise cgi_error.SessionInternalError

	sess_info = sess_manager.GetInfo(session_filename.value, cgi_session.SessionTimeout)
	if sess_info == None :
		raise cgi_error.SessionTimeoutError

	sess_key = sess_info.GetStr(ImageKeyName)

	# Warning: return value changed here !
	if sess_key != skey:
		return (False, '')

	return (True, session_filename.value)


class ImageValidateChecker(object):
	def run(self, request, user_input_key_value, sess_dir=SessionDir):
		try:
			result = CheckImageSession( request, user_input_key_value, sess_dir )
			if result[0]:
				# a session only can checker for one times
				self.remove_session( result[1], sess_dir )
				return True
			else:
				return False

		except cgi_error.SessionTimeoutError:
			return False


	def remove_session( self, skey, sess_dir ):
		try:
			cgi_session.RemoveSession(skey, sess_dir)
		except cgi_error.SessionTimeoutError:
			pass


# fast check function
def CheckValidateImage( request, user_input_value, sess_dir=SessionDir ):
	checker = ImageValidateChecker()

	result = checker.run( request, user_input_value, sess_dir )

	return result


# ================= all below are sample demo ==================
# all demo also can be called by cgi programe directly ^_^

# This is a output image example!!
# You had better inherit it and set your session dir ( here is "/tmp/session" )
class ShowImage(cgi_fcgi.CgiHandlerBase):
	def __init__(self):
		super(ShowImage, self).__init__()
		self._sess_dir = None


	def set_sess_dir(self):
		self._sess_dir = SessionDir


	def real_process(self):
		self.set_sess_dir()
		cv = ImageValidaterCreater( self._sess_dir )
		#cv._ig._im._image_arg.set_disturb_line_num( 2 )

		sess_file_name, image_str = cv.run()

		self._response.SetHeader("Cache-Control", "no-cache")
		self._response.SetHeader("Expires", "0")
		self._response.SetHeader("Pragma", "no-cache")
		self._response.SetHeader("image", "jpeg")
		self._response.SetCookie(ValidaterCookieName, sess_file_name)
		self._response.FinishHeader()
		self._response.Write(image_str)

		'''
		print "Cache-Control: no-cache"
		print "Expires: 0"
		print "Content-Type: image/jpeg\nSet-Cookie: " + ValidaterCookieName + "=" + sess_file_name + "\r\n"
		print image_str
		'''

		return None


# This is a image checker exapmle !!
class CheckImage(cgi_fcgi.CgiHandlerBase):
	def __init__(self):
		super(CheckImage, self).__init__()


	def real_process(self):
		i_key = self._request.GetVal(ImageValueField, '')
		#return self.general_info_page( i_key )

		a = ImageValidateChecker()

		sess_dir = "/home/project/csa/etc/session"

		if a.run( self._request, i_key, sess_dir ):
			self.general_info_page("Successed!")
		else:
			self.general_error_page("failed!")

		return None


if __name__ == "__main__":
	#cgi_fcgi.AcceptAndHandle(ShowImage)
	#cgi_fcgi.AcceptAndHandle(CheckImage)

	pass
	'''
	ios = StringIO()
	print "Cache-Control: no-cache"
	print "Set-Cookie:"
	print "Expires: 0"
	print 'Content-Type: image/jpeg\n'
	#gi = GenerateImageBase()
	#a = gi.gen_base_image( 'A' )
	#a.writeJpeg(ios , 100)
	#print ios.getvalue()
	#b = gi.get_authentication_picture( 'TestString' )
	ig = ImageGenerater()
	b = ig.run()
	print b[1]

	'''
