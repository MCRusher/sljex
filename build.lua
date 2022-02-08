#!/bin/lua
os.execute("gcc -O2 -DEXPORT -shared -fPIC sljex.c ./vector/vector.o -o ./libs/libsljex.so -Llibs -lmutexing -ltldata")
