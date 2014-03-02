/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : transaction_handler.hpp
 * Description : Transaction handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _TRANSACTION_HANDLER_HPP_
#define _TRANSACTION_HANDLER_HPP_

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

void* raw_fetcher (void* arg);
void* raw_handler (void* arg);

void* request_fetcher (void* arg);
void* request_handler (void* arg);

void* transaction_fetcher (void* arg);
void* transaction_handler (void* arg);

void* response_handler (void* arg);

void notification_insert (int tran_type, int ref_id, const char* msisdn);
void* notification_handler (void* arg);

#endif

/******************************************************************************/
