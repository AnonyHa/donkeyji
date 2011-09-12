import os
import sys
import pdb

# ----------------------------------------------------
def pre_proc(dir_path):
	files = os.listdir(dir_path)
	for file in files:
		if os.path.isdir(os.path.join(dir_path, file)):#if it is a directory, recursive
			pre_proc(os.path.join(dir_path, file))
		else:
			(name, extends) = os.path.splitext(file)
			old_path = os.path.join(dir_path, file)

			count = 0
			new_name = '_pre_%d_%s' % (count, name)
			new_path = os.path.join(dir_path, new_name + extends)

			while os.path.exists(new_path):
				count = count + 1
				new_name = '_pre_%d_%s' % (count, name)
				new_path = os.path.join(dir_path, new_name + extends)
			os.rename(old_path, new_path)

# ----------------------------------------------------
def rename_by_num(dir_path):
	pre_proc(dir_path)
	really_rename(dir_path)

def really_rename(dir_path):
	count = 0
	files = os.listdir(dir_path)
	for file_name in files:
		if os.path.isdir(os.path.join(dir_path, file_name)):#if it is a directory, recursive
			rename(os.path.join(dir_path, file_name))
		else:#if a file, rename 
			(old_name, extends) = os.path.splitext(file_name)

			old_path = os.path.join(dir_path, file_name)

			new_name = '%d' % (count,)
			new_path = os.path.join(dir_path, new_name + extends)

			os.rename(old_path, new_path)
			count = count + 1

#-----------------------------------------------------
def main():
	if len(sys.argv) == 1:
		print 'no directory to operate'
		exit()
	all_dir_path = sys.argv[1:]
	for dir_path in all_dir_path:
		rename_by_num(dir_path)

#---------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------
if __name__ == '__main__': 
	main()
