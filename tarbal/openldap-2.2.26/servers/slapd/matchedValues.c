/* $OpenLDAP: pkg/ldap/servers/slapd/matchedValues.c,v 1.17.2.6 2005/01/20 17:01:08 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
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

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"

static int
test_mra_vrFilter(
	Operation	*op,
	Attribute	*a,
	MatchingRuleAssertion *mra,
	char 		***e_flags
);

static int
test_substrings_vrFilter(
	Operation	*op,
	Attribute	*a,
	ValuesReturnFilter *f,
	char		***e_flags
);

static int
test_presence_vrFilter(
	Operation	*op,
	Attribute	*a,
	AttributeDescription *desc,
	char 		***e_flags
);

static int
test_ava_vrFilter(
	Operation	*op,
	Attribute	*a,
	AttributeAssertion *ava,
	int		type,
	char 		***e_flags
);


int
filter_matched_values( 
	Operation	*op,
	Attribute	*a,
	char		***e_flags
)
{
	ValuesReturnFilter *vrf;
	int		rc = LDAP_SUCCESS;

#ifdef NEW_LOGGING
	LDAP_LOG( FILTER, ENTRY, "filter_matched_values: begin\n", 0, 0, 0 );
#else
	Debug( LDAP_DEBUG_FILTER, "=> filter_matched_values\n", 0, 0, 0 );
#endif

	for ( vrf = op->o_vrFilter; vrf != NULL; vrf = vrf->vrf_next ) {
		switch ( vrf->vrf_choice ) {
		case SLAPD_FILTER_COMPUTED:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, DETAIL1, 
				"test_vrFilter: COMPUTED %s (%d)\n",
				vrf->vrf_result == LDAP_COMPARE_FALSE ? "false" :
				vrf->vrf_result == LDAP_COMPARE_TRUE	 ? "true"  :
				vrf->vrf_result == SLAPD_COMPARE_UNDEFINED ? "undefined" :
				"error", vrf->vrf_result, 0 );
#else
			Debug( LDAP_DEBUG_FILTER, "	COMPUTED %s (%d)\n",
				vrf->vrf_result == LDAP_COMPARE_FALSE ? "false" :
				vrf->vrf_result == LDAP_COMPARE_TRUE ? "true" :
				vrf->vrf_result == SLAPD_COMPARE_UNDEFINED ? "undefined" : "error",
				vrf->vrf_result, 0 );
#endif
			/*This type of filter does not affect the result */
			rc = LDAP_SUCCESS;
		break;

		case LDAP_FILTER_EQUALITY:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, DETAIL1, "test_vrFilter: EQUALITY\n", 0, 0, 0 );
#else
			Debug( LDAP_DEBUG_FILTER, "	EQUALITY\n", 0, 0, 0 );
#endif
			rc = test_ava_vrFilter( op, a, vrf->vrf_ava,
				LDAP_FILTER_EQUALITY, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		case LDAP_FILTER_SUBSTRINGS:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, DETAIL1, "test_vrFilter  SUBSTRINGS\n", 0, 0, 0 );
#else
			Debug( LDAP_DEBUG_FILTER, "	SUBSTRINGS\n", 0, 0, 0 );
#endif

			rc = test_substrings_vrFilter( op, a,
				vrf, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		case LDAP_FILTER_PRESENT:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, DETAIL1, "test_vrFilter:	PRESENT\n", 0, 0, 0 );
#else
			Debug( LDAP_DEBUG_FILTER, "	PRESENT\n", 0, 0, 0 );
#endif
			rc = test_presence_vrFilter( op, a,
				vrf->vrf_desc, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		case LDAP_FILTER_GE:
			rc = test_ava_vrFilter( op, a, vrf->vrf_ava,
				LDAP_FILTER_GE, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		case LDAP_FILTER_LE:
			rc = test_ava_vrFilter( op, a, vrf->vrf_ava,
				LDAP_FILTER_LE, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		case LDAP_FILTER_EXT:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, DETAIL1, "test_vrFilter:	EXT\n", 0, 0, 0 );
#else
			Debug( LDAP_DEBUG_FILTER, "	EXT\n", 0, 0, 0 );
#endif
			rc = test_mra_vrFilter( op, a,
				vrf->vrf_mra, e_flags );
			if( rc == -1 ) {
				return rc;
			}
			break;

		default:
#ifdef NEW_LOGGING
			LDAP_LOG( FILTER, INFO, 
				"test_vrFilter:  unknown filter type %lu\n", vrf->vrf_choice, 0, 0 );
#else
			Debug( LDAP_DEBUG_ANY, "	unknown filter type %lu\n",
				vrf->vrf_choice, 0, 0 );
#endif
			rc = LDAP_PROTOCOL_ERROR;
		} 
	}

#ifdef NEW_LOGGING
	LDAP_LOG( FILTER, ENTRY, "filter_matched_values:  return=%d\n", rc, 0, 0 );
#else
	Debug( LDAP_DEBUG_FILTER, "<= filter_matched_values %d\n", rc, 0, 0 );
#endif
	return( rc );
}

static int
test_ava_vrFilter(
	Operation	*op,
	Attribute	*a,
	AttributeAssertion *ava,
	int		type,
	char 		***e_flags
)
{
	int 		i, j;

	for ( i=0; a != NULL; a = a->a_next, i++ ) {

		MatchingRule *mr;
		struct berval *bv;
	
		if ( !is_ad_subtype( a->a_desc, ava->aa_desc ) ) {
			continue;
		}

		switch ( type ) {
		case LDAP_FILTER_APPROX:
			mr = a->a_desc->ad_type->sat_approx;
			if( mr != NULL ) break;
			/* use EQUALITY matching rule if no APPROX rule */

		case LDAP_FILTER_EQUALITY:
			mr = a->a_desc->ad_type->sat_equality;
			break;
		
		case LDAP_FILTER_GE:
		case LDAP_FILTER_LE:
			mr = a->a_desc->ad_type->sat_ordering;
			break;

		default:
			mr = NULL;
		}

		if( mr == NULL ) continue;

		bv = a->a_nvals;
		for ( j=0; bv->bv_val != NULL; bv++, j++ ) {
			int ret;
			int rc;
			const char *text;

			rc = value_match( &ret, a->a_desc, mr, 0,
				bv, &ava->aa_value, &text );
			if( rc != LDAP_SUCCESS ) {
				return rc;
			}

			switch ( type ) {
			case LDAP_FILTER_EQUALITY:
			case LDAP_FILTER_APPROX:
				if ( ret == 0 ) {
					(*e_flags)[i][j] = 1;
				}
				break;
	
			case LDAP_FILTER_GE:
				if ( ret >= 0 ) {
					(*e_flags)[i][j] = 1;
				}
				break;
	
			case LDAP_FILTER_LE:
				if ( ret <= 0 ) {
					(*e_flags)[i][j] = 1;
				}
				break;
			}
		}
	}
	return( LDAP_SUCCESS );
}

static int
test_presence_vrFilter(
	Operation	*op,
	Attribute	*a,
	AttributeDescription *desc,
	char 		***e_flags
)
{
	int i, j;

	for ( i=0; a != NULL; a = a->a_next, i++ ) {
		struct berval *bv;

		if ( !is_ad_subtype( a->a_desc, desc ) ) {
			continue;
		}

		for ( bv = a->a_vals, j=0; bv->bv_val != NULL; bv++, j++ );
		memset( (*e_flags)[i], 1, j);
	}

	return( LDAP_SUCCESS );
}

static int
test_substrings_vrFilter(
	Operation	*op,
	Attribute	*a,
	ValuesReturnFilter *vrf,
	char		***e_flags
)
{
	int i, j;

	for ( i=0; a != NULL; a = a->a_next, i++ ) {
		MatchingRule *mr = a->a_desc->ad_type->sat_substr;
		struct berval *bv;

		if ( !is_ad_subtype( a->a_desc, vrf->vrf_sub_desc ) ) {
			continue;
		}

		if( mr == NULL ) {
			continue;
		}

		bv = a->a_nvals;
		for ( j = 0; bv->bv_val != NULL; bv++, j++ ) {
			int ret;
			int rc;
			const char *text;

			rc = value_match( &ret, a->a_desc, mr, 0,
				bv, vrf->vrf_sub, &text );

			if( rc != LDAP_SUCCESS ) {
				return rc;
			}

			if ( ret == 0 ) {
				(*e_flags)[i][j] = 1;
			}
		}
	}

	return LDAP_SUCCESS;
}

static int
test_mra_vrFilter(
	Operation	*op,
	Attribute	*a,
	MatchingRuleAssertion *mra,
	char 		***e_flags
)
{
	int i, j;

	for ( i=0; a != NULL; a = a->a_next, i++ ) {
		struct berval *bv, assertedValue;

		if ( mra->ma_desc ) {
			if ( !is_ad_subtype( a->a_desc, mra->ma_desc ) ) {
				continue;
			}
			assertedValue = mra->ma_value;

		} else {
			int rc;
			const char	*text = NULL;

			/* check if matching is appropriate */
			if ( !mr_usable_with_at( mra->ma_rule, a->a_desc->ad_type ) ) {
				continue;
			}

			rc = asserted_value_validate_normalize( a->a_desc, mra->ma_rule,
				SLAP_MR_EXT|SLAP_MR_VALUE_OF_ASSERTION_SYNTAX,
				&mra->ma_value, &assertedValue, &text, op->o_tmpmemctx );

			if( rc != LDAP_SUCCESS ) continue;
		}

		/* check match */
		if (mra->ma_rule == a->a_desc->ad_type->sat_equality) {
			bv = a->a_nvals;
		} else {
			bv = a->a_vals;
		}
					
		for ( j = 0; bv->bv_val != NULL; bv++, j++ ) {
			int ret;
			int rc;
			const char *text;

			rc = value_match( &ret, a->a_desc, mra->ma_rule, 0,
				bv, &assertedValue, &text );
			if( rc != LDAP_SUCCESS ) {
				return rc;
			}

			if ( ret == 0 ) {
				(*e_flags)[i][j] = 1;
			}
		}
	}

	return LDAP_SUCCESS;
}

