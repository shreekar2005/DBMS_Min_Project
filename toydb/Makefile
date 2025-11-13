# Some make command info :
#
# make (or make toydb) : Builds the main 'toydb.out' executable by building all layers.
# make run             : Builds and then runs 'toydb.out'.
# make clean           : Removes 'toydb.out' and all build artifacts from all layers.
# make clear           : Runs 'make clean' and then clears the terminal.
#
# Tests
#
# make testall         : Runs all tests from all layers (am, pf, and sp).
#
# make am_test1        : Runs 'test1' from the 'amlayer' directory.
# make am_test2        : Runs 'test2' from the 'amlayer' directory.
# make am_test3        : Runs 'test3' from the 'amlayer' directory.
#
# make pf_testpf       : Runs 'testpf' from the 'pflayer' directory.
# make pf_testhash     : Runs 'testhash' from the 'pflayer' directory.
# make pf_testbuf      : Runs 'testbuf' from the 'pflayer' directory.
#
# make sp_testsp       : Runs 'testsp' from the 'splayer' directory.
# make sp_testconvert  : Runs 'testconvert' from the 'splayer' directory.
# make sp_testfind     : Runs 'testfind' from the 'splayer' directory.
# make sp_testdelete   : Runs 'testdelete' from the 'splayer' directory.
# make sp_testscan     : Runs 'testscan' from the 'splayer' directory.
# make sp_testanalyze     : Runs 'testanalyze' from the 'splayer' directory.

CC = cc
CFLAGS = -g

AMLAYER_DIR = amlayer
SPLAYER_DIR = splayer
PFLAYER_DIR = pflayer

AMLAYER_OBJ = $(AMLAYER_DIR)/build/amlayer.o
SPLAYER_OBJ = $(SPLAYER_DIR)/build/splayer.o
PFLAYER_OBJ = $(PFLAYER_DIR)/build/pflayer.o

TARGET = toydb.out

.PHONY: toydb run clear clean testall
.PHONY: am_test1 am_test2 am_test3
.PHONY: pf_testpf pf_testhash pf_testbuf
.PHONY: sp_testsp sp_testconvert sp_testfind sp_testdelete sp_testscan

#default make
toydb: $(TARGET)

run: toydb
	./$(TARGET)

# build toydb.out (executable) with main.c
$(TARGET): main.c $(AMLAYER_OBJ) $(SPLAYER_OBJ) $(PFLAYER_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $^

$(AMLAYER_OBJ):
	$(MAKE) -C $(AMLAYER_DIR) amlayer

$(SPLAYER_OBJ):
	$(MAKE) -C $(SPLAYER_DIR) splayer

$(PFLAYER_OBJ):
	$(MAKE) -C $(PFLAYER_DIR) pflayer

clear: clean
	clear
clean:
	$(MAKE) -C pflayer clean
	$(MAKE) -C amlayer clean
	$(MAKE) -C splayer clean
	rm -f $(TARGET)

################################### TESTS #####################################

testall: pf_testpf pf_testhash pf_testbuf sp_testsp sp_testconvert sp_testfind sp_testdelete sp_testscan sp_testanalyze am_test1 am_test2 am_test3 

pf_testpf:
	$(MAKE) -C pflayer testpf
pf_testhash:
	$(MAKE) -C pflayer testhash
pf_testbuf:
	$(MAKE) -C pflayer testbuf

sp_testsp:
	$(MAKE) -C splayer testsp
sp_testconvert:
	$(MAKE) -C splayer testconvert
sp_testfind:
	$(MAKE) -C splayer testfind
sp_testinsert:
	$(MAKE) -C splayer testinsert
sp_testdelete:
	$(MAKE) -C splayer testdelete
sp_testscan:
	$(MAKE) -C splayer testscan
sp_testanalyze:
	$(MAKE) -C splayer testanalyze

am_test1:
	$(MAKE) -C amlayer test1
am_test2:
	$(MAKE) -C amlayer test2
am_test3:
	$(MAKE) -C amlayer test3
