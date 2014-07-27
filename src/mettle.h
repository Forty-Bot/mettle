#ifndef METTLE_H
#define METTLE_H

#include <SFML/Graphics.h>

typedef struct coord {
	int x;
	int y;
} coord;

coord get_movement(sfBool);

#endif