#ifndef GRAPHICS_H // NOTE(Zach): This is an include guard
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include "board.h"

#define SCREEN_WIDTH  702
#define SCREEN_HEIGHT  602
#define TOKEN_WIDTH  100
#define TOKEN_HEIGHT  100
#define GRID_OFFSET_Y  1
#define GRID_OFFSET_X  1
#define GRID_WIDTH 700
#define GRID_HEIGHT 600

struct TextureWrapper {
	SDL_Texture *texture;
	int width;
	int height;
};

struct FallingToken {
  int x;
  int y;
  int v;
  // NOTE(brendan): final position of this token
  int yFinal;
  bool isFalling;
  Token token;
};

void drawFallingToken(FallingToken *token);
void clearFallingToken(FallingToken *fallingToken);
void updateFallingToken(FallingToken *fallingToken, float dt);

// NOTE(brendan): Global window/image declarations.
// NOTE(Zach): Eventually these will be local to the graphics module and will
// be removed from here
extern SDL_Window *gWindow;
extern TextureWrapper *gConnect4Board;
extern TextureWrapper *gRedToken;
extern TextureWrapper *gBlueToken;
extern TextureWrapper *gBackground;
extern SDL_Renderer* gRenderer;

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close_sdl();

// TODO(Zach): Move this declaration into graphics.cpp
// NOTE(Zach): Loads individual image
SDL_Surface *loadSurface(std::string path);

// TODO(Zach): Move this declaration into graphics.cpp (maybe not this one)
// NOTE(Zach): blits the token to a cell in the grid
void blitToken(SDL_Surface *token, int row, int col);

// NOTE(Zach): visually drops the token into a cell and add it to the board
void dropToken(Board b, Token tokenColour, int col);

// NOTE(Zach): blit all tokens currently on the board to the window surface
void blitTokens(Board b);

#endif // GRAPHICS_H
