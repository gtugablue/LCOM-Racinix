#ifndef _VECTOR2D_H
#define _VECTOR2D_H

#include "math.h"
#include <stdbool.h>

/** @defgroup vector2D Vector2D Module
 * @{
 *
 * Module responsible for make arithmetic with vectors much easier
 *
 */


typedef struct
{
	double x;
	double y;
} vector2D_t;

/**
 * @brief
 *
 * @param x
 * @param y
 *
 * @return
 */
vector2D_t vectorCreate(double x, double y);

/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
double vectorDistance(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
vector2D_t vectorAdd(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
vector2D_t vectorSubtract(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief
 *
 * @param vector
 * @param factor
 *
 * @return
 */
vector2D_t vectorMultiply(vector2D_t vector, double factor);

/**
 * @brief
 *
 * @param vector
 * @param factor
 *
 * @return
 */
vector2D_t vectorDivide(vector2D_t vector, double factor);

/**
 * @brief
 *
 * @param vector
 * @param degrees
 *
 * @return
 */
vector2D_t vectorRotate(vector2D_t vector, double degrees);

/**
 * @brief
 *
 * @param vector
 *
 * @return
 */
double vectorNorm(vector2D_t vector);

/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
double vectorAngle(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
double vectorScalarProduct(vector2D_t vector1, vector2D_t vector2);

// Dot product between vector1 and a vector perpendicular to vector2
/**
 * @brief
 *
 * @param vector1
 * @param vector2
 *
 * @return
 */
double vectorPerpendicularDotProduct(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief
 *
 * @param vector
 */
void vectorNormalize(vector2D_t* vector);

/**
 * @brief
 *
 * @param top_left_corner
 * @param width
 * @param height
 * @param point
 *
 * @return
 */
bool isPointInAxisAlignedRectangle(vector2D_t top_left_corner, unsigned width, unsigned height, vector2D_t point);

/**
 * @brief Tests if a given point is inside a given polygon
 *
 * @param polygon array of points defining a polygon
 * @param polygon_size number of edges of the polygon
 * @param point point to be tested
 * @return true if point is inside the polygon, false otherwise
 */
bool isPointInPolygon(vector2D_t polygon[], unsigned polygon_size, vector2D_t point);

/** @} end of vector2D */
#endif
