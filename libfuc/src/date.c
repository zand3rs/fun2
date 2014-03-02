/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2003-05-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : date.c
 * Description : Functions for manipulating date.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "date.h"

/*============================================================================*/

static int parse_date_format_ (char *ps_date, char *ps_format, time_t *pt_lt)
{
    char s_buff[255];
    struct tm *ptm_ltstruct;
    struct tm tm;
    int i_status = 0;

    //ptm_ltstruct = localtime (pt_lt);
    localtime_r (pt_lt, &tm);
    ptm_ltstruct = &tm;

    if (! strcmp (ps_format, "yyyy")) {
        sprintf (s_buff, "%04d", ptm_ltstruct->tm_year+1900);
    }
    else if (! strcmp (ps_format, "yy")) {
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_year-100);
    }
    else if (! strcmp (ps_format, "mm")) {  
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_mon+1);
    }
    else if (! strcmp (ps_format, "dd")) {
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_mday);
    }
    else if (! strcmp (ps_format, "HH")) {
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_hour);
    }
    else if (! strcmp (ps_format, "MM")) {
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_min);
    }
    else if (! strcmp (ps_format, "SS")) {
        sprintf (s_buff, "%02d", ptm_ltstruct->tm_sec);
    }
    else {
        strcpy (s_buff, ps_format);
        i_status = -1;
    }
    
    strcpy (ps_date, s_buff); 
    return i_status;
}

/*----------------------------------------------------------------------------*/

static time_t mk_time_ (char *ps_date, char *ps_format)
{
    time_t t_lt;
    struct tm *ptm_mtstruct;
    struct tm tm;
    unsigned char c, c_behind;
    char s_format[255], s_date[255];
    int i_idx;
    
    t_lt = time (NULL);
    //ptm_mtstruct = localtime (&t_lt);
    localtime_r (&t_lt, &tm);
    ptm_mtstruct = &tm;

    /* initialize ... */
    ptm_mtstruct->tm_year = 0;
    ptm_mtstruct->tm_mon  = 0;
    ptm_mtstruct->tm_mday = 1;
    ptm_mtstruct->tm_hour = 0;
    ptm_mtstruct->tm_min  = 0;
    ptm_mtstruct->tm_sec  = 0;

    s_date[0] = 0; s_format[0] = 0;

    while ((c = *ps_format)) { 
        switch (c) {
            case 'y':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==4) {
                    ptm_mtstruct->tm_year = atoi (s_date) - 1900;
                }
                else if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_year = atoi (s_date) + 100;
                }
                break;
            case 'm':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_mon = atoi (s_date) - 1;
                }
                break;
            case 'd':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_mday = atoi (s_date);
                }
                break;
            case 'H':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_hour = atoi (s_date);
                }
                break;
            case 'M':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_min = atoi (s_date);
                }
                break;
            case 'S':
                c_behind = c; i_idx=0;
                while (c && c==c_behind) {
                    s_format[i_idx] = c;
                    s_date[i_idx] = *ps_date;
                    ps_format++;
                    ps_date++;
                    i_idx++;
                    c = *ps_format;
                }
                s_format[i_idx] = 0;
                s_date[i_idx] = 0;
                if (strlen (s_format)==2) {
                    ptm_mtstruct->tm_sec = atoi (s_date);
                }
                break;
        }
        ps_format++;
        ps_date++;
    }

    return mktime (ptm_mtstruct);
}

/*============================================================================*/

int date_is_valid (char *ps_date, char *ps_format)
{
    time_t t_mt;
    char s_ndate[255];
    int i_status = 0;

    if (strlen (ps_date) == strlen (ps_format)) {
        t_mt = mk_time_ (ps_date, ps_format);
        date_get_by_epoch (s_ndate, ps_format, &t_mt);
        i_status = (! (strcmp (ps_date, s_ndate))) ? 1 : 0;
    }
    
    return i_status;    
}

/*----------------------------------------------------------------------------*/

int date_get_by_epoch (char *ps_date, char *ps_format, time_t *pt_lt)
{
    int i_idx;
    unsigned char c, c_behind;
    char s_format[255];
    
    *ps_date = 0;
    while ((c = *ps_format)) { 
        if (c=='y' || c=='m' || c=='d' || c=='H' || c=='M' || c=='S') {
            c_behind = c; i_idx=0;
            while (c && c==c_behind) {
                s_format[i_idx] = c;
                ps_format++;
                i_idx++;
                c = *ps_format;
            }
            s_format[i_idx] = 0;
            parse_date_format_ (ps_date+strlen(ps_date), s_format, pt_lt);
        }
        else {
            sprintf (ps_date+strlen(ps_date), "%c", c);
            ps_format++;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int date_get (char *ps_date, char *ps_format)
{
    time_t t_lt = time (NULL);
    int len;

    if (ps_format && *ps_format) {
        date_get_by_epoch (ps_date, ps_format, &t_lt);
    }
    else {
        //strcpy (ps_date, ctime (&t_lt));
        ctime_r (&t_lt, ps_date);

        //trim trailing spaces...
        len = strlen (ps_date);
        while (len > 0) {
            --len;
            if (isspace (ps_date[len])) {
                ps_date[len] = 0;
            }
            else {
                break;
            }
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

int date_normalize (char *ps_date_d, char *ps_date_s, char *ps_format)
{
    time_t t_mt;
    char s_ndate[255];
    
    t_mt = mk_time_ (ps_date_s, ps_format);
    date_get_by_epoch (s_ndate, ps_format, &t_mt);
    strcpy (ps_date_d, s_ndate);
    return 0;
}

/*----------------------------------------------------------------------------*/

time_t date_get_epoch (char *ps_date, char *ps_format)
{
    time_t t_mt;
    t_mt = mk_time_ (ps_date, ps_format);
    return t_mt;
}

/*----------------------------------------------------------------------------*/

time_t date_compare (char *ps_date1, char *ps_date2, char *ps_format)
{
    time_t t_epoch1, t_epoch2, d_diffsec;

    t_epoch1 = date_get_epoch (ps_date1, ps_format);
    t_epoch2 = date_get_epoch (ps_date2, ps_format);
    d_diffsec = difftime (t_epoch2, t_epoch1);

    return d_diffsec;
}

/******************************************************************************/

#ifdef __MAIN__
int main ()
{
    char s_date[100];
    time_t t;

    t = date_compare ("2004-04-30", "2004-05-30", "yyyy-mm-dd");
    printf ("diff = [%ld]\n", t);
    t = date_compare ("2008-09-04", "2008-09-04", "yyyy-mm-dd");
    printf ("diff = [%ld]\n", t);
    date_get (s_date, "");
    printf ("s_date=[%s]\n", s_date);
    date_get (s_date, "yyyymmdd");
    printf ("s_date=[%s]\n", s_date);
    date_get (s_date, "yyyy-mm-dd HH:MM:SS");
    printf ("s_date=[%s]\n", s_date);
    date_get (s_date, "ss-kk-yyyy/mm/dd=lMn, d;y");
    printf ("s_date=[%s]\n", s_date);
    printf ("is_date=[%d]\n", date_is_valid("2008-09-04", "yyyy-mm-dd"));
    printf ("is_date=[%d]\n", date_is_valid("2008-09/04", "yyyy-mm-dd"));
    return 0;
}
#endif
