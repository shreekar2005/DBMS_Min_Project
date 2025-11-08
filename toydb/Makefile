CC = cc
CFLAGS = -g

AMLAYER_OBJ = amlayer/build/amlayer.o
PFLAYER_OBJ = pflayer/build/pflayer.o

TARGET = toydb.out

.PHONY: run clear clean

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

run: $(TARGET)
	./$(TARGET)

am_test1:
	$(MAKE) -C amlayer test1
am_test2:
	$(MAKE) -C amlayer test2
am_test3:
	$(MAKE) -C amlayer test3

pf_testpf:
	$(MAKE) -C pflayer testpf
pf_testhash:
	$(MAKE) -C pflayer testhash
pf_testbuffer:
	$(MAKE) -C pflayer testbuffer

sp_testsp:
	$(MAKE) -C splayer testsp

clear: clean
	clear
clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	$(MAKE) -C splayer clean
	rm -f $(TARGET)
