FLAGS = -g -Wall
CC = gcc

LIB = -llua5.1 -L . -lposix

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
TARGET = stat


# 默认目标，作为最终目标文件
all: $(TARGET)

DEPENDS:$(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIB)

# 静态模式
$(OBJ): %.o : %.c
	$(CC) -c $(FLAGS) $< $(INC)
