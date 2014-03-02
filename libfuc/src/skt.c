/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-05
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : skt.c
 * Description : TCP socket wrapper functions.
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

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "net.h"
#include "skt.h"

/*============================================================================*/

#if 0
static void mt_msleep (int msec)
{
    struct pollfd fds[1];

    fds[0].fd = -1;
    fds[0].events = POLLIN;
    poll(fds, 1, msec);
}
#endif

/*============================================================================*/

static Skt_t* _skt_new (void)
{
    Skt_t *s = (Skt_t *) malloc(sizeof(Skt_t));

    if (s) {
        memset(s, 0, sizeof(Skt_t));
    }
#ifdef __DEBUG__
    else {
        fprintf(stderr, "%s: Error on malloc(): %d: %s\n", __func__, errno, strerror(errno));
    }
#endif

    return s;
}

/*----------------------------------------------------------------------------*/

static void _skt_free (Skt_t *s)
{
    if (s) {
        memset(s, 0, sizeof(Skt_t));
        free(s);
    }
}

/*----------------------------------------------------------------------------*/

static int _skt_set_nonblocking (int fd)
{
    int fdopts;

    fdopts = fcntl(fd, F_GETFL, 0);
    if (! (fdopts & O_NONBLOCK)) {
        if (-1 == fcntl(fd, F_SETFL, fdopts|O_NONBLOCK)) {
#ifdef __DEBUG__
            fprintf(stderr, "%s: Error on fcntl(): %d: %s\n", __func__, errno, strerror(errno));
#endif
            return -1;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

static void _skt_close (int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

/*----------------------------------------------------------------------------*/

static int _skt_open (void)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd) {
#ifdef __DEBUG__
        fprintf (stderr, "%s: Error on socket(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

#if 0 //-- conflict with connect()...
    if (-1 == _skt_set_nonblocking(fd)) {
        _skt_close(fd);
        return -1;
    }
#endif

    return fd;
}

/*============================================================================*/

Skt_t* skt_open (char *host, int port)
{
    Skt_t *s = _skt_new();

    if (! s) {
        return NULL;
    }

    s->fd = _skt_open();
    if (-1 == s->fd) {
        _skt_free(s);
        return NULL;
    }

    snprintf(s->host, sizeof(s->host), "%s", (host) ? host : "");
    s->port = port;
    s->sin.sin_family = AF_INET;
    s->sin.sin_addr.s_addr = net_get_address(s->host);
    s->sin.sin_port = htons(s->port);
    s->active = 1;

    return s;
}

/*----------------------------------------------------------------------------*/

void skt_close (Skt_t *s)
{
    if (s) {
        _skt_close(s->fd);
        _skt_free(s);
    }
}

/*----------------------------------------------------------------------------*/

int skt_connect (Skt_t *s)
{
    if (! s) {
        return -1;
    }
    
    if (-1 == connect(s->fd, (struct sockaddr *) &s->sin, sizeof(s->sin))) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on connect(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    if (-1 == _skt_set_nonblocking(s->fd)) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Unable to set socket to non-blocking!\n", __func__);
#endif
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int skt_reconnect (Skt_t *s)
{
    if (! s) {
        return -1;
    }
    _skt_close(s->fd);
    s->fd = _skt_open();
    if (-1 == s->fd) {
        return -1;
    }
    return skt_connect(s);
}

/*----------------------------------------------------------------------------*/

int skt_listen (Skt_t *s)
{
    int sopt = 1;

    if (! s) {
        return -1;
    }

    if (! s->active) {
        s->fd = _skt_open();
        if (-1 == s->fd) {
            return -1;
        }
        s->active = 1;
    }

    if (-1 == _skt_set_nonblocking(s->fd)) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Unable to set socket to non-blocking!\n", __func__);
#endif
    }

    if (-1 == setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt))) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on setsockopt(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }
    
    if (-1 == setsockopt(s->fd, SOL_SOCKET, SO_KEEPALIVE, &sopt, sizeof(sopt))) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on setsockopt(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }
    
    if (-1 == bind(s->fd, (struct sockaddr *) &s->sin, sizeof(s->sin))) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on bind(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    if (-1 == listen(s->fd, SKT_MAX_BACKLOG)) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on listen(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    /* set other parameters... */
    net_get_ipaddr(s->host, s->sin.sin_addr);

    return 0;
}

/*----------------------------------------------------------------------------*/

int skt_unlisten (Skt_t *s)
{
    if (! s) {
        return -1;
    }

    _skt_close(s->fd);
    s->active = 0;

    return 0;
}

/*----------------------------------------------------------------------------*/

Skt_t* skt_accept (Skt_t *s, int mtimeout)
{
    Skt_t *c = NULL;

    if (! s) {
        return NULL;
    }
    if (! s->active) {
        return NULL;
    }

    switch (skt_read_wait(s, mtimeout)) {
        case 0:
            //-connection timed out...
            return NULL;
        case -1:
            //-poll error...
            return NULL;
        default:
            //-ok...
            //-accept connection...
            c = _skt_new();
            if (! c) {
                return NULL;
            }

            c->sin_sz = sizeof(c->sin);
            c->fd = accept(s->fd, (struct sockaddr *) &(c->sin), (socklen_t*) &(c->sin_sz));
            if (-1 == c->fd) {
#ifdef __DEBUG__
                fprintf(stderr, "%s: Error on accept(): %d: %s\n", __func__, errno, strerror(errno));
#endif
                _skt_free(c);
                return NULL;
            }

            if (-1 == _skt_set_nonblocking(c->fd)) {
                _skt_close(c->fd);
                _skt_free(c);
                return NULL;
            }
            /* set other parameters... */
            net_get_ipaddr (c->host, c->sin.sin_addr);
            c->port = htons(c->sin.sin_port);
            c->active = 1;
    }

    return c;
}

/*----------------------------------------------------------------------------*/

int skt_poll (Skt_t *s, short events, short *revents, int mtimeout)
{
    int status;

    if (! s) {
        return -1;
    }
    
    s->fds[0].fd = s->fd;
    s->fds[0].events = events;
    
    status = poll(s->fds, 1, mtimeout);
    if (revents != NULL) {
        *revents = s->fds[0].revents;
    }

    return status;
}

/*----------------------------------------------------------------------------*/

int skt_read_wait (Skt_t *s, int mtimeout)
{
    if (! s) {
        return -1;
    }
    
    s->fds[0].fd = s->fd;
    s->fds[0].events = POLLIN;
    
    return poll(s->fds, 1, mtimeout);
}

/*----------------------------------------------------------------------------*/

int skt_write_wait (Skt_t *s, int mtimeout)
{
    if (! s) {
        return -1;
    }
    
    s->fds[0].fd = s->fd;
    s->fds[0].events = POLLOUT;
    
    return poll(s->fds, 1, mtimeout);
}

/*----------------------------------------------------------------------------*/

int skt_read (Skt_t *s, char *buf, int nbytes)
{
    int status;

    if (! s) {
        return -1;
    }
    
    status = read(s->fd, buf, nbytes);
    //-set status to -1 on error and closed socket...
    if (status <= 0) {
        status = -1;
    }

    return status;
}

/*----------------------------------------------------------------------------*/

int skt_write (Skt_t *s, char *buf, int nbytes)
{
    int status;
    int sopt;
    size_t sopt_sz;

    if (! s) {
        return -1;
    }

    status = write(s->fd, buf, nbytes);
    //-check if write is really successful...
    if (status > 0) {
        sopt = 0;
        sopt_sz = sizeof(sopt);
        getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &sopt, (socklen_t*) &sopt_sz);
        if (sopt) {
            status = -1;
        }
    }
    //-set status to -1 on error and closed socket...
    if (status <= 0) {
        status = -1;
    }
    
    return status;
}

/*----------------------------------------------------------------------------*/

int skt_recv (Skt_t *s, char *buf, int nbytes, int mtimeout)
{
    int status;

    if (! s) {
        return -1;
    }
    
    status = skt_read_wait(s, mtimeout);
    switch (status) {
        case 0:
            //-connection timed out...
            break;
        case -1:
            //-poll error...
            break;
        default:
            //-ok...
            //-read packet...
            status = read(s->fd, buf, nbytes);
            //-set status to -1 on error and closed socket...
            if (status <= 0) {
                status = -1;
            }
    }
    
    return status;
}

/*----------------------------------------------------------------------------*/

int skt_send (Skt_t *s, char *buf, int nbytes, int mtimeout)
{
    int status;
    int sopt;
    size_t sopt_sz;

    if (! s) {
        return -1;
    }
    
    status = skt_write_wait(s, mtimeout);
    switch (status) {
        case 0:
            //-connection timed out...
            break;
        case -1:
            //-poll error...
            break;
        default:
            //-ok...
            //-write packet...
            status = write(s->fd, buf, nbytes);
            //-check if write is really successful...
            if (status > 0) {
                sopt = 0;
                sopt_sz = sizeof(sopt);
                getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &sopt, (socklen_t*) &sopt_sz);
                if (sopt) {
                    status = -1;
                }
            }
            //-set status to -1 on error and closed socket...
            if (status <= 0) {
                status = -1;
            }
    }

    return status;
}

/*----------------------------------------------------------------------------*/

int skt_disable_read (Skt_t *s)
{
    if (! s) {
        return -1;
    }

    return shutdown(s->fd, SHUT_RD);
}

/*----------------------------------------------------------------------------*/

int skt_disable_write (Skt_t *s)
{
    if (! s) {
        return -1;
    }

    return shutdown(s->fd, SHUT_WR);
}

/******************************************************************************/

