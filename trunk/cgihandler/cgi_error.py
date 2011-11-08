#-*- coding: GB2312 -*-
#$Id: cgi_error.py 16108 2006-06-08 10:07:05Z linda $



class SessionError(Exception):
	def __init__(self, value = ''):
		Exception.__init__(self)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


# for errors like: cannot create session, session dir missing ...
class SessionInternalError(SessionError):
	def __init__(self, value = 'SessionInternalError'):
		SessionError.__init__(self, value)
		self.value = value
		
	def __str__(self):
		return repr(self.value)

# for session timeout, or invalid session key ...
class SessionTimeoutError(SessionError):
	def __init__(self, value = 'SessionTimeoutError'):
		SessionError.__init__(self, value)
		self.value = value
		
	def __str__(self):
		return repr(self.value)

#check user type error, invalid privilege 
class SessionUserTypeError(SessionError):
	def __init__(self, value = 'SessionUserTypeError'):
		SessionError.__init__(self, value)
		self.value = value
		
	def __str__(self):
		return repr(self.value)

# for errors : upload file session
class SessionUploadError(SessionError):
	def __init__(self, value = 'SessionUploadError'):
		SessionError.__init__(self, value)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


class RunHereError(Exception):
	def __init__(self, value = ''):
		Exception.__init__(self)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


# for invalid arg to functions ...
class InvalidArgError(Exception):
	def __init__(self, value = ''):
		Exception.__init__(self)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


class TmplFileError(Exception):
	def __init__(self, filename):
		Exception.__init__(self)
		self._filename = filename
	
	def __str__(self):
		return "template file error: " + self._filename


class TmplFileLoadError(TmplFileError):
	def __init__(self, filename):
		#super(TmplFileLoadError, self).__init__(filename)
		TmplFileError.__init__(self, filename)
	
	def __str__(self):
		return "template file load error: " + self._filename


class TmplFileStructError(TmplFileError):
	def __init__(self, filename):
		#super(TmplFileStructError, self).__init__(filename)
		TmplFileError.__init__(self, filename)
	
	def __str__(self):
		return "template file struct error: " + self._filename



class CallBaseClassMethodError(Exception):
	def __init__(self, value = ''):
		Exception.__init__(self)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


# base error for CyberBuy specific errors
class CbError(Exception):
	def __init__(self, value = ''):
		Exception.__init__(self)
		self.value = value
		
	def __str__(self):
		return repr(self.value)


class InsufficentBalance(CbError):
	def __init__(self, *arg):
		#super(InsufficentBalance, self).__init__(*arg)
		CbError.__init__(self, *arg)


class ItemNotFound(CbError):
	def __init__(self, *arg):
		#super(ItemNotFound, self).__init__(*arg)
		CbError.__init__(self, *arg)


class UserNotFound(ItemNotFound):
	def __init__(self, *arg):
		#super(UserNotFound, self).__init__(*arg)
		ItemNotFound.__init__(self, *arg)


class NotPriced(CbError):
	def __init__(self, *arg):
		#super(NotPriced, self).__init__(*arg)
		CbError.__init__(self, *arg)


class NoVirtualBankAccount(CbError):
	def __init__(self, *arg):
		#super(NoVirtualBankAccount, self).__init__(*arg)
		CbError.__init__(self, *arg)
		

class UnkownImgFormat(CbError):
	def __init__(self, *arg):
		#super(UnkownImgFormat, self).__init__(*arg)
		CbError.__init__(self, *arg)


class NotAllowed(CbError):
	def __init__(self, *arg):
		#super(NotAllowed, self).__init__(*arg)
		CbError.__init__(self, *arg)


# sth is not ready to start
class NotReady(CbError):
	def __init__(self, *arg):
		#super(NotReady, self).__init__(*arg)
		CbError.__init__(self, *arg)

# you are late!
class TooLate(CbError):
	def __init__(self, *arg):
		#super(NotReady, self).__init__(*arg)
		CbError.__init__(self, *arg)

class NotEnough(CbError):
	def __init__(self, *arg):
		#super(NotReady, self).__init__(*arg)
		CbError.__init__(self, *arg)


# do sth that is already done ...
class WrongSequence(CbError):
	def __init__(self, *arg):
		#super(WrongSequence, self).__init__(*arg)
		CbError.__init__(self, *arg)
		

class NotMatched(NotAllowed):
	def __init__(self, *arg):
		#super(NotMatched, self).__init__(*arg)
		NotAllowed.__init__(self, *arg)


class BruteForceDenied(NotAllowed):
	def __init__(self, *arg):
		#super(BruteForceDenied, self).__init__(*arg)
		NotAllowed.__init__(self, *arg)


#class MySQLDBError(Exception):
#	def __init__(self, conn = None):
#		self._conn = conn
#		
#	def __str__(self):
#		if self._conn != None:
#			return "DB Error: " + self._conn.error()
#		else:
#			return "DB Error"

class UnexceptedError(Exception):
	def __init__(self, value="Unexcepted Error"):
		Exception.__init__(self)
		self.value = value
	
	def __str__(self):		
		return repr(self.value)
	

# testing section
if __name__ == "__main__":
	try:
		raise SessionUploadError
	except SessionUploadError, info:
		print info
