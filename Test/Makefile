FLAGS = -g -Wall
CC = g++

LUA_PATH_INC = "D:\Program Files\Lua\5.1\include" 
LUA_PATH_LIB = "D:\Program Files\Lua\5.1\lib"
INC = -I $(LUA_PATH_INC)
LIB = -L $(LUA_PATH_LIB) -llua51

#SRC = test.cpp mod.cpp
SRC = $(wildcard *.cpp)
#OBJ = test.o mod.o
OBJ = $(patsubst %.cpp, %.o, $(SRC))
TARGET = test


# Ĭ��Ŀ�꣬��Ϊ����Ŀ���ļ�
all: $(TARGET)

DEPENDS:$(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIB)

# ��̬ģʽ
$(OBJ): %.o : %.cpp
	$(CC) -c $(FLAGS) $< $(INC)

# ��ʾ����αĿ��
.PHONY:clean
clean:
	del $(TARGET).exe $(OBJ)