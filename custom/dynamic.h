#ifndef CUSTOM_DYNAMIC_H
#define CUSTOM_DYNAMIC_H

#ifdef _WIN32
#include <windows.h>
typedef HINSTANCE dlibrary_t;
#elif defined(__unix__) || defined(__unix) || defined(unix) || defined(__GNUC__)
#include <dlfcn.h> //may require using -ldl
typedef void * dlibrary_t;
#else
#error no implementation of dynamic available
#endif

dlibrary_t dynamic_loadlib(char const * dl_name);

void * dynamic_loadfunc(dlibrary_t dlib, char const * f_name);

void dynamic_closelib(dlibrary_t dlib);

///returns NULL if load fails
dlibrary_t dynamic_tryloadlib(char const * dl_name);

///returns NULL if load fails
void * dynamic_tryloadfunc(dlibrary_t dlib, char const * f_name);

#endif