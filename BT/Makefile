CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wvla -Winline -Wfloat-equal -Wstack-usage=1024 -std=c99
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
FEATURES := -D_POSIX_C_SOURCE=200809L
STANDARD := -std=c99

# Define the Check library flags
CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LDFLAGS = $(shell pkg-config --libs check)
CLANG_FORMAT = clang-format
CLANG_FORMAT_FLAGS = -i --style=file

# Define the source files
SRC = bt.c bt_unit_test.c
OBJ = $(SRC:.c=.o)
DEPS = bt.h

# Define the executable name
TARGET = bt_test

# Rule to build the target
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CHECK_LDFLAGS)

# Rule to build object files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c -o $@ $<

# Rule to clean up the build
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: valgrind
valgrind: $(TARGET)
	valgrind $(VFLAGS) ./$(TARGET)

.PHONY: debug
debug: $(TARGET)
	gdb ./$(TARGET)

tidy:
	clang-tidy bt.c -- $(CFLAGS) $(CHECK_CFLAGS) $(STANDARD) $(FEATURES)


format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) $(SRC)

# Add debug flags and rebuild
.PHONY: build-debug
build-debug: CFLAGS += -g3
build-debug: $(TARGET)