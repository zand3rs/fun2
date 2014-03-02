/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "httpclient.hpp"
#include "rbt.hpp"

/*============================================================================*/

int RBT::initialize()
{
    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    url = _param.svc_url;
    timeout = _param.timeout;
    memset(url_buffer,0,MAX_LENGTH);

    LOG_INFO("%s::%s: url: %s, timeout: %d", __class__, __func__, url, timeout);

    return 0;
}

/*----------------------------------------------------------------------------*/

int RBT::deinitialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);
    return 0;
}

/*----------------------------------------------------------------------------*/

int RBT::activate(const char *msisdn)
{
    time_t epoch = time(NULL);
    char* token;
    char* pbuf;
    char* p;

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    //-- http://url?MSISDN=<msisdn>&INCLUDE_FLAG&TRANSID=<trans_id>
    //-- test http client...
    snprintf(url_buffer,MAX_LENGTH,"%s?MSISDN=%s&INCLUDE_FLAG=0&TRANSID=%s%lu",url,msisdn,msisdn,(uintmax_t)epoch);
    HttpClient hc;
    int http_code = hc.httpGet(url_buffer,timeout);
    int result_code = -1;

    if (200 == http_code) {
        char result[256];
        snprintf(result, sizeof(result), "%s", hc.getResponseBody());
        result_code = 0;

        p = strstr(result, "<h1>");
        if (p) {
            token = strtok_r(p, " ", &pbuf);
            token = strtok_r(NULL, " ", &pbuf);
            if (token) {
                result_code = strtol(token, NULL, 10);
            }
        }
    }

    LOG_DEBUG("%s::%s: url: %s, http_code: %d, body: %s, result_code: %d", __class__, __func__
            , url_buffer, http_code, hc.getResponseBody(), result_code);

    return result_code;
}

/*----------------------------------------------------------------------------*/

int RBT::deactivate(const char *msisdn)
{
    time_t epoch = time(NULL);
    char* token;
    char* pbuf;
    char* p;

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    //-- http://url?MSISDN=<msisdn>&INCLUDE_FLAG&TRANSID=<trans_id>
    //-- test http client...
    snprintf(url_buffer,MAX_LENGTH,"%s?MSISDN=%s&INCLUDE_FLAG=0&TRANSID=%s%lu",url,msisdn,msisdn,(uintmax_t)epoch);
    HttpClient hc;
    int http_code = hc.httpGet(url_buffer,timeout);
    int result_code = -1;

    if (200 == http_code) {
        char result[256];
        snprintf(result, sizeof(result), "%s", hc.getResponseBody());
        result_code = 0;

        p = strstr(result, "<h1>");
        if (p) {
            token = strtok_r(p, " ", &pbuf);
            token = strtok_r(NULL, " ", &pbuf);
            if (token) {
                result_code = strtol(token, NULL, 10);
            }
        }
    }

    LOG_DEBUG("%s::%s: url: %s, http_code: %d, body: %s, result_code: %d", __class__, __func__
            , url_buffer, http_code, hc.getResponseBody(), result_code);

    return result_code;
}

/******************************************************************************/
