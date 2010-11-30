import os

#------------------------------------------
USER_DEFINED = {
}

# internal type
INTERNAL_DEFINED = {
		'int' : (int, long),
		'float' : (float,),
		'str' : (str,),
}

# seprator
LIST_SEP = ';'
DICT_SEP = ','
KV_SEP = ':'

#------------------------------------------
class proto:
	def __init__(self, id, params):
		self.id = id
		self.params = params# a dict

	def load_proto(self):
		self.__check_proto()

	# check when load proto, if error appear, raise exception to kill this program
	def __check_proto(self):
		for k in self.params:
			fmt = self.params[k][0]
			# check param type
			if (fmt not in USER_DEFINED) and (fmt not in INTERNAL_DEFINED):
				raise Exception('invalid param type: %s' % fmt)

	def __is_internal_type(self, fmt):
		if fmt in INTERNAL_DEFINED:
			return True
		else:
			return False

	# ---------------------------------------------------------
	# pack method
	# ---------------------------------------------------------

	# retrun a string
	def pack(self, dct):
		return '%s@%s' % (self.id, self.__pack(self.params, dct))

	# just check dct, it's uneccessary to check fmt
	def __pack(self, args, dct):
		str_dct = {}# key and value are both str
		for k in dct:
			if k not in args:# not in config
				continue
			val = dct[k]# value to pack
			fmt = args[k][0]
			if len(args[k]) == 2 and args[k][1] == 0:
				islst = 1
			else:
				islst = 0

			if islst == 1:# val should be an array 
				str_dct[k] = self.__list2str(fmt, val)
			else:
				str_dct[k] = self.__type2str(fmt, val)# pack val as fmt

		pack_str = self.__strdict2str(str_dct)# pack str_dct to a string

		return pack_str

	# a list to str
	def __list2str(self, fmt, lst):
		if type(lst) != list:
			raise Exception('not a list')

		str_list = []
		for item in lst:
			item_str = self.__type2str(fmt, item)
			str_list.append(item_str)

		pack_str = self.__strlist2str(str_list)
		return pack_str

	# data must be int/float/str or a dict
	def __type2str(self, fmt, data):
		if self.__is_internal_type(fmt):
			if type(data) not in INTERNAL_DEFINED[fmt]:
				raise Exception('invalid value')
			ret = '%s' % data
		else:
			args = USER_DEFINED[fmt]# cannot be None
			ret = self.__pack(args, data)
		return ret

	# dct is a dct of which all keys and values are str, like {'a':'7', 'b':'8'}
	def __strdict2str(self, dct):
		tmp_lst = []
		for k in dct:
			tmp_lst.append('%s%s%s' % (k, KV_SEP, dct[k]))
		return DICT_SEP.join(tmp_lst)

	# lst is a list of which all items are str, like ['a', '7', 'c']
	def __strlist2str(self, lst):
		return LIST_SEP.join(lst)

	# ---------------------------------------------------------
	#  unpack method
	# ---------------------------------------------------------

	# return a dict
	def unpack(self, string):
		return self.__unpack(self.params, string)

	def __unpack(self, params, string):
		str_dict = self.__str2strdict(string)
		dct = {}
		for k in str_dict:
			if k not in params:# if not in config, ignore it
				continue
			fmt = params[k][0]
			if len(params[k]) == 2 and params[k][1] == 0:
				is_list = 1
			else:
				is_list = 0
			if is_list:
				dct[k] = self.__str2list(fmt, str_dict[k])
			else:
				dct[k] = self.__str2type(fmt, str_dict[k])
		return dct

	def __str2strdict(self, string):
		dct = {}
		kv_list = string.split(DICT_SEP)
		for kv in kv_list:
			idx = kv.find(KV_SEP)
			key = kv[:idx]
			val = kv[idx+1:]
			dct[key] = val
		return dct

	def __str2type(self, fmt, string):
		if self.__is_internal_type(fmt):
			if fmt == 'int':
				data = int(string)
			elif fmt == 'float':
				data = long(string)
			elif fmt == 'float':
				data = float(string)
			elif fmt == 'str':
				data = string
		else:
			data = self.__unpack(USER_DEFINED[fmt], string)
		return data

	def __str2list(self, fmt, list_str):
		items = list_str.split(LIST_SEP)
		lst = []
		for item in items:
			lst.append(self.__str2type(fmt, item))
		return lst

#------------------------------------------
class proto_manager:
	def __init__(self):
		self.protos = {}

	def load_protos(self, path):
		self.__load_types(os.path.join(path, 'types'))
		self.__load_protos(path)

	def __load_protos(self, path):
		files = os.listdir(path)
		for file in files:
			file_path = os.path.join(path, file)
			if os.path.isdir(file_path):
				if file != '.svn':# exclude svn dir
					self.__load_protos(file_path)
			else:
				pto_name, ext = os.path.splitext(file)
				pto_params = eval(open(file_path, 'r').read())
				if pto_name in self.protos:
					raise Exception('same proto name: %s' % pto_name)
				pto = proto(pto_name, pto_params)
				self.protos[pto_name] = pto

	def __load_types(self, path):
		if not os.path.exists(path):
			raise Exception('type not exists')

		files = os.listdir(path)
		for file in files:
			file_path = os.path.join(path, file)
			if os.path.isdir(file_path):
				if file != '.svn':# exclude svn dir
					self.__load_types(file_path)
			else:
				type_name, ext = os.path.splitext(file)
				type_params = eval(open(file_path, 'r').read())
				if type_name in USER_DEFINED:
					raise Exception('same type name: %s' % type_name)
				USER_DEFINED[type_name] = type_params

	def get_proto(self, id):
		if id in self.protos:
			return self.protos[id]
		else:
			return None

#------------------------------------------
# init pto_mgr
pto_mgr = None
def proto_init():
	pto_mgr = proto_manager()
	try:
		pto_mgr.load_protos('./protos')
	except Exception, e:
		print e
		return

	pto = pto_mgr.get_proto('proto_test')

	try:# need to catch exception, check whether error occurs
		dd = pto.pack({
				'uid' : 1,
				'name' : 'huji',
				'info' : {'sex' : 2}, 
				'array' : [1, 2, 3],
			}
		)
	except Exception, e:
		print e
		return
	print dd

	pto_id, pto_msg = dd.split('@')
	try:# only catch exception here, in the top level
		ff = pto_mgr.get_proto(pto_id).unpack(pto_msg)
	except:
		print 'except when unpack'
		return
	print ff

if __name__ == '__main__':
	proto_init()
