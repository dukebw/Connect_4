/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#include "board.h"
#include <stdlib.h>
#include <assert.h>

// NOTE(Zach): Definition of board_type, the implementation of the ADT Board
struct board_type {
	// NOTE(Zach): The board is a 2D-array of type Token
	Token board[NUM_ROWS][NUM_COLS];
};

// NOTE(Zach): Create an instance of Abstract Data Type Board, which is empty
Board board_create(void)
{
	Board b = (Board) malloc(sizeof(struct board_type));
	board_empty(b);
	return b;
}

// NOTE(Zach): Destroy (free the memory of) Board, b.
// NOTE(brendan): the client is responsible for setting dangling pointers to
// null.
void board_destroy(Board b)
{
	// don't need to check if b is NULL here
	// free can accept NULL pointers too
	free(b);
}

// NOTE(Zach): Return the value of the Board, b, at (row, col)
Token board_checkCell(Board b, int row, int col)
{
	assert(b != NULL);
	if (b == NULL) return EMPTY;
	return b->board[row][col];
}

// NOTE(Zach): Insert a token into the board, only allow dropping tokens in
// Return 0 for success and -1 for failure
int board_dropToken(Board b, Token token, int col) {
	assert(0 <= col && col < NUM_COLS);
	if (b == NULL) return -1;
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
	assert(0 <= col && col < NUM_COLS);
	if (b == NULL) return -1;
	int row;

	if (b->board[0][col] != EMPTY) return -1;
	for (row = 0; row + 1 < NUM_ROWS && b->board[row + 1][col] == EMPTY; row++);
	return row;
}

// NOTE(Zach): Empty the Board, b
void board_empty(Board b)
{
	if (b == NULL) return;
	int row, col;

	for (row = 0; row < NUM_ROWS; row++)
		for (col = 0; col < NUM_COLS; col++)
			b->board[row][col] = EMPTY;

	return;
}

// NOTE(brendan): load the board from a previously saved game
void board_load(Board b, FILE *in_file) {
  if (in_file == 0) {
    printf("Invalid file pointer in board_load function\n");
  }
  else {
    fread(b, sizeof(*b), 1, in_file);
  }
}

// NOTE(brendan): save the board
void board_save(Board b, FILE *out_file) {
  if (out_file == 0) {
    printf("Invalid file pointer in board_save function\n");
  }
  else {
    fwrite(b, sizeof(*b), 1, out_file);
  }
}

// NOTE(Zach): switch the token
void switchToken(Token *token) {
  if (*token == RED) {
    *token = BLUE;
  } else {
    *token = RED;
  }
}
