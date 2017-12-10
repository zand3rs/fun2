/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "httpclient.hpp"
#include "pugixml.hpp"
#include "sset.hpp"

/*============================================================================*/

int SSET::initialize()
{
    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    url = _param.svc_url;
    timeout = _param.timeout;

    key = _param.svc_key;
    cert = _param.svc_cert;
    cacert = _param.svc_cacert;

    LOG_INFO("%s::%s: url: %s, timeout: %d", __class__, __func__, url, timeout);

    return 0;
}

/*----------------------------------------------------------------------------*/

int SSET::deinitialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);
    return 0;
}

/*----------------------------------------------------------------------------*/

int SSET::activate(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    std::string req = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<soapenv:Envelope\n"
                      " xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\"\n"
                      " xmlns:sset=\"http://BasicService_SSETLib/SSETService\">\n"
                      "<soapenv:Header/>\n"
                      "<soapenv:Body>\n"
                      "    <sset:provision>\n"
                      "        <request>\n"
                      "            <element>\n"
                      "                <msisdn>" + std::string(msisdn) + "</msisdn>\n"
                      "                <service>VOICE</service>\n"
                      "                <serviceName>ROAMING</serviceName>\n"
                      "                <requestType>2</requestType>\n"
                      "                <shallow>false</shallow>\n"
                      "            </element>\n"
                      "        </request>\n"
                      "    </sset:provision>\n"
                      "</soapenv:Body>\n"
                      "</soapenv:Envelope>\n";

    HttpClient hc(key, cert, cacert);
    int res_code = hc.httpPost(url, req.c_str(), "text/xml", timeout);
    std::string res = hc.getResponseBody();
    int retr = -1;

    pugi::xml_document doc;

    if (200 == res_code) {
        if (doc.load(res.c_str())) {
            pugi::xml_node element = doc
                .child("soapenv:Envelope")
                .child("soapenv:Body")
                .child("se:provisionResponse")
                .child("response")
                .child("element");

            if (element.child("returnCode")) {
                int returnCode = atoi(element.child("returnCode").child_value());
                if (0 == returnCode) {
                    retr = returnCode;
                }
            }
            LOG_INFO("%s::%s: returnCode: %s, description: %s", __class__, __func__,
                    element.child("returnCode").child_value(),
                    element.child("description").child_value());
        } else {
            LOG_ERROR("%s::%s: Malformed XML response!: %s", __class__, __func__,
                    url, res_code, res.c_str());
        }
    } else {
        if (doc.load(res.c_str())) {
            pugi::xml_node fault = doc
                .child("soapenv:Envelope")
                .child("soapenv:Body")
                .child("soapenv:Fault");

            LOG_INFO("%s::%s: faultcode: %s, faultstring: %s", __class__, __func__,
                    fault.child("faultcode").child_value(),
                    fault.child("faultstring").child_value());
        }
    }

    LOG_DEBUG("%s::%s: url: %s, req: %s, res_code: %d, res: %s, retr: %d", __class__, __func__,
            url, req.c_str(), res_code, res.c_str(), retr);

    return retr;
}

/*----------------------------------------------------------------------------*/

int SSET::deactivate(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    std::string req = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<soapenv:Envelope\n"
                      " xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\"\n"
                      " xmlns:sset=\"http://BasicService_SSETLib/SSETService\">\n"
                      "<soapenv:Header/>\n"
                      "<soapenv:Body>\n"
                      "    <sset:provision>\n"
                      "        <request>\n"
                      "            <element>\n"
                      "                <msisdn>" + std::string(msisdn) + "</msisdn>\n"
                      "                <service>VOICE</service>\n"
                      "                <serviceName>ROAMING</serviceName>\n"
                      "                <requestType>3</requestType>\n"
                      "                <shallow>false</shallow>\n"
                      "            </element>\n"
                      "        </request>\n"
                      "    </sset:provision>\n"
                      "</soapenv:Body>\n"
                      "</soapenv:Envelope>\n";

    HttpClient hc(key, cert, cacert);
    int res_code = hc.httpPost(url, req.c_str(), "text/xml", timeout);
    std::string res = hc.getResponseBody();
    int retr = -1;

    pugi::xml_document doc;

    if (200 == res_code) {
        if (doc.load(res.c_str())) {
            pugi::xml_node element = doc
                .child("soapenv:Envelope")
                .child("soapenv:Body")
                .child("se:provisionResponse")
                .child("response")
                .child("element");

            if (element.child("returnCode")) {
                int returnCode = atoi(element.child("returnCode").child_value());
                if (0 == returnCode) {
                    retr = returnCode;
                }
            }
            LOG_INFO("%s::%s: returnCode: %s, description: %s", __class__, __func__,
                    element.child("returnCode").child_value(),
                    element.child("description").child_value());
        } else {
            LOG_ERROR("%s::%s: Malformed XML response!: %s", __class__, __func__,
                    url, res_code, res.c_str());
        }
    } else {
        if (doc.load(res.c_str())) {
            pugi::xml_node fault = doc
                .child("soapenv:Envelope")
                .child("soapenv:Body")
                .child("soapenv:Fault");

            LOG_INFO("%s::%s: faultcode: %s, faultstring: %s", __class__, __func__,
                    fault.child("faultcode").child_value(),
                    fault.child("faultstring").child_value());
        }
    }

    LOG_DEBUG("%s::%s: url: %s, req: %s, res_code: %d, res: %s, retr: %d", __class__, __func__,
            url, req.c_str(), res_code, res.c_str(), retr);

    return retr;
}

/******************************************************************************/
