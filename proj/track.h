#ifndef _TRACK_H
#define _TRACK_H

#include <stdlib.h>
#include "vector2D.h"
#include <stdbool.h>
#include "math.h"
#include "video_gr.h"
#include <stdbool.h>

#define TRACK_INTERP_PERIOD	0.07

// TODO - fazer esta função retornar uma array com os pontos pertencentes à pista, em vez de desenhar diretamente.
bool *track_generate(unsigned width, unsigned height, unsigned long seed);

void track_draw(bool *track, unsigned width, unsigned height);

#endif
