/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-09-14
 *     /         Updated : 2009-09-14
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : misc.cpp
 * Description : Miscellaneous functions.
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

#include "global.hpp"
#include "config.hpp"
#include "sysmsg.hpp"
#include "misc.hpp"
#include "httpclient.hpp"
#include "httputils.hpp"
#include "nsn.hpp"

/*============================================================================*/

void string_replace (char* buf, int buf_size, char* search, char* replace)
{
    int len = buf_size;
    char* src = buf;
    char* dest = (char*) malloc(len);

    if (dest) {
        memset(dest, 0, len);
        str_replace(dest, src, len, search, replace);
        snprintf(buf, buf_size, "%s", dest);
        free(dest);
    }
}

/*----------------------------------------------------------------------------*/

float getBalance(const char *msisdn)
{
    float retr = -1;
    NSN nsnh(Config::getOraAuth());

    int balance = nsnh.getBalance(msisdn);
    if (balance >= 0) {
        char buf[64];
        char buf2[64];
        snprintf(buf, sizeof(buf), "%d", balance);
        snprintf(buf2, sizeof(buf2), "%d", balance);
        if (strlen(buf) > 2) {
            sprintf(&buf2[strlen(buf2)-2], ".%s", &buf[strlen(buf)-2]);
        }
        retr = (float)strtod(buf2, NULL);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

void send_system_msg (const char* customer_type, int tran_type, int ref_id,
        const char* a_no, const char* b_no,
        const char* msg_id, int msg_part, ...)
{
    va_list ap;
    time_t epoch;
    struct tm tm;
    int status;
    response_t response;

    LOG_DEBUG("%s: msg_id: %s, msg_part: %d", __func__, msg_id, msg_part);

    string* msg_fmt = Sysmsg::getMessage(msg_id, msg_part);
    epoch = time(NULL);
    localtime_r(&epoch, &tm);

    if (msg_fmt && ! msg_fmt->empty()) {
        memset(&response, 0, sizeof(response_t));

#if 0
        snprintf(response.txid, sizeof(response.txid), "%04d%02d%02d%02d%02d%02d%08d",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ref_id);
#endif
        snprintf(response.txid, sizeof(response.txid), "fun2%s%04d%02d%02d%02d%02d%02d", b_no,
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        snprintf(response.customer_type, sizeof(response.customer_type), "%s", customer_type);
        response.tran_type = tran_type;
        response.ref_id = ref_id;
        response.cluster_node = Config::getClusterNode();
        snprintf(response.a_no, sizeof(response.a_no), "%s", a_no);
        snprintf(response.b_no, sizeof(response.b_no), "%s", b_no);
        snprintf(response.msg_id, sizeof(response.msg_id), "%s", msg_id);

        va_start(ap, msg_part);
        status = vsnprintf(response.msg, sizeof(response.msg), msg_fmt->c_str(), ap);
        va_end(ap);

        if (EOF != status) {
            if (0 != c2q_enqueue(Global::getResponseQ(), (void*) &response, sizeof(response_t))) {
                LOG_ERROR("%s: Unable to insert to response queue: ref_id: %d, a_no: %s, b_no: %s, msg: %s", __func__,
                        response.ref_id, response.a_no, response.b_no, response.msg);
            }
        } else {
            LOG_ERROR("%s: Unable to create system message: ref_id: %d, a_no: %s, b_no: %s, msg_id: %s", __func__,
                    response.ref_id, response.a_no, response.b_no, response.msg_id);
        }
    } else {
        LOG_DEBUG("%s: disregarding empty message: msg_id: %s, msg_part: %d", __func__, msg_id, msg_part);
    }
}

/*----------------------------------------------------------------------------*/

void send_system_msg (const char* customer_type, int tran_type, int ref_id,
        const char* a_no, const char* b_no,
        const char* msg_id, const char* msg, ...)
{
    va_list ap;
    time_t epoch;
    struct tm tm;
    int status;
    response_t response;

    LOG_DEBUG("%s: msg_id: %s, msg: %s", __func__, msg_id, msg);

    string msgp = msg;
    string* msg_fmt = &msgp;
    epoch = time(NULL);
    localtime_r(&epoch, &tm);

    if (msg_fmt && ! msg_fmt->empty()) {
        memset(&response, 0, sizeof(response_t));

#if 0
        snprintf(response.txid, sizeof(response.txid), "%04d%02d%02d%02d%02d%02d%08d",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ref_id);
#endif
        snprintf(response.txid, sizeof(response.txid), "fun2%s%04d%02d%02d%02d%02d%02d", b_no,
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        snprintf(response.customer_type, sizeof(response.customer_type), "%s", customer_type);
        response.tran_type = tran_type;
        response.ref_id = ref_id;
        response.cluster_node = Config::getClusterNode();
        snprintf(response.a_no, sizeof(response.a_no), "%s", a_no);
        snprintf(response.b_no, sizeof(response.b_no), "%s", b_no);
        snprintf(response.msg_id, sizeof(response.msg_id), "%s", msg_id);

        va_start(ap, msg);
        status = vsnprintf(response.msg, sizeof(response.msg), msg_fmt->c_str(), ap);
        va_end(ap);

        if (EOF != status) {
            if (0 != c2q_enqueue(Global::getResponseQ(), (void*) &response, sizeof(response_t))) {
                LOG_ERROR("%s: Unable to insert to response queue: ref_id: %d, a_no: %s, b_no: %s, msg: %s", __func__,
                        response.ref_id, response.a_no, response.b_no, response.msg);
            }
        } else {
            LOG_ERROR("%s: Unable to create system message: ref_id: %d, a_no: %s, b_no: %s, msg_id: %s", __func__,
                    response.ref_id, response.a_no, response.b_no, response.msg_id);
        }
    } else {
        LOG_DEBUG("%s: disregarding empty message: msg_id: %s, msg: %s", __func__, msg_id, msg);
    }
}

/*----------------------------------------------------------------------------*/

#ifdef __DEVEL__
int csp_charge (const char* msisdn)
{
    return 0;
}
#else
int csp_charge (const char* msisdn)
{
    char url[1024];
    time_t epoch;
    struct tm tm;

    char CSP_Txid[64];
    char CP_Id[64];
    char CP_UserId[64];
    char CP_Password[64];
    char CSP_A_Keyword[64];

    epoch = time(NULL);
    localtime_r(&epoch, &tm);

    snprintf(CSP_Txid, sizeof(CSP_Txid), "fun2%s%04d%02d%02d%02d%02d%02d", msisdn,
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    url_encode(Config::getCspCpId(), CP_Id, sizeof(CP_Id));
    url_encode(Config::getCspUserId(), CP_UserId, sizeof(CP_UserId));
    url_encode(Config::getCspPassword(), CP_Password, sizeof(CP_Password));
    url_encode(Config::getCspKeyword(), CSP_A_Keyword, sizeof(CSP_A_Keyword));

    snprintf(url, sizeof(url), "%s?CSP_Txid=%s&CP_Id=%s&CP_UserId=%s&CP_Password=%s&SUB_C_Mobtel=%s&CSP_A_Keyword=%s",
            Config::getCspUrl(), CSP_Txid, CP_Id, CP_UserId, CP_Password, msisdn, CSP_A_Keyword);

    HttpClient hc;
    int res_code;

    bool done = false;
    for (int i=0; i<2 && !done; ++i) {
        LOG_DEBUG("%s: try: %d, url: %s, timeout: %d sec", __func__,
                i+1, url, Config::getCspTimeoutSec());

        res_code = hc.httpGet(url, Config::getCspTimeoutSec());

        switch (res_code) {
            case 200:
                done = true;
                break;
            case 401:
                done = true;
                break;
            case 402:
                //-- retry
                break;
            case 405:
                //-- retry
                break;
            case 407:
                done = true;
                break;
            case 409:
                done = true;
                break;
            case 410:
                done = true;
                break;
            case 411:
                done = true;
                break;
            case 412:
                done = true;
                break;
            case 413:
                done = true;
                break;
            case 414:
                done = true;
                break;
            case 415:
                done = true;
                break;
            case 416:
                //-- retry
                break;
            case 503:
                done = true;
                break;
            default:
                done = true;
        }
    }

    LOG_INFO("%s: url: %s, res_code: %d, res_body: %s, res_error: %s", __func__,
            url, res_code, hc.getResponseBody(), hc.getError());

    return (res_code == 200) ? 0 : -1;
}
#endif

/******************************************************************************/