CC = cc
CFLAGS = -g

SRC_DIR = src
BUILD_DIR = build

OBJS = $(BUILD_DIR)/am.o $(BUILD_DIR)/amfns.o $(BUILD_DIR)/amsearch.o $(BUILD_DIR)/aminsert.o $(BUILD_DIR)/amstack.o $(BUILD_DIR)/amglobals.o $(BUILD_DIR)/amscan.o $(BUILD_DIR)/amprint.o $(BUILD_DIR)/misc.o

TARGET = $(BUILD_DIR)/amlayer.o

.PHONY: all clean test1 test2 test3

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJS)
	ld -r -o $(TARGET) $(OBJS)

$(BUILD_DIR)/am.o: $(SRC_DIR)/am.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amfns.o: $(SRC_DIR)/amfns.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amsearch.o: $(SRC_DIR)/amsearch.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/aminsert.o: $(SRC_DIR)/aminsert.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amscan.o: $(SRC_DIR)/amscan.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amstack.o: $(SRC_DIR)/amstack.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amglobals.o: $(SRC_DIR)/amglobals.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amprint.o: $(SRC_DIR)/amprint.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/misc.o: $(SRC_DIR)/misc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)


################################### TESTS #####################################

PF_LAYER_OBJ = ../pflayer/build/pflayer.o

$(PF_LAYER_OBJ):
	$(MAKE) -C ../pflayer

TEST_SRC_1 = $(SRC_DIR)/test1.c
TEST_OBJ_1 = $(BUILD_DIR)/test1.o
TEST_BIN_1 = $(BUILD_DIR)/test1

TEST_SRC_2 = $(SRC_DIR)/test2.c
TEST_OBJ_2 = $(BUILD_DIR)/test2.o
TEST_BIN_2 = $(BUILD_DIR)/test2

TEST_SRC_3 = $(SRC_DIR)/test3.c
TEST_OBJ_3 = $(BUILD_DIR)/test3.o
TEST_BIN_3 = $(BUILD_DIR)/test3

test1: $(BUILD_DIR) $(TEST_BIN_1)
	$(TEST_BIN_1)

test2: $(BUILD_DIR) $(TEST_BIN_2)
	$(TEST_BIN_2)

test3: $(BUILD_DIR) $(TEST_BIN_3)
	$(TEST_BIN_3)

$(TEST_BIN_1): $(TEST_OBJ_1) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_OBJ_1): $(TEST_SRC_1)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN_2): $(TEST_OBJ_2) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_OBJ_2): $(TEST_SRC_2)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN_3): $(TEST_OBJ_3) $(TARGET) $(PF_LAYER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_OBJ_3): $(TEST_SRC_3)
	$(CC) $(CFLAGS) -c $< -o $@
