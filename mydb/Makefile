CC = g++
FLAG = -g
INC = ./
LIBS = -llua5.1 -lmysqlclient -levent 

TARGET = main

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

all:$(TARGET)

DEPENDS: $(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET):$(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

$(OBJ): %.o : %.c
	$(CC) -c $< $(FLAG) $(INC)

.PHONY:clean
clean:
	rm *.o
