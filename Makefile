CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lsqlite3
SRC_DIR = src
OBJ_DIR = obj
BIN = atm_system

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/menu.c \
       $(SRC_DIR)/auth.c \
       $(SRC_DIR)/database_operations.c \
       $(SRC_DIR)/atm_operations.c \
       $(SRC_DIR)/system.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link all object files to create the executable
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file to .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/header.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN)

# Phony targets
.PHONY: all clean