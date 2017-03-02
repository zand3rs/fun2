/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2017-03-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpclient.hpp
 * Description : http client
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _HTTPCLIENT_HPP_
#define _HTTPCLIENT_HPP_

#include <curl/curl.h>
#include <string>
#include <vector>

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class HttpClient {

    public:
        HttpClient(char const* key = "", char const* cert = "", char const* cacert = "", bool ignore_cert = false);
        ~HttpClient();

        void init();
        void deinit();

        //----------------------------------------------------------------------

        short httpGet(char const* url, unsigned short timeout_sec = 0);
        short httpPost(char const* url, char const* fields,
                unsigned short timeout_sec = 0);
        short httpPost(char const* url, char const* content,
                char const* content_type, unsigned short timeout_sec = 0);
        short httpPost(char const* url, char const* content,
                char const* headers[], unsigned short timeout_sec = 0);
        short httpPost(char const* url, char const* content,
                std::vector<std::string>& headers, unsigned short timeout_sec = 0);

        char const* getResponseHeaders() const {
            return _hdr_buf.c_str();
        }
        char const* getResponseBody() const {
            return _out_buf.c_str();
        }
        char const* getError() const {
            return _err_buf.c_str();
        }

    private:
        CURL* _conn;
        std::string _hdr_buf;
        std::string _out_buf;
        std::string _err_buf;
        std::string _key;
        std::string _cert;
        std::string _cacert;
        bool _ignore_cert;

        static bool _global_init;
        static size_t _writer(void* ptr, size_t size, size_t nmemb, void* stream);

        short _perform(char const* url, unsigned short timeout_sec);
};

#endif

/******************************************************************************/
