/* ch_malloc.c - malloc routines that test returns from malloc and friends */
/* $OpenLDAP: pkg/ldap/servers/slapd/ch_malloc.c,v 1.21.2.3 2005/01/20 17:01:06 kurt Exp $ */
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
/* Portions Copyright (c) 1995 Regents of the University of Michigan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of Michigan at Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#define CH_FREE 1

#include "portable.h"

#include <stdio.h>

#include <ac/stdlib.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"

BerMemoryFunctions ch_mfuncs = {
	(BER_MEMALLOC_FN *)ch_malloc,
	(BER_MEMCALLOC_FN *)ch_calloc,
	(BER_MEMREALLOC_FN *)ch_realloc,
	(BER_MEMFREE_FN *)ch_free 
};

void *
ch_malloc(
    ber_len_t	size
)
{
	void	*new;

	if ( (new = (void *) ber_memalloc_x( size, NULL )) == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG( OPERATION, ERR, 
			   "ch_malloc: allocation of %lu bytes failed\n", (long)size, 0,0 );
#else
		Debug( LDAP_DEBUG_ANY, "ch_malloc of %lu bytes failed\n",
			(long) size, 0, 0 );
#endif
		assert( 0 );
		exit( EXIT_FAILURE );
	}

	return( new );
}

void *
ch_realloc(
    void		*block,
    ber_len_t	size
)
{
	void	*new, *ctx;

	if ( block == NULL ) {
		return( ch_malloc( size ) );
	}

	if( size == 0 ) {
		ch_free( block );
	}

	ctx = sl_context( block );
	if ( ctx ) {
		return sl_realloc( block, size, ctx );
	}

	if ( (new = (void *) ber_memrealloc_x( block, size, NULL )) == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG( OPERATION, ERR, 
			   "ch_realloc: reallocation of %lu bytes failed\n", (long)size, 0,0 );
#else
		Debug( LDAP_DEBUG_ANY, "ch_realloc of %lu bytes failed\n",
			(long) size, 0, 0 );
#endif
		assert( 0 );
		exit( EXIT_FAILURE );
	}

	return( new );
}

void *
ch_calloc(
    ber_len_t	nelem,
    ber_len_t	size
)
{
	void	*new;

	if ( (new = (void *) ber_memcalloc_x( nelem, size, NULL )) == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG( OPERATION, ERR, 
			   "ch_calloc: allocation of %lu elements of %lu bytes faild\n",
			   (long)nelem, (long)size, 0 );
#else
		Debug( LDAP_DEBUG_ANY, "ch_calloc of %lu elems of %lu bytes failed\n",
		  (long) nelem, (long) size, 0 );
#endif
		assert( 0 );
		exit( EXIT_FAILURE );
	}

	return( new );
}

char *
ch_strdup(
    const char *string
)
{
	char	*new;

	if ( (new = ber_strdup_x( string, NULL )) == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG( OPERATION, ERR, 
			"chr_strdup: duplication of \"%s\" failed\n", string, 0, 0 );
#else
		Debug( LDAP_DEBUG_ANY, "ch_strdup(%s) failed\n", string, 0, 0 );
#endif
		assert( 0 );
		exit( EXIT_FAILURE );
	}

	return( new );
}

void
ch_free( void *ptr )
{
	void *ctx;

	ctx = sl_context( ptr );
	if (ctx) {
		sl_free( ptr, ctx );
	} else {
		ber_memfree_x( ptr, NULL );
	}
}

