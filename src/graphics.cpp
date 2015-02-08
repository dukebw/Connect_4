#include "graphics.h"
#include "board.h"

// NOTE(brendan): Global window/image declarations.
SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gConnect4Board = NULL;
SDL_Surface *gRedToken = NULL;
SDL_Surface *gBlueToken = NULL;
SDL_Surface *gBackground = NULL;

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
      printf("Window could not be created! SDL_Error: %s\n", 
		SDL_GetError());
    } else {
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			} else {
				// NOTE(Zach): Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
    }
  }
  return success;
}

bool loadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

	// NOTE(Zach): Load the background
	gBackground = loadSurface("../misc/white_background.bmp");
	if (gBackground == NULL) {
		printf("Failed to load background!\n");
		success = false;
	}

	// NOTE(brendan): Load splash image
	gConnect4Board = loadSurface("../misc/board.bmp");
	if (gConnect4Board == NULL) {
		printf( "Failed to load board!\n" );
		success = false;
	}
	// NOTE(Zach): make all white pixels transparent
	SDL_SetColorKey(gConnect4Board, SDL_TRUE, SDL_MapRGB(gBackground->format, 255, 255, 255));

	// NOTE(Zach): Load the red token
	gRedToken = loadSurface("../misc/red_token.bmp");
	//gRedToken = loadSurface("../misc/red_token.png");
	if (gRedToken == NULL) {
		printf("Failed to load red token!\n");
		success = false;
	}
	// NOTE(Zach): make all white pixels transparent
	SDL_SetColorKey(gRedToken, SDL_TRUE, SDL_MapRGB(gRedToken->format, 255, 255, 255));

	// NOTE(Zach): Load the blue token
	gBlueToken = loadSurface("../misc/blue_token.bmp");
	if (gBlueToken == NULL) {
		printf("Failed to load blue token!\n");
		success = false;
	}
	// NOTE(Zach): make all white pixels transparent
	SDL_SetColorKey(gBlueToken, SDL_TRUE, SDL_MapRGB(gBlueToken->format, 255, 255, 255));

	return success;
}

void close_sdl() {
  // NOTE(brendan): de-allocate surface
  SDL_FreeSurface(gConnect4Board);
  SDL_FreeSurface(gRedToken);
  SDL_FreeSurface(gBlueToken);
  SDL_FreeSurface(gBackground);

	gConnect4Board = NULL;
	gRedToken = NULL;
	gBlueToken = NULL;
	gBackground = NULL;

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

// NOTE(Zach): blits the token to a cell in the grid
void blitToken(SDL_Surface *token, int row, int col)
{
		// NOTE(Zach): determine the position for the token
		SDL_Rect tokenRect;
		tokenRect.x = GRID_OFFSET_X + TOKEN_WIDTH * col;
		tokenRect.y = GRID_OFFSET_Y + TOKEN_HEIGHT * row;
		tokenRect.w = TOKEN_WIDTH;
		tokenRect.h = TOKEN_HEIGHT;

		// Note(Zach): blit the token to the desired position
      SDL_BlitSurface(token, NULL, gScreenSurface, &tokenRect);  
		return;
}

// NOTE(Zach): visually drops the token into a cell and add it to the board
void dropToken(Token tokenColour, int col)
{
	SDL_Surface *token;
	if (tokenColour == RED) token = gRedToken;
	else if (tokenColour == BLUE) token = gBlueToken;

	// NOTE(Zach): Find the row where the token should land
	int row = insertPosition(col);
	// NOTE(Zach): Check if the column was full
	if (row == -1) return;

	// NOTE(Zach): Destination rectangle to blit to
	SDL_Rect DestR;

	// NOTE(Zach): Initial position of the token
	DestR.x = GRID_OFFSET_X + TOKEN_WIDTH * col;
	DestR.y = GRID_OFFSET_Y;
	DestR.w = TOKEN_WIDTH;
	DestR.h = TOKEN_HEIGHT;

	// NOTE(Zach): Final height of the token
	int minHeight = GRID_OFFSET_Y + row * TOKEN_HEIGHT;

	// NOTE(Zach): Velocity of token
	int v = 0;
	for (;;) {
		// NOTE(Zach): df = v*t + di
		DestR.y += v;
		// NOTE(Zach): vf = a*t + vi
		v += 5;
		// NOTE(Zach): Remove energy when token hits a surface and the token
		// is moving down
		if (DestR.y >= minHeight && v > 0) {
			v = -v/3;
			DestR.y = minHeight;
		}
		// NOTE(Zach): Eliminate small velocity noise to let the token settle
		if (v <= 5 && v >= -5 && DestR.y >= minHeight) {
			DestR.y = minHeight;
			// NOTE(Zach): blit background, tokens, falling token and board;
			// then update the window surface
			SDL_BlitSurface(gBackground, NULL, gScreenSurface, NULL);
			blitTokens();
			SDL_BlitSurface(token, NULL, gScreenSurface, &DestR);
			SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);
			break;
		}
		// NOTE(Zach): blit background, tokens, falling token and board;
		// then update the window surface
		SDL_BlitSurface(gBackground, NULL, gScreenSurface, NULL);
		blitTokens();
		SDL_BlitSurface(token, NULL, gScreenSurface, &DestR);
		SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);
		SDL_UpdateWindowSurface(gWindow);
		// NOTE(Zach): delay 32 milliseconds to produce ~30fps,
		// using 32 instead of 33 to give some time for system delays
		SDL_Delay(32);
	}
	// NOTE(Zach): Insert the token into the board
	if (token == gRedToken) {
		insertToken(RED, col);
	} else if (token == gBlueToken) {
		insertToken(BLUE, col);
	}
	return;
}

// NOTE(Zach): blit all tokens currently on the board to the window surface
void blitTokens(void)
{
	int row, col;

	// NOTE(Zach): Loop through all the cells of the board and if there is a
	// token present blit it to the window surface
	for (row = 0; row < NUM_ROWS; row++) {
		for (col = 0; col < NUM_COLS; col++) {
			if (checkCell(row, col) == RED) {
				blitToken(gRedToken, row, col);
			} else if (checkCell(row, col) == BLUE) {
				blitToken(gBlueToken, row, col);
			}
		}
	}
}
