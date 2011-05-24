#coding:gbk
import traceback

def step_1():
	raise '111'

def step_2():
	raise '222'

def step_3():
	pass

def do_test():
	try:
		step_1()# raise inside
	except:
		raise

	try:
		step_2()# raise inside
	except:
		pass

	step_3()# no raise inside

def init_test():
	raise 'init'

if __name__ == "__main__":
	init_test()# if raise, exit directly

	while 1:# if raise, donot exit, just catch the exception
		try:
			do_test()
		except:
			pass
		eleep(1)
