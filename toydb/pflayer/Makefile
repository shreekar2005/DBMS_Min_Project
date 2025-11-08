CC = cc
CFLAGS = -g

SRC_DIR = src
BUILD_DIR = build

LIB_SRC = $(SRC_DIR)/buf.c $(SRC_DIR)/hash.c $(SRC_DIR)/pf.c
LIB_OBJ = $(BUILD_DIR)/buf.o $(BUILD_DIR)/hash.o $(BUILD_DIR)/pf.o

TARGET = $(BUILD_DIR)/pflayer.o

.PHONY: all clean testhash testpf

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(LIB_OBJ)
	ld -r -o $(TARGET) $(LIB_OBJ)

$(BUILD_DIR)/buf.o: $(SRC_DIR)/buf.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/hash.o: $(SRC_DIR)/hash.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pf.o: $(SRC_DIR)/pf.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)


################################### TESTS #####################################

TEST_SRC_HASH = $(SRC_DIR)/testhash.c
TEST_OBJ_HASH = $(BUILD_DIR)/testhash.o
TEST_BIN_HASH = $(BUILD_DIR)/testhash

TEST_SRC_PF = $(SRC_DIR)/testpf.c
TEST_OBJ_PF = $(BUILD_DIR)/testpf.o
TEST_BIN_PF = $(BUILD_DIR)/testpf

TEST_SRC_BUFFER = $(SRC_DIR)/testbuf.c
TEST_OBJ_BUFFER = $(BUILD_DIR)/testbuf.o
TEST_BIN_BUFFER = $(BUILD_DIR)/testbuf

testhash: $(BUILD_DIR) $(TEST_BIN_HASH)
	$(TEST_BIN_HASH)

testpf: $(BUILD_DIR) $(TEST_BIN_PF)
	$(TEST_BIN_PF)

testbuf: $(BUILD_DIR) $(TEST_BIN_BUFFER)
	$(TEST_BIN_BUFFER)

$(TEST_BIN_HASH): $(TEST_OBJ_HASH) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testhash.o: $(SRC_DIR)/testhash.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN_PF): $(TEST_OBJ_PF) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testpf.o: $(SRC_DIR)/testpf.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN_BUFFER) : $(TEST_OBJ_BUFFER) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/testbuf.o: $(SRC_DIR)/testbuf.c
	$(CC) $(CFLAGS) -c $< -o $@