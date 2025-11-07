CC = cc
CFLAGS = -g -I./header -I../pflayer/include
LDFLAGS = -g

SRC_DIR = src
BUILD_DIR = build
HEADER_DIR = header

OBJS = $(BUILD_DIR)/am.o $(BUILD_DIR)/amfns.o $(BUILD_DIR)/amsearch.o $(BUILD_DIR)/aminsert.o $(BUILD_DIR)/amstack.o $(BUILD_DIR)/amglobals.o $(BUILD_DIR)/amscan.o $(BUILD_DIR)/amprint.o $(BUILD_DIR)/misc.o

HDR = $(HEADER_DIR)/am.h $(HEADER_DIR)/testam.h

TARGET = $(BUILD_DIR)/amlayer.o

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJS)
	ld -r -o $(TARGET) $(OBJS)

$(BUILD_DIR)/am.o: $(SRC_DIR)/am.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amfns.o: $(SRC_DIR)/amfns.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amsearch.o: $(SRC_DIR)/amsearch.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/aminsert.o: $(SRC_DIR)/aminsert.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amscan.o: $(SRC_DIR)/amscan.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amstack.o: $(SRC_DIR)/amstack.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amglobals.o: $(SRC_DIR)/amglobals.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/amprint.o: $(SRC_DIR)/amprint.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/misc.o: $(SRC_DIR)/misc.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
