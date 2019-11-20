.POSIX:

CFLAGS=-Wall -Wextra -Wpedantic

all: pax cpio tar

pax: pax.o tar.o cpio.o
pax.o: pax.h
tar.o: pax.h
cpio.o: pax.h

tar cpio: pax
	ln -sf pax $@

clean:
	rm -f $$(cat .gitignore)
