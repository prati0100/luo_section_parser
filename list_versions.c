// SPDX-License-Identifier: GPL-2.0

/*
 * Copyright (C) 2025 Pratyush Yadav <pratyush@kernel.org>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <gelf.h>
#include <libelf.h>

#include "luo_abi.h"

void *get_section_data(char *filename, char *section, size_t *size)
{
	if (elf_version(EV_CURRENT) == EV_NONE) {
		fprintf(stderr, "ELF library initialization failed.\n");
		return NULL;
	}

	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return NULL;
	}

	Elf *e = elf_begin(fd, ELF_C_READ, NULL);
	if (!e) {
		fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
		close(fd);
		return NULL;
	}

	size_t shstrndx;
	if (elf_getshdrstrndx(e, &shstrndx) != 0) {
		fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
		elf_end(e);
		close(fd);
		return NULL;
	}

	Elf_Scn *scn = NULL;
	GElf_Shdr shdr;

	while ((scn = elf_nextscn(e, scn)) != NULL) {
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
			continue;
		}

		const char *name = elf_strptr(e, shstrndx, shdr.sh_name);
		if (!name)
			continue;

		if (strcmp(name, section) == 0) {
			Elf_Data *data = elf_getdata(scn, NULL);
			if (!data) {
				fprintf(stderr, "elf_getdata failed: %s\n", elf_errmsg(-1));
				break;
			}

			void *buf = malloc(data->d_size);
			if (!buf) {
				perror("malloc");
				elf_end(e);
				close(fd);
				return NULL;
			}

			memcpy(buf, data->d_buf, data->d_size);
			*size = data->d_size;
			elf_end(e);
			close(fd);
			return buf;
		}
	}

	return NULL;
}

int main(int argc, char **argv)
{
	struct liveupdate_ver_table *table;
	unsigned long nr;
	size_t size;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
		return 1;
	}

	table = get_section_data(argv[1], ".liveupdate_versions", &size);
	if (!table || !size || size < sizeof(struct liveupdate_ver_hdr) ||
		(size - sizeof(struct liveupdate_ver_hdr)) % LIVEUPDATE_HNDL_COMPAT_LENGTH) {
		fprintf(stderr, "Invalid table size\n");
		return 1;
	}

	/* Validate the header. */
	if (table->hdr.magic != LIVEUPDATE_VER_HDR_MAGIC) {
		fprintf(stderr, "Unknown header magic: 0x%x\n", table->hdr.magic);
		return 1;
	}

	if (table->hdr.version != LIVEUPDATE_VER_HDR_VER) {
		fprintf(stderr, "Unknown header version %u\n", table->hdr.version);
		return 1;
	}

	nr = (size - sizeof(table->hdr)) / LIVEUPDATE_HNDL_COMPAT_LENGTH;

	/* The rest are the versions of the objects. */
	printf("Versions are:\n");
	for (unsigned int i = 0; i < nr; i++) {
		printf("\t%s\n", table->versions[i]);
	}

	return 0;
}
