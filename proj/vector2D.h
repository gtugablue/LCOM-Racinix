#ifndef _VECTOR2D_H
#define _VECTOR2D_H

#include "math.h"
#include <stdbool.h>

typedef struct
{
	double x;
	double y;
} vector2D_t;

vector2D_t vectorCreate(double x, double y);

double vectorDistance(vector2D_t vector1, vector2D_t vector2);

vector2D_t vectorAdd(vector2D_t vector1, vector2D_t vector2);

vector2D_t vectorSubtract(vector2D_t vector1, vector2D_t vector2);

vector2D_t vectorMultiply(vector2D_t vector, double factor);

vector2D_t vectorDivide(vector2D_t vector, double factor);

double vectorNorm(vector2D_t vector);

void normalize(vector2D_t* vector);

/**
 * @brief Tests if a given point is inside a given polygon
 *
 * @param polygon array of points defining a polygon
 * @param polygon_size number of edges of the polygon
 * @param point point to be tested
 * @return true if point is inside the polygon, false otherwise
 */
bool isPointInPolygon(vector2D_t polygon[], unsigned polygon_size, vector2D_t* point);

#endif
