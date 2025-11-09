CC = cc
CFLAGS = -g

SRC_DIR = src
BUILD_DIR = build

OBJS = $(BUILD_DIR)/sp.o
TARGET = $(BUILD_DIR)/splayer.o

PFLAYER_LIB = ../pflayer/build/pflayer.o

.PHONY: all clean testsp

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJS)
	ld -r -o $(TARGET) $(OBJS)

$(PFLAYER_LIB):
	$(MAKE) -C ../pflayer

$(BUILD_DIR)/sp.o: $(SRC_DIR)/sp.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)


################################### TESTS #####################################

TEST_SRC_SP = $(SRC_DIR)/test_sp.c
TEST_OBJ_SP = $(BUILD_DIR)/testsp.o
TEST_BIN_SP = $(BUILD_DIR)/testsp

testsp: $(BUILD_DIR) $(TEST_BIN_SP)
	$(TEST_BIN_SP)

$(TEST_BIN_SP): $(TEST_OBJ_SP) $(TARGET) $(PFLAYER_LIB)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ_SP) $(TARGET) $(PFLAYER_LIB)

$(TEST_OBJ_SP): $(TEST_SRC_SP)
	$(CC) $(CFLAGS) -c $< -o $@