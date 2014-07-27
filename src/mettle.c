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

	//Load the hero sprite
	sfTexture* texture;
	if(!(texture = sfTexture_createFromFile("data/hero.png", NULL))) {
		printf("Unable to load data/hero.png");
		exit(EXIT_FAILURE);
	}
	sfSprite* sprite;
	if(!(sprite = sfSprite_create())) {
		printf("Unable to create sprite");
		exit(EXIT_FAILURE);
	}
	sfSprite_setTexture(sprite, texture, sfTrue);

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

		//Draw the hero
		sfRenderWindow_drawSprite(window, sprite, NULL);

		sfRenderWindow_display(window);
	}

	//Cleanup
	sfRenderWindow_destroy(window);
	return EXIT_SUCCESS;

}