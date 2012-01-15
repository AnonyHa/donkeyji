#-*- coding: gb2312 -*-
import time
import string
import os
import traceback

class BaseLog(object):
	"""
		如果log所在的文件夹不存在，会自己建立
	"""
	DEEP, DEBUG, INFO, WARN, ERROR, EXCEPTION = 5, 10, 20, 30, 40, 50
	def __init__(self, fname='', flush=True):
		self._fd = None
		self._show = False
		self._level = self.DEBUG
		self._flush = flush

		if fname:
			self.set_filename(fname)
	
	def set_filename(self, fname):
		filedir = os.path.dirname(fname)
		if not os.path.exists(filedir):
			try:
				os.makedirs(filedir)
			except :
				#多进程的时候，可能会出现：
				#判断文件夹是否存在时：不存在
				#mkdirs的时候：已经存在
				#所以这里pass，而不是raise
				#如果是权限不够，下面的open会raise Exception
				pass
		self._fd = open(fname, 'a')

	def set_level(self, level):
		self._level = level
	
	def set_show(self, show):
		self._show = show

	def writeMsg(self, msg, level='info'):
		msgInfo = {'datetime' : time.strftime("%Y-%m-%d %H:%M:%S"), 'level' : string.upper(level), 'msg': msg} 
		msgstr = "[%(datetime)s] [%(level)s] %(msg)s\n" % msgInfo
		if self._fd:
			self._fd.write(msgstr)
			if self._flush:
				self._fd.flush()

		if self._show:
			print msgstr,

	def exception(self, msg=None):
		if self._level > self.EXCEPTION:
			return 

		if msg is not None:
			self.writeMsg(msg, 'exception')

		err = traceback.format_exc()
		for line in err.strip().split("\n"):
			self.writeMsg(line, 'exception')

	def info(self, msg):
		if self._level > self.INFO:
			return 

		self.writeMsg(msg, 'info')

	def error(self, msg):
		if self._level > self.ERROR:
			return 

		self.writeMsg(msg, 'error')

	def warn(self, msg):
		if self._level > self.WARN:
			return 

		self.writeMsg(msg, 'warn')

	def __del__(self):
		if self._fd:
			self._fd.flush()

	def debug(self, msg):
		if self._level > self.DEBUG:
			return 

		self.writeMsg(msg, 'debug')

	def deep(self, msg):
		if self._level > self.DEEP:
			return 

		self.writeMsg(msg, 'deep')


class DailyLog(BaseLog):
	"""
		按日期写log
	"""
	def __init__(self, path='', prefix='', flush=True):
		super(DailyLog, self).__init__(flush=flush)
		self._path = path
		self._prefix = prefix

		if self._path and self._prefix:
			self.set_path_and_prefix(self._path, self._prefix)
	
	def set_path_and_prefix(self,path,prefix):
		self._logname_format = os.path.join(path, str(prefix)+"%s.log")
		self._date = time.strftime("%Y%m%d")
		fname = self._logname_format % self._date
		self.set_filename(fname)

	def writeMsg(self, msg, level='info'):
		date = time.strftime("%Y%m%d")
		if self._date != date:
			self._fd.flush()
			self._date = date
			self._fd = open(self._logname_format % self._date, 'a')
		
		super(DailyLog, self).writeMsg(msg, level)
			


log = DailyLog()
def init_daily_log(path, prefix):
	global log
	log.set_path_and_prefix(path, prefix)

init_daily_log('/home/donitz/work/g-code/dwitter/dwitter/log', 'dwt')

if __name__ == '__main__':
	init_daily_log('./', 'x')
	log.info("o")
