#include "vector2D.h"

vector2D_t vectorCreate(double x, double y)
{
	vector2D_t vector;
	vector.x = x;
	vector.y = y;
	return vector;
}

double vectorDistance(vector2D_t vector1, vector2D_t vector2)
{
	return vectorNorm(vectorSubtract(vector1, vector2));
}

vector2D_t vectorAdd(vector2D_t vector1, vector2D_t vector2)
{
	vector1.x += vector2.x;
	vector1.y += vector2.y;
	return vector1;
}

vector2D_t vectorSubtract(vector2D_t vector1, vector2D_t vector2)
{
	vector1.x -= vector2.x;
	vector1.y -= vector2.y;
	return vector1;
}

vector2D_t vectorMultiply(vector2D_t vector, double factor)
{
	printf("vector.x = %d, factor = %d, vector.x * factor = %d\n", (int)vector.x, (int)factor, (int)vector.x * factor);
	vector.x *= factor;
	vector.y *= factor;
	return vector;
}

vector2D_t vectorDivide(vector2D_t vector, double factor)
{
	vector.x /= factor;
	vector.y /= factor;
	return vector;
}

double vectorNorm(vector2D_t vector)
{
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2));
}

void normalize(vector2D_t* vector)
{
	double norm = vectorNorm(*vector);
	vector->x /= norm;
	vector->y /= norm;
	return;
}
