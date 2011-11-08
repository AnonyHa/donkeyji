#-*- coding: GB2312 -*-
# $Id: cgi_util.py 28562 2008-03-13 07:45:02Z linda $


import string
import time
from cStringIO import StringIO
import cgi_error

YEAR_MIN = 2002
YEAR_MAX = 2010
MONTH_MIN = 1
MONTH_MAX = 12
DAY_MIN = 1
DAY_MAX = 31

def convert_encode(s, fromenc, toenc, mode='strict'):
	u = unicode(s, fromenc, mode)
	return u.encode(toenc, mode)

def calc_next_month(year, month):
	n_year = year
	n_month = month+1
	if n_month>12:
		n_month = 1
		n_year += 1
	return n_year, n_month

def safe_inc_map_value(m, k, inc=1):
	i = m.get(k, 0)
	m[k] = i+inc
	return i+inc


def safe_issubclass(c, classinfo):
	try:
		if issubclass(c, classinfo):
			return True
	except TypeError:
		return False


def safe_int(i):
	if i == None:
		return None

	try:
		return int(i)
	except (ValueError, TypeError):
		return None


def force_int(i):
	if i == None:
		return 0
	try:
		return int(i)
	except (ValueError, TypeError):
		return 0


def safe_itoa(i):
	if i == None:
		return '0'
	return str(i)


# get the first element in a sequence if it != None
def almostsafe_getfirst(m):
	if m == None:
		return None

	if len(m) == 0:
		return None
	
	return m[0]


def safe_float(i):
	if i == None:
		return None

	try:
		return float(i)
	except ValueError:
		return None


def safe_str(s):
	if s == None:
		return ""
	return str(s)


def is_integer(i):
	return type(i) == int or type(i)==long

def is_postive_int(i):
	if is_integer(i) and i>0:
		return True
	return False


def ParseDBTimeStr(s):
	st = time.strptime(s, "%Y-%m-%d %H:%M:%S")

	import datetime
	return datetime.datetime(st[0], st[1], st[2], st[3], st[4], st[5])


def TimeDelta2Str(td, how_precise=None):
	s = ''
	p = 0
	if td.days != 0:
		s += '%d天' % td.days
		p += 1

	if how_precise != None and p >= how_precise:
		return s
	
	if td.seconds != 0:
		h = td.seconds//3600
		r = td.seconds%3600
		m = r//60
		sec = r%60
		if h != 0:
			s += '%d小时' % h
			p += 1

		if how_precise != None and p >= how_precise:
			return s
		
		if m != 0:
			s += '%d分钟'% m
			p +=1

		if how_precise != None and p >= how_precise:
			return s
		
		if sec != 0:
			s += '%d秒' % sec

	return s


def GenerateRandomStr(keylen):
	keychar = "abcdefghijklimopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
	fd = file("/dev/urandom", 'r');
        if fd == None :
                return False

	randnum = fd.read(keylen)
	if (len(randnum)!=keylen) :
		fd.close()
		return False

	keybuf = ""
	for i in xrange(keylen) :
		c = ord(randnum[i])
		keybuf += keychar[c%64];

	fd.close()
	return keybuf


def RandomIntCode():
	fd = file("/dev/urandom", 'r');
	if fd == None :
		return None

	randnum = fd.read(4)
	if (len(randnum)!=4) :
		fd.close()
		return False

	fd.close()
	
	import struct
	r = struct.unpack("I", randnum)[0]
	# r is unsinged
	
	return r
 
def IsEmailAddrStr(sEmail) :
	"""If sEmail is a email address, return 1, other than return 0.
	"""
	nIndex = sEmail.find('@')
	if nIndex in (0, -1, len(sEmail)-1) :
		return False
	if sEmail.find('.')==-1:
		return False
	return True


def IsValidUserName(name, minlen=-1, maxlen=-1):
	if len(name)==0 :
		return False

	for i in xrange(len(name)) :
		c = name[i]
		if not c.isalnum() and c != '_' and c != '.' and c != '-':
			return False
	
	if (minlen >= 0 and len(name) < minlen) or \
		(maxlen >= 0 and len(name) > maxlen):
		return False

	return True


# a class for record some cgi vars
# later you can get them back in the form of QueryString
class CgiVarRecorder(object):
	def __init__(self):
		self._var_table = []


	def RegVar(self, name, value):
		if value != None:
			self._var_table.append((name, str(value)) )


	def QueryString(self):
		import urllib
		return urllib.urlencode(self._var_table)

	
	
# copied from Python Cook book
def unique(s):
	""" Return a list of the elements in s in arbitrary order, but without
	duplicates. """

	# Get the special case of an empty s out of the way very rapidly
	n = len(s)
	if n == 0:
		return []

	# Try using a dict first, because it's the fastest and will usually work
	u = {}
	try:
		for x in s:
			u[x] = 1
	except TypeError:
		del u  # Move on to the next method
	else:
		return u.keys(  )

	# Since you can't hash all elements, try sorting, to bring equal items
	# together and weed them out in a single pass
	try:
		t = list(s)
		t.sort(  )
	except TypeError:
		del t  # Move on to the next method
	else:
		assert n > 0
		last = t[0]
		lasti = i = 1
		while i < n:
			if t[i] != last:
				t[lasti] = last = t[i]
				lasti += 1
			i += 1
		
		return t[:lasti]

	# Brute force is all that's left
	u = []
	for x in s:
		if x not in u:
			u.append(x)
	return u

		

def get_price_int(req, name):
	f = safe_float(req.params.get(name))
	if f == None:
		return None
		
	return safe_int(f*100)


def safe_strip(v):
	if v != None:
		return v.strip()

	return v
	

def req_safe_getdatestr(v):
	import datetime
	try:
		t = time.strptime(v, "%Y-%m-%d")
		return datetime.date(t[0], t[1], t[2])
		
	except ValueError:
		return None


def ReplaceSp4Disp(s):
	import jon
	return jon.cgi.html_encode(s)


def ReplaceSp4Value(s):
	s = string.replace(s, '"', '&amp;')
	s = string.replace(s, '"', '&quot;')
	return s


# filter the unuseable string, such as "c:\windows" or "d:\picture" and so on
def revise_upfile_name(filename):
        return filename[ string.rfind(filename,'\\') + 1 : ].lower()


def get_file_type(filename):
	return filename[ string.rfind(filename,'.') + 1 : ].lower()


#Parser for ini config
"""
returns a dictionary with keys of the form
<section>.<option> and the corresponding values
[gs_local_reader]
name = gamesales
user = gswriter

[gs_slave_reader]
name = gamesales
user = gswriter

the LoadConfid will return 
{'gs_local_reader.name': 'gamesales',
'gs_local_reader.user': 'gswriter',
'gs_slave_reader.name': 'gamesales',
'gs_slave_reader.name': 'gswriter',}

"""
def LoadConfig(file):
	import ConfigParser
	config = {}
	cp = ConfigParser.ConfigParser()
	cp.read(file)
	for sec in cp.sections():
		name = string.lower(sec)
		for opt in cp.options(sec):
		    config[name + "." + string.lower(opt)] = string.strip(cp.get(sec, opt))
	return config


#=============================================patten util=============================================

# to get two labels, which define a block in template file	
def CextGet2Label(cext, b, e, throw_except=True):

	tc_b = cext.getLabel(b)
	tc_e = cext.getLabel(e)

	if tc_b >= tc_e or tc_b<0 or tc_e<0:
		if not throw_except:
			return None, None
		raise

	return tc_b, tc_e
	

def fast_get_label( cext, label_name ):
	return ( cext.getLabel( label_name + "Begin" ), cext.getLabel( label_name + "End" ) )

#disable a pair of label
def fast_disable_label( cext, label_name ):
	lb, le = fast_get_label( cext, label_name )
	if lb == -1 or le == -1 : 
		return
	cext.disable(lb + 1, le -1)

#enable a pair of label.
def fast_enable_label(cext, label_name):
	lb, le = fast_get_label(cext, label_name)
	if lb == -1 or le == -1 : 
		return 
	cext.enable(lb + 1, le -1)
	
	
# repeat set dictionary value
class RepeatSetVar(object):
	def __init__(self, result_set, autodisable=True):
		super(RepeatSetVar, self).__init__()
		self._result_set = result_set
		if self._result_set == None:
			self._result_set = ()
		self._autodisable = autodisable
	
	def rename_values(self, key_name, rename_dic):
		#in result set if you want change a key's value you must give key_name and rename_dic arguments
		#then the value in result which key is key_name can be raname by value defined in rename_dic dictionary
		
		# key_name must be string type
		key_name = str(key_name)
		
		for i in self._result_set:
			if i.has_key(key_name) and rename_dic.has_key(i[key_name]):
				i[key_name] = rename_dic[i[key_name]]
			else:
				raise KeyError(key_name)

		return self._result_set
	

	def checked_value(self, key_name, value_list, label_name, True_result, False_result):
		# you can choice which set be checked, selected or disabled use this function before repeat set var
		key_name = str(key_name)

		for i in self._result_set:
			if i[key_name] in value_list:
				i[label_name] = str(True_result)
			else:
				i[label_name] = str(False_result)

		return self._result_set
	
	def set_result_var(self, cext, label_name):
		#repeat set value between <!--#CGIEXT# ???Begin:--> and <!--#CGIEXT# ???End:--> labels
		#you can use functions in this class to do some related operate before set value
		if self._result_set == ():
			if self._autodisable:
				fast_disable_label( cext, label_name )
			else:
				pass

		(lb, le) = fast_get_label( cext, label_name )
		stro = StringIO()

		for i in xrange( len(self._result_set) ):
			for k, v in self._result_set[i].items():
				cext.setVal( str(k), str(v) )

			cext.output( lb+1, le-1, stro )
			cext.clean(lb+1, le-1)

		cext.replaceSection(lb+1, le-1, stro.getvalue())
		stro.close()


	def get_result( self ):
		return self._result_set


        			
# gp == go_page, tp = total_page, epn = every_page_num
def get_be_page(gp, tp, epn):
	if gp <= epn/2 + epn%2:
		beginp = 1
		if tp < epn:
			endp = tp
		else:
			endp = epn
		return beginp, endp
	if gp > tp - epn/2 - epn%2:
		endp = tp
		beginp = tp - epn + 1
		if beginp < 1:
			beginp = 1
		return beginp, endp

	beginp = gp - epn/2
	endp = gp + epn/2 - 1 + epn%2
	return beginp, endp


#
#PageMange class manage page's number , generate the various page link's start page and 
# the passing query string the web page need
#

#NOTE: all page-related value is int in this object 
class PageManage(object):
	# here must recv int argument , so you may need to force it change to string type later
	# page is current page
	# every page nums can't be 0
	# page num is number of page such as 1, 2, .....9
	def __init__(self, page, total_nums, every_page_nums=1, page_num = 10):
		super(PageManage, self).__init__()
		# divisor can't be zero
		if every_page_nums == 0 :
			every_page_nums = 1

		self._page = safe_int(page)
		self._total_nums = total_nums
		self._every_page_nums = every_page_nums
		self._page_num = page_num
		if total_nums == 0:
			self._total_pages = 1
		elif self._total_nums % self._every_page_nums == 0:
			self._total_pages = safe_int(self._total_nums/self._every_page_nums) 
		else:
			self._total_pages = safe_int(self._total_nums/self._every_page_nums) + 1

		if self._total_pages < self._page:
			# can add an exception class to throw out
			self._page = self._total_pages
			
		if self._page <=0 :
			self._page = 1

		#  next page
		self._next_page = safe_int(self._page) + 1
		if self._next_page > self._total_pages:
			self._next_page = self._page

		# previous page 
		self._previous_page = safe_int(self._page) - 1
		if self._previous_page < 1:
			self._previous_page = self._page

		# page num . pnb is page number begin
		self._pnb, self._pne = get_be_page(self._page, self._total_pages, self._page_num)
		
		self._begin_pos = (self._page - 1) * self._every_page_nums
		self._passargs = CgiVarRecorder()

	def get_begin_pos(self):
		return self._begin_pos

	def get_next_page(self):
		return self._next_page
	
	def get_previous_page(self):
		return self._previous_page
			
	def get_first_page(self):
		return 1	

	def get_last_page(self):
		return self._total_pages
		
	def get_cur_page(self):
		return self._page

	def get_total_page(self):
		return self._total_pages
	
	def get_every_page_nums(self):
		return self._every_page_nums
		
	def get_var_recorder(self):
		return self._passargs
		
	# register the argument which need to be passed, then the caller can set cext var
	def reg_pass_var(self, n, v):
		# if the var is not reg , pass query string will be set as '', so you can call this function or not as you like
		self._passargs.RegVar(n, v)		
	
	# set page var
	def set_cext_var(self, cext, disflag=True):
		passqs = self._passargs.QueryString()
		cext.setVal('PassQueryString', passqs)
		
		tb, te = fast_get_label(cext, 'Page')
		cext.enable(tb+1, te-1)
		if self._total_pages <=1 and disflag:
			cext.disable(tb+1, te-1)
		else:
			cext.enable(tb+1, te-1)

		fpb, fpe = fast_get_label(cext, 'FPPage')
		nlb, nle = fast_get_label(cext, 'NLPage')
		if self._page == 1:
			# no previous page, no first page
			# last page will be set as total page later
			cext.disable(fpb+1, fpe-1)
			
		if self._page == self._total_pages:
			# no next page, no last page
			cext.disable(nlb+1, nle-1)
		
		# previous page and next page will be set by cext module as current page +1 and current page -1
		cext.setVal('TotalPage', str(self._total_pages))
		cext.setVal('CurrentPage', str(self._page))

		# set page numbers
		pnb, pne = fast_get_label(cext, 'PageNum')
		linkb, linke = fast_get_label(cext, 'PagePreLink')
		linkb1, linke1 = fast_get_label(cext, 'PageEndLink')
		ios = StringIO()
		for page_num in xrange(self._pnb, self._pne+1):
			if page_num == self._page:
				cext.disable(linkb+1, linke-1)
				cext.disable(linkb1+1, linke1-1)
			else:
				cext.enable(linkb+1, linke-1)
				cext.enable(linkb1+1, linke1-1)

			cext.setVal('PageNum', str(page_num))
			cext.output(pnb+1, pne-1, ios)
			cext.clean(pnb+1, pne-1)
		cext.replaceSection(pnb+1, pne-1, ios.getvalue().replace('\r\n','').replace('\n',''))
		ios.close()
		
		
#=========================oneqiong==============================


def SetYearSelect(cext, ChooseItem = "", LabelName = "YearSelect"):
	if ChooseItem == "" or ChooseItem == None:
		ChooseItem, m, d,i,g,n,o,r,e = time.localtime(time.time())
	SetListSelect(cext, xrange(YEAR_MIN, YEAR_MAX + 1), \
		xrange(YEAR_MIN, YEAR_MAX + 1), ChooseItem, LabelName)

def SetMonthSelect(cext, ChooseItem = "", LabelName = "MonthSelect"):
	if ChooseItem == "" or ChooseItem == None:
		y, ChooseItem, d,i,g,n,o,r,e = time.localtime(time.time())	
	SetListSelect(cext, xrange(MONTH_MIN, MONTH_MAX + 1),\
		xrange(MONTH_MIN, MONTH_MAX + 1), ChooseItem, LabelName)

def SetDaySelect(cext, ChooseItem = "", LabelName = "DaySelect"):
	if ChooseItem == "" or ChooseItem == None:
		y, m, ChooseItem,i,g,n,o,r,e = time.localtime(time.time())
	SetListSelect(cext, xrange(DAY_MIN, DAY_MAX + 1), \
		xrange(DAY_MIN, DAY_MAX + 1), ChooseItem, LabelName)


#pass a dictionary , set the dictionary to select button .
#eg if you pass {"select1":"name1", "select2":"name2", ...}, 
#the select button will be : "select1":"name1", "select2":"name2", ...
def SetSelect(cext, SelectDic, ChooseItem = "", LabelName = "Select"):
	if len(SelectDic) == 0 : 
		fast_disable_label(cext, LabelName)
		return 
	SetListSelect(cext, SelectDic.keys(), SelectDic.values(), ChooseItem, LabelName)


#pass two list or tuple one represent select values, another represent select names , 
#connect the pair of container to select button .eg if you pass ("select1", "select2", ...) ("name1", "name2", ...)
#the select button will be : "select1":"name1", "select2":"name2", ... this function suit for year month day select
# button and so on.
def SetListSelect(cext, SelectValues, SelectNames, ChooseItem = "", LabelName = "Select"):
	if len(SelectValues) != len(SelectNames) : return 
	
	Begin , End = fast_get_label(cext, LabelName)
	#if pat does't have the pair of label, not set select button 
	if Begin == -1 or End == -1 : return
	
	Buf = StringIO()
	for item in xrange(len(SelectValues)):
		cext.setVal("ItemValue", str(SelectValues[item]) )
		cext.setVal("ItemName", str(SelectNames[item]) )
		if str(ChooseItem) == str(SelectValues[item]):
			cext.setVal("Selected", "selected")
		else:
			cext.setVal("Selected", "")
		cext.output(Begin + 1, End - 1, Buf)
		cext.clean(Begin + 1, End - 1)
	
	cext.replaceSection(Begin + 1, End -1, Buf.getvalue() )
	Buf.close()



#FUNCTION :repeating output database dictionary records to a form list
#PARAMETER:cext		: MYCEXT obj
#	   ResultsDic	: database results select from database the type of which id a list of deictionary 
#	   LabelName	: the pat label name of single record default "LineBegin" and "LineEnd".

#USAGE:	eg : the results you query from database is :
#	results = ({"agentpassnem":"haha", "agentid":22, "cityid":5},{"agentpassnem":"hehe", "agentid":24, "cityid":6},...)}
#	1. pass the results to parameter ResultsDic, 
#	2. first set the pat label <!--#CGIEXT# LineBegin:--> and <!--#CGIEXT# LineEnd:-->
#	3. set the variable the same as the database field name (here is "agentpassnem", "agentid", "cityid")
def SetResultsByDic(cext, ResultsDic, LabelName = "Line", DefaultVal = "-"):
	if len(ResultsDic) == 0 : 
		fast_disable_label(cext, LabelName)
		return 
	
	Begin, End = fast_get_label(cext, LabelName)
	if Begin == -1 or End == -1 : 
		return  
	
	Buf = StringIO()
	for res in ResultsDic:
		SetOneResultByList(cext, res.values(), res.keys(), DefaultVal)
		cext.output(Begin + 1, End - 1, Buf)
		cext.clean(Begin + 1, End - 1)
	
	cext.replaceSection(Begin + 1, End - 1, Buf.getvalue())
	Buf.close()

#FUNCTION :repeating output database list records to a form list
#PARAMETER:cext		: MYCEXT obj
#	   ResultsList	: database results select from database the type of which id a list of two dimensions
#	   FieldList	: database results's corresponding field name
#	   LabelName	: the pat label name of single record default "LineBegin" and "LineEnd"

#USAGE:	eg : the results you query from database is :
#	results = (("haha", 22, 5),("hehe", 24, 6),...)) corresponded field is ("agentpassnem", "agentid", "cityid" )
#	1. pass the results to parameter ResultsDic, and field to FieldList,  
#	2. set the pat label <!--#CGIEXT# LineBegin:--> and <!--#CGIEXT# LineEnd:-->, 
#	3. set the variable the same as the database field name (in this example is "agentpassnem", "agentid", "cityid")
def SetResultsByList(cext, ResultsList, FieldList, LabelName = "Line", DefaultVal = "-"):
	if len(ResultsList) == 0:
		fast_disable_label(cext, LabelName)
		return 
	
	Begin, End = fast_get_label(cext, LabelName)
	if Begin == -1 or End == -1 : 
		return  
	
	Buf = StringIO()
	for res in ResultsList:
		SetOneResultByList(cext, res, FieldList, DefaultVal)
		cext.output(Begin + 1, End - 1, Buf)
		cext.clean(Begin + 1, End - 1)
	
	cext.replaceSection(Begin + 1, End - 1, Buf.getvalue())
	Buf.close()


#if you select database records using dictionary format and the count of results only have one record , 
#you can pass cext object, database the single result (*the type of ResultDic is dictionary type*), 
#the results wiil be output.
def SetOneResultByDic(cext, ResultDic, DefaultVal = "-"):
	SetOneResultByList(cext, ResultDic.values(), ResultDic.keys(), DefaultVal)


#if you select database records using dictionary format , and want to output the results
#you can pass cext object, database the single resut(*the result of ResultList is dictionary type*), 
#the results wiil be output.
def SetOneResultByList(cext, ResultList, FieldList, DefaultVal = "-"):
	for item in xrange(len(ResultList)):
		if ResultList[item] == None or ResultList[item] == "":
			cext.setVal(str(FieldList[item]) , DefaultVal)
		else :
			cext.setVal(str(FieldList[item]) , str(ResultList[item]))
	return 

HAS_COOKIE = 1
NO_COOKIE = 0
class UrsCheck:
	"""
	the urs login check of v3
	"""
	def __init__(self):
		self._check_code = None
		self._check_msg = ""
		self._cookie = ""
		self._results = None
	
	def get_cookie(self):
		return self._cookie
	
	def get_check_code(self):
		return self._check_code
	
	def get_check_msg(self):
		return self._check_msg
	
	def check(self, urs, passwd, ip, product, cookie_type=0):
		"""
		if the interface return 200 or 201 return true, else return false
		"""
		import urllib
		import md5
		url = "http://reg.163.com/services/userlogin?"
		#passwd = passwd.replace("\\", "\\\\").replace("'", "\\'")
		uri = urllib.urlencode({"username":urs, "password":md5.new(passwd).hexdigest(), \
			"userip":ip, "product":product, "type":cookie_type})  
		url +=uri
		try:
			fp=urllib.urlopen(url)
		except urllib.socket.error:
			return False
		
		if fp==None :
			return False
		self._results = fp.read().split("\n")
		
		self._check_code = self._results[0]
		if not self._check_code.isdigit():
			raise cgi_error.UnexceptedError, "unexcepted urs_check interface return value:%s"%url
		
		self.__set_check_msg()
		self.__set_cookie_str()
		
		if self._check_code not in ("200", "201"):
			return False
		
		return True

	def __set_check_msg(self):
		for res in self._results[1:]:
			if res=="":
				break
			self._check_msg += res
	
	def __set_cookie_str(self):
		try:
			cookie_index = self._results.index("")+1
		except ValueError:
			return

		try:
			other_dict = dict([k.split("=") for k in self._results[cookie_index].split("&")])
			self._cookie = other_dict["cookie"]
		except IndexError:
			return
		except KeyError:
			return

if __name__=="__main__":
	check_obj = UrsCheck()
	if not check_obj.check("ursjj", "aaaaaaa", "192.168.10.233", "gps", 1):
		print "oh! why"

	print check_obj.get_check_code()
	print check_obj.get_check_msg()
	print check_obj.get_cookie()
