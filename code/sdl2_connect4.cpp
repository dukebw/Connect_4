/* Source code by team struct by_lightning{}; */

#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// TODO(brendan): Move these functions declarations to header file?
// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close();

// NOTE(brendan): Global window/image declarations.
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gConnect4Board = NULL;

bool init() {
  // NOTE(brendan): Initialization flag.
  bool success = true;

  // NOTE(brendan): Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    // TODO(brendan): Diagnostics
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }
  else {
    // NOTE(brendan): Create window
    gWindow = SDL_CreateWindow("Connect 4", 
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH,
                               SCREEN_HEIGHT,
                               0);
    if(gWindow == NULL) {
      // NOTE(brendan): get window surface
      printf("Window could not be created! SDL_Error: %s\n", 
          SDL_GetError());
    }
    else {
      // NOTE(brendan): Get window surface
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }
  return success;
}

bool loadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

	// NOTE(brendan): Load splash image
	gConnect4Board = SDL_LoadBMP( "../misc/connect4_board.bmp" );
	if(gConnect4Board == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", 
        "connect4_board.bmp", SDL_GetError());
		success = false;
	}

	return success;
}

void close() {
  // NOTE(brendan): de-allocate surface
  SDL_FreeSurface(gConnect4Board);

	gConnect4Board = NULL;

	// NOTE(brendan): Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// NOTE(brendan): Quit SDL subsystems
	SDL_Quit();
}
  
int main(int argc, char* args[]) {
  // NOTE(brendan): Start up SDL and create window.
  if(!init()) {
    // TODO(brendan): Diagnostics
    printf("Failed to initialize!\n");
  }
  else {
    if(!loadMedia()) {
      printf("Failed to load media!\n");
    }
    else {
      // NOTE(brendan): Apply the image
      SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);

      // NOTE(brendan): Update the surface
      SDL_UpdateWindowSurface(gWindow);
      SDL_Delay(2000);
    }
  }

  // NOTE(brendan): Free resources and close SDL.
  // TODO(brendan): Figure out how much of close() is necessary?
  close();

  return 0;
}
