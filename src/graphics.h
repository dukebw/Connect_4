#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

// NOTE(brendan): Global window/image declarations.
// NOTE(Zach): Eventually these will be local to the graphics module and will
// be removed from here
extern SDL_Window *gWindow;
extern SDL_Surface *gScreenSurface;
extern SDL_Surface *gConnect4Board;
extern SDL_Surface *gRedToken;
extern SDL_Surface *gBlueToken;
extern SDL_Surface *gBackground;

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close_sdl();

// TODO(Zach): Move this declaration into graphics.cpp
// NOTE(Zach): Loads individual image
SDL_Surface *loadSurface(std::string path);

// TODO(Zach): Move this declaration into graphics.cpp
// NOTE(Zach): blits the token to a cell in the grid
void blitToken(SDL_Surface *token, int row, int col);

// NOTE(Zach): visually drops the token into a cell and add it to the board
void dropToken(Token tokenColour, int col);

// NOTE(Zach): blit all tokens currently on the board to the window surface
void blitTokens(void);
