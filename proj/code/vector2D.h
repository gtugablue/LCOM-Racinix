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
 * @brief Creates the vector
 *
 * @param x x coordinate of the vector
 * @param y y coordinate of the vector
 *
 * @return The vector created
 */
vector2D_t vectorCreate(double x, double y);

/**
 * @brief Calculate distance between vectors
 *
 * @param vector1 vector1 info (coordinates)
 * @param vector2 vector2 info (coordinates)
 *
 * @return The distance between two vectors given
 */
double vectorDistance(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief Add a vector
 *
 * Add a vector to another vector (sum)
 *
 * @param vector1 vector1 info
 * @param vector2 vector2 info
 *
 * @return Resulting vector of the sum
 */
vector2D_t vectorAdd(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief Subtract a vector
 *
 * Subtract a vector to another vector (subtraction)
 *
 * @param vector1 vector1 info
 * @param vector2 vector2 info
 *
 * @return Resulting vector of the subtraction
 */
vector2D_t vectorSubtract(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief Multiply a vector
 *
 * Multiply a vector by a factor (multiplication)
 *
 * @param vector vector inf
 * @param factor factor that will multiply with the vector
 *
 * @return Resulting vector of the multiplication
 */
vector2D_t vectorMultiply(vector2D_t vector, double factor);

/**
 * @brief Divides a vector
 *
 * Divides a vector by a factor
 *
 * @param vector vector info
 * @param factor factor that will divide by the vector given
 *
 * @return Resulting vector
 */
vector2D_t vectorDivide(vector2D_t vector, double factor);

/**
 * @brief Rotates the vector
 *
 * @param vector vector info
 * @param degrees degrees by which the vector will rotate
 *
 * @return Resulting vector
 */
vector2D_t vectorRotate(vector2D_t vector, double degrees);

/**
 * @brief Calculate the normal of the vector
 *
 * @param vector vector info
 *
 * @return Return the normal
 */
double vectorNorm(vector2D_t vector);

/**
 * @brief Calculate the angle between two vectors
 *
 * @param vector1 vector1 info
 * @param vector2 vector2 info
 *
 * @return Return the angle
 */
double vectorAngle(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief Calculate the scalar product of the two vectors
 *
 * @param vector1 vector1 info
 * @param vector2 vector2 info
 *
 * @return Return the scalar product
 */
double vectorScalarProduct(vector2D_t vector1, vector2D_t vector2);

// Dot product between vector1 and a vector perpendicular to vector2
/**
 * @brief Calculate the perpendicular dot from the vector
 *
 * @param vector1 vector1 info
 * @param vector2 vector2 info
 *
 * @return Return dot product between vector1 and a vector perpendicular to vector2
 */
double vectorPerpendicularDotProduct(vector2D_t vector1, vector2D_t vector2);

/**
 * @brief Put the vector's norm to 1
 *
 * @param vector vector info
 */
void vectorNormalize(vector2D_t* vector);

/**
 * @brief Calculates if the point is in the rectangle
 *
 * @param top_left_corner the top left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param point point that will be compared
 *
 * @return Return true on success, false otherwise
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
