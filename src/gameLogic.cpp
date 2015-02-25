#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameLogic.h"
#include "graphics.h"
#include "linkedList.h"

// NOTE(Zach): Determine next MenuState based on where the user clicked
// NOTE(Jean): Values fixed for the new modified and re-scaled image
MenuState handleMainMenuMouseClick(int x, int y) {

  //if (x >= 405 && y >= 455 && x <= 511 && y <= 490) return ONEPLAYER; 
  //if (x >= 530 && y>= 455 && x <= 642 && y <= 490) return TWOPLAYER; 
  if (x >= 430 && y>= 545 && x <= 602 && y <= 610) return SETUP; 
  if (x >= 452 && y>=700 && x <= 575 && y <= 760) return QUIT;
  //if (x >= 890 && y>= 840 && x <= 972 && y <= 868) return CREDITS; 

  return MAINMENU;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}

static void switchPlayer(Player *player) {
	if (*player == PLAYERONE) {
		*player = PLAYERTWO;
	} else {
		*player = PLAYERONE;
	}
}

static void switchToken(Token *token) {
	if (*token == RED) {
		*token = BLUE;
	} else {
		*token = RED;
	}
}

void setupLogic() {
  List<FallingToken>::traverseList(clearFallingToken, gFallingTokens);
  List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
  List<FallingToken>::traverseList(drawFallingToken, gFallingTokens);
  List<FallingToken>::traverseList(deleteStillToken, gFallingTokens);
}

// NOTE(brendan): inline function to convert Token colour -> Board status
static inline BoardStatus
winningColour(Token colour) {
  if(colour == RED) {
    return RED_WON;
  }
  else {
    return BLUE_WON;
  }
}

static inline int
countTokens(Board board, Token colour) {
  int numberOfTokens = 0;
  for(int row = 0; row < NUM_ROWS; ++row) {
    for(int col = 0; col < NUM_COLS; ++col) {
      if(board_checkCell(board, row, col) == colour) {
        ++numberOfTokens;
      }
    }
  }
  return numberOfTokens;
}

static inline int
square(int x) {
  return x*x;
}

// NOTE(brendan): checks if the board is in progress, won, drawn or an error
BoardStatus checkBoardStatus(Board board) {
  for(int row = 0; row < NUM_ROWS; ++row) {
    for(int col = 0; col < NUM_COLS; ++col) {
      Token firstColour = board_checkCell(board, row, col);
      if(firstColour != EMPTY) {
        // NOTE(brendan): Check for 4-in-a-row in a row
        for(int currentCol = col, currentRow = row;
            (currentCol >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentCol) {
          if(col - currentCol == 3) {
            return winningColour(firstColour);
          }
        }

        // NOTE(brendan): Check for 4-in-a-row in a column
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow) {
          if(row - currentRow == 3) {
            return winningColour(firstColour);
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal decreasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol >= 0) &&
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow, --currentCol) {
          if(row - currentRow == 3) {
            return winningColour(firstColour);
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal increasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol >= 0) &&
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow, ++currentCol) {
          if(row - currentRow == 3) {
            return winningColour(firstColour);
          }
        }
      }
    }
  }

  int numberRedTokens = countTokens(board, RED);
  int numberBlueTokens = countTokens(board, BLUE);
  if(numberRedTokens + numberBlueTokens == NUM_ROWS*NUM_COLS) {
    return DRAW;
  }
  else if(square(numberRedTokens - numberBlueTokens) > 1) {
    return INVALID_BOARD;
  }

  return IN_PROGRESS;
}
