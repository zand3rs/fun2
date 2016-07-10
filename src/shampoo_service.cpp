/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-07-10
 *     /         Updated : 2016-07-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : shampoo_service.cpp
 * Description : Shampoo Service
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
#include "httputils.hpp"
#include "shampoo_service.hpp"

using namespace std;

/*============================================================================*/

void ShampooService::_deinit()
{
    if (_active) {
        _conn.destroy_db();
        httpStop();
        _active = 0;
    }
}

/*============================================================================*/

int ShampooService::initialize(const int port, const int nThreads, const char* ora_auth)
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

int ShampooService::start()
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

int ShampooService::stop()
{
    _deinit();
    return 0;
}

/*----------------------------------------------------------------------------*/

void ShampooService::handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse)
{
    char htmlBody[1024*4];
    const char* sKey;
    const char* sVal;
    request_t request;

    snprintf(htmlBody, sizeof(htmlBody), "*** -- FUN2 Shampoo Handler -- *** <br><br>\n\n");

    LOG_INFO("%s: Request URI: %s", __func__, httpRequest->getUri());

    memset(&request, 0, sizeof(request_t));
    request.cluster_node = Config::getClusterNode();

    //-- check if client is requesting for the correct service...
    if (!strcmp(httpRequest->getService(), "/subscribe") || !strcmp(httpRequest->getService(), "/subscribe/")) {
        snprintf(request.svc_type, sizeof(request.svc_type), "SUBSCRIBE");
    } else if (!strcmp(httpRequest->getService(), "/unsubscribe") || !strcmp(httpRequest->getService(), "/unsubscribe/")) {
        snprintf(request.svc_type, sizeof(request.svc_type), "UNSUBSCRIBE");
    } else {
        LOG_ERROR("%s: Service not found: %s", __func__, httpRequest->getService());
        httpResponse->setResponseCode(HTTPRESPONSECODE_404_NOTFOUND);
        return;
    }

    sKey = "msisdn";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.svc_msisdn, sizeof(request.svc_msisdn), "%s", sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.svc_msisdn);

    sKey = "plan";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.svc_plan, sizeof(request.svc_plan), "%s", sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.svc_plan);

    sKey = "start_date";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.svc_start, sizeof(request.svc_start), "%s", sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.svc_start);

    sKey = "end_date";
    sVal =  httpRequest->getFormValue(sKey);
    snprintf(request.svc_end, sizeof(request.svc_end), "%s", sVal);
    LOG_DEBUG("%s: sKey=[%s], sVal=[%s], %s=[%s]", __func__, sKey, sVal, sKey, request.svc_end);

    //-- check for required params
    if (! *(request.svc_msisdn)) {
        LOG_ERROR("%s: Invalid request.", __func__);
    } else {
        if (_conn.processShampoo(&request) < 0) {
            LOG_ERROR("%s: process_shampoo failed msisdn: %s, tran_type: %s", __func__, request.svc_msisdn, request.svc_type);
        }
    }

    //httpResponse->setBody(htmlBody);
    httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
}

/******************************************************************************/
