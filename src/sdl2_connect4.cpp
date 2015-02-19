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

// TODO(brendan): example; marked for deletion
/* 
   int (* state[])(void) = { entry_state, foo_state, bar_state, exit_state}; 
   */

#if 0
// TODO(brendan): populate these arrays with their correct functions
void (*logic[NUMBER_OF_STATES])() = {0};
void (*render[NUMBER_OF_STATES])() = {0};
void (*handleEvents[NUMBER_OF_STATES])() = {0};
#endif

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
      dropToken(gameState->board, gameState->currentToken, dropColumn);

      // NOTE(Zach): Insert the token into the board
      board_dropToken(gameState->board, gameState->currentToken, dropColumn);
    }   
  }
}

void setupRender() {
  displayBoard();
  SDL_RenderPresent(gRenderer);
  SDL_Delay(32);
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

      unsigned int currentTime;
      unsigned int elapsedTime;

      // NOTE(Zach): State of the menu
      gameState.currentState = MAINMENU;

      if ((gameState.board = board_create()) == NULL) {
        gameState.currentState = QUIT;
      }

      // NOTE(brendan): game loop: event handling -> logic -> rendering
      while(gameState.currentState != QUIT) {
        switch(gameState.currentState) {
          case MAINMENU:
          {
            mainMenuLogic();
            mainMenuRender();
            mainMenuHandleEvents(&gameState);
          } break;
          case ONEPLAYER:
          {
          } break;
          case TWOPLAYER:
          {
          } break;
          case SETUP:
          {
            setupLogic();
            setupRender();
            setupHandleEvents(&gameState);
          } break;
          case CREDITS:
          {
          } break;
          default:
          {
            printf("Error: Bad state in game loop\n");
          } break;
        }
      }
      board_destroy(gameState.board);
    }
  }
  close_sdl();
  return 0;
}
