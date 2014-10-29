/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpdb.cpp
 * Description : http handler + oracle db
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

#include <string>

#include "libfuc.h"

#include "config.hpp"
#include "sysmsg.hpp"
#include "httputils.hpp"
#include "httpdb.hpp"

using namespace std;

/*============================================================================*/

void HttpDB::_deinit()
{
    if (_active) {
        _conn.destroy_db();
        httpStop();
        _active = 0;
    }
}

/*============================================================================*/

int HttpDB::initialize(const int port, const int nThreads, const char* ora_auth)
{
    _ora_auth = ora_auth;
    _port = port;

    if (! _initialized) {
        if (OraDBRequest::init_lib(true) < 0) {
            return -1;
        }
    }
    _initialized = 1;

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpDB::start()
{
    if (! _initialized) {
        return -1;
    }
    if (_active) {
        return -1;
    }
    if (_conn.initialize(_ora_auth.c_str()) < 0) {
        return -1;
    }

    if (httpStart(_port) < 0) {
        _conn.destroy_db();
        return -1;
    }
    _active = 1;

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpDB::stop()
{
    _deinit();
    return 0;
}

/*----------------------------------------------------------------------------*/

void HttpDB::handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse)
{
    char htmlBody[1024*4];
    const char* sKey;
    const char* sVal;
    request_t request;

    snprintf(htmlBody, sizeof(htmlBody), "*** -- FUN2 Request Handler -- *** <br><br>\n\n");

    LOG_INFO("%s: Request URI: %s", __func__, httpRequest->getUri());

    //-- check if client is requesting for the correct service...

    if (!strcmp(httpRequest->getService(), "/usurf") || !strcmp(httpRequest->getService(), "/usurf/")) {
        //-- USURF Handler...
        memset(&request, 0, sizeof(request_t));
        request.cluster_node = Config::getClusterNode();

        sKey = "msisdn";
        sVal =  httpRequest->getFormValue(sKey);
        snprintf(request.msisdn, sizeof(request.msisdn), "%s", sVal);
        snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
                "%s: %s <br>\n", sKey, sVal);
        LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.msisdn);

        sKey = "opt";
        sVal =  httpRequest->getFormValue(sKey);
        int opt = strtol(sVal, NULL, 10);
        snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
                "%s: %s <br>\n", sKey, sVal);
        LOG_DEBUG("%s: sKey=[%s], sVal=[%s]", __func__, sKey, sVal);

        sKey = "service_id";
        sVal =  httpRequest->getFormValue(sKey);
        snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
                "%s: %s <br>\n", sKey, sVal);
        LOG_DEBUG("%s: sKey=[%s], sVal=[%s]", __func__, sKey, sVal);

        sKey = "opt_time";
        sVal =  httpRequest->getFormValue(sKey);
        snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
                "%s: %s <br>\n", sKey, sVal);
        LOG_DEBUG("%s: sKey=[%s], sVal=[%s]", __func__, sKey, sVal);

        //-- check for required params
        if (! *(request.msisdn) || opt != 2) {
            LOG_ERROR("%s: Invalid request.", __func__);
        } else {
            if (_conn.usurfDeactivation(&request) < 0) {
                LOG_ERROR("%s: Unable to process request: %s.", __func__, request.msisdn);
            }
        }

        httpResponse->setBody(htmlBody);
        httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
        return;
    }

    if (strcmp(httpRequest->getService(), "/")) {
        httpResponse->setResponseCode(HTTPRESPONSECODE_404_NOTFOUND);
        return;
    }

    memset(&request, 0, sizeof(request_t));
    request.cluster_node = Config::getClusterNode();
    snprintf(request.request_origin, sizeof(request.request_origin), "%s", "SMS");

    sKey = "CSP_Txid";
    sVal = httpRequest->getFormValue(sKey);
    snprintf(request.CSP_Txid, sizeof(request.CSP_Txid), "%s", sVal);
    snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
            "%s: %s <br>\n", sKey, sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.CSP_Txid);

    sKey = "SMS_Message_String";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.SMS_Message_String, sizeof(request.SMS_Message_String), "%s", sVal);
    snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
            "%s: %s <br>\n", sKey, sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.SMS_Message_String);

    sKey = "SMS_SourceAddr";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.SMS_SourceAddr, sizeof(request.SMS_SourceAddr), "%s", sVal);
    snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
            "%s: %s <br>\n", sKey, sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.SMS_SourceAddr);

    sKey = "SUB_Mobtel";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.SUB_Mobtel, sizeof(request.SUB_Mobtel), "%s", sVal);
    snprintf(&htmlBody[strlen(htmlBody)], sizeof(htmlBody)-strlen(htmlBody),
            "%s: %s <br>\n", sKey, sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.SUB_Mobtel);

    //-- check for required params
    if (! *(request.CSP_Txid) ||
        ! *(request.SMS_Message_String) ||
        ! *(request.SMS_SourceAddr) ||
        ! *(request.SUB_Mobtel)
       ) {
        LOG_DEBUG("%s: Missing required params, disregarding request.", __func__);
    } else {
        if (_conn.insertRequest(&request) < 0) {
            LOG_ERROR("%s: Unable to insert request, txid: %s.", __func__, request.txid);
        }
    }

    httpResponse->setBody(htmlBody);
    httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
}

/******************************************************************************/
