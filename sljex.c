/*
Copyright (C) 2023 MCRusher

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; version 2.1.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA 
*/

#include "sljex.h"

#include "vector.h"

#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>

//leverage C11 native support for thread local variables,
// should be faster than get/setspecific
#if __STDC_VERSION__ >= 201112L
#define pthread_key_t _Thread_local vector *
#define pthread_key_create(...) 0
#define pthread_getspecific(tlv) tlv
#define pthread_setspecific(tlv, val) (tlv = val, 0)
#define pthread_key_delete(...) (void)0
#endif

///takes a fmt string and variadics, prints to stderr and calls exit(EXIT_FAILURE)
#define panic(...) do{fprintf(stderr, __VA_ARGS__);exit(EXIT_FAILURE);}while(0)

//gcc gives an "error returning array from function"
// when returning jmp_buf, so void * is used instead
//this is fine since the jmp_buf is part of a heap
// allocated struct, and does not go out of scope.
typedef void * jmp_buf_ptr;

bool sljex_init(void);
void sljex_deinit(void);
jmp_buf_ptr sljex_trybuf_(void);
bool sljex_catch_(int excode);
bool sljex_catchany_(void);
jmp_buf_ptr sljex_throwbuf_(int excode, char const * exstr);
jmp_buf_ptr sljex_rethrowbuf_(void);
void sljex_finally_(void);
int sljex_excode(void);
char const * sljex_exstr(void);

static bool sljex_exstate_vinit(void * * statespace);
static void sljex_exstate_vdeinit(void * * statespace);
static bool vector_vinit(void * * vecspace);
static void vector_vdeinit(void * * vecspace);

//holds all the internal information of an exception, one per exception
typedef struct sljex_exstate {
    jmp_buf jb;         //holds the info that setjmp/longjmp uses
    int excode;         //stores the exception code
    char const * exstr; //stores the exception message
    bool caught;        //indicates whether the exception has already been caught
} sljex_exstate;

//holds a reference to a vector<exstate> for each thread,
// allocated and given by global_local_vec_holder
static pthread_key_t tlvec;
//stores the vector<exstate> threadlocal values to destroy all at once
static vector global_local_vec_holder;
//global used to sync pushes to global_local_vec_holder
static pthread_mutex_t mtx;

//initialize the library without automatic atexit cleanup
//pre: this is called exactly once before
//     using any library functions
//post: the library will be ready for use
//returns: false if initialization fails,
//         library is left in an uninitialized state
//note: does not store sljex_deinit on atexit stack,
//      sljex_deinit must be called manually before program exit,
//      and memory will leak to OS if program ends before it is called.
//
//      the library may be loaded by the user at runtime since
//      deinitialization is handled manually.
bool sljex_initNoCleanup(void) {
    if(pthread_mutex_init(&mtx, NULL)){
        return false;
    }else if(pthread_key_create(&tlvec, NULL)){
        pthread_mutex_destroy(&mtx);
        return false;
    }else if(!vector_init(&global_local_vec_holder, vector_vinit, vector_vdeinit)){
        pthread_mutex_destroy(&mtx);
        pthread_key_delete(tlvec);
        return false;
    }
    return true;
}

//initialize the library
//pre: this is called exactly once before
//     using any library functions
//post: the library will be ready for use
//returns: false if initialization fails,
//         library is left in an uninitialized state
//note: stores sljex_deinit on atexit stack, to automatically perform cleanup,
//      do not load this library at runtime with custom/dynamic or similar,
//      or at least do not close the library.
bool sljex_init(void) {
    if(sljex_initNoCleanup()){
        atexit(sljex_deinit);
        return true;
    }
    return false;
}

//deinitializes the library
//note: called automatically using atexit if initialized with sljex_init
//      and not sljex_initNoCleanup
//pre: the library has been initialized exactly once
//post: the library is fully deinitialized
void sljex_deinit(void) {
    vector_deinit(&global_local_vec_holder);
    pthread_key_delete(tlvec);
    pthread_mutex_destroy(&mtx);
}

//returns a jmp_buf reference
//pre: library has been initialized exactly once
//post: a new exstate is pushed to the global stack,
//      and its jmp_buf member is returned as a reference.
//note: calls panic on failure and exits program
//note: the library should be properly deinitialized even upon failure
jmp_buf_ptr sljex_trybuf_(void) {
    //get the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //initialize the thread's exstate vector if it doesn't exist
    if(local_vec == NULL){
        if(pthread_mutex_lock(&mtx)){
            panic("sljex: failed to lock mutex.\n");
        }
        //call panic if adding a new default-initialized
        // vector to the global stack fails
        if(!vector_pushInit(&global_local_vec_holder)){
            //cannot delete a mutex while locked
            pthread_mutex_unlock(&mtx);//should be impossible to fail if lock succeeded
            //calls sljex_deinit through exit, which deletes the mutex
            panic("sljex: failed to allocate exception vector.\n");
        }
        //get a reference to the new vector instance
        local_vec = vector_getLast(&global_local_vec_holder);
        //call util::panic if setting threadlocal storage to
        // the new vector instance reference fails,
        if(pthread_setspecific(tlvec, local_vec)){
            //cannot delete a mutex while locked
            pthread_mutex_unlock(&mtx);//should be impossible to fail if lock succeeded
            //calls sljex_deinit through exit, which deletes the mutex
            panic("sljex: failed to initalize threadlocal exception vector.\n");
        }
        pthread_mutex_unlock(&mtx);//should be impossible to fail if lock succeeded
    }
    
    //create a new default initialzed exstate instance,
    // and call panic if initialization fails
    if(!vector_pushInit(local_vec)){
        panic("sljex: failed to initalize threadlocal exception state.\n");
    }
    //obtain reference to newly created exstate instance
    sljex_exstate * local_state = vector_getLast(local_vec);
    //return a reference the the exstate instance's jump_buf member
    return local_state->jb;
}

//pre: the library has been initialized exactly once
//post: false if excode does not match the current exception's,
//returns: calls panic if called without an associated try statement
//note: will only ever be called when an exception has been thrown
//      by virture of the structure of the library macros
bool sljex_catch_(int excode) {
    //obtain a reference to the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //stores reference to exception state being caught
    sljex_exstate * local_state;
    //Obtain a reference to the current exception state.
    //If there are no exceptions on the stack
    // or the current exception was caught already,
    // then catch was called without a
    // try statement and panic is called
    if(vector_size(local_vec) == 0 || (local_state = vector_getLast(local_vec))->caught){
        panic("sljex: catch without try.\n");
    }
    //return true if the thrown exception's excode
    // matches the excode argument
    if(local_state->excode == excode){
        //sets the current exception's state to caught
        // to avoid accidental recatching
        local_state->caught = true;
        return true;
    }
    //return false otherwise
    return false;
}

//pre: the library has been initialized exactly once,
//post: always returns true
//returns: calls panic if called without an associated try statement
//note: will only ever be called when an exception has been thrown
//      by virture of the structure of the library macros
bool sljex_catchany_(void) {
    //obtain a reference to the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //stores reference to exception state being caught
    sljex_exstate * local_state;
    //Obtain a reference to the current exception state.
    //If there are no exceptions on the stack
    // or the current exception was caught already,
    // then catch was called without a
    // try statement and panic is called
    if(vector_size(local_vec) == 0 || (local_state = vector_getLast(local_vec))->caught){
        panic("sljex: catchany without try.\n");
    }
    //sets the current exception's state to caught
    // to avoid accidental recatching
    local_state->caught = true;
    return true;
}

//returns a jmp_buf reference
//pre: library has been initialized exactly once
//note: calls panic if called outside a try block,
//      intentional behavior that mimics C++'s exception handling, not a failure
//note: the library should be properly deinitialized when panic is called
jmp_buf_ptr sljex_throwbuf_(int excode, char const * exstr) {
    //obtain a reference to the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //discards a previously caught exception
    if(vector_size(local_vec) > 0 && ((sljex_exstate *)vector_getLast(local_vec))->caught){
        vector_popDeinit(local_vec);
    }
    //if there is no valid exstate instance to assign to,
    // then throw was called outside a catch block and is an
    // unhandled exception, and panic is called
    if(vector_size(local_vec) == 0){
        panic("sljex_terminate: unhandled \"%s\"(%d) thrown.\n", exstr, excode);
    }
    //obtain a reference to the current exception state
    sljex_exstate * local_state = vector_getLast(local_vec);
    //assign exception info to exstate
    local_state->excode = excode;
    local_state->exstr = exstr;
    //return a reference to the exstate's jmp_buf member
    return local_state->jb;
}

//returns a jmp_buf reference
//pre: library has been initialized exactly once
//note: fails and calls panic if called outside catch/catchany,
//      also calls panic if called outside a try block,
//      intentional behavior that mimics C++'s exception handling, not a failure.
//note: the library should be properly deinitialized when panic is called
jmp_buf_ptr sljex_rethrowbuf_(void) {
    //obtain a reference to the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //stores reference to current caught, and then new uncaught exception.
    sljex_exstate * local_state;
    //stores current caught exception into local_state
    //if there is no current caught exception to rethrow,
    // rethrow was called outside catch/catchany,
    // and panic is called to report a programmer error.
    if(vector_size(local_vec) == 0 || !(local_state = (sljex_exstate *)vector_getLast(local_vec))->caught){
        panic("sljex: rethrow outside catch/catchany.\n");
    }
    
    int const excode = local_state->excode;
    char const * const exstr = local_state->exstr;
    
    //delete current, caught exception (invalidates local_state)
    vector_popDeinit(local_vec);
    
    //if there is no valid exstate instance to assign to,
    // then rethrow was called outside a catch block and is an
    // unhandled exception, and panic is called
    if(vector_size(local_vec) == 0){
        panic("sljex_terminate: unhandled \"%s\"(%d) thrown.\n", exstr, excode);
    }
    
    //obtain a reference to the new current exception state
    local_state = vector_getLast(local_vec);
    //assign exception info to exstate
    local_state->excode = excode;
    local_state->exstr = exstr;
    //return a reference to the exstate's jmp_buf member
    return local_state->jb;
}

//cleans up exstate in the case that no exceptions were thrown,
// and terminates program if there is an uncaught exception remaining
//pre: library has been initialized exactly once,
//     and finally block follows a try block
//post: returns no value, cleans up exception state
//      created by try
//note: calls panic if called with an active exception state (uncaught exception),
//      intentional behavior that mimics C++'s exception handling, not a failure
void sljex_finally_(void) {
    //obtain a reference to the current thread's exception stack
    vector * local_vec = pthread_getspecific(tlvec);
    //the try & finally macros ensure there is no 
    // easy way to call try and finally unpaired,
    // so the runtime check has been removed.
    //stores reference to exception state being caught
    sljex_exstate * local_state = vector_getLast(local_vec);
    //if the current exstate excode is not 0 and is uncaught,
    // it is an unhandled exception, and panic is called
    if(local_state->excode != 0 && !local_state->caught){
        panic(
            "sljex_terminate: unhandled \"%s\"(%d) thrown.\n",
            local_state->exstr, local_state->excode
        );
    }
    //cleans up exstate created by try
    vector_popDeinit(local_vec);
}

//pre: library has been initialized exactly once,
//     and the function is called inside a catch/catchany block
//post: returns the integer code representing the exception type,
//      fails and calls panic if called outside catch/catchany
int sljex_excode(void) {
    vector * local_vec = pthread_getspecific(tlvec);
    //stores reference to exception state being caught
    sljex_exstate * local_state;
    //if there is no valid exstate instance to access,
    // then sljex_excode was called outside a catch block
    // and panic is called to report a programmer error
    if(vector_size(local_vec) == 0 || !(local_state = vector_getLast(local_vec))->caught){
        panic("sljex: sljex_excode outside catch/catchany.\n");
    }
    //return the excode of the current exception
    return local_state->excode;
}

//pre: library has been initialized exactly once,
//     and the function is called inside a catch/catchany block
//post: returns the string value accompanying the exception type,
//      fails and calls panic if called outside catch/catchany
char const * sljex_exstr(void) {
    vector * local_vec = pthread_getspecific(tlvec);
    //stores reference to exception state being caught
    sljex_exstate * local_state;
    //if there is no valid exstate instance to access,
    // then sljex_excode was called outside a catch block
    // and panic is called to report a programmer error
    if(vector_size(local_vec) == 0 || !(local_state = vector_getLast(local_vec))->caught){
        panic("sljex: sljex_exstr outside catch/catchany.\n");
    }
    //return the exstr of the current exception
    return local_state->exstr;
}

//allocates a new exstate in-place
//pre: statespace represents an unallocated exstate slot in a vector
//post: statespace is assigned a new valid, allocated exspace instance
//returns: false if fails to initialize, statespace is unchanged
bool sljex_exstate_vinit(void * * statespace) {
    //allocate memory for the new exstate
    sljex_exstate * sp = malloc(sizeof(sljex_exstate));
    if(sp == NULL){
        return false;
    }
    //initialize members to show that exstate
    // does not currently hold an exception.
    sp->excode = 0;//excode 0 means not-an-exception
    sp->caught = false;
    //assign the new exstate to the reference
    // and return indicating success
    *statespace = sp;
    return true;
}

//deinitializes an exstate in-place
//pre: statespace is a valid, allocated exstate instance
//post: statespace's instance will be deallocated
static void sljex_exstate_vdeinit(void * * statespace) {
    free(*statespace);
}

//allocates a new vector in-place
//pre: vecspace represents an unallocated vector slot in another vector
//post: vecspace is assigned a new valid, allocated vector instance
//returns: false if fails to initialize, statespace is unchanged
static bool vector_vinit(void * * vecspace) {
    //allocate memory for vector
    vector * vp = malloc(sizeof(vector));
    //if allocation or initialization fails,
    // return indicating failure
    if(vp == NULL || !vector_init(vp, sljex_exstate_vinit, sljex_exstate_vdeinit)){
        return false;
    }
    *vecspace = vp;
    return true;
}

//deinitializes an vector instance in-place
//pre: vecspace is a valid, allocated vector instance
//post: vecspace's instance will be deallocated
static void vector_vdeinit(void * * vecspace) {
    vector_deinit(*vecspace);
}
