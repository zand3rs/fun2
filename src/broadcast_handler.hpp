/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-10
 *     /         Updated : 2009-09-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : broadcast_handler.hpp
 * Description : Broadcast handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _BROADCAST_HANDLER_HPP_
#define _BROADCAST_HANDLER_HPP_

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

void* daily_bal_fetcher (void* arg);
void* daily_bal_handler (void* arg);

void* broadcast_fetcher (void* arg);
void* broadcast_handler (void* arg);

void* response_handler (void* arg);

#endif

/******************************************************************************/
