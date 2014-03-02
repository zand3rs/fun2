/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-10-26
 *     /         Updated : 2009-10-26
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : ccb_handler.cpp
 * Description : CCB handler.
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
#include <signal.h>

#include "global.hpp"
#include "config.hpp"
#include "oradbccb.hpp"
#include "ccb_handler.hpp"

/*============================================================================*/

void ccb_insert (const char* msisdn, const char* action)
{
    ccb_t ccb;

    memset(&ccb, 0, sizeof(ccb_t));
    snprintf(ccb.msisdn, sizeof(ccb.msisdn), "%s", msisdn);
    snprintf(ccb.ccb_action, sizeof(ccb.ccb_action), "%s", action);
    ccb.cluster_node = Config::getClusterNode();
    ccb.status = TXN_STATUS_UNPROCESSED;

    if (0 != c2q_enqueue(Global::getCCBQ(), (void*) &ccb, sizeof(ccb_t))) {
        LOG_ERROR("%s: Unable to insert to CCB queue: msisdn: %s, ccb_action: %s", __func__,
                ccb.msisdn, ccb.ccb_action);
    }
}

/*----------------------------------------------------------------------------*/

void* ccb_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBCCB::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBCCB conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<ccb_t> ccbs;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        ccbs.clear();

        if (conn.getCCBs(&ccbs, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (ccbs.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)ccbs.size(); ++i) {
            ccb_t& ccb = ccbs[i];

            //-- set status
            ccb.status = TXN_STATUS_PROCESSED;

            if (conn.updateCCB(&ccb) < 0) {
                LOG_ERROR("%s: Unable to update ccb id: %d", __func__, ccb.id);
            } else {
                if (0 != c2q_enqueue(Global::getCCBQ(), (void*) &ccb, sizeof(ccb_t))) {
                    LOG_ERROR("%s: Unable to insert to ccb queue id: %d.", __func__, ccb.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* ccb_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBCCB::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBCCB conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        ccb_t ccb;

        while (! c2q_dequeue(Global::getCCBQ(), &ccb, sizeof(ccb_t))) {
            LOG_DEBUG("%s: %d: ccb id: %d, msisdn: %s, ccb_action: %s, cluster_node: %d, status: %d", __func__, proc_id
                    , ccb.id, ccb.msisdn, ccb.ccb_action, ccb.cluster_node, ccb.status);

            if (conn.insertCCB(&ccb) < 0) {
                LOG_ERROR("%s: Unable to insert ccb msisdn: %s, ccb_action: %s", __func__, ccb.msisdn, ccb.ccb_action);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/******************************************************************************/
