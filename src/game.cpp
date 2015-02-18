#include "game.h"
#include "graphics.h"
#include <stdbool.h>

typedef enum {PLAYERONE, PLAYERTWO} Player; 

static void switchPlayer(Player *player);
static void switchToken(Token *token);

static void switchPlayer(Player *player)
{
	if (*player == PLAYERONE) {
		*player = PLAYERTWO;
	} else {
		*player = PLAYERONE;
	}
}

static void switchToken(Token *token)
{
	if (*token == RED) {
		*token = BLUE;
	} else {
		*token = RED;
	}
}

// NOTE(Zach): setup mode
MenuState setupGame(Board b)
{
	Token currentToken = RED;
	MenuState currentState = SETUP;

	SDL_RenderClear(gRenderer);
	displayBoard();
	displaySetupTokens();
	SDL_RenderPresent(gRenderer);

	// Event handler
	SDL_Event e;

	while(currentState == SETUP) {
		while( SDL_PollEvent( &e ) != 0 ) {

			// User requests quit
			if (e.type == SDL_QUIT) {
				currentState = QUIT;
			} else if (e.type == SDL_MOUSEBUTTONDOWN &&
				e.button.button == SDL_BUTTON_LEFT) {
				int x, y;
				x = e.button.x;
				y = e.button.y;
				// NOTE(Zach): We will need these lines
				#if 0
				// If the menu button was clicked
				if (x >= 426 && y >= GRID_OFFSET_Y + GRID_HEIGHT + 70) {
					currentState = MAINMENU;
				// If the ONEPLAYER button was clicked
				} else if (x <= 40 && y >= GRID_OFFSET_Y + GRID_HEIGHT + 60) {
					currentState = ONEPLAYER;
				// If the TWOPLAYER button was clicked
				} else if (x <= 40 && y >= GRID_OFFSET_Y + GRID_HEIGHT + 60) {
					currentState = TWOPLAYER;
				}
				#endif

				if (50*50 >= (x-75)*(x-75) + (y - (GRID_OFFSET_Y + 50))*(y - (GRID_OFFSET_Y + 50))) currentToken = RED;
				else if (50*50 >= (x-(SCREEN_WIDTH-75))*(x-(SCREEN_WIDTH-75)) + (y - (GRID_OFFSET_Y + 50))*(y - (GRID_OFFSET_Y + 50))) currentToken = BLUE;

				// NOTE(Zach): If the click was outside the GRID
				if (x <= GRID_OFFSET_X || x >= GRID_OFFSET_X + GRID_WIDTH) continue;
				if (y <= GRID_OFFSET_Y || y >= GRID_OFFSET_Y + GRID_HEIGHT) continue;
				dropToken(b, currentToken, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
			} else {
				;
			}
		}
	traverseList(clearFallingToken, gFallingTokens);
	traverseList(updateFallingToken, 0.5, gFallingTokens);
	traverseList(drawFallingToken, gFallingTokens);
	traverseList(deleteStillToken, gFallingTokens);
	displayBoard();
	SDL_RenderPresent(gRenderer);
	SDL_Delay(32);
	//SDL_FlushEvent(SDL_MOUSEMOTION);
	}
	return currentState;
}

// NOTE(Zach): the actual game
MenuState connect4Game(Board b, int numPlayers) {
	return MAINMENU;
}
