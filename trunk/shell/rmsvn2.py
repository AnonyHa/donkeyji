import os
import sys

if __name__ == '__main__' :
	if len(sys.argv) == 1:
		print 'wrong arg'
		exit()
	dirs = sys.argv[1:]
	all_svn = []
	for _dir in dirs:
		for (p,d,f) in os.walk(_dir):
			#print p
			#print p[-4:]
			#if len(p) >= 4:
			if p[-4:] == '.svn':
				all_svn.append(p)

	#print len(all_svn)
	for svn_path in all_svn:
		#os.popen('rm -Rf "%s"'%p)
		os.system('rm -Rf "%s"'%svn_path)
		print svn_path
