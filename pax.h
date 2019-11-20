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

#include <stdint.h>
#include <sys/stat.h>

#define PAX_BLOCK_SIZE 512

struct tar_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
};

enum {
	PAX_APPEND = 1<<0,
	PAX_DIRECTORY = 1<<1,
	PAX_INTERACTIVE = 1<<2,
	PAX_KEEP = 1<<3,
	PAX_LINK = 1<<4,
	PAX_TIMESTAMP = 1<<5,
	PAX_UPDATE = 1<<6,
	PAX_VERBOSE = 1<<7,
	PAX_SAMEDEVICE = 1<<8,
};

enum pax_format {
	PAX_FORMAT_UNKNOWN,
	PAX_FORMAT_PAX,
	PAX_FORMAT_CPIO,
	PAX_FORMAT_TAR,
};

int cpio_list(FILE *input, size_t nblocks, void *firstblock);
int tar_list(FILE *input, size_t nblocks, void *firstblock);

void pax_list_file(struct stat *st, const char *filename);
uintmax_t pax_atoi(size_t n, const char s[static n], int base);

int tar_main(int argc, char *argv[]);
int cpio_main(int argc, char *argv[]);

extern const size_t cpio_header_size;
