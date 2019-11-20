/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _XOPEN_SOURCE 700
#include <cpio.h>
#include <errno.h>
#include <libgen.h>
#include <locale.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <tar.h>
#include <unistd.h>

#include "pax.h"

enum pax_format pax_identify(size_t n, void *header)
{
	if (n < sizeof(MAGIC)) {
		return PAX_FORMAT_UNKNOWN;
	}

	if (n >= cpio_header_size) {
		if (memcmp(header, MAGIC, sizeof(MAGIC) - 1) == 0) {
			return PAX_FORMAT_CPIO;
		}
	}

	/* TODO: differentiate between tar and pax format */
	if (n >= TAR_HEADER_SIZE) {
		struct tar_header *th = header;
		if (memcmp(th->magic, TMAGIC, TMAGLEN) == 0) {
			return PAX_FORMAT_TAR;
		}
	}

	fprintf(stderr, "pax: unknown archive format\n");
	return PAX_FORMAT_UNKNOWN;
}

int pax_list(FILE *input)
{
	char header[TAR_HEADER_SIZE];
	int nread = fread(header, 1, sizeof(header), input);

	switch (pax_identify(nread, header)) {
	case PAX_FORMAT_CPIO:
		return cpio_list(input, nread, header);
		break;

	case PAX_FORMAT_TAR:
		return tar_list(input, nread, header);
		break;

	default:
		break;
	}

	fprintf(stderr, "pax: unknown file format\n");
	return 1;
}

int pax_main(int argc, char *argv[])
{
	unsigned int flags = 0;
	//unsigned int blocksize = 0;
	enum { PAX_LIST, PAX_READ, PAX_WRITE, PAX_COPY } mode = PAX_LIST;
	//enum { PAX_EXCLUDE, PAX_INCLUDE } match = -1;
	//enum { FOLLOW, NOFOLLOW } symlinks = -1;
	//char *format = NULL;
	//char *options = NULL;
	//char *replace = NULL;
	//char *archive = NULL;
	//char *preserve = NULL;

	setlocale(LC_ALL, "");

	int c;
	while ((c = getopt(argc, argv, "rwab:cdf:HikLno:p:s:tuvx:X")) != -1) {
		switch (c) {
		case 'r':
			if (mode == PAX_WRITE || mode == PAX_COPY) {
				mode = PAX_COPY;
			} else {
				mode = PAX_READ;
			}
			break;

		case 'w':
			if (mode == PAX_READ || mode == PAX_COPY) {
				mode = PAX_COPY;
			} else {
				mode = PAX_WRITE;
			}
			break;

		case 'a':
			flags |= PAX_APPEND;
			break;

		case 'b':
			//blocksize = atoi(optarg);
			break;

		case 'c':
			//match = PAX_EXCLUDE;
			break;

		case 'd':
			flags |= PAX_DIRECTORY;
			break;

		case 'f':
			//archive = optarg;
			break;

		case 'H':
			//symlinks = FOLLOW;
			break;

		case 'i':
			flags |= PAX_INTERACTIVE;
			break;

		case 'k':
			flags |= PAX_KEEP;
			break;

		case 'l':
			flags |= PAX_LINK;
			break;

		case 'L':
			//symlinks = NOFOLLOW;
			break;

		case 'n':
			//match = PAX_INCLUDE;
			break;

		case 'o':
			//options = optarg;
			break;

		case 'p':
			//preserve = optarg;
			break;

		case 's':
			//replace = optarg;
			break;

		case 't':
			flags |= PAX_TIMESTAMP;
			break;

		case 'u':
			flags |= PAX_UPDATE;
			break;

		case 'v':
			flags |= PAX_VERBOSE;
			break;

		case 'x':
			//format = optarg;
			break;

		case 'X':
			flags |= PAX_SAMEDEVICE;
			break;

		default:
			return 1;
		}
	}

	if (mode == PAX_LIST) {
		return pax_list(stdin);
	}

	return 1;
}

void pax_list_file(struct stat *st, const char *name)
{
	if (S_ISDIR(st->st_mode)) {
		putchar('d');
	} else if (S_ISFIFO(st->st_mode)) {
		putchar('f');
	} else if (S_ISLNK(st->st_mode)) {
		putchar('l');
	} else if (S_ISBLK(st->st_mode)) {
		putchar('b');
	} else if (S_ISCHR(st->st_mode)) {
		putchar('c');
	//} else if (S_ISSOCK(st->st_mode)) {
		//putchar('S');
	//} else if (S_ISCTG(st->st_mode)) {
		//putchar('?');
	} else /* if (S_ISREG(st->st_mode)) */ {
		putchar('-');
	}

	putchar(st->st_mode & S_IRUSR ? 'r' : '-');
	putchar(st->st_mode & S_IWUSR ? 'w' : '-');
	if (st->st_mode & S_ISUID) {
		putchar(st->st_mode & S_IXUSR ? 's' : 'S');
	} else {
		putchar(st->st_mode & S_IXUSR ? 'x' : '-');
	}

	putchar(st->st_mode & S_IRGRP ? 'r' : '-');
	putchar(st->st_mode & S_IWGRP ? 'w' : '-');
	if (st->st_mode & S_ISGID) {
		putchar(st->st_mode & S_IXGRP ? 's' : 'S');
	} else {
		putchar(st->st_mode & S_IXGRP ? 'x' : '-');
	}

	putchar(st->st_mode & S_IROTH ? 'r' : '-');
	putchar(st->st_mode & S_IWOTH ? 'w' : '-');
	if (st->st_mode & S_ISVTX) {
		putchar(st->st_mode & S_IXOTH ? 's' : 'S');
	} else {
		putchar(st->st_mode & S_IXOTH ? 'x' : '-');
	}

	printf(" %ju", (uintmax_t)st->st_nlink);
	printf(" %ju", (uintmax_t)st->st_uid);
	printf("/%ju", (uintmax_t)st->st_gid);

	if (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)) {
		printf(" %jx", (uintmax_t)st->st_rdev);
	} else {
		printf(" %ju", (uintmax_t)st->st_size);
	}

	printf(" mtime");	/* TODO */

	printf(" %s\n", name);
}

uintmax_t pax_atoi(size_t n, const char _s[static n], int base)
{
	char s[64] = { 0 };
	strncpy(s, _s, n);
	return strtoumax(s, NULL, base);
}

int main(int argc, char *argv[])
{
	char *base = basename(argv[0]);
	if (!strcmp(base, "tar")) {
		return tar_main(argc, argv);
	}

	if (!strcmp(base, "cpio")) {
		return cpio_main(argc, argv);
	}

	return pax_main(argc, argv);
}
