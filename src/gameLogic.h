/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "board.h"
#include "graphics.h"

#define NUMBER_OF_STATES 7

typedef enum {PLAYERONE, PLAYERTWO, RANDOMPLAYER} Player; 

typedef enum {
  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
} MenuState;

typedef enum {INPROGRESS, DRAW, REDWON, BLUEWON} GameProgress;

// TODO(brendan): Move this "model" to its own module?
// NOTE(brendan): contains the "model" of the game
struct GameState {
  MenuState currentState;
	GameProgress currentProgress;
  Token currentToken;
  Player currentPlayer;
  Board board;
  GraphicsState graphicsState;
  // NOTE(brendan): loadGame and saveGame are set in sdl2_connect4 (event
  // handing) and turned off in graphics, when the graphic state is reloaded
  bool loadGame;
  bool saveGame;
};

// NOTE(brendan): handle MainMenu logic; load and save game for now
void mainMenuLogic(GameState *gameState);

// TODO(brendan): change to update (from logic)
void setupLogic(GameState *gameState);

// NOTE(Zach): do the one player mode logic
void onePlayerLogic(GameState *gameState);

// NOTE(Zach): do the two player mode logic
void twoPlayerLogic(GameState *gameState);

// NOTE(brendan): returns true if game is IN_PROGRESS; otherwise returns
// false and indicates DRAW, INVALID_BOARD, RED_WON  or BLUE_WON 
// (highlights winning tokens)
bool readyToTransitionSetupTwoPlayer(GameState *gameState);

// NOTE(Zach): Given a board and a token colour this function will return
// an int corresponding to the column where the AI should move
int AI_move(Board b, Token colour);

#endif // GAMELOGIC_H
