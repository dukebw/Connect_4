#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameMenus.h"
#include "game.h"
#include "graphics.h"

// NOTE(Zach): The main connect4 application
int connect4(void)
{
	// NOTE(Zach): Seed the random number generator
	srand((unsigned) time(NULL));
	// NOTE(Zach): Start SDL and create a window
	if (!init()) {
		printf("Failed to initialize!\n");
	} else {
		// NOTE(Zach): Load media file
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			// NOTE(Zach): State of the menu
			MenuState currentState = MAINMENU;

			Board b;
			if ((b = board_create()) == NULL) currentState = QUIT;

			while(currentState != QUIT) {
				switch (currentState) {
					case  MAINMENU:
						currentState = mainMenu();
						break;
					case ONEPLAYER:
						currentState = connect4Game(b, 1);
						board_empty(b);
						break;
					case TWOPLAYER:
						currentState = connect4Game(b, 2);
						board_empty(b);
						break;
					case SETUP:
						board_empty(b);
						currentState = setupGame(b);
						break;
					case CREDITS:
						currentState = creditsMenu();
						break;
					default:
						break;
				}
			}
			board_destroy(b);
		}
	}
	close_sdl();
	return 0;
}

// NOTE(Zach): Display and handle mouse clicks/motion of the Main Menu
MenuState mainMenu(void)
{
	MenuState currentState = MAINMENU;
	displayMainMenu();
	// NOTE(Zach): Event handler
	SDL_Event e;

	int x, y;

	while (currentState == MAINMENU) {
		// NOTE(Zach): Wait for an event to occur
		SDL_WaitEvent(&e);

		// NOTE(Zach): User requests quit
		if (e.type == SDL_QUIT) {
			return QUIT;
		} else if (e.type == SDL_MOUSEBUTTONDOWN &&
			e.button.button == SDL_BUTTON_LEFT) {

			x = e.button.x;
			y = e.button.y;

			currentState = handleMainMenuMouseClick(x, y);
		} else {
			//handleMainMenuMouseMotion();
		}
	}
	return currentState;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleMainMenuMouseClick(int x, int y)
{
	//if (x >= 102 && y>= 145 && x <= 398 && y <= 211) return ONEPLAYER;
	//if (x >= 96 && y>= 221 && x <= 406 && y <= 287) return TWOPLAYER;
	//if (x >= 96 && y>= 221 && x <= 406 && y <= 287) return SETUP;
	//if (x >= 338 && y>= 441 && x <= 493 && y <= 493) return CREDITS;
	//if (x >= 190 && y>= 297 && x <= 313 && y <= 353) return QUIT;
	return SETUP; // NOTE(Zach): TEMPORARY
	return MAINMENU;
}

// NOTE(Zach): Display and handle mouse clicks/motion of the Credits Menu
MenuState creditsMenu(void)
{
	MenuState currentState = CREDITS;
	displayCreditsMenu();
	// NOTE(Zach): Event handler
	SDL_Event e;

	int x, y;

	while (currentState == CREDITS) {
		// NOTE(Zach): Wait for an event to occur
		SDL_WaitEvent(&e);

		// NOTE(Zach): User requests quit
		if (e.type == SDL_QUIT) {
			return QUIT;
		} else if (e.type == SDL_MOUSEBUTTONDOWN &&
			e.button.button == SDL_BUTTON_LEFT) {

			x = e.button.x;
			y = e.button.y;
			currentState = handleCreditsMenuMouseClick(x, y);
		} else {
			//handleCreditsMenuMouseMotion();
		}
	}
	return currentState;
}

// NOTE(Zach): Determine next MenuState based on where the user clicked
MenuState handleCreditsMenuMouseClick(int x, int y)
{
	//if (x >= 48 && y>= 413 && x <= 454 && y <= 465) return MAINMENU;
	return MAINMENU;
}
