#include "board.h"

// NOTE(Zach): The board is a 2D-array of type Token
Token board[NUM_ROWS][NUM_COLS] = {{EMPTY}};

// NOTE(Zach): Return the value of the board at (row, col)
Token checkCell(int row, int col)
{
	return board[row][col];
}

// TODO(Zach): Find the best place to add macros for EXIT_FAILURE and EXIT_SUCCESS
// NOTE(Zach): Insert a token into the board, only allow dropping tokens in
// Return 0 for success and -1 for failure
int insertToken(Token token, int col)
{
	int row;

	if ((row = insertPosition(col)) != -1) {
		board[row][col] = token;
		return 0;
	}
	return -1;
}

// NOTE(Zach): Return row where a token will land when dropped into the board.
// Return -1 for failure.
int insertPosition(int col)
{
	int row;

	if (board[0][col] != EMPTY) return -1;	
	for (row = 0; row + 1 < NUM_ROWS && board[row + 1][col] == EMPTY; row++);
	return row;
}

// NOTE(Zach): Empty the board
void emptyBoard(void)
{
	int row, col;

	for (row = 0; row < NUM_ROWS; row++)
		for (col = 0; col < NUM_COLS; col++)
			board[row][col] = EMPTY;

	return;
}
