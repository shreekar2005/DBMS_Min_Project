CC = cc
CFLAGS = -g

AMLAYER_DIR = amlayer
PFLAYER_DIR = pflayer

AMLAYER_OBJ = $(AMLAYER_DIR)/build/amlayer.o
PFLAYER_OBJ = $(PFLAYER_DIR)/build/pflayer.o

TARGET = toydb.out

.PHONY: all run clear clean testall
.PHONY: am_test1 am_test2 am_test3
.PHONY: pf_testpf pf_testhash pf_testbuf
.PHONY: sp_testsp

# Default rule: build the main executable
all: $(TARGET)

#
# -------------------- MAIN EXECUTABLE RULES --------------------
#

# This rule links your main.c with the .o files from other layers
$(TARGET): main.c $(AMLAYER_OBJ) $(PFLAYER_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $^

# Rule to build the AM Layer (it depends on the PF layer)
$(AMLAYER_OBJ): $(PFLAYER_OBJ)
	$(MAKE) -C $(AMLAYER_DIR) all

# Rule to build the PF Layer
$(PFLAYER_OBJ):
	$(MAKE) -C $(PFLAYER_DIR) all

# 'run' now depends on 'all' to ensure the target is built
run: all
	./$(TARGET)

#
# -------------------- TEST RULES --------------------
#

testall: pf_testhash pf_testpf pf_testbuf am_test1 am_test2 am_test3 sp_testsp

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
pf_testbuf:
	$(MAKE) -C pflayer testbuf

sp_testsp:
	$(MAKE) -C splayer testsp

#
# -------------------- CLEANUP RULES --------------------
#

clear: clean
	clear
clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	$(MAKE) -C splayer clean
	rm -f $(TARGET)