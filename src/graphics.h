#ifndef GRAPHICS_H // NOTE(Zach): This is an include guard
#define GRAPHICS_H

#include <string>
#include "board.h"
#include "linkedList.h"

// NOTE(brendan): keeps the window from opening under my start bar and
// hence being unusable; probably solved with window resizing
const int CONNECT4_WINDOW_OFFSET_Y = 100;

// TODO(brendan): load these values when textures are loaded
const float SCALE  = 0.8; 
const int SCREEN_WIDTH = (int)(1002*(SCALE));
const int SCREEN_HEIGHT = (int)(902*(SCALE));
const int TOKEN_WIDTH = (int)(100*(SCALE));
const int TOKEN_HEIGHT = (int)(100*(SCALE));
const int GRID_OFFSET_Y = (int)(51*(SCALE));
const int GRID_OFFSET_X = (int)(151*(SCALE));
const int GRID_WIDTH = (int)(700*(SCALE));
const int GRID_HEIGHT = (int)(600*(SCALE));
const int MAINMENU_SETUP_BUTTON_LEFT = (int)(430*(SCALE));
const int MAINMENU_SETUP_BUTTON_RIGHT = (int)(602*(SCALE));
const int MAINMENU_SETUP_BUTTON_TOP = (int)(545*(SCALE));
const int MAINMENU_SETUP_BUTTON_BOTTOM = (int)(610*(SCALE));
const int MAINMENU_QUIT_BUTTON_LEFT = (int)(452*(SCALE));
const int MAINMENU_QUIT_BUTTON_RIGHT = (int)(575*(SCALE));
const int MAINMENU_QUIT_BUTTON_TOP = (int)(700*(SCALE));
const int MAINMENU_QUIT_BUTTON_BOTTOM = (int)(760*(SCALE));
const int SETUP_BOTTOM_BUTTONS_OFFSET = (int)(10*(SCALE));
const int SETUP_CLICKY_TOKENS_OFFSET = (int)(25*(SCALE));
const int SETUP_2PLAYER_BUTTON_WIDTH = (int)(130*(SCALE));
const int SETUP_2PLAYER_BUTTON_HEIGHT = (int)(45*(SCALE));
const int SETUP_1PLAYER_BUTTON_WIDTH = (int)(120*(SCALE));
const int SETUP_1PLAYER_BUTTON_HEIGHT = (int)(45*(SCALE));
const int SETUP_MENU_BUTTON_WIDTH = (int)(130*(SCALE));
const int SETUP_MENU_BUTTON_HEIGHT = (int)(45*(SCALE));
const int INVALID_MESSAGE_WIDTH = (int)(250*(SCALE));
const int INVALID_MESSAGE_HEIGHT = (int)(40*(SCALE));
const int INVALID_MESSAGE_X = 
  (int)((float)(SCREEN_WIDTH-INVALID_MESSAGE_WIDTH)/2.0f);
const int INVALID_MESSAGE_Y = (int)(700*(SCALE));
const int INVALID_TOKEN_MESSAGE_WIDTH = (int)(350*(SCALE));
const int INVALID_TOKEN_MESSAGE_HEIGHT = (int)(50*(SCALE));
const int INVALID_TOKEN_MESSAGE_X = 
  (int)((float)(SCREEN_WIDTH-INVALID_TOKEN_MESSAGE_WIDTH)/2.0f);
const int INVALID_TOKEN_MESSAGE_Y = (int)(775*(SCALE));

// NOTE(brendan): defined in graphics.cpp; implementation details not leaked
struct FallingToken;
struct TextureWrapper;
// NOTE(brendan): convenient wrapper; used for highlighting
struct TokenLocation {
  int row;
  int column;
  Token colour;
}; 
// NOTE(brendan): contains rendering information
struct GraphicsState {
  bool renderInvalidMessage;
  bool clearInvalidMessage;
  bool renderInvalidTokenMessage;
  bool clearInvalidTokenMessage;
};

void drawFallingToken(FallingToken *token);
void clearFallingToken(FallingToken *fallingToken);
void updateFallingToken(FallingToken *fallingToken, float dt);

// TODO(brendan): Make these local at some point
// NOTE(brendan): Global window/image declarations.
// NOTE(Zach): Eventually these will be local to the graphics module and will
// be removed from here
extern List<FallingToken> *gFallingTokens;

// NOTE(brendan): does rendering for credits menu
//void creditsMenuRender();

// NOTE(brendan): does rendering for main menu
void mainMenuRender(GraphicsState *graphicsState);

// NOTE(Zach): performs the rendering that needs to be done when transitioning
// to setup from another state
void transitionSetupRender(void);

// NOTE(brendan): does rendering for setup
void setupRender(GraphicsState *graphicsState);

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close_sdl();

// NOTE(Zach): visually drops the token into a cell and add it to the board
bool dropToken(Board b, Token tokenColour, int col);

// NOTE(brendan): delete stationary tokens from gFallingTokens
void deleteStillToken(FallingToken *fallingToken);

// NOTE(brendan): sets the list of highlighted tokens, freeing the old one
void setHighlightedTokenList(List<TokenLocation> *highlightedTokenList);
#endif // GRAPHICS_H
