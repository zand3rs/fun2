/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-06-28
 *     /         Updated : 2016-06-28
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : mlp_service.cpp
 * Description : MLP Service
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

#include "ezxml.h"
#include "libfuc.h"

#include "config.hpp"
#include "httputils.hpp"
#include "mlp_service.hpp"

using namespace std;

/*============================================================================*/

void MlpService::_deinit()
{
    if (_active) {
        _conn.destroy_db();
        httpStop();
        _active = 0;
    }
}

/*============================================================================*/

int MlpService::initialize(const int port, const int nThreads, const char* ora_auth)
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

int MlpService::start()
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

int MlpService::stop()
{
    _deinit();
    return 0;
}

/*----------------------------------------------------------------------------*/

void MlpService::handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse)
{
    char htmlBody[1024*4];
    const char* sKey;
    const char* sVal;
    request_t request;

    snprintf(htmlBody, sizeof(htmlBody), "*** -- FUN2 MLP Handler -- *** <br><br>\n\n");

    LOG_INFO("%s: Request URI: %s", __func__, httpRequest->getUri());

    //-- check if client is requesting for the correct service...
    if (strcmp(httpRequest->getService(), "/subscribe") && strcmp(httpRequest->getService(), "/subscribe/")) {
        httpResponse->setResponseCode(HTTPRESPONSECODE_404_NOTFOUND);
        return;
    }

    memset(&request, 0, sizeof(request_t));
    request.cluster_node = Config::getClusterNode();

    snprintf(htmlBody, sizeof(htmlBody), "%s", httpRequest->getBody());

    //-- parse xml
    ezxml_t xml = ezxml_parse_str(htmlBody, strlen(htmlBody));
    ezxml_t head = ezxml_child(xml, "Header");
    ezxml_t body = ezxml_child(xml, "Body");

    const char* TransactionCode = ezxml_child(head, "TransactionCode")->txt;
    const char* TransactionID = ezxml_child(head, "TransactionID")->txt;
    const char* MSISDN = ezxml_child(head, "MSISDN")->txt;
    const char* BillCycleNo = ezxml_child(head, "BillCycleNo")->txt;

    LOG_DEBUG("%s: TransactionCode=[%s], TransactionID=[%s], MSISDN=[%s], BillCycleNo=[%s]", __func__,
            TransactionCode, TransactionID, MSISDN, BillCycleNo);

    snprintf(request.svc_txcode, sizeof(request.svc_txcode), "%s", TransactionCode);
    snprintf(request.svc_txid, sizeof(request.svc_txid), "%s", TransactionID);
    snprintf(request.svc_msisdn, sizeof(request.svc_msisdn), "%s", MSISDN);
    snprintf(request.svc_bill_cycle, sizeof(request.svc_bill_cycle), "%s", BillCycleNo);

    for (ezxml_t service = ezxml_child(body, "Service"); service; service = service->next) {
        const char* Type = ezxml_child(service, "Type")->txt;
        const char* Soc = ezxml_child(service, "Soc")->txt;
        const char* EffectiveDate = ezxml_child(service, "EffectiveDate")->txt;

        LOG_DEBUG("%s: Service: Type=[%s], Soc=[%s], EffectiveDate=[%s]", __func__,
                Type, Soc, EffectiveDate);

        snprintf(request.svc_type, sizeof(request.svc_type), "%s", Type);
        snprintf(request.svc_soc, sizeof(request.svc_soc), "%s", Soc);
        snprintf(request.svc_eff_date, sizeof(request.svc_eff_date), "%s", EffectiveDate);

        //-- check for required params
        if (! *(request.svc_msisdn)) {
            LOG_ERROR("%s: Invalid request.", __func__);
        } else {
            if (_conn.processMlp(&request) < 0) {
                LOG_ERROR("%s: process_mlp failed msisdn: %s.", __func__, request.svc_msisdn);
            }
        }
    }

    //-- free xml
    ezxml_free(xml);

    //httpResponse->setBody(htmlBody);
    httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
}

/******************************************************************************/
