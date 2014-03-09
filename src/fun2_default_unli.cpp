/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2014-03-03
 *     /         Updated : 2014-03-03
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : fun2_default_unli.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <vector>

#include "global.hpp"
#include "config.hpp"
#include "oradb.hpp"
#include "signal_handler.hpp"
#include "default_unli_handler.hpp"

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

    /* -- initialize default_unli queue -- */
    if (0 != c2q_init(Global::getDefaultUnliQ())) {
        LOG_CRITICAL("%s: Unable to initialize default_unli queue!", app_name);
        exit(-1);
    }

    /* -- initialize default_unli file queue -- */
    if (0 != c2q_init(Global::getDefaultUnliFileQ())) {
        LOG_CRITICAL("%s: Unable to initialize default_unli file queue!", app_name);
        exit(-1);
    }

    /* -- check local processed dir -- */
    if (0 != access(Config::getLocalProcessedDir(), F_OK)) {
        if (ENOTDIR == errno) {
            // exists but not a directory
            LOG_CRITICAL("%s: %s should be a directory!", app_name, Config::getLocalProcessedDir());
            exit(-1);
        }
        if (ENOENT == errno) {
            // does not exist, try to create it
            if (0 != mkdir(Config::getLocalProcessedDir(), 0775)) {
                LOG_CRITICAL("%s: Unable to create directory: %s", app_name, Config::getLocalProcessedDir());
                exit(-1);
            }
        }
    }

    /* -- check local completed dir -- */
    if (0 != access(Config::getLocalCompletedDir(), F_OK)) {
        if (ENOTDIR == errno) {
            // exists but not a directory
            LOG_CRITICAL("%s: %s should be a directory!", app_name, Config::getLocalCompletedDir());
            exit(-1);
        }
        if (ENOENT == errno) {
            // does not exist, try to create it
            if (0 != mkdir(Config::getLocalCompletedDir(), 0775)) {
                LOG_CRITICAL("%s: Unable to create directory: %s", app_name, Config::getLocalCompletedDir());
                exit(-1);
            }
        }
    }

    /* -- some info -- */
    LOG_INFO("%s: local directory: %s", __func__, Config::getLocalDir());
    LOG_INFO("%s: processed local directory: %s", __func__, Config::getLocalProcessedDir());
    LOG_INFO("%s: completed local directory: %s", __func__, Config::getLocalCompletedDir());

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

    // DefaultUnli handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t default_unli_thr;
        if (0 != pthread_create(&default_unli_thr, &pthread_attr_norm, default_unli_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create default_unli_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(default_unli_thr);
    }

    // DefaultUnli parser
    pthread_t default_unli_parser_thr;
    if (0 != pthread_create(&default_unli_parser_thr, &pthread_attr_norm, default_unli_parser, NULL)) {
        LOG_CRITICAL("%s: Unable to create default_unli_parser thread (%d)!!!", app_name);
        abort();
    }
    thrs.push_back(default_unli_parser_thr);

    // DefaultUnli fetcher
    pthread_t default_unli_fetcher_thr;
    if (0 != pthread_create(&default_unli_fetcher_thr, &pthread_attr_norm, default_unli_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create default_unli_fetcher thread (%d)!!!", app_name);
        abort();
    }
    thrs.push_back(default_unli_fetcher_thr);

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
    c2q_deinit(Global::getDefaultUnliQ());

    /* -- deinitialize log -- */
    LOG_DEINIT();

    return 0;
}

/******************************************************************************/
