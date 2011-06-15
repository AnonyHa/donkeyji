import os
import sys
import pdb

#---------------------------------------------------------------------------------------------------------------
def rename(dir):

	count = 0
	files = os.listdir(dir)
	newNameExist = []

	for file in files:
		if os.path.isdir(os.path.join(dir, file)):#if it is a directory, recursive
			rename(os.path.join(dir, file))
		else:#if a file, rename 
			(name, extends) = os.path.splitext(file)

			old = os.path.join(dir, file)
			new = os.path.join(dir, "%d" % (count,) + extends)

			if old in newNameExist:
				newNameExist.remove(old)
				if os.path.exists(new): 
					newNameExist.append(new)
				continue
			else:
				if os.path.exists(new): 
					if new == old:#itself
						count = count +1
						continue
					else:
						while os.path.exists(new):#if new name exists, pass it and count increase
							newNameExist.append(new)
							count = count + 1
							new = os.path.join(dir, "%d" % (count,) + extends)

			os.rename(old, new)
			count = count + 1

#---------------------------------------------------------------------------------------------------------------
def main():
	if len(sys.argv) == 1:
		print 'no directory to operate'
		exit()
	dirs = sys.argv[1:]
	for dir in dirs:
		rename(dir)

#---------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------
if __name__ == '__main__': 
	main()
