/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#include "graphics.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string>

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

// NOTE(brendan): flag for tracking whether we need to render highlighted
// tokens or not
// NOTE(brendan): Global window/image declarations.
static SDL_Window *gWindow = NULL;
static TextureWrapper *gConnect4Board = NULL;
static TextureWrapper *gRedToken = NULL;
static TextureWrapper *gBlueToken = NULL;
static TextureWrapper *gMainMenu = NULL;
static TextureWrapper *gGlow = NULL;
static TextureWrapper *gInvalidMessage = NULL;
static TextureWrapper *gInvalidTokenMessage = NULL;
static TextureWrapper *gDrawGameMessage = NULL;
static SDL_Renderer* gRenderer = NULL;
static TextureWrapper *gCreditScreen = NULL;
static TextureWrapper *gSetupScreen = NULL;
static TextureWrapper *gTwoPlayerScreen = NULL;
static TextureWrapper *gStatusBlueWon = NULL;
static TextureWrapper *gStatusRedWon = NULL;
static TextureWrapper *gStatusDraw = NULL;
static TextureWrapper *gStatusInProgress = NULL;
static TextureWrapper *gRefresh = NULL;

static List<TokenLocation> *gHighlightedTokens = NULL;
List<FallingToken> *gFallingTokens = NULL;

void resetGraphicsState(GraphicsState *graphicsState)
{
	graphicsState->renderInvalidMessage = false;
	graphicsState->renderInvalidTokenMessage = false;
	graphicsState->renderHighlighted = false;
	graphicsState->renderIndicatorToken = true;

  graphicsState->renderStatusInProgress = true;
  graphicsState->renderStatusDrawGame = false;
  graphicsState->renderStatusBlueWon = false;
  graphicsState->renderStatusRedWon = false;

}

// NOTE(brendan): does rendering for credits menu
void creditsRender(GraphicsState *graphicsState) 
{
  SDL_RenderCopy(gRenderer, gCreditScreen->texture, NULL, NULL); 
	SDL_RenderPresent(gRenderer);
}

void renderIndicatorToken(TokenLocation *indicatorToken)
{
	if (indicatorToken->row == -1 || indicatorToken->column == -1) return;
	TextureWrapper *token = NULL;
	if (indicatorToken->colour == RED) token = gRedToken;
	else if (indicatorToken->colour == BLUE) token = gBlueToken;
  // TODO(brendan): did this to get rid of syntastic error; clean up above?
  // Specifically: didn't handle indicatorToken->colour not BLUE or RED case
  if (token) {
    SDL_Rect destRect = {GRID_OFFSET_X + TOKEN_WIDTH*indicatorToken->column,
      GRID_OFFSET_Y + TOKEN_HEIGHT * indicatorToken->row,
      TOKEN_WIDTH,
      TOKEN_HEIGHT};
    SDL_SetTextureColorMod(token->texture, 153, 153, 153);
    SDL_RenderCopy(gRenderer, token->texture, NULL, &destRect);
    SDL_SetTextureColorMod(token->texture, 255, 255, 255);
  }
}

void displayBoard() 
{
  // NOTE(Zach): determine the position for the board
  SDL_Rect DestR;
  DestR.x = GRID_OFFSET_X - 1;
  DestR.y = GRID_OFFSET_Y - 1;
  DestR.w = gConnect4Board->width;
  DestR.h = gConnect4Board->height;
  SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, &DestR );
}

void displaySetupTokens() 
{
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
void mainMenuRender(GraphicsState *graphicsState) 
{
	displayMainMenu();
	SDL_RenderPresent(gRenderer);
}

// NOTE(Zach): highlight a token at (row, col)
static void highlightToken(TokenLocation *tokenToHighlight) 
{
	// NOTE(Zach): do not delete
	// un-condition this out to add highlighting instead or in addition
	// to the glow
	// NOTE(Zach): highlight
	SDL_Rect fillRect = {GRID_OFFSET_X + TOKEN_WIDTH*tokenToHighlight->column,
								GRID_OFFSET_Y + TOKEN_HEIGHT * tokenToHighlight->row,
								TOKEN_WIDTH,
								TOKEN_HEIGHT};

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x66);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

	SDL_RenderFillRect(gRenderer, &fillRect);
	SDL_SetRenderDrawColor( gRenderer, 128, 128, 128, 0xFF );
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
	//displayBoard();
//	SDL_RenderPresent(gRenderer);

	SDL_RenderCopy(gRenderer, gGlow->texture, NULL, &fillRect);
	//SDL_RenderPresent(gRenderer);
}

// NOTE(brendan): places the image at (x, y)
inline void
placeImage(SDL_Texture *image, int x, int y, int width, int height) 
{
	SDL_Rect destRect;
  destRect.x = x;
  destRect.y = y;
  destRect.w = width;
  destRect.h = height;
	SDL_RenderCopy( gRenderer, image, NULL, &destRect ); 
}

void setupRender(GraphicsState *graphicsState)
{
	SDL_RenderClear(gRenderer);
	displaySetupTokens();

  // Place setup Screen
  placeImage(gSetupScreen->texture, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);

  List<FallingToken>::traverseList(drawFallingToken, gFallingTokens);
  renderIndicatorToken(&graphicsState->indicatorToken);
	displayBoard();
  // NOTE(brendan): only render highlighted when we need to 
  // TODO(brendan): (bug: board hides the aura)
  if(graphicsState->renderHighlighted) {
    List<TokenLocation>::traverseList(highlightToken, gHighlightedTokens);
  }
  if(graphicsState->renderInvalidMessage) {
    // NOTE(Zach): Place the Invalid Message Button
    placeImage(gInvalidMessage->texture, INVALID_MESSAGE_X, INVALID_MESSAGE_Y,
        gInvalidMessage->width, gInvalidMessage->height);
  }
  if(graphicsState->renderInvalidTokenMessage) {
    // NOTE(Zach): Place the Invalid Token Message Button
    placeImage(gInvalidTokenMessage->texture, INVALID_TOKEN_MESSAGE_X, 
        INVALID_TOKEN_MESSAGE_Y, gInvalidTokenMessage->width, 
        gInvalidTokenMessage->height);
  }
	SDL_RenderPresent(gRenderer);
}

void onePlayerRender(GraphicsState *graphicsState)
{
	twoPlayerRender(graphicsState);
}

void twoPlayerRender(GraphicsState *graphicsState)
{
	SDL_RenderClear(gRenderer);

  // Place two player screen
  placeImage(gTwoPlayerScreen->texture, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);

  List<FallingToken>::traverseList(drawFallingToken, gFallingTokens);

  if(graphicsState->renderHighlighted) {
    List<TokenLocation>::traverseList(highlightToken, gHighlightedTokens);
  }
  if (graphicsState->renderIndicatorToken)
	  renderIndicatorToken(&graphicsState->indicatorToken);

  if (graphicsState->renderStatusInProgress) {
    placeImage(gStatusInProgress->texture, STATUS_MESSAGE_X,STATUS_MESSAGE_Y,
        gStatusInProgress->width, gStatusInProgress->height);
  }

  if (graphicsState->renderStatusBlueWon) {
    placeImage(gStatusBlueWon->texture, STATUS_MESSAGE_X,STATUS_MESSAGE_Y,
        gStatusBlueWon->width, gStatusBlueWon->height);
  }

  if (graphicsState->renderStatusRedWon) {
      placeImage(gStatusRedWon->texture, STATUS_MESSAGE_X,STATUS_MESSAGE_Y,
          gStatusRedWon->width, gStatusRedWon->height);
  }

  if (graphicsState->renderStatusDrawGame) {
      placeImage(gStatusDraw->texture, STATUS_MESSAGE_X,STATUS_MESSAGE_Y,
          gStatusDraw->width, gStatusDraw->height);
  }
	displayBoard();
	SDL_RenderPresent(gRenderer);
}

// NOTE(brendan): free myTexture's memory
static void freeTexture(TextureWrapper *myTexture) 
{
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
        CONNECT4_WINDOW_OFFSET_Y, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
static TextureWrapper *loadTexture(std::string path) 
{
  TextureWrapper *loadedTexture = NULL;
  // NOTE(Zach): The final optimized image
  SDL_Texture *newTexture = NULL;

  // NOTE(Zach): Load image at specified path
  SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str()); //use a c string
  if(loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), 
        SDL_GetError());
  }
  else {
    SDL_Rect scaleRect;
    scaleRect.x = 0;
    scaleRect.y = 0;
    scaleRect.w = (int)(loadedSurface->w*(SCALE));
    scaleRect.h = (int)(loadedSurface->h*(SCALE));
    SDL_Surface *scaledSurface = 
      SDL_CreateRGBSurface(0, scaleRect.w, scaleRect.h, 32, 0, 0, 0, 0);
    if(scaledSurface == NULL) {
      printf("Couldn't create surface scaledSurface");
    } else {
      SDL_BlitScaled(loadedSurface, NULL, scaledSurface, &scaleRect);

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
  }
  return loadedTexture;
}

// NOTE(Jean): A function to load all the media files; will return true if all
// loaded successfully

 static bool 
 loadAllFiles(std::string fileNames[], 
     TextureWrapper **textureNames[], int size) 
{
   for (int i = 0; i < size; i++) {
     *textureNames[i] = loadTexture("../misc/"+fileNames[i]+".bmp");
     if (*textureNames[i] == NULL) {
       printf( "Failed to load %s\n", fileNames[i].c_str());
       return false;
     }
   }
   return true;
 }


bool loadMedia() 
{
	// NOTE(brendan): Loading success flag
	bool success = true;

	// NOTE(Zach): I fixed the function loadAllFiles above and the textureNames 
  // array below
	// NOTE(Jean): array of bmp names to be loaded
	std::string filesToLoad[] = {"boardCopy","creditsMenu","setupScreen",
    "twoPlayerScreen","statusBlueWon","statusRedWon",
		"statusDraw","statusProgress","invalidBoard","invalidMessage",
    "redToken2","blueToken2", "gameTitlePage", "glow", "drawGameMessage"};

	// NOTE(Zach): array of pointer to pointers to TextureWrappers
	TextureWrapper **textureNames[] = {&gConnect4Board,&gCreditScreen,&gSetupScreen,&gTwoPlayerScreen,&gStatusBlueWon,&gStatusRedWon,
		&gStatusDraw,&gStatusInProgress,&gInvalidMessage,&gInvalidTokenMessage,&gRedToken, &gBlueToken,
		&gMainMenu,&gGlow,&gDrawGameMessage};

	success = loadAllFiles(filesToLoad, textureNames, 
      sizeof(filesToLoad)/sizeof(filesToLoad[0]));                                
  return success;
}

void close_sdl() 
{
  // NOTE(brendan): de-allocate surface
  freeTexture(gConnect4Board);
  freeTexture(gRedToken);
  freeTexture(gBlueToken);
  freeTexture(gMainMenu);
	freeTexture(gGlow);
  freeTexture(gInvalidMessage);
  freeTexture(gInvalidTokenMessage);
  freeTexture(gDrawGameMessage);
  
  freeTexture(gCreditScreen);
  freeTexture(gSetupScreen);
  freeTexture(gTwoPlayerScreen);
  freeTexture(gStatusBlueWon);
  freeTexture(gStatusRedWon);
  freeTexture(gStatusDraw);
  freeTexture(gStatusInProgress);
  freeTexture(gRefresh);

  gConnect4Board = NULL;
  gRedToken = NULL;
  gBlueToken = NULL;
  gMainMenu = NULL;

  gGlow = NULL;
  gInvalidMessage = NULL;
  gInvalidTokenMessage = NULL;
  gDrawGameMessage = NULL;

  gCreditScreen = NULL;
  gSetupScreen = NULL;
  gTwoPlayerScreen = NULL;
  gStatusBlueWon = NULL;
  gStatusRedWon = NULL;
  gStatusDraw = NULL;
  gStatusInProgress = NULL;
  gRefresh = NULL;

  // NOTE(brendan): Destroy window
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  // NOTE(brendan): Quit SDL subsystems
  SDL_Quit();
}

// NOTE(brendan): returns true if the two tokens are in the same column,
// false otherwise
static bool 
compareXPosition(FallingToken *listItem, FallingToken *item) 
{
  return listItem->x == item->x;
}

// NOTE(Zach): visually drops the token into a cell if drop is valid
bool dropToken(Board b, Token tokenColour, int col) 
{
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
void drawFallingToken(FallingToken *fallingToken) 
{
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

void clearFallingToken(FallingToken *fallingToken) 
{
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
void updateFallingToken(FallingToken *fallingToken, float dt) 
{
	if (!fallingToken->isFalling) return;
#define ACCEL 5
  fallingToken->y += (int)(fallingToken->v * dt);
  fallingToken->v += (int)(ACCEL * dt);
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

// NOTE(brendan): free token
static void freeTokenLocation(TokenLocation *tokenLocation) 
{
  free(tokenLocation);
}

// NOTE(brendan): sets the list of highlighted tokens, freeing the old one
void setHighlightedTokenList(List<TokenLocation> *highlightedTokenList,
    GraphicsState *graphicsState) 
{
  // NOTE(brendan): get rid of old list
  List<TokenLocation>::traverseList(freeTokenLocation, gHighlightedTokens);
  gHighlightedTokens = highlightedTokenList;
  graphicsState->renderHighlighted = true;
}

// NOTE(brendan): load the graphics state (falling tokens/highlighted tokens?)
void loadGraphics(GraphicsState *graphicsState, FILE *in_file) 
{
  if (in_file == 0) {
    printf("Invalid file pointer in loadGraphics function\n");
  }
  else {
    gFallingTokens = List<FallingToken>::readListFromFile(gFallingTokens, 
        in_file);
  }
}

// NOTE(brendan): save the graphics state (falling tokens/highlighted tokens?)
void saveGraphics(GraphicsState *graphicsState, FILE *out_file) 
{
  if (out_file == 0) {
    printf("Invalid file pointer in saveGraphics function\n");
  }
  else {
    List<FallingToken>::writeListToFile(gFallingTokens, out_file);
  }
}
