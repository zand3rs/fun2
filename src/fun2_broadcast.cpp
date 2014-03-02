/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-10
 *     /         Updated : 2009-09-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : fun2_broadcast.cpp
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
#include "broadcast_handler.hpp"

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

    /* -- initialize daily_bal queue -- */
    if (0 != c2q_init(Global::getDailyBalQ())) {
        LOG_CRITICAL("%s: Unable to initialize daily_bal queue!", app_name);
        exit(-1);
    }

    /* -- initialize broadcast queue -- */
    if (0 != c2q_init(Global::getBroadcastQ())) {
        LOG_CRITICAL("%s: Unable to initialize broadcast queue!", app_name);
        exit(-1);
    }

    /* -- initialize response queue -- */
    if (0 != c2q_init(Global::getResponseQ())) {
        LOG_CRITICAL("%s: Unable to initialize response queue!", app_name);
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

    // Response handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t response_thr;
        if (0 != pthread_create(&response_thr, &pthread_attr_norm, response_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create response_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(response_thr);
    }

    // Daily Balance handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t daily_bal_thr;
        if (0 != pthread_create(&daily_bal_thr, &pthread_attr_norm, daily_bal_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create daily_bal_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(daily_bal_thr);
    }

    // Daily Balance fetcher
    pthread_t daily_bal_fetcher_thr;
    if (0 != pthread_create(&daily_bal_fetcher_thr, &pthread_attr_norm, daily_bal_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create daily_bal_fetcher thread (%d)!!!", app_name);
        abort();
    }
    thrs.push_back(daily_bal_fetcher_thr);

    // Broadcast handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t broadcast_thr;
        if (0 != pthread_create(&broadcast_thr, &pthread_attr_norm, broadcast_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create broadcast_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(broadcast_thr);
    }

    // Broadcast fetcher
    pthread_t broadcast_fetcher_thr;
    if (0 != pthread_create(&broadcast_fetcher_thr, &pthread_attr_norm, broadcast_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create broadcast_fetcher thread (%d)!!!", app_name);
        abort();
    }
    thrs.push_back(broadcast_fetcher_thr);

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
    c2q_deinit(Global::getDailyBalQ());
    c2q_deinit(Global::getBroadcastQ());
    c2q_deinit(Global::getResponseQ());

    /* -- deinitialize log -- */
    LOG_DEINIT();

    return 0;
}

/******************************************************************************/
