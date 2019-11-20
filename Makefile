.POSIX:

CFLAGS=-Wall -Wextra -Wpedantic

all: pax cpio tar

pax: pax.o tar.o cpio.o

tar cpio: pax
	ln -sf pax $@

clean:
	rm -f $$(cat .gitignore)
