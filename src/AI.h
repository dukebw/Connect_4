#ifndef AI_H
#define AI_H

#include "graphics.h"
#include "board.h"

// Note(Zach): Given a board and a token colour this function will modify
// what value pointed to by row and col to be the row and column for the
// AI's move
void AI_move(Board b, Token colour, int *row, int *col);




#endif  // AI_H
