/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2008-09-05
 *     /         Updated : 2009-07-25
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : sys.c
 * Description : System miscellaneous functions.
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

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netdb.h>

#include "sys.h"

/*============================================================================*/

void sys_msleep (int msec)
{
    struct pollfd fds[1];

    fds[0].fd = -1;
    fds[0].events = POLLIN;
    poll(fds, 1, msec);
}

/*----------------------------------------------------------------------------*/

void sys_daemon (void)
{
    pid_t mypid;
        
    mypid = fork();
    if (-1 == mypid) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on fork(): %d: %s.\n", __func__, errno, strerror(errno));
#endif
        fprintf(stderr, "Unable to start background process...\n");
        exit(-1);
    } 
    if (0 != mypid) {
        fprintf(stdout, "Starting background process...\n");
        exit(0);
    }
    setsid();
    mypid = getpid();
}

/*----------------------------------------------------------------------------*/

int sys_bind (int port)
{
    struct sockaddr_in sin;
    int fd, sopt;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on socket(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sin.sin_port = htons(port);

    sopt = 1;
#if 0
    if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt))) {
        fprintf(stderr, "%s: Error on setsockopt(): %d: %s\n", __func__, errno, strerror(errno));
        return -1;
    }
#endif

    if (-1 == bind(fd, (struct sockaddr *) &sin, sizeof(sin))) {
#ifdef __DEBUG__
        fprintf(stderr, "%s: Error on bind(): %d: %s\n", __func__, errno, strerror(errno));
#endif
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/*
 *  opt = sys_getopt(argc, argv, "c:", opt_arg);
 *  Returns the option character, -1 otherwise...
 */
int sys_getopt (int argc, char **argv, char *opts, char *opt_arg, int opt_arg_size)
{
    int opt;

    if (! opts || ! opt_arg) {
        return -1;
    }
    opt = getopt(argc, argv, opts);
    snprintf(opt_arg, opt_arg_size, "%s", (optarg) ? optarg : "");

    return opt;
}

/*----------------------------------------------------------------------------*/

void sys_sigblock (void)
{
    sigset_t sigset;

    /* block all signals except for SIGSEGV */
    sigfillset(&sigset);
    sigdelset(&sigset, SIGSEGV);

    pthread_sigmask(SIG_SETMASK, &sigset, NULL);
    /* ignore zombies */
    signal(SIGCHLD, SIG_IGN);
}

/*----------------------------------------------------------------------------*/
/*
 * Wait for a signal and returns it, -1 on error...
 */
int sys_sigwait (void)
{
    sigset_t sigset;
    int sig;

    /* catch all signals except for SIGSEGV */
    sigfillset(&sigset); 
    sigdelset(&sigset, SIGSEGV);

    if (sigwait(&sigset, &sig) != 0) {
        return -1;
    }
    return sig;
}

/*----------------------------------------------------------------------------*/
/*
 * Send SIGABRT signal to every process in the process group of the current process.
 */
void sys_abort (void)
{
    kill(0, SIGABRT);
}

/******************************************************************************/

