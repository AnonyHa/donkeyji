#coding:gbk
import sys
from optparse import OptionParser


__description__ = ''
__version__ = ''
__usage__ = 'usage: python mykdig.py inputfile outputfile'

parser = OptionParser(description=__description__, usage=__usage__, version=__version__)
#parser.add_option('-v', '--verbose', 
                    #dest='verbose', action='store_true', default=False,
                    #help='Output many log to stderr')

#parser.add_option('-o', '--output', 
                    #dest='output file', action='store_true', default='./out.txt',
                    #help='Output file to write')
options = None
args = []

def do_kdig(infile, outfile):
	result = []
	f = open(infile, 'r')
	for line in f:
		items = line.rstrip('\n').split(',')
		#print type(items)
		result.append(items)

	result.sort(key=lambda d:float(d[1]))
	#print result

	fo = file(outfile, 'w')
	for items in result:
		fo.write(','.join(items))
		fo.write('\n')
	

if __name__ == '__main__':
	options, args = parser.parse_args()
	print options

	print args

	if len(args) < 2:
		parser.print_help()
		sys.exit(1)

	infile = args[0]# input file
	outfile = args[1]# output file
	do_kdig(infile, outfile)
