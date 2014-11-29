#ifndef _VECTOR_H
#define _VECTOR_H

#include "minix/syslib.h"

typedef struct {
	void** data;
	size_t allocated_size;
	unsigned item_number;
} vector_t;

void vector_create(vector_t* v);

int vector_size(vector_t* v);

int vector_push_back(vector_t* vector, void* data);

void vector_insert(vector_t* v, void* info, int index);

void *vector_at(vector_t* v, int index);

void vector_erase(vector_t* v, int index);

void vector_delete(vector_t* v);

#endif
