for_maker = {}
for_caller = {}

---������Ϣ������
for_maker['s_test'] = function(vfd, a, b)
	print('a = ', a)
	print('b = ', b)
	for_caller['c_test'](vfd, a, b)
end

---����Э��
networklib.add_pto('s_test')
networklib.add_pto('c_test')
