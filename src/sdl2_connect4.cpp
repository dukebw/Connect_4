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
#include "gameLogic.h"
#include "sdl2_connect4.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#define MS_PER_UPDATE 13

static void logicStub();
static void handleEventsStub(GameState *gameState);
static void renderStub(GraphicsState *graphicsState);
static void mainMenuHandleEvents(GameState *gameState);
static void creditsMenuHandleEvents(GameState *gameState);
static void setupHandleEvents(GameState *gameState);

// NOTE(Zach): JUST FOR REFERENCE!!
//typedef enum {
//  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
//} MenuState;

// NOTE(Zach): Arrays of function pointers.
// NOTE(Zach): The order of the array elements MUST be synchronized with
// NOTE(Zach): the enumeration MenuState!
static void (*handleEvents[NUMBER_OF_STATES])(GameState *gameState) = 
  {mainMenuHandleEvents, handleEventsStub, handleEventsStub, 
    setupHandleEvents, handleEventsStub, handleEventsStub, handleEventsStub};

static void (*logic[NUMBER_OF_STATES])() = {logicStub, logicStub, logicStub, 
  setupLogic, logicStub, logicStub, logicStub};

static void (*render[NUMBER_OF_STATES])(GraphicsState *graphicsState) = 
{mainMenuRender, renderStub, renderStub, setupRender, renderStub, 
  renderStub, renderStub}; 

// NOTE(brendan): Stub functions so we don't have to test for NULL functions
static void logicStub() {}
static void handleEventsStub(GameState *gameState) {}
static void renderStub(GraphicsState *graphicsState) {}

// NOTE(Zach): Determine next MenuState based on where the user clicked
// NOTE(Jean): Values fixed for the new modified and re-scaled image
static MenuState handleMainMenuMouseClick(int x, int y) {
  //if (x >= 405 && y >= 455 && x <= 511 && y <= 490) return ONEPLAYER; 
  //if (x >= 530 && y>= 455 && x <= 642 && y <= 490) return TWOPLAYER; 
  if ((x >= MAINMENU_SETUP_BUTTON_LEFT) && 
      (y >= MAINMENU_SETUP_BUTTON_TOP) && 
      (x <= MAINMENU_SETUP_BUTTON_RIGHT) && 
      (y <= MAINMENU_SETUP_BUTTON_BOTTOM)) {
    return SETUP; 
  }
  if ((x >= MAINMENU_QUIT_BUTTON_LEFT) && 
      (y >= MAINMENU_QUIT_BUTTON_TOP) && 
      (x <= MAINMENU_QUIT_BUTTON_RIGHT) && 
      (y <= MAINMENU_QUIT_BUTTON_BOTTOM)) {
    return QUIT;
  }
  //if (x >= 890 && y>= 840 && x <= 972 && y <= 868) return CREDITS; 

  return MAINMENU;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
static MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}

// NOTE(Zach): Display and handle mouse clicks/motion of the Main Menu
static void mainMenuHandleEvents(GameState *gameState) {
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

	if (gameState->currentState == SETUP) {
		gameState->currentToken = RED;
		transitionSetupRender();
	}
}

// NOTE(Zach): Display and handle mouse clicks/motion of the Credits Menu
static void creditsMenuHandleEvents(GameState *gameState) {
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

// NOTE(brendan): handles mouse clicks in the SETUP state
static void setupHandleEvents(GameState *gameState) {
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
      int setup2PlayerButtonRight = 
        2*SETUP_BOTTOM_BUTTONS_OFFSET + SETUP_1PLAYER_BUTTON_WIDTH + 
        SETUP_2PLAYER_BUTTON_WIDTH;
      int setup2PlayerButtonLeft = 
        setup2PlayerButtonRight - SETUP_2PLAYER_BUTTON_WIDTH;
      int setup2PlayerButtonTop = SCREEN_HEIGHT - 
        (SETUP_2PLAYER_BUTTON_HEIGHT + SETUP_BOTTOM_BUTTONS_OFFSET);
      int setup2PlayerButtonBottom = 
        SCREEN_HEIGHT - SETUP_BOTTOM_BUTTONS_OFFSET;
      if (x >= setup2PlayerButtonLeft && 
          x <= setup2PlayerButtonRight &&
          y >= setup2PlayerButtonTop &&
          y <= setup2PlayerButtonBottom) {
        if(transitionSetupTwoPlayer(gameState)) {
          /* gameState->currentState = TWOPLAYER; */
          printf("Successfully entered TWOPLAYER from SETUP\n");
        }
			}

      const int TOKEN_RADIUS = TOKEN_WIDTH/2;
      int fromRedTokenCenterX = x - SETUP_CLICKY_TOKENS_OFFSET - TOKEN_RADIUS;
      int fromRedTokenCenterY = y - (GRID_OFFSET_Y + TOKEN_RADIUS);
      int fromBlueTokenCenterX = x - (SCREEN_WIDTH - 
          SETUP_CLICKY_TOKENS_OFFSET - TOKEN_RADIUS);
      int fromBlueTokenCenterY = fromRedTokenCenterY;
			if(square(TOKEN_RADIUS) >= 
					square(fromRedTokenCenterX) + square(fromRedTokenCenterY)) { 
				gameState->currentToken = RED;
			}
			else if(square(TOKEN_RADIUS) >= 
					square(fromBlueTokenCenterX) + square(fromBlueTokenCenterY)) {
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
      gameState.graphicsState = 
        (GraphicsState *)malloc(sizeof(GraphicsState));
      *gameState.graphicsState = {};

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
				render[gameState.currentState](gameState.graphicsState);
			}
			board_destroy(gameState.board);
		}
	}
	close_sdl();
	return 0;
}
