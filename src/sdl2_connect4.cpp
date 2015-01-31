/* Source code by team struct by_lightning{}; */

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 702;
const int SCREEN_HEIGHT = 602;

// TODO(brendan): Move these functions declarations to header file?
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

// NOTE(brendan): Global window/image declarations.
SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gConnect4Board = NULL;
SDL_Surface *gRedToken = NULL;

bool init() {
  // NOTE(brendan): Initialization flag.
  bool success = true;

  // NOTE(brendan): Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    // TODO(brendan): Diagnostics
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } else {
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
    } else {
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			} else {
				//NOTE(Zach): Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
    }
  }
  return success;
}

bool loadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

	// NOTE(brendan): Load splash image
	//gConnect4Board = loadSurface( "../misc/connect4_board.bmp" );
	gConnect4Board = loadSurface("../misc/simple_grid_on_white.bmp");
	if (gConnect4Board == NULL) {
		printf( "Failed to load board!\n" );
		success = false;
	}

	//gRedToken = loadSurface("../misc/red_token.bmp");
	gRedToken = loadSurface("../misc/red_token.png");
	if (gRedToken == NULL) {
		printf("Failed to load red token!\n");
		success = false;
	}

	return success;
}

void close() {
  // NOTE(brendan): de-allocate surface
  SDL_FreeSurface(gConnect4Board);
  SDL_FreeSurface(gRedToken);

	gConnect4Board = NULL;
	gRedToken = NULL;

	// NOTE(brendan): Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// NOTE(brendan): Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

/*
// NOTE(Zach): Loads bitmaps
SDL_Surface *loadSurface(std::string path)
{
	// NOTE(Zach): The final optimized image
	SDL_Surface *optimizedSurface = NULL;

	// NOTE(Zach): Load image at specified path
	SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str()); //use a c string
	if(loadedSurface == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	} else {
		// NOTE(Zach): Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
		if(optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// NOTE(Zach): Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}*/

// NOTE(Zach): Loads more than just bitmaps
// TODO(Zach): Look more into exactly what it can load -- atm loading png and bmp with it
SDL_Surface *loadSurface(std::string path)
{
	// NOTE(Zach): The final optimized image
	SDL_Surface *optimizedSurface = NULL;

	// NOTE(Zach): Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		// NOTE(Zach): Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
		if (optimizedSurface == NULL)	{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// NOTE(Zach): Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

void blitToken(SDL_Surface *token, int row, int col)
{
		// NOTE(Zach): determine the position for the token
		SDL_Rect tokenRect;
		tokenRect.x = 1 + 100 * col;
		tokenRect.y = 1 + 100 * row;
		tokenRect.w = 100;
		tokenRect.h = 100;

		// Note(Zach): blit the token to the desired position
      SDL_BlitSurface(token, NULL, gScreenSurface, &tokenRect);  
		return;
}
  
int main(int argc, char *argv[]) {
	// NOTE(brendan): Start up SDL and create window.
	if (!init()) {
		// TODO(brendan): Diagnostics
		printf("Failed to initialize!\n");
	} else {
		if (!loadMedia()) {
      	printf("Failed to load media!\n");
		} else {
			// NOTE(Zach): Main loop flag
			bool quit = false;

			// NOTE(Zach): Event handler
			SDL_Event e;

			// NOTE(Zach): While application is running
			while(!quit) {
				// NOTE(Zach): Wait for an event to occur
				SDL_WaitEvent(&e);

				// NOTE(Zach): User requests quit
				if (e.type == SDL_QUIT) {
					quit = true;
				}

				// NOTE(brendan): Apply the image
				SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);

				// NOTE(Zach): blit the red token  ***Transparency not working*** 
				SDL_BlitSurface(gRedToken, NULL, gScreenSurface, NULL);
				// NOTE(Zach): blit red token to 2nd position along main diagonal
				blitToken(gRedToken, 1, 1);
				// NOTE(Zach): blit red token to bottom right cell
				blitToken(gRedToken, 5, 6);

				// NOTE(brendan): Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	// NOTE(brendan): Free resources and close SDL.
	// TODO(brendan): Figure out how much of close() is necessary?
	close();

	return 0;
}
