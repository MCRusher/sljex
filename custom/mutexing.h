#ifndef MUTEXING_H
#define MUTEXING_H

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
	typedef void * mutex_t;
#elif defined(unix) || defined(__unix) || defined(__unix__)
	#include <pthread.h>
	typedef pthread_mutex_t mutex_t;
#elif __STDC_VERSION__ >=  201112L && !defined( __STDC_NO_THREADS__)
	#include <threads.h>
	typedef mtx_t mutex_t;
#else
	#error no implementation of mutexing.h is available
#endif

typedef enum mstatus {
	mstatus_failed = false,
	mstatus_success = true,
	mstatus_timeout,
} mstatus;

///returns mstatus_failed(false) on failure, mstatus_success(true) on success
mstatus mutex_init(mutex_t * mtx);
///returns mstatus_failed(false) on failure, mstatus_success(true) on success
mstatus mutex_lock(mutex_t * mtx);
///returns mstatus_failed(false) on failure, mstatus_success(true) on success, 
///mstatus_timeout(true) if timedout
mstatus mutex_timedlock(mutex_t * mtx, uint32_t ms);
///returns mstatus_failed(false) on failure, mstatus_success(true) on success. 
///should only fail when calling thread does not own mutex
mstatus mutex_unlock(mutex_t * mtx);
///returns mstatus_failed(false) on failure, mstatus_success(true) on success
mstatus mutex_delete(mutex_t * mtx);

#endif //MUTEXING_H