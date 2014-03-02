/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-21
 *     /         Updated : 2009-09-21
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : geoprobe_handler.cpp
 * Description : Geoprobe handler.
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
#include "oradbgeoprobe.hpp"
#include "geoprobe_handler.hpp"

/*============================================================================*/

void* geoprobe_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBGeoProbe::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBGeoProbe conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<geo_probe_t> geo_probes;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        geo_probes.clear();

        if (conn.getGeoProbes(&geo_probes, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (geo_probes.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)geo_probes.size(); ++i) {
            geo_probe_t& geo_probe = geo_probes[i];

            //-- set status
            geo_probe.status = TXN_STATUS_PROCESSED;

            if (conn.updateGeoProbe(&geo_probe) < 0) {
                LOG_ERROR("%s: Unable to update geo_probe id: %d", __func__, geo_probe.id);
            } else {
                if (0 != c2q_enqueue(Global::getGeoProbeQ(), (void*) &geo_probe, sizeof(geo_probe_t))) {
                    LOG_ERROR("%s: Unable to insert to geo_probe queue id: %d.", __func__, geo_probe.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* geoprobe_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBGeoProbe::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBGeoProbe conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        geo_probe_t geo_probe;

        while (! c2q_dequeue(Global::getGeoProbeQ(), &geo_probe, sizeof(geo_probe_t))) {
            LOG_DEBUG("%s: %d: geo_probe id: %d, imsi: %s, sccp: %s", __func__, proc_id
                    , geo_probe.id, geo_probe.imsi, geo_probe.sccp);

            if (conn.processGeoProbe(&geo_probe) < 0) {
                LOG_ERROR("%s: %d: Unable to process geo_probe id: %d, msisdn: %s, imsi: %s, sccp: %s", __func__, proc_id
                        , geo_probe.id, geo_probe.msisdn, geo_probe.imsi, geo_probe.sccp);
            } else {
                LOG_DEBUG("%s: %d: geo_probe id: %d, imsi: %s, sccp: %s, msisdn: %s, country: %s, deactivation_date: %s", __func__, proc_id
                        , geo_probe.id, geo_probe.imsi, geo_probe.sccp, geo_probe.msisdn, geo_probe.country, geo_probe.deactivation_date);

                switch (geo_probe.db_retr) {
                    case DB_RETR_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY:
                        send_system_msg("TRIGGER", TRAN_TYPE_GEO_PROBE, 0,
                                Config::getAccessCode(), geo_probe.msisdn, SYSMSG_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY, 1
                                , geo_probe.country, geo_probe.deactivation_date);
                        break;
                    case DB_RETR_GEO_PROBE_CHANGE_TO_HOT_COUNTRY:
                        send_system_msg("TRIGGER", TRAN_TYPE_GEO_PROBE, 0,
                                Config::getAccessCode(), geo_probe.msisdn, SYSMSG_GEO_PROBE_CHANGE_TO_HOT_COUNTRY, 1
                                , geo_probe.country, geo_probe.deactivation_date);
                        break;
                    case DB_RETR_GEO_PROBE_EXCEED_STAY_LIMIT_ON_HOT_COUNTRY:
                        send_system_msg("TRIGGER", TRAN_TYPE_GEO_PROBE, 0,
                                Config::getAccessCode(), geo_probe.msisdn, SYSMSG_GEO_PROBE_EXCEED_STAY_LIMIT_ON_HOT_COUNTRY, 1);
                        break;
                    case DB_RETR_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY_W_DURATION:
                        send_system_msg("TRIGGER", TRAN_TYPE_GEO_PROBE, 0,
                                Config::getAccessCode(), geo_probe.msisdn, SYSMSG_GEO_PROBE_CHANGE_TO_NON_HOT_COUNTRY_W_DURATION, 1
                                , geo_probe.country, geo_probe.deactivation_date);
                        break;
                }
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
