#include "vector2D.h"

#define PI 					3.14159265358979323846

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

vector2D_t vectorRotate(vector2D_t vector, double degrees)
{
	double temp;
	temp = vector.x;
	vector.x = vector.x * cos(degrees) - vector.y * sin(degrees);
	vector.y = temp * sin(degrees) + vector.y * cos(degrees);
	return vector;
}

double vectorNorm(vector2D_t vector)
{
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2));
}

double vectorAngle(vector2D_t vector1, vector2D_t vector2)
{
	return acos(vectorScalarProduct(vector1, vector2) / (vectorNorm(vector1) * vectorNorm(vector2)));
}

double vectorScalarProduct(vector2D_t vector1, vector2D_t vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y;
}

double vectorPerpendicularDotProduct(vector2D_t vector1, vector2D_t vector2)
{
	return vector1.x * vector2.y - vector1.y * vector2.x;
}

void vectorNormalize(vector2D_t* vector)
{
	double norm = vectorNorm(*vector);
	vector->x /= norm;
	vector->y /= norm;
	return;
}

bool isPointInAxisAlignedRectangle(vector2D_t top_left_corner, unsigned width, unsigned height, vector2D_t point)
{
	if (point.x >= top_left_corner.x && point.y >= top_left_corner.y && point.x <= top_left_corner.x + width && point.y <= top_left_corner.y + height)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isPointInPolygon(vector2D_t polygon[], unsigned polygon_size, vector2D_t point)
{
	/*
	Copyright (c) 1970-2003, Wm. Randolph Franklin

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

		1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
		2. Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
		3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	*/

	int i, j;
	bool c = false;
	for (i = 0, j = polygon_size - 1; i < polygon_size; j = i++) {
		if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) && (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
		{
			c = !c;
		}
	}
	return c;
}
