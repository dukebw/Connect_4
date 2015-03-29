/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/
#include "board.h"
#include "gameLogic.h"
#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool didColourWin(Board board, Token colour);
static bool checkDraw(Board board);
static List<TokenLocation> *
getSequentialTokens(Board board);
static void boardToArray(Board b, Token arr[][NUM_COLS]);
static char saveGameFilename[] = "saved_game.dat";
static int negamax(Token token_array[][NUM_COLS], Token colour, int column, 
                   int depthRemaining);
// TODO(brendan): debugging; remove
/* static void DEBUGPrintBoard(Token token_array[][NUM_COLS]); */

// NOTE(Zach): switch the player
Player otherPlayer(Player player) 
{
  if (player == PLAYERONE) {
    return PLAYERTWO;
  } else {
    return PLAYERONE;
  }
}

// NOTE(brendan): load all the game state from a file
void loadGame(GameState *gameState) 
{
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
void saveGame(GameState *gameState) 
{
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
void mainMenuLogic(GameState *gameState) 
{
  // NOTE(brendan): load game
  if (gameState->loadGame) {
    loadGame(gameState);
    gameState->loadGame = false;
  }
}

void setupLogic(GameState *gameState) 
{
  // NOTE(brendan): save game
  if (gameState->saveGame) {
    gameState->saveGame = false;
    saveGame(gameState);
  }
  List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
}

// NOTE(brendan): INPUT: game state and a column
// checks if a drop is valid; if so changes the current player and token, and 
// changes the graphics state so that the token will be dropped next render
// TODO(brendan): change this so it doesn't call the graphics function
static void tryDrop(GameState *gameState, int dropColumn) 
{
  if(dropToken(gameState->board, gameState->currentToken, dropColumn)) {
    board_dropToken(gameState->board, gameState->currentToken, dropColumn);
    gameState->currentPlayer = otherPlayer(gameState->currentPlayer);
    gameState->currentToken = otherToken(gameState->currentToken);
  }
}

// NOTE(Zach): do the one player mode logic
void onePlayerLogic(GameState *gameState) 
{
  if ((gameState->currentPlayer == PLAYERONE) && 
      (gameState->currentProgress == INPROGRESS)) {
    int aiDropColumn = AI_move(gameState->board, gameState->currentToken);
    tryDrop(gameState, aiDropColumn);
  }
	twoPlayerLogic(gameState);
}

// NOTE(Zach): do the two player mode logic
void twoPlayerLogic(GameState *gameState) 
{
  // NOTE(brendan): save game
  if (gameState->saveGame) {
    gameState->saveGame = false;
    saveGame(gameState);
  }

	// NOTE(Zach): update the physics of all the falling tokens
	List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
	// NOTE(Zach): if the game is not in progress there is no need to do all
	// the checking of the gamestate
	
  if (gameState->currentProgress != INPROGRESS) {
		gameState->graphicsState.renderIndicatorToken = false;
	}

  int playerDropColumn = gameState->graphicsState.playerDrop.column;
  if (playerDropColumn != NO_DROP_COLUMN) {
    tryDrop(gameState, playerDropColumn);
    gameState->graphicsState.playerDrop.column = NO_DROP_COLUMN;
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
}

static inline int
countTokens(Board board, Token colour) 
{
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
// TODO(brendan): do efficient ninja implementation with bitboard
static bool
didColourWin(Board board, Token colour) 
{
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
static bool checkDraw(Board board) 
{
  int numberRedTokens = countTokens(board, RED);
  int numberBlueTokens = countTokens(board, BLUE);
  if(numberRedTokens + numberBlueTokens == NUM_ROWS*NUM_COLS) {
    return true;
  }
  return false;
}

inline int abs(int x) 
{
  return x < 0 ? -x : x;
}

static bool checkInvalidBoard(Board board) 
{
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
equals(TokenLocation *tokenA, TokenLocation *tokenB) 
{
  return (tokenA->row == tokenB->row) && 
    (tokenA->column == tokenB->column) &&
    (tokenA->colour == tokenB->colour);
}

// NOTE(brendan): adds a tokenList(row, column, colour) to tokenList
inline List<TokenLocation> *
addNewTokenLocation(List<TokenLocation> *tokenList, int row, int column, 
    Token colour) 
{
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
getSequentialTokens(Board board) 
{
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
void setCurrentToken(GameState *gameState) 
{
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
bool readyToTransitionSetupTwoPlayer(GameState *gameState) 
{
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

// ---------------------------------------------------------------------------
// AI
// ---------------------------------------------------------------------------

#define MAX_DEPTH 4
enum Weighting {WIN_VALUE = 10000, LOSE_VALUE = -10000, DRAW_VALUE = 0,
                THREAT_WEIGHT = 500};

// NOTE(Zach): Read the board into a two dimensional array
static void boardToArray(Board b, Token arr[][NUM_COLS])
{
	int row, col;

	for (row = 0; row < NUM_ROWS; row++) {
		for (col = 0; col < NUM_COLS; col++) {
			arr[row][col] = board_checkCell(b, row, col);
		}
	}
}

// NOTE(Zach): Given a board and a token colour this function will return
// an int corresponding to the column where the AI should move
int AI_move(Board b, Token colour)
{
	Token arr[NUM_ROWS][NUM_COLS];
	boardToArray(b, arr);
  int moveColumn = 0, maxValue = LOSE_VALUE, value;
  for (int col = 0; col < NUM_COLS; ++col) {
    if (board_dropPosition(b, col) != -1) {
      if ((value = negamax(arr, colour, col, MAX_DEPTH)) > maxValue) {
        moveColumn = col;
        maxValue = value;
      }
    }
  }
  // TODO(brendan): remove; debugging
  printf("Max value: %d\n", maxValue);
  return moveColumn;
}

// NOTE(brendan): INPUT: array of tokens; current token colour
// OUTPUT: none
// UPDATE: the array of tokens
// reverses the last move made in the column
static void
reverseDrop(Token token_array[][NUM_COLS], int column)
{
  for (int rowIndex = 0; rowIndex < NUM_ROWS; ++rowIndex) {
    if (token_array[rowIndex][column] != EMPTY) {
      token_array[rowIndex][column] = EMPTY;
      return;
    }
  }
}

// NOTE(brendan): INPUT: array of tokens; current token colour
// OUTPUT: a value indicating the score or weighting of the current board
// position for the current token colour
// currently finds all the "threats": moves that could make 4-in-a-row
static int
evaluateBoard(Token token_array[][NUM_COLS], Token colour)
{
  int resultWeight = 0;
  bool threatsCounted[NUM_ROWS][NUM_COLS] = {};
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (token_array[row][col] == colour) {
        // NOTE(brendan): Check for threat starting at the (row, col) token
        for (int currentCol = col, currentRow = row;
            (currentCol >= 1) &&
            (token_array[currentRow][currentCol] == colour);
            --currentCol) {
          // TODO(brendan): look for threats in both directions?
          if(col - currentCol == 2) {
            if ((token_array[currentRow][currentCol - 1] == EMPTY) &&
                (threatsCounted[currentRow][currentCol - 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow][currentCol - 1] = true;
            }
            break;
          }
        }
        // NOTE(brendan): Check for threat starting at the (row, col) token
        for (int currentCol = col, currentRow = row;
            (currentCol < (NUM_COLS - 1)) &&
            (token_array[currentRow][currentCol] == colour);
            ++currentCol) {
          // TODO(brendan): look for threats in both directions?
          if(col - currentCol == 2) {
            if ((token_array[currentRow][currentCol - 1] == EMPTY) &&
                (threatsCounted[currentRow][currentCol - 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow][currentCol - 1] = true;
            }
            break;
          }
        }

        // NOTE(brendan): check for threat diagonal decreasing left
        for(int currentCol = col, currentRow = row;
            (currentRow >= 1) && (currentCol >= 1) &&
            (token_array[currentRow][currentCol] == colour);
            --currentRow, --currentCol) {
          if(row - currentRow == 2) {
            if ((token_array[currentRow - 1][currentCol - 1] == EMPTY) &&
                (threatsCounted[currentRow - 1][currentCol - 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow - 1][currentCol - 1] = true;
            }
            break;
          }
        }
        // NOTE(brendan): check for threat diagonal decreasing left
        for(int currentCol = col, currentRow = row;
            (currentRow < (NUM_ROWS - 1)) && (currentCol < (NUM_COLS - 1)) &&
            (token_array[currentRow][currentCol] == colour);
            ++currentRow, ++currentCol) {
          if(row - currentRow == 2) {
            if ((token_array[currentRow - 1][currentCol - 1] == EMPTY) &&
                (threatsCounted[currentRow - 1][currentCol - 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow - 1][currentCol - 1] = true;
            }
            break;
          }
        }

        // NOTE(brendan): check for threat diagonal increasing left
        for (int currentCol = col, currentRow = row;
            (currentRow >= 1) && (currentCol < (NUM_COLS - 1)) &&
            (token_array[currentRow][currentCol] == colour);
            --currentRow, ++currentCol) {
          if(row - currentRow == 2) {
            if ((token_array[currentRow - 1][currentCol + 1] == EMPTY) &&
                (threatsCounted[currentRow - 1][currentCol + 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow - 1][currentCol + 1] = true;
            }
            break;
          }
        }
        // NOTE(brendan): check for threat diagonal increasing left
        for (int currentCol = col, currentRow = row;
            (currentRow < (NUM_ROWS - 1)) && (currentCol >= 1) &&
            (token_array[currentRow][currentCol] == colour);
            ++currentRow, --currentCol) {
          if(row - currentRow == 2) {
            if ((token_array[currentRow - 1][currentCol + 1] == EMPTY) &&
                (threatsCounted[currentRow - 1][currentCol + 1] == false)) {
              resultWeight += THREAT_WEIGHT;
              threatsCounted[currentRow - 1][currentCol + 1] = true;
            }
            break;
          }
        }
      }
    }
  }
  return resultWeight;
}

// TODO(brendan): ongoing bug; doesn't recognize blocking
// NOTE(brendan): INPUT: array of tokens; current token colour
// OUTPUT: a value indicating the best move for the token colour given the
// board state corresponding to the array of tokens
static int
negamax(Token token_array[][NUM_COLS], Token colour, int column, 
        int depthRemaining) 
{
  // TODO(brendan): return colour * heuristic value of node?
	Board b = (Board)token_array;
	board_dropToken(b, colour, column);
	if (didColourWin(b, colour)) {
    reverseDrop(token_array, column);
    return WIN_VALUE;
  }
	if (checkDraw(b)) {
    reverseDrop(token_array, column);
    return DRAW_VALUE;
  }
  // NOTE(brendan): reached max depth of the game tree; use a heuristic
  // function to evaluate the board and return
  if (depthRemaining == 0) {
    int resultWeight = evaluateBoard(token_array, colour);
    reverseDrop(token_array, column);
    return resultWeight;
  }

	int bestValue = LOSE_VALUE;
	int value;
	// NOTE(Zach): for each child node
	for (int childCol = 0; childCol < NUM_COLS; ++childCol) {
		if (board_dropPosition(b, childCol) != -1) {
			value = -negamax(token_array, otherToken(colour), childCol, 
                       depthRemaining - 1);
			bestValue = maximum(bestValue, value);
		}
	}
  reverseDrop(token_array, column);
	return bestValue;
}

#if 0
// TODO(brendan): debugging; remove
static void
DEBUGPrintBoard(Token token_array[][NUM_COLS])
{
  printf("\n");
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (token_array[row][col] == EMPTY) {
        printf("empty ");
      }
      else if (token_array[row][col] == RED) {
        printf("red ");
      }
      else if (token_array[row][col] == BLUE) {
        printf("blue ");
      }
    }
    printf("\n");
  }
}
#endif
