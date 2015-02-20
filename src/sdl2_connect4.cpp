/* Source code by team struct by_lightning{}; */

// TODO
// Decide which modules we need and split existing code into them
// Specifically: re-organize game/event loop (sdl_connect4.cpp), graphics,
// and logic modules so that each module performs only one function
// (this is partially done).
// Fixed update time-step/variable rendering
// Move from switch statement to array-of-function-pointer based
// finite state machine (handleEvents[], logic[] and render[])

/*****/
// Add token-token collisions
// Add the main menu
// Add a checkWin function
// Add a way to highlight tokens
// Add the function to check if the SETUP is valid: 
// i.e. checkWin == FALSE, check number of red and blue tokens
// Add the actual connect4 game fuction
// Add the AI

/****************************************************************************/

// TODO(brendan): for debugging; marked for deletion
#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
		, __LINE__, __FILE__)

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include "graphics.h"
#include "board.h"
#include <stdio.h>
#include "gameMenus.h"
#include "game.h"
#include "sdl2_connect4.h"

#define MS_PER_UPDATE 13

void mainMenuHandleEvents(GameState *gameState);
void mainMenuLogic();
void mainMenuRender();
void creditsMenuHandleEvents(GameState *gameState);
void creditsMenuRender();
void setupHandleEvents(GameState *gameState);
void setupRender();

// TODO(brendan): example; marked for deletion
/* 
	int (* state[])(void) = { entry_state, foo_state, bar_state, exit_state}; 
 */

// NOTE(Zach): JUST FOR REFERENCE!!
//typedef enum {
//  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
//} MenuState;

// TODO(brendan): populate these arrays with their correct functions
// NOTE(Zach): Arrays of function pointers.
// NOTE(Zach): The order of the array elements MUST be synchronized with
// NOTE(Zach): the enumeration MenuState!
void (*logic[NUMBER_OF_STATES])() = {mainMenuLogic, NULL, NULL, setupLogic};
void (*render[NUMBER_OF_STATES])() = {mainMenuRender, NULL, NULL, setupRender};
void (*handleEvents[NUMBER_OF_STATES])(GameState *gameState) = {mainMenuHandleEvents, NULL, NULL, setupHandleEvents};

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
		else {
			//handleMainMenuMouseMotion();
		}
	}

	if(gameState->currentState == SETUP) {
		gameState->currentToken = RED;
		gameState->currentState = SETUP;
		SDL_RenderClear(gRenderer);
		displaySetupTokens();
		SDL_RenderPresent(gRenderer);
	}
}

// NOTE(brendan): stub
void mainMenuLogic() {
}

// NOTE(brendan): does rendering for main menu
void mainMenuRender() {
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);
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

void creditsMenuRender() {
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);

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

			if(50*50 >= 
					(x-75)*(x-75) + 
					(y - (GRID_OFFSET_Y + 50))*(y - (GRID_OFFSET_Y + 50))) { 
				gameState->currentToken = RED;
			}
			else if(50*50 >= 
					(x-(SCREEN_WIDTH-75))*(x-(SCREEN_WIDTH-75)) + 
					(y - (GRID_OFFSET_Y + 50))*(y - (GRID_OFFSET_Y + 50))) {
				gameState->currentToken = BLUE;
			}

			// NOTE(Zach): If the click was outside the GRID
			if (x <= GRID_OFFSET_X || x >= GRID_OFFSET_X + GRID_WIDTH) {
				continue;
			}
			if (y <= GRID_OFFSET_Y || y >= GRID_OFFSET_Y + GRID_HEIGHT) {
				continue;
			}
			int dropColumn = (x - GRID_OFFSET_X)/TOKEN_WIDTH;
			// NOTE(brendan): add token to list of falling tokens if valid drop
			if(dropToken(gameState->board, gameState->currentToken, dropColumn)) {
				// NOTE(Zach): Insert the token into the board
				board_dropToken(gameState->board, gameState->currentToken, dropColumn);
			}
		}   
	}
}

void setupRender() {
	displayBoard();
	SDL_RenderPresent(gRenderer);
	//SDL_Delay(32);
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
