/* close.c - close ldbm backend */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldbm/close.c,v 1.16.2.4 2005/04/21 02:41:24 hyc Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2005 The OpenLDAP Foundation.
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

#include <stdio.h>

#include <ac/socket.h>

#include "slap.h"
#include "back-ldbm.h"

int
ldbm_back_db_close( Backend *be )
{
	struct ldbminfo *li = (struct ldbminfo *) be->be_private;
#ifdef NEW_LOGGING
	LDAP_LOG( BACK_LDBM, CRIT,
		   "ldbm_back_db_close: ldbm backend syncing\n", 0, 0, 0 );
#else
	Debug( LDAP_DEBUG_TRACE, "ldbm backend syncing\n", 0, 0, 0 );
#endif

	ldbm_cache_flush_all( be );
#ifdef NEW_LOGGING
	LDAP_LOG( BACK_LDBM, CRIT,
		   "ldbm_back_db_close: ldbm backend synch'ed\n", 0, 0, 0 );
#else
	Debug( LDAP_DEBUG_TRACE, "ldbm backend done syncing\n", 0, 0, 0 );
#endif

	cache_release_all( &((struct ldbminfo *) be->be_private)->li_cache );

	return 0;
}
