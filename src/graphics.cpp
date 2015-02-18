#include "graphics.h"

#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
    , __LINE__, __FILE__)

#if 0
struct TextureWrapper {
	SDL_Texture *texture;
	int width;
	int height;
};
#endif

#if 0
struct FallingToken {
  int x;
  int y;
  int v;
  Token token;
};
#endif

// NOTE(brendan): Global window/image declarations.
SDL_Window *gWindow = NULL;
TextureWrapper *gConnect4Board = NULL;
TextureWrapper *gRedToken = NULL;
TextureWrapper *gBlueToken = NULL;
TextureWrapper *gBackground = NULL;
SDL_Renderer* gRenderer = NULL;
Node<FallingToken> *gFallingTokens = NULL;

static TextureWrapper *loadTexture(std::string path);
bool compareXPosition(FallingToken *listItem, FallingToken *item);

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
        SDL_SetRenderDrawColor( gRenderer, 128, 128, 128, 0xFF );
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

// TODO(brendan): Do we still need this? (Due to drawFallingToken)
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
void dropToken(Board b, Token tokenColour, int col) {
  TextureWrapper *token;
  if (tokenColour == RED) {
    token = gRedToken;
  }
  else if (tokenColour == BLUE) {
    token = gBlueToken;
  }

  // NOTE(Zach): Find the row where the token should land
  int row = board_dropPosition(b, col);
  // NOTE(Zach): Check if the column was full
  if (row == -1) {
    return;
  }

  FallingToken *newToken = (FallingToken *)malloc(sizeof(FallingToken));

  // NOTE(Zach): Initial position of the token
  newToken->x = GRID_OFFSET_X + TOKEN_WIDTH * col;
  newToken->y = GRID_OFFSET_Y;
  FallingToken *currentHighest = reduceList(compareXPosition, newToken, gFallingTokens);
  if (currentHighest != NULL) { 
  		if (newToken->y + TOKEN_HEIGHT > currentHighest->y) {
			newToken->y = currentHighest->y - TOKEN_HEIGHT;
		}
  }


  // NOTE(Zach): Velocity of token
  newToken->v = 0;
  // NOTE(Zach): Final height of the token
  newToken->yFinal = GRID_OFFSET_Y + row * TOKEN_HEIGHT;
  newToken->isFalling = true;
  newToken->token = tokenColour;

  gFallingTokens = addToList(newToken, gFallingTokens);

  // NOTE(Zach): Insert the token into the board
  board_dropToken(b, tokenColour, col);
  return;
}

// NOTE(Zach): blit all tokens currently on the board to the window surface
void blitTokens(Board b) {
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

// NOTE(brendan): draw a falling token
void drawFallingToken(FallingToken *fallingToken) {
  TextureWrapper *tokenTexture;
  if(fallingToken->token == RED) {
    tokenTexture = gRedToken;
  }
  else {
    tokenTexture = gBlueToken;
  }

  // NOTE(Zach): determine the position for the fallingToken
  SDL_Rect tokenRect;
  tokenRect.x = fallingToken->x;
  tokenRect.y = fallingToken->y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;
 
	//Render texture to screen
	SDL_RenderCopy( gRenderer, tokenTexture->texture, NULL, &tokenRect ); 
}

void clearFallingToken(FallingToken *fallingToken) {
  // NOTE(Zach): determine the position for the fallingToken
  SDL_Rect tokenRect;
  tokenRect.x = fallingToken->x;
  tokenRect.y = fallingToken->y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;

  SDL_RenderFillRect(gRenderer, &tokenRect);
  return;
}

// TODO(Zach): make macros for constants
// NOTE(Zach): update position/velocity of falling token
void updateFallingToken(FallingToken *fallingToken, float dt) {
	#define ACCEL 5
	fallingToken->y += fallingToken->v * dt;
	fallingToken->v += ACCEL * dt;
	// NOTE(Zach): Remove energy when token hits a surface and the token
	// is moving down
  if((fallingToken->y >= fallingToken->yFinal) && 
      (fallingToken->v > 0)) {
    fallingToken->v = -fallingToken->v/3;
    fallingToken->y = fallingToken->yFinal;
	}
	// NOTE(Zach): Eliminate small velocity noise to let the token settle
	if((fallingToken->v <= 5) && 
      (fallingToken->v >= -5) && 
      (fallingToken->y >= fallingToken->yFinal)) {
		fallingToken->y = fallingToken->yFinal;
    fallingToken->v = 0;
		fallingToken->isFalling = false;
	}
	#undef ACCEL
}

// NOTE(brendan): delete stationary tokens from gFallingTokens
// TODO(brendan): pass list as parameter?
void deleteStillToken(FallingToken *fallingToken) {
  if(fallingToken->isFalling == false) {
    gFallingTokens = deleteFromList(fallingToken, gFallingTokens);
  }
}

bool compareXPosition(FallingToken *listItem, FallingToken *item) {
	return listItem->x == item->x;
}

void displayBoard(void)
{
	// NOTE(Zach): determine the position for the board
	SDL_Rect DestR;
	DestR.x = GRID_OFFSET_X - 1;
	DestR.y = GRID_OFFSET_Y - 1;
	DestR.w = gConnect4Board->width;
	DestR.h = gConnect4Board->height;
	SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, &DestR );
}

void displaySetupTokens(void)
{
  // NOTE(Zach): determine the position for the setup tokens
  SDL_Rect tokenRect;
  tokenRect.x = 25;
  tokenRect.y = GRID_OFFSET_Y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;
 
	//Render texture to screen
	SDL_RenderCopy( gRenderer, gRedToken->texture, NULL, &tokenRect ); 

  tokenRect.x = SCREEN_WIDTH - 125;
  tokenRect.y = GRID_OFFSET_Y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;
 
	//Render texture to screen
	SDL_RenderCopy( gRenderer, gBlueToken->texture, NULL, &tokenRect ); 
}

void displayMainMenu(void) {
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);
}
void displayCreditsMenu(void) {
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);
}
