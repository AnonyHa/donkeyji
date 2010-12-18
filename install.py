import os
import sys
import shutil

def _copy(src_dir, dst_dir, special_list):
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
				# finally find a name that never used
				os.rename(dstname, new_dst_name)

		try:
			if os.path.isdir(srcname):
				print '----', srcname
				# recursively _copy
				_copy(srcname, dstname, [])# no special list
			else:
				print '++++', srcname
				shutil.copy2(srcname, dstname)
		except (IOError, os.error), why:
			errors.append((srcname, dstname, str(why)))
			print 'error, some file are being used'
		except Error, err:
			errors.extend(err.args[0])
			#exit(1)

		try:	
			shutil.copystat(src_dir, dst_dir)
		except WindowsError:
			pass
		if errors:
			raise Error(errors)

def setup(src_dir, dst_dir):
	# never exists, copy directly
	if not os.path.exists(dst_dir):
		try:
			shutil.copytree(src_dir, dst_dir)
		except:
			raise

	if dst_dir == src_dir:
		print 'same dir'
		return

	special_list = ['hello', 'hellosvr']
	_copy(src_dir, dst_dir, special_list)

#------------------------------
if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'argument wrong'
		raise

	dst_dir = sys.argv[1]
	print 'dst dir', dst_dir

	real_dst_dir = os.path.join(dst_dir, 'itown_sdk')
	
	setup('./sdk', real_dst_dir)
