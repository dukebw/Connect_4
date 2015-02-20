#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameMenus.h"
#include "game.h"
#include "graphics.h"

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleMainMenuMouseClick(int x, int y) {

  // NOTE(Jean): New values corresponding to the main screen layout
  //     please note that a new MenuState has been added : "SETUPTWOPLATER"

  //if (x >= 420 && y >= 405 && x <= 560 && y <= 435) return ONEPLAYER; 
  //if (x >= 580 && y>= 405 && x <= 710 && y <= 435) return TWOPLAYER; 
  //if (x >= 420 && y>= 565 && x <= 560 && y <= 595) return SETUPONEPLAYER; 
  //if (x >= 580 && y>= 565 && x <= 710 && y <= 595) return SETUPTWOPLAYER; 
  //if (x >= 975 && y>= 745 && x <= 1070 && y <= 770) return CREDITS; 
  //if (x >= 500 && y>=650 && x <= 630 && y <= 710) return QUIT;

  // NOTE(Jean): These are the old values created by someone else
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
