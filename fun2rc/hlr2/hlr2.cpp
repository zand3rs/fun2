/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "httpclient.hpp"
#include "pugixml.hpp"
#include "hlr2.hpp"

/*============================================================================*/

void HLR2::_init_steps()
{
    Step_t a_steps[] = {
        {&HLR2::_deactivateCallForwarding1, &HLR2::_noop, "deactivateCallForwarding1", ""},
        {&HLR2::_deactivateCallForwarding2, &HLR2::_noop, "deactivateCallForwarding2", ""},
        {&HLR2::_deactivateCallForwarding3, &HLR2::_noop, "deactivateCallForwarding3", ""},
        {&HLR2::_deactivateCallForwarding4, &HLR2::_noop, "deactivateCallForwarding4", ""},
        {&HLR2::_activateOutgoingCallBarring1, &HLR2::_deactivateOutgoingCallBarring1, "activateOutgoingCallBarring1", "deactivateOutgoingCallBarring1"},
        {&HLR2::_activateOutgoingCallBarring2, &HLR2::_deactivateOutgoingCallBarring2, "activateOutgoingCallBarring2", "deactivateOutgoingCallBarring2"},
        {&HLR2::_activateOutgoingCallBarring3, &HLR2::_deactivateOutgoingCallBarring3, "activateOutgoingCallBarring3", "deactivateOutgoingCallBarring3"},
        {&HLR2::_activateIncomingCallBarring1, &HLR2::_deactivateIncomingCallBarring1, "activateIncomingCallBarring1", "deactivateIncomingCallBarring1"},
        {&HLR2::_activateIncomingCallBarring2, &HLR2::_deactivateIncomingCallBarring2, "activateIncomingCallBarring2", "deactivateIncomingCallBarring2"},
        {&HLR2::_deactivateMultipartyConferenceCall1, &HLR2::_activateMultipartyConferenceCall1, "deactivateMultipartyConferenceCall1", "activateMultipartyConferenceCall1"},
        {&HLR2::_deactivateMultipartyConferenceCall2, &HLR2::_activateMultipartyConferenceCall2, "deactivateMultipartyConferenceCall2", "activateMultipartyConferenceCall2"},
        {&HLR2::_disableRoamingData1, &HLR2::_enableRoamingData1, "disableRoamingData1", "enableRoamingData1"},
        {&HLR2::_disableRoamingData2, &HLR2::_enableRoamingData2, "disableRoamingData2", "enableRoamingData2"},
        {&HLR2::_disableRoamingData3, &HLR2::_noop              , "disableRoamingData3", ""},
        {&HLR2::_enableOutgoingVoice1, &HLR2::_disableOutgoingVoice1, "enableOutgoingVoice1", "disableOutgoingVoice1"},
        {&HLR2::_enableOutgoingVoice2, &HLR2::_disableOutgoingVoice2, "enableOutgoingVoice2", "disableOutgoingVoice2"},
        {&HLR2::_enableOutgoingVoice3, &HLR2::_disableOutgoingVoice3, "enableOutgoingVoice3", "disableOutgoingVoice3"},
        {&HLR2::_disableRingBackTone, &HLR2::_enableRingBackTone, "disableRingBackTone", "enableRingBackTone"},
        {&HLR2::_enableIncomingVoice, &HLR2::_disableIncomingVoice, "enableIncomingVoice", "disableIncomingVoice"},
        {&HLR2::_noop, &HLR2::_enableSmsCsi, "", "enableSmsCsi"}
    };

    Step_t d_steps[] = {
        {&HLR2::_noop, &HLR2::_noop, "", ""},
        {&HLR2::_noop, &HLR2::_noop, "", ""},
        {&HLR2::_noop, &HLR2::_noop, "", ""},
        {&HLR2::_noop, &HLR2::_noop, "", ""},
        {&HLR2::_deactivateOutgoingCallBarring1, &HLR2::_activateOutgoingCallBarring1, "deactivateOutgoingCallBarring1", "activateOutgoingCallBarring1"},
        {&HLR2::_deactivateOutgoingCallBarring2, &HLR2::_activateOutgoingCallBarring2, "deactivateOutgoingCallBarring2", "activateOutgoingCallBarring2"},
        {&HLR2::_deactivateOutgoingCallBarring3, &HLR2::_activateOutgoingCallBarring3, "deactivateOutgoingCallBarring3", "activateOutgoingCallBarring3"},
        {&HLR2::_deactivateIncomingCallBarring1, &HLR2::_activateIncomingCallBarring1, "deactivateIncomingCallBarring1", "activateIncomingCallBarring1"},
        {&HLR2::_deactivateIncomingCallBarring2, &HLR2::_activateIncomingCallBarring2, "deactivateIncomingCallBarring2", "activateIncomingCallBarring2"},
        {&HLR2::_activateMultipartyConferenceCall1, &HLR2::_deactivateMultipartyConferenceCall1, "activateMultipartyConferenceCall1", "deactivateMultipartyConferenceCall1"},
        {&HLR2::_activateMultipartyConferenceCall2, &HLR2::_deactivateMultipartyConferenceCall2, "activateMultipartyConferenceCall2", "deactivateMultipartyConferenceCall2"},
        {&HLR2::_enableRoamingData1, &HLR2::_disableRoamingData1, "enableRoamingData1", "disableRoamingData1"},
        {&HLR2::_enableRoamingData2, &HLR2::_disableRoamingData2, "enableRoamingData2", "disableRoamingData2"},
        {&HLR2::_noop, &HLR2::_noop, "", ""},
        {&HLR2::_disableOutgoingVoice1, &HLR2::_enableOutgoingVoice1, "disableOutgoingVoice1", "enableOutgoingVoice1"},
        {&HLR2::_disableOutgoingVoice2, &HLR2::_enableOutgoingVoice2, "disableOutgoingVoice2", "enableOutgoingVoice2"},
        {&HLR2::_disableOutgoingVoice3, &HLR2::_enableOutgoingVoice3, "disableOutgoingVoice3", "enableOutgoingVoice3"},
        {&HLR2::_enableRingBackTone, &HLR2::_disableRingBackTone, "enableRingBackTone", "disableRingBackTone"},
        {&HLR2::_disableIncomingVoice, &HLR2::_enableIncomingVoice, "disableIncomingVoice", "enableIncomingVoice"},
        {&HLR2::_enableSmsCsi, &HLR2::_noop, "enableSmsCsi", ""}
    };

    _activate_steps.assign(a_steps, a_steps + (sizeof(a_steps)/sizeof(Step_t)));
    _deactivate_steps.assign(d_steps, d_steps + (sizeof(d_steps)/sizeof(Step_t)));
}

/*============================================================================*/
/*============================================================================*/

int HLR2::initialize()
{
    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    _timeout = _param.timeout;
    _url = _param.svc_url;
    _username = _param.svc_user;
    _password = _param.svc_pass;

    _location.clear();
    _msisdn.clear();
    _imsi.clear();

    _init_steps();

    LOG_INFO("%s::%s: url: %s, timeout: %d, username: %s, password: %s", __class__, __func__,
            _url.c_str(), _timeout, _username.c_str(), _password.c_str());

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
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 > _login(msisdn)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }

    //-- get imsi...
    if (0 > _getIMSI()) {
        LOG_ERROR("%s::%s: Unable to get IMSI information!", __class__, __func__);
        _logout();
        return -1;
    }

    for (int i = 0; i < (int)_activate_steps.size(); ++i) {
        if (0 > (this->*_activate_steps[i].execute)()) {
            for (int j = i; j >= 0; --j) {
                if (0 > (this->*_activate_steps[j].rollback)()) {
                    LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_activate_steps[j].rollback_desc);
                }
            }
            LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_activate_steps[i].execute_desc);
            _logout();
            return -1;
        }
    }

    //-- successful
    _logout();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::deactivate(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 > _login(msisdn)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }

    //-- get imsi...
    if (0 > _getIMSI()) {
        LOG_ERROR("%s::%s: Unable to get IMSI information!", __class__, __func__);
        _logout();
        return -1;
    }

    for (int i = 0; i < (int)_deactivate_steps.size(); ++i) {
        if (0 > (this->*_deactivate_steps[i].execute)()) {
            for (int j = i; j >= 0; --j) {
                if (0 > (this->*_deactivate_steps[j].rollback)()) {
                    LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_deactivate_steps[j].rollback_desc);
                }
            }
            LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_deactivate_steps[i].execute_desc);
            _logout();
            return -1;
        }
    }

    //-- successful
    _logout();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (0 > _login(msisdn)) {
        return -1;
    }

    int status = _getIMSI();
    snprintf(imsi, imsi_size, "%s", _imsi.c_str());
    LOG_DEBUG("%s::%s: msisdn: %s, imsi: %s", __class__, __func__, msisdn, imsi);

    _logout();

    return status;
}

/*============================================================================*/
/*============================================================================*/

bool _isResult(pugi::xml_node node)
{
    return !strcasecmp(node.name(), "Result");
}

int HLR2::_exec(const char *url, const char *payload, unsigned short timeout)
{
    std::string headers;
    pugi::xml_document doc;

    return _exec(url, payload, timeout, doc, headers);
}

int HLR2::_exec(const char *url, const char *payload, unsigned short timeout, std::string& headers)
{
    pugi::xml_document doc;

    return _exec(url, payload, timeout, doc, headers);
}

int HLR2::_exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc)
{
    std::string headers;

    return _exec(url, payload, timeout, doc, headers);
}

int HLR2::_exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc, std::string& headers)
{
    const short maxTry = 3;
    short retry = 0;

    if (!url || !*url) {
        LOG_ERROR("%s::%s: Invalid URL!", __class__, __func__);
        return -1;
    }

    while (maxTry >= ++retry) {
        short status = _hc.httpPost(url, payload, "text/xml", timeout);
        LOG_INFO("%s::%s: url: %s, payload: %s, timeout: %d, status: %d, headers: %s, body: %s", __class__, __func__,
                url, payload, timeout, status, _hc.getResponseHeaders(), _hc.getResponseBody());

        if (200 != status && 307 != status) {
            return -1;
        }

        headers = _hc.getResponseHeaders();
        std::string body = _hc.getResponseBody();

        if (!doc.load(body.c_str())) {
            LOG_ERROR("%s::%s: Malformed XML response!: %s", __class__, __func__, body.c_str());
            return -1;
        }

        pugi::xml_node result = doc.find_node(_isResult);
        int resultCode = atoi(result.child("ResultCode").child_value());

        switch (resultCode) {
            case 0:
            case 3016:
            case 3810:
                //-- success, exit...
                return 0;
            case 5004:
                //-- invalid session, retry...
                LOG_INFO("%s::%s: Will retry: %d", __class__, __func__, retry);
                break;
            default:
                LOG_ERROR("%s::%s: ResultCode: %s, ResultDesc: %s", __class__, __func__,
                        result.child("ResultCode").child_value(), result.child("ResultDesc").child_value());
                return -1;
        }

        //-- wait for a while...
        sys_msleep(1000);
    }

    return -1;
}

/*============================================================================*/
/*============================================================================*/

int HLR2::_login(char const* msisdn)
{
    //-- initialize http client...
    _hc.init();

    std::string headers;
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGI>\n"
          "    <OPNAME>" + _username + "</OPNAME>\n"
          "    <PWD>" + _password + "</PWD>\n"
          "  </LGI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    //-- initialize parameters...
    _location.clear();
    _msisdn = msisdn;
    _imsi = "";

    if (0 > _exec(_url.c_str(), req.c_str(), _timeout, headers)) {
        return -1;
    }

    //-- parse headers
    char buf[512];
    snprintf(buf, sizeof(buf), "%s", headers.c_str());

    char* pbuf = strstr(buf, "Location");
    if (pbuf && strchr(pbuf, ':')) {
        char* token;
        token = strtok_r(NULL, ":\r\n", &pbuf);
        token = strtok_r(NULL, "\r\n", &pbuf);
        if (token) {
            char loc[256];
            snprintf(loc, sizeof(loc), "%s", token);
            str_trim(loc);
            _location = loc;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::_logout()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
          "<soapenv:Body>\n"
          "  <LGO></LGO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    int status = _exec(_location.c_str(), req.c_str(), _timeout);

    //-- clear parameters...
    _location.clear();
    _msisdn.clear();
    _imsi.clear();

    //-- deinitialize http client...
    _hc.deinit();

    return status;
}

/*----------------------------------------------------------------------------*/

int HLR2::_getIMSI()
{
    std::string req;

    //-- clear imsi buffer
    _imsi.clear();

    //-- Query IMSI given MSISDN
    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:lst=\"http://www.huawei.com/HLR9820/LST_IMSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <lst:LST_IMSI>\n"
          "     <lst:ISDN>" + _msisdn + "</lst:ISDN>\n"
          "  </lst:LST_IMSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    pugi::xml_document doc;
    if (0 > _exec(_location.c_str(), req.c_str(), _timeout, doc)) {
        return -1;
    }

    pugi::xml_node result = doc
        .child("SOAP-ENV:Envelope")
        .child("SOAP-ENV:Body")
        .child("LST_IMSIResponse")
        .child("Result");

    pugi::xml_node resultData = result.child("ResultData");

    for (pugi::xml_node group = resultData.child("Group"); group; group = group.next_sibling("Group")) {
        if (pugi::xml_node imsi = group.child("IMSI")) {
            _imsi = imsi.child_value();
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR2::_noop()
{
    return 0;
}

/*============================================================================*/
//-- Activation

/*----------------------------------------------------------------------------*/
//-- Deactivation of CallForwarding Services

int HLR2::_deactivateCallForwarding1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFB\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFB>\n"
          "     <era:ISDN>" + _msisdn + "</era:ISDN>\n"
          "  </era:ERA_CFB>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateCallForwarding2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFU\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFU>\n"
          "     <era:ISDN>" + _msisdn + "</era:ISDN>\n"
          "  </era:ERA_CFU>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateCallForwarding3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFNRC\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFNRC>\n"
          "     <era:ISDN>" + _msisdn + "</era:ISDN>\n"
          "  </era:ERA_CFNRC>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateCallForwarding4()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:era=\"http://www.huawei.com/HLR9820/ERA_CFNRY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <era:ERA_CFNRY>\n"
          "     <era:ISDN>" + _msisdn + "</era:ISDN>\n"
          "  </era:ERA_CFNRY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Activate Barring Service for Outgoing Call outside HPLMN

int HLR2::_activateOutgoingCallBarring1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + _imsi + "</act:IMSI>\n"
          "     <act:BSG>TS1X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_activateOutgoingCallBarring2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + _imsi + "</act:IMSI>\n"
          "     <act:BSG>BS2X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_activateOutgoingCallBarring3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BORO>\n"
          "     <act:IMSI>" + _imsi + "</act:IMSI>\n"
          "     <act:BSG>BS3X</act:BSG>\n"
          "  </act:ACT_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Activate Barring Service for Incoming when roaming

int HLR2::_activateIncomingCallBarring1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BICROM>\n"
          "     <act:IMSI>" + _imsi + "</act:IMSI>\n"
          "     <act:BSG>BS2X</act:BSG>\n"
          "  </act:ACT_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_activateIncomingCallBarring2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:act=\"http://www.huawei.com/HLR9820/ACT_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <act:ACT_BICROM>\n"
          "     <act:IMSI>" + _imsi + "</act:IMSI>\n"
          "     <act:BSG>BS3X</act:BSG>\n"
          "  </act:ACT_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Deactivation of Multiparty/Conference Call

int HLR2::_deactivateMultipartyConferenceCall1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_MPTY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_MPTY>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_MPTY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateMultipartyConferenceCall2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_BARPWD\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_BARPWD>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PWD>0000</mod:PWD>\n"
          "  </mod:MOD_BARPWD>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Disable Roaming Data
//-- if encountered ERR3810 mapped to SUCCESS

int HLR2::_disableRoamingData1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NAM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NAM>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:NAM>BOTH</mod:NAM>\n"
          "  </mod:MOD_NAM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_disableRoamingData2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_DIAMRRS\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_DIAMRRS>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:DIAMNODETPL_ID>1</mod:DIAMNODETPL_ID>\n"
          "  </mod:MOD_DIAMRRS>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_disableRoamingData3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>SGSN</mod:TPLTYPE>\n"
          "     <mod:TPLID>5</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}


/*----------------------------------------------------------------------------*/
//-- Enabling Outgoing Voice

int HLR2::_enableOutgoingVoice1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_RSZI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_RSZI>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:RROPTION>ALL_PLMNS</mod:RROPTION>\n"
          "  </mod:MOD_RSZI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_enableOutgoingVoice2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>VLR</mod:TPLTYPE>\n"
          "     <mod:TPLID>4</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_enableOutgoingVoice3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_LCK\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_LCK>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:IC>FALSE</mod:IC>\n"
          "     <mod:OC>FALSE</mod:OC>\n"
          "     <mod:GPRSLOCK>FALSE</mod:GPRSLOCK>\n"
          "     <mod:CSUPLLCK>FALSE</mod:CSUPLLCK>\n"
          "     <mod:PSUPLLCK>FALSE</mod:PSUPLLCK>\n"
          "     <mod:EPSLOCK>FALSE</mod:EPSLOCK>\n"
          "  </mod:MOD_LCK>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Disable Ring Back Tone (RBT) [NRBT=1600]

int HLR2::_disableRingBackTone()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NRBT\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NRBT>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_NRBT>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Enable Incoming Voice

int HLR2::_enableIncomingVoice()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_TCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_TCSI>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLID>110</mod:TPLID>\n"
          "  </mod:MOD_TCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Disabling of SMS CSI to avoid double charging for TM

int HLR2::_disableSmsCsi()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_SMSCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_SMSCSI>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "     <mod:MOMTSWITCH>MOSMSCSI</mod:MOMTSWITCH>\n"
          "  </mod:MOD_SMSCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*============================================================================*/
//-- Deactivation

/*----------------------------------------------------------------------------*/
//-- Deactivate Barring Service for Outgoing Call outside HPLMN

int HLR2::_deactivateOutgoingCallBarring1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + _imsi + "</dea:IMSI>\n"
          "     <dea:BSG>TS1X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateOutgoingCallBarring2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + _imsi + "</dea:IMSI>\n"
          "     <dea:BSG>BS2X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateOutgoingCallBarring3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BORO\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BORO>\n"
          "     <dea:IMSI>" + _imsi + "</dea:IMSI>\n"
          "     <dea:BSG>BS3X</dea:BSG>\n"
          "  </dea:DEA_BORO>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Deactivate Barring Service for Incoming when roaming

int HLR2::_deactivateIncomingCallBarring1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BICROM>\n"
          "     <dea:IMSI>" + _imsi + "</dea:IMSI>\n"
          "     <dea:BSG>BS2X</dea:BSG>\n"
          "  </dea:DEA_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_deactivateIncomingCallBarring2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:dea=\"http://www.huawei.com/HLR9820/DEA_BICROM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <dea:DEA_BICROM>\n"
          "     <dea:IMSI>" + _imsi + "</dea:IMSI>\n"
          "     <dea:BSG>BS3X</dea:BSG>\n"
          "  </dea:DEA_BICROM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Activation of Multiparty/Conference Call

int HLR2::_activateMultipartyConferenceCall1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_MPTY\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_MPTY>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "  </mod:MOD_MPTY>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_activateMultipartyConferenceCall2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_BARPWD\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_BARPWD>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PWD>1234</mod:PWD>\n"
          "  </mod:MOD_BARPWD>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Enable Roaming Data

int HLR2::_enableRoamingData1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NAM\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NAM>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:NAM>BOTH</mod:NAM>\n"
          "  </mod:MOD_NAM>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_enableRoamingData2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_DIAMRRS\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_DIAMRRS>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_DIAMRRS>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Disabling Outgoing Voice

int HLR2::_disableOutgoingVoice1()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_RSZI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_RSZI>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:RROPTION>ONLY_HPLMN</mod:RROPTION>\n"
          "  </mod:MOD_RSZI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_disableOutgoingVoice2()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_VSRR\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_VSRR>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:TPLTYPE>VLR</mod:TPLTYPE>\n"
          "     <mod:TPLID>7</mod:TPLID>\n"
          "  </mod:MOD_VSRR>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

int HLR2::_disableOutgoingVoice3()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_LCK\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_LCK>\n"
          "     <mod:IMSI>" + _imsi + "</mod:IMSI>\n"
          "     <mod:IC>FALSE</mod:IC>\n"
          "     <mod:OC>FALSE</mod:OC>\n"
          "     <mod:GPRSLOCK>FALSE</mod:GPRSLOCK>\n"
          "     <mod:CSUPLLCK>FALSE</mod:CSUPLLCK>\n"
          "     <mod:PSUPLLCK>FALSE</mod:PSUPLLCK>\n"
          "     <mod:EPSLOCK>FALSE</mod:EPSLOCK>\n"
          "  </mod:MOD_LCK>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Enable Ring Back Tone (RBT) [NRBT=1600]

int HLR2::_enableRingBackTone()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_NRBT\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_NRBT>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:NRBT>1600</mod:NRBT>\n"
          "  </mod:MOD_NRBT>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Disable Incoming Voice

int HLR2::_disableIncomingVoice()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_TCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_TCSI>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>FALSE</mod:PROV>\n"
          "  </mod:MOD_TCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/*----------------------------------------------------------------------------*/
//-- Enabling of SMS CSI to avoid double charging for TM
//-- TPLID 15 for testbed 110 for prod

int HLR2::_enableSmsCsi()
{
    std::string req;

    req = "<?xml version=\"1.0\" ?>\n"
          "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
          " xmlns:mod=\"http://www.huawei.com/HLR9820/MOD_SMSCSI\">\n"
          "<soapenv:Header/>\n"
          "<soapenv:Body>\n"
          "  <mod:MOD_SMSCSI>\n"
          "     <mod:ISDN>" + _msisdn + "</mod:ISDN>\n"
          "     <mod:PROV>TRUE</mod:PROV>\n"
          "     <mod:MOMTSWITCH>MOSMSCSI</mod:MOMTSWITCH>\n"
          "     <mod:TPLID>110</mod:TPLID>\n"
          "  </mod:MOD_SMSCSI>\n"
          "</soapenv:Body>\n"
          "</soapenv:Envelope>\n";

    return _exec(_location.c_str(), req.c_str(), _timeout);
}

/******************************************************************************/
