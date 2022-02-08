#include "vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../custom/export.h"

#define VECTOR_INITIAL 5

EXPORTED bool vector_init(vector * v, bool(*init)(void * *), void(*deinit)(void * *)) {
	assert(v != NULL);
	
	v->count = 0;
	v->_ = malloc(VECTOR_INITIAL * sizeof(void *));
	if(v == NULL){
		return false;
	}
	v->max = VECTOR_INITIAL;
	v->init = init;
	v->deinit = deinit;
	return true;
}

EXPORTED void vector_deinit(vector * v) {
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

EXPORTED bool vector_push(vector * v, void * p) {
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

EXPORTED bool vector_pushInit(vector * v) {
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

EXPORTED void vector_pop(vector * v) {
	assert(v != NULL && v->_ != NULL);
	assert(v->count > 0);
	
	--v->count;
}

EXPORTED void vector_popDeinit(vector * v) {
	assert(v != NULL && v->_ != NULL);
	assert(v->count > 0);
	assert(v->deinit != NULL);
	
	v->deinit(&v->_[--v->count]);
}

EXPORTED void * vector_get(vector * v, size_t index) {
	assert(v != NULL && v->_ != NULL);
	assert(index < v->count);
	
	return v->_[index];
}

EXPORTED void * vector_getLast(vector * v) {
	assert(v != NULL && v->_ != NULL);
	assert(v->count > 0);
	
	return v->_[v->count - 1];
}

EXPORTED size_t vector_size(vector * v) {
	if(v == NULL || v->_ == NULL)
		return 0;
	return v->count;
}
