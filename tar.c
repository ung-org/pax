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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <tar.h>

#include "pax.h"

static mode_t tar_mode_to_st_mode(char typeflag, const char tm[static 8])
{
	uintmax_t mode = pax_atoi(8, tm, 8);
	mode_t m = 0;

	switch (typeflag) {
	case REGTYPE:
	case AREGTYPE:
	case LNKTYPE:
		m = S_IFREG;
		break;

	case SYMTYPE:
		m = S_IFLNK;
		break;

	case CHRTYPE:
		m = S_IFCHR;
		break;

	case BLKTYPE:
		m = S_IFBLK;
		break;

	case DIRTYPE:
		m = S_IFDIR;
		break;

	case FIFOTYPE:
		m = S_IFIFO;
		break;

	case CONTTYPE:
		//m = S_IFCONT;
		break;

	default:
		fprintf(stderr, "pax: unknown file type '%c' (%hhx)\n",
			typeflag, typeflag);
		break;
	}

	m |= (mode & TSUID) ? S_ISUID : 0;
	m |= (mode & TSGID) ? S_ISGID : 0;
	m |= (mode & TSVTX) ? S_ISVTX : 0;

	m |= (mode & TUREAD) ? S_IRUSR : 0;
	m |= (mode & TUWRITE) ? S_IWUSR : 0;
	m |= (mode & TUEXEC) ? S_IXUSR : 0;

	m |= (mode & TGREAD) ? S_IRGRP : 0;
	m |= (mode & TGWRITE) ? S_IWGRP : 0;
	m |= (mode & TGEXEC) ? S_IXGRP : 0;

	m |= (mode & TOREAD) ? S_IROTH : 0;
	m |= (mode & TOWRITE) ? S_IWOTH : 0;
	m |= (mode & TOEXEC) ? S_IXOTH : 0;

	return m;
}

static struct stat tar_header_to_stat(struct tar_header *th)
{
	struct stat st = { 0 };
	st.st_mode = tar_mode_to_st_mode(th->typeflag[0], th->mode);
	st.st_nlink = 1;
	st.st_uid = pax_atoi(sizeof(th->uid), th->uid, 8);
	st.st_gid = pax_atoi(sizeof(th->gid), th->gid, 8);
	st.st_size = pax_atoi(sizeof(th->size), th->size, 8);
	st.st_mtim.tv_sec = pax_atoi(sizeof(th->mtime), th->mtime, 8);
	st.st_blocks = st.st_size / 512 + 1;
	return st;
}

int tar_list(FILE *input, size_t firstlen, void *firstblock)
{
	char zero[512] = {0};

	(void)firstlen;
	for (;;) {
		struct tar_header *th = firstblock;
		struct stat st = tar_header_to_stat(th);
		pax_list_file(&st, th->name);
		for (blkcnt_t i = 0; i < st.st_blocks + 1; i++) {
			if (fread(firstblock, 1, 512, input) != 512) {
				fprintf(stderr, "pax: error reading input\n");
				return 1;
			} else if (memcmp(firstblock, zero, 512) == 0) {
				return 0;
			}
		}
	}
	return 0;
}
