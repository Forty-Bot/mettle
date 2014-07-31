/*
 * Mettle - A roguelike using sfml
 * Copyright (C) 2014 Sean Anderson
 *
 * Mettle is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mettle.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SFML/Graphics.h>

int main(int argc, char **argv) {

	int window_width = 640;
	int window_height = 480;

	//Create the render window
	sfRenderWindow *window = NULL;
	if(!(window = sfRenderWindow_create((sfVideoMode) {window_width, window_height, 32}, "Mettle", sfClose|sfResize, NULL))) {
		printf("Unable to create RenderWindow\n");
		exit(EXIT_FAILURE);
	}

	//Create a queue of inputs
	actionQueue *actions = actionQueue_create();
	playerAction action = actionQueue_next(actions);
	//Load the hero sprite
	sfTexture *hero_texture = NULL;
	static const char *hero_texture_location = "data/img/leatherarmor.png";
	if(!(hero_texture = sfTexture_createFromFile(hero_texture_location, NULL))) {
		printf("Unable to load %s\n", hero_texture_location);
		exit(EXIT_FAILURE);
	}
	entity *hero = entity_createPlayer((sfVector2i) {0, 0}, &action, hero_texture);


	//Create the "map"
	sfTexture *map_texture = NULL;
	static const char *map_texture_location = "data/img/tilesheet.png";
	if(!(map_texture = sfTexture_createFromFile(map_texture_location, NULL))) {
		printf("Unable to load %s\n", map_texture_location);
		exit(EXIT_FAILURE);
	}

	int map_width = (int) ceilf((float) (window_width / TILE_SIZE));
	int map_height = (int) ceilf((float) (window_height / TILE_SIZE));
	//Allocate space
	sfSprite *tile = NULL;
	if(!(tile = sfSprite_create())) {
		printf("Unable to create sprite\n");
		exit(EXIT_FAILURE);
	}
	//Initialize the tile
	sfSprite_setTexture(tile, map_texture, sfFalse);
	sfSprite_setTextureRect(tile, XY_TO_RECT(4, 2, TILE_SIZE, TILE_SIZE));

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
						playerAction action = handleKey(&event.key);
						if(action) {
							actionQueue_add(actions, action);
						}
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

		//While there are still actions to do
		while(action = actionQueue_next(actions)) {
			hero->ai->think(hero->ai);
		}

		//Clear the screen and render
		sfRenderWindow_clear(window, sfMagenta);

		//Draw the map
		int x,y;
		for(x = 0; x < map_width; x++) {
			for(y = 0; y < map_height; y++) {
				sfSprite_setPosition(tile, (sfVector2f) {x * TILE_SIZE, y * TILE_SIZE});
				sfRenderWindow_drawSprite(window, tile, NULL);
			}
		}

		//Draw the hero
		displayComp_draw(hero->display, window);

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

playerAction handleKey(const sfKeyEvent *key) {

	//Only do key pressed events
	if(key->type == sfEvtKeyReleased) {
		return ACTION_NONE;
	}

	switch(key->code) {
		case(sfKeyK):
		case(sfKeyW):
			return ACTION_MOVE_N;
		case(sfKeyU):
			return ACTION_MOVE_NE;
		case(sfKeyL):
		case(sfKeyD):
			return ACTION_MOVE_E;
		case(sfKeyN):
			return ACTION_MOVE_SE;
		case(sfKeyJ):
		case(sfKeyS):
			return ACTION_MOVE_S;
		case(sfKeyB):
			return ACTION_MOVE_SW;
		case(sfKeyH):
		case(sfKeyA):
			return ACTION_MOVE_W;
		case(sfKeyY):
			return ACTION_MOVE_NW;
		default:
			return ACTION_NONE;
	}

}

/*
 * Scalar multiplication of vectors
 */
inline sfVector2i sfVector2i_scalar(const sfVector2i v, const int i) {

	return (sfVector2i) {v.x * i, v.y * i};

}

/*
 * Scalar multiplication of vectors
 */
inline sfVector2f sfVector2f_scalar(const sfVector2f v, const float f) {

	return (sfVector2f) {v.x * f, v.y * f};

}

aiComp *aiComp_createNone(entity *parent) {

	aiComp *this = malloc(sizeof(aiComp));
	this->parent = parent;
	this->think = aiNone_think;
	return this;

}

void aiNone_think(aiComp *this) {}

aiComp *aiComp_createInput(entity *parent, const playerAction *action) {

	aiInput *this = malloc(sizeof(aiInput));
	this->parent = parent;
	this->think = aiInput_think;
	this->action = action;
	return (aiComp *) this;

}

void aiInput_think(aiComp *ai) {

	aiInput *this = (aiInput *) ai;
	sfVector2i d = (sfVector2i) {0, 0};
	switch(*this->action) {
		case(ACTION_MOVE_NE):
			d.x += 1;
		case(ACTION_MOVE_N):
			d.y += -1;
			break;
		case(ACTION_MOVE_SE):
			d.y += 1;
		case(ACTION_MOVE_E):
			d.x += 1;
			break;
		case(ACTION_MOVE_SW):
			d.x += -1;
		case(ACTION_MOVE_S):
			d.y += 1;
			break;
		case(ACTION_MOVE_NW):
			d.y += -1;
		case(ACTION_MOVE_W):
			d.x += -1;
		case(ACTION_NONE):
			break;
	}
	entity_move(this->parent, d);

}

void aiComp_del(aiComp *this) {free(this);}

displayComp *displayComp_create(entity *parent, const sfSprite *sprite) {

	displayComp *this = malloc(sizeof(displayComp));
	this->parent = parent;
	this->sprite = sfSprite_copy(sprite);
	return this;

}

void displayComp_draw(displayComp *this, sfRenderWindow *window) {

	sfVector2f pos = (sfVector2f) {this->parent->x * TILE_SIZE, this->parent->y * TILE_SIZE};
	sfSprite_setPosition(this->sprite, pos);
	sfRenderWindow_drawSprite(window, this->sprite, NULL);

}

void displayComp_del(displayComp *this) {

	sfSprite_destroy(this->sprite);
	free(this);

}

entity *entity_createPlayer(const sfVector2i coord, const playerAction *action, const sfTexture* texture) {

	//Allocate resources
	entity *this = malloc(sizeof(entity));
	this->coord = coord;
	this->ai = aiComp_createInput(this, action);

	//Allocate the sprite
	sfSprite *sprite = NULL;
	if(!(sprite = sfSprite_create())) {
		printf("Could not create sprite");
		exit(EXIT_FAILURE);
	}
	//Set the texture
	sfSprite_setTexture(sprite, texture, sfFalse);
	sfSprite_setTextureRect(sprite, XY_TO_RECT(PLAYER_TEXTURE_X, PLAYER_TEXTURE_Y, SPRITE_SIZE, SPRITE_SIZE));
	this->display = displayComp_create(this, sprite);
	sfSprite_destroy(sprite);

	return this;

}

void entity_move(entity *this, const sfVector2i d) {

	this->x += d.x;
	this->y += d.y;

}

void entity_del(entity *this) {

	aiComp_del(this->ai);
	displayComp_del(this->display);
	free(this);

}

actionQueue *actionQueue_create() {

	actionQueue *this = malloc(sizeof(actionQueue));
	this->first = 0;
	this->last = 0;
	return this;

}

int actionQueue_add(actionQueue *this, const playerAction action) {

	//Check to see if there's room in the queue
	if(((this->last + 1) % ACTION_QUEUE_SIZE) == this->first) {
		return -1;
	}
	this->last = (this->last + 1) % ACTION_QUEUE_SIZE;
	this->queue[this->last] = action;
	return 0;

}

playerAction actionQueue_next(actionQueue *this) {

	//Check to see if the queue is empty
	if(this->first == this->last) {
		return ACTION_NONE;
	}
	this->first = (this->first + 1) % ACTION_QUEUE_SIZE;
	return this->queue[this->first];

}

void actionQueue_del(actionQueue *this) {

	free(this);

}