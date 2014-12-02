#ifndef _FONT_H
#define _FONT_H

#include "video_gr.h"

typedef struct
{
	// Lower case
	bitmap_t *char_a;
	bitmap_t *char_b;
	bitmap_t *char_c;
	bitmap_t *char_d;
	bitmap_t *char_e;
	bitmap_t *char_f;
	bitmap_t *char_g;
	bitmap_t *char_h;
	bitmap_t *char_i;
	bitmap_t *char_j;
	bitmap_t *char_k;
	bitmap_t *char_l;
	bitmap_t *char_m;
	bitmap_t *char_n;
	bitmap_t *char_o;
	bitmap_t *char_p;
	bitmap_t *char_q;
	bitmap_t *char_r;
	bitmap_t *char_s;
	bitmap_t *char_t;
	bitmap_t *char_u;
	bitmap_t *char_v;
	bitmap_t *char_w;
	bitmap_t *char_x;
	bitmap_t *char_y;
	bitmap_t *char_z;

	// Upper case
	bitmap_t *char_A;
	bitmap_t *char_B;
	bitmap_t *char_C;
	bitmap_t *char_D;
	bitmap_t *char_E;
	bitmap_t *char_F;
	bitmap_t *char_G;
	bitmap_t *char_H;
	bitmap_t *char_I;
	bitmap_t *char_J;
	bitmap_t *char_K;
	bitmap_t *char_L;
	bitmap_t *char_M;
	bitmap_t *char_N;
	bitmap_t *char_O;
	bitmap_t *char_P;
	bitmap_t *char_Q;
	bitmap_t *char_R;
	bitmap_t *char_S;
	bitmap_t *char_T;
	bitmap_t *char_U;
	bitmap_t *char_V;
	bitmap_t *char_W;
	bitmap_t *char_X;
	bitmap_t *char_Y;
	bitmap_t *char_Z;

	// Digits
	bitmap_t *char_0;
	bitmap_t *char_1;
	bitmap_t *char_2;
	bitmap_t *char_3;
	bitmap_t *char_4;
	bitmap_t *char_5;
	bitmap_t *char_6;
	bitmap_t *char_7;
	bitmap_t *char_8;
	bitmap_t *char_9;
} font_t;

font_t *font_load(char* folder);

void font_delete(font_t *font);

#endif
