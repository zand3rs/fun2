/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httputils.hpp
 * Description : HTTP helper functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _HTTPUTILS_HPP_
#define _HTTPUTILS_HPP_

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

void url_encode(const char* src, char* dest, const int dest_size);
void url_decode(const char* src, char* dest, const int dest_size);

#endif

/******************************************************************************/
