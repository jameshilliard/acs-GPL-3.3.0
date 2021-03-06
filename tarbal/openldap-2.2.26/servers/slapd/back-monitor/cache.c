/* cache.c - routines to maintain an in-core cache of entries */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-monitor/cache.c,v 1.12.2.5 2005/01/20 17:01:14 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2001-2005 The OpenLDAP Foundation.
 * Portions Copyright 2001-2003 Pierangelo Masarati.
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
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by Pierangelo Masarati for inclusion
 * in OpenLDAP Software.
 */

#include "portable.h"

#include <stdio.h>

#include "slap.h"

#include "back-monitor.h"

/*
 * compares entries based on the dn
 */
int
monitor_cache_cmp(
		const void *c1,
		const void *c2
)
{
	struct monitorcache 	*cc1 = ( struct monitorcache * )c1;
	struct monitorcache 	*cc2 = ( struct monitorcache * )c2;

	/*
	 * case sensitive, because the dn MUST be normalized
	 */
	return ber_bvcmp( &cc1->mc_ndn, &cc2->mc_ndn );
}

/*
 * checks for duplicate entries
 */
int
monitor_cache_dup(
		void *c1,
		void *c2
)
{
	struct monitorcache *cc1 = ( struct monitorcache * )c1;
	struct monitorcache *cc2 = ( struct monitorcache * )c2;

	/*
	 * case sensitive, because the dn MUST be normalized
	 */
	return ber_bvcmp( &cc1->mc_ndn, &cc2->mc_ndn ) == 0 ? -1 : 0;
}

/*
 * adds an entry to the cache and inits the mutex
 */
int
monitor_cache_add(
		struct monitorinfo	*mi,
		Entry			*e
)
{
	struct monitorcache	*mc;
	struct monitorentrypriv *mp;
	int			rc;

	assert( mi != NULL );
	assert( e != NULL );

	mp = ( struct monitorentrypriv *)e->e_private;
	ldap_pvt_thread_mutex_init( &mp->mp_mutex );

	mc = ( struct monitorcache * )ch_malloc( sizeof( struct monitorcache ) );
	mc->mc_ndn = e->e_nname;
	mc->mc_e = e;
	ldap_pvt_thread_mutex_lock( &mi->mi_cache_mutex );
	rc = avl_insert( &mi->mi_cache, ( caddr_t )mc,
			monitor_cache_cmp, monitor_cache_dup );
	ldap_pvt_thread_mutex_unlock( &mi->mi_cache_mutex );

	return rc;
}

/*
 * locks the entry (no r/w)
 */
int
monitor_cache_lock(
		Entry			*e
)
{
		struct monitorentrypriv *mp;

		assert( e != NULL );
		assert( e->e_private != NULL );

		mp = ( struct monitorentrypriv * )e->e_private;
		ldap_pvt_thread_mutex_lock( &mp->mp_mutex );

		return( 0 );
}

/*
 * gets an entry from the cache based on the normalized dn 
 * with mutex locked
 */
int
monitor_cache_get(
		struct monitorinfo      *mi,
		struct berval		*ndn,
		Entry			**ep
)
{
	struct monitorcache tmp_mc, *mc;

	assert( mi != NULL );
	assert( ndn != NULL );
	assert( ep != NULL );

	tmp_mc.mc_ndn = *ndn;
	ldap_pvt_thread_mutex_lock( &mi->mi_cache_mutex );
	mc = ( struct monitorcache * )avl_find( mi->mi_cache,
			( caddr_t )&tmp_mc, monitor_cache_cmp );

	if ( mc != NULL ) {
		/* entry is returned with mutex locked */
		monitor_cache_lock( mc->mc_e );
		ldap_pvt_thread_mutex_unlock( &mi->mi_cache_mutex );
		*ep = mc->mc_e;

		return( 0 );
	}
	
	ldap_pvt_thread_mutex_unlock( &mi->mi_cache_mutex );
	*ep = NULL;

	return( -1 );
}

/*
 * If the entry exists in cache, it is returned in locked status;
 * otherwise, if the parent exists, if it may generate volatile 
 * descendants an attempt to generate the required entry is
 * performed and, if successful, the entry is returned
 */
int
monitor_cache_dn2entry(
		Operation		*op,
		struct berval		*ndn,
		Entry			**ep,
		Entry			**matched
)
{
	struct monitorinfo *mi = (struct monitorinfo *)op->o_bd->be_private;
	int 			rc;
	struct berval		p_ndn = { 0L, NULL };
	Entry 			*e_parent;
	struct monitorentrypriv *mp;
		
	assert( mi != NULL );
	assert( ndn != NULL );
	assert( ep != NULL );
	assert( matched != NULL );

	*matched = NULL;

	rc = monitor_cache_get( mi, ndn, ep );
       	if ( !rc && *ep != NULL ) {
		return( 0 );
	}

	/* try with parent/ancestors */
	if ( ndn->bv_len ) {
		dnParent( ndn, &p_ndn );
	}

	if ( p_ndn.bv_val == NULL ) {
		p_ndn.bv_val = "";
		p_ndn.bv_len = 0;
		
	} else {
		p_ndn.bv_len = ndn->bv_len 
			- ( ber_len_t ) ( p_ndn.bv_val - ndn->bv_val );
	}

	rc = monitor_cache_dn2entry( op, &p_ndn, &e_parent, matched );
	if ( rc || e_parent == NULL) {
		return( -1 );
	}

	mp = ( struct monitorentrypriv * )e_parent->e_private;
	rc = -1;
	if ( mp->mp_flags & MONITOR_F_VOLATILE_CH ) {
		/* parent entry generates volatile children */
		rc = monitor_entry_create( op, ndn, e_parent, ep );
	}

	if ( !rc ) {
		monitor_cache_release( mi, e_parent );
	} else {
		*matched = e_parent;
	}
	
	return( rc );
}

/*
 * releases the lock of the entry; if it is marked as volatile, it is
 * destroyed.
 */
int
monitor_cache_release(
	struct monitorinfo	*mi,
	Entry			*e
)
{
	struct monitorentrypriv *mp;

	assert( mi != NULL );
	assert( e != NULL );
	assert( e->e_private != NULL );
	
	mp = ( struct monitorentrypriv * )e->e_private;

	if ( mp->mp_flags & MONITOR_F_VOLATILE ) {
		struct monitorcache	*mc, tmp_mc;

		/* volatile entries do not return to cache */
		ldap_pvt_thread_mutex_lock( &mi->mi_cache_mutex );
		tmp_mc.mc_ndn = e->e_nname;
		mc = avl_delete( &mi->mi_cache,
				( caddr_t )&tmp_mc, monitor_cache_cmp );
		ldap_pvt_thread_mutex_unlock( &mi->mi_cache_mutex );
		ch_free( mc );
		
		ldap_pvt_thread_mutex_unlock( &mp->mp_mutex );
		ldap_pvt_thread_mutex_destroy( &mp->mp_mutex );
		ch_free( mp );
		e->e_private = NULL;
		entry_free( e );

		return( 0 );
	}
	
	ldap_pvt_thread_mutex_unlock( &mp->mp_mutex );

	return( 0 );
}

