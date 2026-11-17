/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1994-2009 Red Hat, Inc. All rights reserved.
 */

/*
FUNCTION
	<<ffs>>---find first bit set in a word

INDEX
	ffs

SYNOPSIS
	#include <strings.h>
	int ffs(int <[word]>);

DESCRIPTION

<<ffs>> returns the first bit set in a word.

RETURNS
<<ffs>> returns 0 if <[c]> is 0, 1 if <[c]> is odd, 2 if <[c]> is a multiple of
2, etc.

PORTABILITY
<<ffs>> is not ANSI C.

No supporting OS subroutines are required.  */

#include <strings.h>

int
ffs(int i)
{

#ifndef __rtems__
	return (__builtin_ffs(i));
#else /* __rtems__ */
	if (i == 0)
		return (0);

	return (__builtin_ctz((unsigned int)i) + 1);
#endif /* __rtems__ */
}
