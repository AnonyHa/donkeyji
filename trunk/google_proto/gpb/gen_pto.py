import os
import sys

dst_dir = './pto_def'

i = 0
def gen_pto(pto_dir):
	files = os.listdir(pto_dir)
	for file in files:
		path = os.path.join(dir, file)
		if os.path.isdir(path):
			if file != '.svn':# ingore svn dir
				gen_pto(path)
		else:
			global i
			i = i + 1
			cmd = 'protoc.exe %s --cpp_out=%s' % (path, dst_dir)
			print cmd
			os.system(cmd)
			name, ext = os.path.splitext(file)
			name.upper()
			f_macro.write('#define %s_T %s\n' % (name.upper(), name.lower()))
			f_macro.write('#define %s_ID %s\n' % (name.upper(), i))
			if name[0] == 's':
				f_issvr.write('\ttrue,\n')
			else:
				f_issvr.write('\tfalse,\n')
	pass

if __name__ == '__main__':
	dir = sys.argv[1]
	print dir
	f_macro = file('pto_macro.h', 'w')
	f_issvr = file('pto_issvr.c', 'w')
	f_issvr.write('bool is_svr[] = {\n')
	gen_pto(dir)
	f_macro.write('#define MAX_PROTO_ID %s\n' % i)
	f_issvr.write('};')
