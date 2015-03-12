/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#ifndef BOARD_H // NOTE(Zach): This is an include guard
#define BOARD_H

#define NUM_ROWS 6
#define NUM_COLS 7

// NOTE(Zach): Create a new type called Token
typedef enum {EMPTY, RED, BLUE, RANDOMTOKEN} Token;

// NOTE(Zach): Board is a pointer to the incomplete type board_type
typedef struct board_type *Board;

// NOTE(Zach): Create an instance of Abstract Data Type board, which is empty 
Board board_create(void);

// NOTE(Zach): Destroy (free the memory of) Board, b
void board_destroy(Board b);

// NOTE(Zach): Return the value of the Board, b, at (row, col)
Token board_checkCell(Board b, int row, int col);

// NOTE(Zach): Drop a token into the Board, b
// Return 0 for success and -1 for failure
int board_dropToken(Board b, Token token, int col);

// NOTE(Zach): Return row where a token will land when dropped into the
// Board, b.
// Return -1 for failure.
int board_dropPosition(Board b, int col);

// NOTE(Zach): Empty the Board, b
void board_empty(Board b);

// NOTE(brendan): load the board from a previously saved game
void board_load(Board b);

// NOTE(brendan): save the board
void board_save(Board b);

#endif // BOARD_H
