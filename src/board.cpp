#include "board.h"
#include <stdio.h>
#include <stdlib.h>

// NOTE(Zach): Definition of board_type, the implementation of the ADT Board
struct board_type {
	// NOTE(Zach): The board is a 2D-array of type Token
	Token board[NUM_ROWS][NUM_COLS];
};

// NOTE(Zach): A function that terminates the entire program with an error message
static void terminate(const char *message)
{
	printf("%s\n", message);
	exit(EXIT_FAILURE);
}

// NOTE(Zach): Create an instance of Abstract Data Type Board 
Board board_create(void)
{
	Board b = (Board) malloc(sizeof(struct board_type));
	if (b == NULL)
		terminate("Error in board_create: board could not be created.");
	board_empty(b);
	return b;
}

// NOTE(Zach): Destroy (free the memory of) Board, b.
void board_destroy(Board b)
{
	free(b);
}

// NOTE(Zach): Return the value of the Board, b, at (row, col)
Token board_checkCell(Board b, int row, int col)
{
	return b->board[row][col];
}

// NOTE(Zach): Insert a token into the board, only allow dropping tokens in
// Return 0 for success and -1 for failure
int board_dropToken(Board b, Token token, int col)
{
	int row;

	if ((row = board_dropPosition(b, col)) != -1) {
		b->board[row][col] = token;
		return 0;
	}
	return -1;
}

// NOTE(Zach): Return row where a token will land when dropped into the
// Board, b.
// Return -1 for failure.
int board_dropPosition(Board b, int col)
{
	int row;

	if (b->board[0][col] != EMPTY) return -1;
	for (row = 0; row + 1 < NUM_ROWS && b->board[row + 1][col] == EMPTY; row++);
	return row;
}

// NOTE(Zach): Empty the Board, b
void board_empty(Board b)
{
	int row, col;

	for (row = 0; row < NUM_ROWS; row++)
		for (col = 0; col < NUM_COLS; col++)
			b->board[row][col] = EMPTY;

	return;
}
