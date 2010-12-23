import os
import sys

def check_class_name(pto_file):
	f = file(pto_file, 'r')
	data = f.read()
	pass

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
			check_class_name(path)
			name, ext = os.path.splitext(file)
			PROTO_TYPE = '%s_T' % name.upper()
			PROTO_ID = '%s_ID' % name.upper()
			f_macro.write('#define %s %s\n' % (PROTO_TYPE, name.lower()))
			f_macro.write('#define %s %s\n' % (PROTO_ID, i))
			if name[0] == 's':
				f_issvr.write('\ttrue,\n')
			else:
				f_issvr.write('\tfalse,\n')
				declare_str = '''
int send_%s(%s* obj);
				'''
				f_sendpto_h.write(declare_str)

				func_str = '''
//---------------------------
//send %s interface
//---------------------------
int send_%s(%s* obj)
{
	char buf[2048];
	unsigned int proto_id = %s;
	int proto_len = sizeof(unsigned int);
	memcpy(buf, &proto_id, proto_len);
	memcpy(buf + proto_len, obj);
	return 0;
}\n
				''' % (PROTO_TYPE, PROTO_TYPE, PROTO_TYPE, PROTO_ID)
				f_sendpto_c.write(func_str)
	pass

if __name__ == '__main__':
	dir = sys.argv[1]
	print dir
	f_macro = file('pto_macro.h', 'w')
	f_issvr = file('pto_issvr.c', 'w')

	f_sendpto_c = file('pto_send.c', 'w')
	f_sendpto_c.write('#include "pto_macro.h"\n')

	f_sendpto_h = file('pto_send.h', 'w')
	f_sendpto_h.write('#ifndef _SEND_PTO_H\n#define _SEND_PTO_H\n')

	f_issvr.write('bool is_svr[] = {\n')
	gen_pto(dir)
	f_macro.write('#define MAX_PROTO_ID %s\n' % i)
	f_issvr.write('};')
	f_sendpto_h.write(
			'''
#endif
			'''
	)
