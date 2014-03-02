/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : response_handler.cpp
 * Description : Response handler.
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
#include "oradbresponse.hpp"
#include "response_handler.hpp"

/*============================================================================*/

static char* create_cart_mt_request(response_t* response, char* buf, int buf_sz)
{
    char tmp[1024*2];
    std::string url = Config::getCartUrl();

    url.append("?CP_Id=");

    url_encode(response->txid, tmp, sizeof(tmp));
    url.append("&CSP_Txid=");
    url.append(tmp);

    url_encode(Config::getCartUserId(), tmp, sizeof(tmp));
    url.append("&CP_UserId=");
    url.append(tmp);

    url_encode(Config::getCartPassword(), tmp, sizeof(tmp));
    url.append("&CP_Password=");
    url.append(tmp);

    url_encode(response->msg, tmp, sizeof(tmp));
    url.append("&SMS_MsgTxt=");
    url.append(tmp);

    url.append("&SMS_Msgdata=");

    url_encode(response->a_no, tmp, sizeof(tmp));
    url.append("&SMS_SourceAddr=");
    url.append(tmp);

    url.append("&SUB_DeviceType=");
    url.append("&SUB_Device_Details=");

    url_encode(response->b_no, tmp, sizeof(tmp));
    url.append("&SUB_R_Mobtel=");
    url.append(tmp);
    url.append("&SUB_C_Mobtel=");
    url.append(tmp);

    url.append("&CSP_ContentType=TM");
    url.append("&CSP_A_Keyword=");
    url.append("&CSP_S_Keyword=");
    url.append("&CSP_ChargeIndicator=Yes");
    url.append("&CSP_Remarks=");

    snprintf(buf, buf_sz, "%s", url.c_str());

    return buf;
}

/*============================================================================*/

void* response_fetcher (void* arg)
{
    void* retr = NULL;

    if (OraDBResponse::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBResponse conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);

    std::vector<response_t> responses;

    while (! IS_SHUTDOWN()) {
        //-- clear vector...
        responses.clear();

        if (conn.getResponses(&responses, Config::getClusterNode(), TXN_STATUS_UNPROCESSED, Config::getOraFetchLimit()) < 0) {
            sys_msleep(1000);
            continue;
        }

        if (responses.size() == 0) {
            sys_msleep(1000);
            continue;
        }

        for (int i = 0; i < (int)responses.size(); ++i) {
            response_t& response = responses[i];
            response.status = TXN_STATUS_PROCESSED;

            if (conn.updateResponse(&response) < 0) {
                LOG_ERROR("%s: Unable to update response id: %d, status: %d.", __func__, response.id, response.status);
            } else {
                if (0 != c2q_enqueue(Global::getResponseQ(), (void*) &response, sizeof(response_t))) {
                    LOG_ERROR("%s: Unable to insert to response queue id: %d.", __func__, response.id);
                }
            }
        }
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* response_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;
    char url[1024*8];

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
            LOG_DEBUG("%s: %d: response txid: %s, a_no: %s, b_no: %s, msg: %s", __func__, proc_id,
                    response.txid, response.a_no, response.b_no, response.msg);

            create_cart_mt_request(&response, url, sizeof(url));

            HttpClient hc;
            int res_code = hc.httpGet(url);

            LOG_INFO("%s: %d: url: %s, res_code: %d, res_body: %s, res_error: %s", __func__, proc_id,
                    url, res_code, hc.getResponseBody(), hc.getError());

            response.status = (200 == res_code) ? TXN_STATUS_SUCCESSFUL : TXN_STATUS_ERROR;

            if (conn.updateResponse(&response) < 0) {
                LOG_ERROR("%s: %d: Unable to update response id: %d, status: %d.", __func__, proc_id, response.id, response.status);
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
