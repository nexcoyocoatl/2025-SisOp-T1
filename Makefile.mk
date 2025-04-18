# Windows

BIN = sisop_t1
SRC = main.c
DEP = # single header files
OBJ = $(SRC:.c=.o)
CFLAGS = -g -Wall -Werror
LDFLAGS = 
CC = gcc

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(DEP) -o $@ $(LDFLAGS)

clean:
	-@ del $(OBJ) $(BIN)
