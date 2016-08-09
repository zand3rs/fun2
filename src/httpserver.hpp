/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-08-20
 *     /         Updated : 2009-08-20
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpserver.hpp
 * Description : http server
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _HTTPSERVER_HPP_
#define _HTTPSERVER_HPP_

#include "libfuc.h"

#include <map>
#include <string>

/*-defines--------------------------------------------------------------------*/

#define HTTPSERVER_DEFAULT_PORT   80
#define HTTPSERVER_LISTEN_TIMEOUT 1000
#define HTTPSERVER_CLIENT_TIMEOUT 60000
#define HTTPSERVER_MAX_PACKET_LEN 1024*64
#define HTTPSERVER_STACK_SIZE     0x00040000

/*-functions------------------------------------------------------------------*/

class Http {

    public:
        typedef enum HttpResponseCode_t {
            HTTPRESPONSECODE_INVALID = 0,
            HTTPRESPONSECODE_200_OK = 200,
            HTTPRESPONSECODE_301_MOVEDPERMANENTLY = 301,
            HTTPRESPONSECODE_302_FOUND = 302,
            HTTPRESPONSECODE_400_BADREQUEST = 400,
            HTTPRESPONSECODE_401_UNAUTHORIZED = 401,
            HTTPRESPONSECODE_403_FORBIDDEN = 403,
            HTTPRESPONSECODE_404_NOTFOUND = 404,
            HTTPRESPONSECODE_500_INTERNALSERVERERROR = 500
        } HttpResponseCode_t;

        typedef enum HttpContentType_t {
            HTTPCONTENTTYPE_HTML,
            HTTPCONTENTTYPE_XML,
            HTTPCONTENTTYPE_JSON,
            HTTPCONTENTTYPE_URLENCODED
        } HttpContentType_t;

    public:
        Http() {}
        ~Http() {}

        const char* getResponsePhrase(HttpResponseCode_t responseCode);
        const char* getContentTypeString(HttpContentType_t contentType);

    protected:
        static const char* responsePhrase[];
        static const char* contentTypeString[];

};

/*----------------------------------------------------------------------------*/

class HttpRequest : public Http {

    public:
        HttpRequest()
            : _http_version("HTTP/1.0"), _headers(""), _body("")
            , _method(""), _uri(""), _service("")
            , _contentType(HTTPCONTENTTYPE_HTML) {}
        ~HttpRequest() {}

        int parseHttpRequestPayload(const char* buf, int buf_size);
        const char* getFormValue(const char* key);

        const char* getUri() {
            return _uri.c_str();
        }

        const char* getService() {
            return _service.c_str();
        }

        const char* getBody() {
            return _body.c_str();
        }

        const char* getParam(const char* param) {
            return _form_value_map[std::string(param)].c_str();
        }

        HttpContentType_t getContentType() {
            return _contentType;
        }

    private:
        int _parseQuery(const char* buf);
        int _parseUri(const char* buf);

        int _parseRequestLine(const char* buf);
        int _parseHeaders(const char* buf);
        int _parseBody(const char* buf);

        std::map<std::string, std::string> _form_value_map;
        std::map<std::string, std::string> _headers_map;
        std::string _http_version;
        std::string _headers;
        std::string _body;

        std::string _method;
        std::string _uri;
        std::string _service;

        HttpContentType_t _contentType;

};

/*----------------------------------------------------------------------------*/

class HttpResponse : public Http {

    public:
        HttpResponse()
            : _response_code(HTTPRESPONSECODE_200_OK), _http_version("HTTP/1.0")
            , _headers(""), _body("")
            , _contentType(HTTPCONTENTTYPE_HTML)
            , _contentTypeString("") {}
        ~HttpResponse() {}

        int setHttpVersion(char* httpVersion);
        int addHeader(char* htmlHeader);
        
        int setContentType(HttpContentType_t contentType);
        int setContentType(char* contentType);

        int setBody(char* htmlBody);
        int setResponseCode(HttpResponseCode_t httpCode);

        int createHttpResponsePayload(char* buf, int buf_size);

    private:
        int _addHeader(const char* key, const char* val);
        int _addHeader(const char* key, int val);

        HttpResponseCode_t _response_code;
        std::string _http_version;
        std::string _headers;
        std::string _body;

        HttpContentType_t _contentType;
        std::string _contentTypeString;
};

/*============================================================================*/

class HttpServer : public Http {

    public:
        HttpServer()
            : _shutdown(1), _skt(NULL) {}
        virtual ~HttpServer() {}

        int httpStart(int port = HTTPSERVER_DEFAULT_PORT);
        int httpStop();

    protected:
        // override the handleRequest method -- called once for each request received
        virtual void handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse);

    private:
        friend class _HttpServer;

        int _shutdown;
        Skt_t* _skt;
        pthread_t _conn_thr;

};

/*============================================================================*/

class _HttpServer {

    public:
        _HttpServer(HttpServer* hs) : _hs(hs) {}
        ~_HttpServer() {}

        void handleRequest(const char* ibuf, int ibuf_size, char* obuf, int obuf_size);

        int isShutdown() const {
            return _hs->_shutdown;
        }
        Skt_t* getSocketListener() const {
            return _hs->_skt;
        }

    private:
        HttpServer* _hs;

};

#endif

/******************************************************************************/
