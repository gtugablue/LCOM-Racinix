#include "sprite.h"
#include <stdlib.h>
#include "xpm.h"
#include "video_gr.h"

static int draw_sprite(Sprite *sp);

Sprite *create_sprite(char *pic[], unsigned x, unsigned y, double velocity_x, double velocity_y, unsigned h_res, unsigned v_res)
{
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height), h_res, v_res);
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}
	sp->position = vectorCreate(x, y);
	sp->velocity = vectorCreate(velocity_x, velocity_y);
	return sp;
}

int clear_sprite_area(Sprite *sp)
{
	vg_draw_rectangle((int)sp->position.x, (int)sp->position.y, sp->width, sp->height, VIDEO_GR_256_BLACK);
}

int animate_sprite(Sprite *sp, double delta_time)
{
	sp->position = vectorAdd(sp->position, vectorMultiply(sp->velocity, delta_time));
	return draw_sprite(sp);
}

static int draw_sprite(Sprite *sp)
{
	return vg_draw_pixmap((int)sp->position.x, (int)sp->position.y, sp->map, sp->width, sp->height);
}

void destroy_sprite(Sprite *sp)
{
	if( sp == NULL )
		return;
	free(sp->map);
	free(sp);
}
