CC = g++
FLAG = -g
INC = -I ./
LIBS = -llua5.1 -levent -lrt

TARGET = sm

SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))

all:$(TARGET)

DEPENDS: $(SRC)
	$(CC) -M $(SRC) $(INC) > DEPENDS
include DEPENDS

$(TARGET):$(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

$(OBJ): %.o : %.cc
	$(CC) -c $< $(FLAG) $(INC)

.PHONY:clean
clean:
	rm *.o
