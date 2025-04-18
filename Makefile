# Linux

BIN = sisop_t1
SRC = main.c program.c
DEP = # single header files
OBJ = $(SRC:.c=.o)
CFLAGS = -g -Wall -Werror
LDFLAGS = 
CC = gcc

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(DEP) $(LDFLAGS) -o $(BIN)

clean:
	-@ rm -f $(OBJ) $(BIN)
