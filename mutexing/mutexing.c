#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../custom/export.h"

#include "mutexing.h"

EXPORTED mstatus mutex_init(mutex_t * mtx);
EXPORTED mstatus mutex_lock(mutex_t * mtx);
EXPORTED mstatus mutex_timedlock(mutex_t * mtx, uint32_t ms);
EXPORTED mstatus mutex_unlock(mutex_t * mtx);
EXPORTED mstatus mutex_delete(mutex_t * mtx);

#ifdef _WIN32
#include <winbase.h>

mstatus mutex_init(mutex_t * mtx) {
    *mtx = CreateMutexA(NULL, 0, NULL);
	if(*mtx == NULL){
		return mstatus_failed;
	}
	return mstatus_success;
}
mstatus mutex_lock(mutex_t * mtx) {
    if(WaitForSingleObject(*mtx, INFINITE) == WAIT_FAILED){
		return mstatus_failed;
    }
	return mstatus_success;
}
mstatus mutex_timedlock(mutex_t * mtx, uint32_t ms) {
	//INFINITE == -1 (UINT32_MAX)
	DWORD status;
	//sets wait time to one less than 
	// specified to avoid infinite wait.
	if(ms == INFINITE) --ms;
	if((status=WaitForSingleObject(*mtx, ms)) == WAIT_FAILED){
		return mstatus_failed;
	}
	if(status == WAIT_TIMEOUT){
		return mstatus_timeout;
	}
	return mstatus_success;
}
mstatus mutex_unlock(mutex_t * mtx) {
    if(ReleaseMutex(*mtx) == 0){
		return mstatus_failed;
    }
	return mstatus_success;
}
mstatus mutex_delete(mutex_t * mtx) {
	if(CloseHandle(*mtx) == 0){
		return mstatus_failed;
	}
	return mstatus_success;
}
#elif defined(unix) || defined(__unix) || defined(__unix__)
#include <time.h>
#include <errno.h>

mstatus mutex_init(mutex_t * mtx) {
	pthread_mutexattr_t mtx_attr;
	if(pthread_mutexattr_init(&mtx_attr) != 0){
		return mstatus_failed;
	}
	if(pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_RECURSIVE) != 0){
		pthread_mutexattr_destroy(&mtx_attr);
		return mstatus_failed;
	}
	if(pthread_mutex_init(mtx,&mtx_attr) != 0){
		pthread_mutexattr_destroy(&mtx_attr);
		return mstatus_failed;
	}
	//can't fail, mtx_attr is sure to be valid
	pthread_mutexattr_destroy(&mtx_attr);
	return mstatus_success;
}
mstatus mutex_lock(mutex_t * mtx) {
    if(pthread_mutex_lock(mtx) != 0){
		return mstatus_failed;
    }
	return mstatus_success;
}
mstatus mutex_timedlock(mutex_t * mtx, uint32_t ms) {
	int status;
	//sets wait time to one less
	// than specified to conform
	// to windows implementation
	if(ms == UINT32_MAX) --ms;
	struct timespec const time_val = {
		.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000L,
	};
	if((status=pthread_mutex_timedlock(mtx, &time_val)) != 0){
		if(status == ETIMEDOUT){
			return mstatus_timeout;
		}else{
			return mstatus_failed;
		}
	}
	return mstatus_success;
}
mstatus mutex_unlock(mutex_t * mtx) {
    if(pthread_mutex_unlock(mtx) != 0){
		return mstatus_failed;
    }
	return mstatus_success;
}
mstatus mutex_delete(mutex_t * mtx) {
    if(pthread_mutex_destroy(mtx) != 0){
		return mstatus_failed;
    }
	return mstatus_success;
}
#elif __STDC_VERSION__ >=  201112L && !defined( __STDC_NO_THREADS__)
mstatus mutex_init(mutex_t * mtx) {
	if(mtx_init(&mtx, mtx_timed | mtx_recursive) != thrd_success){
		return mstatus_failed;
	}
	return mstatus_success;
}
mstatus mutex_lock(mutex_t * mtx) {
    if(mtx_lock(mtx) != thrd_success){
		return mstatus_failed;
    }
	return mstatus_success;
}
mstatus mutex_timedlock(mutex_t * mtx, uint32_t ms) {
	int status;
	//sets wait time to one less
	// than specified to conform
	// to windows implementation
	if(ms == UINT32_MAX) --ms;
	struct timespec const time_val = {
		.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000L,
	};
	if((status=mtx_timedlock(mtx, &time_val)) != thrd_success){
		if(status == thrd_timedout){
			return mstatus_timeout;
		}else{
			return mstatus_failed;
		}
	}
	return mstatus_success;
}
mstatus mutex_unlock(mutex_t * mtx) {
	if(mtx_unlock(mtx) != thrd_success){
		return mstatus_failed;
	}
	return mstatus_success;
}
mstatus mutex_delete(mutex_t * mtx) {
	//no return value to check for failure
	//so it better just work I guess
	mtx_destroy(mtx);
	return mstatus_success;
}
#endif
