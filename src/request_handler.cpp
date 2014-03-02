/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : request_handler.cpp
 * Description : Request handler.
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
#include "hlr.hpp"
#include "httpdb.hpp"
#include "oradbrequest.hpp"
#include "request_handler.hpp"

/*============================================================================*/

static int getMSISDN(const char *imsi, char *msisdn, int msisdn_size)
{
    HLR hlrh(Config::getOraAuth());
    return hlrh.getMSISDN(imsi, msisdn, msisdn_size);
}

/*============================================================================*/

void* request_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBRequest::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<request_t> requests;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        requests.clear();

        if (conn.getManualRequests(&requests, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (requests.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)requests.size(); ++i) {
            request_t& request = requests[i];
            request.status = TXN_STATUS_PROCESSED;

            LOG_DEBUG("%s: request id: %d, msg: %s, a_no: %s, b_no: %s, gsm_num: %s, silent: %d, request_origin: %s, imsi: %s", __func__,
                    request.id, request.msg, request.a_no, request.b_no, request.gsm_num, request.silent, request.request_origin, request.imsi);

            if (conn.updateManualRequest(&request) < 0) {
                LOG_ERROR("%s: Unable to update request id: %d, status: %d.", __func__, request.id, request.status);
            } else {
                if (0 != c2q_enqueue(Global::getRequestQ(), (void*) &request, sizeof(request_t))) {
                    LOG_ERROR("%s: Unable to insert to request queue id: %d.", __func__, request.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* request_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBRequest::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBRequest conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        request_t request;

        while (! c2q_dequeue(Global::getRequestQ(), &request, sizeof(request_t))) {
            LOG_DEBUG("%s: %d: request id: %d, msg: %s, a_no: %s, b_no: %s, gsm_num: %s, silent: %d", __func__, proc_id,
                    request.id, request.msg, request.a_no, request.b_no, request.gsm_num, request.silent);

            request.ref_id = request.id;
            snprintf(request.b_no, sizeof(request.b_no), "%s", Config::getAccessCode()); 
            if (! *(request.request_origin)) {
                snprintf(request.request_origin, sizeof(request.request_origin), "%s", "WEB"); 
            }

            if (! strcasecmp(request.msg, "ACTIVATION")) {
                request.tran_type = TRAN_TYPE_GROAM_ON; 
            } else if (! strcasecmp(request.msg, "DEACTIVATION")) {
                request.tran_type = TRAN_TYPE_GROAM_OFF; 
            } else if (! strcasecmp(request.msg, "EXTENSION")) {
                request.tran_type = TRAN_TYPE_GROAM_EXTEND; 
            } else if (! strcasecmp(request.msg, "CANCEL")) {
                request.tran_type = TRAN_TYPE_CANCEL; 
            } else if (! strcasecmp(request.msg, "LINK")) {
                request.tran_type = TRAN_TYPE_LINK; 
            } else if (! strcasecmp(request.msg, "REG")) {
                request.tran_type = TRAN_TYPE_REG; 
            } else if (! strcasecmp(request.msg, "UNREG")) {
                request.tran_type = TRAN_TYPE_UNREG; 
            } else {
                request.tran_type = TRAN_TYPE_UNKNOWN; 
            }

            if (*(request.imsi) && strlen(request.a_no) < 3) {
                LOG_DEBUG("%s: request id: %d, invalid msisdn: %s, getting msisdn from server using imsi: %s", __func__,
                        request.id, request.a_no, request.imsi);

                //-- get msisdn
                memset(request.a_no, 0, sizeof(request.a_no));
                getMSISDN(request.imsi, request.a_no, sizeof(request.a_no));

                //-- disregard invalid msisdn
                if (strlen(request.a_no) < 3) {
                    LOG_ERROR("%s: request id: %d, got invalid msisdn: %s from server using imsi: %s", __func__,
                            request.id, request.a_no, request.imsi);
                    continue;
                }
            }

            if (conn.insertRequest(&request) < 0) {
                LOG_ERROR("%s: %d: Unable to insert request ref_id: %d.", __func__, proc_id, request.ref_id);
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

void* request_listener (void* arg)
{
    void* retr = NULL;
    HttpDB httpdb;

    httpdb.initialize(Config::getListenPort(), Config::getThreadCount(), Config::getOraAuth());

    if (0 != httpdb.start()) {
        LOG_CRITICAL("%s: Unable to start http handler!", __func__);
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started. Listening on port %d.", __func__, Config::getListenPort());

    while (! IS_SHUTDOWN()) {
        sys_msleep(1000);
    }

    httpdb.stop();

    LOG_INFO("%s: Terminated.", __func__);

    return retr;
}

/******************************************************************************/
