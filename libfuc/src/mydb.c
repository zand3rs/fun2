/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-04-23
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : mydb.c
 * Description : MySQL wrapper functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mydb.h"

/*============================================================================*/

int mydb_connect (MYDB_CONPARAMS *pconn)
{
    MYSQL *mysql;
    
    mysql = mysql_init (&pconn->mydb);
    mysql = mysql_real_connect(&pconn->mydb,
                                pconn->pshost,
                                pconn->psuser,
                                pconn->pspass, 
                                pconn->psdbname, 
                                pconn->uiport,
                                NULL, 0);

    if (! mysql)
    {
#ifdef __MYDB_DEBUG__    
        fprintf (stderr,"Unable to establish MySQL connection!\n%s\n",
                 mysql_error (&pconn->mydb));
#endif
        return -1;
    }
    
    return 0;
}

/*----------------------------------------------------------------------------*/

int mydb_disconnect (MYDB_CONPARAMS *pconn)
{
    mysql_close (&pconn->mydb);
    return 0;
}

/*----------------------------------------------------------------------------*/

int mydb_ping (MYDB_CONPARAMS *pconn)
{
    int istatus;

    istatus = mysql_ping (&pconn->mydb);

    if (istatus)
    {
#ifdef __MYDB_DEBUG__    
        fprintf (stderr,"Unable to establish MySQL connection!\n%s\n",
                 mysql_error (&pconn->mydb));
#endif
    }
    
    return istatus;
}

/*----------------------------------------------------------------------------*/

int mydb_query (MYDB_RES *presult, MYDB_CONPARAMS *pconn, char *pssql)
{
    int istatus = 0;
    unsigned int ierr;

    presult->pres = NULL;
    istatus = mysql_query (&pconn->mydb, pssql);
    if (istatus)
    {
#ifdef __MYDB_DEBUG__    
        fprintf (stderr,"Unable to execute query!\n%s\n",
                 mysql_error (&pconn->mydb));
#endif
        istatus = -1;
    } 
    else
    {
        presult->pres = mysql_store_result (&pconn->mydb);
        ierr = mysql_errno (&pconn->mydb);
        if (ierr)
        {
#ifdef __MYDB_DEBUG__    
            fprintf (stderr,"Error: %s\n", mysql_error (&pconn->mydb));
#endif
            presult->pres = NULL;
            istatus = -1;
        }
        else
        {
            istatus = (int) mysql_affected_rows (&pconn->mydb);
        }
    }
    
    return istatus;
}

/*----------------------------------------------------------------------------*/

int mydb_free_result (MYDB_RES *presult)
{
    if (NULL == presult->pres)
    {
        return -1;
    }
    mysql_free_result (presult->pres);
    presult->pres = NULL;
    return 0;
}

/*----------------------------------------------------------------------------*/

int mydb_fetch_row (MYDB_ROW *prow, MYDB_RES *presult)
{
    if (NULL == presult->pres)
    {
        return -1;
    }
    prow->pres = presult->pres;
    prow->row = mysql_fetch_row (presult->pres);
    if (NULL == prow->row)
    {
        return -1;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

int mydb_fetch_field (MYDB_FIELD *pfield, MYDB_ROW *prow, unsigned int ifield)
{
    MYSQL_RES *pres;
    MYSQL_ROW row;
    MYSQL_FIELD *pfld;
    unsigned int i;

    pfield->pname = NULL;
    pfield->pvalue = NULL;

    pres = prow->pres;
    row = prow->row;
    if (NULL == pres || NULL == row)
    {
        return -1;
    }
    i = mysql_num_fields (pres);
    if (ifield >= i)
    {
        return -1;
    }
    pfld = mysql_fetch_field_direct (pres, ifield);
    pfield->pname = pfld->name;
    pfield->pvalue = row[ifield];
    
    return 0;
}

/*----------------------------------------------------------------------------*/

int mydb_fetch_field_by_colname (MYDB_FIELD *pfield, MYDB_ROW *prow, char *pscolname)
{
    MYSQL_RES *pres;
    MYSQL_ROW row;
    MYSQL_FIELD *pfld;
    unsigned int ifields, i;

    pfield->pname = NULL;
    pfield->pvalue = NULL;

    pres = prow->pres;
    row = prow->row;
    if (NULL == pres || NULL == row)
    {
        return -1;
    }
    ifields = mysql_num_fields (pres);
    for (i = 0; i < ifields; i++)
    {
        pfld = mysql_fetch_field_direct (pres, i);
        if (! strcasecmp (pfld->name, pscolname))
        {
            pfield->pname = pfld->name;
            pfield->pvalue = row[i];
            return 0;
        }
    }

    return -1;
}

/*----------------------------------------------------------------------------*/

unsigned int mydb_num_fields (MYDB_RES *presult)
{
    if (NULL == presult->pres)
    {
        return 0;
    }
    return mysql_num_fields (presult->pres);
}

/******************************************************************************/

#if 0
int main (void)
{
    MYDB_CONPARAMS conn;
    MYDB_RES result;
    MYDB_ROW row;
    MYDB_FIELD field;
    int istatus = 0;
    
    memset (&conn, 0, sizeof (conn));
    conn.pshost = "localhost";
    conn.uiport = 0;
    conn.psdbname = "chikka";
    conn.psuser = "chikka";
    conn.pspass = "chikka";
    istatus = mydb_connect (&conn);
    printf ("istatus = [%d]\n", istatus);

    istatus = mydb_query (&result, &conn,
                          "select msg_id, gsm_num, access_code from sms_in");
    printf ("istatus = [%d]\n", istatus);
    if (istatus > 0)
    {
        while (! mydb_fetch_row (&row, &result))
        {
            printf ("got a row...\n");
            istatus = mydb_fetch_field (&field, &row, 0);
            printf ("    istatus = [%d], row[0] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");
            istatus = mydb_fetch_field (&field, &row, 1);
            printf ("    istatus = [%d], row[1] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");
            istatus = mydb_fetch_field (&field, &row, 2);
            printf ("    istatus = [%d], row[2] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");

            printf ("\n");
            
            istatus = mydb_fetch_field_by_colname (&field, &row, "MSG_ID");
            printf ("    istatus = [%d], row[MSG_ID] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");
            istatus = mydb_fetch_field_by_colname (&field, &row, "GSM_NUM");
            printf ("    istatus = [%d], row[GSM_NUM] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");
            istatus = mydb_fetch_field_by_colname (&field, &row, "ACCESS_CODE");
            printf ("    istatus = [%d], row[ACCESS_CODE] = [%s]\n", istatus, (NULL != field.pvalue) ? field.pvalue : "NULL");
        }
        mydb_free_result (&result);
    }
    istatus = mydb_disconnect (&conn);
    return 0;
}
#endif
