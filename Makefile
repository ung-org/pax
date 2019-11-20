.POSIX:

CFLAGS=-Wall -Wextra -Wpedantic

pax: pax.o tar.o cpio.o

clean:
	rm -f $$(cat .gitignore)
