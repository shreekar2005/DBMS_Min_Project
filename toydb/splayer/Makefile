# Some make command info :
#
# make (or make splayer) : Builds the 'splayer' relocatable object (build/splayer.o).
# make clean             : Removes the entire 'build/' directory and 'testdata.tdb'.
# make clear             : Runs 'make clean' and then clears the terminal.
#
# Tests
#
# make testsp            : Builds and runs the 'testsp' executable (build/testsp).
# make testconvert       : Builds and runs 'testconvert' to create 'testdata.tdb' from 'testdata.txt'.
# make testscan          : Builds and runs 'testscan' on 'testdata.tdb'.
# make testfind          : Builds and runs 'testfind' to search for a record in 'testdata.tdb'.
# make testdelete        : Builds and runs 'testdelete' to delete a record from 'testdata.tdb'.
# make testanalyze       : Builds and runs the 'testanalyze' executable on 'testdata.tdb'.


CC = cc
CFLAGS = -g -Wall
INCLUDE_DIR = include

SRC_DIR = src
BUILD_DIR = build
LIB_SRC = $(SRC_DIR)/sp.c $(SRC_DIR)/spconvert.c $(SRC_DIR)/spscan.c $(SRC_DIR)/spfind.c $(SRC_DIR)/spdelete.c $(SRC_DIR)/spanalyze.c
LIB_OBJ = $(BUILD_DIR)/sp.o $(BUILD_DIR)/spconvert.o $(BUILD_DIR)/spscan.o $(BUILD_DIR)/spfind.o $(BUILD_DIR)/spdelete.o $(BUILD_DIR)/spanalyze.o
TARGET = $(BUILD_DIR)/splayer.o
PFLAYER_LIB = ../pflayer/build/pflayer.o

.PHONY: splayer clear clean testsp testconvert testscan testfind testdelete testanalyze

#default make
splayer: $(BUILD_DIR) $(TARGET)

$(TARGET): $(LIB_OBJ)
	ld -r -o $(TARGET) $(LIB_OBJ)

$(PFLAYER_LIB):
	$(MAKE) -C ../pflayer

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/sp.o: $(SRC_DIR)/sp.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/spconvert.o: $(SRC_DIR)/spconvert.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/spscan.o: $(SRC_DIR)/spscan.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/spfind.o: $(SRC_DIR)/spfind.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/spdelete.o: $(SRC_DIR)/spdelete.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/spanalyze.o: $(SRC_DIR)/spanalyze.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clear: clean
	clear
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.tdb

#Needed by some tests (e.g. delete, scan, find)
testdata.tdb: 
	$(MAKE) testconvert

################################### TESTS #####################################

# test_sp
TEST_SRC_SP = $(SRC_DIR)/test_sp.c
TEST_OBJ_SP = $(BUILD_DIR)/testsp.o
TEST_BIN_SP = $(BUILD_DIR)/testsp

testsp: $(BUILD_DIR) $(TEST_BIN_SP)
	$(TEST_BIN_SP)

$(TEST_BIN_SP): $(TEST_OBJ_SP) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testsp.o: $(TEST_SRC_SP)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_spconvert
TEST_SRC_CONVERT = $(SRC_DIR)/test_spconvert.c
TEST_OBJ_CONVERT = $(BUILD_DIR)/testconvert.o
TEST_BIN_CONVERT = $(BUILD_DIR)/testconvert

testconvert: $(BUILD_DIR) $(TEST_BIN_CONVERT) testdata.txt
	$(TEST_BIN_CONVERT) testdata.txt 20 0

$(TEST_BIN_CONVERT): $(TEST_OBJ_CONVERT) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testconvert.o: $(TEST_SRC_CONVERT)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_spscan
TEST_SRC_SCAN = $(SRC_DIR)/test_spscan.c
TEST_OBJ_SCAN = $(BUILD_DIR)/testscan.o
TEST_BIN_SCAN = $(BUILD_DIR)/testscan

testscan: $(BUILD_DIR) $(TEST_BIN_SCAN) testdata.tdb
	$(TEST_BIN_SCAN) testdata.tdb 20 0

$(TEST_BIN_SCAN): $(TEST_OBJ_SCAN) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testscan.o: $(TEST_SRC_SCAN)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_spfind
TEST_SRC_FIND = $(SRC_DIR)/test_spfind.c
TEST_OBJ_FIND = $(BUILD_DIR)/testfind.o
TEST_BIN_FIND = $(BUILD_DIR)/testfind

testfind: $(BUILD_DIR) $(TEST_BIN_FIND) testdata.tdb
	$(TEST_BIN_FIND) 20 0

$(TEST_BIN_FIND): $(TEST_OBJ_FIND) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testfind.o: $(TEST_SRC_FIND)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_spdelete
TEST_SRC_DELETE = $(SRC_DIR)/test_spdelete.c
TEST_OBJ_DELETE = $(BUILD_DIR)/testdelete.o
TEST_BIN_DELETE = $(BUILD_DIR)/testdelete

testdelete: $(BUILD_DIR) $(TEST_BIN_DELETE) testdata.tdb
	$(TEST_BIN_DELETE) 20 0

$(TEST_BIN_DELETE): $(TEST_OBJ_DELETE) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testdelete.o: $(TEST_SRC_DELETE)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_spanalyze
TEST_SRC_ANALYZE = $(SRC_DIR)/test_spanalyze.c
TEST_OBJ_ANALYZE = $(BUILD_DIR)/test_spanalyze.o
TEST_BIN_ANALYZE = $(BUILD_DIR)/testanalyze

testanalyze: $(BUILD_DIR) $(TEST_BIN_ANALYZE) testdata.tdb
	$(TEST_BIN_ANALYZE) 20 0

$(TEST_BIN_ANALYZE): $(TEST_OBJ_ANALYZE) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_spanalyze.o: $(TEST_SRC_ANALYZE)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@