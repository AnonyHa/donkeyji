import os
import signal
import time

print '[start]'
for i in range(0, 5000, 1):
	os.system("./main_c&")

print '[sleep]'
time.sleep(3)

print '[kill]'
os.system("ps -C main_c -o pid>pid.txt")
f = file('pid.txt', 'r')
i = 0
for line in f:
	if i == 0:
		i = i + 1
		continue
	else:
		new_line = line.rstrip('\n')
		pid = int(new_line)
		os.kill(pid, signal.SIGTERM)
		#time.sleep(0.1)

#os.system('killall main_c')

print('[over]')
