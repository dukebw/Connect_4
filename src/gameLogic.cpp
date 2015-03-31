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
#include <inttypes.h>

static List<TokenLocation> *
getSequentialTokens(Board board);
static void boardToArray(Board b, Token arr[][NUM_COLS]);
static char saveGameFilename[] = "saved_game.dat";
static int minimax(uint64 currentPlayerBitboard, uint64 otherPlayerBitboard, 
                   int column, int depthRemaining, bool maximizingPlayer);
static void checkForWinDraw(GameState *gameState, Token colour);
static void mutualOneTwoPlayerLogic(GameState *gameState);
inline uint64 toggleDrop(uint64 bitboard, int row, int column);
static bool checkBitboardForWin(uint64 bitboard);
static bool checkDraw(uint64 bitboardOne, uint64 bitboardTwo);
static void tryDrop(GameState *gameState, int dropColumn);
static int countBits(uint64 value);
static int bitboardDropPosition(uint64 bitboard, int column);

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
  int playerDropColumn = gameState->graphicsState.playerDrop.column;
  if (playerDropColumn != NO_DROP_COLUMN) {
    tryDrop(gameState, playerDropColumn);
    gameState->graphicsState.playerDrop.column = NO_DROP_COLUMN;
  }
  List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
}

// NOTE(brendan): INPUT: game state and a column
// checks if a drop is valid; if so changes the current player and token, and 
// changes the graphics state so that the token will be dropped next render
// TODO(brendan): change this so it doesn't call the graphics function
static void tryDrop(GameState *gameState, int dropColumn) 
{
  if (dropToken(gameState->board, gameState->currentToken, dropColumn)) {
    board_dropToken(gameState->board, gameState->currentToken, dropColumn);
    uint64 bitboardAllTokens = gameState->redBitboard|gameState->blueBitboard;
    if (gameState->currentToken == RED) {
      gameState->redBitboard = toggleDrop(gameState->redBitboard, 
          bitboardDropPosition(bitboardAllTokens, dropColumn), dropColumn);
    }
    else {
      gameState->blueBitboard = toggleDrop(gameState->blueBitboard, 
          bitboardDropPosition(bitboardAllTokens, dropColumn), dropColumn);
    }
    // NOTE(brendan): don't switch tokens/player in SETUP mode
    if (gameState->currentState != SETUP) {
      gameState->currentPlayer = otherPlayer(gameState->currentPlayer);
      gameState->currentToken = otherToken(gameState->currentToken);
    }
  }
}

// NOTE(Zach): do the one player mode logic
void onePlayerLogic(GameState *gameState) 
{
  mutualOneTwoPlayerLogic(gameState);
  // NOTE(brendan): don't check if game not in progress
  if (gameState->currentProgress == INPROGRESS) {
    if ((gameState->currentPlayer == PLAYERONE) && 
        (gameState->currentProgress == INPROGRESS)) {
      int aiDropColumn = AI_move(gameState->board, gameState->currentToken);
      tryDrop(gameState, aiDropColumn);
    }
    else {
      int playerDropColumn = gameState->graphicsState.playerDrop.column;
      if (playerDropColumn != NO_DROP_COLUMN) {
        tryDrop(gameState, playerDropColumn);
        gameState->graphicsState.playerDrop.column = NO_DROP_COLUMN;
      }
    }
    checkForWinDraw(gameState, otherToken(gameState->currentToken));
  }
}

static void mutualOneTwoPlayerLogic(GameState *gameState)
{
  // NOTE(brendan): save game
  if (gameState->saveGame) {
    gameState->saveGame = false;
    saveGame(gameState);
  }

	// NOTE(Zach): update the physics of all the falling tokens
	List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
}

static void checkForWinDraw(GameState *gameState, Token colour)
{
  // NOTE(Zach): do the checking of the gamestate
  bool colourWon;
  if (colour == RED) {
    colourWon = checkBitboardForWin(gameState->redBitboard);
  }
  else {
    colourWon = checkBitboardForWin(gameState->blueBitboard);
  }
  bool isDraw = checkDraw(gameState->redBitboard, gameState->blueBitboard);

  if (colourWon) {
    setHighlightedTokenList(getSequentialTokens(gameState->board), 
        &gameState->graphicsState);
    gameState->graphicsState.renderIndicatorToken = false;
    gameState->graphicsState.renderStatusInProgress = false;
    if (colour == RED) {
      gameState->currentProgress = REDWON;
      gameState->graphicsState.renderStatusRedWon = true;
    }
    else {
      gameState->currentProgress = BLUEWON;
      gameState->graphicsState.renderStatusBlueWon = true;
    }
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

// NOTE(Zach): do the two player mode logic
void twoPlayerLogic(GameState *gameState) 
{
  mutualOneTwoPlayerLogic(gameState);
  int playerDropColumn = gameState->graphicsState.playerDrop.column;
  if (playerDropColumn != NO_DROP_COLUMN) {
    tryDrop(gameState, playerDropColumn);
    gameState->graphicsState.playerDrop.column = NO_DROP_COLUMN;
    // NOTE(brendan): token changed by tryDrop
    checkForWinDraw(gameState, otherToken(gameState->currentToken));
  }
}

inline int abs(int x) 
{
  return x < 0 ? -x : x;
}

static bool checkInvalidBoard(uint64 redBitboard, uint64 blueBitboard) 
{
  int numberRedTokens = countBits(redBitboard);
  int numberBlueTokens = countBits(blueBitboard);
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
static List<TokenLocation> *
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
// TODO(brendan): do a version of this using bitboards?
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
  int redCount = countBits(gameState->redBitboard);
  int blueCount = countBits(gameState->blueBitboard);
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
	// NOTE(Zach): Reset all the transition from setup to two player flags to 
  // false
	resetGraphicsState(&gameState->graphicsState);

  bool didRedWin = checkBitboardForWin(gameState->redBitboard);
  bool didBlueWin = checkBitboardForWin(gameState->blueBitboard);
  // TODO(brendan): test evaluation function

  bool isDraw = checkDraw(gameState->redBitboard, gameState->blueBitboard);
  bool isBoardInvalid = checkInvalidBoard(gameState->redBitboard, 
                                          gameState->blueBitboard);
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

// NOTE(brendan): source: http://tromp.github.io/c4/fhour.html
// INPUT: 64-bit column-wise bitboard where 1 is the colour under
// consideration's token and 0 otherwise
// OUTPUT: true if there is 4-in-a-row on the bitboard; false otherwise
static bool checkBitboardForWin(uint64 bitboard)
{
  // NOTE(brendan): check \ diagonal
  uint64 y = bitboard & (bitboard >> 6);
  if (y & (y >> 2*6)) {
    return true;
  }

  // NOTE(brendan): check horizontal
  y = bitboard & (bitboard >> 7);
  if (y & (y >> 2*7)) {
    return true;
  }

  // NOTE(brendan): check / diagonal
  y = bitboard & (bitboard >> 8);
  if (y & (y >> 2*8)) {
    return true;
  }

  // NOTE(brendan): check / diagonal
  y = bitboard & (bitboard >> 1);
  if (y & (y >> 2*1)) {
    return true;
  }
  return false;
}

// NOTE(brendan): INPUT: 7x6 token array and a colour to consider
// OUTPUT: 64-bit column-wise bitboard where 1 is a token of that colour
// and 0 is anything else (EMPTY or the other colour)
static uint64 bitboardFromTokenArray(Token tokenArray[][NUM_COLS],
                                     Token colour)
{
  uint64 resultBitboard = 0L;
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (tokenArray[row][col] == colour) {
        resultBitboard = toggleDrop(resultBitboard, row, col);
      }
    }
  }
  return resultBitboard;
}

// ---------------------------------------------------------------------------
// AI
// ---------------------------------------------------------------------------

// TODO(brendan): optimize algorithm to get MAX_DEPTH up from 4 to 6 
// (at least)
// TODO(brendan): use timer to control depth of search tree instead of fixed
// depth
#define MAX_DEPTH 7
enum {WIN_VALUE = 10000, LOSE_VALUE = -10000, DRAW_VALUE = 0,
      THREAT_WEIGHT = 500, INFINITY = 99999};

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

  uint64 aiBitboard = bitboardFromTokenArray(arr, colour);
  uint64 playerBitboard = bitboardFromTokenArray(arr, otherToken(colour));

  int moveColumn = 0, maxValue = -INFINITY, value;
  for (int col = 0; col < NUM_COLS; ++col) {
    if (bitboardDropPosition(aiBitboard|playerBitboard, col) != -1) {
      value = minimax(aiBitboard, playerBitboard, col, MAX_DEPTH, true);
      if (value > maxValue) {
        moveColumn = col;
        maxValue = value;
      }
    }
  }
  // TODO(brendan): remove; debugging
  printf("Max value: %d\n", maxValue);
  return moveColumn;
}

inline int
evaluateBoardOneColour(uint64 colourBitboard, uint64 emptyBitboard)
{
  // NOTE(brendan): 2-in-a-row
  uint64 y = colourBitboard & (colourBitboard >> 7);
  // NOTE(brendan): 3-in-a-row
  y = colourBitboard & (y >> 7);
  // NOTE(brendan): O O O EMPTY
  uint64 threatPositions = emptyBitboard & (y >> 7);
  // NOTE(brendan): EMPTY O O O
  threatPositions |= emptyBitboard & (y << 3*7);

  return THREAT_WEIGHT*countBits(threatPositions);
}

// NOTE(brendan): INPUT: 64-bit column-wise bitboard of tokens of colour
// and likewise bitboard of empty tokens
// OUTPUT: a value indicating the score or weighting of the current board
// position for the current token colour
// currently finds all the ROW "threats": moves that could make 4-in-a-row
// TODO(brendan): pass otherColourBitboard; subtract away ITS threats
static int
evaluateBoard(uint64 currentColourBitboard, uint64 otherColourBitboard,
              uint64 emptyBitboard)
{
  return evaluateBoardOneColour(currentColourBitboard, emptyBitboard) -
         evaluateBoardOneColour(otherColourBitboard, emptyBitboard);
}

#define FULL_BIT_BOARD 0xfdfbf7efdfbf

// NOTE(brendan): INPUT: two disjoint 64-bit column-wise bitboards
// OUTPUT: true iff the union of the two boards is a filled board
inline bool
checkDraw(uint64 bitboardOne, uint64 bitboardTwo)
{
  // NOTE(brendan): casting here because the compiler doesn't understand 
  // uint64_t
  return ((bitboardOne ^ bitboardTwo) == FULL_BIT_BOARD);
}

// NOTE(brendan): INPUT: 64-bit column-wise bitboard, row, column
// OUTPUT: new bitboard with that drop reversed
inline uint64
toggleDrop(uint64 bitboard, int row, int column)
{
  int shiftAmount = (NUM_ROWS - row - 1) + column*NUM_COLS;
  return (bitboard ^ (((uint64)1) << shiftAmount));
}

// NOTE(brendan): source: http://graphics.stanford.edu/~seander/bithacks.html
// INPUT: 64-bit int. OUTPUT: number of 1s set in the input
static int
countBits(uint64 value)
{
  // NOTE(brendan): binary magic numbers
  static const uint64 B[] = {0x5555555555555555L, 0x3333333333333333L, 
                             0x0F0F0F0F0F0F0F0FL, 0x00FF00FF00FF00FFL,
                             0x0000FFFF0000FFFFL, 0x00000000FFFFFFFFL};
  // NOTE(brendan): powers of 2
  static const int S[] = {1, 2, 4, 8, 16, 32};
  value = ((value >> S[0]) & B[0]) + (value & B[0]);
  value = ((value >> S[1]) & B[1]) + (value & B[1]);
  value = ((value >> S[2]) & B[2]) + (value & B[2]);
  value = ((value >> S[3]) & B[3]) + (value & B[3]);
  value = ((value >> S[4]) & B[4]) + (value & B[4]);
  value = ((value >> S[5]) & B[5]) + (value & B[5]);
  return value;
}

// NOTE(brendan): INPUT: bitboard, column. OUTPUT: position of next EMPTY
// location
// where rows go 0 -> 5 top to bottom. Columns go 0 -> 6 left to right
static int bitboardDropPosition(uint64 bitboard, int column)
{
  // NOTE(brendan): isolate column
  bitboard = (bitboard >> column*7) & 0x3f;
  int resultRow = 0;
  // NOTE(brendan): find highest bit
  while (bitboard) {
    bitboard >>= 1;
    ++resultRow;
  }
  return (resultRow < NUM_ROWS) ? (NUM_ROWS - 1 - resultRow) : -1;
}

// NOTE(brendan): INPUT: array of tokens; current token colour;
// drop column; depth remaining, and maximizingPlayer
// OUTPUT: a value indicating the the value of this move for the maximizing
// player
static int
minimax(uint64 currentPlayerBitboard, uint64 otherPlayerBitboard, int column, 
        int depthRemaining, bool maximizingPlayer)
{
  // NOTE(brendan): heuristic value: favourability of node for maximizing
  // player
  int dropRow =
    bitboardDropPosition(currentPlayerBitboard|otherPlayerBitboard, column);
  currentPlayerBitboard = toggleDrop(currentPlayerBitboard, dropRow, column);

	if (checkBitboardForWin(currentPlayerBitboard)) {
    return (maximizingPlayer) ? WIN_VALUE : LOSE_VALUE;
  }
	if (checkDraw(currentPlayerBitboard, otherPlayerBitboard)) {
    return DRAW_VALUE;
  }
  // NOTE(brendan): reached max depth of the game tree; use a heuristic
  // function to evaluate the board and return
  uint64 bitboardAllTokens = currentPlayerBitboard|otherPlayerBitboard;
  if (depthRemaining == 0) {
    int resultWeight = evaluateBoard(currentPlayerBitboard, 
                                     otherPlayerBitboard,
                                     FULL_BIT_BOARD^bitboardAllTokens);
    return (maximizingPlayer) ? resultWeight : -resultWeight;
  }

  int bestValue, value;
  if (maximizingPlayer) {
    bestValue = INFINITY;
    for (int childCol = 0; 
         (childCol < NUM_COLS) && (bestValue > LOSE_VALUE); 
         ++childCol) {
      if (bitboardDropPosition(bitboardAllTokens, childCol) != -1) {
        value = minimax(otherPlayerBitboard, currentPlayerBitboard, childCol, 
                        depthRemaining - 1, false);
        bestValue = minimum(bestValue, value);
      }
    }
  }
  else {
    bestValue = -INFINITY;
    for (int childCol = 0; 
         (childCol < NUM_COLS) && (bestValue < WIN_VALUE); 
         ++childCol) {
      if (bitboardDropPosition(bitboardAllTokens, childCol) != -1) {
        value = minimax(otherPlayerBitboard, currentPlayerBitboard, childCol, 
                        depthRemaining - 1, true);
        bestValue = maximum(bestValue, value);
      }
    }
  }

  return bestValue;
}
