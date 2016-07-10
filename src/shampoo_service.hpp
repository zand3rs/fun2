/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-07-10
 *     /         Updated : 2016-07-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : shampoo_service.hpp
 * Description : Shampoo Service
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _SHAMPOO_SERVICE_HPP_
#define _SHAMPOO_SERVICE_HPP_

#include <string>

#include "oradbrequest.hpp"
#include "httpserver.hpp"

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class ShampooService : public HttpServer {

    public:
        ShampooService()
            : _active(0), _initialized(0) {}

        ~ShampooService() {
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
