/*
 * $Id: modutil_getlogin.c,v 1.5 2005/06/09 17:29:20 kukuk Exp $
 *
 * A central point for invoking getlogin(). Hopefully, this is a
 * little harder to spoof than all the other versions that are out
 * there.
 */

#include "pammodutil.h"

#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>

#define _PAMMODUTIL_GETLOGIN "_pammodutil_getlogin"

const char *_pammodutil_getlogin(pam_handle_t *pamh)
{
    int status;
    const void *logname;
    const void *void_curr_tty;
    const char *curr_tty;
    char *curr_user;
    struct utmp *ut, line;

    status = pam_get_data(pamh, _PAMMODUTIL_GETLOGIN, &logname);
    if (status == PAM_SUCCESS) {
	return logname;
    }

    status = pam_get_item(pamh, PAM_TTY, &void_curr_tty);
    if ((status != PAM_SUCCESS) || (void_curr_tty == NULL))
      curr_tty = ttyname(0);
    else
      curr_tty = (const char*)void_curr_tty;

    if ((curr_tty == NULL) || memcmp(curr_tty, "/dev/", 5)) {
	return NULL;
    }

    curr_tty += 5;  /* strlen("/dev/") */
    logname = NULL;

    setutent();
    strncpy(line.ut_line, curr_tty, sizeof(line.ut_line));

    if ((ut = getutline(&line)) == NULL) {
	goto clean_up_and_go_home;
    }

    curr_user = calloc(sizeof(line.ut_user)+1, 1);
    if (curr_user == NULL) {
	goto clean_up_and_go_home;
    }

    strncpy(curr_user, ut->ut_user, sizeof(ut->ut_user));
    /* calloc already zeroed the memory */

    status = pam_set_data(pamh, _PAMMODUTIL_GETLOGIN, curr_user,
			  _pammodutil_cleanup);
    if (status != PAM_SUCCESS) {
	free(curr_user);
	goto clean_up_and_go_home;
    }

    logname = curr_user;

clean_up_and_go_home:

    endutent();

    return logname;
}
