# Some make command info :
#
# make (or make pflayer) : Builds the 'pflayer' relocatable object (build/pflayer.o).
# make clean             : Removes the entire 'build/' directory and any '*.db' files.
# make clear             : Runs 'make clean' and then clears the terminal.
#
# Tests
#
# make testpf            		: Builds and runs the 'testpf' executable (build/testpf).
# make testbuf           		: Builds and runs the 'testbuf' executable (build/testbuf).
# make testbuf_generategraph	: Builds and runs the 'testbuf' executable (build/testbuf) and also generate graph.
# make testhash          		: Builds and runs the 'testhash' executable (build/testhash).

CC = cc
CFLAGS = -g -Wall
INCLUDE_DIR = include

SRC_DIR = src
BUILD_DIR = build
LIB_SRC = $(SRC_DIR)/pf.c $(SRC_DIR)/pfbuf.c $(SRC_DIR)/pfhash.c
LIB_OBJ = $(BUILD_DIR)/pf.o $(BUILD_DIR)/pfbuf.o $(BUILD_DIR)/pfhash.o
TARGET = $(BUILD_DIR)/pflayer.o

.PHONY: pflayer clear clean testpf testbuf testhash testbuf_generategraph

#default make
pflayer: $(BUILD_DIR) $(TARGET)

$(TARGET): $(LIB_OBJ)
	ld -r -o $(TARGET) $(LIB_OBJ)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/pf.o: $(SRC_DIR)/pf.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/pfbuf.o: $(SRC_DIR)/pfbuf.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/pfhash.o: $(SRC_DIR)/pfhash.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clear: clean
	clear
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.db

################################### TESTS #####################################

# test_pf
TEST_SRC_PF = $(SRC_DIR)/test_pf.c
TEST_OBJ_PF = $(BUILD_DIR)/testpf.o
TEST_BIN_PF = $(BUILD_DIR)/testpf

testpf: $(BUILD_DIR) $(TEST_BIN_PF)
	$(TEST_BIN_PF)

$(TEST_BIN_PF): $(TEST_OBJ_PF) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testpf.o: $(TEST_SRC_PF)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_pfbuf
TEST_SRC_BUFFER = $(SRC_DIR)/test_pfbuf.c
TEST_OBJ_BUFFER = $(BUILD_DIR)/testbuf.o
TEST_BIN_BUFFER = $(BUILD_DIR)/testbuf

testbuf: $(BUILD_DIR) $(TEST_BIN_BUFFER)
	$(TEST_BIN_BUFFER)

$(TEST_BIN_BUFFER): $(TEST_OBJ_BUFFER) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testbuf.o: $(TEST_SRC_BUFFER)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test_pfbuf_generategraph
testbuf_generategraph: testbuf 
	python plot.py

# test_pfhash
TEST_SRC_HASH = $(SRC_DIR)/test_pfhash.c
TEST_OBJ_HASH = $(BUILD_DIR)/testhash.o
TEST_BIN_HASH = $(BUILD_DIR)/testhash

testhash: $(BUILD_DIR) $(TEST_BIN_HASH)
	$(TEST_BIN_HASH)

$(TEST_BIN_HASH): $(TEST_OBJ_HASH) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testhash.o: $(TEST_SRC_HASH)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@