/* ldapcompare.c -- LDAP compare tool */
/* $OpenLDAP: pkg/ldap/clients/tools/ldapcompare.c,v 1.25.2.7 2005/01/20 17:00:55 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2005 The OpenLDAP Foundation.
 * Portions Copyright 1998-2003 Kurt D. Zeilenga.
 * Portions Copyright 1998-2001 Net Boolean Incorporated.
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
/* Portions Copyright (c) 1992-1996 Regents of the University of Michigan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of Michigan at Ann Arbor.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.  This
 * software is provided ``as is'' without express or implied warranty.
 */
/* Portions Copyright 2002, F5 Networks, Inc, All rights reserved.
 * This software is not subject to any license of F5 Networks.
 * This is free software; you can redistribute and use it
 * under the same terms as OpenLDAP itself.
 */
/* ACKNOWLEDGEMENTS:
 * This work was originally developed by Jeff Costlow (F5 Networks)
 * based, in part, on existing LDAP tools and adapted for inclusion
 * into OpenLDAP Software by Kurt D. Zeilenga.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/stdlib.h>

#include <ac/ctype.h>
#include <ac/string.h>
#include <ac/unistd.h>
#include <ac/errno.h>
#include <sys/stat.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif

#include <ldap.h>

#include "lutil.h"
#include "lutil_ldap.h"
#include "ldap_defaults.h"

#include "common.h"


static int quiet = 0;


void
usage( void )
{
	fprintf( stderr, _("usage: %s [options] DN <attr:value|attr::b64value>\n"), prog);
	fprintf( stderr, _("where:\n"));
	fprintf( stderr, _("  DN\tDistinguished Name\n"));
	fprintf( stderr, _("  attr\tassertion attribute\n"));
	fprintf( stderr, _("  value\tassertion value\n"));
	fprintf( stderr, _("  b64value\tbase64 encoding of assertion value\n"));

	fprintf( stderr, _("Compare options:\n"));
	fprintf( stderr, _("  -z         Quiet mode,"
		" don't print anything, use return values\n"));
	tool_common_usage();
	exit( EXIT_FAILURE );
}

static int docompare LDAP_P((
	LDAP *ld,
	char *dn,
	char *attr,
	struct berval *bvalue,
	int quiet,
	LDAPControl **sctrls,
	LDAPControl **cctrls));


const char options[] = "z"
	"Cd:D:e:h:H:IkKMnO:p:P:QR:U:vVw:WxX:y:Y:Z";

int
handle_private_option( int i )
{
	switch ( i ) {
#if 0
		char	*control, *cvalue;
		int		crit;
	case 'E': /* compare extensions */
		if( protocol == LDAP_VERSION2 ) {
			fprintf( stderr, _("%s: -E incompatible with LDAPv%d\n"),
				prog, protocol );
			exit( EXIT_FAILURE );
		}

		/* should be extended to support comma separated list of
		 *	[!]key[=value] parameters, e.g.  -E !foo,bar=567
		 */

		crit = 0;
		cvalue = NULL;
		if( optarg[0] == '!' ) {
			crit = 1;
			optarg++;
		}

		control = strdup( optarg );
		if ( (cvalue = strchr( control, '=' )) != NULL ) {
			*cvalue++ = '\0';
		}
		fprintf( stderr, _("Invalid compare extension name: %s\n"), control );
		usage();
#endif

	case 'z':
		quiet = 1;
		break;

	default:
		return 0;
	}
	return 1;
}


int
main( int argc, char **argv )
{
	char	*compdn = NULL, *attrs = NULL;
	char	*sep;
	int		rc;
	LDAP	*ld = NULL;
	struct berval bvalue = { 0, NULL };

	tool_init();
	prog = lutil_progname( "ldapcompare", argc, argv );

	tool_args( argc, argv );

	if ( argc - optind != 2 ) {
		usage();
	}

	compdn = argv[optind++];
	attrs = argv[optind++];

	/* user passed in only 2 args, the last one better be in
	 * the form attr:value or attr::b64value
	 */
	sep = strchr(attrs, ':');
	if (!sep) {
		usage();
	}

	*sep++='\0';
	if ( *sep != ':' ) {
		bvalue.bv_val = strdup( sep );
		bvalue.bv_len = strlen( bvalue.bv_val );

	} else {
		/* it's base64 encoded. */
		bvalue.bv_val = malloc( strlen( &sep[1] ));
		bvalue.bv_len = lutil_b64_pton( &sep[1],
			(unsigned char *) bvalue.bv_val, strlen( &sep[1] ));

		if (bvalue.bv_len == (ber_len_t)-1) {
			fprintf(stderr, _("base64 decode error\n"));
			exit(-1);
		}
	}

	ld = tool_conn_setup( 0, 0 );

	if ( pw_file || want_bindpw ) {
		if ( pw_file ) {
			rc = lutil_get_filed_password( pw_file, &passwd );
			if( rc ) return EXIT_FAILURE;
		} else {
			passwd.bv_val = getpassphrase( _("Enter LDAP Password: ") );
			passwd.bv_len = passwd.bv_val ? strlen( passwd.bv_val ) : 0;
		}
	}

	tool_bind( ld );

	if ( assertion || authzid || manageDSAit || noop ) {
		tool_server_controls( ld, NULL, 0 );
	}

	if ( verbose ) {
		fprintf( stderr, _("DN:%s, attr:%s, value:%s\n"),
			compdn, attrs, sep );
	}

	rc = docompare( ld, compdn, attrs, &bvalue, quiet, NULL, NULL );

	free( bvalue.bv_val );

	ldap_unbind_ext( ld, NULL, NULL );

	return rc;
}


static int docompare(
	LDAP *ld,
	char *dn,
	char *attr,
	struct berval *bvalue,
	int quiet,
	LDAPControl **sctrls,
	LDAPControl **cctrls )
{
	int			rc;

	if ( not ) {
		return LDAP_SUCCESS;
	}

	rc = ldap_compare_ext_s( ld, dn, attr, bvalue,
		sctrls, cctrls );

	/* if we were told to be quiet, use the return value. */
	if ( !quiet ) {
		if ( rc == LDAP_COMPARE_TRUE ) {
			printf(_("TRUE\n"));
		} else if ( rc == LDAP_COMPARE_FALSE ) {
			printf(_("FALSE\n"));
		} else {
			printf(_("UNDEFINED\n"));
			ldap_perror( ld, "ldap_compare" );
		}
	}

	return( rc );
}
