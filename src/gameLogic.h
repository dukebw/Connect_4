#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "board.h"

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
