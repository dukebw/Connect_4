#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#define SCREEN_WIDTH  702
#define SCREEN_HEIGHT  602
#define TOKEN_WIDTH  100
#define TOKEN_HEIGHT  100
#define GRID_OFFSET_Y  1
#define GRID_OFFSET_X  1

// NOTE(brendan): Global window/image declarations.
extern SDL_Window *gWindow;
extern SDL_Surface *gScreenSurface;
extern SDL_Surface *gConnect4Board;
extern SDL_Surface *gRedToken;
extern SDL_Surface *gBackground;

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close();

// NOTE(Zach): Loads individual image
SDL_Surface *loadSurface(std::string path);

// NOTE(Zach): blits the token to a cell in the grid
void blitToken(SDL_Surface *token, int row, int col);
