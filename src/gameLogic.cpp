#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameLogic.h"
#include "graphics.h"
#include "linkedList.h"

// NOTE(Zach): Determine next MenuState based on where the user clicked
// NOTE(Jean): Values fixed for the new modified and re-scaled image
MenuState handleMainMenuMouseClick(int x, int y) {
  // TODO(brendan): add rect struct to make these offset calculations more
  // concise
  int marginX = getMarginX();
  int marginY = getMarginY();
#if 0
  int setupTop = 545 - marginY;
  int setupBottom = 610 - marginY;
#endif
  // NOTE(brendan): temporary fix to get game to display properly on 768
  // pixel height monitors
  int setupTop = 545;
  int setupBottom = 610;
  int setupLeft = 430 + marginX;
  int setupRight = 602 + marginX;
  int quitTop = 700 - marginY;
  int quitBottom = 760 - marginY;
  int quitLeft = 452 + marginX;
  int quitRight = 575 + marginX;
  //if (x >= 405 && y >= 455 && x <= 511 && y <= 490) return ONEPLAYER; 
  //if (x >= 530 && y>= 455 && x <= 642 && y <= 490) return TWOPLAYER; 
  if (x >= setupLeft && y >= setupTop && x <= setupRight && y <= setupBottom) {
    return SETUP; 
  }
  if (x >= quitLeft && y >= quitTop && x <= quitRight && y <= quitBottom) {
    return QUIT;
  }
  //if (x >= 890 && y>= 840 && x <= 972 && y <= 868) return CREDITS; 
  return MAINMENU;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}

static void switchPlayer(Player *player) {
	if (*player == PLAYERONE) {
		*player = PLAYERTWO;
	} else {
		*player = PLAYERONE;
	}
}

static void switchToken(Token *token) {
	if (*token == RED) {
		*token = BLUE;
	} else {
		*token = RED;
	}
}

void setupLogic() {
  List<FallingToken>::traverseList(clearFallingToken, gFallingTokens);
  List<FallingToken>::traverseList(updateFallingToken, 0.5, gFallingTokens);
  List<FallingToken>::traverseList(drawFallingToken, gFallingTokens);
  List<FallingToken>::traverseList(deleteStillToken, gFallingTokens);
}
