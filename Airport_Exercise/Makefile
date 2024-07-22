CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wvla -Winline -Wfloat-equal -Wstack-usage=1024 -std=c99
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
FEATURES := -D _DEFAULT_SOURCE
STANDARD := -std=c99

# Define the Check library flags
CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LDFLAGS = $(shell pkg-config --libs check)
CLANG_FORMAT = clang-format
CLANG_FORMAT_FLAGS = -i --style=file

# Define the source files
SRC = Airport.c ../Hash_Table/hash_table.c ../Linked_List/linked_list.c
OBJ = $(SRC:.c=.o)
DEPS = ../Hash_Table/hash_table.h ../Linked_List/linked_list.h

# Define the executable name
TARGET = airport

# Rule to build the target
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CHECK_LDFLAGS) 

# Rule to build object files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) $(FEATURES) $(STANDARD)  -c -o $@ $<

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
	clang-tidy Airport.c -- $(CFLAGS) $(CHECK_CFLAGS) $(STANDARD) $(FEATURES)


format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) $(SRC)

# Add debug flags and rebuild
.PHONY: build-debug
build-debug: CFLAGS += -g3
build-debug: $(TARGET)