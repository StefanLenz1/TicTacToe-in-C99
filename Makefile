CC=gcc
CFLAGS= -std=c99 -Wall -g -w -Werror -O2

all: tictactoe

tictactoe: tictactoe.c
	$(CC) tictactoe.c -o tictactoe $(CFLAGS)

clean: 
	$(RM) tictactoe.c