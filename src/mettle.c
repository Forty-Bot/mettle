#include "mettle.h"

#include <stdio.h>
#include <stdlib.h>

#include <SFML/Graphics.h>

int main(int argc, char **argv) {

	int window_width = 640;
	int window_height = 480;

	//Create the render window
	sfRenderWindow* window;
	if(!(window = sfRenderWindow_create((sfVideoMode) {window_width, window_height, 32}, "Mettle", sfClose, NULL))) {
		printf("Unable to create RenderWindow");
		exit(EXIT_FAILURE);
	}

	//Event holder
	sfEvent event;

	//Main loop
	while(sfRenderWindow_isOpen(window)) {
		//Process events
		while(sfRenderWindow_pollEvent(window, &event)) {
			switch(event.type) {
				case(sfEvtClosed): sfRenderWindow_close(window); break;
				default: break;
			}
		}
		//Clear the screen and render
		sfRenderWindow_clear(window, sfBlack);
		sfRenderWindow_display(window);
	}

	//Cleanup
	sfRenderWindow_destroy(window);
	return EXIT_SUCCESS;

}