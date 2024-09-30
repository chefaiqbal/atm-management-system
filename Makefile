CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRCS = src/main.c src/atm_operations.c
OBJS = $(SRCS:.c=.o)
EXEC = atm_system

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c src/header.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)