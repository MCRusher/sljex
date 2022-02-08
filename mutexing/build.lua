#!/bin/lua
os.execute("gcc -O2 -DEXPORT -pthread -shared -fPIC mutexing.c -o ../libs/libmutexing.so")
