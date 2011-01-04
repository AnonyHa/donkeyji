CC = g++
CFLAGS = -g -Wall
INC = -I ./
LIBS = -llua5.1 -levent -lrt


SUBDIR = login util common
TOPSRC = $(wildcard *.cc)
TOPOBJ = $(TOPSRC: %.cc=%.o)
SUBSRC = $(shell find $(SUBDIR) -name '*.cc')
SUBOBJ = $(SUBSRC: %.cc = %.o)

TARGET = sm

all:$(TARGET)

$(TARGET):$(TOPOBJ) $(SUBOBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(TOPOBJ) $(SUBOBJ) $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) -c $(@D)/$(<F) -o $(@D)/$(@F)

clean:
	rm $(TARGET) $(TOPOBJ) $(SUBOBJ)
