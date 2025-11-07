CC = cc
CFLAGS = -g -I./amlayer/header -I./pflayer/include
LDFLAGS = -g

AMLAYER_OBJ = amlayer/build/amlayer.o
PFLAYER_OBJ = pflayer/build/pflayer.o

TARGET = a.out

.PHONY: all test clean

all: $(PFLAYER_OBJ) $(AMLAYER_OBJ)

$(PFLAYER_OBJ):
	$(MAKE) -C pflayer

$(AMLAYER_OBJ):
	$(MAKE) -C amlayer

test: $(TARGET)
	./$(TARGET)

$(TARGET): main.o $(AMLAYER_OBJ) $(PFLAYER_OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) main.o $(AMLAYER_OBJ) $(PFLAYER_OBJ)

main.o: main.c amlayer/header/am.h amlayer/header/testam.h pflayer/include/pf.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	rm -f *.o $(TARGET) amlayer/testrel.* pflayer/file1 pflayer/file2
