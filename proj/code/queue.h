#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct queue_node_t queue_node_t;

/** @name Queue node */
/** @{
 *
 * Queue node structure
 */
struct queue_node_t{
	void* p;
	queue_node_t* next;
};
/** @} end of queue_node_t */

/** @name Queue */
/** @{
 *
 * Queue structure
 */
typedef struct
{
	queue_node_t* first;
	queue_node_t* last;
	unsigned size;
} queue_t;
/** @} end of queue_t */

/**
 * @brief Creates the queue
 *
 * @return A pointer to the queue created
 */
queue_t *queue_create();

/**
 * @brief The size of a queue
 *
 * @param queue the queue that is measured
 *
 * @return The size of the queue passed as argument
 */
unsigned queue_size(queue_t* queue);

/**
 * @brief Sees if the queue size is zero
 *
 * @param queue the queue that will be worked
 *
 * @return Returns true on sucess, false otherwise
 */
bool queue_empty(queue_t* queue);

// For debugging char queues
/**
 * @brief Print the queue
 *
 * @param queue the queue that will be printed
 */
void queue_print(queue_t* queue);

/**
 * @brief Push an item to the queue
 *
 * Push of the item that is given as parameter into the queue, also given
 *
 * @param queue the queue that will be worked
 * @param p the element that will be inserted in the queue
 *
 * @return Returns true on sucess, false otherwise
 */
bool queue_push(queue_t* queue, void* p);

/**
 * @brief Pop the first element of the queue
 *
 * @param queue the queue that will be worked
 *
 */
void* queue_pop(queue_t* queue);

/**
 * @brief Delete the queue
 *
 * @param queue the queue that will be worked
 */
void queue_delete(queue_t* queue);
/** @} end of Queue */

#endif
