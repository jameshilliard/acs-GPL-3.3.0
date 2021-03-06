/* $OpenLDAP: pkg/ldap/servers/slapd/back-sql/sql-wrap.c,v 1.15.2.9 2005/01/20 17:01:17 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * Portions Copyright 1999 Dmitry Kovalev.
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
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by Dmitry Kovalev for inclusion
 * by OpenLDAP Software.
 */

#include "portable.h"

#ifdef SLAPD_SQL

#include <stdio.h>
#include "ac/string.h"
#include <sys/types.h>

#include "slap.h"
#include "ldap_pvt.h"
#include "proto-sql.h"

#define MAX_ATTR_LEN 16384

typedef struct backsql_db_conn {
	unsigned long	ldap_cid;
	SQLHDBC		dbh;
} backsql_db_conn;

void
backsql_PrintErrors( SQLHENV henv, SQLHDBC hdbc, SQLHSTMT sth, int rc )
{
	SQLCHAR	msg[SQL_MAX_MESSAGE_LENGTH];		/* msg. buffer    */
	SQLCHAR	state[SQL_SQLSTATE_SIZE];		/* statement buf. */
	SDWORD	iSqlCode;				/* return code    */
	SWORD	len = SQL_MAX_MESSAGE_LENGTH - 1;	/* return length  */ 

	Debug( LDAP_DEBUG_TRACE, "Return code: %d\n", rc, 0, 0 );

	for ( ; rc = SQLError( henv, hdbc, sth, state, &iSqlCode, msg,
			SQL_MAX_MESSAGE_LENGTH - 1, &len ), BACKSQL_SUCCESS( rc ); ) {
		Debug( LDAP_DEBUG_TRACE,
				"   Native error code: %d\n"
				"   SQL engine state:  %s\n"
				"   Message:           %s\n", 
				(int)iSqlCode, state, msg );
	}
}

RETCODE
backsql_Prepare( SQLHDBC dbh, SQLHSTMT *sth, char *query, int timeout )
{
	RETCODE		rc;
	char		drv_name[ 30 ];
	SWORD		len;

	rc = SQLAllocStmt( dbh, sth );
	if ( rc != SQL_SUCCESS ) {
		return rc;
	}

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "==>backsql_Prepare()\n", 0, 0, 0 );
#endif /* BACKSQL_TRACE */

	SQLGetInfo( dbh, SQL_DRIVER_NAME, drv_name, sizeof( drv_name ), &len );

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "backsql_Prepare(): driver name=\"%s\"\n",
			drv_name, 0, 0 );
#endif /* BACKSQL_TRACE */

	ldap_pvt_str2upper( drv_name );
	if ( !strncmp( drv_name, "SQLSRV32.DLL", sizeof( drv_name ) ) ) {
		/*
		 * stupid default result set in MS SQL Server
		 * does not support multiple active statements
		 * on the same connection -- so we are trying 
		 * to make it not to use default result set...
		 */
		Debug( LDAP_DEBUG_TRACE, "_SQLprepare(): "
			"enabling MS SQL Server default result "
			"set workaround\n", 0, 0, 0 );
		rc = SQLSetStmtOption( *sth, SQL_CONCURRENCY, 
				SQL_CONCUR_ROWVER );
		if ( rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO ) {
			Debug( LDAP_DEBUG_TRACE, "backsql_Prepare(): "
				"SQLSetStmtOption(SQL_CONCURRENCY,"
				"SQL_CONCUR_ROWVER) failed:\n", 
				0, 0, 0 );
			backsql_PrintErrors( SQL_NULL_HENV, dbh, *sth, rc );
		}
	}

	if ( timeout > 0 ) {
		Debug( LDAP_DEBUG_TRACE, "_SQLprepare(): "
			"setting query timeout to %d sec.\n", 
			timeout, 0, 0 );
		rc = SQLSetStmtOption( *sth, SQL_QUERY_TIMEOUT, timeout );
		if ( rc != SQL_SUCCESS ) {
			backsql_PrintErrors( SQL_NULL_HENV, dbh, *sth, rc );
		}
	}

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "<==backsql_Prepare() calling SQLPrepare()\n",
			0, 0, 0 );
#endif /* BACKSQL_TRACE */

	return SQLPrepare( *sth, query, SQL_NTS );
}

RETCODE
backsql_BindRowAsStrings( SQLHSTMT sth, BACKSQL_ROW_NTS *row )
{
	RETCODE		rc;
	SQLCHAR		colname[ 64 ];
	SQLSMALLINT	name_len, col_type, col_scale, col_null;
	UDWORD		col_prec;
	int		i;

	if ( row == NULL ) {
		return SQL_ERROR;
	}

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "==> backsql_BindRowAsStrings()\n", 0, 0, 0 );
#endif /* BACKSQL_TRACE */
	
	rc = SQLNumResultCols( sth, &row->ncols );
	if ( rc != SQL_SUCCESS ) {
#ifdef BACKSQL_TRACE
		Debug( LDAP_DEBUG_TRACE, "backsql_BindRowAsStrings(): "
			"SQLNumResultCols() failed:\n", 0, 0, 0 );
#endif /* BACKSQL_TRACE */
		
		backsql_PrintErrors( SQL_NULL_HENV, SQL_NULL_HDBC, sth, rc );

	} else {
#ifdef BACKSQL_TRACE
		Debug( LDAP_DEBUG_TRACE, "backsql_BindRowAsStrings: "
			"ncols=%d\n", (int)row->ncols, 0, 0 );
#endif /* BACKSQL_TRACE */

		row->col_names = (BerVarray)ch_calloc( row->ncols + 1, 
				sizeof( struct berval ) );
		row->cols = (char **)ch_calloc( row->ncols + 1, 
				sizeof( char * ) );
		row->col_prec = (UDWORD *)ch_calloc( row->ncols,
				sizeof( UDWORD ) );
		row->value_len = (SQLINTEGER *)ch_calloc( row->ncols,
				sizeof( SQLINTEGER ) );
		for ( i = 1; i <= row->ncols; i++ ) {
			rc = SQLDescribeCol( sth, (SQLSMALLINT)i, &colname[ 0 ],
					(SQLUINTEGER)( sizeof( colname ) - 1 ),
					&name_len, &col_type,
					&col_prec, &col_scale, &col_null );
			ber_str2bv( colname, 0, 1, &row->col_names[ i - 1 ] );
#ifdef BACKSQL_TRACE
			Debug( LDAP_DEBUG_TRACE, "backsql_BindRowAsStrings: "
				"col_name=%s, col_prec[%d]=%d\n",
				colname, (int)i, (int)col_prec );
#endif /* BACKSQL_TRACE */
			if ( col_type == SQL_LONGVARCHAR 
					|| col_type == SQL_LONGVARBINARY) {
#if 0
				row->cols[ i - 1 ] = NULL;
				row->col_prec[ i - 1 ] = -1;

				/*
				 * such fields must be handled 
				 * in some other way since they return 2G 
				 * as their precision (at least it does so 
				 * with MS SQL Server w/native driver)
				 * for now, we just set fixed precision 
				 * for such fields - dirty hack, but...
				 * no time to deal with SQLGetData()
				 */
#endif
				col_prec = MAX_ATTR_LEN;
				row->cols[ i - 1 ] = (char *)ch_calloc( col_prec + 1, sizeof( char ) );
				row->col_prec[ i - 1 ] = col_prec;
				rc = SQLBindCol( sth, (SQLUSMALLINT)i,
						SQL_C_CHAR,
						(SQLPOINTER)row->cols[ i - 1 ],
						col_prec + 1,
						&row->value_len[ i - 1 ] );
			} else {
				row->cols[ i - 1 ] = (char *)ch_calloc( col_prec + 1, sizeof( char ) );
				row->col_prec[ i - 1 ] = col_prec;
				rc = SQLBindCol( sth, (SQLUSMALLINT)i,
						SQL_C_CHAR,
						(SQLPOINTER)row->cols[ i - 1 ],
						col_prec + 1,
						&row->value_len[ i - 1 ] );
			}
		}

		row->col_names[ i - 1 ].bv_val = NULL;
		row->col_names[ i - 1 ].bv_len = 0;
		row->cols[ i - 1 ] = NULL;
	}

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "<== backsql_BindRowAsStrings()\n", 0, 0, 0 );
#endif /* BACKSQL_TRACE */

	return rc;
}

RETCODE
backsql_FreeRow( BACKSQL_ROW_NTS *row )
{
	if ( row->cols == NULL ) {
		return SQL_ERROR;
	}

	ber_bvarray_free( row->col_names );
	ldap_charray_free( row->cols );
	free( row->col_prec );
	free( row->value_len );

	return SQL_SUCCESS;
}

static int
backsql_cmp_connid( const void *v_c1, const void *v_c2 )
{
	const backsql_db_conn *c1 = v_c1, *c2 = v_c2;
	if ( c1->ldap_cid > c2->ldap_cid ) {
		return 1;
	}
	
	if ( c1->ldap_cid < c2->ldap_cid ) {
		return -1;
	}
	
	return 0;
}

static int
backsql_close_db_conn( backsql_db_conn *conn )
{
	Debug( LDAP_DEBUG_TRACE, "==>backsql_close_db_conn()\n", 0, 0, 0 );

	/*
	 * Default transact is SQL_ROLLBACK; commit is required only
	 * by write operations, and it is explicitly performed after
	 * each atomic operation succeeds.
	 */

	/* TimesTen */
	SQLTransact( SQL_NULL_HENV, conn->dbh, SQL_ROLLBACK );
	SQLDisconnect( conn->dbh );
	SQLFreeConnect( conn->dbh );
	Debug( LDAP_DEBUG_TRACE, "<==backsql_close_db_conn()\n", 0, 0, 0 );
	return 1;
}

int
backsql_init_db_env( backsql_info *si )
{
	RETCODE		rc;
	int		ret = SQL_SUCCESS;
	
	Debug( LDAP_DEBUG_TRACE, "==>backsql_init_db_env()\n", 0, 0, 0 );
	rc = SQLAllocEnv( &si->db_env );
	if ( rc != SQL_SUCCESS ) {
		Debug( LDAP_DEBUG_TRACE, "init_db_env: SQLAllocEnv failed:\n",
				0, 0, 0 );
		backsql_PrintErrors( SQL_NULL_HENV, SQL_NULL_HDBC,
				SQL_NULL_HENV, rc );
		ret = SQL_ERROR;
	}
	Debug( LDAP_DEBUG_TRACE, "<==backsql_init_db_env()=%d\n", ret, 0, 0 );
	return ret;
}

int
backsql_free_db_env( backsql_info *si )
{
	Debug( LDAP_DEBUG_TRACE, "==>backsql_free_db_env()\n", 0, 0, 0 );

#ifdef BACKSQL_TRACE
	Debug( LDAP_DEBUG_TRACE, "free_db_env(): delete AVL tree here!!!\n",
			0, 0, 0 );
#endif /* BACKSQL_TRACE */

	/*
	 * stop, if frontend waits for all threads to shutdown 
	 * before calling this -- then what are we going to delete?? 
	 * everything is already deleted...
	 */
	Debug( LDAP_DEBUG_TRACE, "<==backsql_free_db_env()\n", 0, 0, 0 );
	return SQL_SUCCESS;
}

static int
backsql_open_db_conn( backsql_info *si, unsigned long ldap_cid, backsql_db_conn **pdbc )
{
	/* TimesTen */
	char			DBMSName[ 32 ];
	backsql_db_conn		*dbc;
	int			rc;

	assert( pdbc );
	*pdbc = NULL;
 
	Debug( LDAP_DEBUG_TRACE, "==>backsql_open_db_conn()\n", 0, 0, 0 );
	dbc = (backsql_db_conn *)ch_calloc( 1, sizeof( backsql_db_conn ) );
	dbc->ldap_cid = ldap_cid;
	rc = SQLAllocConnect( si->db_env, &dbc->dbh );
	if ( !BACKSQL_SUCCESS( rc ) ) {
		Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn: "
			"SQLAllocConnect() failed:\n", 0, 0, 0 );
		backsql_PrintErrors( si->db_env, SQL_NULL_HDBC,
				SQL_NULL_HENV, rc );
		return LDAP_UNAVAILABLE;
	}

	rc = SQLConnect( dbc->dbh,
			(SQLCHAR*)si->dbname, SQL_NTS,
			(SQLCHAR*)si->dbuser, SQL_NTS,
			(SQLCHAR*)si->dbpasswd, SQL_NTS );
	if ( rc != SQL_SUCCESS ) {
		Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn: "
			"SQLConnect() to database \"%s\" as user \"%s\" "
			"%s:\n", si->dbname, si->dbuser,
			rc == SQL_SUCCESS_WITH_INFO ?
			"succeeded with info" : "failed" );
		backsql_PrintErrors( si->db_env, dbc->dbh, SQL_NULL_HENV, rc );
		if ( rc != SQL_SUCCESS_WITH_INFO ) {
			return LDAP_UNAVAILABLE;
		}
	}

	/* 
	 * TimesTen : Turn off autocommit.  We must explicitly
	 * commit any transactions. 
	 */
	SQLSetConnectOption( dbc->dbh, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF );

	/* 
	 * See if this connection is to TimesTen.  If it is,
	 * remember that fact for later use.
	 */
	/* Assume until proven otherwise */
	si->bsql_flags &= ~BSQLF_USE_REVERSE_DN;
	DBMSName[ 0 ] = '\0';
	rc = SQLGetInfo( dbc->dbh, SQL_DBMS_NAME, (PTR)&DBMSName,
			sizeof( DBMSName ), NULL );
	if ( rc == SQL_SUCCESS ) {
		if ( strcmp( DBMSName, "TimesTen" ) == 0 ||
				strcmp( DBMSName, "Front-Tier" ) == 0 ) {
			Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn: "
				"TimesTen database!\n", 0, 0, 0 );
			si->bsql_flags |= BSQLF_USE_REVERSE_DN;
		}
	} else {
		Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn: "
			"SQLGetInfo() failed:\n", 0, 0, 0 );
		backsql_PrintErrors( si->db_env, dbc->dbh, SQL_NULL_HENV, rc );
		return rc;
	}
	/* end TimesTen */

	Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn(): "
		"connected, adding to tree\n", 0, 0, 0 );
	ldap_pvt_thread_mutex_lock( &si->dbconn_mutex );
	if ( avl_insert( &si->db_conns, dbc, backsql_cmp_connid, avl_dup_error ) ) {
		Debug( LDAP_DEBUG_TRACE, "backsql_open_db_conn: "
			"duplicate connection ID\n", 0, 0, 0 );
		return LDAP_OTHER;
	}
	ldap_pvt_thread_mutex_unlock( &si->dbconn_mutex );
	Debug( LDAP_DEBUG_TRACE, "<==backsql_open_db_conn()\n", 0, 0, 0 );

	*pdbc = dbc;

	return LDAP_SUCCESS;
}

int
backsql_free_db_conn( Operation *op )
{
	backsql_info		*si = (backsql_info *)op->o_bd->be_private;
	backsql_db_conn		tmp = { 0 },
				*conn;

	Debug( LDAP_DEBUG_TRACE, "==>backsql_free_db_conn()\n", 0, 0, 0 );
	tmp.ldap_cid = op->o_connid;
	ldap_pvt_thread_mutex_lock( &si->dbconn_mutex );
	conn = avl_delete( &si->db_conns, &tmp, backsql_cmp_connid );
	ldap_pvt_thread_mutex_unlock( &si->dbconn_mutex );

	/*
	 * we have one thread per connection, as I understand -- so we can
	 * get this out of critical section
	 */
	if ( conn != NULL ) {
		Debug( LDAP_DEBUG_TRACE, "backsql_free_db_conn(): "
			"closing db connection\n", 0, 0, 0 );
		backsql_close_db_conn( conn );
	}
	Debug( LDAP_DEBUG_TRACE, "<==backsql_free_db_conn()\n", 0, 0, 0 );
	return SQL_SUCCESS;
}

int
backsql_get_db_conn( Operation *op, SQLHDBC *dbh )
{
	backsql_info		*si = (backsql_info *)op->o_bd->be_private;
	backsql_db_conn		*dbc,
				tmp = { 0 };
	int			rc = LDAP_SUCCESS;

	Debug( LDAP_DEBUG_TRACE, "==>backsql_get_db_conn()\n", 0, 0, 0 );

	assert( dbh );
	*dbh = SQL_NULL_HDBC;

	tmp.ldap_cid = op->o_connid;

	/*
	 * we have one thread per connection, as I understand -- 
	 * so we do not need locking here
	 */
	dbc = avl_find( si->db_conns, &tmp, backsql_cmp_connid );
	if ( !dbc ) {
		rc = backsql_open_db_conn( si, op->o_connid, &dbc );
		if ( rc != LDAP_SUCCESS) {
			Debug( LDAP_DEBUG_TRACE, "backsql_get_db_conn(): "
				"could not get connection handle "
				"-- returning NULL\n", 0, 0, 0 );
			return rc;
		}
	}

	ldap_pvt_thread_mutex_lock( &si->schema_mutex );
	if ( !BACKSQL_SCHEMA_LOADED( si ) ) {
		Debug( LDAP_DEBUG_TRACE, "backsql_get_db_conn(): "
			"first call -- reading schema map\n", 0, 0, 0 );
		rc = backsql_load_schema_map( si, dbc->dbh );
		if ( rc != LDAP_SUCCESS ) {
			ldap_pvt_thread_mutex_unlock( &si->schema_mutex );
			backsql_free_db_conn( op );
			return rc;
		}
	}
	ldap_pvt_thread_mutex_unlock( &si->schema_mutex );

	*dbh = dbc->dbh;

	Debug( LDAP_DEBUG_TRACE, "<==backsql_get_db_conn()\n", 0, 0, 0 );

	return LDAP_SUCCESS;
}

#endif /* SLAPD_SQL */

