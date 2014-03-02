/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-10
 *     /         Updated : 2009-09-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : broadcast_handler.cpp
 * Description : Broadcast handler.
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
#include "misc.hpp"
#include "httpclient.hpp"
#include "httputils.hpp"
#include "oradbdailybal.hpp"
#include "oradbbroadcast.hpp"
#include "oradbresponse.hpp"
#include "broadcast_handler.hpp"

/*============================================================================*/

void* daily_bal_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBDailyBal::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBDailyBal conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<daily_bal_t> daily_bals;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        daily_bals.clear();

        if (conn.getDailyBals(&daily_bals, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (daily_bals.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)daily_bals.size(); ++i) {
            daily_bal_t& daily_bal = daily_bals[i];

            //-- set status
            daily_bal.status = TXN_STATUS_PROCESSED;

            if (conn.updateDailyBal(&daily_bal) < 0) {
                LOG_ERROR("%s: Unable to update daily_bal id: %d, msisdn: %s, daily_balance: %d", __func__
                        , daily_bal.id, daily_bal.msisdn, daily_bal.daily_balance);
            } else {
                if (0 != c2q_enqueue(Global::getDailyBalQ(), (void*) &daily_bal, sizeof(daily_bal_t))) {
                    LOG_ERROR("%s: Unable to insert to daily_bal queue id: %d, msisdn: %s.", __func__, daily_bal.id, daily_bal.msisdn);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* daily_bal_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBDailyBal::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBDailyBal conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        daily_bal_t daily_bal;

        while (! c2q_dequeue(Global::getDailyBalQ(), &daily_bal, sizeof(daily_bal_t))) {
            LOG_DEBUG("%s: %d: daily_bal id:%d, msisdn: %s, daily_ctr: %d, daily_balance: %d", __func__, proc_id
                    , daily_bal.id, daily_bal.msisdn, daily_bal.daily_ctr, daily_bal.daily_balance);

            float current_balance = getBalance(daily_bal.msisdn);
            LOG_DEBUG("%s: %d: GET BALANCE: msisdn: %s, daily_balance: %d, current_balance: %0.2f", __func__, proc_id
                    , daily_bal.msisdn, daily_bal.daily_balance, current_balance);

            //-- set current balance
            daily_bal.current_balance = (int)current_balance;

            //-- init status
            daily_bal.status = TXN_STATUS_ERROR;

            if (daily_bal.current_balance >= 0) {
                if (conn.processDailyBal(&daily_bal) < 0) {
                    LOG_ERROR("%s: %d: Unable to process daily_bal msisdn: %s, daily_ctr: %d, daily_balance: %d, current_balance: %d", __func__, proc_id
                            , daily_bal.msisdn, daily_bal.daily_ctr, daily_bal.daily_balance, daily_bal.current_balance);
                } else {
                    //-- set status
                    daily_bal.status = TXN_STATUS_SUCCESSFUL;

                    switch (daily_bal.db_retr) {
                        case DB_RETR_DAILY_BALANCE_INSUFF_BAL:
                            send_system_msg("TRIGGER", TRAN_TYPE_DAILY_BALANCE, 0,
                                    Config::getAccessCode(), daily_bal.msisdn, SYSMSG_DAILY_BALANCE_INSUFF_BAL, 1,
                                    daily_bal.deactivation_date, daily_bal.deactivation_date);
                            break;
                        case DB_RETR_DAILY_BALANCE_INSUFF_BAL_DEACT:
                        default:
                            LOG_DEBUG("%s: %d: Unhandled retr: %d, msisdn: %s, daily_ctr: %d, daily_balance: %d, current_balance: %d", __func__, proc_id
                                    , daily_bal.db_retr, daily_bal.msisdn, daily_bal.daily_ctr, daily_bal.daily_balance, daily_bal.current_balance);
                            break;
                    }
                }
            }

            //-- update status...
            if (conn.updateDailyBal(&daily_bal) < 0) {
                LOG_ERROR("%s: %d: Unable to update daily_bal id: %d, msisdn: %s, daily_balance: %d, current_balance: %d", __func__, proc_id
                        , daily_bal.id, daily_bal.msisdn, daily_bal.daily_balance, daily_bal.current_balance);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* broadcast_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBBroadcast::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBBroadcast conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<broadcast_t> broadcasts;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        broadcasts.clear();

        if (conn.getBroadcasts(&broadcasts, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (broadcasts.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)broadcasts.size(); ++i) {
            broadcast_t& broadcast = broadcasts[i];

            //--set status to processed...
            broadcast.status = TXN_STATUS_PROCESSED;

            if (conn.updateBroadcast(&broadcast) < 0) {
                LOG_ERROR("%s: Unable to update broadcast id: %d", __func__, broadcast.id);
            } else {
                if (0 != c2q_enqueue(Global::getBroadcastQ(), (void*) &broadcast, sizeof(broadcast_t))) {
                    LOG_ERROR("%s: Unable to insert to broadcast queue id: %d", __func__, broadcast.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* broadcast_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBBroadcast::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBBroadcast conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        broadcast_t broadcast;

        while (! c2q_dequeue(Global::getBroadcastQ(), &broadcast, sizeof(broadcast_t))) {
            LOG_DEBUG("%s: %d: broadcast id: %d, msisdn: %s, msg_id: %s, msg: %s", __func__, proc_id
                    , broadcast.id, broadcast.msisdn, broadcast.msg_id, broadcast.msg);

            if (*(broadcast.msg)) {
                send_system_msg("TRIGGER", TRAN_TYPE_BROADCAST, 0,
                        Config::getAccessCode(), broadcast.msisdn, broadcast.msg_id, broadcast.msg);
            } else {
                send_system_msg("TRIGGER", TRAN_TYPE_BROADCAST, 0,
                        Config::getAccessCode(), broadcast.msisdn, broadcast.msg_id, 1);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* response_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBResponse::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBResponse conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        response_t response;

        while (! c2q_dequeue(Global::getResponseQ(), &response, sizeof(response_t))) {
            LOG_DEBUG("%s: %d: response txid: %s, ref_id: %d, a_no: %s, b_no: %s, msg_id: %s, msg: %s", __func__, proc_id,
                    response.txid, response.ref_id, response.a_no, response.b_no, response.msg_id, response.msg);

            if (conn.insertResponse(&response) < 0) {
                LOG_ERROR("%s: %d: Unable to insert response ref_id: %d, a_no: %s, b_no: %s, msg: %s", __func__, proc_id,
                        response.ref_id, response.a_no, response.b_no, response.msg);
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
