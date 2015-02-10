/* Source code by team struct by_lightning{}; */
#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
									, __LINE__, __FILE__)

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include "graphics.h"
#include "board.h"
#include <stdio.h>
  
int main(int argc, char *argv[]) {
	// NOTE(Zach): Create a board
	Board b;
	b = board_create();

	// NOTE(brendan): Start up SDL and create window.
	if (!init()) {
		// TODO(brendan): Diagnostics
		printf("Failed to initialize!\n");
	} else {
		if (!loadMedia()) {
      	printf("Failed to load media!\n");
		} else {
			// NOTE(Zach): Main loop flag
			bool quit = false;

			// NOTE(Zach): Event handler
			SDL_Event e;

			// NOTE(Zach): blit the background and the board; and update the window surface
			SDL_BlitSurface(gBackground, NULL, gScreenSurface, NULL);
			SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);

			// NOTE(Zach): While application is running
			while(!quit) {
				// NOTE(Zach): Wait for an event to occur
				SDL_WaitEvent(&e);

				// NOTE(Zach): User requests quit
				if (e.type == SDL_QUIT) {
					quit = true;
				} else if (e.type == SDL_MOUSEBUTTONDOWN) {
					// NOTE(Zach): Get mouse position when the mouse was pressed
					int x, y;
					x = e.button.x;
					y = e.button.y;

					// NOTE(Zach): We only want to place token on left mouse click
					// For illustration, left click places red, right click places blue
					// TODO(Zach): Keep trying to figure out why just e.button doesn't work here
					if (e.button.button == SDL_BUTTON_LEFT) {
					// NOTE(Zach): Blit the token in cell that was clicked
					//blitToken(gRedToken, (y - GRID_OFFSET_Y)/TOKEN_HEIGHT, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					dropToken(b, RED, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					} else if (e.button.button == SDL_BUTTON_RIGHT) {
					//blitToken(gBlueToken, (y - GRID_OFFSET_Y)/TOKEN_HEIGHT, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					dropToken(b, BLUE, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					}

					// NOTE(Zach): Blit the board on the tokens
					SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);
				}
				// NOTE(Zach): Remove any SLD_MOUSEBUTTONDOWN events that occured
				// while the token was falling from the event queue.
				SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

				// NOTE(brendan): Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	board_destroy(b);
	// NOTE(brendan): Free resources and close SDL.
	close_sdl();

	return 0;
}
