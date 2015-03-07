/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

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
#include "helper.h"
#include "graphics.h"
#include "board.h"
#include "gameLogic.h"
#include "sdl2_connect4.h"
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>

#define MS_PER_UPDATE 13

static void logicStub(GameState *gameState);
static void handleEventsStub(GameState *gameState);
static void renderStub(GraphicsState *graphicsState);
static void mainMenuHandleEvents(GameState *gameState);
/* static void creditsMenuHandleEvents(GameState *gameState); */
static void setupHandleEvents(GameState *gameState);
static void twoPlayerHandleEvents(GameState *gameState);

// NOTE(Zach): JUST FOR REFERENCE!!
//typedef enum {
//  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
//} MenuState;

// NOTE(Zach): Arrays of function pointers.
// NOTE(Zach): The order of the array elements MUST be synchronized with
// NOTE(Zach): the enumeration MenuState!
static void (*handleEvents[NUMBER_OF_STATES])(GameState *gameState) = 
{mainMenuHandleEvents, handleEventsStub, twoPlayerHandleEvents, 
  setupHandleEvents, handleEventsStub, handleEventsStub, handleEventsStub};

static void (*logic[NUMBER_OF_STATES])(GameState *gameState) = 
{logicStub, logicStub, twoPlayerLogic, setupLogic, logicStub, logicStub, logicStub};

static void (*render[NUMBER_OF_STATES])(GraphicsState *graphicsState) = 
{mainMenuRender, renderStub, twoPlayerRender, setupRender, renderStub, 
  renderStub, renderStub}; 

// NOTE(brendan): Stub functions so we don't have to test for NULL functions
static void logicStub(GameState *gameState) {}
static void handleEventsStub(GameState *gameState) {}
static void renderStub(GraphicsState *graphicsState) {}

// NOTE(brendan): checks if a click landed in the rect
inline bool
pointInsideRect(int x, int y, Rectangle rect) {
  if ((x >= rect.topLeft.x) && (y >= rect.topLeft.y) && 
      (x <= rect.bottomRight.x) && (y <= rect.bottomRight.y)) {
    return true; 
  }
  else {
    return false;
  }
}

inline int
square(int x) {
  return x*x;
}

// NOTE(brendan): checks if a click landed in the circle
inline bool
pointInsideCircle(int x, int y, Circle circle) {
  int fromCircleCenterX = x - circle.center.x;
  int fromCircleCenterY = y - circle.center.y;
  if(square(circle.radius) >= 
      square(fromCircleCenterX) + square(fromCircleCenterY)) { 
    return true;
  }
  return false;
}

// NOTE(brendan): Return PLAYERTWO or PLAYERONE randomly
Player choosePlayer() {
  return (rand() % 2) ? PLAYERONE : PLAYERTWO;
}

// NOTE(brendan): Return RED or BLUE randomly
Token chooseToken() {
  return (rand() % 2) ? RED : BLUE;
}

// NOTE(brendan): The transition "state" from setup to twoplayer
void transitionSetupTwoPlayer(GameState *gameState) {
  gameState->currentPlayer = choosePlayer();
  if(gameState->currentToken == RANDOMTOKEN) {
    gameState->currentToken = chooseToken();
  }
  logic[TWOPLAYER] = twoPlayerLogic;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
// NOTE(Jean): Values fixed for the new modified and re-scaled image
static MenuState handleMainMenuMouseClick(int x, int y) {
  //if (x >= 405 && y >= 455 && x <= 511 && y <= 490) return ONEPLAYER; 
  //if (x >= 530 && y>= 455 && x <= 642 && y <= 490) return TWOPLAYER; 
  if(pointInsideRect(x, y, MAINMENU_SETUP_BUTTON_RECT)) {
    return SETUP;
  }
  if(pointInsideRect(x, y, MAINMENU_QUIT_BUTTON_RECT)) {
    return QUIT;
  }
  //if (x >= 890 && y>= 840 && x <= 972 && y <= 868) return CREDITS; 
  return MAINMENU;
}

#if 0
// NOTE(Zach): Determine next MenuState based on where the user clicked
static MenuState handleCreditsMenuMouseClick(int x, int y) {
  //if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
  return MAINMENU;
}
#endif

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
  }
}

#if 0
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
#endif

static MenuState handleSetupMouseClick(int x, int y, GameState *gameState) {
  if(pointInsideRect(x, y, SETUP_2PLAYER_BUTTON_RECT)) {
    if(readyToTransitionSetupTwoPlayer(gameState)) {
      logic[TWOPLAYER] = transitionSetupTwoPlayer;
      return TWOPLAYER;
    }
  }

  if(pointInsideCircle(x, y, SETUP_RED_CLICKY_TOKENS_CIRCLE)) {
    gameState->currentToken = RED;
  }
  else if(pointInsideCircle(x, y, SETUP_BLUE_CLICKY_TOKENS_CIRCLE)) {
    gameState->currentToken = BLUE;
  }
  else if(pointInsideRect(x, y, SETUP_MENU_BUTTON_RECT)) {
    return MAINMENU;
  }
  return SETUP;
}

// NOTE(brendan): handles mouse clicks in the SETUP state
static void setupHandleEvents(GameState *gameState) {
  // Event handler
  SDL_Event e;

  while( SDL_PollEvent( &e ) != 0 ) {
    // User requests quit
    if (e.type == SDL_QUIT) {
      gameState->currentState = QUIT;
    } 
    else if (e.type == SDL_MOUSEBUTTONDOWN &&
        e.button.button == SDL_BUTTON_LEFT) {
      int x, y;
      x = e.button.x;
      y = e.button.y;

      gameState->currentState = handleSetupMouseClick(x, y, gameState);

      // NOTE(brendan): if current state changed, click was outside grid area
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

// NOTE(Zach): switch the player
static void switchPlayer(Player *player) {
  if (*player == PLAYERONE) {
    *player = PLAYERTWO;
  } else {
    *player = PLAYERONE;
  }
}

// NOTE(Zach): switch the token
static void switchToken(Token *token) {
  if (*token == RED) {
    *token = BLUE;
  } else {
    *token = RED;
  }
}

// NOTE(brendan): handles mouse clicks while in the 2 player state
static MenuState twoPlayerHandleMouseClick(int x, int y, GameState *gameState) {
  return TWOPLAYER;
}

// NOTE(brendan): handles mouse clicks in the SETUP state
static void twoPlayerHandleEvents(GameState *gameState) {
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

      gameState->currentState = twoPlayerHandleMouseClick(x, y, gameState);
      // NOTE(Zach): if the game is not in progress, don't allow any more
      // tokens to be dropped
      if (gameState->currentProgress != INPROGRESS) continue;

      // NOTE(brendan): if current state changed, click was outside grid area
      // NOTE(Zach): If the click was outside the GRID
      if (x <= GRID_OFFSET_X || x >= GRID_OFFSET_X + GRID_WIDTH) continue;
      if (y <= GRID_OFFSET_Y || y >= GRID_OFFSET_Y + GRID_HEIGHT) continue;

      int dropColumn = (x - GRID_OFFSET_X)/TOKEN_WIDTH;
      // NOTE(brendan): add token to list of falling tokens if valid drop
      if(dropToken(gameState->board, gameState->currentToken, dropColumn)) {
        // NOTE(Zach): Insert the token into the board
        board_dropToken(gameState->board, gameState->currentToken, dropColumn);
        switchPlayer(&gameState->currentPlayer);
        switchToken(&gameState->currentToken);
      }
    }
  }
}

int connect4() {
  srand((unsigned)(time(0)));
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
          logic[gameState.currentState](&gameState);
          lag -= MS_PER_UPDATE;
        }

        // NOTE(Zach): render images that occur in gameState.currentState
        render[gameState.currentState](&gameState.graphicsState);
      }
      board_destroy(gameState.board);
    }
  }
  close_sdl();
  return 0;
}
