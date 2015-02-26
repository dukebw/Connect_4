#ifndef GRAPHICS_H // NOTE(Zach): This is an include guard
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include "board.h"
#include "linkedList.h"

constexpr float SCALE  = 0.85; 
constexpr int SCREEN_WIDTH = (1002*(SCALE));
constexpr int SCREEN_HEIGHT = (902*(SCALE));
constexpr int TOKEN_WIDTH = (100*(SCALE));
constexpr int TOKEN_HEIGHT = (100*(SCALE));
constexpr int GRID_OFFSET_Y = (51*(SCALE));
constexpr int GRID_OFFSET_X = (151*(SCALE));
constexpr int GRID_WIDTH = (700*(SCALE));
constexpr int GRID_HEIGHT = (600*(SCALE));
constexpr int MAINMENU_SETUP_BUTTON_LEFT = (430*(SCALE));
constexpr int MAINMENU_SETUP_BUTTON_RIGHT = (602*(SCALE));
constexpr int MAINMENU_SETUP_BUTTON_TOP = (545*(SCALE));
constexpr int MAINMENU_SETUP_BUTTON_BOTTOM = (610*(SCALE));
constexpr int MAINMENU_QUIT_BUTTON_LEFT = (452*(SCALE));
constexpr int MAINMENU_QUIT_BUTTON_RIGHT = (575*(SCALE));
constexpr int MAINMENU_QUIT_BUTTON_TOP = (700*(SCALE));
constexpr int MAINMENU_QUIT_BUTTON_BOTTOM = (760*(SCALE));
constexpr int SETUP_BOTTOM_BUTTONS_OFFSET = (10*(SCALE));
constexpr int SETUP_CLICKY_TOKENS_OFFSET = (25*(SCALE));
constexpr int SETUP_2PLAYER_BUTTON_WIDTH = (130*(SCALE));
constexpr int SETUP_2PLAYER_BUTTON_HEIGHT = (45*(SCALE));
constexpr int SETUP_1PLAYER_BUTTON_WIDTH = (120*(SCALE));
constexpr int SETUP_1PLAYER_BUTTON_HEIGHT = (45*(SCALE));
constexpr int SETUP_MENU_BUTTON_WIDTH = (130*(SCALE));
constexpr int SETUP_MENU_BUTTON_HEIGHT = (45*(SCALE));

// NOTE(brendan): defined in graphics.cpp; implementation details not leaked
struct FallingToken;
struct TextureWrapper;
struct TokenLocation
// NOTE(Jean): moved definition of TokenLocation to graphics.cpp
//				hope that wont affect anything
// NOTE(brendan): convenient wrapper; used for highlighting
// struct TokenLocation {
//   int row;
//   int column;
//   Token colour;
// }; 

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
void mainMenuRender();

// NOTE(Zach): performs the rendering that needs to be done when transitioning
// to setup from another state
void transitionSetupRender(void);

// NOTE(brendan): does rendering for setup
void setupRender();

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
