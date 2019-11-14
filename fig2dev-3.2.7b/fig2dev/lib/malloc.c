/*
 * malloc.c: Provide a replacement for malloc().
 * Copyright (c) 2015 by Thomas Loimer
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved. This file is offered as-is,
 * without any warranty.
 *
 */

/*
 * Provide a replacement function for malloc, if ./configure finds
 * that the original malloc() is not portable. Copied from the
 * info pages of autoconf, section "Particular Function Checks".
 */

#undef malloc

#include <stddef.h>

void *malloc();

/* Allocate an N-byte block of memory from the heap.
   If N is zero, allocate a 1-byte block.  */

void *
rpl_malloc(size_t n)
{
	if (n == 0)
		n = 1;
	return malloc(n);
}
