/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-08-20
 *     /         Updated : 2009-08-20
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpserver.cpp
 * Description : http server
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

#include "httputils.hpp"
#include "httpserver.hpp"

#include <string>
#include <vector>

#include <iostream>

/*============================================================================*/

const char* Http::responsePhrase[] = {
    "INVALID",
    "OK",
    "Moved Permanently",
    "Found",
    "Bad Request",
    "Unauthorized",
    "Forbidden",
    "Not Found",
    "Internal Server Error"
};

/*----------------------------------------------------------------------------*/

const char* Http::contentTypeString[] = {
    "text/html",
    "text/xml",
    "application/json",
    "application/x-www-form-urlencoded"
};

/*----------------------------------------------------------------------------*/

const char* Http::getResponsePhrase(HttpResponseCode_t responseCode)
{
    const char* retr = NULL;

    switch (responseCode) {
        case HTTPRESPONSECODE_200_OK:
            retr = responsePhrase [ 1 ];
            break;
        case HTTPRESPONSECODE_301_MOVEDPERMANENTLY:
            retr = responsePhrase [ 2 ];
            break;
        case HTTPRESPONSECODE_302_FOUND:
            retr = responsePhrase [ 3 ];
            break;
        case HTTPRESPONSECODE_400_BADREQUEST:
            retr = responsePhrase [ 4 ];
            break;
        case HTTPRESPONSECODE_401_UNAUTHORIZED:
            retr = responsePhrase [ 5 ];
            break;
        case HTTPRESPONSECODE_403_FORBIDDEN:
            retr = responsePhrase [ 6 ];
            break;
        case HTTPRESPONSECODE_404_NOTFOUND:
            retr = responsePhrase [ 7 ];
            break;
        case HTTPRESPONSECODE_500_INTERNALSERVERERROR:
            retr = responsePhrase [ 8 ];
            break;
        default:
            retr = responsePhrase [ 0 ];
            break;
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

const char* Http::getContentTypeString(HttpContentType_t contentType)
{
    return contentTypeString[contentType];
}

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*----------------------------------------------------------------------------*/

int HttpRequest::_parseQuery(const char* buf)
{
    char* token;
    char* pbuf;
    char* token2;
    char* pbuf2;

    pbuf = (char*)buf;

    //-- get key value pair...
    while ((token = strtok_r(NULL, "&", &pbuf))) {
        token2 = strtok_r(token, "=", &pbuf2);
        if (token2) {
            std::string key(token2);
            std::string val = "";
            token2 = strtok_r(NULL, "=", &pbuf2);
            if (token2) {
                char tmp[1024];
                url_decode(token2, tmp, sizeof(tmp));
                val.append(tmp);
            }
            //std::cout <<  __func__ << ": key=" << key << ", val=" << val << std::endl;
            _form_value_map[key] = val;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpRequest::_parseUri(const char* buf)
{
    _uri = std::string(buf);

    char* token;
    char* pbuf;

    token = strtok_r((char*)buf, "?", &pbuf);
    if (! token) {
        return -1;
    }

    _service = std::string(token);
    _parseQuery(pbuf);

    //std::cout <<  __func__ << ": uri=" << _uri << std::endl;
    //std::cout <<  __func__ << ": service=" << _service << std::endl;

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpRequest::_parseRequestLine(const char* buf)
{
    char* token;
    char* pbuf;

    //-- get method...
    token = strtok_r((char*)buf, " ", &pbuf);
    if (! token) {
        return -1;
    }
    if (0 == strcasecmp(token, "GET")) {
        _method = "GET";
    } else if (0 == strcasecmp(token, "POST")) {
        _method = "POST";
    } else {
        return -1;
    }

    //-- get uri...
    token = strtok_r(NULL, " ", &pbuf);
    if (! token) {
        return -1;
    }
    _parseUri(token);

    //-- get http version...
    token = strtok_r(NULL, " \r\n", &pbuf);
    if (! token) {
        return -1;
    }
    _http_version = std::string(token);
    //std::cout <<  __func__ << ": http_version=" << _http_version << std::endl;

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpRequest::_parseHeaders(const char* buf)
{
    _headers = std::string(buf);

    char* pbuf = (char*) _headers.c_str();
    char  token[1024];

    while (0 == str_tok2(token, &pbuf, sizeof(token), "\r\n")) {
        char* pbuf2 = token;
        char  token2[1024];

        if (0 == str_tok2(token2, &pbuf2, sizeof(token2), ":")) {
            str_trim(token2);
            str_ucwords(token2);

            std::string key(token2);
            std::string val = "";

            if (0 == str_tok2(token2, &pbuf2, sizeof(token2), ":")) {
                str_trim(token2);
                val.append(token2);
            }
            //std::cout <<  __func__ << ": key=" << key << ", val=" << val << std::endl;
            _headers_map[key] = val;

            if (0 == strcasecmp(key.c_str(), "Content-Type")) {
                for (short i = 0; i < (short)(sizeof(contentTypeString)/sizeof(char*)); ++i) {
                    if (0 == strncasecmp(val.c_str(), contentTypeString[i], sizeof(contentTypeString[i]))) {
                        _contentType = (HttpContentType_t)i;
                    }
                }
            }
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpRequest::_parseBody(const char* buf)
{
    _body = std::string(buf);

    if (_contentType == HTTPCONTENTTYPE_URLENCODED) {
        _parseQuery(buf);
    }

    //std::cout <<  __func__ << ": _contentType=" << _contentType << ", _body" << std::endl << _body << std::endl;

    return 0;
}

/*----------------------------------------------------------------------------*/

// POST / HTTP/1.1
// User-Agent: curl/7.15.1 (i386-redhat-linux-gnu) libcurl/7.15.1 OpenSSL/0.9.8a zlib/1.2.3 libidn/0.6.2
// Host: localhost:1234
// Accept: */*
// Content-Type: text/xml
// Content-Length: 19
//
// key1=val1&key2=val2

int HttpRequest::parseHttpRequestPayload(const char* buf, int buf_size)
{
    char* pbuf = (char*) buf;
    char  token[1024];

    str_tok2(token, &pbuf, sizeof(token), "\r\n");
    _parseRequestLine(token);

    str_tok2(token, &pbuf, sizeof(token), "\r\n\r\n");

    _parseHeaders(token);
    _parseBody(pbuf);

    return 0;
}

/*----------------------------------------------------------------------------*/

const char* HttpRequest::getFormValue(const char* key)
{
    std::string skey = key;
    std::string* sval = &_form_value_map[skey];
    return sval->c_str();
}

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*----------------------------------------------------------------------------*/

int HttpResponse::setHttpVersion(char* httpVersion)
{
    _http_version = httpVersion;
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::addHeader(char* htmlHeader)
{
    _headers.append("\r\n");
    _headers.append(htmlHeader);
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::setContentType(HttpContentType_t contentType)
{
    _contentType = contentType;
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::setContentType(char* contentType)
{
    _contentTypeString = std::string(contentType);
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::setBody(char* htmlBody)
{
    _body = htmlBody;
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::setResponseCode(HttpResponseCode_t httpCode)
{
    _response_code = httpCode;
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::_addHeader(const char* key, const char* val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "\r\n%s: %s", key, val);
    _headers.append(buf);
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::_addHeader(const char* key, int val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "\r\n%s: %d", key, val);
    _headers.append(buf);
    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpResponse::createHttpResponsePayload(char* buf, int buf_size)
{
    char stime[64];
    time_t t;
    struct tm lt;

    t = time(NULL);
    localtime_r(&t, &lt);
    strftime(stime, sizeof(stime), "%a, %d %b %Y %H:%M:%S %z", &lt);

    _addHeader("Date", stime);
    _addHeader("Content-Length", _body.length());

    if (! _contentTypeString.empty()) {
        _addHeader("Content-Type", _contentTypeString.c_str());
    } else {
        _addHeader("Content-Type", getContentTypeString(_contentType));
    }

    snprintf(buf, buf_size, "%s %d %s%s\r\n\r\n%s",
            _http_version.c_str(),
            _response_code,
            Http::getResponsePhrase(_response_code),
            _headers.c_str(),
            _body.c_str());

    return 0;
}

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*----------------------------------------------------------------------------*/

typedef struct Client_t {
    _HttpServer *parent;
    pthread_t tid;
    Skt_t* skt;
} Client_t;

/*----------------------------------------------------------------------------*/

static Client_t* new_client (_HttpServer* parent, Skt_t* skt)
{
    Client_t* client = (Client_t*) malloc(sizeof(Client_t));
    if (client) {
        memset(client, 0, sizeof(Client_t));
        client->parent = parent;
        client->skt = skt;
    }
    return client;
}

/*----------------------------------------------------------------------------*/

static void free_client (Client_t* client)
{
    if (client) {
        skt_close(client->skt);
        free(client);
    }
}

/*----------------------------------------------------------------------------*/

static void* client_handler(void* arg)
{
    Client_t* client = (Client_t*) arg;
    _HttpServer* parent = client->parent;
    Skt_t* s = client->skt;
    char ibuf[HTTPSERVER_MAX_PACKET_LEN+1];
    char obuf[HTTPSERVER_MAX_PACKET_LEN+1];
    int status;
    void* retr = NULL;

    //LOG_DEBUG("%s: start...", __func__);
    //LOG_DEBUG("%s: tid=[%u]", __func__, client->tid);
    LOG_DEBUG("%s: Client connection established (%s:%d)...", __func__, s->host, s->port);

    memset(ibuf, 0, HTTPSERVER_MAX_PACKET_LEN+1);
    memset(obuf, 0, HTTPSERVER_MAX_PACKET_LEN+1);

    status = skt_recv(s, ibuf, HTTPSERVER_MAX_PACKET_LEN, HTTPSERVER_CLIENT_TIMEOUT);
    //LOG_DEBUG("%s: skt_recv: status=[%d]", __func__, status);

    if (0 == status) {
        LOG_DEBUG("%s: Client connection timed-out (%s:%d)...", __func__, s->host, s->port);
        return retr;
    }
    if (0 > status) {
        LOG_INFO("%s: Client connection closed (%s:%d), exiting...", __func__, s->host, s->port);
        return retr;
    }
    LOG_DEBUG("%s: Packet read = [%s]", __func__, ibuf);

    /*-- parse request --*/
    parent->handleRequest(ibuf, strlen(ibuf), obuf, sizeof(obuf));

    status = skt_send(s, obuf, strlen(obuf), HTTPSERVER_CLIENT_TIMEOUT);
    //LOG_DEBUG("%s: skt_send: status=[%d]", __func__, status);

    if (0 >= status) {
        LOG_WARNING("%s: Unable to send response packet to client (%s:%d)!!!", __func__, s->host, s->port);
    } else {
        LOG_DEBUG("%s: Packet sent = [%s]", __func__, obuf);
    }

    free_client(client);

    //LOG_DEBUG("%s: end...", __func__);

    return retr;
}

/*----------------------------------------------------------------------------*/

static void spawn_child (_HttpServer* parent, Skt_t* s)
{
    Client_t* client;

    pthread_attr_t pthread_attr_norm;
    pthread_attr_init(&pthread_attr_norm);
    pthread_attr_setstacksize(&pthread_attr_norm, HTTPSERVER_STACK_SIZE);

    //LOG_DEBUG("%s: start...", __func__);
    client = new_client(parent, s);
    if (client) {
        if (0 != pthread_create(&client->tid, &pthread_attr_norm, client_handler, (void*) client)) {
            LOG_ERROR("%s: Error in creating client thread!", __func__);
            free_client(client);
        }
        else {
            // detach client...
            pthread_detach(client->tid);
        }
    }
    else {
        LOG_ERROR("%s: Unable to spawn client instance!", __func__);
        skt_close(s);
    }
    //LOG_DEBUG("%s: end...", __func__);
}

/*----------------------------------------------------------------------------*/

static void* connection_handler(void* arg)
{
    HttpServer* hs = (HttpServer*) arg;
    _HttpServer *parent = new _HttpServer(hs);
    Skt_t* skt;
    void* retr = NULL;

    while (! parent->isShutdown()) {
        skt = skt_accept(parent->getSocketListener(), HTTPSERVER_LISTEN_TIMEOUT);
        if (skt) {
            spawn_child(parent, skt);
        } else {
            /* -- sleep for a while -- */
            sys_msleep(500);
        }
    }

    delete parent;
    return retr;
}

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*----------------------------------------------------------------------------*/

int HttpServer::httpStart(int port)
{
    if (! _shutdown) {
        return -1;
    }

    _shutdown = 0;
    _skt = skt_open(NULL, port);

    if (! _skt) {
        LOG_ERROR("%s: Unable to open socket!", __func__);
        return -1;
    }
    if (0 != skt_listen(_skt)) {
        LOG_ERROR("%s: Unable to listen from socket!", __func__);
        return -1;
    }

    pthread_attr_t pthread_attr_norm;
    pthread_attr_init(&pthread_attr_norm);
    pthread_attr_setstacksize(&pthread_attr_norm, HTTPSERVER_STACK_SIZE);

    if (0 != pthread_create(&_conn_thr, &pthread_attr_norm, connection_handler, (void*) this)) {
        LOG_ERROR("%s: Unable to create server instance!", __func__);
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

int HttpServer::httpStop()
{
    if (! _shutdown) {
        _shutdown = 1;
        pthread_join(_conn_thr, NULL);
        skt_close(_skt);
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

void HttpServer::handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse)
{
    LOG_DEBUG("%s: called from parent...", __func__);
    httpResponse->setBody("");
    httpResponse->setResponseCode(HTTPRESPONSECODE_200_OK);
}

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/*----------------------------------------------------------------------------*/

void _HttpServer::handleRequest(const char* ibuf, int ibuf_size, char* obuf, int obuf_size)
{
    HttpRequest* httpRequest = new HttpRequest();
    HttpResponse* httpResponse = new HttpResponse();

    httpRequest->parseHttpRequestPayload(ibuf, ibuf_size);
    _hs->handleRequest(httpRequest, httpResponse);
    httpResponse->createHttpResponsePayload(obuf, obuf_size);

    delete httpRequest;
    delete httpResponse;
}

/******************************************************************************/
