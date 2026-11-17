/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1994-2009 Red Hat, Inc. All rights reserved.
 */

/*
FUNCTION
	<<strspn>>---find initial match

INDEX
	strspn

SYNOPSIS
	#include <string.h>
	size_t strspn(const char *<[s1]>, const char *<[s2]>);

DESCRIPTION
	This function computes the length of the initial segment of
	the string pointed to by <[s1]> which consists entirely of
	characters from the string pointed to by <[s2]> (excluding the
	terminating null character).

RETURNS
	<<strspn>> returns the length of the segment found.

PORTABILITY
<<strspn>> is ANSI C.

<<strspn>> requires no supporting OS subroutines.

QUICKREF
	strspn ansi pure
*/

#include <string.h>

size_t
strspn (const char *s1,
	const char *s2)
{
  const char *s = s1;
  const char *c;

  while (*s1)
    {
      for (c = s2; *c; c++)
	{
	  if (*s1 == *c)
	    goto found;
	}
#ifndef __rtems__
      if (*c == '\0')
	break;
#else /* __rtems__ */
      break;
#endif /* __rtems__ */
found:
      s1++;
    }

  return s1 - s;
}
