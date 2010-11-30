for_maker = {}
for_caller = {}

---定义消息处理函数
for_maker['s_test'] = function(vfd, a, b)
	print('a = ', a)
	print('b = ', b)
	for_caller['c_test'](vfd, a, b)
end

---加载协议
networklib.add_pto('s_test')
networklib.add_pto('c_test')
