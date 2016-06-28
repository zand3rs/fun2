/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2016-06-28
 *     /         Updated : 2016-06-28
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : mlp_service.hpp
 * Description : MLP Service
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _MLP_SERVICE_HPP_
#define _MLP_SERVICE_HPP_

#include <string>

#include "oradbrequest.hpp"
#include "httpserver.hpp"

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class MlpService : public HttpServer {

    public:
        MlpService()
            : _active(0), _initialized(0) {}

        ~MlpService() {
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
