# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wvla -Winline -Wfloat-equal -Wstack-usage=1024 -std=c99

# Define the Check library flags
CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LDFLAGS = $(shell pkg-config --libs check)
CLANG_FORMAT = clang-format
CLANG_FORMAT_FLAGS = -i --style=file

# Define the source files
SRC = bst.c bst_unit_test.c
OBJ = $(SRC:.c=.o)
DEPS = bst.h

# Define the executable name
TARGET = bst_test

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

format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) $(SRC) 