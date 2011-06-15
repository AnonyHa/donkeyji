import os
import sys
import stat


def findSVNDir( path ) :
    for file in os.listdir( path ) :
        subpath = os.path.join( path, file )
        if os.path.isdir( subpath ) :
            if file == ".svn" :
                print subpath
                deleteSVNDir( subpath )
            else :
                findSVNDir( subpath )
                
def deleteSVNDir( path ) :
    for file in os.listdir( path ) :
        subpath = os.path.join( path, file )
        
        if os.path.isdir( subpath ) :
            deleteSVNDir( subpath )
        else :
            os.chmod( subpath, stat.S_IREAD | stat.S_IWRITE )
            os.remove( subpath )
            
    os.rmdir( path )

def cleanSVNDir( path ) :
    findSVNDir( path )

if __name__ == '__main__' :
	if len(sys.argv) == 1:
		print 'wrong arg'
		exit()
	dirs = sys.argv[1:]
	for dir in dirs:
		cleanSVNDir(dir)
