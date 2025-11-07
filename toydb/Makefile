CC = cc
CFLAGS = -g

AMLAYER_OBJ = amlayer/build/amlayer.o
PFLAYER_OBJ = pflayer/build/pflayer.o

TARGET = toydb.out

.PHONY: run clear clean

$(TARGET): main.c $(AMLAYER_OBJ) $(PFLAYER_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(AMLAYER_OBJ) $(PFLAYER_OBJ)

run: $(TARGET)
	./$(TARGET)

$(PFLAYER_OBJ):
	$(MAKE) -C pflayer

$(AMLAYER_OBJ):
	$(MAKE) -C amlayer

clear: clean
	clear
clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	$(MAKE) -C splayer clean
	rm -f $(TARGET)
