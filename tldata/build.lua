#!/bin/lua
os.execute("gcc -O2 -DEXPORT -pthread -shared -fPIC tldata.c -o ../libs/libtldata.so")
