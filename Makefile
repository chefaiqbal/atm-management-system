CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lsqlite3 -lssl -lcrypto
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
BIN = $(BIN_DIR)/atm_system

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link all object files to create the executable
$(BIN): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file to .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/header.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up object files and the executable, but keep the data directory
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean