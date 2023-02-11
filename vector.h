///@file
///@internal
/*
Copyright (C) 2023 MCRusher

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; version 2.1.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA 
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdbool.h>

///a growable stack that stores elements by reference
typedef struct vector {
    ///element buffer
    void * * data;
    ///number of elements
    size_t count;
    ///capacity of vector
    size_t max;
    ///optional initializer
    bool(*init)(void * *);
    ///optional deinitializer
    void(*deinit)(void * *);
} vector;

///initialize vector with optional initializer and deinitializer
bool vector_init(vector * v, bool(*init)(void * *), void(*deinit)(void * *));

///deinitializer vector, calling deinitializer on remaining elements if provided
void vector_deinit(vector * v);

///push new element to end of vector
bool vector_push(vector * v, void * p);

///push new element to end of vector using initializer
bool vector_pushInit(vector * v);

///remove element from end of vector
void vector_pop(vector * v);

///remove element from end of vector, calling deinitializer if provided
void vector_popDeinit(vector * v);

///get an element from vector
void * vector_get(vector * v, size_t index);

///get the last element from vector
void * vector_getLast(vector * v);

///get the size of the vector
size_t vector_size(vector * v);

#endif
