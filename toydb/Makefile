CC = cc
CFLAGS = -g

AMLAYER_OBJ = amlayer/build/amlayer.o
PFLAYER_OBJ = pflayer/build/pflayer.o

TARGET = toydb.out

.PHONY: run clean

test: $(TARGET)
	./$(TARGET)

$(PFLAYER_OBJ):
	$(MAKE) -C pflayer

$(AMLAYER_OBJ):
	$(MAKE) -C amlayer

$(TARGET): main.c $(AMLAYER_OBJ) $(PFLAYER_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(AMLAYER_OBJ) $(PFLAYER_OBJ)

clear: clean
	clear
clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	rm -f $(TARGET)
