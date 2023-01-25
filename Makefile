CC=gcc
PREFIX=/usr/local
CFLAGS=-O2 -pthread --shared -fPIC

.PHONY: all
all : libsljex.so

libsljex.so : sljex.c vector.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean :
	@rm -rf libsljex.so examples/example1 examples/example2 examples/example3

.PHONY: install
install : libsljex.so
	mkdir -p $(PREFIX)/include/sljex/
	cp libsljex.so $(PREFIX)/lib/libsljex.so
	cp sljex.h $(PREFIX)/include/sljex/sljex.h

.PHONY: examples
examples : libsljex.so
	$(CC) examples/example1.c -o examples/example1 -lsljex -L. -Wl,-R..
	$(CC) examples/example2.c -o examples/example2 -lsljex -L. -Wl,-R..
	$(CC) examples/example3.c -o examples/example3 -lsljex -L. -Wl,-R..
