/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hlr.hpp"

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

bool HLR::_initialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);

    if (OraDB::init_lib(true) < 0) {
        LOG_CRITICAL("%s::%s: Unable to initialize OCI!!!", __class__, __func__);
        return false;
    }
    if (_conn.initialize(_ora_auth.c_str()) < 0) {
        LOG_ERROR("%s::%s: Unable to connect to oracle: ora_auth: %s", __class__, __func__, _ora_auth.c_str());
        return false;
    }
    _res = OraDBSimple::STH_INIT;

    _buffer = (char*) malloc(_buffer_size);
    if (! _buffer) {
        LOG_CRITICAL("%s::%s: Unable to allocate memory for buffer!", __class__, __func__);
        return false;
    }
    memset(_buffer, 0, _buffer_size);

    return true;
}

/*----------------------------------------------------------------------------*/

void HLR::_deinitialize()
{
    LOG_INFO("%s::%s: ...", __class__, __func__);

    _sockDisconnect();
    if (_buffer) {
        free(_buffer);
        _buffer = NULL;
    }
    _conn.destroy_db();
}

/*----------------------------------------------------------------------------*/

bool HLR::_getLoginParams()
{
    bool retr = false;

    LOG_INFO("%s::%s: ...", __class__, __func__);

    //-- initialize parameters...
    _username = "";
    _password = "";
    _host = "";
    _port = 0;
    _mtimeout = 60000;

    std::string q = "select s.SERVER_IP, s.SERVER_PORT, s.SERVER_NAME, s.USERNAME, s.PASSWORD, s.TIMEOUT, s.RETRY_CNT";
    if (_imsi.empty()) {
        //-- get imsi by msisdn (default)
        q.append(" from HLR_SERVERS s, HLR_SERVER_MAPPING m, SSET_MAPPING c");
        q.append(" where s.SERVER_NAME = m.SERVER_NAME and (m.MSISDN_FR<='");
        q.append(_msisdn);
        q.append("' and m.MSISDN_TO>='");
        q.append(_msisdn);
        q.append("') and (c.MSISDN_FR<='");
        q.append(_msisdn);
        q.append("' and c.MSISDN_TO>='");
        q.append(_msisdn);
        q.append("')");
    } else {
        //-- get msisdn by imsi
        q.append(" from HLR_SERVERS s, HLR_IMSI_MAPPING m");
        q.append(" where s.SERVER_IP = m.SERVER_IP");
        q.append(" and (m.IMSI = substr('");
        q.append(_imsi);
        q.append("', 1, 7))");
    }

    LOG_DEBUG("%s::%s: sql = [%s]", __class__, __func__, q.c_str());

    _conn.query(&_res, q.c_str());

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

        retr = true;
    } else {
        LOG_ERROR("%s::%s: sql = [%s]: returned NULL", __class__, __func__, q.c_str());
    }
    _conn.freeResult(_res);

    LOG_DEBUG("%s::%s: Required parameters: host: %s, port: %d, user: %s, pass: %s", __class__, __func__,
            _host.c_str(), _port, _username.c_str(), _password.c_str());

    return retr;
}

/*----------------------------------------------------------------------------*/

bool HLR::_login(char const* msisdn, char const* imsi)
{
    bool retr = false;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return retr;
    }

    //-- set msisdn and imsi
    _msisdn = msisdn;
    _imsi = imsi;

    if (! _getLoginParams()) { 
        LOG_ERROR("%s::%s: Failed to get login params!", __class__, __func__);
        return retr;
    }

    if (! _sockConnect()) {
        LOG_ERROR("%s::%s: Unable to connect to server!", __class__, __func__);
        return retr;
    }

    //-- get user prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "ENTER USERNAME <")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"ENTER USERNAME <\" from response!", __class__, __func__);
        return retr;
    }

    //-- send username
    snprintf(_buffer, _buffer_size, "%s\r\n", _username.c_str());
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send username to server!", __class__, __func__);
        return retr;
    }

    //-- get pass prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "ENTER PASSWORD <")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"ENTER PASSWORD <\" from response!", __class__, __func__);
        return retr;
    }

    //-- send password
    snprintf(_buffer, _buffer_size, "%s\r\n", _password.c_str());
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send password to server!", __class__, __func__);
        return retr;
    }

    //-- get main prompt
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "MAIN LEVEL COMMAND <___>")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"MAIN LEVEL COMMAND <___>\" from response!", __class__, __func__);
        return retr;
    }

    //-- successful
    retr = true;

    return retr;
}

/*----------------------------------------------------------------------------*/

bool HLR::_getRealIMSI(char* imsi, int imsi_size)
{
    //ZMIO:MSISDN=<MSISDN>;

    bool retr = false;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return retr;
    }

    //-- initialize imsi...
    memset(imsi, 0, imsi_size);

    snprintf(_buffer, _buffer_size, "ZMIO:MSISDN=%s;\r\n", _msisdn.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return retr;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!", __class__, __func__);
        return retr;
    }

    //-- parse response
    char searchKey[256] = "INTERNATIONAL MOBILE SUBSCRIBER IDENTITY ... ";
    char* p = strstr(_buffer, searchKey);
    if (p) {
        p += strlen(searchKey);
        snprintf(imsi, imsi_size, "%.*s", _imsi_size, p);
    }

    //-- successful
    retr = true;

    LOG_DEBUG("%s::%s: IMSI: %s", __class__, __func__, imsi);
    return retr;
}

/*----------------------------------------------------------------------------*/

bool HLR::_getRealMSISDN(char* msisdn, int msisdn_size)
{
    //ZMIO:IMSI=<IMSI>;

    bool retr = false;

    if (! _buffer) { 
        LOG_ERROR("%s::%s: Communication buffer is not available!", __class__, __func__);
        return retr;
    }

    //-- initialize msisdn...
    memset(msisdn, 0, msisdn_size);

    snprintf(_buffer, _buffer_size, "ZMIO:IMSI=%s;\r\n", _imsi.c_str());
    LOG_DEBUG("%s::%s: command: %s", __class__, __func__, _buffer);

    //-- send request
    if (! _sockSend()) {
        LOG_ERROR("%s::%s: Unable to send request to server!", __class__, __func__);
        return retr;
    }

    //-- get response
    memset(_buffer, 0, _buffer_size);
    _sockReceive();
    if (! strstr(_buffer, "COMMAND EXECUTED")) {
        LOG_DEBUG("%s::%s: Got: %s", __class__, __func__, _buffer);
        LOG_ERROR("%s::%s: Was expecting \"COMMAND EXECUTED\" from response!!", __class__, __func__);
        return retr;
    }

    //-- parse response
    char searchKey[256] = "MOBILE SUBSCRIBER INTERNATIONAL ISDN NUMBER. ";
    char* p = strstr(_buffer, searchKey);
    if (p) {
        p += strlen(searchKey);
        snprintf(msisdn, msisdn_size, "%.*s", _msisdn_size, p);
    }

    //-- successful
    retr = true;

    LOG_DEBUG("%s::%s: MSISDN: %s", __class__, __func__, msisdn);
    return retr;
}

/*============================================================================*/

int HLR::getIMSI(char const* msisdn, char* imsi, int imsi_size)
{
    LOG_INFO("%s::%s: msisdn: %s: start", __class__, __func__, msisdn);

    if (! _login(msisdn, imsi)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }

    if (! _getRealIMSI(imsi, imsi_size)) {
        LOG_ERROR("%s::%s: Unable to get IMSI information!", __class__, __func__);
        return -1;
    }

    LOG_INFO("%s::%s: msisdn: %s, imsi: %s: done", __class__, __func__, msisdn, imsi);

    return 0;
}

/*----------------------------------------------------------------------------*/

int HLR::getMSISDN(char const* imsi, char* msisdn, int msisdn_size)
{
    LOG_INFO("%s::%s: imsi: %s: start", __class__, __func__, imsi);

    if (! _login(msisdn, imsi)) { 
        LOG_ERROR("%s::%s: Unable to login!", __class__, __func__);
        return -1;
    }

    if (! _getRealMSISDN(msisdn, msisdn_size)) {
        LOG_ERROR("%s::%s: Unable to get MSISDN information!", __class__, __func__);
        return -1;
    }

    LOG_INFO("%s::%s: imsi: %s, msisdn: %s: done", __class__, __func__, imsi, msisdn);

    return 0;
}

/******************************************************************************/
