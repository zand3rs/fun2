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

#include "mxml.h"
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
    request_t request;

    memset(htmlBody, 0, sizeof(htmlBody));

    LOG_INFO("%s: Request URI: %s", __func__, httpRequest->getUri());

    //-- check if client is requesting for the correct service...
    if (strcmp(httpRequest->getService(), "/subscribe") && strcmp(httpRequest->getService(), "/subscribe/")) {
        LOG_ERROR("%s: Service not found: %s", __func__, httpRequest->getService());
        httpResponse->setResponseCode(HTTPRESPONSECODE_404_NOTFOUND);
        return;
    }

    //-- check for valid content-type
    if (httpRequest->getContentType() != HTTPCONTENTTYPE_XML) {
        LOG_ERROR("%s: Content-Type not supported: %s", __func__, httpRequest->getContentTypeString(httpRequest->getContentType()));
        httpResponse->setResponseCode(HTTPRESPONSECODE_400_BADREQUEST);
        return;
    }

    memset(&request, 0, sizeof(request_t));
    request.cluster_node = Config::getClusterNode();

    //-- parse xml
    mxml_node_t *tree = mxmlLoadString(NULL, httpRequest->getBody(), MXML_NO_CALLBACK);
    mxml_node_t *head = mxmlFindElement(tree, tree, "Header", NULL, NULL, MXML_DESCEND);
    mxml_node_t *body = mxmlFindElement(tree, tree, "Body", NULL, NULL, MXML_DESCEND);
    mxml_node_t *services = mxmlFindElement(body, body, "Services", NULL, NULL, MXML_DESCEND);

    mxml_node_t *node;

    node = mxmlFindElement(head, head, "TransactionCode", NULL, NULL, MXML_DESCEND);
    const char* TransactionCode = mxmlGetText(node, NULL);

    node = mxmlFindElement(head, head, "TransactionID", NULL, NULL, MXML_DESCEND);
    const char* TransactionID = mxmlGetText(node, NULL);

    node = mxmlFindElement(head, head, "MSISDN", NULL, NULL, MXML_DESCEND);
    const char* MSISDN = mxmlGetText(node, NULL);

    node = mxmlFindElement(head, head, "BillCycleNo", NULL, NULL, MXML_DESCEND);
    const char* BillCycleNo = mxmlGetText(node, NULL);

    LOG_DEBUG("%s: TransactionCode=[%s], TransactionID=[%s], MSISDN=[%s], BillCycleNo=[%s]", __func__,
            TransactionCode, TransactionID, MSISDN, BillCycleNo);

    snprintf(request.svc_txcode, sizeof(request.svc_txcode), "%s", TransactionCode);
    snprintf(request.svc_txid, sizeof(request.svc_txid), "%s", TransactionID);
    snprintf(request.svc_msisdn, sizeof(request.svc_msisdn), "%s", MSISDN);
    snprintf(request.svc_bill_cycle, sizeof(request.svc_bill_cycle), "%s", BillCycleNo);

    HttpResponseCode_t statusCode = HTTPRESPONSECODE_200_OK;

    for (mxml_node_t *service = mxmlFindElement(services, services, "Service", NULL, NULL, MXML_DESCEND_FIRST);
         service != NULL;
         service = mxmlFindElement(service, services, "Service", NULL, NULL, MXML_NO_DESCEND)) {

        node = mxmlFindElement(service, service, "Type", NULL, NULL, MXML_DESCEND);
        const char* Type = mxmlGetText(node, NULL);

        node = mxmlFindElement(service, service, "Soc", NULL, NULL, MXML_DESCEND);
        const char* Soc = mxmlGetText(node, NULL);

        node = mxmlFindElement(service, service, "EffectiveDate", NULL, NULL, MXML_DESCEND);
        const char* EffectiveDate = mxmlGetText(node, NULL);

        LOG_DEBUG("%s: Service: Type=[%s], Soc=[%s], EffectiveDate=[%s]", __func__,
                Type, Soc, EffectiveDate);

        snprintf(request.svc_type, sizeof(request.svc_type), "%s", Type);
        snprintf(request.svc_soc, sizeof(request.svc_soc), "%s", Soc);
        snprintf(request.svc_eff_date, sizeof(request.svc_eff_date), "%s", EffectiveDate);

        //-- execute
        if (_conn.processMlp(&request) < 0 || request.db_retr != 1) {
            LOG_ERROR("%s: process_mlp failed: retr: %d, msisdn: %s, txcode: %s, txid: %s, "
                    "bill_cycle: %s, type: %s, soc: %s, eff_date: %s" , __func__,
                    request.db_retr, request.svc_msisdn, request.svc_txcode, request.svc_txid,
                    request.svc_bill_cycle, request.svc_type, request.svc_soc, request.svc_eff_date);
            statusCode = HTTPRESPONSECODE_400_BADREQUEST;
            break;
        }
    }

    //-- free xml
    mxmlDelete(tree);

    httpResponse->setResponseCode(statusCode);
    httpResponse->setBody(htmlBody);
}

/******************************************************************************/
