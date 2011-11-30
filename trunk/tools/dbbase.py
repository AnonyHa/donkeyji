#-*- coding: GBK -*-
"""
Database Connection
"""
import time
import MySQLdb

DEBUG = True
if DEBUG:
	from filelog import log
else:
	log = None


class DBBase(object):
	def __init__(self, myname=''):
		super(DBBase,self).__init__(self)
		self.conn = None
		self._cursor = None
		self._trans = None
		self._last_execute_time = 0
		self.myname = myname
	
	def connect(self, *args, **kwargs):
		self.conn = MySQLdb.connect(*args, **kwargs)
		self._trans = False
		
		#After mysql-python 1.12, * Turn autocommit off initially
		#But here we enable autocommit again.
		self.conn.autocommit(True)

		return self.conn
	
	def is_connected(self):
		try:
			self.conn.ping()
			return True
		except:
			return False

	def begin(self):
		if self._trans:
			return
		
		self.execute("BEGIN")
		self._trans = True

	def rollback(self):
		if not self._trans:
			return
		self.execute("ROLLBACK")
		self._trans = False
		
	def commit(self):
		if not self._trans:
			return
		self.execute("COMMIT")
		self._trans = False
	
	#para is tuple or None or dict
	#if ReturnDict==True, return a dictonary instead of tuple
	#if nofetch==Ture, return the cursor, not result
	def query(self, sql, para=(), returndict=False, nofetch=False):
		r = self.execute(sql, para, returndict)
		if r == 18446744073709551615:
			## memery leak
			if self._trans:
				try:
					self.rollback()
				except:
					pass
			self.conn.close()
			
			raise Exception("Query Mem Leak")
			
		if nofetch:
			return self._cursor
		else:
			data = self._cursor.fetchall()
			return data
		
	#para is tuple or None
	def modify(self, sql, para=()):
		return  self.execute(sql, para)
	
	def update_cursor(self, returndict=False):
		curcls = None
		if returndict:
			curcls = MySQLdb.cursors.DictCursor
		self._cursor = self.conn.cursor(curcls)
		
	def execute(self, sql, para=None, returndict=False):
		self.update_cursor(returndict)
		if log:
			tmpsql = sql 
			if para:
				try:
					tmpsql = sql%para
				except:
					log.debug('sql:%r, para:%r'%(sql,para))
					raise
				log.debug('SQL:%s'%tmpsql)
		result = self._cursor.execute(sql, para)
		self.reset_time()
		return result

	def reset_time(self):
		self._last_execute_time = time.time()
	
	def get_idle_time(self):
		return time.time() - self._last_execute_time

	def close(self):
		if self.conn != None:
			if self._trans:
				try:
					self.rollback()
				except:
					pass
			self.conn.close()
		self.conn = None
		self._trans = None
		self._cursor = None
	
if __name__ == '__main__':
	db = DBBase()
	sql = 'select * from account where aid >= %s limit 5'
	print db.query(sql, (100,), True, nofetch=False)
	print 
	cur = db.query(sql, (100,), True, nofetch=True)
	for row in cur:
		print row
	
	print 'is_connected:', db.is_connected()
	
	db.close()
	print 'is_connected:', db.is_connected()
	

