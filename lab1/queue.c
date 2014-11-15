#include "queue.h"

queue_t *queue_create()
{
	queue_t *queue = malloc(sizeof(queue_t));
	if (queue == NULL)
	{
		return NULL;
	}
	queue->size = 0;
	queue->first = NULL;
	queue->last = NULL;
	return queue;
}

unsigned queue_size(queue_t* queue)
{
	return queue->size;
}

bool queue_push_back(queue_t* queue, void* p)
{
	queue_node_t* node;
	if ((node = malloc(sizeof(queue_node_t))) == NULL)
	{
		return false;
	}
	node->p = p;
	node->next = NULL;
	if (queue->size == 0)
	{
		queue->first = queue->last = node;
	}
	else
	{
		queue->last->next = node;
	}
	queue->size++;
	return true;
}

void* queue_pop_front(queue_t* queue)
{
	void* p = queue->first->p;
	queue->first = queue->first->next;
	queue->size--;
	return p;
}

void queue_delete(queue_t* queue)
{
	queue_node_t* queue_node;
	size_t i;
	for (i = 0; i < queue_size(queue); ++i)
	{
		queue_node = queue->first->next;
		free(queue->first);
	}
	return;
}