#ifndef CUSTOM_VARGS_H
#define CUSTOM_VARGS_H

#include <stdarg.h>
#include <stdlib.h>

//weakly typechecks (for compatibility) as a side-affect
// promotion rules are still in effect,
// func(vargsSize(int, 1LL), 1LL) will pass but
// in reality, the argument is a long long, not an int.
// trying to get the varg as an int will cause problems
#define vargsSize(T, ...)\
    (sizeof((T const[]){__VA_ARGS__}))

#define vargsCount(T, ...)\
    (vargsSize(T, __VA_ARGS__) / sizeof(T))

//stores vargs into provided array
// array is assumed to be large enough
#define vargsCollect(T, arr, count_arg)\
do{\
    va_list vl;\
    va_start(vl, count_arg);\
    for(size_t i = 0; i < count_arg; i++){\
        arr[i] = va_arg(vl, T);\
    }\
    va_end(vl);\
}while(0)

//uses a VLA to allocate the vargs array on the stack
// VLA is identified by the arr_name argument identifier
#define vargsMakeStack(T, arr_name, count_arg)\
T arr_name[count_arg];\
vargsCollect(T, arr_name, count_arg)

//uses malloc to allocate the vargs array on the heap
// VLA is identified by the arr_name argument identifier
#define vargsMakeHeap(T, arr_name, count_arg)\
T * arr_name = malloc(count_arg * sizeof(T));\
do{\
    if(arr_name != NULL){\
        vargsCollect(T, arr_name, count_arg);\
    }\
}while(0)

#endif