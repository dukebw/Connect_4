#ifndef GAMEMENUS_H
#define GAMEMENUS_H

#define NUMBER_OF_STATES 7

typedef enum {
  MAINMENU, ONEPLAYER, TWOPLAYER, SETUP, CREDITS, QUIT, DONOTHING
} MenuState;

// NOTE(Zach): Display and handle mouse clicks/motion of the Main Menu
MenuState mainMenu(void);

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleMainMenuMouseClick(int x, int y);

// NOTE(Zach): Display and handle mouse clicks/motion of the Credits Menu
MenuState creditsMenu(void);

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y);

#endif // GAMEMENUS_H