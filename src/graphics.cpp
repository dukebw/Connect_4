#include "graphics.h"

#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
    , __LINE__, __FILE__)

// NOTE(brendan): a texture that knows its dimensions
struct TextureWrapper {
	SDL_Texture *texture;
	int width;
	int height;
};

// NOTE(brendan): tokens that are still in motion
struct FallingToken {
  int x;
  int y;
  int v;
  // NOTE(brendan): final position of this token
  int yFinal;
  bool isFalling;
  Token token;
};

// NOTE(brendan): convenient wrapper; used for highlighting
struct TokenLocation {
  int row;
  int column;
  Token colour;
};

// NOTE(brendan): flag for tracking whether we need to render highlighted
// tokens or not
static bool gRenderHighlighted = false;
// NOTE(brendan): Global window/image declarations.
SDL_Window *gWindow = NULL;
TextureWrapper *gConnect4Board = NULL;
TextureWrapper *gRedToken = NULL;
TextureWrapper *gBlueToken = NULL;
TextureWrapper *gMainMenu = NULL;
TextureWrapper *gOnePlayerButton = NULL;
TextureWrapper *gTwoPlayerButton = NULL;
TextureWrapper *gMenuButton = NULL;
TextureWrapper *gGlow = NULL;
SDL_Renderer* gRenderer = NULL;
List<FallingToken> *gFallingTokens = NULL;
List<TokenLocation> *gHighlightedTokens = NULL;

// NOTE(brendan): does rendering for credits menu
// NOTE(Jean): wait until image for credit menu is complete
// void creditsMenuRender() {
// 	SDL_RenderClear(gRenderer);
// 	SDL_RenderPresent(gRenderer);
// }

void displayBoard() {
  // NOTE(Zach): determine the position for the board
  SDL_Rect DestR;
  DestR.x = GRID_OFFSET_X - 1;
  DestR.y = GRID_OFFSET_Y - 1;
  DestR.w = gConnect4Board->width;
  DestR.h = gConnect4Board->height;
  SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, &DestR );
}

void displaySetupTokens() {
  // NOTE(Zach): determine the position for the setup tokens
  SDL_Rect tokenRect;
  tokenRect.x = SETUP_CLICKY_TOKENS_OFFSET;
  tokenRect.y = GRID_OFFSET_Y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;

  //Render texture to screen
  SDL_RenderCopy( gRenderer, gRedToken->texture, NULL, &tokenRect ); 

  tokenRect.x = SCREEN_WIDTH - SETUP_BOTTOM_BUTTONS_OFFSET - TOKEN_WIDTH;
  tokenRect.y = GRID_OFFSET_Y;
  tokenRect.w = TOKEN_WIDTH;
  tokenRect.h = TOKEN_HEIGHT;

  //Render texture to screen
  SDL_RenderCopy( gRenderer, gBlueToken->texture, NULL, &tokenRect ); 
}

// NOTE(Zach): display the main menu
void displayMainMenu(void)
{
  SDL_RenderCopy(gRenderer, gMainMenu->texture, NULL, NULL); 
}

// NOTE(brendan): does rendering for main menu
void mainMenuRender() {
	displayMainMenu();
	SDL_RenderPresent(gRenderer);
}

// NOTE(Zach): highlight a token at (row, col)
static void highlightToken(TokenLocation *tokenToHighlight) {
	// NOTE(Zach): do not delete
	// un-condition this out to add highlighting instead or in addition
	// to the glow
	// NOTE(Zach): highlight
	SDL_Rect fillRect = {GRID_OFFSET_X + TOKEN_WIDTH*tokenToHighlight->column,
								GRID_OFFSET_Y + TOKEN_HEIGHT * tokenToHighlight->row,
								TOKEN_WIDTH,
								TOKEN_HEIGHT};
  // NOTE(brendan): redraw token first -- so blending doesn't become whiter
  // and whiter if we highlight the same token repeatedly.
  TextureWrapper *tokenColour = 
    (tokenToHighlight->colour == RED) ?  gRedToken : gBlueToken;
  SDL_RenderCopy(gRenderer, tokenColour->texture, NULL, &fillRect);

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x66);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

	SDL_RenderFillRect(gRenderer, &fillRect);
	SDL_SetRenderDrawColor( gRenderer, 128, 128, 128, 0xFF );
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
	displayBoard();
//	SDL_RenderPresent(gRenderer);

	SDL_RenderCopy(gRenderer, gGlow->texture, NULL, &fillRect);
	//SDL_RenderPresent(gRenderer);
}

// Positions and renders the buttons to be used 
// in the setup game mode
void transitionSetupRender(void)
{
	SDL_Rect destRect;

	SDL_RenderClear(gRenderer);
	displaySetupTokens();

	// NOTE(Zach): Place the Menu Button
	destRect.x = SCREEN_WIDTH - gMenuButton->width - SETUP_BOTTOM_BUTTONS_OFFSET;
	destRect.y = SCREEN_HEIGHT - gMenuButton->height - 
    SETUP_BOTTOM_BUTTONS_OFFSET;
	destRect.w = gMenuButton->width;
	destRect.h = gMenuButton->height;
	SDL_RenderCopy( gRenderer, gMenuButton->texture, NULL, &destRect ); 

	// NOTE(Zach): Place the One Player Button
	destRect.x = SETUP_BOTTOM_BUTTONS_OFFSET;
	destRect.y = SCREEN_HEIGHT - gOnePlayerButton->height - 
    SETUP_BOTTOM_BUTTONS_OFFSET;
	destRect.w = gOnePlayerButton->width;
	destRect.h = gOnePlayerButton->height;
	SDL_RenderCopy( gRenderer, gOnePlayerButton->texture, NULL, &destRect ); 

	// NOTE(Zach): Place the Two Player Button
	destRect.x = 2*SETUP_BOTTOM_BUTTONS_OFFSET + gOnePlayerButton->width;
	destRect.y = SCREEN_HEIGHT - gTwoPlayerButton->height - 
    SETUP_BOTTOM_BUTTONS_OFFSET;
	destRect.w = gTwoPlayerButton->width;
	destRect.h = gTwoPlayerButton->height;
	SDL_RenderCopy( gRenderer, gTwoPlayerButton->texture, NULL, &destRect ); 
}

// NOTE(brendan): does rendering for setup
void setupRender() {
	displayBoard();
  // NOTE(brendan): only render highlighted when we need to 
  // TODO(brendan): (bug: board hides the aura)
  if(gRenderHighlighted) {
    List<TokenLocation>::traverseList(highlightToken, gHighlightedTokens);
    gRenderHighlighted = false;
  }
	SDL_RenderPresent(gRenderer);
}

// NOTE(brendan): free myTexture's memory
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
    gWindow = SDL_CreateWindow("Connect 4", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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

// NOTE(Zach): Loads bitmaps
static TextureWrapper *loadTexture(std::string path) {
  TextureWrapper *loadedTexture;
  // NOTE(Zach): The final optimized image
  SDL_Texture *newTexture = NULL;

  // NOTE(Zach): Load image at specified path
  SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str()); //use a c string

  SDL_Rect scaleRect;
  scaleRect.x = 0;
  scaleRect.y = 0;
  scaleRect.w = loadedSurface->w*(SCALE);
  scaleRect.h = loadedSurface->h*(SCALE);
  SDL_Surface *scaledSurface = 
    SDL_CreateRGBSurface(0, scaleRect.w, scaleRect.h, 32, 0, 0, 0, 0);
  SDL_BlitScaled(loadedSurface, NULL, scaledSurface, &scaleRect);
  /* SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect); */
  if(scaledSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), 
        SDL_GetError());
  } else {
    // Color key image
    SDL_SetColorKey( scaledSurface, SDL_TRUE, 
        SDL_MapRGB( scaledSurface->format, 0xFF, 0xFF, 0xFF));

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, scaledSurface);
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
        loadedTexture->width = scaledSurface->w;
        loadedTexture->height = scaledSurface->h;
      }
    }

    // NOTE(Zach): Get rid of old loaded surface
    SDL_FreeSurface(scaledSurface);
  }

  return loadedTexture;
}

bool loadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

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

  // NOTE(Zach): Load the main menu
  gMainMenu = loadTexture("../misc/gameTitlePage.bmp");
  if (gBlueToken == NULL) {
    printf("Failed to load the main menu!\n");
    success = false;
  }
  // NOTE(Zach): Load the One Player button graphic
  gOnePlayerButton = loadTexture("../misc/1PlayerSetup.bmp");
  if (gOnePlayerButton == NULL) {
    printf("Failed to load the One Player button graphic!\n");
    success = false;
  }

  // NOTE(Zach): Load the Two Player button graphic
  gTwoPlayerButton = loadTexture("../misc/2PlayerSetup.bmp");
  if (gTwoPlayerButton == NULL) {
    printf("Failed to load the Two Player button graphic!\n");
    success = false;
  }

  // NOTE(Zach): Load the Menu button graphic
  gMenuButton = loadTexture("../misc/menuSetup.bmp");
  if (gMenuButton == NULL) {
    printf("Failed to load the Menu button graphic!\n");
    success = false;
  }

  // NOTE(Zach): Load the glow graphic
  gGlow = loadTexture("../misc/glow.bmp");
  if (gGlow == NULL) {
    printf("Failed to load the glow graphic!\n");
    success = false;
  }

  return success;
}

void close_sdl() {
  // NOTE(brendan): de-allocate surface
  freeTexture(gConnect4Board);
  freeTexture(gRedToken);
  freeTexture(gBlueToken);
  freeTexture(gMainMenu);
  freeTexture(gOnePlayerButton);
  freeTexture(gTwoPlayerButton);
  freeTexture(gMenuButton);
	freeTexture(gGlow);

  gConnect4Board = NULL;
  gRedToken = NULL;
  gBlueToken = NULL;
  gMainMenu = NULL;
  gOnePlayerButton = NULL;
  gTwoPlayerButton = NULL;
  gMenuButton = NULL;
  gGlow = NULL;

  // NOTE(brendan): Destroy window
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  // NOTE(brendan): Quit SDL subsystems
  SDL_Quit();
}

// NOTE(brendan): returns true if the two tokens are in the same column,
// false otherwise
static bool 
compareXPosition(FallingToken *listItem, FallingToken *item) {
  return listItem->x == item->x;
}

// NOTE(Zach): visually drops the token into a cell if drop is valid
bool dropToken(Board b, Token tokenColour, int col) {
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
    return false;
  }

  FallingToken *newToken = (FallingToken *)malloc(sizeof(FallingToken));

  // NOTE(Zach): Initial position of the token
  newToken->x = GRID_OFFSET_X + TOKEN_WIDTH * col;
  newToken->y = GRID_OFFSET_Y;
  // NOTE(brendan): check if there is another falling token that is above
  // the top of the board; if so drop this next token ABOVE that token
  FallingToken *currentHighest = 
    List<FallingToken>::reduceList(compareXPosition, newToken, gFallingTokens);
  if(currentHighest != NULL) { 
    if(newToken->y + TOKEN_HEIGHT > currentHighest->y) {
      newToken->y = currentHighest->y - TOKEN_HEIGHT;
    }
  }

  // NOTE(Zach): Velocity of token
  newToken->v = 0;
  // NOTE(Zach): Final height of the token
  newToken->yFinal = GRID_OFFSET_Y + row * TOKEN_HEIGHT;
  newToken->isFalling = true;
  newToken->token = tokenColour;

  gFallingTokens = List<FallingToken>::addToList(newToken, gFallingTokens);
  return true;
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
void deleteStillToken(FallingToken *fallingToken) {
  if(fallingToken->isFalling == false) {
  		// NOTE(Zach): This call is for testing highlighting only
    gFallingTokens = List<FallingToken>::deleteFromList(fallingToken, 
        gFallingTokens);
  }
}

// NOTE(brendan): free token
static void freeTokenLocation(TokenLocation *tokenLocation) {
  free(tokenLocation);
}

// NOTE(brendan): sets the list of highlighted tokens, freeing the old one
void setHighlightedTokenList(List<TokenLocation> *highlightedTokenList) {
  // NOTE(brendan): get rid of old list
  List<TokenLocation>::traverseList(freeTokenLocation, gHighlightedTokens);
  gHighlightedTokens = highlightedTokenList;
  gRenderHighlighted = true;
}
