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
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL.h>

static uint64 FULL_BIT_BOARD = (uint64)0xfdfbf7efdfbfLL;

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
static uint64 
lowestThreatsOneColour(uint64 colourBitboard, uint64 emptyBitboard,
                       bool bitboardFirstTurn);
static int threadAIMove(void *gameStatePointer);
static int
evaluateBoard(uint64 currentColourBitboard, uint64 otherColourBitboard,
              uint64 emptyBitboard);

// NOTE(Zach): switch the player
Player otherPlayer(Player player) 
{
  if (player == PLAYERONE) {
    return PLAYERTWO;
  } else {
    return PLAYERONE;
  }
}

// TODO(brendan): move to own module? fix breaking when no save file
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
  if (gameState->playerDropColumn != NO_DROP_COLUMN) {
    tryDrop(gameState, gameState->playerDropColumn);
    gameState->playerDropColumn = NO_DROP_COLUMN;
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
    if (gameState->currentPlayer == PLAYERONE) {
      // NOTE(brendan): don't render indicator token for the AI
      gameState->graphicsState.renderIndicatorToken = false;
      if (gameState->computingAIMove == false) {
        // NOTE(brendan): Run the AI move thread
        if (SDL_CreateThread(threadAIMove, "AI Move Thread", 
                             (void*)gameState)) {
          gameState->computingAIMove = true;
        }
        else {
          printf("Failed to create AI move thread!\n");
        }
      }
      else {
        // NOTE(brendan): an AI move has finally been made
        if (gameState->computerDropColumn != NO_DROP_COLUMN) {
          tryDrop(gameState, gameState->computerDropColumn);
          gameState->computerDropColumn = NO_DROP_COLUMN;
          gameState->computingAIMove = false;
        }
      }
    }
    else {
      gameState->graphicsState.renderIndicatorToken = true;
      if (gameState->playerDropColumn != NO_DROP_COLUMN) {
        tryDrop(gameState, gameState->playerDropColumn);
        gameState->playerDropColumn = NO_DROP_COLUMN;
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
  if (gameState->playerDropColumn != NO_DROP_COLUMN) {
    tryDrop(gameState, gameState->playerDropColumn);
    gameState->playerDropColumn = NO_DROP_COLUMN;
    // NOTE(brendan): token changed by tryDrop
    checkForWinDraw(gameState, otherToken(gameState->currentToken));
  }
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

// ---------------------------------------------------------------------------
// AI
// ---------------------------------------------------------------------------

#define MIN_DEPTH 7
// NOTE(brendan): time for AI to move in ms
#define AI_MOVE_TIME 300
#define EVEN_ROWS (uint64)0x54a952a54a95LL
#define ODD_ROWS (uint64)0xa952a54a952aLL

enum {WIN_WEIGHT = 10000, LOSE_WEIGHT = -10000, DRAW_WEIGHT = 0,
      THREAT_WEIGHT = 500, INFINITY_WEIGHT = 99999};

// NOTE(brendan): INPUT: has to be void * for SDL threads
// OUTPUT: 0?
static int threadAIMove(void *gameStatePointer)
{
  GameState *gameState = (GameState *)gameStatePointer;
  gameState->computerDropColumn = AI_move(gameState->board, 
                                          gameState->currentToken);
  return 0;
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
  uint64 resultBitboard = (uint64)0LL;
  for (int row = 0; row < NUM_ROWS; ++row) {
    for (int col = 0; col < NUM_COLS; ++col) {
      if (tokenArray[row][col] == colour) {
        resultBitboard = toggleDrop(resultBitboard, row, col);
      }
    }
  }
  return resultBitboard;
}

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

  // NOTE(brendan): go in the middle on the first move
  if ((aiBitboard|playerBitboard) == 0) {
    return 3;
  }

  int moveColumn = 0;
  int maxValue = -INFINITY_WEIGHT;
  int value;
  unsigned int timeElapsedMaxValue = 0;
  unsigned int timeElapsedValue;
  unsigned int startTime = SDL_GetTicks();
  unsigned int endTime = startTime;
  for (int depth = MIN_DEPTH; 
      ((endTime - startTime) < AI_MOVE_TIME) && (depth < NUM_COLS*NUM_ROWS);
      endTime = SDL_GetTicks(), ++depth) {
    maxValue = -INFINITY_WEIGHT;
    for (int col = 0; 
        (col < NUM_COLS) && (maxValue < WIN_WEIGHT); 
        ++col) {
      if (bitboardDropPosition(aiBitboard|playerBitboard, col) != -1) {
        unsigned int startTimeValue = SDL_GetTicks();
        value = minimax(aiBitboard, playerBitboard, col, depth, true);
        timeElapsedValue = SDL_GetTicks() - startTimeValue;
        if ((value > maxValue) || 
            ((value == maxValue) && 
             (timeElapsedValue > timeElapsedMaxValue))) {
          moveColumn = col;
          maxValue = value;
          timeElapsedMaxValue = timeElapsedValue;
        }
      }
    }
    // TODO(brendan): remove; testing
    printf("Depth: %d\n", depth);
  }
  // TODO(brendan): remove; debugging
  printf("Max value: %d\n", maxValue);
  return moveColumn;
}

// NOTE(brendan): INPUT: 64-bit integer. OUTPUT: true if odd # of bits set;
// false if even # of bits set
// source: http://graphics.stanford.edu/~seander/bithacks.html
inline bool
oddParity(uint64 v)
{
  v ^= v >> 32;
  v ^= v >> 16;
  v ^= v >> 8;
  v ^= v >> 4;
  v &= 0xf;
  return (((0x6996 >> v) & 1) == 1);
}

// NOTE(brendan): INPUT: bitboard, shift amount, empty bitboard.
// OUTPUT: threat positions due to the type of threat: 8 == /,
// 6 == \ and 7 is horizontal
static uint64
threatPositionsOfType(uint64 colourBitboard, uint64 emptyBitboard, 
                      int shiftAmount)
{
  // NOTE(brendan): 2-in-a-row
  uint64 twoInARow = colourBitboard & (colourBitboard >> shiftAmount);
  // NOTE(brendan): 3-in-a-row
  uint64 threeInARow = colourBitboard & (twoInARow >> shiftAmount);
  // NOTE(brendan): EMPTY O O O
  uint64 threatPositions = emptyBitboard & (threeInARow >> shiftAmount);
  // NOTE(brendan): O O O EMPTY
  threatPositions |= emptyBitboard & (threeInARow << 3*shiftAmount);

  // NOTE(brendan): EMPTY O O
  uint64 emptyoo = emptyBitboard & (twoInARow >> shiftAmount);
  // NOTE(brendan): O EMPTY O O
  threatPositions |= (colourBitboard << shiftAmount) & emptyoo;

  // NOTE(brendan): O O EMPTY
  uint64 ooEmpty = emptyBitboard & (twoInARow << 2*shiftAmount);
  // NOTE(brendan): O O EMPTY O
  return threatPositions | ((colourBitboard >> shiftAmount) & ooEmpty);

}

// NOTE(brendan): INPUT: bitboard for a colour; empty board.
// OUTPUT: THREAT_WEIGHT multiplied by the number of threats that the
// given player has on the board
static uint64
lowestThreatsOneColour(uint64 colourBitboard, uint64 emptyBitboard,
                       bool bitboardFirstTurn)
{
  uint64 threatPositions = threatPositionsOfType(colourBitboard, 
                                                 emptyBitboard, 7);
  threatPositions |= threatPositionsOfType(colourBitboard, emptyBitboard, 8);
  threatPositions |= threatPositionsOfType(colourBitboard, emptyBitboard, 6);

  if (bitboardFirstTurn) {
    threatPositions &= EVEN_ROWS;
  }
  else {
    threatPositions &= ODD_ROWS;
  }

  // NOTE(brendan): only the lowest threat in each column counts
  uint64 lowestThreats = 0;
  for (int col = 0; col < NUM_COLS; ++col) {
    uint64 lowestThreatInCol = threatPositions & ((uint64)0x3fLL << 7*col);
    // NOTE(brendan): extracting lowest 1 bit in value
    if (lowestThreatInCol) {
      lowestThreatInCol = (lowestThreatInCol^(lowestThreatInCol - 1))^
                          ((lowestThreatInCol^(lowestThreatInCol - 1)) >> 1);
    }
    lowestThreats |= lowestThreatInCol;
  }

  return lowestThreats;
}

// NOTE(brendan): INPUT: 64-bit column-wise bitboard of tokens of colour
// and likewise bitboard of empty tokens
// OUTPUT: a value indicating the score or weighting of the current board
// position for the current token colour
// currently finds all the ROW "threats": moves that could make 4-in-a-row
static int
evaluateBoard(uint64 currentColourBitboard, uint64 otherColourBitboard,
              uint64 emptyBitboard)
{
  // NOTE(brendan): board is evaluated AFTER the current player dropped.
  // Therefore the board parity will be odd if the current player went first,
  // and even if the other player went first
  bool currentPlayerFirstTurn = 
    oddParity(currentColourBitboard|otherColourBitboard);
  uint64 lowestThreatsCurrent = 
    lowestThreatsOneColour(currentColourBitboard, emptyBitboard, 
                           currentPlayerFirstTurn);
  uint64 lowestThreatsOther = 
    lowestThreatsOneColour(otherColourBitboard, emptyBitboard, 
                           !currentPlayerFirstTurn);

  // NOTE(brendan): each player's threats are only valid if that threat is on
  // a lower row than any threats the other player has in the same column
  uint64 validThreatsCurrent = 0;
  uint64 validThreatsOther = 0;
  for (int col = 0; col < NUM_COLS; ++col) {
    uint64 threatInColCurrent = lowestThreatsCurrent & ((uint64)0x3fLL << 7*col);
    uint64 threatInColOther = lowestThreatsOther & ((uint64)0x3fLL << 7*col);
    if (threatInColCurrent && threatInColOther) {
      if (threatInColCurrent < threatInColOther) {
        validThreatsCurrent |= threatInColCurrent;
      }
      else {
        validThreatsOther |= threatInColOther;
      }
    }
    else if (threatInColCurrent) {
      validThreatsCurrent |= threatInColCurrent;
    }
    else if (threatInColOther) {
      validThreatsOther |= threatInColOther;
    }
  }

  return THREAT_WEIGHT*(countBits(validThreatsCurrent) - 
                        countBits(validThreatsOther));
}

// NOTE(brendan): INPUT: two disjoint 64-bit column-wise bitboards
// OUTPUT: true iff the union of the two boards is a filled board
inline bool
checkDraw(uint64 bitboardOne, uint64 bitboardTwo)
{
  return ((bitboardOne ^ bitboardTwo) == FULL_BIT_BOARD);
}

// NOTE(brendan): INPUT: 64-bit column-wise bitboard, row, column
// OUTPUT: new bitboard with that drop reversed
inline uint64
toggleDrop(uint64 bitboard, int row, int column)
{
  int shiftAmount = (NUM_ROWS - row - 1) + column*NUM_COLS;
  return (bitboard ^ (((uint64)1LL) << shiftAmount));
}

// NOTE(brendan): source: http://graphics.stanford.edu/~seander/bithacks.html
// INPUT: 64-bit int. OUTPUT: number of 1s set in the input
static int
countBits(uint64 value)
{
  // NOTE(brendan): binary magic numbers
  static const uint64 B[] = {
    (uint64)0x5555555555555555LL, (uint64)0x3333333333333333LL, 
    (uint64)0x0F0F0F0F0F0F0F0FLL, (uint64)0x00FF00FF00FF00FFLL, 
    (uint64)0x0000FFFF0000FFFFLL, (uint64)0x00000000FFFFFFFFLL};
  // NOTE(brendan): powers of 2
  static const int S[] = {1, 2, 4, 8, 16, 32};
  value = ((value >> S[0]) & B[0]) + (value & B[0]);
  value = ((value >> S[1]) & B[1]) + (value & B[1]);
  value = ((value >> S[2]) & B[2]) + (value & B[2]);
  value = ((value >> S[3]) & B[3]) + (value & B[3]);
  value = ((value >> S[4]) & B[4]) + (value & B[4]);
  value = ((value >> S[5]) & B[5]) + (value & B[5]);
  return (int)value;
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
    return (maximizingPlayer) ? WIN_WEIGHT : LOSE_WEIGHT;
  }
	if (checkDraw(currentPlayerBitboard, otherPlayerBitboard)) {
    return DRAW_WEIGHT;
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

  // TODO(brendan): compress these two loops with loop from AI_move
  int bestValue, value;
  if (maximizingPlayer) {
    bestValue = INFINITY_WEIGHT;
    for (int childCol = 0; 
         (childCol < NUM_COLS) && (bestValue > LOSE_WEIGHT); 
         ++childCol) {
      if (bitboardDropPosition(bitboardAllTokens, childCol) != -1) {
        value = minimax(otherPlayerBitboard, currentPlayerBitboard, childCol, 
                        depthRemaining - 1, false);
        bestValue = minimum(bestValue, value);
      }
    }
  }
  else {
    bestValue = -INFINITY_WEIGHT;
    for (int childCol = 0; 
         (childCol < NUM_COLS) && (bestValue < WIN_WEIGHT); 
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
