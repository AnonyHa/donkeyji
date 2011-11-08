#-*- coding: GB2312 -*-
#$Id: upload_session.py 9474 2005-06-15 02:50:37Z romeo $


import os
import string

import cgi_error
import cgi_util


class UpLoadSessionBase(object):
	def __init__(self, Throw=True):
		# file save result is True means save file successed !
		self._session_result = True
		self._path_list = []
		self._saved_filelist = []
		self._throw = Throw


	def __del__(self):
		if not self._session_result:
			try:
				for filename in self._saved_filelist:
					os.remove( filename )
				for pathname in self._path_list:
					os.rmdir( pathname )
			except OSError:
				pass


	def failed_session(self, failed_type, exception_info):
		# result for __del__ function
		self._session_result = False

		# if need throw , throw error exception
		if self._throw:
			raise cgi_error.SessionUploadError(failed_type + ' ' + str(exception_info))

		# You can return other information by failed_type
		'''
		if failed_type == "OSError":
			return False

		elif failed_type == "IOError":
			return False

		elif failed_type == "AttributeError":
			return False
			'''

		return False


	def safe_save(self, upload_file, save_path, save_filename=None):
		try:
			if not os.path.isdir(save_path):
				os.makedirs(save_path)
				self._path_list += [ save_path ]
		except OSError, exception_info:
			return self.failed_session( 'OSError', exception_info )

		try:
			if save_filename == '' or save_filename == None :
				save_filename = cgi_util.revise_upfile_name( upload_file.content_disposition[1]['filename'] )
		except (SyntaxError, AttributeError, TypeError), exception_info:
			return self.failed_session( 'AttributeError', exception_info )

		try:
			fp = file( save_path + os.sep + save_filename , "w" )
			fp.write( upload_file.body )

			self._saved_filelist += [ save_path + os.sep + save_filename ]
			fp.close()

		except IOError, exception_info:
			return self.failed_session( 'IOError', exception_info )

		return True


	def get_session_result(self):
		return self._session_result


if __name__=="__main__":
	fs = UpLoadSessionBase()
	fs.safe_save(None, '/home/romeo/test', None)
