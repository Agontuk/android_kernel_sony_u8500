/*
 * Copyright (C) 2014 Sergey Senozhatsky.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * Based on zcomp_lzo.c and on the following patch from Zeev Tarantov:
 * http://driverdev.linuxdriverproject.org/pipermail/driverdev-devel/2011-April/015126.html
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include "../../staging/snappy/csnappy.h" /* if built in drivers/block */

#include "zcomp_snappy.h"
#define WMSIZE_ORDER	((PAGE_SHIFT > 14) ? (15) : (PAGE_SHIFT+1))

static void *zcomp_snappy_create(void)
{
	return kzalloc((1 << WMSIZE_ORDER), GFP_KERNEL);
}

static void zcomp_snappy_destroy(void *private)
{
	kfree(private);
}

static int snappy_compress_(const unsigned char *src, size_t src_len, unsigned char *dst,
		size_t *dst_len, void *workmem)
{
	const unsigned char *end = csnappy_compress_fragment(
		src, (uint32_t)src_len, dst, workmem, WMSIZE_ORDER);
	*dst_len = end - dst;
	return 0;
}

static int snappy_decompress_(const unsigned char *src, size_t src_len, unsigned char *dst,
	size_t *dst_len)
{
	uint32_t dst_len_ = (uint32_t)*dst_len;
	int ret = csnappy_decompress_noheader(src, src_len, dst, &dst_len_);
	*dst_len = (size_t)dst_len_;
	return ret;
}

static int zcomp_snappy_compress(const unsigned char *src, unsigned char *dst,
		size_t *dst_len, void *private)
{
	int ret = snappy_compress_(src, PAGE_SIZE, dst, dst_len, private);
	return ret == CSNAPPY_E_OK ? 0 : ret;
}

static int zcomp_snappy_decompress(const unsigned char *src, size_t src_len,
		unsigned char *dst)
{
	size_t dst_len = PAGE_SIZE;
	int ret = snappy_decompress_(src, src_len, dst, &dst_len);
	return ret == CSNAPPY_E_OK ? 0 : ret;
}

struct zcomp_backend zcomp_snappy = {
	.compress = zcomp_snappy_compress,
	.decompress = zcomp_snappy_decompress,
	.create = zcomp_snappy_create,
	.destroy = zcomp_snappy_destroy,
	.name = "snappy",
};
