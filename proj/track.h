#ifndef _TRACK_H
#define _TRACK_H

#include <stdlib.h>
#include "vector2D.h"
#include <stdbool.h>
#include "math.h"
#include "video_gr.h"

#define TRACK_INTERP_PERIOD	0.07

// TODO - fazer esta função retornar uma array com os pontos pertencentes à pista, em vez de desenhar diretamente.
void track_generate(unsigned width, unsigned height, unsigned long seed);

#endif
