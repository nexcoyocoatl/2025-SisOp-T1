# Linux

BIN = sisop_t1
SRC = main.c program.c program_linkedlist.c scheduler.c
DEP = # single header files
OBJ = $(SRC:.c=.o)
CFLAGS =
DBGFLAGS = -g -Wall -Werror
LDFLAGS = 
CC = gcc

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(DEP) $(LDFLAGS) $(DBGFLAGS) -o $(BIN)

clean:
	-@ rm -f $(OBJ) $(BIN)

proglist_test:
	$(CC) $(DBGFLAGS) proglist_test.c program_linkedlist.c program.c -o proglist_test