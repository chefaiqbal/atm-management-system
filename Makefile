# Define source files and objects
SRC_DIR = src
OBJ_DIR = obj
EXECUTABLE = atm
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Phony target for cleaning up
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
