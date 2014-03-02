/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-10-26
 *     /         Updated : 2009-10-26
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : ods_handler.cpp
 * Description : ODS handler.
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
#include "oradbods.hpp"
#include "ods_handler.hpp"

/*============================================================================*/

void* ods_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBODS::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBODS conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<ods_t> odss;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        odss.clear();

        if (conn.getODSs(&odss, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (odss.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)odss.size(); ++i) {
            ods_t& ods = odss[i];

            //-- set status
            ods.status = TXN_STATUS_PROCESSED;

            if (conn.updateODS(&ods) < 0) {
                LOG_ERROR("%s: Unable to update ods id: %d", __func__, ods.id);
            } else {
                if (0 != c2q_enqueue(Global::getODSQ(), (void*) &ods, sizeof(ods_t))) {
                    LOG_ERROR("%s: Unable to insert to ods queue id: %d.", __func__, ods.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* ods_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBODS::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBODS conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        ods_t ods;

        while (! c2q_dequeue(Global::getODSQ(), &ods, sizeof(ods_t))) {
            LOG_DEBUG("%s: %d: ods id: %d, msisdn: %s", __func__, proc_id
                    , ods.id, ods.msisdn);

            if (conn.processODS(&ods) < 0) {
                LOG_ERROR("%s: %d: Unable to process ods id: %d, msisdn: %s", __func__, proc_id
                        , ods.id, ods.msisdn);
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
