/* $OpenLDAP: pkg/ldap/servers/slapd/back-perl/close.c,v 1.9.2.6 2005/01/20 17:01:16 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * Portions Copyright 1999 John C. Quillan.
 * Portions Copyright 2002 myinternet Limited.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "perl_back.h"

/**********************************************************
 *
 * Close
 *
 **********************************************************/

int
perl_back_close(
	BackendInfo *bd
)
{
	ldap_pvt_thread_mutex_lock( &perl_interpreter_mutex );	

	perl_destruct(PERL_INTERPRETER);

	ldap_pvt_thread_mutex_unlock( &perl_interpreter_mutex );	

	return 0;
}

int
perl_back_destroy(
	BackendInfo *bd
)
{
	perl_free(PERL_INTERPRETER);
	PERL_INTERPRETER = NULL;

	ldap_pvt_thread_mutex_destroy( &perl_interpreter_mutex );	

	return 0;
}

int
perl_back_db_destroy(
	BackendDB *be
)
{
	free( be->be_private );
	be->be_private = NULL;

	return 0;
}
