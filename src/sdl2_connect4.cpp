/* Source code by team struct by_lightning{}; */
#define CHECK_POINT printf("*** Reached line %d of file %s ***\n"\
		, __LINE__, __FILE__)

// NOTE(Zach): Using SDL, SDL_image, standard IO, and strings
#include "graphics.h"
#include "board.h"
#include "linkedList.h"
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
			SDL_RenderClear(gRenderer);
			SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
			SDL_RenderPresent(gRenderer);

      /*********************************************************************/
      FallingToken fallingTokens[] = {{100,1,1,500},
        {200,2,2,500},
        {300,3,3,500},
        {400,4,4,500}};
      Node *list = addToList(&fallingTokens[0], NULL);
      list = addToList(&fallingTokens[1], list);
      list = addToList(&fallingTokens[2], list);
      list = addToList(&fallingTokens[3], list);
      traverseList(drawFallingToken, list);
      //deleteFromList(&fallingTokens[2], list);
      //traverseList(drawFallingToken, list);

		SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
		SDL_RenderPresent(gRenderer);
      /*********************************************************************/
      SDL_Delay(1000);
      traverseList(clearFallingToken, list);
		SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
		SDL_RenderPresent(gRenderer);
      SDL_Delay(1000);
		traverseList(updateFallingToken, 5, list);
		traverseList(updateFallingToken, 5, list);
      traverseList(drawFallingToken, list);
		SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
		SDL_RenderPresent(gRenderer);
      SDL_Delay(1000);
      traverseList(clearFallingToken, list);
		SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
		SDL_RenderPresent(gRenderer);
      SDL_Delay(1000);
      /*********************************************************************/
		for (int j=0; j < 100; j++) {
			traverseList(clearFallingToken, list);
			traverseList(updateFallingToken, 0.5, list);
			traverseList(drawFallingToken, list);
			SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
			SDL_RenderPresent(gRenderer);
			SDL_Delay(32);
		}

			// NOTE(Zach): While application is running
			while(!quit) {
				// These four lines are for testing purposes
				SDL_PumpEvents();
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					SDL_Log("Mouse Button 1 (left) is pressed.");
				}

				while( SDL_PollEvent( &e ) != 0 )
				{
					// NOTE(Zach): Wait for an event to occur
					//SDL_WaitEvent(&e);

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
						SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
					}
					// NOTE(Zach): Remove any SLD_MOUSEBUTTONDOWN events that occured
					// while the token was falling from the event queue.
					SDL_FlushEvent(SDL_MOUSEMOTION);
					//SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

					SDL_RenderCopy( gRenderer, gConnect4Board->texture, NULL, NULL );
					// NOTE(brendan): Update the surface
					SDL_RenderPresent(gRenderer);
				}
				SDL_Delay(32);
			}
		}
	}

	board_destroy(b);
	// NOTE(brendan): Free resources and close SDL.
	close_sdl();

	return 0;
}
