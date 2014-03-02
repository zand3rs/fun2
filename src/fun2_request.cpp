/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : fun2_request.cpp
 * Description : Main application.
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
#include <pthread.h>

#include <vector>

#include "global.hpp"
#include "config.hpp"
#include "oradb.hpp"
#include "sysmsg.hpp"
#include "signal_handler.hpp"
#include "request_handler.hpp"

/*============================================================================*/

static void usage (const char *app)
{
    fprintf(stdout, "%s v%s (Copyright %s)\n", PACKAGE_NAME, PACKAGE_VERSION, COPYRIGHT);
    fprintf(stdout, "%s -c <config_file>\n", app);
}

/*============================================================================*/

int main (int argc, char *argv[])
{
    char cfg_file[256];
    char app_name[256];
    char* p;

    p = strrchr(argv[0], '/');
    snprintf(app_name, sizeof(app_name), "%s", p+1);

    /* -- get the config filename -- */
    switch (sys_getopt(argc, argv, "c:", cfg_file, sizeof(cfg_file))) {
        case 'c':
#ifdef __DEBUG__
            fprintf(stdout, "config file = [%s]\n", cfg_file);
#endif
            if (0 != Config::load(cfg_file)) {
                fprintf(stderr, "%s: Unable to load config file [%s]!!!\n", app_name, cfg_file);
                exit(-1);
            }
            break;
        default:
            usage(argv[0]);
            exit(0);
    }

    /* -- check if already running -- */
    if (0 != sys_bind(Config::getLockPort())) {
        fprintf(stderr, "%s: Unable to continue, make sure there's no other instance running!!!\n", app_name);
        exit(-1);
    }

    /* -- initialize log -- */
    LOG_INIT(Config::getLogFile(), Config::getLogLevel());

    /* -- check if logger is successfully initialized -- */
    if (0 != LOG_GET_STATUS()) {
        fprintf(stderr, "%s: Unable to initialize logger!\n", app_name);
        exit(-1);
    }

    /* -- initialize libsqlora8 -- */
    if (OraDB::init_lib(true) < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", app_name);
        exit(-1);
    }

    /* -- load system messages from db -- */
    if (0 != Sysmsg::load(Config::getOraAuth())) {
        LOG_CRITICAL("%s: Unable to load system messages from db (%s).", app_name, Config::getOraAuth());
        exit(-1);
    }

    /* -- initialize queue -- */
    if (0 != c2q_init(Global::getRequestQ())) {
        LOG_CRITICAL("%s: Unable to initialize request queue!", app_name);
        exit(-1);
    }

    /* -- run in background -- */
    sys_daemon();

    /* -- block all signals -- */
    sys_sigblock();

    // Thread attributes
    pthread_attr_t pthread_attr_norm;
    pthread_attr_init(&pthread_attr_norm);
    pthread_attr_setstacksize(&pthread_attr_norm, Global::thr_stack_size);

    std::vector<pthread_t> thrs;

    // Signal handler
    pthread_t signal_thr;
    if (0 != pthread_create(&signal_thr, &pthread_attr_norm, signal_handler, NULL)) {
        LOG_CRITICAL("%s: Unable to create signal_handler thread!!!", app_name);
        exit(-1);
    }
    thrs.push_back(signal_thr);

    // Request handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t request_thr;
        if (0 != pthread_create(&request_thr, &pthread_attr_norm, request_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create request_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(request_thr);
    }

    // Request fetcher
    pthread_t req_fetcher_thr;
    if (0 != pthread_create(&req_fetcher_thr, &pthread_attr_norm, request_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create request_fetcher thread!!!", app_name);
        abort();
    }
    thrs.push_back(req_fetcher_thr);

    // Request listener
    pthread_t req_listener_thr;
    if (0 != pthread_create(&req_listener_thr, &pthread_attr_norm, request_listener, NULL)) {
        LOG_CRITICAL("%s: Unable to create request_listener thread!!!", app_name);
        abort();
    }
    thrs.push_back(req_listener_thr);

    LOG_INFO("%s: Started.", app_name);

    while (! IS_SHUTDOWN()) {
        sys_msleep(1000);
    }

#if 0
    for (int i=0; i<(int)thrs.size(); ++i) {
        pthread_join(thrs[i], NULL);
    }
#endif

    while (! thrs.empty()) {
        pthread_join(thrs.back(), NULL);
        thrs.pop_back();
    }

    LOG_INFO("%s: Terminated.", app_name);

    /* -- deinitialize queue -- */
    c2q_deinit(Global::getRequestQ());

    /* -- deinitialize log -- */
    LOG_DEINIT();

    return 0;
}

/******************************************************************************/