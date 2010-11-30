import gevent
from gevent import socket
from gevent import core

def f(ev, type):
	print '88888'

core.init()

#core.dispatch()
core.loop()
print '00000000000'
