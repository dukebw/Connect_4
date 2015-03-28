#include "AI.h"

static void boardToArray(Board b, Token arr[][NUM_COLS]);

// Note(Zach): Given a board and a token colour this function will modify
// what value pointed to by row and col to be the row and column for the
// AI's move
void AI_move(Board b, Token colour, int *row, int *col)
{
	Token arr[NUM_ROWS][NUM_COLS];
	boardToArray(b, arr);
}

// NOTE(Zach): Read the board into a two dimensional array
static void boardToArray(Board b, Token arr[][NUM_COLS]){
	int row, col;

	for (row = 0; row < NUM_ROWS; row++) {
		for (col = 0; col < NUM_COLS; col++) {
			arr[row][col] = board_checkCell(b, row, col);
		}
	}
}
