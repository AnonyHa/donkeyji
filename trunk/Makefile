CC = gcc

CFLAGS = -Wall -g

LUA_PATH_INC = "D:\Program Files\Lua\5.1\include" 
LUA_PATH_LIB = "D:\Program Files\Lua\5.1\lib"

LIBS = -L $(LUA_PATH_LIB) -llua51
LDFLAGS := $(LDFLAGS) $(LIBS)

INCLUDES = -I $(LUA_PATH_INC)   # 指明包含外部头文件的目录

DEPENDFLAG = -MM             # 生成依赖关系文件的参数

CFLAGS := $(CFLAGS) $(INCLUDES)

SRCDIRS = common login util#子目录
SRCDIRS += .#加上当前根目录

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
