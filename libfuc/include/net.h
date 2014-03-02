/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-05-20
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : net.h
 * Description : Network miscellaneous functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __NET_H__
#define __NET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-functions------------------------------------------------------------------*/

unsigned long net_get_address (char *host);
void          net_get_ipaddr (char *ipaddr, struct in_addr in);
void          net_get_hostname (char *host, int host_size);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

