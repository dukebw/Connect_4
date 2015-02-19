#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameMenus.h"
#include "game.h"
#include "graphics.h"

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleMainMenuMouseClick(int x, int y) {
  //if (x >= 102 && y>= 145 && x <= 398 && y <= 211) return ONEPLAYER;
  //if (x >= 96 && y>= 221 && x <= 406 && y <= 287) return TWOPLAYER;
  //if (x >= 96 && y>= 221 && x <= 406 && y <= 287) return SETUP;
  //if (x >= 338 && y>= 441 && x <= 493 && y <= 493) return CREDITS;
  //if (x >= 190 && y>= 297 && x <= 313 && y <= 353) return QUIT;
  return SETUP; // NOTE(Zach): TEMPORARY
  return MAINMENU;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}
