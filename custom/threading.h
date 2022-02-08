#ifndef THREADING_H
#define THREADING_H

#include <stdbool.h>

#ifdef _WIN32
#include <stdint.h>
typedef uintptr_t thread_t;
///Required attribute for a function used in thread_start
#define threaded __stdcall
typedef long uniquecall_flag;
#define UNIQUECALL_FLAG_INIT ((long)0)

#elif defined(unix) || defined(__unix) || defined(__unix__)
#include <pthread.h>
typedef pthread_t thread_t;
///required attribute for a function used in thread_start
#define threaded

typedef pthread_once_t uniquecall_flag;
#define UNIQUECALL_FLAG_INIT PTHREAD_ONCE_INIT

#elif __STDC_VERSION__ >=  201112L && !defined( __STDC_NO_THREADS__)
#include <threads.h>
typedef thrd_t thread_t;
///required attribute for a function used in thread_start
#define threaded

typedef once_flag uniquecall_flag;
#define UNIQUECALL_FLAG_INIT ONCE_FLAG_INIT

#else
#error no implementation of threading.h is available
#endif

typedef int(threaded *thread_func_t)(void *);
typedef void(*uniquecall_func_t)(void);

bool thread_start(thread_t * id, thread_func_t func, void * arg);

///returns a thread_t associated with the current thread
thread_t thread_this(void);

///hints the processor to allow other
/// threads to run before the current thread
void thread_yield(void);

///if ret == NULL, it is ignored
bool thread_join(thread_t id, int * ret);

///Separates the execution of the
/// thread from the current thread
bool thread_detach(thread_t id);

///Can be interrupted by signals, etc.
void thread_sleep(uint32_t ms);

///call func only once ever, threadsafe
void uniquecall(uniquecall_flag volatile * flag, uniquecall_func_t func);

#endif