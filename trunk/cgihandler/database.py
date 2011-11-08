#-*- coding: GB2312 -*-
#$Id: database.py 22019 2007-05-25 05:34:23Z gundam78 $


# more than version PythonMySQLdb-1.2.0
import MySQLdb
#import _mysql_exceptions as exc

class DBBase(object):
	def __init__(self):
		super(DBBase,self).__init__(self)
		self.conn = None
		self._cursor = None
		self._trans = None
		
	def __del__(self):
		if self.conn != None:
			if self._trans:
				self.RollBack()
			self.conn.close()	
		self.conn = None
		self._trans = None
		self._cursor = None
		

	def Connect(self, host, user, passwd, db, port=None):
		kwargs = {
			"host" : host,
			"user" : user,
			"passwd" : passwd,
			"db" : db,
		}

		if port != None:
			kwargs["port"] = port

		self.conn = MySQLdb.connect(**kwargs)
		self._cursor = self.conn.cursor()
		self._trans = False
		
		#After mysql-python 1.12, * Turn autocommit off initially
		#But here we enable autocommit again.
		self.conn.autocommit(True)

		return self.conn
		

	#MySQL python Version 1.1.2
	# * Remove Connection.begin(); use SQL BEGIN or START TRANSACTION instead self.conn.begin()
	def Begin(self):
		#If already start transaction, donn't restart it. maybe commit the last trans
		if self._trans:
			return
		self._cursor.execute("BEGIN")
		self._trans = True

	def RollBack(self):
		if not self._trans:
			return
		self.conn.rollback()
		self._trans = False
		
	def Commit(self):
		if not self._trans:
			return
		self.conn.commit()
		self._trans = False
	
	#if ReturnDict==True, return a dictonary instead of tuple
	def Query(self, sql, para, ReturnDict=False):
		self._cursor = self.conn.cursor()
		self._cursor.execute(sql, para)
		if self._cursor.rowcount == 0:
			return ()
			
		if ReturnDict:
			return self._convertToName()
		else:
			return self._cursor.fetchall()

		
	#para is tuple or None
	def Modify(self, sql, para):
		return  self._cursor.execute(sql, para)
			
	#para is list of tuples or None
	def ModifyMany(self, sql, para):
		return self._cursor.executemany(sql, para)
		
	def _getfields(self):
		"""map indices to fieldnames"""

		if not self._cursor.description:
			return {}

		results = {}
		column = 0

		for des in self._cursor.description:
			fieldname = des[0]
			results[column] = fieldname 
			column = column + 1
		
		return results	
		
	def _convertToName(self):
		records = self._cursor.fetchall()
		fields = self._getfields()

		results = [] 
		for record in records:
			rec = {}
			for i in xrange(len(fields)):
				rec[fields[i]]=record[i]	
			results.append(rec)
		
		return tuple(results)
		

	# ================= all function below is old interface ==========================
	def connect(self, host, user, passwd, db):
		return self.Connect( host, user, passwd, db )

	def setdealway(self, deal):
		if deal is True or deal is False:
			self._deal = deal
			
	def resetdealway(self):
		self._deal = None

	def begin(self):
		return self.Begin()

	def rollback(self):
		return self.RollBack()

	def commit(self):
		return self.Commit()

	def query(self, sql, para, throw=False, how=0):
		return self.Query( sql, para, throw )

	def modify(self, sql, para, throw=False):
		return self.Modify( sql, para )

	def modifymany(self, sql, para, throw=False):
		return self.ModifyMany( sql, para )


class Transaction(object):
	def __init__(self, db):
		super(Transaction,self).__init__(self)
		self.db = db
		self.db.Begin()

	def Commit(self):
		self.db.Commit()

	def RollBack(self):
		self.db.RollBack()

	def __del__(self):
		if self.db._trans:
			self.db.RollBack()

	# ================= all function below is old interface ==========================
	def commit(self):
		self.db.Commit()

