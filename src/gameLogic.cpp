/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#include "gameLogic.h"
#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool didColourWin(Board board, Token colour);
static bool checkDraw(Board board);
static List<TokenLocation> *
getSequentialTokens(Board board);
static char saveGameFilename[] = "saved_game.dat";

// NOTE(brendan): load all the game state from a file
void loadGame(GameState *gameState) {
  FILE *in_file = fopen(saveGameFilename, "r");
  if (in_file == 0) {
    fprintf(stderr, "?Couldn't open %s\n", saveGameFilename);
  }
  else {
    Board tempBoard = board_create();
    board_load(tempBoard, in_file);
    fread(gameState, sizeof(*gameState), 1, in_file);
    loadGraphics(&gameState->graphicsState, in_file);
    gameState->board = tempBoard;
  }
  fclose(in_file);
}

// NOTE(brendan): save all the game state to a file so we can read it back
void saveGame(GameState *gameState) {
  FILE *out_file = fopen(saveGameFilename, "w");
  if (out_file == 0) {
    fprintf(stderr, "?Couldn't open %s\n", saveGameFilename);
  }
  else {
    board_save(gameState->board, out_file);
    fwrite(gameState, sizeof(*gameState), 1, out_file);
    saveGraphics(&gameState->graphicsState, out_file);
  }
  fclose(out_file);
}

// NOTE(brendan): handle MainMenu logic; load and save game for now
void mainMenuLogic(GameState *gameState) {
  // NOTE(brendan): load game
  if (gameState->loadGame) {
    loadGame(gameState);
    gameState->loadGame = false;
  }
}

void setupLogic(GameState *gameState) {
  // NOTE(brendan): save game
  if (gameState->saveGame) {
    gameState->saveGame = false;
    saveGame(gameState);
  }
  List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
}

// NOTE(Zach): do the two player mode logic
void twoPlayerLogic(GameState *gameState) {
  // NOTE(brendan): save game
  if (gameState->saveGame) {
    gameState->saveGame = false;
    saveGame(gameState);
  }

	resetGraphicsState(&gameState->graphicsState);
	// NOTE(Zach): update the physics of all the falling tokens
	List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
	// NOTE(Zach): if the game is not in progress there is no need to do all
	// the checking of the gamestate
	
  if (gameState->currentProgress != INPROGRESS) {
		gameState->graphicsState.renderIndicatorToken = false;
		
	}
	// NOTE(Zach): do the checking of the gamestate
	bool didRedWin = didColourWin(gameState->board, RED);
	bool didBlueWin = didColourWin(gameState->board, BLUE);
	bool isDraw = checkDraw(gameState->board);



  if (didRedWin || didBlueWin) {
    setHighlightedTokenList(getSequentialTokens(gameState->board), 
        &gameState->graphicsState);
  }
  if (didRedWin) {
		gameState->currentProgress = REDWON;
		gameState->graphicsState.renderIndicatorToken = false;
    gameState->graphicsState.renderStatusRedWon = true;
    gameState->graphicsState.renderStatusInProgress = false;
		return;
	}
	if (didBlueWin) {
		gameState->currentProgress = BLUEWON;
		gameState->graphicsState.renderIndicatorToken = false;
    gameState->graphicsState.renderStatusBlueWon = true;
    gameState->graphicsState.renderStatusInProgress = false;
		return;
	}
	if (isDraw) {
		gameState->currentProgress = DRAW;
		gameState->graphicsState.renderIndicatorToken = false;
    gameState->graphicsState.renderStatusDrawGame = true;
    gameState->graphicsState.renderStatusInProgress = false;
		return;
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

// NOTE(brendan): checks if the board has been won by colour
// TODO(brendan): do a version of this just checking for ONE token (the
// body of inner loop) for two-player mode.
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
        for (int currentCol = col, currentRow = row;
            (currentRow >= 0) && (currentCol < NUM_COLS) &&
            (board_checkCell(board, currentRow, currentCol)) == colour;
            --currentRow, ++currentCol) {
          if (row - currentRow == 3) {
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

inline int abs(int x) {
  return x < 0 ? -x : x;
}

static bool checkInvalidBoard(Board board) {
  int numberRedTokens = countTokens(board, RED);
  int numberBlueTokens = countTokens(board, BLUE);
  // NOTE(brendan): difference between number of each type of token is >1
  if(abs(numberRedTokens - numberBlueTokens) > 1) {
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
  free(newHighlightedToken);
  return tokenList;
}

// NOTE(brendan): returns a list of all tokens of the given colour that
// are part of four-or-more in-a-row sequences
// Yes I realize this is brutally inefficient
// TODO(brendan): do a version of this just checking for ONE token (the
// body of inner loop) for two-player mode.
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

// NOTE(brendan): sets the current token to red if there are more blue than
// red tokens, or vice versa. If the counts are equal, sets the current token
// to RANDOMTOKEN
void setCurrentToken(GameState *gameState) {
  int redCount = countTokens(gameState->board, RED);
  int blueCount = countTokens(gameState->board, BLUE);
  if(redCount > blueCount) {
    gameState->currentToken = BLUE;
  }
  else if(redCount < blueCount) {
    gameState->currentToken = RED;
  }
  else {
    gameState->currentToken = RANDOMTOKEN;
  }
}

// NOTE(brendan): returns true if game is IN_PROGRESS; otherwise returns
// false and indicates DRAW, INVALID_BOARD, RED_WON  or BLUE_WON 
// (highlights winning tokens)
bool readyToTransitionSetupTwoPlayer(GameState *gameState) {
	// Note(Zach): Reset all the transition from setup to two player flags to 
  // false
	resetGraphicsState(&gameState->graphicsState);

  bool didRedWin = didColourWin(gameState->board, RED);
  bool didBlueWin = didColourWin(gameState->board, BLUE);
  bool isDraw = checkDraw(gameState->board);
  bool isBoardInvalid = checkInvalidBoard(gameState->board);
  // NOTE(brendan): set flag to render invalid message if there is some error
  // clear invalid message otherwise
  if(isDraw || isBoardInvalid || didRedWin || didBlueWin) {
    gameState->graphicsState.renderInvalidMessage = true;
  }
  if(isDraw) {
    printf("Error! The game is a draw!\n");
  }
  if(isBoardInvalid) {
    gameState->graphicsState.renderInvalidTokenMessage = true;
    printf("Error! Invalid board setup (red tokens - blue tokens > 1)\n");
  }
  if(didRedWin || didBlueWin) {
    setHighlightedTokenList(getSequentialTokens(gameState->board), 
        &gameState->graphicsState);
  }
  if(didRedWin) {
    printf("Error! Red has already won.\n");
  }
  if(didBlueWin) {
    printf("Error! Blue has already won.\n");
  }
  if(!(didRedWin || didBlueWin || isDraw || isBoardInvalid)) {
    gameState->currentState = TWOPLAYER;
    setCurrentToken(gameState);
    return true;
  }
  // NOTE(brendan): game not in progress: continue setup
  return false;
}
