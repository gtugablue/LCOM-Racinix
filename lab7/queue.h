#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct queue_node_t queue_node_t;
struct queue_node_t{
	void* p;
	queue_node_t* next;
};

typedef struct
{
	queue_node_t* first;
	queue_node_t* last;
	unsigned size;
} queue_t;

queue_t *queue_create();

unsigned queue_size(queue_t* queue);

bool queue_push(queue_t* queue, void* p);

void* queue_pop(queue_t* queue);

void queue_delete(queue_t* queue);

#endif
