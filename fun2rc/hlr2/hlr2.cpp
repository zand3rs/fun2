/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "httpclient.hpp"
#include "pugixml.hpp"
#include "hlr2.hpp"

/*============================================================================*/

int HLR2::initialize()
{
    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    timeout = _param.timeout;
    url = _param.svc_url;
    username = _param.svc_user;
    password = _param.svc_pass;
    location.clear();

    LOG_INFO("%s::%s: url: %s, timeout: %d, username: %s, password: %s", __class__, __func__,
            url.c_str(), timeout, username.c_str(), password.c_str());

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::deinitialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);
    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::activate(const char *msisdn)
{
    HttpClient hc;
    std::string req;
    int status;
    char imsi[32];

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 != _login()) {
        return -1;
    }

    if (0 != _getIMSI(msisdn, imsi, sizeof(imsi))) {
        return -1;
    }

    //-- Deactivation of CallForwarding Services
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFB\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFB>\n"
          "     <era:ISDN>" + std::string(msisdn) + "</era:ISDN>\n"
          "  </era:ERA_CFB>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFU\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFU>\n"
          "     <era:ISDN>" + std::string(msisdn) + "</era:ISDN>\n"
          "  </era:ERA_CFU>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFNRC\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFNRC>\n"
          "     <era:ISDN>" + std::string(msisdn) + "</era:ISDN>\n"
          "  </era:ERA_CFNRC>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFNRY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFNRY>\n"
          "     <era:ISDN>" + std::string(msisdn) + "</era:ISDN>\n"
          "  </era:ERA_CFNRY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Activate Barring Service for Outgoing Call outside HPLMN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + std::string(imsi) + "</act:IMSI>\n"
          "     <act:BSG>TS1X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + std::string(imsi) + "</act:IMSI>\n"
          "     <act:BSG>BS2X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + std::string(imsi) + "</act:IMSI>\n"
          "     <act:BSG>BS3X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());


    //-- Activate Barring Service for Incoming  when roaming
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BICROM>\n"
          "     <act:IMSI>" + std::string(imsi) + "</act:IMSI>\n"
          "     <act:BSG>BS2X</act:BSG>\n"
          "  </act:ACT_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BICROM>\n"
          "     <act:IMSI>" + std::string(imsi) + "</act:IMSI>\n"
          "     <act:BSG>BS3X</act:BSG>\n"
          "  </act:ACT_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());


    //-- Deactivation of Multiparty/Conference Call
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_MPTY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_MPTY>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_MPTY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_BARPWD\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_BARPWD>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PWD>FALSE</mod:PWD>\n"
          "  </mod:MOD_BARPWD>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());


    //-- Disable Roaming Data
    //-- if encountered ERR3810 mapped to SUCCESS
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NAM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NAM>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:NAM>BOTH</mod:NAM>\n"
          "  </mod:MOD_NAM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>SGSN</mod:TPLTYPE>\n"
          "     <mod:TPLID>5</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_DIAMRRS\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_DIAMRRS>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:DIAMNODETPL_ID>1</mod:DIAMNODETPL_ID>\n"
          "  </mod:MOD_DIAMRRS>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enabling Outgoing Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_RSZI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_RSZI>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:RROPTION>ALL_PLMNS</mod:RROPTION>\n"
          "  </mod:MOD_RSZI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>VLR</mod:TPLTYPE>\n"
          "     <mod:TPLID>4</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_LCK\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_LCK>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:IC>FALSE</mod:IC>\n"
          "     <mod:OC>FALSE</mod:OC>\n"
          "     <mod:GPRSLOCK>FALSE</mod:GPRSLOCK>\n"
          "     <mod:CSUPLLCK>FALSE</mod:CSUPLLCK>\n"
          "     <mod:PSUPLLCK>FALSE</mod:PSUPLLCK>\n"
          "     <mod:EPSLOCK>FALSE</mod:EPSLOCK>\n"
          "  </mod:MOD_LCK>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- SSET Remove Ring Back Tone (RBT) [NRBT=1600]
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NRBT\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NRBT>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_NRBT>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enable Incoming Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_TCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_TCSI>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLID>110</mod:TPLID>\n"
          "  </mod:MOD_TCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Suppression of SMS CSI to avoid double charging for TM
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_SMSCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_SMSCSI>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:MOMTSWITCH>MOSMSCSI</mod:MOMTSWITCH>\n"
          "  </mod:MOD_SMSCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    _logout();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::deactivate(const char *msisdn)
{
    HttpClient hc;
    std::string req;
    int status;
    char imsi[32];

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 != _login()) {
        return -1;
    }

    if (0 != _getIMSI(msisdn, imsi, sizeof(imsi))) {
        return -1;
    }

    //-- Barring of Outgoing Call outside HPLMN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + std::string(imsi) + "</dea:IMSI>\n"
          "     <dea:BSG>TS1X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + std::string(imsi) + "</dea:IMSI>\n"
          "     <dea:BSG>BS2X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + std::string(imsi) + "</dea:IMSI>\n"
          "     <dea:BSG>BS3X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Barring of Incoming  when roaming
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BICROM>\n"
          "     <dea:IMSI>" + std::string(imsi) + "</dea:IMSI>\n"
          "     <dea:BSG>BS2X</dea:BSG>\n"
          "  </dea:DEA_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BICROM>\n"
          "     <dea:IMSI>" + std::string(imsi) + "</dea:IMSI>\n"
          "     <dea:BSG>BS3X</dea:BSG>\n"
          "  </dea:DEA_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());


    //-- Multiparty
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_BARPWD\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_BARPWD>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PWD>1234</mod:PWD>\n"
          "  </mod:MOD_BARPWD>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_MPTY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_MPTY>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "  </mod:MOD_MPTY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Data
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NAM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NAM>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:NAM>BOTH</mod:NAM>\n"
          "  </mod:MOD_NAM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_DIAMRRS\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_DIAMRRS>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_DIAMRRS>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Outgoing Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_RSZI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_RSZI>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:RROPTION>ONLY_HPLMN</mod:RROPTION>\n"
          "  </mod:MOD_RSZI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>VLR</mod:TPLTYPE>\n"
          "     <mod:TPLID>7</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_LCK\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_LCK>\n"
          "     <mod:IMSI>" + std::string(imsi) + "</mod:IMSI>\n"
          "     <mod:IC>FALSE</mod:IC>\n"
          "     <mod:OC>FALSE</mod:OC>\n"
          "     <mod:GPRSLOCK>FALSE</mod:GPRSLOCK>\n"
          "     <mod:CSUPLLCK>FALSE</mod:CSUPLLCK>\n"
          "     <mod:PSUPLLCK>FALSE</mod:PSUPLLCK>\n"
          "     <mod:EPSLOCK>FALSE</mod:EPSLOCK>\n"
          "  </mod:MOD_LCK>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Remove Incoming Voice Trigger for charging
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_TCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_TCSI>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_TCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Automatic activation of RBT (if applicable) [NRBT=1600]
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NRBT\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NRBT>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:NRBT>1600</mod:NRBT>\n"
          "  </mod:MOD_NRBT>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enabling SMS CSI to avoid double charging for TM
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_SMSCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_SMSCSI>\n"
          "     <mod:ISDN>" + std::string(msisdn) + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:MOMTSWITCH>MOSMSCSI</mod:MOMTSWITCH>\n"
          "     <mod:TPLID>110</mod:TPLID>\n"
          "  </mod:MOD_SMSCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    _logout();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 != _login()) {
        return -1;
    }

    int status = _getIMSI(msisdn, imsi, imsi_size);

    _logout();

    return status;
}

/*============================================================================*/

int HLR2::_login()
{
    HttpClient hc;
    std::string req;

    LOG_INFO("%s::%s: url: %s, timeout: %d, username: %s, password: %s", __class__, __func__,
            url.c_str(), timeout, username.c_str(), password.c_str());

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGI>\n"
          "    <OPNAME>" + username + "</OPNAME>\n"
          "    <PWD>" + password + "</PWD>\n"
          "  </LGI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    int status = hc.httpPost(url.c_str(), req.c_str(), "text/xml", timeout);

    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            url.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- initialize location...
    location.clear();

    if (307 != status) {
        return -1;
    }

    //-- parse payload
    pugi::xml_document doc;
    std::string res = hc.getResponseBody();

    if (!doc.load(res.c_str())) {
        LOG_ERROR("%s::%s: Malformed XML response!: %s", __class__, __func__, res.c_str());
        return -1;
    }

    pugi::xml_node result = doc
        .child("SOAP-ENV:Envelope")
        .child("SOAP-ENV:Body")
        .child("LGIResponse")
        .child("Result");

    int resultCode = atoi(result.child("ResultCode").child_value());
    if (0 != resultCode) {
        LOG_ERROR("%s::%s: ResultCode: %s, ResultDesc: %s", __class__, __func__,
                result.child("ResultCode").child_value(), result.child("ResultDesc").child_value());
        return -1;
    }

    //-- parse headers
    char buf[512];
    snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());

    char* pbuf = strstr(buf, "Location");
    if (pbuf && strchr(pbuf, ':')) {
        char* token;
        token = strtok_r(NULL, ":\r\n", &pbuf);
        token = strtok_r(NULL, ":\r\n", &pbuf);
        if (token) {
            location = token;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::_logout()
{
    HttpClient hc;
    std::string req;

    LOG_INFO("%s::%s: url: %s, timeout: %d", __class__, __func__,
            location.c_str(), timeout);

    if (location.empty()) {
        return -1;
    }

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGO></LGO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    int status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);

    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    if (200 == status) {
        location.clear();
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::_getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    HttpClient hc;
    std::string req;
    int status;

    if (location.empty()) {
        return -1;
    }

    //-- Query IMSI given MSISDN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:lst=\"http://www.huawei.com/HLR9820/LST_IMSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <lst:LST_IMSI>\n"
          "     <lst:ISDN>" + std::string(msisdn) + "</lst:ISDN>\n"
          "  </lst:LST_IMSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location.c_str(), req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location.c_str(), timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    if (200 != status) {
        return -1;
    }

    pugi::xml_document doc;
    std::string res = hc.getResponseBody();

    if (!doc.load(res.c_str())) {
        LOG_ERROR("%s::%s: Malformed XML response!: %s", __class__, __func__, res.c_str());
        return -1;
    }

    pugi::xml_node result = doc
        .child("SOAP-ENV:Envelope")
        .child("SOAP-ENV:Body")
        .child("LST_IMSIResponse")
        .child("Result");

    int resultCode = atoi(result.child("ResultCode").child_value());
    if (0 != resultCode) {
        LOG_ERROR("%s::%s: ResultCode: %s, ResultDesc: %s", __class__, __func__,
                result.child("ResultCode").child_value(), result.child("ResultDesc").child_value());
        return -1;
    }

    pugi::xml_node resultData = result.child("ResultData");

    for (pugi::xml_node group = resultData.child("Group"); group; group = group.next_sibling("Group")) {
        if (pugi::xml_node _imsi = group.child("IMSI")) {
            snprintf(imsi, imsi_size, "%s", _imsi.child_value());
        }
    }

    return 0;
}

/******************************************************************************/
