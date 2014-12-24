#include "vector.h"

#include <stdlib.h>
#include <string.h>

#define VECTOR_CAPACITY_GROW_RATE 			2
#define VECTOR_CAPACITY_GROW_FACTOR 		10

void vector_create(vector_t* vector)
{
	vector->data = NULL;
	vector->allocated_size = 0;
	vector->item_number = 0;
}

int vector_size(vector_t* vector)
{
	return vector->item_number;
}

int vector_push_back(vector_t* vector, void* data)
{
	if (vector->allocated_size == 0) // Vector empty
	{
		if ((vector->data = malloc(VECTOR_CAPACITY_GROW_FACTOR * sizeof(void *))) == NULL)
		{
			vector_delete(vector);
			return 1;
		}
		vector->allocated_size = VECTOR_CAPACITY_GROW_FACTOR;
	}

	if (vector->allocated_size == vector->item_number) // Vector full
	{
		if ((vector->data = (void**)realloc(vector->data, vector->allocated_size * sizeof(void *))) == NULL)
		{
			vector_delete(vector);
			return 1;
		}
		vector->allocated_size *= VECTOR_CAPACITY_GROW_RATE;
	}

	vector->data[vector->item_number] = data;
	++vector->item_number;
	return 0;
}

void *vector_at(vector_t* vector, int index)
{
	return vector->data[index];
}

void vector_erase(vector_t* vector, int index)
{
	while (index < vector->item_number)
	{
		vector->data[index] = vector->data[++index];
	}
	--vector->item_number;
}


void vector_delete(vector_t* vector)
{
    free(vector->data);
}
