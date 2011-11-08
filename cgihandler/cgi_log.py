#-*- coding: GB2312 -*-
#$Id#

import time

class LogBase(object):
	def __init__(self, auto_set=True):
		self._log_class = {}
		self._need_raise_io = True
		self._default_fp = None
		self._default_cc = None
		if auto_set:
			self.auto_set()


	def __del__(self):
		for i in self._log_class:
			if self._log_class[i]['canclose']:
				self._log_class[i]['fp'].close()


	# set log default values
	def auto_set(self):
		# set the default outpat path when log file open failed
		import sys
		self.set_default_fp( sys.stdout )

	# set whether need raise io exception
	def set_need_raise_io(self, method):
		self._need_raise_io = method

	def get_need_raise_io(self):
		return self._need_raise_io

	# set default fp when open log file failed
	def set_default_fp(self, fp, default_cc=False):
		self.set_need_raise_io(False)
		self._default_fp = fp
		self._default_cc = default_cc

	def get_default_fp(self):
		return self._default_fp, self._default_cc


	# regist a new log function
	# log channel such as "error_log","warning_log","notice_log" and so on
	# full_path is the path which this channel need to write
	# wtype means open log file type, you can set it yourself, default is "a+"

	def regist_new_log( self, log_channel, full_path, wtype="a+" ):
		# here are not catch IOError exception , you can deal with it yourself
		# some fd type can not close by user such as sys.stdout, stderr
		if self._log_class.has_key( log_channel ):
			return

		CC = True
		try:
			fp = file( full_path, wtype )
		except IOError:
			if self.get_need_raise_io():
				raise
			else:
				fp, CC = self.get_default_fp()

		self._log_class[ log_channel ] = { "fp":fp, "full_path":full_path, "canclose":CC }


	def log( self, log_channel, log_msg ):
		self._log_class[ log_channel ]["fp"].write( str(log_msg) )


	def close_log( self, log_channel ):
		if not self._log_class.has_key( log_channel ):
			return

		if self._log_class[ log_channel ]['canclose']:
			self._log_class[ log_channel ]['fp'].close()

		self._log_class.pop( log_channel )

#by Adun 2005-11-18 log with date
class LogWithDate(LogBase):
	def __init__(self):
		super(LogWithDate, self).__init__()
	
	def log_online(self, log_channel, log_msg):
		log_line = "================================================\n" + \
				"DateTime : " + time.ctime() + " Msg : " + log_msg + "\n"
		self.log(log_channel, log_line)
	
	def log_field(self, log_channel, log_msg):
		log_info = "================================================\n" + \
				"DateTime=" + time.ctime() + "\n" + "Msg : " + log_msg + "\n"
		self.log(log_channel, log_info)
	
if __name__=="__main__":
	lb = LogBase()
	lb.regist_new_log( "error_log", "/home/romeo/test/cgihandler_channel_error.log" )
	lb.regist_new_log( "notice_log", "/home/romeo/test/cgihandler_channel_notice.log" )
	lb.regist_new_log( "notice_log1", "" )
	lb.regist_new_log( "notice_log2", "" )

	lb.log( "error_log", "hahahahaa\n" )
	lb.close_log( "error_log" )
	lb.close_log( "error_log" )
	#print lb._log_fp_list
	lb.log( "notice_log1", "wo " )
	lb.close_log( "notice_log1" )
	lb.log( "notice_log2", "wo " )
	
	lwb = LogWithDate()
	lwb.regist_new_log( "error_log", "./error.log" )
	lwb.log_online( "error_log", "1234djaskdhaskdfh" )
	lwb.log_field( "error_log", "122hd98q3hbru8awrg8bfuysdgg193y2187bduiqwguwf" )
	lwb.close_log( "error_log" )
