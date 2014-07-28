#include "mettle.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SFML/Graphics.h>

int main(int argc, char **argv) {

	int window_width = 640;
	int window_height = 480;

	//Create the render window
	sfRenderWindow* window = NULL;
	if(!(window = sfRenderWindow_create((sfVideoMode) {window_width, window_height, 32}, "Mettle", sfClose|sfResize, NULL))) {
		printf("Unable to create RenderWindow\n");
		exit(EXIT_FAILURE);
	}

	//Load the hero sprite
	sfTexture* hero_texture = NULL;
	static const char* hero_texture_location = "data/img/leatherarmor.png";
	if(!(hero_texture = sfTexture_createFromFile(hero_texture_location, NULL))) {
		printf("Unable to load %s\n", hero_texture_location);
		exit(EXIT_FAILURE);
	}
	sfSprite* hero_sprite = NULL;
	if(!(hero_sprite = sfSprite_create())) {
		printf("Unable to create sprite\n");
		exit(EXIT_FAILURE);
	}
	sfSprite_setTexture(hero_sprite, hero_texture, sfFalse);
	//Select sprite (1, 9) from the sprite sheet
	sfSprite_setTextureRect(hero_sprite, XY_TO_RECT(1, 9, 32, 32));
	//Scale it up some
	//sfSprite_setScale(hero_sprite, (sfVector2f) {1.5f, 1.5f});


	//Create the "map"
	sfTexture* map_texture = NULL;
	static const char* map_texture_location = "data/img/tilesheet.png";
	if(!(map_texture = sfTexture_createFromFile(map_texture_location, NULL))) {
		printf("Unable to load %s\n", map_texture_location);
		exit(EXIT_FAILURE);
	}

	int map_width = (int) ceilf(window_width / 16.0f);
	int map_height = (int) ceilf(window_height / 16.0f);
	//Allocate space
	sfSprite* tile = NULL;
	if(!(tile = sfSprite_create())) {
		printf("Unable to create sprite\n");
		exit(EXIT_FAILURE);
	}
	//Initialize the tile
	sfSprite_setTexture(tile, map_texture, sfFalse);
	sfSprite_setTextureRect(tile, XY_TO_RECT(13, 8, 16, 16));

	//Event holder
	sfEvent event;
	//Window active variable
	sfBool window_active = sfFalse;

	//Main loop
	while(sfRenderWindow_isOpen(window)) {

		//Process events
		while(sfRenderWindow_pollEvent(window, &event)) {
			switch(event.type) {
				//Handle keyboard input
				case(sfEvtKeyPressed):
				case(sfEvtKeyReleased):
					{
						sfVector2i d = handle_key((sfKeyEvent *) &event);
						sfVector2f offset = (sfVector2f) {(float) d.x, (float) d.y};
						sfSprite_move(hero_sprite, offset);
						break;
					}

				case(sfEvtClosed):
					sfRenderWindow_close(window);
					break;

				case(sfEvtGainedFocus):
					window_active = sfTrue;
					break;

				case(sfEvtLostFocus):
					window_active = sfFalse;
				default:
					break;
			}
		}

		//Update position
		/*sfVector2i d = get_movement(window_active);
		sfVector2f2f offset = (sfVector2f) {(float) d.x, (float) d.y};
		sfSprite_move(sprite, offset);*/

		//Clear the screen and render
		sfRenderWindow_clear(window, sfMagenta);

		//Draw the map
		int x,y;
		for(x = 0; x < map_width; x++) {
			for(y = 0; y < map_height; y++) {
				sfSprite_setPosition(tile, (sfVector2f) {x * 16, y * 16});
				sfRenderWindow_drawSprite(window, tile, NULL);
			}
		}

		//Draw the hero
		sfRenderWindow_drawSprite(window, hero_sprite, NULL);

		sfRenderWindow_display(window);
	}

	//Cleanup
	sfRenderWindow_destroy(window);
	return EXIT_SUCCESS;

}

/*
 * Check for keyboard input, and return a struct with movement
 */
/*sfVector2i get_movement(const sfBool active) {

	//Don't check for input if we're not active
	if(!active) {
		return (sfVector2i) {0, 0};
	}

	int x = 0;
	int y = 0;

	#define IF_KEY(key) if(sfKeyboard_isKeyPressed(sfKey##key))

	IF_KEY(H) {
		x += -10;
	}
	IF_KEY(J) {
		y += 10;
	}
	IF_KEY(K) {
		y += -10;
	}
	IF_KEY(L) {
		x += 10;
	}

	#undef IF_KEY

	return (sfVector2i) {x, y};

}*/

sfVector2i handle_key(const sfKeyEvent *key) {

	//Only do key pressed events
	if(key->type == sfEvtKeyReleased) {
		return (sfVector2i) {0, 0};
	}

	sfVector2i d = (sfVector2i) {0, 0};
	//TODO: make this a global constant
	static const int tile = 16;

	switch(key->code) {
		case(sfKeyY):
			d.y -= tile;
		case(sfKeyH):
		case(sfKeyA):
			d.x -= tile;
			break;

		case(sfKeyB):
			d.x -= tile;
		case(sfKeyJ):
		case(sfKeyS):
			d.y += tile;
			break;

		case(sfKeyU):
			d.x += tile;
		case(sfKeyK):
		case(sfKeyW):
			d.y -= tile;
			break;

		case(sfKeyN):
			d.y += tile;
		case(sfKeyL):
		case(sfKeyD):
			d.x += tile;
		default:
			break;
	}

	return d;

}

