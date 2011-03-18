#coding:gbk
import a

game_name = "连连看"
desc = "很好的成就"
content = "获得了#c9BFF52%s#n的#c00B6FF%s#n成就" % (game_name, desc)
content = '%s%s%s' % (a.a, a.b, content)
content = content.decode('gbk').encode('utf-8')
print content
