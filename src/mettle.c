#include "mettle.h"

#include <stdio.h>
#include <stdlib.h>

#include <SFML/Graphics.h>

int main(int argc, char **argv) {

	int window_width = 640;
	int window_height = 480;

	//Create the render window
	sfRenderWindow* window = NULL;
	if(!(window = sfRenderWindow_create((sfVideoMode) {window_width, window_height, 32}, "Mettle", sfClose|sfResize, NULL))) {
		printf("Unable to create RenderWindow");
		exit(EXIT_FAILURE);
	}

	//Load the hero sprite
	sfTexture* texture = NULL;
	if(!(texture = sfTexture_createFromFile("data/hero.png", NULL))) {
		printf("Unable to load data/hero.png");
		exit(EXIT_FAILURE);
	}
	sfSprite* sprite = NULL;
	if(!(sprite = sfSprite_create())) {
		printf("Unable to create sprite");
		exit(EXIT_FAILURE);
	}
	sfSprite_setTexture(sprite, texture, sfTrue);

	//Event holder
	sfEvent event;
	//Window active variable
	sfBool window_active = sfFalse;

	//Main loop
	while(sfRenderWindow_isOpen(window)) {

		//Process events
		while(sfRenderWindow_pollEvent(window, &event)) {
			switch(event.type) {
				case(sfEvtClosed): sfRenderWindow_close(window); break;
				case(sfEvtGainedFocus): window_active = sfTrue; break;
				case(sfEvtLostFocus): window_active = sfFalse; break;
				default: break;
			}
		}

		//Update position
		coord d = get_movement(window_active);
		sfVector2f offset;
		offset.x = (float) d.x;
		offset.y = (float) d.y;
		sfSprite_move(sprite, offset);

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

/*
 * Check for keyboard input, and return a struct with movement
 */
coord get_movement(const sfBool active) {

	//Don't check for input if we're not active
	if(!active) {
		return (coord) {0, 0};
	}

	int x = 0;
	int y = 0;

	#define IF_KEY(key) if(sfKeyboard_isKeyPressed(sfKey##key))

	IF_KEY(H) {
		x--;
	}
	IF_KEY(J) {
		y++;
	}
	IF_KEY(K) {
		y--;
	}
	IF_KEY(L) {
		x++;
	}

	#undef IF_KEY

	return (coord) {x, y};

}