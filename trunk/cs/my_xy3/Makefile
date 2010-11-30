CC = g++
FLAG = -g
LIB = -levent -llua5.1
INC = ./

TARGET1 = main_c
TARGET2 = main_s

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

OBJ0 = server.o client.o databuf.o arg.o op_epoll.o
OBJ1 = proto_s.o 
OBJ2 = proto_c.o

all:$(TARGET1) $(TARGET2)

DEPENDS: $(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET2):$(OBJ0) $(OBJ1) main_s.o
	$(CC) -o $(TARGET2) main_s.o $(OBJ0) $(OBJ1) $(LIB)

$(TARGET1):$(OBJ0) $(OBJ2) main_c.o
	$(CC) -o $(TARGET1) main_c.o $(OBJ0) $(OBJ2) $(LIB)

$(OBJ): %.o : %.cpp
	$(CC) -c $< $(FLAG) $(INC)

.PHONY:clean
clean:
	rm *.o
