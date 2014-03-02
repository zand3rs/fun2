/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-10-26
 *     /         Updated : 2009-10-26
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : ccb_handler.hpp
 * Description : CCB handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _CCB_HANDLER_HPP_
#define _CCB_HANDLER_HPP_

/*-defines--------------------------------------------------------------------*/

#define CCB_ACTION_ACTIVATE    "A"
#define CCB_ACTION_DEACTIVATE  "D"

/*-functions------------------------------------------------------------------*/

void ccb_insert (const char* msisdn, const char* action);
void* ccb_fetcher (void* arg);
void* ccb_handler (void* arg);

#endif

/******************************************************************************/
