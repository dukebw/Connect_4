#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameMenus.h"
#include "game.h"
#include "graphics.h"

// NOTE(Zach): Determine next MenuState based on where the user clicked
// NOTE(Jean): Values fixed for the new modified and re-scaled image
MenuState handleMainMenuMouseClick(int x, int y) {

  //if (x >= 405 && y >= 455 && x <= 511 && y <= 490) return ONEPLAYER; 
  //if (x >= 530 && y>= 455 && x <= 642 && y <= 490) return TWOPLAYER; 
  if (x >= 430 && y>= 545 && x <= 602 && y <= 610) return SETUP; 
  if (x >= 452 && y>=700 && x <= 575 && y <= 760) return QUIT;
  //if (x >= 890 && y>= 840 && x <= 972 && y <= 868) return CREDITS; 

  return MAINMENU;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}
