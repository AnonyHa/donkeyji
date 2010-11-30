CC = gcc
FLAG = -g
INC = -I  ../
LIB = -L ../ -ltimer -llua5.1

TARGET = main

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

all:$(TARGET)

DEPENDS: $(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET):$(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIB)

$(OBJ): %.o : %.c
	$(CC) -c $< $(FLAG) $(INC)

.PHONY:clean
clean:
	rm *.o
