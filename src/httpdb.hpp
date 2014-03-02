/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpdb.hpp
 * Description : http handler + oracle db
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _HTTPDB_HPP_
#define _HTTPDB_HPP_

#include <string>

#include "oradbrequest.hpp"
#include "httpserver.hpp"

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class HttpDB : public HttpServer {

    public:
        HttpDB()
            : _active(0), _initialized(0) {}

        ~HttpDB() {
            _deinit();
        }

        int initialize(const int port, const int nThreads, const char* ora_auth);
        int start();
        int stop();

    protected:
        // override the handleRequest method -- called once for each request received
        virtual void handleRequest(HttpRequest *httpRequest, HttpResponse *httpResponse);

    private:
        void _deinit();

        OraDBRequest _conn;
        std::string _ora_auth;
        int _port;

        int _active;
        int _initialized;

};

#endif

/******************************************************************************/
