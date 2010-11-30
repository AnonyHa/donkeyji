for_maker = {}
for_caller = {}

for_maker['c_test'] = function(a, b)
	print('a = ', a)
	print('b = ', b)
	--for_caller['s_test'](a, b)
end

---º”‘ÿ–≠“È
networklib.add_pto('s_test')--0
networklib.add_pto('c_test')--1
