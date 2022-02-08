#ifndef CUSTOM_ATOMICS_H
#define CUSTOM_ATOMICS_H

#include <stdint.h>
#include <stdbool.h>

#include <custom/os.h>

#ifdef OS_WINDOWS
	typedef long atomic32_t;
	#define atomic32_LOCKFREE ((bool)true)
#else
	#include <custom/mutexing.h>

	typedef struct atomic32_t {
		mutex_t mtx;
		int32_t val;
	} atomic32_t;
	#define atomic32_LOCKFREE ((bool)false)
#endif

///initialization should be performed only once, and is not atomic
bool atomic32_init(atomic32_t * atom, int32_t val);

///the store to val is not atomic, so val should not
/// be used in multiple threads
bool atomic32_get(atomic32_t volatile * dest, int32_t * val);

bool atomic32_set(atomic32_t volatile * dest, int32_t val);
///the read and store to val are not atomic, so val should not
/// be used in multiple threads
bool atomic32_xchg(atomic32_t volatile * dest, int32_t * val);
///frees resources used by locking implementations
bool atomic32_delete(atomic32_t * atom);

#endif