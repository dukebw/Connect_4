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

// NOTE(Zach): Display and handle mouse clicks/motion of the Main Menu
MenuState mainMenu(void);

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleMainMenuMouseClick(int x, int y);

// NOTE(Zach): Display and handle mouse clicks/motion of the Credits Menu
MenuState creditsMenu(void);

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y);

void setupLogic();

#endif // GAMELOGIC_H
