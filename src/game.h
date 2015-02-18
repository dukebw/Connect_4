#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "gameMenus.h"

MenuState setupGame(Board b);
MenuState connect4Game(Board b, int numPlayers);

#endif // GAME_H
