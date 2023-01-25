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
