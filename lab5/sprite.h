#ifndef _SPRITE_H
#define _SPRITE_H

#include "vector2D.h"

typedef struct {
vector2D_t position; // current position
int width, height; // dimensions
vector2D_t velocity;
char *map; // the pixmap
} Sprite;

#endif

Sprite *create_sprite(char *pic[], unsigned x, unsigned y, double velocity_x, double velocity_y, unsigned h_res, unsigned v_res);

int clear_sprite_area(Sprite *sp);

int animate_sprite(Sprite *sp, double delta_time);

void destroy_sprite(Sprite *sp);
