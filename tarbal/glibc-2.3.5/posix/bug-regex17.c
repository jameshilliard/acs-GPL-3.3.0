/* Turkish regular expression tests.
   Copyright (C) 2002, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sys/types.h>
#include <mcheck.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

/* Tests supposed to match.  */
struct
{
  const char *pattern;
  const char *string;
  int flags, nmatch;
  regmatch_t rm[5];
} tests[] = {
  /* \xc3\x84	LATIN CAPITAL LETTER A WITH DIAERESIS
     \xc3\x96	LATIN CAPITAL LETTER O WITH DIAERESIS
     \xc3\xa4	LATIN SMALL LETTER A WITH DIAERESIS
     \xc3\xb6	LATIN SMALL LETTER O WITH DIAERESIS  */
  { "\xc3\x84\xc3\x96*\xc3\xb6$", "aB\xc3\xa4\xc3\xb6\xc3\xb6\xc3\x96", REG_ICASE, 2,
    { { 2, 10 }, { -1, -1 } } },
  { "[\xc3\x84x]\xc3\x96*\xc3\xb6$", "aB\xc3\x84\xc3\xb6\xc3\xb6\xc3\x96", REG_ICASE, 2,
    { { 2, 10 }, { -1, -1 } } },
  { "[\xc3\x84x]\xc3\x96*\xc3\xb6$", "aB\xc3\xa4\xc3\xb6\xc3\xb6\xc3\x96", REG_ICASE, 2,
    { { 2, 10 }, { -1, -1 } } },
  { "[^x]\xc3\x96*\xc3\xb6$", "aB\xc3\xa4\xc3\xb6\xc3\xb6\xc3\x96", REG_ICASE, 2,
    { { 2, 10 }, { -1, -1 } } },
};

int
main (void)
{
  regex_t re;
  regmatch_t rm[5];
  size_t i;
  int n, ret = 0;

  setlocale (LC_ALL, "de_DE.UTF-8");
  for (i = 0; i < sizeof (tests) / sizeof (tests[0]); ++i)
    {
      n = regcomp (&re, tests[i].pattern, tests[i].flags);
      if (n != 0)
	{
	  char buf[500];
	  regerror (n, &re, buf, sizeof (buf));
	  printf ("regcomp %zd failed: %s\n", i, buf);
	  ret = 1;
	  continue;
	}

      if (regexec (&re, tests[i].string, tests[i].nmatch, rm, 0))
	{
	  printf ("regexec %zd failed\n", i);
	  ret = 1;
	  regfree (&re);
	  continue;
	}

      for (n = 0; n < tests[i].nmatch; ++n)
	if (rm[n].rm_so != tests[i].rm[n].rm_so
              || rm[n].rm_eo != tests[i].rm[n].rm_eo)
	  {
	    if (tests[i].rm[n].rm_so == -1 && tests[i].rm[n].rm_eo == -1)
	      break;
	    printf ("regexec match failure rm[%d] %d..%d\n",
		    n, rm[n].rm_so, rm[n].rm_eo);
	    ret = 1;
	    break;
	  }

      regfree (&re);
    }

  return ret;
}
