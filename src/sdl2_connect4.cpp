/* Source code by team struct by_lightning{}; */

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include "graphics.h"
#include "board.h"
//#include <stdio.h>
  
int main(int argc, char *argv[]) {
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
					//Get mouse position
					int x, y;
					SDL_GetMouseState( &x, &y );

					// NOTE(Zach): We only want to place token on left mouse click
					// For illustration, left click places red, right click places blue
					// TODO(Zach): Keep trying to figure out why just e.button doesn't work here
					if (e.button.button == SDL_BUTTON_LEFT) {
					// NOTE(Zach): Blit the token in cell that was clicked
					//blitToken(gRedToken, (y - GRID_OFFSET_Y)/TOKEN_HEIGHT, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					dropToken(gRedToken, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					} else if (e.button.button == SDL_BUTTON_RIGHT) {
					//blitToken(gBlueToken, (y - GRID_OFFSET_Y)/TOKEN_HEIGHT, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					dropToken(gBlueToken, (x - GRID_OFFSET_X)/TOKEN_WIDTH);
					}

					// NOTE(Zach): Blit the board on the tokens
					SDL_BlitSurface(gConnect4Board, NULL, gScreenSurface, NULL);
				}

				// NOTE(brendan): Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	// NOTE(brendan): Free resources and close SDL.
	close_sdl();

	return 0;
}
