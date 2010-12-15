import urllib

fd = urllib.urlopen('http://hi.163.com/simpleapi/game/role/get_token?game=itown')
rsp = fd.read()
print rsp
