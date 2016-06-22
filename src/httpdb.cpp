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

#include "ezxml.h"
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
    if (!strcmp(httpRequest->getService(), "/subscribe") || !strcmp(httpRequest->getService(), "/subscribe/")) {
        //-- MLP Handler...
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

        httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
        return;
    }

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
        snprintf(request.service_id, sizeof(request.service_id), "%s", sVal);
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
                LOG_ERROR("%s: usurf_deactivation failed msisdn: %s.", __func__, request.msisdn);
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
