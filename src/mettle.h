#ifndef METTLE_H
#define METTLE_H

#include <SFML/Graphics.h>

sfVector2i handle_key(const sfKeyEvent*);

//This macro converts tile coordinates to pixel coordinates (i.e for sprite sheets)
#define TILE_TO_RECT(tile, size) (sfIntRect) {((tile).x - 1) * (size).x, ((tile).y - 1) * (size).y, (size).x, (size).y}
#define XY_TO_RECT(tilex, tiley, sizex, sizey) TILE_TO_RECT(((sfVector2i) {(tilex), (tiley)}), ((sfVector2i) {sizex, sizey}))

#endif