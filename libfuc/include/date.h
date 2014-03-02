/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2003-05-02
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : date.h
 * Description : Functions for manipulating date.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __DATE_H__
#define __DATE_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * date format:
 *
 * yyyy = year
 * yy   = year
 * mm   = month
 * dd   = day
 * HH   = hour
 * MM   = minute
 * SS   = second
 * 
 */ 

/*-functions------------------------------------------------------------------*/

/*
 * date_is_valid - Returns 1 if ps_date is a valid date based on ps_format,
 *                 returns 0 otherwise.
 */ 
int date_is_valid (char *ps_date, char *ps_format);

/*
 * date_get_by_epoch - Set ps_date with date based on ps_format using time pt_lt.
 */ 
int date_get_by_epoch (char *ps_date, char *ps_format, time_t *pt_lt);

/*
 * date_get - Set ps_date with current date based on ps_format.
 *          - Set ps_date to local timestamp if ps_format is empty.
 */ 
int date_get (char *ps_date, char *ps_format);

/*
 * date_normalize - Set ps_date_d with normalized ps_date_s based on ps_format.
 */ 
int date_normalize (char *ps_date_d, char *ps_date_s, char *ps_format);

/*
 * date_get_epoch - Returns the time in seconds based on ps_date using ps_format.
 */ 
time_t date_get_epoch (char *ps_date, char *ps_format);

/*
 * date_compare - Returns the difference time in seconds of ps_date1 and ps_date2.
 */ 
time_t date_compare (char *ps_date1, char *ps_date2, char *ps_format);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

