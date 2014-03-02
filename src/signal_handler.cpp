/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : signal_handler.cpp
 * Description : Signal handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.hpp"
#include "signal_handler.hpp"

/*============================================================================*/

void* signal_handler (void* arg)
{
    int sig;
    void* retr = NULL;

    while (1) {
        /* wait for a signal */
        sig = sys_sigwait();

#ifdef __DEBUG__
        fprintf(stdout, "signal catched = [%d]\n", sig);
#endif
        switch (sig) {
            case SIGTERM:
                LOG_WARNING("Received SIGTERM, shutting down...");
                DO_SHUTDOWN();
                return retr;
            case SIGINT:
                LOG_WARNING("Received SIGINT, shutting down...");
                DO_SHUTDOWN();
                return retr;
            case SIGABRT:
                LOG_CRITICAL("Received SIGABRT, shutting down...");
                DO_SHUTDOWN();
                return retr;
            case SIGHUP:
                LOG_WARNING("Received SIGHUP, rotating logs...");
                LOG_ROTATE();
                break;
            case SIGPIPE:
                LOG_WARNING("Received SIGPIPE, ignoring signal...");
                break;
            default:
#ifdef __DEBUG__
                fprintf(stderr, "Unhandled SIGNAL...\n");
#endif
                LOG_CRITICAL("Unhandled SIGNAL [%d], exiting...", sig);
                DO_SHUTDOWN();
                return retr;
        }
    }
}

/******************************************************************************/
