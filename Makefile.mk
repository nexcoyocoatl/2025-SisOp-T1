# Windows

BIN = sisop_t1
SRC = main.c program.c program_linkedlist.c scheduler.c
DEP = # single header files
OBJ = $(SRC:.c=.o)
CFLAGS =
DBGFLAGS = -g -Wall -Werror
LDFLAGS = 
CC = gcc

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(DEP) -o $@ $(LDFLAGS)

clean:
	-@ del $(OBJ) $(BIN)