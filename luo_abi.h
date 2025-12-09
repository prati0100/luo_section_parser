// SPDX-License-Identifier: GPL-2.0

#ifndef LUO_ABI_H_
#define LUO_ABI_H_

#include <stdint.h>

/*
 * NOTE: This should be taken from the linux ABI headers but since they aren't
 * shipped yet just copy the defines here.
 */

#define LIVEUPDATE_HNDL_COMPAT_LENGTH	48

#define LIVEUPDATE_VER_HDR_MAGIC 0x4c565550 /* 'LVUP' */
#define LIVEUPDATE_VER_HDR_VER   1

/**
 * struct liveupdate_ver_hdr - Header of vmlinux section with version lists
 * @magic:     Magic number.
 * @version:   Version of the header format.
 *
 * This struct is the header for the vmlinux section ".liveupdate_versions". The
 * section contains the list of file handler versions that the kernel can
 * support.
 */
struct liveupdate_ver_hdr {
	uint32_t magic;
	uint32_t version;
};

/**
 * struct liveupdate_ver_table - Table of file handler versions that the kernel
 * can support.
 *
 * @hdr:        Table header.
 * @versions:   List of versions the kernel supports. The strings ate
 *              NUL-terminated, but to keep the format simpler always take up
 *              LIVEUPDATE_HNDL_COMPAT_LENGTH bytes.
 *
 * The list of verisons immediately follows the header. The number of versions
 * are determined by section length.
 */
struct liveupdate_ver_table {
	struct liveupdate_ver_hdr hdr;
	char versions[][LIVEUPDATE_HNDL_COMPAT_LENGTH];
};

#endif // LUO_ABI_H_
