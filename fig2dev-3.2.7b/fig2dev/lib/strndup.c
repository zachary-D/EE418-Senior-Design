/*
 * strndup.c: Provide a strndup() function.
 * Copyright (c) 2015 by Thomas Loimer
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved. This file is offered as-is,
 * without any warranty.
 *
 */

/*
 * strndup.c
 * Copied and modified from strdup.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
strndup(char const *str, size_t n)
{
	char	*p;

	if (!str)
		return (char *)NULL;

	p = (char *) malloc(n + 1);
	if (p != NULL) {
		strncpy(p, str, n);
		p[n] = '\0';
	}
	return p;
}
