/* command interface to Pluto
 * Copyright (C) 1997 Angelos D. Keromytis.
 * Copyright (C) 1998-2001  D. Hugh Redelmeier.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: whacklib.c,v 1.2 2004/01/20 18:53:25 mcr Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <assert.h>

#include <freeswan.h>
#include <stdarg.h>

#include "constants.h"
#include "defs.h"
#include "whack.h"
#include "diag.h"

static bool
pack_str(struct whackpacker *wp, char **p)
{
    const char *s = *p == NULL? "" : *p;	/* note: NULL becomes ""! */
    size_t len = strlen(s) + 1;

    if (wp->str_roof - wp->str_next < (ptrdiff_t)len)
    {
	return FALSE;	/* fishy: no end found */
    }
    else
    {
	strcpy(wp->str_next, s);
	wp->str_next += len;
	*p = NULL;	/* don't send pointers on the wire! */
	return TRUE;
    }
}

static bool
unpack_str(struct whackpacker *wp, char **p)
{
    char *end = memchr(wp->str_next, '\0', wp->str_roof - wp->str_next);

    if (end == NULL)
    {
	return FALSE;	/* fishy: no end found */
    }
    else
    {
	*p = wp->str_next == end? NULL : wp->str_next;
	wp->str_next = end + 1;
	return TRUE;
    }
}

err_t pack_whack_msg (struct whackpacker *wp)
{
    err_t ugh = NULL;
    /**
     * Pack strings
     */

    /* pack strings for inclusion in message */
    wp->str_next = wp->msg->string;
    wp->str_roof = &wp->msg->string[sizeof(wp->msg->string)];

    if (!pack_str(wp, &wp->msg->name)	/* string 1 */
	|| !pack_str(wp, &wp->msg->left.id)	/* string 2 */
	|| !pack_str(wp, &wp->msg->left.updown)	/* string 3 */
	|| !pack_str(wp, &wp->msg->right.id)	/* string 4 */
	|| !pack_str(wp, &wp->msg->right.updown)	/* string 5 */
	|| !pack_str(wp, &wp->msg->keyid)	/* string 6 */
	|| !pack_str(wp, &wp->msg->myid)	/* string 7 */
	|| wp->str_roof - wp->str_next < (ptrdiff_t)wp->msg->keyval.len)    /* chunk (sort of string 5) */
    {
	ugh = "too many bytes of strings to fit in message to pluto";
	return ugh;
    }

    if(wp->msg->keyval.ptr)
    {
	memcpy(wp->str_next, wp->msg->keyval.ptr, wp->msg->keyval.len);
    }
    wp->msg->keyval.ptr = NULL;
    wp->str_next += wp->msg->keyval.len;

    return ugh;
}

err_t unpack_whack_msg (struct whackpacker *wp)
{
    err_t ugh = NULL;

    if (wp->str_next > wp->str_roof)
    {
	ugh = builddiag("ignoring truncated message from whack: got %d bytes; expected %u"
			, (int) wp->n, (unsigned) sizeof(wp->msg));
    }
    if(!unpack_str(wp, &wp->msg->name)	        /* string 1 */
       || !unpack_str(wp, &wp->msg->left.id)	        /* string 2 */
       || !unpack_str(wp, &wp->msg->left.updown)	/* string 3 */
       || !unpack_str(wp, &wp->msg->right.id)	/* string 4 */
       || !unpack_str(wp, &wp->msg->right.updown)	/* string 5 */
       || !unpack_str(wp, &wp->msg->keyid)	        /* string 6 */
       || !unpack_str(wp, &wp->msg->myid)	        /* string 7 */
       || wp->str_roof - wp->str_next != (ptrdiff_t)wp->msg->keyval.len)	/* check chunk */
    {
	ugh = "message from whack contains bad string";
    }

    return ugh;
}

/* Build up a diagnostic in a static buffer.
 * Although this would be a generally useful function, it is very
 * hard to come up with a discipline that prevents different uses
 * from interfering.  It is intended that by limiting it to building
 * diagnostics, we will avoid this problem.
 * Juggling is performed to allow an argument to be a previous
 * result: the new string may safely depend on the old one.  This
 * restriction is not checked in any way: violators will produce
 * confusing results (without crashing!).
 */
char diag_space[sizeof(diag_space)];

err_t
builddiag(const char *fmt, ...)
{
    static char diag_space[LOG_WIDTH];	/* longer messages will be truncated */
    char t[sizeof(diag_space)];	/* build result here first */
    va_list args;

    va_start(args, fmt);
    t[0] = '\0';	/* in case nothing terminates string */
    vsnprintf(t, sizeof(t), fmt, args);
    va_end(args);
    strcpy(diag_space, t);
    return diag_space;
}

