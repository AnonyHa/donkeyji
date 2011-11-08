#-*- coding: GB2312 -*-
# $Id: cgiext_driver.py 9291 2005-06-02 07:41:33Z romeo $

# This is a sample implementation to handle different URI by dynamically
# loading the python module in a certain dir.

from mod_python import apache
from mod_python import util

from string import *

import re
import imp


def GetSearchDir(options):
	if options == None:
		return None

	if options.has_key("CGIEXT_URI_Mod_Dir"):
		return options["CGIEXT_URI_Mod_Dir"]


def handler(req):
	options = req.get_options()
	SearchDir = GetSearchDir(options)

	# if configuration error, return 500
	if not SearchDir:
		return apache.HTTP_INTERNAL_SERVER_ERROR

	lastslash = rfind(req.uri, '/')
	if not lastslash:
		return apache.HTTP_NOT_FOUND

	filename = req.uri[lastslash+1:]
	if filename == None or filename == "" :
		return apache.HTTP_NOT_FOUND

	
	# the file name can contain only alphanum and _-
	if re.compile("^[a-zA-Z0-9_-]+$").match(filename) == None:
		return apache.HTTP_NOT_FOUND

	try:
		fp, pathname, description = imp.find_module(filename, [SearchDir])
	except ImportError:
		return apache.HTTP_NOT_FOUND

	try:
		mod = imp.load_module(filename, fp, pathname, description)

	finally:
		if fp:
			fp.close()

	return mod.handler(req)

