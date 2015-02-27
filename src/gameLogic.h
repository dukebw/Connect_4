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

// TODO(brendan): Move this "model" to its own module?
// NOTE(brendan): contains the "model" of the game
struct GameState {
  MenuState currentState;
  Token currentToken;
  Player currentPlayer;
  Board board;
  GraphicsState *graphicsState;
};

inline int
square(int x) {
  return x*x;
}

void setupLogic();

// NOTE(brendan): returns true if game is IN_PROGRESS; otherwise returns
// false and indicates DRAW, INVALID_BOARD, RED_WON  or BLUE_WON 
// (highlights winning tokens)
bool transitionSetupTwoPlayer(GameState *gameState);

#endif // GAMELOGIC_H
