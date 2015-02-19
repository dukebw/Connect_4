#include "game.h"
#include "graphics.h"
#include "linkedList.h"
#include <stdbool.h>

static void switchPlayer(Player *player);
static void switchToken(Token *token);

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
