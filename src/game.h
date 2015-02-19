#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "gameMenus.h"

typedef enum {PLAYERONE, PLAYERTWO} Player; 

// NOTE(brendan): contains the "model" of the game
struct GameState {
  MenuState currentState;
  Token currentToken;
  Player currentPlayer;
  Board board;
};

void setupLogic();

#endif // GAME_H
