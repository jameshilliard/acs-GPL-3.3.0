/* user.c - set user id, group id and group access list */
/* $OpenLDAP: pkg/ldap/servers/slapd/user.c,v 1.19.2.3 2005/01/20 17:01:10 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2005 The OpenLDAP Foundation.
 * Portions Copyright 1999 PM Lashley.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "portable.h"

#if defined(HAVE_SETUID) && defined(HAVE_SETGID)

#include <stdio.h>

#include <ac/stdlib.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif

#include <ac/ctype.h>
#include <ac/unistd.h>

#include "slap.h"

/*
 * Set real and effective user id and group id, and group access list
 * The user and group arguments are freed.
 */

void
slap_init_user( char *user, char *group )
{
    uid_t	uid = 0;
    gid_t	gid = 0;
    int		got_uid = 0, got_gid = 0;

    if ( user ) {
	struct passwd *pwd;
	if ( isdigit( (unsigned char) *user )) {
	    got_uid = 1;
	    uid = atoi( user );
#ifdef HAVE_GETPWUID
	    pwd = getpwuid( uid );
	    goto did_getpw;
#else
	    free( user );
	    user = NULL;
#endif
	} else {
	    pwd = getpwnam( user );
	did_getpw:
	    if ( pwd == NULL ) {
#ifdef NEW_LOGGING
		    LDAP_LOG( OPERATION, INFO, 
				"slap_init_user: No passwd entry for user %s\n", user, 0, 0 );
#else
		Debug( LDAP_DEBUG_ANY, "No passwd entry for user %s\n",
		       user, 0, 0 );
#endif

		exit( EXIT_FAILURE );
	    }
	    if ( got_uid ) {
		free( user );
		user = (pwd != NULL ? ch_strdup( pwd->pw_name ) : NULL);
	    } else {
		got_uid = 1;
		uid = pwd->pw_uid;
	    }
	    got_gid = 1;
	    gid = pwd->pw_gid;
#ifdef HAVE_ENDPWENT
	    endpwent();
#endif
	}
    }

    if ( group ) {
	struct group *grp;
	if ( isdigit( (unsigned char) *group )) {
	    gid = atoi( group );
#ifdef HAVE_GETGRGID
	    grp = getgrgid( gid );
	    goto did_group;
#endif
	} else {
	    grp = getgrnam( group );
	    if ( grp != NULL )
		gid = grp->gr_gid;
	did_group:
	    if ( grp == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG( OPERATION, INFO, 
			"slap_init_user: No group entry for group %s\n", group, 0, 0 );
#else
		Debug( LDAP_DEBUG_ANY, "No group entry for group %s\n",
		       group, 0, 0 );
#endif

		exit( EXIT_FAILURE );
	    }
	}
	free( group );
	got_gid = 1;
    }

    if ( user ) {
	if ( getuid() == 0 && initgroups( user, gid ) != 0 ) {
#ifdef NEW_LOGGING
	    LDAP_LOG( OPERATION, INFO,
			"slap_init_user: Could not set the group access (gid) list.\n", 
			0, 0, 0 );
#else
	    Debug( LDAP_DEBUG_ANY,
		   "Could not set the group access (gid) list\n", 0, 0, 0 );
#endif

	    exit( EXIT_FAILURE );
	}
	free( user );
    }

#ifdef HAVE_ENDGRENT
    endgrent();
#endif

    if ( got_gid ) {
	if ( setgid( gid ) != 0 ) {
#ifdef NEW_LOGGING
	    LDAP_LOG( OPERATION, INFO, 
			"slap_init_user: could not set real group id to %d\n", 
			(int)gid, 0, 0);
#else
	    Debug( LDAP_DEBUG_ANY, "Could not set real group id to %d\n",
		       (int) gid, 0, 0 );
#endif

	    exit( EXIT_FAILURE );
	}
#ifdef HAVE_SETEGID
	if ( setegid( gid ) != 0 ) {
#ifdef NEW_LOGGING
	    LDAP_LOG( OPERATION, INFO, 
		   "slap_init_user: Could not set effective group id to %d\n",
		   (int)gid, 0, 0);
#else
	    Debug( LDAP_DEBUG_ANY, "Could not set effective group id to %d\n",
		       (int) gid, 0, 0 );
#endif

	    exit( EXIT_FAILURE );
	}
#endif
    }

    if ( got_uid ) {
	if ( setuid( uid ) != 0 ) {
#ifdef NEW_LOGGING
	    LDAP_LOG( OPERATION, INFO, 
			"slap_init_user: Could not set real user id to %d\n", 
			(int)uid, 0, 0 );
#else
	    Debug( LDAP_DEBUG_ANY, "Could not set real user id to %d\n",
		       (int) uid, 0, 0 );
#endif

	    exit( EXIT_FAILURE );
	}
#ifdef HAVE_SETEUID
	if ( seteuid( uid ) != 0 ) {
#ifdef NEW_LOGGING
	    LDAP_LOG( OPERATION, INFO, 
			"slap_init_user: Could not set effective user id to %d\n", 
			(int)uid, 0, 0 );
#else
	    Debug( LDAP_DEBUG_ANY, "Could not set effective user id to %d\n",
		       (int) uid, 0, 0 );
#endif

	    exit( EXIT_FAILURE );
	}
#endif
    }
}

#endif /* HAVE_PWD_H && HAVE_GRP_H */
