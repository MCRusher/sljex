#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdbool.h>

typedef struct vector {
	void * * _;
	size_t count;
	size_t max;
	bool(*init)(void * *);
	void(*deinit)(void * *);
} vector;

bool vector_init(vector * v, bool(*init)(void * *), void(*deinit)(void * *));
void vector_deinit(vector * v);
bool vector_push(vector * v, void * p);
bool vector_pushInit(vector * v);
void vector_pop(vector * v);
void vector_popDeinit(vector * v);
void * vector_get(vector * v, size_t index);
void * vector_getLast(vector * v);
size_t vector_size(vector * v);

#endif