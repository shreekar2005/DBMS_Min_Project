CC = cc
CFLAGS = -g -I./include
LDFLAGS = -g

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

LIB_SRC = $(SRC_DIR)/buf.c $(SRC_DIR)/hash.c $(SRC_DIR)/pf.c $(SRC_DIR)/sp.c
LIB_OBJ = $(BUILD_DIR)/buf.o $(BUILD_DIR)/hash.o $(BUILD_DIR)/pf.o $(BUILD_DIR)/sp.o

TEST_SRC_1 = $(SRC_DIR)/testhash.c
TEST_OBJ_1 = $(BUILD_DIR)/testhash.o
TEST_BIN_1 = $(BUILD_DIR)/testhash

TEST_SRC_2 = $(SRC_DIR)/testpf.c
TEST_OBJ_2 = $(BUILD_DIR)/testpf.o
TEST_BIN_2 = $(BUILD_DIR)/testpf

TEST_SRC_3 = $(SRC_DIR)/test_sp.c
TEST_OBJ_3 = $(BUILD_DIR)/test_sp.o
TEST_BIN_3 = $(BUILD_DIR)/test_sp

HDR = $(INCLUDE_DIR)/pf.h $(INCLUDE_DIR)/pftypes.h $(INCLUDE_DIR)/sp.h

TARGET = $(BUILD_DIR)/pflayer.o

.PHONY: all clean tests

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(LIB_OBJ)
	ld -r -o $(TARGET) $(LIB_OBJ)

tests: $(TEST_BIN_1) $(TEST_BIN_2) $(TEST_BIN_3)

$(TEST_BIN_1): $(TEST_OBJ_1) $(TARGET)
	$(CC) $(LDFLAGS) -o $@ $^

$(TEST_BIN_2): $(TEST_OBJ_2) $(TARGET)
	$(CC) $(LDFLAGS) -o $@ $^

$(TEST_BIN_3): $(TEST_OBJ_3) $(TARGET)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/buf.o: $(SRC_DIR)/buf.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/hash.o: $(SRC_DIR)/hash.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pf.o: $(SRC_DIR)/pf.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sp.o: $(SRC_DIR)/sp.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/testhash.o: $(SRC_DIR)/testhash.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/testpf.o: $(SRC_DIR)/testpf.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_sp.o: $(SRC_DIR)/test_sp.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
