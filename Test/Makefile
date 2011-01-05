CC = gcc

CFLAGS = -Wall -g #make������ʹ�����������c����������
#CXXFLAGS = #c++����������
#CPPFLAGS = #cԤ����������

LUA_PATH_INC = "D:\Program Files\Lua\5.1\include" 
LUA_PATH_LIB = "D:\Program Files\Lua\5.1\lib"

LIBS = -L $(LUA_PATH_LIB) -llua51
LDFLAGS := $(LDFLAGS) $(LIBS) # make������ʹ���������������������

INCLUDES = -I $(LUA_PATH_INC)   # ָ�������ⲿͷ�ļ���Ŀ¼

DEPENDFLAG = -MM             # ����������ϵ�ļ��Ĳ���

CFLAGS := $(CFLAGS) $(INCLUDES)

SRCDIRS = common login util#��Ŀ¼
SRCDIRS += .#���ϵ�ǰ��Ŀ¼

TARGET = test

SRCS = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS = $(SRCS:.c=.o)

DEPENDS = $(SRCS:.c=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(filter %.o ,$+) $(LDFLAGS)

%.d: %.c
	$(CC) $(DEPENDFLAG) $(CFLAGS) $< |\
 sed "s?\\(.*\\):?$(basename $<).o $(basename $<).d :?g" \
 > $@ || $(RM) $@

-include $(DEPENDS)

clean:
	rm $(OBJS) $(DEPENDS) $(TARGET)
