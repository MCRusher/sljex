/*
Copyright (C) 2023 MCRusher

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; version 2.1.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA 
*/

#include "vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define VECTOR_INITIAL 5

bool vector_init(vector * v, bool(*init)(void * *), void(*deinit)(void * *)) {
    assert(v != NULL);
    
    v->count = 0;
    v->_ = malloc(VECTOR_INITIAL * sizeof(void *));
    if(v->_ == NULL){
        return false;
    }
    v->max = VECTOR_INITIAL;
    v->init = init;
    v->deinit = deinit;
    return true;
}

void vector_deinit(vector * v) {
    assert(v != NULL);
    if(v->_ != NULL){
        if(v->deinit != NULL){
            for(size_t i = 0; i < v->count; i++){
                v->deinit(&v->_[i]);
            }
        }
        free(v->_);
        v->_ = NULL;
    }
}

bool vector_push(vector * v, void * p) {
    assert(v != NULL && v->_ != NULL);
    assert(p != NULL);
    
    if(v->count == v->max){
        v->max *= 2;
        void * tmp = realloc(v->_, v->max * sizeof(void *));
        if(tmp == NULL){
            return false;
        }
        v->_ = tmp;
    }
    v->_[v->count++] = p;
    return true;
}

bool vector_pushInit(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->init != NULL);
    
    if(v->count == v->max){
        v->max *= 2;
        void * tmp = realloc(v->_, v->max * sizeof(void *));
        if(tmp == NULL){
            return false;
        }
        v->_ = tmp;
    }
    return v->init(&v->_[v->count++]);
}

void vector_pop(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    
    --v->count;
}

void vector_popDeinit(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    assert(v->deinit != NULL);
    
    v->deinit(&v->_[--v->count]);
}

void * vector_get(vector * v, size_t index) {
    assert(v != NULL && v->_ != NULL);
    assert(index < v->count);
    
    return v->_[index];
}

void * vector_getLast(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    
    return v->_[v->count - 1];
}

size_t vector_size(vector * v) {
    if(v == NULL || v->_ == NULL)
        return 0;
    return v->count;
}
