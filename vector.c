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

///determines starting size of a vector when initialized with vector_init
#define VECTOR_INITIAL 5

/**
    initializes a vector with an optional initializer/deinitializer function
    that can be used by vector_pushInit/vector_popDeinit
@pre
    v is a reference to an uninitialized vector
@post
    v is an initialized vector with at least VECTOR_INITIAL starting capacity
@returns
    false if fails to initialize, vector is unchanged
*/
bool vector_init(vector * v, bool(*init)(void * *), void(*deinit)(void * *)) {
    assert(v != NULL);
    
    v->_ = malloc(VECTOR_INITIAL * sizeof(void *));
    if(v->_ == NULL){
        return false;
    }
    v->count = 0;
    v->max = VECTOR_INITIAL;
    v->init = init;
    v->deinit = deinit;
    
    return true;
}

/**
    deinitializes a vector, deinitializing all remaining elements
@pre
    v is a reference to an initialized vector
@post
    v is an uninitialized vector
*/
void vector_deinit(vector * v) {
    assert(v != NULL);
    
    //prevent deinit from being called on
    // an already uninitialized vector
    if(v->_ != NULL){
        //if the vector has a deinitializer, deinit all elements
        if(v->deinit != NULL){
            for(size_t i = 0; i < v->count; i++){
                v->deinit(&v->_[i]);
            }
        }
        free(v->_);
        v->_ = NULL;
    }
}

/**
    add an element to the end of the vector
@pre
    v is a reference to an initialized vector
@post
    v's element count increases by one,
    reallocating if necessary, and the element is added
@returns
    false if reallocation fails
*/
bool vector_push(vector * v, void * p) {
    assert(v != NULL && v->_ != NULL);
    assert(p != NULL);
    
    //grow vector capacity by 2x if full
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

/**
    add an element to the end of the vector,
    using the default initializer instead of an explicit value
@pre
    v is a reference to an initialized vector
@post
    v's element count increases by one,
    reallocating if necessary, and the element is added
@returns
    false if reallocation or initializer fails
*/
bool vector_pushInit(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->init != NULL);
    
    //grow vector capacity by 2x if full
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

/**
    remove an element from the end of the vector
@pre
    v is a reference to an initialized vector
@post
    v's element count decreases by one
*/
void vector_pop(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    
    --v->count;
}

/**
    remove an element from the end of the vector,
    calling the deinitializer on the element
@pre
    v is a reference to an initialized vector
@post
    v's element count decreases by one,
    and the element has been deinitialized
@note
    if the deinitializer frees the element,
    any reference to the element becomes invalid
*/
void vector_popDeinit(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    assert(v->deinit != NULL);
    
    v->deinit(&v->_[--v->count]);
}

/**
    get an element from the vector
@pre
    v is a reference to an initialized vector,
    index is less than the size of the vector
@returns
    the element in v at the index
*/
void * vector_get(vector * v, size_t index) {
    assert(v != NULL && v->_ != NULL);
    assert(index < v->count);
    
    return v->_[index];
}

/**
    get the last element from the vector
@pre
    v is a reference to an initialized vector,
    v has at least one element
@returns
    the last element in v
*/
void * vector_getLast(vector * v) {
    assert(v != NULL && v->_ != NULL);
    assert(v->count > 0);
    
    return v->_[v->count - 1];
}

/**
    get the size of the vector
@pre
    v is a reference to an initialized vector,
    a deinitialized vector, or a NULL pointer
@returns
    0 if v is NULL or if v has been deinitialized,
    otherwise returns the element count of v
*/
size_t vector_size(vector * v) {
    if(v == NULL || v->_ == NULL)
        return 0;
    
    return v->count;
}
