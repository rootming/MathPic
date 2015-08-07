CC = gcc
CFLAGS = -lpthread -lm -Wall -std=c99
OBJ = main.o
TARGET = MathPic

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

.PRONY:clean  
clean:
	rm -f $(TARGET) $(OBJ)
