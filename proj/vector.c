#include "vector.h"
#include "utilities.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define RATE_CAPACITY 1.5f
#define SET_CAPACITY_SIZE 10

void vector_new(vector* v)
{
	v->buf = NULL;
	v->size_capacity = 0;
	v->num = 0;
}

void vector_free(vector* v)
{
    free(v->buf);
}

int vector_size(vector* v)
{
	return v->num;
}

int vector_capacity(vector* v)
{
	return v->size_capacity;
}

void vector_push_back(vector* v, void* number)
{
	if (!(v->size_capacity))
	{
		v->size_capacity = SET_CAPACITY_SIZE;

		v->buf = (void**)malloc(v->size_capacity * sizeof(void*));
		memset(v->buf, 0, v->size_capacity * sizeof(void*));
	}

	if (v->size_capacity == v->num)
	{
		v->size_capacity *= RATE_CAPACITY; //
		v->buf = (void**)realloc(v->buf, v->size_capacity * sizeof(void*)); //
	}

	v->buf[v->num] = number;
	v->num++;
}

void vector_insert(vector* v, void* info, int ind)
{
	int p = v->num +1;
	if (v->size_capacity == p)
	{
		v->size_capacity *= RATE_CAPACITY; //
		v->buf = realloc(v->buf, v->size_capacity * sizeof(void*)); //
	}
	int n = v->size;
	for (int i = n; i >= ind; --i)
	{
		v->buf[i + 1] = v->buf[i];
	}

	v->buf[ind] = info;
	v->num++;
}

void* vector_get(vector* v, int ind)
{
	return v->buf[ind];
}

void vector_erase(vector* v, int ind)
{
	int n = v->num -1;
	if (ind != n)
	{
		int i = ind;
		while(i!=n)
		{
			v->buf[i] = v->buf[i+1];
			i++;
		}
	}
	v->buf[n] = NULL;
	(v->num)--;
}
