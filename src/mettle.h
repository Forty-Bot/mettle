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

#ifndef METTLE_H
#define METTLE_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <string.h>

//For debugging, we can fill allocated memory with 0xFF
#ifdef DEBUG
#pragma message ( "DEBUG flag set" )
void *lmalloc(size_t size) {
	static const unsigned char fill = 0xFF;
	void *ptr = malloc(size);
	for(int i = 0; i < size; i++) { memcpy(ptr+i,&fill,1); }
	return ptr;
}
#define malloc(size) lmalloc(size)
#endif

//The size of a tile in pixels
enum {TILE_SIZE = 16};

//The size of a sprite
enum {SPRITE_SIZE = 32};

//The coordinates of the player on a texture
enum {PLAYER_TEXTURE_X = 1};
enum {PLAYER_TEXTURE_Y = 9};

//Size of an action queue
enum {ACTION_QUEUE_SIZE = 8};

//The actions the player can take
typedef enum playerAction {
	ACTION_NONE = 0,
	ACTION_MOVE_N,
	ACTION_MOVE_S,
	ACTION_MOVE_E,
	ACTION_MOVE_W,
	ACTION_MOVE_NE,
	ACTION_MOVE_SE,
	ACTION_MOVE_SW,
	ACTION_MOVE_NW,
} playerAction;

typedef struct entity entity;

////////////////////////////////////////////////////////////////////////
// aiComp
////////////////////////////////////////////////////////////////////////

//An aiComponent decides what an entity will do
typedef struct aiComp aiComp;

//A method called when the aiComponent takes a turn
typedef void (*aiThink)(aiComp *);

#define AI_COMP entity *parent;	aiThink think

struct aiComp {
	AI_COMP;
};

//Creates an ai which does nothing
aiComp *aiComp_createNone(entity *);
void aiNone_think(aiComp *);
//Creates an ai which follows user input
aiComp *aiComp_createInput(entity *, const playerAction *);
void aiInput_think(aiComp *);

typedef struct aiInput {
	AI_COMP;
	const playerAction *action;
} aiInput;

void aiComp_del(aiComp *);

#undef AI_COMP

////////////////////////////////////////////////////////////////////////
// displayComp
////////////////////////////////////////////////////////////////////////

typedef struct displayComp {
	entity *parent;
	sfSprite *sprite;
} displayComp;

//Creates a new displayComp
displayComp *displayComp_create(entity *, const sfSprite *);

//Draw a display
void displayComp_draw(displayComp *, sfRenderWindow *);

void displayComp_del(displayComp *);

////////////////////////////////////////////////////////////////////////
// entity
////////////////////////////////////////////////////////////////////////

//The basic structure for items, objects, monsters, and anything not part of the map geometry
struct entity {
	//Convenience structure; access x like ent.coord.x or ent.x
	//Nice for passing ent.coord to stuff in one package
	union {
		sfVector2i coord;
		struct {
			int x;
			int y;
		};
	};

	//Anything which controls an entity
	aiComp* ai;
	//Stuff responsible for drawing an entity
	displayComp* display;
};

entity *entity_createPlayer(const sfVector2i, const playerAction *, const sfTexture *);

//Move reletive to the current position (no pathfinding)
void entity_move(entity *, const sfVector2i);
void entity_del(entity *);

sfSprite *createSprite(const sfTexture *, const sfVector2i);

////////////////////////////////////////////////////////////////////////
// actionQueue
////////////////////////////////////////////////////////////////////////

typedef struct actionQueue {
	//Indices of the first and last elements
	int first, last;
	//The actions in the queue
	playerAction queue[ACTION_QUEUE_SIZE];
} actionQueue;

//Create an actionQueue
actionQueue *actionQueue_create();
//Add an action to the queue; returns non-zero if an error occured
int actionQueue_add(actionQueue *, const playerAction);
//Get the next action from the queue, NULL if empty
playerAction actionQueue_next(actionQueue *);
//Delete an actionQueue
void actionQueue_del(actionQueue *);

inline sfVector2i sfVector2i_scalar(const sfVector2i, const int);
inline sfVector2f sfVector2f_scalar(const sfVector2f, const float);
playerAction handleKey(const sfKeyEvent *);

//This macro converts tile coordinates to pixel coordinates (i.e for sprite sheets)
#define TILE_TO_RECT(tile, size) (sfIntRect) {((tile).x - 1) * (size).x, ((tile).y - 1) * (size).y, (size).x, (size).y}
#define XY_TO_RECT(tilex, tiley, sizex, sizey) TILE_TO_RECT(((sfVector2i) {(tilex), (tiley)}), ((sfVector2i) {sizex, sizey}))

#endif