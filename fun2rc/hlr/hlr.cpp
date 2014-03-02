/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hlr.hpp"

/*============================================================================*/

void HLR::_init_steps()
{
    Step_t a_steps[] = {
        {&HLR::_callForwardingUnconditionalErased, NULL, "callForwardingUnconditionalErased", ""},
        {&HLR::_callForwardingBusyErased, NULL, "callForwardingBusyErased", ""},
        {&HLR::_callForwardingNotReachableErased, NULL, "callForwardingNotReachableErased", ""},
        {&HLR::_callForwardingNoReplyErased, NULL, "callForwardingNoReplyErased", ""},
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11"},
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16"},
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F"},
        {&HLR::_defineSuplementaryServicesDeactivated, &HLR::_defineSuplementaryServicesActivated, "defineSuplementaryServicesDeactivated", "defineSuplementaryServicesActivated"},
        {&HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16, &HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16, "setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16", "setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16"},
        {&HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F, &HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F, "setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F", "setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F"},
        {&HLR::_setActivateDisableGPRS, &HLR::_setDeactivateEnableGPRS, "setActivateDisableGPRS", "setDeactivateEnableGPRS"},
        {&HLR::_setActivateSAM, &HLR::_setDeactivateSAM, "setActivateSAM", "setDeactivateSAM"}
    };

    Step_t d_steps[] = {
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11"},
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16"},
        {&HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F, &HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F, "setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F", "setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F"},
        {&HLR::_defineSuplementaryServicesActivated, &HLR::_defineSuplementaryServicesDeactivated, "defineSuplementaryServicesActivated", "defineSuplementaryServicesDeactivated"},
        {&HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16, &HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16, "setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16", "setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16"},
        {&HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F, &HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F, "setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F", "setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F"},
        {&HLR::_setDeactivateEnableGPRS, &HLR::_setActivateDisableGPRS, "setDeactivateEnableGPRS", "setActivateDisableGPRS"},
        {&HLR::_setDeactivateSAM, &HLR::_setActivateSAM, "setDeactivateSAM", "setActivateSAM"}
    };

    _activate_steps.assign(a_steps, a_steps + (sizeof(a_steps)/sizeof(Step_t)));
    _deactivate_steps.assign(d_steps, d_steps + (sizeof(d_steps)/sizeof(Step_t)));
}

/*============================================================================*/
/*============================================================================*/

bool HLR::_sockConnect()
{
    _sock = skt_open((char*) _host.c_str(), _port);
    if (! _sock || skt_connect(_sock)) {
        LOG_ERROR("%s::%s: Connection failed: host: %s, port: %d", __class__, __func__, _host.c_str(), _port);
        _sockDisconnect();
        return false;
    }

    return true;
}

/*----------------------------------------------------------------------------*/

bool HLR::_sockDisconnect()
{
    if (_sock) {
        skt_close(_sock);
        _sock = NULL;
    }

    return true;
}

/*----------------------------------------------------------------------------*/

bool HLR::_sockReceive()
{
    if (! _sock) {
        LOG_ERROR("%s::%s: No existing connection to server!", __class__, __func__);
        return false;
    }

    int status = skt_recv(_sock, _buffer, _buffer_size, _mtimeout);
    if (0 >= status) {
        return false;
    }

    //-- get remaining packets...
    short retry = _max_sock_retry;
    while (true) {
        size_t buffer_len = strlen(_buffer);
        if (buffer_len >= _buffer_size) {
            LOG_WARNING("%s::%s: Buffer size (%u) is insufficient!", __class__, __func__, _buffer_size);
            return false;
        } else {
            status = skt_recv(_sock, &_buffer[buffer_len], _buffer_size-buffer_len, 100);
            retry = (0 >= status) ? retry-1 : _max_sock_retry;
        }
        if (0 >= status && 0 >= retry) {
            break;
        }
    }

    return true;
}

/*----------------------------------------------------------------------------*/

bool HLR::_sockSend()
{
    if (! _sock) {
        LOG_ERROR("%s::%s: No existing connection to server!", __class__, __func__);
        return false;
    }

    return (0 >= skt_send(_sock, _buffer, strlen(_buffer), _mtimeout)) ? false : true;
}

/*============================================================================*/
/*============================================================================*/

int HLR::_login(char const* msisdn)
{
    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- logout last session...
    _logout();

    //-- initialize parameters...
    _msisdn = msisdn;
    _imsi = "";

    _username = "";
    _password = "";
    _ir_sset = "";
    _normal_sset = "";
    _host = "";

    _port = 0;
    _mtimeout = 60000;

    //-- get login params-------------------------------------------------------
    std::string q = "select s.SERVER_IP, s.SERVER_PORT, s.SERVER_NAME, s.USERNAME, s.PASSWORD,"
        " s.TIMEOUT, s.RETRY_CNT, m.MSISDN_FR, m.MSISDN_TO, c.IRSSET, c.NORMALSSET"
        " from HLR_SERVERS s, HLR_SERVER_MAPPING m, SSET_MAPPING c"
        " where s.SERVER_NAME = m.SERVER_NAME and (m.MSISDN_FR<='";
    q.append(_msisdn);
    q.append("' and m.MSISDN_TO>='");
    q.append(_msisdn);
    q.append("') and (c.MSISDN_FR<='");
    q.append(_msisdn);
    q.append("' and c.MSISDN_TO>='");
    q.append(_msisdn);
    q.append("')");

    LOG_DEBUG("%s::%s: sql = [%s]", __class__, __func__, q.c_str());

    if (0 > _conn.query(&_res, q.c_str())) {
        LOG_ERROR("%s::%s: Failed to execute sql = [%s]", __class__, __func__, q.c_str());
        return -1;
    }

    const char** row;
    if ((row = _conn.fetchRow(_res)) != NULL) {
        if (row[0] && *(row[0])) {
            _host = (char*) row[0];
        }
        if (row[1] && *(row[1])) {
            _port = atoi((char *)row[1]);
        }
        if (row[3] && *(row[3])) {
            _username = (char*) row[3];
        }
        if (row[4] && *(row[4])) {
            _password = (char*) row[4];
        } 
        if (row[5] && *(row[5])) {
            _mtimeout = atoi((char*) row[5]) * 1000;
        }
        if (row[9] && *(row[9])) {
            _ir_sset = (char*) row[9];
        }
        if (row[10] && *(row[10])) {
            _normal_sset = (char*) row[10];
        }

        _conn.freeResult(_res);
    } else {
        LOG_ERROR("%s::%s: sql = [%s]: returned NULL", __class__, __func__, q.c_str());
        return -1;
    }

    LOG_DEBUG("%s::%s: Required parameters: host: %s, port: %d, user: %s, pass: %s", __class__, __func__,
            _host.c_str(), _port, _username.c_str(), _password.c_str());
    //-- get login params-------------------------------------------------------

    if (! _sockConnect()) {
        LOG_ERROR("%s::%s: Unable to connect to server!", __class__, __func__);
        return -1;
    }

    //-- get user prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "ENTER USERNAME <")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"ENTER USERNAME <\" from response!", __class__, __func__);
        _sockDisconnect();
        return -1;
    }

    //-- send username
    snprintf(_buffer, _buffer_size, "%s\r\n", _username.c_str());
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send username to server!", __class__, __func__);
        _sockDisconnect();
        return -1;
    }

    //-- get pass prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "ENTER PASSWORD <")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"ENTER PASSWORD <\" from response!", __class__, __func__);
        _sockDisconnect();
        return -1;
    }

    //-- send password
    snprintf(_buffer, _buffer_size, "%s\r\n", _password.c_str());
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send password to server!", __class__, __func__);
        _sockDisconnect();
        return -1;
    }

    //-- get main prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "MAIN LEVEL COMMAND <___>")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"MAIN LEVEL COMMAND <___>\" from response!", __class__, __func__);
        _sockDisconnect();
        return -1;
    }

    //-- successful
    LOG_INFO("%s::%s: Successful: host: %s, port: %d, user: %s, pass: %s", __class__, __func__,
            _host.c_str(), _port, _username.c_str(), _password.c_str());

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_logout()
{
    _sockDisconnect();

    //-- clear parameters...
    _msisdn = "";
    _imsi = "";

    _username = "";
    _password = "";
    _ir_sset = "";
    _normal_sset = "";
    _host = "";

    _port = 0;
    _mtimeout = 60000;

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingUnconditionalErased()
{
    //ZMSS:MSISDN=<MSISDN>:CFU=E;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFU=E;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingBusyErased()
{
    //ZMSS:MSISDN=<MSISDN>:CFB=E;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFB=E;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingNotReachableErased()
{
    //ZMSS:MSISDN=<MSISDN>:CFNR=E;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFNR=E;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingNoReplyErased()
{
    //ZMSS:MSISDN=<MSISDN>:CFNA=E;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFNA=E;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingUnconditionalDeactivated()
{
    //ZMSS:MSISDN=<MSISDN>:CFU=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFU=D;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingBusyDeactivated()
{
    //ZMSS:MSISDN=<MSISDN>:CFB=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFB=D;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingNotReachableDeactivated()
{
    //ZMSS:MSISDN=<MSISDN>:CFNR=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFNR=D;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_callForwardingNoReplyDeactivated()
{
    //ZMSS:MSISDN=<MSISDN>:CFNA=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:MSISDN=%s:CFNA=D;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_defineSuplementaryServicesDeactivated()
{
    //ZMSD:MSISDN=<MSISDN>:MPTY=N,PSW=N;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSD:MSISDN=%s:MPTY=N,PSW=N;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_defineSuplementaryServicesActivated()
{
    //ZMSD:MSISDN=<MSISDN>:MPTY=Y,PSW=1234;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSD:MSISDN=%s:MPTY=Y,PSW=1234;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11()
{
    //ZMSS:IMSI=<IMSI>,BSERV=T11:BORO=D:;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=T11:BORO=D;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B16:BORO=D:;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B16:BORO=D;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B1F:BORO=D:;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B1F:BORO=D;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}


/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11()
{
    //ZMSS:IMSI=<IMSI>,BSERV=T11:BORO=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=T11:BORO=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B16:BORO=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B16:BORO=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B1F:BORO=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B1F:BORO=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B16:BIRO=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B16:BIRO=D;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B1F:BIRO=D;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B1F:BIRO=D;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B1F:BIRO=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B1F:BIRO=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16()
{
    //ZMSS:IMSI=<IMSI>,BSERV=B16:BIRO=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMSS:IMSI=%s,BSERV=B16:BIRO=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setActivateSAM()
{
    //ZMIM:IMSI=<IMSI>:SAM=ALL,RP=4,ACT=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMIM:IMSI=%s:SAM=ALL,RP=4,ACT=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setDeactivateSAM()
{
    //ZMIM:IMSI=<IMSI>:SAM=NAT,RP=6,ACT=A;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMIM:IMSI=%s:SAM=NAT,RP=6,ACT=A;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setActivateDisableGPRS()
{
    //ZMNM:IMSI=<IMSI>:NWACC=NONGPRS,GRP=5;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMNM:IMSI=%s:NWACC=NONGPRS,GRP=5;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setDeactivateEnableGPRS()
{
    //ZMNM:IMSI=<IMSI>:NWACC=BOTH:;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMNM:IMSI=%s:NWACC=BOTH;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_setServiceIndex(char const* sset)
{
    //ZMQG:MSISDN=<MSISDN>:SSET=<SSET>;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMQG:MSISDN=%s:SSET=%s;\r\n", _msisdn.c_str(), sset);
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::_getRealIMSI()
{
    //ZMIO:MSISDN=<MSISDN>;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return -1;
    }

    //-- set buffer...
    snprintf(_buffer, _buffer_size, "ZMIO:MSISDN=%s;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return -1;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED") && ! strstr(_buffer, "COMMAND PARTLY SUCCESSFUL") && ! strstr(_buffer, "SERVICE EXISTS")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return -1;
    }

    //-- parse response
    char imsi[32] = "";
    char searchKey[256] = "INTERNATIONAL MOBILE SUBSCRIBER IDENTITY ... ";
    char* p = strstr(_buffer, searchKey);
    if (p) {
        p += strlen(searchKey);
        snprintf(imsi, sizeof(imsi), "%.*s", 15, p);
    }

    //-- set imsi
    _imsi = imsi;

    LOG_DEBUG("%s::%s: imsi: %s", __class__, __func__, _imsi.c_str());

    return 0;
}

/*============================================================================*/
/*============================================================================*/

int HLR::initialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);

    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    if (OraDB::init_lib(true) < 0) {
        LOG_CRITICAL("%s::%s: Unable to initialize OCI!!!", __class__, __func__);
        return -1;
    }

    _ora_auth = _param.db_user;
    _ora_auth.append("/");
    _ora_auth.append(_param.db_pass);
    _ora_auth.append("@");
    _ora_auth.append(_param.db_host);

    if (_conn.initialize(_ora_auth.c_str()) < 0) {
        LOG_ERROR("%s::%s: Unable to connect to oracle: ora_auth: %s", __class__, __func__, _ora_auth.c_str());
        return -1;
    }
    _res = OraDBSimple::STH_INIT;

    _buffer = (char*) malloc(_buffer_size);
    if (! _buffer) {
        LOG_CRITICAL("%s::%s: Unable to allocate memory for buffer!", __class__, __func__);
        return -1;
    }
    memset(_buffer, 0, _buffer_size);

    _init_steps();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::deinitialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);

    if (_buffer) {
        free(_buffer);
        _buffer = NULL;
    }
    _conn.destroy_db();

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::activate(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s: start", __class__, __func__, msisdn);

    LOG_DEBUG("%s::%s: msisdn: %s: login start", __class__, __func__, msisdn);
    if (0 > _login(msisdn)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }
    LOG_DEBUG("%s::%s: msisdn: %s: login done", __class__, __func__, msisdn);

    if (0 > _getRealIMSI()) {
        LOG_ERROR("%s::%s: Unable to get IMSI information!", __class__, __func__);
        _logout();
        return -1;
    }

    for (int i = 0; i < (int)_activate_steps.size(); ++i) {
        if (this->_activate_steps[i].execute) {
            if (0 > (this->*_activate_steps[i].execute)()) {
                for (int j = i; j >= 0; --j) {
                    if (this->_activate_steps[j].rollback) {
                        if (0 > (this->*_activate_steps[j].rollback)()) {
                            LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_activate_steps[j].rollback_desc);
                        }
                    }
                }
                LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_activate_steps[i].execute_desc);
                _logout();
                return -1;
            }
        }
    }

    //-- successful
    _logout();

    LOG_INFO("%s::%s: msisdn: %s: done", __class__, __func__, msisdn);

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::deactivate(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s: start", __class__, __func__, msisdn);

    LOG_DEBUG("%s::%s: msisdn: %s: login start", __class__, __func__, msisdn);
    if (0 > _login(msisdn)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }
    LOG_DEBUG("%s::%s: msisdn: %s: login done", __class__, __func__, msisdn);

    if (0 > _getRealIMSI()) {
        LOG_ERROR("%s::%s: Unable to get IMSI information!", __class__, __func__);
        _logout();
        return -1;
    }

    for (int i = 0; i < (int)_deactivate_steps.size(); ++i) {
        if (this->_deactivate_steps[i].execute) {
            if (0 > (this->*_deactivate_steps[i].execute)()) {
                for (int j = i; j >= 0; --j) {
                    if (this->_deactivate_steps[j].rollback) {
                        if (0 > (this->*_deactivate_steps[j].rollback)()) {
                            LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_deactivate_steps[j].rollback_desc);
                        }
                    }
                }
                LOG_ERROR("%s::%s: Failed: %s", __class__, __func__, this->_deactivate_steps[i].execute_desc);
                _logout();
                return -1;
            }
        }
    }

    //-- successful
    _logout();

    LOG_INFO("%s::%s: msisdn: %s: done", __class__, __func__, msisdn);

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (! imsi) {
        return -1;
    }

    snprintf(imsi, imsi_size, "%s", _imsi.c_str());
    LOG_DEBUG("%s::%s: msisdn: %s, imsi: %s", __class__, __func__, msisdn, imsi);

    return 0;
}

/******************************************************************************/
