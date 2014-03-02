/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-04-23
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : mydb.h
 * Description : MySQL wrapper functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __MYDB_H__
#define __MYDB_H__

#include "mysql.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/

typedef struct MYDB_FIELD_ {
    char *pname;
    char *pvalue;
} MYDB_FIELD;

/*----------------------------------------------------------------------------*/

typedef struct MYDB_ROW_ {
    MYSQL_RES *pres;
    MYSQL_ROW row;
} MYDB_ROW;

/*----------------------------------------------------------------------------*/

typedef struct MYDB_RES_ {
    MYSQL_RES *pres;
} MYDB_RES;

/*----------------------------------------------------------------------------*/

typedef struct MYDB_CONPARAMS_ {
    MYSQL mydb;
    char *pshost;
    unsigned int uiport;
    char *psdbname;
    char *psuser;
    char *pspass;
} MYDB_CONPARAMS;

/*-functions------------------------------------------------------------------*/

int mydb_connect (MYDB_CONPARAMS *pconn);
int mydb_disconnect (MYDB_CONPARAMS *pconn);
int mydb_ping (MYDB_CONPARAMS *pconn);
int mydb_query (MYDB_RES *presult, MYDB_CONPARAMS *pconn, char *pssql);
int mydb_free_result (MYDB_RES *presult);
int mydb_fetch_row (MYDB_ROW *prow, MYDB_RES *presult);
int mydb_fetch_field (MYDB_FIELD *pfield, MYDB_ROW *prow, unsigned int ifield);
int mydb_fetch_field_by_colname (MYDB_FIELD *pfield, MYDB_ROW *prow, char *pscolname);
unsigned int mydb_num_fields (MYDB_RES *presult);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/
