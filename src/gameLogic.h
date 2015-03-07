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

typedef enum {PLAYERONE, PLAYERTWO} Player; 

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
};

// TODO(brendan): change to update (from logic)
void setupLogic(GameState *gameState);
void twoPlayerLogic(GameState *gameState);

// NOTE(brendan): returns true if game is IN_PROGRESS; otherwise returns
// false and indicates DRAW, INVALID_BOARD, RED_WON  or BLUE_WON 
// (highlights winning tokens)
bool readyToTransitionSetupTwoPlayer(GameState *gameState);

#endif // GAMELOGIC_H
