CC = g++

CXXFLAGS = -Wall -g

LIBS =
LDFLAGS := $(LDFLAGS) $(LIBS)

DEPENDFLAG = -MM             # 生成依赖关系文件的参数

INCLUDES = -I ./include     # 指明包含外部头文件的目录

CXXFLAGS := $(CXXFLAGS) $(INCLUDES)

SRCDIRS = common login util
SRCDIRS += .

TARGET = main

SRCS = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cc))

OBJS = $(SRCS:.cc=.o)

DEPENDS = $(SRCS:.cc=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o ,$+)

%.d: %.cc
	$(CC) $(DEPENDFLAG) $(CXXFLAGS) $< |\
 sed "s?\\(.*\\):?$(basename $<).o $(basename $<).d :?g" \
 > $@ || $(RM) $@

-include $(DEPENDS)

clean:
	rm $(OBJS) $(DEPENDS) $(TARGET)
