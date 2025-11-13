# Some make command info :
#
# make (or make amlayer) : Builds the 'amlayer' relocatable object (build/amlayer.o).
# make clean           : Removes the entire 'build/' directory.
# make clear           : Runs 'make clean' and then clears the terminal.
#
# Tests
#
# make test1           : Builds and runs the 'test1' executable (build/test1).
# make test2           : Builds and runs the 'test2' executable (build/test2).
# make test3           : Builds and runs the 'test3' executable (build/test3).

CC = cc
CFLAGS = -g -Wall
INCLUDE_DIR = include

SRC_DIR = src
BUILD_DIR = build
LIB_SRC = $(SRC_DIR)/am.c $(SRC_DIR)/amfns.c $(SRC_DIR)/amsearch.c \
          $(SRC_DIR)/aminsert.c $(SRC_DIR)/amstack.c $(SRC_DIR)/amglobals.c \
          $(SRC_DIR)/amscan.c $(SRC_DIR)/amprint.c $(SRC_DIR)/misc.c
LIB_OBJ = $(BUILD_DIR)/am.o $(BUILD_DIR)/amfns.o $(BUILD_DIR)/amsearch.o \
          $(BUILD_DIR)/aminsert.o $(BUILD_DIR)/amstack.o $(BUILD_DIR)/amglobals.o \
          $(BUILD_DIR)/amscan.o $(BUILD_DIR)/amprint.o $(BUILD_DIR)/misc.o
TARGET = $(BUILD_DIR)/amlayer.o
PF_LAYER_OBJ = ../pflayer/build/pflayer.o

.PHONY: amlayer clear clean test1 test2 test3

#default make
amlayer: $(BUILD_DIR) $(TARGET)

$(TARGET): $(LIB_OBJ)
	ld -r -o $(TARGET) $(LIB_OBJ)

$(PF_LAYER_OBJ):
	$(MAKE) -C ../pflayer

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/am.o: $(SRC_DIR)/am.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amfns.o: $(SRC_DIR)/amfns.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amsearch.o: $(SRC_DIR)/amsearch.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/aminsert.o: $(SRC_DIR)/aminsert.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amstack.o: $(SRC_DIR)/amstack.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amglobals.o: $(SRC_DIR)/amglobals.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amscan.o: $(SRC_DIR)/amscan.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/amprint.o: $(SRC_DIR)/amprint.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/misc.o: $(SRC_DIR)/misc.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clear: clean
	clear
clean:
	rm -rf $(BUILD_DIR)

################################### TESTS #####################################

# test1
TEST_SRC_1 = $(SRC_DIR)/test1.c
TEST_OBJ_1 = $(BUILD_DIR)/test1.o
TEST_BIN_1 = $(BUILD_DIR)/test1

test1: $(BUILD_DIR) $(TEST_BIN_1)
	$(TEST_BIN_1)

$(TEST_BIN_1): $(TEST_OBJ_1) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test1.o: $(TEST_SRC_1)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test2
TEST_SRC_2 = $(SRC_DIR)/test2.c
TEST_OBJ_2 = $(BUILD_DIR)/test2.o
TEST_BIN_2 = $(BUILD_DIR)/test2

test2: $(BUILD_DIR) $(TEST_BIN_2)
	$(TEST_BIN_2)

$(TEST_BIN_2): $(TEST_OBJ_2) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test2.o: $(TEST_SRC_2)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# test3
TEST_SRC_3 = $(SRC_DIR)/test3.c
TEST_OBJ_3 = $(BUILD_DIR)/test3.o
TEST_BIN_3 = $(BUILD_DIR)/test3

test3: $(BUILD_DIR) $(TEST_BIN_3)
	$(TEST_BIN_3)

$(TEST_BIN_3): $(TEST_OBJ_3) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test3.o: $(TEST_SRC_3)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@