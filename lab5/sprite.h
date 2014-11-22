#ifndef _SPRITE_H
#define _SPRITE_H

typedef struct {
int x, y; // current position
int width, height; // dimensions
double xspeed, yspeed; // current speed
char *map; // the pixmap
} Sprite;

#endif

Sprite *create_sprite(char *pic[], char *bas);

int animate_sprite(Sprite *sp, char *base);

void destroy_sprite(Sprite *sp, char *base);
