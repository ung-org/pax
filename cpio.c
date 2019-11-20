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
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pax.h"

struct cpio_header {
	char magic[6];
	char dev[6];
	char ino[6];
	char mode[6];
	char uid[6];
	char gid[6];
	char nlink[6];
	char rdev[6];
	char mtime[11];
	char namesize[6];
	char filesize[11];
};

struct cpio_entry {
	struct stat st;
	uintmax_t namesize;
};

const size_t cpio_header_size = sizeof(struct cpio_header);

static mode_t cpio_convert_mode(unsigned long mode)
{
	mode_t m = 0;
	m |= (mode & C_IRUSR) ? S_IRUSR : 0;
	m |= (mode & C_IWUSR) ? S_IWUSR : 0;
	m |= (mode & C_IXUSR) ? S_IXUSR : 0;

	m |= (mode & C_IRGRP) ? S_IRGRP : 0;
	m |= (mode & C_IWGRP) ? S_IWGRP : 0;
	m |= (mode & C_IXGRP) ? S_IXGRP : 0;

	m |= (mode & C_IROTH) ? S_IROTH : 0;
	m |= (mode & C_IWOTH) ? S_IWOTH : 0;
	m |= (mode & C_IXOTH) ? S_IXOTH : 0;

	m |= (mode & C_ISUID) ? S_ISUID : 0;
	m |= (mode & C_ISGID) ? S_ISGID : 0;
	m |= (mode & C_ISVTX) ? S_ISVTX : 0;

	m |= (mode & C_ISDIR) ? S_IFDIR : 0;
	m |= (mode & C_ISFIFO) ? S_IFIFO : 0;
	m |= (mode & C_ISREG) ? S_IFREG : 0;
	m |= (mode & C_ISBLK) ? S_IFBLK : 0;
	m |= (mode & C_ISCHR) ? S_IFCHR : 0;
	//m |= (mode & C_ISCTG) ? S_IFCTG : 0;
	m |= (mode & C_ISLNK) ? S_IFLNK : 0;
	m |= (mode & C_ISSOCK) ? S_IFSOCK : 0;

	return m;
}

static struct cpio_entry cpio_deserialize(struct cpio_header *dh)
{
	struct cpio_entry e = { 0 };
	e.st.st_dev = pax_atoi(sizeof(dh->dev), dh->dev, 8);
	e.st.st_ino = pax_atoi(sizeof(dh->ino), dh->ino, 8);
	e.st.st_mode = cpio_convert_mode(pax_atoi(sizeof(dh->mode), dh->mode, 8));
	e.st.st_uid = pax_atoi(sizeof(dh->uid), dh->uid, 8);
	e.st.st_gid = pax_atoi(sizeof(dh->gid), dh->gid, 8);
	e.st.st_nlink = pax_atoi(sizeof(dh->nlink), dh->nlink, 8);
	e.st.st_rdev = pax_atoi(sizeof(dh->rdev), dh->rdev, 8);
	e.st.st_mtim.tv_sec = pax_atoi(sizeof(dh->mtime), dh->mtime, 8);
	e.st.st_size = pax_atoi(sizeof(dh->filesize), dh->filesize, 8);

	e.namesize = pax_atoi(sizeof(dh->namesize), dh->namesize, 8);

	return e;
}

int cpio_list(FILE *input, size_t firstlen, void *firstblock)
{
	struct cpio_header *dh = firstblock;

	//for (;;) {
		struct cpio_entry e = cpio_deserialize(dh);
		pax_list_file(&e.st, (char*)firstblock + sizeof(*dh));
	//}

	return 0;
}

int cpio_main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	int c;

	while ((c = getopt(argc, argv, "oip")) != -1) {
		switch (c) {
		case 'o':
			// create
			break;

		case 'i':
			// extract
			break;

		case 'p':
			// copy?
			break;

		default:
			return 1;
		}

		break;
	}

	for (size_t i = 2; argv[optind][i] != '\0'; i++) {
		printf("checking %c\n", argv[optind][i]);
		switch (argv[optind][i]) {
		case 'a':
			// reset atimes
			break;

		case 'B':
			// blocksize 5120
			break;

		case 'd':
			// create directories
			break;

		case 'c':
			// read and write in character form for portability
			break;

		case 'r':
			// interactively rename files
			break;

		case 't':
			// list table of contents
			break;

		case 'u':
			// copy unconditionally
			break;

		case 'v':
			// verbose
			break;

		case 'l':
			// link instead of copy, requires -p
			break;

		case 'm':
			// reset mtime
			break;

		case 'f':
			// ignore files in pattern
			break;

		default:
			fprintf(stderr, "cpio: unknown option '%c'\n",
				argv[optind][i]);
			return 1;
		}
	}

	return 0;
}
