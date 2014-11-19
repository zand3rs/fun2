/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-14
 *     /         Updated : 2009-09-14
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : misc.hpp
 * Description : Miscellaneous functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _MISC_HPP_
#define _MISC_HPP_

#include <stdarg.h>

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

void string_replace (char* buf, int buf_size, char* search, char* replace);
float getBalance(const char *msisdn);

void send_system_msg (const char* customer_type, int tran_type, int ref_id,
        const char* a_no, const char* b_no,
        const char* msg_id, int msg_part, ...);
void send_system_msg (const char* customer_type, int tran_type, int ref_id,
        const char* a_no, const char* b_no,
        const char* msg_id, const char* msg, ...);

int csp_charge (const char* msisdn);

int nf_provision (const char* msisdn, const char* service_id, int duration);
int nf_deprovision (const char* msisdn, const char* service_id);

#endif

/******************************************************************************/
