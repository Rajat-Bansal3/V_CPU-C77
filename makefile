CC = clang
CFLAGS = -Wall -Wextra -std=c99
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/arch.bin

SRCS = $(wildcard src/*.c src/helper/*.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
run:
	./$(TARGET)

run-file:
	@echo "Running file: $(file)"
	./build/arch.bin $(file)

.PHONY: all clean

