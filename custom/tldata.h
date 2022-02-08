#ifndef TLDATA_H
#define TLDATA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
typedef DWORD tldata_t;
#elif defined(unix) || defined(__unix) || defined(__unix__)
#include <pthread.h>
typedef pthread_key_t tldata_t;
#elif __STDC_VERSION__ >=  201112L && !defined( __STDC_NO_THREADS__)
#include <threads.h>
typedef tss_t tldata_t;
#endif

bool tldata_init(tldata_t * tld);
bool tldata_deinit(tldata_t * tld);
void * tldata_get(tldata_t tld);
bool tldata_set(tldata_t tld, void * data);

#endif

