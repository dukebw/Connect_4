/* Source code by team struct by_lightning{}; */

/*****/
// TODO
// Add resizable window
// Add token-token collisions
// Add a checkWin function
// Add a way to highlight tokens
// Add the function to check if the SETUP is valid: 
// i.e. checkWin == FALSE, check number of red and blue tokens
// Add the AI

/****************************************************************************/

// TODO(brendan): for debugging; marked for deletion
#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
		, __LINE__, __FILE__)

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include "graphics.h"
#include "board.h"
#include <stdio.h>
#include "gameLogic.h"
#include "sdl2_connect4.h"

#define MS_PER_UPDATE 13

void logicStub();
void handleEventsStub(GameState *gameState);
void renderStub();
void mainMenuHandleEvents(GameState *gameState);
void creditsMenuHandleEvents(GameState *gameState);
void setupHandleEvents(GameState *gameState);

// NOTE(Zach): JUST FOR REFERENCE!!
//typedef enum {
//  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
//} MenuState;

// NOTE(Zach): Arrays of function pointers.
// NOTE(Zach): The order of the array elements MUST be synchronized with
// NOTE(Zach): the enumeration MenuState!
void (*handleEvents[NUMBER_OF_STATES])(GameState *gameState) = 
  {mainMenuHandleEvents, handleEventsStub, handleEventsStub, 
    setupHandleEvents, handleEventsStub, handleEventsStub, handleEventsStub};

void (*logic[NUMBER_OF_STATES])() = {logicStub, logicStub, logicStub, 
  setupLogic, logicStub, logicStub, logicStub};

void (*render[NUMBER_OF_STATES])() = {mainMenuRender, renderStub, renderStub, 
  setupRender, renderStub, renderStub, renderStub};

// NOTE(brendan): Stub functions so we don't have to test for NULL functions
void logicStub() {}
void handleEventsStub(GameState *gameState) {}
void renderStub() {}

// NOTE(Zach): Display and handle mouse clicks/motion of the Main Menu
void mainMenuHandleEvents(GameState *gameState) {
	// NOTE(Zach): Event handler
	SDL_Event e;

	int x, y;

	while(SDL_PollEvent(&e) != 0) {
		// NOTE(Zach): User requests quit
		if (e.type == SDL_QUIT) {
			gameState->currentState = QUIT;
		} 
		else if(e.type == SDL_MOUSEBUTTONDOWN &&
				e.button.button == SDL_BUTTON_LEFT) {

			x = e.button.x;
			y = e.button.y;

			gameState->currentState = handleMainMenuMouseClick(x, y);
		} 
    else if(e.type == SDL_WINDOWEVENT) {
      if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        updateWindowDimensions(e.window.data1, e.window.data2);
      }
    }
		else {
			//handleMainMenuMouseMotion();
		}
	}

	if (gameState->currentState == SETUP) {
		gameState->currentToken = RED;
		transitionSetupRender();
	}
}

// NOTE(Zach): Display and handle mouse clicks/motion of the Credits Menu
void creditsMenuHandleEvents(GameState *gameState) {
	// NOTE(Zach): Event handler
	SDL_Event e;

	int x, y;

	while(SDL_PollEvent(&e) != 0) {
		// NOTE(Zach): User requests quit
		if (e.type == SDL_QUIT) {
			gameState->currentState = QUIT;
		} else if (e.type == SDL_MOUSEBUTTONDOWN &&
				e.button.button == SDL_BUTTON_LEFT) {

			x = e.button.x;
			y = e.button.y;
			gameState->currentState = handleCreditsMenuMouseClick(x, y);
		} else {
			//handleCreditsMenuMouseMotion();
		}
	}
}

void setupHandleEvents(GameState *gameState) {
	// Event handler
	SDL_Event e;

	while( SDL_PollEvent( &e ) != 0 ) {
		// User requests quit
		if (e.type == SDL_QUIT) {
			gameState->currentState = QUIT;
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

      int redTokenX = 25 + getMarginX();
      int redTokenY = GRID_OFFSET_Y - getMarginY();
      int blueTokenX = SCREEN_WIDTH - 125 + getMarginX();
      int blueTokenY = GRID_OFFSET_Y - getMarginY();
      int tokenRadius = 50;
      // NOTE(brendan): radii from center of selector-token
      int redRadiusSquared = 
        (x - (redTokenX + tokenRadius))*(x - (redTokenX + tokenRadius)) + 
        (y - (GRID_OFFSET_Y + tokenRadius))*(y - (redTokenY + tokenRadius));
      int blueRadiusSquared = 
        (x - (blueTokenX + tokenRadius))*(x-(blueTokenX + tokenRadius)) + 
        (y - (blueTokenY + tokenRadius)) *
        (y - (blueTokenY + tokenRadius));

			if(tokenRadius*tokenRadius >= redRadiusSquared) { 
				gameState->currentToken = RED;
			}
			else if(tokenRadius*tokenRadius >= blueRadiusSquared) {
				gameState->currentToken = BLUE;
			}

			// NOTE(Zach): If the click was outside the GRID
			if (x <= GRID_OFFSET_X + getMarginX() || 
          x >= GRID_OFFSET_X + GRID_WIDTH + getMarginX()) {
				continue;
			}
			if (y <= GRID_OFFSET_Y - getMarginY() || 
          y >= GRID_OFFSET_Y + GRID_HEIGHT - getMarginY()) {
				continue;
			}

			int dropColumn = (x - GRID_OFFSET_X - getMarginX())/TOKEN_WIDTH;
			// NOTE(brendan): add token to list of falling tokens if valid drop
			if(dropToken(gameState->board, gameState->currentToken, dropColumn)) {
        // NOTE(Zach): Insert the token into the board
        board_dropToken(gameState->board, gameState->currentToken, 
            dropColumn);
			}
		}   
    else if(e.type == SDL_WINDOWEVENT) {
      if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        updateWindowDimensions(e.window.data1, e.window.data2);
        redrawWindow(gameState->board);
      }
    }
	}
}

int connect4() {
	// NOTE(brendan): Start up SDL and create window.
	if (!init()) {
		// TODO(brendan): Diagnostics
		printf("Failed to initialize!\n");
	} else {
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			// NOTE(brendan): initialize our game state to 0
			GameState gameState = {};

      // TODO(brendan): Put these all in one struct for clarity?
			// NOTE(Zach): timing variables used to run the game loop
			unsigned int currentTime;
			unsigned int elapsedTime;
			unsigned int previousTime;
			unsigned int lag;

			// NOTE(Zach): State of the menu
			gameState.currentState = MAINMENU;

			if ((gameState.board = board_create()) == NULL) {
				gameState.currentState = QUIT;
			}

			previousTime = SDL_GetTicks();
			lag = 0;
			// NOTE(brendan): game loop: event handling -> logic -> rendering
			while(gameState.currentState != QUIT) {
				currentTime = SDL_GetTicks();
				elapsedTime = currentTime - previousTime;
				previousTime = currentTime;
				// NOTE(Zach): lag is how much the game's time is behind
				// the real world's time
				lag += elapsedTime;

				// NOTE(Zach): handle events that occur in gameState.currentState
				handleEvents[gameState.currentState](&gameState);

				// NOTE(Zach): loop until the game time is up-to-date with
				// the real time
				while (lag >= MS_PER_UPDATE) {
					// NOTE(Zach): update the game logic of gameState.currentState
					logic[gameState.currentState]();
					lag -= MS_PER_UPDATE;
				}

				// NOTE(Zach): render images that occur in gameState.currentState
				render[gameState.currentState]();
			}
			board_destroy(gameState.board);
		}
	}
	close_sdl();
	return 0;
}
