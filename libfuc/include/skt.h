/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-05
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : skt.h
 * Description : TCP socket wrapper functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef __SKT_H__
#define __SKT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-defines--------------------------------------------------------------------*/
    
#define SKT_MAX_BACKLOG 5

typedef struct Skt {
    struct pollfd fds[1];
    struct sockaddr_in sin;
    size_t sin_sz;
    char host[256];
    int port;
    int fd;
    char active;
} Skt_t;

/*-functions------------------------------------------------------------------*/

Skt_t* skt_open (char *host, int port);
void   skt_close (Skt_t *s);
int    skt_connect (Skt_t *s);
int    skt_reconnect (Skt_t *s);
int    skt_listen (Skt_t *s);
int    skt_unlisten (Skt_t *s);
Skt_t* skt_accept (Skt_t *s, int mtimeout);
int    skt_poll (Skt_t *s, short events, short *revents, int mtimeout);
int    skt_read_wait (Skt_t *s, int mtimeout);
int    skt_write_wait (Skt_t *s, int mtimeout);
int    skt_read (Skt_t *s, char *buf, int nbytes);
int    skt_write (Skt_t *s, char *buf, int nbytes);
int    skt_recv (Skt_t *s, char *buf, int nbytes, int mtimeout);
int    skt_send (Skt_t *s, char *buf, int nbytes, int mtimeout);
int    skt_disable_read (Skt_t *s);
int    skt_disable_write (Skt_t *s);

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/

