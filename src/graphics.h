/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#ifndef GRAPHICS_H // NOTE(Zach): This is an include guard
#define GRAPHICS_H

#include <string>
#include "board.h"
#include "linkedList.h"
#include "helper.h"

// NOTE(brendan): convenience function for making a scaled rectangle
inline Rectangle
initScaledRectangle(int left, int top, int right, int bottom, float scale) {
  Rectangle resultRect = {};
  resultRect.topLeft.x = (int)(left*scale);
  resultRect.topLeft.y = (int)(top*scale);
  resultRect.bottomRight.x = (int)(right*scale);
  resultRect.bottomRight.y = (int)(bottom*scale);
  return resultRect;
}

// NOTE(brendan): convenience function for a non-scaled circle
inline Circle
initCircle(int centerX, int centerY, int radius) {
  Circle resultCircle = {};
  resultCircle.center.x = centerX;
  resultCircle.center.y = centerY;
  resultCircle.radius = radius;
  return resultCircle;
}

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
 
  //main menu
const Rectangle MAINMENU_TWOPLAYER_BUTTON_RECT =
  initScaledRectangle(530, 455, 642, 490, SCALE);
const Rectangle MAINMENU_SETUP_BUTTON_RECT =
  initScaledRectangle(430, 545, 602, 610, SCALE);
const Rectangle MAINMENU_LOADGAME_BUTTON_RECT = 
  initScaledRectangle(357,660,683,706,SCALE);
const Rectangle MAINMENU_CREDIT_BUTTON_RECT =
  initScaledRectangle(890, 840, 972, 868, SCALE);
const Rectangle MAINMENU_QUIT_BUTTON_RECT =
  initScaledRectangle(452, 775, 575, 835, SCALE);

  //credits screen
const Rectangle CREDITS_QUIT_BUTTON_RECT =
  initScaledRectangle(809, 823, 914, 854, SCALE);

  //setup screen
const Rectangle SETUP_1PLAYER_BUTTON_RECT =
  initScaledRectangle(94, 770, 374, 832, SCALE);
const Rectangle SETUP_2PLAYER_BUTTON_RECT =
  initScaledRectangle(397, 770, 681, 832, SCALE);
const Rectangle SETUP_MENU_BUTTON_RECT =
  initScaledRectangle(771, 770, 921, 832, SCALE);
const Rectangle REFRESH_BUTTON_RECT =
  initScaledRectangle(45, 588, 103, 648, SCALE);

  //save button in setup, 1player and 2player game modes
 const Rectangle SAVE_BUTTON_RECT = 
  initScaledRectangle(868,598,988,648,SCALE);


const int SETUP_CLICKY_TOKENS_OFFSET = (int)(25*(SCALE));

const int TOKEN_RADIUS = TOKEN_WIDTH/2;

const Circle SETUP_RED_CLICKY_TOKENS_CIRCLE =
  initCircle(SETUP_CLICKY_TOKENS_OFFSET + TOKEN_RADIUS, 
    GRID_OFFSET_Y + TOKEN_RADIUS, TOKEN_RADIUS);

const Circle SETUP_BLUE_CLICKY_TOKENS_CIRCLE =
  initCircle(SCREEN_WIDTH - SETUP_CLICKY_TOKENS_OFFSET - TOKEN_RADIUS, 
    SETUP_RED_CLICKY_TOKENS_CIRCLE.center.y, TOKEN_RADIUS);

const Rectangle TWOPLAYER_MENU_BUTTON_RECT =
  initScaledRectangle(862, 847, 992, 892, SCALE);

const int SETUP_BOTTOM_BUTTONS_OFFSET = (int)(10*(SCALE));
const int SETUP_1PLAYER_BUTTON_WIDTH = (int)(120*(SCALE));
const int SETUP_1PLAYER_BUTTON_HEIGHT = (int)(45*(SCALE));
const int SETUP_MENU_BUTTON_WIDTH = (int)(130*(SCALE));
const int SETUP_MENU_BUTTON_HEIGHT = (int)(45*(SCALE));

//invalid board msg
const int INVALID_MESSAGE_WIDTH = (int)(240*(SCALE));
const int INVALID_MESSAGE_HEIGHT = (int)(50*(SCALE));
const int INVALID_MESSAGE_X = (int)(147*(SCALE));
const int INVALID_MESSAGE_Y = (int)(680*(SCALE));

//invalid number of tokens msg
const int INVALID_TOKEN_MESSAGE_WIDTH = (int)(500*(SCALE));
const int INVALID_TOKEN_MESSAGE_HEIGHT = (int)(50*(SCALE));
const int INVALID_TOKEN_MESSAGE_X = (int)(422*(SCALE));
const int INVALID_TOKEN_MESSAGE_Y = (int)(680*(SCALE));

//refresh button
const int REFRESH_BUTTON_WIDTH = (int)(60*(SCALE));
const int REFRESH_BUTTON_HEIGHT = (int)(60*(SCALE));

//save button
const int SAVE_BUTTON_WIDTH = (int)(120*(SCALE));
const int SAVE_BUTTON_HEIGHT = (int)(50*(SCALE));

//twoplayer and oneplayer status messages 
const int STATUS_MESSAGE_HEIGHT = (int)(60*(SCALE));
const int STATUS_MESSAGE_WIDTH = (int)(400*(SCALE));

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
  bool renderHighlighted;
  bool renderIndicatorToken;
  int previousTokensWritten;
  TokenLocation indicatorToken;
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
void creditsRender(GraphicsState *graphicsState);

// NOTE(brendan): does rendering for main menu
void mainMenuRender(GraphicsState *graphicsState);

// NOTE(brendan): does rendering for setup
void setupRender(GraphicsState *graphicsState);

// NOTE(Zach): does the rendering for two player mode
void twoPlayerRender(GraphicsState *graphicsState);

// NOTE(brendan): Starts up SDL and creates window
bool init();

// NOTE(brendan): Loads media
bool loadMedia();

// NOTE(brendan): Frees media and shuts down SDL
void close_sdl();

// NOTE(Zach): visually drops the token into a cell and add it to the board
bool dropToken(Board b, Token tokenColour, int col);

// NOTE(brendan): sets the list of highlighted tokens, freeing the old one
void setHighlightedTokenList(List<TokenLocation> *highlightedTokenList,
    GraphicsState *graphicsState);

// NOTE(Zach): reset the graphic state flags to false
void resetGraphicsState(GraphicsState *graphicsState);

// NOTE(Zach): render the indicator token
void renderIndicatorToken(TokenLocation *indicatorToken);

// NOTE(brendan): load the graphics state (falling tokens/highlighted tokens?)
void loadGraphics(GraphicsState *graphicsState, FILE *out_file);

// NOTE(brendan): save the graphics state (falling tokens/highlighted tokens?)
void saveGraphics(GraphicsState *graphicsState, FILE *in_file);

#endif // GRAPHICS_H
