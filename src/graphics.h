#ifndef GRAPHICS_H // NOTE(Zach): This is an include guard
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include "board.h"
#include "linkedList.h"

#define SCREEN_WIDTH  1002
#define SCREEN_HEIGHT  902
#define TOKEN_WIDTH  100
#define TOKEN_HEIGHT  100
#define GRID_OFFSET_Y  151
#define GRID_OFFSET_X  151
#define GRID_WIDTH 700
#define GRID_HEIGHT 600

// NOTE(brendan): defined in graphics.cpp; implementation details not leaked
struct FallingToken;
struct TextureWrapper;

void drawFallingToken(FallingToken *token);
void clearFallingToken(FallingToken *fallingToken);
void updateFallingToken(FallingToken *fallingToken, float dt);
void displayBoard(void);
void displaySetupTokens(void);
void displayMainMenu(void);
void displayCreditsMenu(void);

// TODO(brendan): Make these local at some point
// NOTE(brendan): Global window/image declarations.
// NOTE(Zach): Eventually these will be local to the graphics module and will
// be removed from here
extern List<FallingToken> *gFallingTokens;

// NOTE(brendan): does rendering for credits menu
void creditsMenuRender();

// NOTE(brendan): does rendering for main menu
void mainMenuRender();

// NOTE(Zach): performs the rendering that needs to be done when transitioning
// to setup from another state
void transitionSetupRender(void);

// NOTE(brendan): does rendering for setup
void setupRender();

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close_sdl();

// NOTE(Zach): visually drops the token into a cell and add it to the board
bool dropToken(Board b, Token tokenColour, int col);

// NOTE(brendan): delete stationary tokens from gFallingTokens
void deleteStillToken(FallingToken *fallingToken);
#endif // GRAPHICS_H
