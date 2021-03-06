/*
 * $Id: modutil_cleanup.c,v 1.1 2001/12/09 22:15:12 agmorgan Exp $
 *
 * This function provides a common pam_set_data() friendly version of free().
 */

#include "pammodutil.h"

void _pammodutil_cleanup(pam_handle_t *pamh, void *data, int error_status)
{
    if (data) {
	/* junk it */
	(void) free(data);
    }
}

