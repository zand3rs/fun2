/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2004-05-20
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : net.c
 * Description : Network miscellaneous functions.
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
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>

#include "net.h"

/*============================================================================*/

#ifndef INADDR_NONE
#    define INADDR_NONE -1
#endif

#define NET_MAX_IPADDR_LEN 15

static pthread_mutex_t net_lock = PTHREAD_MUTEX_INITIALIZER;

/*============================================================================*/

#ifndef gethostbyname_r
unsigned long net_get_address (char *host)
{
    struct hostent *hp;
    unsigned long laddr = htonl(INADDR_ANY);

    pthread_mutex_lock(&net_lock);

    if (host && *host) {
        laddr = inet_addr(host);
        if ((htonl(INADDR_NONE) == laddr) && (strcmp(host, "255.255.255.255"))) {
            hp = gethostbyname(host);
            if (hp) {
                laddr = *((unsigned long *) (hp->h_addr));
            }
        }
    }

    pthread_mutex_unlock(&net_lock);

    return laddr;
}
#else
unsigned long net_get_address (char *host)
{
    struct hostent hret;
    struct hostent *hp;
    char buf[1024];
    int herr;
    unsigned long laddr = htonl(INADDR_ANY);

    if (host && *host) {
        laddr = inet_addr(host);
        if ((htonl(INADDR_NONE) == laddr) && (strcmp(host, "255.255.255.255"))) {
            gethostbyname_r(host, &hret, buf, sizeof(buf), &hp, &herr);
            if (hp) {
                laddr = *((unsigned long *) (hp->h_addr));
            }
        }
    }

    return laddr;
}
#endif

/*----------------------------------------------------------------------------*/

void net_get_ipaddr (char *ipaddr, struct in_addr in)
{
    if (ipaddr) {
        pthread_mutex_lock(&net_lock);
        snprintf(ipaddr, NET_MAX_IPADDR_LEN+1, "%s", inet_ntoa(in));
        pthread_mutex_unlock(&net_lock);
    }
}

/*----------------------------------------------------------------------------*/

void net_get_hostname (char *host, int host_size)
{
    if (host) {
        pthread_mutex_lock(&net_lock);
        gethostname(host, host_size);
        if (host_size > 0) {
            host[(host_size-1)] = 0;
        }
        pthread_mutex_unlock(&net_lock);
    }
}

/******************************************************************************/
