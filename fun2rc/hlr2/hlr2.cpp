/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "httpclient.hpp"
#include "hlr2.hpp"

/*============================================================================*/

int HLR2::initialize()
{
    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    timeout = _param.timeout;
    snprintf(url, sizeof(url), "%s", _param.svc_url);
    snprintf(username, sizeof(username), "%s", _param.svc_user);
    snprintf(password, sizeof(password), "%s", _param.svc_pass);

    LOG_INFO("%s::%s: url: %s, timeout: %d, username: %s, password: %s", __class__, __func__,
            url, timeout, username, password);

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
    char location[256];
    char buf[512];

    char* token;
    char* pbuf;

    HttpClient hc;
    std::string req;
    int status;

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    //-- login
    memset(location, 0, sizeof(location));
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGI>\n"
          "    <OPNAME>" + std::string(username) + "</OPNAME>\n"
          "    <PWD>" + std::string(password) + "</PWD>\n"
          "  </LGI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(url, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            url, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    if (307 == status) {
        snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());
        pbuf = strstr(buf, "Location");
        if (pbuf) {
            token = strtok_r(NULL, ":\r\n", &pbuf);
            token = strtok_r(NULL, ":\r\n", &pbuf);
            if (token) {
                snprintf(location, sizeof(location), "%s", token);
            }
        }
    }

    if (200 == status) {
    }

    if (!*location) {
        return -1;
    }
    
    //-- Deactivation of CallForwarding Services
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <ERA_CFU>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "  </ERA_CFU>\n"
          "  <ERA_CFB>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "  </ERA_CFB>\n"
          "  <ERA_CFNRC>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "  </ERA_CFNRC>\n"
          "  <ERA_CFNRY>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "  </ERA_CFNRY>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Activate Barring Service for Outgoing Call outside HPLMN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <ACT_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>TS1X</BSG>\n"
          "  </ACT_BORO>\n"
          "  <ACT_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS2X</BSG>\n"
          "  </ACT_BORO>\n"
          "  <ACT_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS3X</BSG>\n"
          "  </ACT_BORO>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Activate Barring Service for Incoming  when roaming
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <ACT_BICROM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS2X</BSG>\n"
          "  </ACT_BICROM>\n"
          "  <ACT_BICROM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS3X</BSG>\n"
          "  </ACT_BICROM>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Deactivation of Multiparty/Conference Call
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_MPTY>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>FALSE</PROV>\n"
          "  </MOD_MPTY>\n"
          "  <MOD_BARPWD>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PWD>0000</PWD>\n"
          "  </MOD_BARPWD>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Disable Roaming Data
    //-- if encountered ERR3810 mapped to SUCCESS
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_NAM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <NAM>BOTH</NAM>\n"
          "  </MOD_NAM>\n"
          "  <MOD_VSRR>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <TPLTYPE>SGSN</TPLTYPE>\n"
          "     <TPLID>5</TPLID>\n"
          "  </MOD_VSRR>\n"
          "  <MOD_DIAMRRS>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <DIAMNODETPL_ID>1</DIAMNODETPL_ID>\n"
          "  </MOD_DIAMRRS>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enabling Outgoing Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_RSZI>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <RROPTION>ALL_PLMNS</RROPTION>\n"
          "  </MOD_RSZI>\n"
          "  <MOD_VSRR>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <TPLTYPE>VLR</TPLTYPE>\n"
          "     <TPLID>4</TPLID>\n"
          "  </MOD_VSRR>\n"
          "  <MOD_LCK>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <IC>FALSE</IC>\n"
          "     <OC>FALSE</OC>\n"
          "     <GPRSLOCK>FALSE</GPRSLOCK>\n"
          "     <CSUPLLCK>FALSE</CSUPLLCK>\n"
          "     <PSUPLLCK>FALSE</PSUPLLCK>\n"
          "     <EPSLOCK>FALSE</EPSLOCK>\n"
          "  </MOD_LCK>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- SSET Remove Ring Back Tone (RBT) [NRBT=1600]
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_NRBT>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>FALSE</PROV>\n"
          "  </MOD_NRBT>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enable Incoming Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_TCSI>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <TPLID>110</TPLID>\n"
          "  </MOD_TCSI>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Suppression of SMS CSI to avoid double charging for TM
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_SMSCSI>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>FALSE</PROV>\n"
          "     <MOMTSWITCH>MOSMSCSI</MOMTSWITCH>\n"
          "  </MOD_SMSCSI>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- logout
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGO></LGO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::deactivate(const char *msisdn)
{
    char location[256];
    char buf[512];

    char* token;
    char* pbuf;

    HttpClient hc;
    std::string req;
    int status;

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    //-- login
    memset(location, 0, sizeof(location));
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGI>\n"
          "    <OPNAME>" + std::string(username) + "</OPNAME>\n"
          "    <PWD>" + std::string(password) + "</PWD>\n"
          "  </LGI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(url, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            url, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    if (307 == status) {
        snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());
        pbuf = strstr(buf, "Location");
        if (pbuf) {
            token = strtok_r(NULL, ":\r\n", &pbuf);
            token = strtok_r(NULL, ":\r\n", &pbuf);
            if (token) {
                snprintf(location, sizeof(location), "%s", token);
            }
        }
    }

    if (200 == status) {
    }

    if (!*location) {
        return -1;
    }

    //-- Barring of Outgoing Call outside HPLMN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <DEA_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>TS1X</BSG>\n"
          "  </DEA_BORO>\n"
          "  <DEA_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS2X</BSG>\n"
          "  </DEA_BORO>\n"
          "  <DEA_BORO>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS3X</BSG>\n"
          "  </DEA_BORO>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Barring of Incoming  when roaming
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <DEA_BICROM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS2X</BSG>\n"
          "  </DEA_BICROM>\n"
          "  <DEA_BICROM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <BSG>BS3X</BSG>\n"
          "  </DEA_BICROM>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Multiparty
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_BARPWD>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISN>\n"
          "     <PWD>1234</PWD>\n"
          "  </MOD_BARPWD>\n"
          "  <MOD_MPTY>\n"
          "     <ISDN>515xxxxxxxxxx</ISDN>\n"
          "     <PROV>TRUE</PROV>\n"
          "  </MOD_MPTY>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Data
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_NAM>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <NAM>BOTH</NAM>\n"
          "  </MOD_NAM>\n"
          "  <MOD_DIAMRRS>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <PROV>FALSE</PROV>\n"
          "  </MOD_DIAMRRS>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Outgoing Voice
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_RSZI>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <RROPTION>ONLY_HPLMN</RROPTION>\n"
          "  </MOD_RSZI>\n"
          "  <MOD_VSRR>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <TPLTYPE>VLR</TPLTYPE>\n"
          "     <TPLID>7</TPLID>\n"
          "  </MOD_VSRR>\n"
          "  <MOD_LCK>\n"
          "     <IMSI>515xxxxxxxxxx</IMSI>\n"
          "     <IC>FALSE</IC>\n"
          "     <OC>FALSE</OC>\n"
          "     <GPRSLOCK>FALSE</GPRSLOCK>\n"
          "     <CSUPLLCK>FALSE</CSUPLLCK>\n"
          "     <PSUPLLCK>FALSE</PSUPLLCK>\n"
          "     <EPSLOCK>FALSE</EPSLOCK>\n"
          "  </MOD_LCK>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Remove Incoming Voice Trigger for charging
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_TCSI>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>FALSE</PROV>\n"
          "  </MOD_TCSI>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Automatic activation of RBT (if applicable) [NRBT=1600]
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_NRBT>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <NRBT>1600</NRBT>\n"
          "  </MOD_NRBT>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- Enabling SMS CSI to avoid double charging for TM
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <MOD_SMSCSI>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "     <PROV>TRUE</PROV>\n"
          "     <MOMTSWITCH>MOSMSCSI</MOMTSWITCH>\n"
          "     <TPLID>110</TPLID>\n"
          "  </MOD_SMSCSI>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- logout
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGO></LGO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    char location[256];
    char buf[512];

    char* token;
    char* pbuf;

    HttpClient hc;
    std::string req;
    int status;

    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    //-- login
    memset(location, 0, sizeof(location));
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGI>\n"
          "    <OPNAME>" + std::string(username) + "</OPNAME>\n"
          "    <PWD>" + std::string(password) + "</PWD>\n"
          "  </LGI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(url, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            url, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    if (307 == status) {
        snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());
        pbuf = strstr(buf, "Location");
        if (pbuf) {
            token = strtok_r(NULL, ":\r\n", &pbuf);
            token = strtok_r(NULL, ":\r\n", &pbuf);
            if (token) {
                snprintf(location, sizeof(location), "%s", token);
            }
        }
    }

    if (200 == status) {
    }

    if (!*location) {
        return -1;
    }

    //-- Query IMSI given MSISDN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<SOAP-ENV:Body>\n"
          "  <LST_NSUB>\n"
          "     <ISDN>" + std::string(msisdn) +"</ISDN>\n"
          "  </LST_NSUB>\n"
          "</SOAP-ENV:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    //-- logout
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGO></LGO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";
    status = hc.httpPost(location, req.c_str(), "text/xml", timeout);
    LOG_INFO("%s::%s: url: %s, timeout: %d, payload: %s, status: %d, headers: %s, body: %s", __class__, __func__,
            location, timeout, req.c_str(), status, hc.getResponseHeaders(), hc.getResponseBody());

    return 0;
}

/******************************************************************************/
