#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameLogic.h"
#include "graphics.h"
#include "linkedList.h"

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

// NOTE(brendan): checks if the board has been won by colour
static bool
didColourWin(Board board, Token colour) {
  for(int row = 0; row < NUM_ROWS; ++row) {
    for(int col = 0; col < NUM_COLS; ++col) {
      if(board_checkCell(board, row, col) == colour) {
        // NOTE(brendan): Check for 4-in-a-row in a row starting at the
        // (row, col) token
        for(int currentCol = col, currentRow = row;
            (currentCol >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == colour;
            --currentCol) {
          if(col - currentCol == 3) {
            return true;
          }
        }

        // NOTE(brendan): Check for 4-in-a-row in a column
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == colour;
            --currentRow) {
          if(row - currentRow == 3) {
            return true;
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal decreasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol >= 0) &&
            (board_checkCell(board, currentRow, currentCol)) == colour;
            --currentRow, --currentCol) {
          if(row - currentRow == 3) {
            return true;
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal increasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol >= 0) &&
            (board_checkCell(board, currentRow, currentCol)) == colour;
            --currentRow, ++currentCol) {
          if(row - currentRow == 3) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

// NOTE(brendan): checks if the board is drawn
static bool checkDraw(Board board) {
  int numberRedTokens = countTokens(board, RED);
  int numberBlueTokens = countTokens(board, BLUE);
  if(numberRedTokens + numberBlueTokens == NUM_ROWS*NUM_COLS) {
    return true;
  }
  return false;
}

static bool checkInvalidBoard(Board board) {
  int numberRedTokens = countTokens(board, RED);
  int numberBlueTokens = countTokens(board, BLUE);
  // NOTE(brendan): difference between number of each type of token is >1
  if(square(numberRedTokens - numberBlueTokens) > 1) {
    return true;
  }
  return false;
}

// TODO(brendan): checks if tokenA and tokenB are "equal"; replace with
// operator overloading
static bool
equals(TokenLocation *tokenA, TokenLocation *tokenB) {
  return (tokenA->row == tokenB->row) && 
    (tokenA->column == tokenB->column) &&
    (tokenA->colour == tokenB->colour);
}

// NOTE(brendan): adds a tokenList(row, column, colour) to tokenList
inline List<TokenLocation> *
addNewTokenLocation(List<TokenLocation> *tokenList, int row, int column, 
    Token colour) {
  TokenLocation *newHighlightedToken = 
    (TokenLocation *)malloc(sizeof(TokenLocation));
  newHighlightedToken->row = row;
  newHighlightedToken->column = column;
  newHighlightedToken->colour = colour;
  TokenLocation *matchingToken = 
    List<TokenLocation>::reduceList(equals, newHighlightedToken, tokenList);
  if(matchingToken == NULL) {
    return List<TokenLocation>::addToList(newHighlightedToken, 
        tokenList);
  }
  return tokenList;
}

// NOTE(brendan): returns a list of all tokens of the given colour that
// are part of four-or-more in-a-row sequences
// Yes I realize this is brutally inefficient
static List<TokenLocation> *
getSequentialTokens(Board board) {
  List<TokenLocation> *sequentialTokens = NULL;
  for(int row = 0; row < NUM_ROWS; ++row) {
    for(int col = 0; col < NUM_COLS; ++col) {
      Token firstColour = board_checkCell(board, row, col);
      if(firstColour != EMPTY) {
        // NOTE(brendan): Check for 4-in-a-row in a row starting at the
        // (row, col) token
        for(int currentCol = col, currentRow = row;
            (currentCol >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentCol) {
          if(col - currentCol == 3) {
            while(currentCol <= col) {
              sequentialTokens = addNewTokenLocation(sequentialTokens, row, 
                    currentCol++, firstColour);
            }
            break;
          }
        }

        // NOTE(brendan): Check for 4-in-a-row in a column
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && 
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow) {
          if(row - currentRow == 3) {
            while(currentRow <= row) {
              sequentialTokens = addNewTokenLocation(sequentialTokens, 
                  currentRow++, col, firstColour);
            }
            break;
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal decreasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol >= 0) &&
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow, --currentCol) {
          if(row - currentRow == 3) {
            while(currentRow <= row) {
              sequentialTokens = addNewTokenLocation(sequentialTokens, 
                  currentRow++, currentCol++, firstColour);
            }
            break;
          }
        }

        // NOTE(brendan): check for 4-in-a-row diagonal increasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol < NUM_COLS) &&
            (board_checkCell(board, currentRow, currentCol)) == firstColour;
            --currentRow, ++currentCol) {
          if(row - currentRow == 3) {
            while(currentRow <= row) {
              sequentialTokens = addNewTokenLocation(sequentialTokens, 
                  currentRow++, currentCol--, firstColour);
            }
            break;
          }
        }
      }
    }
  }

  return sequentialTokens;
}

// NOTE(brendan): returns true if game is IN_PROGRESS; otherwise returns
// false and indicates DRAW, INVALID_BOARD, RED_WON  or BLUE_WON 
// (highlights winning tokens)
bool transitionSetupTwoPlayer(GameState *gameState) {
  bool didRedWin = didColourWin(gameState->board, RED);
  bool didBlueWin = didColourWin(gameState->board, BLUE);
  bool isDraw = checkDraw(gameState->board);
  bool isBoardInvalid = checkInvalidBoard(gameState->board);
  if(isDraw) {
    printf("Error! The game is a draw!\n");
  }
  if(isBoardInvalid) {
    printf("Error! Invalid board setup (red tokens - blue tokens > 1)\n");
  }
  if(didRedWin || didBlueWin) {
    setHighlightedTokenList(getSequentialTokens(gameState->board));
  }
  if(didRedWin) {
    printf("Error! Red has already won.\n");
  }
  if(didBlueWin) {
    printf("Error! Blue has already won.\n");
  }
  if(!(didRedWin || didBlueWin || isDraw || isBoardInvalid)) {
    return true;
  }
  // NOTE(brendan): game not in progress: continue setup
  return false;
}
