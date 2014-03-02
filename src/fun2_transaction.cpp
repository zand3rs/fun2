/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : fun2_transaction.cpp
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
#include "geoprobe_handler.hpp"
#include "ods_handler.hpp"
#include "ccb_handler.hpp"
#include "transaction_handler.hpp"

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

    /* -- load services from db -- */
    if (0 != Global::loadRC(Config::getOraAuth(), Config::getLibraryPath())) {
        LOG_CRITICAL("%s: Unable to load services from db (%s).", app_name, Config::getOraAuth());
        exit(-1);
    }

    /* -- initialize request queue -- */
    if (0 != c2q_init(Global::getRequestQ())) {
        LOG_CRITICAL("%s: Unable to initialize request queue!", app_name);
        exit(-1);
    }

    /* -- initialize transaction queue -- */
    if (0 != c2q_init(Global::getTransactionQ())) {
        LOG_CRITICAL("%s: Unable to initialize transaction queue!", app_name);
        exit(-1);
    }

    /* -- initialize response queue -- */
    if (0 != c2q_init(Global::getResponseQ())) {
        LOG_CRITICAL("%s: Unable to initialize response queue!", app_name);
        exit(-1);
    }

    /* -- initialize notification queue -- */
    if (0 != c2q_init(Global::getNotificationQ())) {
        LOG_CRITICAL("%s: Unable to initialize notification queue!", app_name);
        exit(-1);
    }

    /* -- initialize queue -- */
    int rc_size = Global::getRCSize();
    LOG_DEBUG("%s: rc_size: %d", app_name, rc_size);
    if (Global::initTransactionQ(rc_size) < 0) {
        LOG_CRITICAL("%s: Unable to initialize transaction queue!", app_name);
        exit(-1);
    }
    int tran_size = Global::getTransactionQSize();
    LOG_DEBUG("%s: tran_size: %d", app_name, tran_size);
    for (int i=0; i<tran_size; ++i) {
        if (0 != c2q_init(Global::getTransactionQ(i))) {
            LOG_CRITICAL("%s: Unable to initialize transaction queue: %d!", app_name, i);
            exit(-1);
        }
    }

    /* -- initialize geoprobe queue -- */
    if (0 != c2q_init(Global::getGeoProbeQ())) {
        LOG_CRITICAL("%s: Unable to initialize geoprobe queue!", app_name);
        exit(-1);
    }

    /* -- initialize ods queue -- */
    if (0 != c2q_init(Global::getODSQ())) {
        LOG_CRITICAL("%s: Unable to initialize ods queue!", app_name);
        exit(-1);
    }

    /* -- initialize ccb queue -- */
    if (0 != c2q_init(Global::getCCBQ())) {
        LOG_CRITICAL("%s: Unable to initialize ccb queue!", app_name);
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

    // Notification handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t notification_thr;
        if (0 != pthread_create(&notification_thr, &pthread_attr_norm, notification_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create notification_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(notification_thr);
    }

    // Transaction fetcher
    int proc_id = 0;
    int step_no = 0;
    rc_t* rc;
    while ((rc = Global::getNextRC(step_no))) {
        rc_proc_t* rc_proc = new(rc_proc_t);
        rc_proc->rc = rc;
        rc_proc->proc_id = proc_id;
        rc_proc->parent_id = -1;
        rc_proc->ppid = getpid();
        pthread_t tran_fetcher_thr;
        if (0 != pthread_create(&tran_fetcher_thr, &pthread_attr_norm, transaction_fetcher, (void*) rc_proc)) {
            LOG_CRITICAL("%s: Unable to create transaction_fetcher thread (%d)!!!", app_name, proc_id);
            abort();
        }
        thrs.push_back(tran_fetcher_thr);
        step_no = rc->step_no;
        proc_id++;
    }

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
    pthread_t request_fetcher_thr;
    if (0 != pthread_create(&request_fetcher_thr, &pthread_attr_norm, request_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create request_fetcher thread!!!", app_name);
        abort();
    }
    thrs.push_back(request_fetcher_thr);

    // Raw handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t raw_thr;
        if (0 != pthread_create(&raw_thr, &pthread_attr_norm, raw_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create raw_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(raw_thr);
    }

    // Raw fetcher
    pthread_t raw_fetcher_thr;
    if (0 != pthread_create(&raw_fetcher_thr, &pthread_attr_norm, raw_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create raw_fetcher thread!!!", app_name);
        abort();
    }
    thrs.push_back(raw_fetcher_thr);

    // Geoprobe handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t geoprobe_thr;
        if (0 != pthread_create(&geoprobe_thr, &pthread_attr_norm, geoprobe_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create geoprobe_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(geoprobe_thr);
    }

    // Geoprobe fetcher
    pthread_t geoprobe_fetcher_thr;
    if (0 != pthread_create(&geoprobe_fetcher_thr, &pthread_attr_norm, geoprobe_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create geoprobe_fetcher thread!!!", app_name);
        abort();
    }
    thrs.push_back(geoprobe_fetcher_thr);

    // ODS handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t ods_thr;
        if (0 != pthread_create(&ods_thr, &pthread_attr_norm, ods_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create ods_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(ods_thr);
    }

    // ODS fetcher
    pthread_t ods_fetcher_thr;
    if (0 != pthread_create(&ods_fetcher_thr, &pthread_attr_norm, ods_fetcher, NULL)) {
        LOG_CRITICAL("%s: Unable to create ods_fetcher thread!!!", app_name);
        abort();
    }
    thrs.push_back(ods_fetcher_thr);

    // CCB handler
    for (int i=0; i<Config::getThreadCount(); ++i) {
        pthread_t ccb_thr;
        if (0 != pthread_create(&ccb_thr, &pthread_attr_norm, ccb_handler, (void*)i)) {
            LOG_CRITICAL("%s: Unable to create ccb_handler thread (%d)!!!", app_name, i);
            abort();
        }
        thrs.push_back(ccb_thr);
    }

    LOG_INFO("%s: Started.", app_name);

    if (Config::bypassARDS()) {
        LOG_WARNING("%s: ARDS bypass is ENABLED!", app_name);
    }

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
    for (int i=0; i<Global::getTransactionQSize(); ++i) {
        c2q_deinit(Global::getTransactionQ(i));
    }
    Global::deinitTransactionQ();

    c2q_deinit(Global::getRequestQ());
    c2q_deinit(Global::getTransactionQ());
    c2q_deinit(Global::getResponseQ());
    c2q_deinit(Global::getGeoProbeQ());
    c2q_deinit(Global::getODSQ());
    c2q_deinit(Global::getCCBQ());

    /* -- deinitialize log -- */
    LOG_DEINIT();

    return 0;
}

/******************************************************************************/
