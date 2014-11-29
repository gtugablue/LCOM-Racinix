#ifndef VECTOR_H
#define VECTOR_H

/** @defgroup vector vector
 * @{
 * Functions to manage vector ADT
 */

typedef struct {
    void** buf; // Data
    int size_capacity; // Allocated size of elements
    int num; // Number of elements
} vector;

void vector_new(vector* v); // Create a new vector
void vector_free(vector* v); // Delete all elements of the vector
int vector_size(vector* v); // Size of vector
int vector_capacity(vector* v); // Capacity of the vector
void vector_push_back(vector* v, void* number); // Adds element to back of vector
void vector_insert(vector* v, void* info, int ind); // Adds element at position index
void* vector_get(vector* v, int ind); // Element at position index
void vector_erase(vector* v, int ind); // Removes element at position index

/**@}*/

#endif // VECTOR_H
