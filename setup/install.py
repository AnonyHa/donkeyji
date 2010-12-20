import os
import sys
import shutil
import time
import exceptions
# ------------------------------------------------------------------------
# extension for win32
# ------------------------------------------------------------------------
import win32api, win32pdhutil, win32con
import win32pdh, string

# ------------------------------------------------------------------------
def get_process_id(name):
    object = "Process"
    items, instances = win32pdh.EnumObjectItems(None,None,object, win32pdh.PERF_DETAIL_WIZARD)
    val = None

    if name in instances :
        hq = win32pdh.OpenQuery()
        hcs = []
        item = "ID Process"
        path = win32pdh.MakeCounterPath( (None,object,name, None, 0, item) )
        hcs.append(win32pdh.AddCounter(hq, path))
        win32pdh.CollectQueryData(hq)
        time.sleep(0.01)
        win32pdh.CollectQueryData(hq)

        for hc in hcs:
            type, val = win32pdh.GetFormattedCounterValue(hc, win32pdh.PDH_FMT_LONG)
            win32pdh.RemoveCounter(hc)
            win32pdh.CloseQuery(hq)
            return val

# ------------------------------------------------------------------------
# setup 
def my_copytree(src_dir, dst_dir, special_list):
	if not os.path.exists(dst_dir):
		print 'make dir', dst_dir
		#os.mkdir(dst_dir)
		os.makedirs(dst_dir)

	errors = []
	names = os.listdir(src_dir)
	for name in names:
		srcname = os.path.join(src_dir, name)
		dstname = os.path.join(dst_dir, name) 

		# special operation
		if name in special_list:# bak first
			if os.path.exists(dstname):
				i = 1
				new_dst_name = os.path.join(dst_dir, '%s_bak_%d' % (name, i))
				while os.path.exists(new_dst_name):
					i = i + 1
					new_dst_name = os.path.join(dst_dir, '%s_bak_%d' % (name, i))
					if i > 1000:# prevent endless loop
						print 'need to del some backup files'
						raise Exception, 'too many backup of directory %s exist, delete some of them' % name
				# finally find a name that never used
				os.rename(dstname, new_dst_name)

		try:
			if os.path.isdir(srcname):
				print '[copy dir:]', srcname
				# recursively my_copytree
				my_copytree(srcname, dstname, [])# no special list
			else:
				print '[copy file:]', srcname
				#shutil.copy2(srcname, dstname)
				shutil.copy(srcname, dstname)
		except (IOError, os.error), why:
			errors.append((srcname, dstname, str(why)))
			print 'error, some file are being used'
		except Error, err:
			errors.extend(err.args[0])
			# do not raise here

		'''
		# copy per
		try:	
			shutil.copystat(src_dir, dst_dir)
		except WindowsError:
			pass
		'''

		if errors:
			print 'have error when copy' 
			raise Exception, 'have error when copy'

# ------------------------------------------------------------------------
def setup(src_dir, dst_dir):
	# never exists, copy directly
	'''
	if not os.path.exists(dst_dir):
		try:
			shutil.copytree(src_dir, dst_dir)
		except:
			raise
	'''

	all_process = ['client', 'sdk_app', 'w9xopen', 'errorrpt']
	for proc_name in all_process:
		pid = get_process_id(proc_name)
		if pid:
			print 'you need to close your process %s first and try again' % proc_name 
			raise Exception, 'the process %s is running'

	if dst_dir == src_dir:
		print 'the src dir is same as the dst dir'
		raise Exception, 'the src dir is same as the dst dir'

	special_list = ['hello', 'hellosvr']
	my_copytree(src_dir, dst_dir, special_list)

# ------------------------------------------------------------------------
if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'wrong argument' 
		raise Exception, 'argument number wrong'

	dst_dir = sys.argv[1]# dst_dir
	print 'dst dir', dst_dir

	real_dst_dir = os.path.join(dst_dir, 'igate_sdk')#add a default dir "igate_sdk"
	print 'real dst dir', real_dst_dir
	
	setup('./sdk', real_dst_dir)
