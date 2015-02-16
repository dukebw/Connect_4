#include "graphics.h"
#include "board.h"

#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
    , __LINE__, __FILE__)

#if 0
struct TextureWrapper {
	SDL_Texture *texture;
	int width;
	int height;
};
#endif

// NOTE(brendan): Global window/image declarations.
SDL_Window *gWindow = NULL;
TextureWrapper *gConnect4Board = NULL;
TextureWrapper *gRedToken = NULL;
TextureWrapper *gBlueToken = NULL;
TextureWrapper *gBackground = NULL;
SDL_Renderer* gRenderer = NULL;

static TextureWrapper *loadTexture(std::string path);

static void freeTexture(TextureWrapper *myTexture) {
  if(myTexture != NULL) {
    if(myTexture->texture != NULL) {
      SDL_DestroyTexture(myTexture->texture);
      myTexture->texture = NULL;
    }
    free(myTexture); 
  }
}

bool init() {
  // NOTE(brendan): Initialization flag.
  bool success = true;

  // NOTE(brendan): Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    // TODO(brendan): Diagnostics
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } 
  else {
    // NOTE(brendan): Create window
    gWindow = SDL_CreateWindow("Connect 4", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(gWindow == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", 
          SDL_GetError());
    } 
    else {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, 
          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
      if( gRenderer == NULL ) {
        printf( "Renderer could not be created! SDL Error: %s\n", 
            SDL_GetError() );
        success = false;
      }
      else {
        //Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
      }
    }
  }
  return success;
}

bool loadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

  // NOTE(Zach): Load the background
  gBackground = loadTexture("../misc/white_background.bmp");
  if (gBackground == NULL) {
    printf("Failed to load background!\n");
    success = false;
  }

  // NOTE(brendan): Load splash image
  gConnect4Board = loadTexture("../misc/board.bmp");
  if (gConnect4Board == NULL) {
    printf( "Failed to load board!\n" );
    success = false;
  }

  // NOTE(Zach): Load the red token
  gRedToken = loadTexture("../misc/red_token.bmp");
  //gRedToken = loadTexture("../misc/red_token.png");
  if (gRedToken == NULL) {
    printf("Failed to load red token!\n");
    success = false;
  }
  // NOTE(Zach): Load the blue token
  gBlueToken = loadTexture("../misc/blue_token.bmp");
  if (gBlueToken == NULL) {
    printf("Failed to load blue token!\n");
    success = false;
  }

  return success;
}

void close_sdl() {
  // NOTE(brendan): de-allocate surface
  freeTexture(gConnect4Board);
  freeTexture(gRedToken);
  freeTexture(gBlueToken);
  freeTexture(gBackground);

  gConnect4Board = NULL;
  gRedToken = NULL;
  gBlueToken = NULL;
  gBackground = NULL;

  // NOTE(brendan): Destroy window
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  // NOTE(brendan): Quit SDL subsystems
  SDL_Quit();
}

// NOTE(Zach): Loads bitmaps
static TextureWrapper *loadTexture(std::string path)
{
	TextureWrapper *loadedTexture;
  // NOTE(Zach): The final optimized image
  SDL_Texture *newTexture = NULL;

  // NOTE(Zach): Load image at specified path
  SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str()); //use a c string
  if(loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), 
        SDL_GetError());
  } else {
		// Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, 
        SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", 
          path.c_str(),SDL_GetError());
		} else {
			loadedTexture = (TextureWrapper *)malloc(sizeof(TextureWrapper));
			if (loadedTexture == NULL) {
				printf("Unable to allocate the TextureWrapper structure for %s!\n", 
            path.c_str());
			} else {
				loadedTexture->texture = newTexture;
				loadedTexture->width = loadedSurface->w;
				loadedTexture->height = loadedSurface->h;
			}
		}

    // NOTE(Zach): Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  return loadedTexture;
}

// NOTE(Zach): blits the token to a cell in the grid
void blitToken(TextureWrapper *token, int row, int col)
{
  // NOTE(Zach): determine the position for the token
  SDL_Rect tokenRect;
  tokenRect.x = GRID_OFFSET_X + TOKEN_WIDTH * col;
  tokenRect.y = GRID_OFFSET_Y + TOKEN_HEIGHT * row;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;
 
	//Render texture to screen
	SDL_RenderCopy( gRenderer, token->texture, NULL, &tokenRect ); 

  return;
}

// NOTE(Zach): visually drops the token into a cell and add it to the Board, b
void dropToken(Board b, Token tokenColour, int col)
{
  TextureWrapper *token;
  if (tokenColour == RED) token = gRedToken;
  else if (tokenColour == BLUE) token = gBlueToken;

  // NOTE(Zach): Find the row where the token should land
  int row = board_dropPosition(b, col);
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
		//Clear screen
		SDL_RenderClear( gRenderer );
      blitTokens(b);
		SDL_RenderCopy( gRenderer, token->texture, NULL, &DestR );
		SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
		SDL_RenderPresent(gRenderer);
      break;
    }
    // NOTE(Zach): blit background, tokens, falling token and board;
	 // then update the window surface
	 SDL_RenderClear( gRenderer );
	 blitTokens(b);
	 SDL_RenderCopy( gRenderer, token->texture, NULL, &DestR );
	 SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
	 SDL_RenderPresent(gRenderer);
    // NOTE(Zach): delay 32 milliseconds to produce ~30fps,
    // using 32 instead of 33 to give some time for system delays
    //SDL_Delay(32);
  }
  // NOTE(Zach): Insert the token into the board
  board_dropToken(b, tokenColour, col);
  return;
}

// NOTE(Zach): blit all tokens currently on the board to the window surface
void blitTokens(Board b)
{
  int row, col;

  // NOTE(Zach): Loop through all the cells of the board and if there is a
  // token present blit it to the window surface
  for (row = 0; row < NUM_ROWS; row++) {
    for (col = 0; col < NUM_COLS; col++) {
      if (board_checkCell(b, row, col) == RED) {
        blitToken(gRedToken, row, col);
      } else if (board_checkCell(b, row, col) == BLUE) {
        blitToken(gBlueToken, row, col);
      }
    }
  }
}
