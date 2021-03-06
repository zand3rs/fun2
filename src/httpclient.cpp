/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2010-10-23
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpclient.cpp
 * Description : http client
 *
 * $Id$
 */

/******************************************************************************/

#include <curl/curl.h>
#include "httpclient.hpp"

/*============================================================================*/

bool HttpClient::_global_init = (0 == curl_global_init(CURL_GLOBAL_ALL))
    ? true : false;

/*============================================================================*/

HttpClient::HttpClient(char const* key, char const* cert, char const* cacert, bool ignore_cert) :
    _out_buf(""), _err_buf(""), _key(key), _cert(cert), _cacert(cacert), _ignore_cert(ignore_cert)
{
}

/*----------------------------------------------------------------------------*/

HttpClient::~HttpClient()
{
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpGet(char const* url, unsigned short timeout_sec)
{
    CURL *conn;
    CURLcode res;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];
    short retr = -1;

    _out_buf.clear();
    _err_buf.clear();

    if (! HttpClient::_global_init) {
        _err_buf = "System Error";
        return retr;
    }

    conn = curl_easy_init();
    if (conn) {
        curl_easy_setopt(conn, CURLOPT_URL, url);

        if (timeout_sec > 0) {
            curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
            curl_easy_setopt(conn, CURLOPT_TIMEOUT, (long) timeout_sec);
        }

        if (strncasecmp(url, "https", 5) == 0) {
            if (_key.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLKEY, _key.c_str());
            }
            if (_cert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLCERT, _cert.c_str());
            }
            if (_cacert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_CAINFO, _cacert.c_str());
            }
            if (_ignore_cert) {
                curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0L);
            }
        }

        curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, err_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEDATA, &out_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));

        res = curl_easy_perform(conn);
        if (CURLE_OK == res) {
            long resp_code;
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &resp_code);
            retr = (short) resp_code;
        } else {
            _err_buf = err_buf;
        }
        _out_buf = out_buf;

        /* cleanup */
        curl_easy_cleanup(conn);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* fields,
        unsigned short timeout_sec)
{
    CURL *conn;
    CURLcode res;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];
    short retr = -1;

    _out_buf.clear();
    _err_buf.clear();

    if (! HttpClient::_global_init) {
        _err_buf = "System Error";
        return retr;
    }

    conn = curl_easy_init();
    if (conn) {
        curl_easy_setopt(conn, CURLOPT_URL, url);
        curl_easy_setopt(conn, CURLOPT_POST, 1L);
        curl_easy_setopt(conn, CURLOPT_POSTFIELDS, fields);

        if (timeout_sec > 0) {
            curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
            curl_easy_setopt(conn, CURLOPT_TIMEOUT, (long) timeout_sec);
        }

        if (strncasecmp(url, "https", 5) == 0) {
            if (_key.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLKEY, _key.c_str());
            }
            if (_cert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLCERT, _cert.c_str());
            }
            if (_cacert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_CAINFO, _cacert.c_str());
            }
            if (_ignore_cert) {
                curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0L);
            }
        }

        curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, err_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEDATA, &out_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));

        res = curl_easy_perform(conn);
        if (CURLE_OK == res) {
            long resp_code;
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &resp_code);
            retr = (short) resp_code;
        } else {
            _err_buf = err_buf;
        }
        _out_buf = out_buf;

        /* cleanup */
        curl_easy_cleanup(conn);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        char const* content_type, unsigned short timeout_sec)
{
    CURL *conn;
    CURLcode res;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];
    short retr = -1;

    _out_buf.clear();
    _err_buf.clear();

    if (! HttpClient::_global_init) {
        _err_buf = "System Error";
        return retr;
    }

    conn = curl_easy_init();
    if (conn) {
        struct curl_slist* headerlist = NULL;
        std::string header(std::string("Content-Type: ") + std::string(content_type));

        headerlist = curl_slist_append(headerlist, header.c_str());

        curl_easy_setopt(conn, CURLOPT_URL, url);
        curl_easy_setopt(conn, CURLOPT_POST, 1L);
        curl_easy_setopt(conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(conn, CURLOPT_HTTPHEADER, headerlist);

        if (timeout_sec > 0) {
            curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
            curl_easy_setopt(conn, CURLOPT_TIMEOUT, (long) timeout_sec);
        }

        if (strncasecmp(url, "https", 5) == 0) {
            if (_key.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLKEY, _key.c_str());
            }
            if (_cert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLCERT, _cert.c_str());
            }
            if (_cacert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_CAINFO, _cacert.c_str());
            }
            if (_ignore_cert) {
                curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0L);
            }
        }

        curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, err_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEDATA, &out_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));

        res = curl_easy_perform(conn);
        if (CURLE_OK == res) {
            long resp_code;
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &resp_code);
            retr = (short) resp_code;
        } else {
            _err_buf = err_buf;
        }
        _out_buf = out_buf;

        /* free header list */
        curl_slist_free_all(headerlist);

        /* cleanup */
        curl_easy_cleanup(conn);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        char const* headers[], unsigned short timeout_sec)
{
    CURL *conn;
    CURLcode res;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];
    short retr = -1;

    _out_buf.clear();
    _err_buf.clear();

    if (! HttpClient::_global_init) {
        _err_buf = "System Error";
        return retr;
    }

    conn = curl_easy_init();
    if (conn) {
        struct curl_slist* headerlist = NULL;
        int header_len = sizeof(headers) / sizeof(headers[0]);

        for (int i = 0; i < header_len; ++i) {
            headerlist = curl_slist_append(headerlist, headers[i]);
        }

        curl_easy_setopt(conn, CURLOPT_URL, url);
        curl_easy_setopt(conn, CURLOPT_POST, 1L);
        curl_easy_setopt(conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(conn, CURLOPT_HTTPHEADER, headerlist);

        if (timeout_sec > 0) {
            curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
            curl_easy_setopt(conn, CURLOPT_TIMEOUT, (long) timeout_sec);
        }

        if (strncasecmp(url, "https", 5) == 0) {
            if (_key.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLKEY, _key.c_str());
            }
            if (_cert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLCERT, _cert.c_str());
            }
            if (_cacert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_CAINFO, _cacert.c_str());
            }
            if (_ignore_cert) {
                curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0L);
            }
        }

        curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, err_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEDATA, &out_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));

        res = curl_easy_perform(conn);
        if (CURLE_OK == res) {
            long resp_code;
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &resp_code);
            retr = (short) resp_code;
        } else {
            _err_buf = err_buf;
        }
        _out_buf = out_buf;

        /* free header list */
        curl_slist_free_all(headerlist);

        /* cleanup */
        curl_easy_cleanup(conn);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        std::vector<std::string>& headers, unsigned short timeout_sec)
{
    CURL *conn;
    CURLcode res;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];
    short retr = -1;

    _out_buf.clear();
    _err_buf.clear();

    if (! HttpClient::_global_init) {
        _err_buf = "System Error";
        return retr;
    }

    conn = curl_easy_init();
    if (conn) {
        struct curl_slist* headerlist = NULL;
        for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            headerlist = curl_slist_append(headerlist, ((std::string)*it).c_str());
        }

        curl_easy_setopt(conn, CURLOPT_URL, url);
        curl_easy_setopt(conn, CURLOPT_POST, 1L);
        curl_easy_setopt(conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(conn, CURLOPT_HTTPHEADER, headerlist);

        if (timeout_sec > 0) {
            curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
            curl_easy_setopt(conn, CURLOPT_TIMEOUT, (long) timeout_sec);
        }

        if (strncasecmp(url, "https", 5) == 0) {
            if (_key.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLKEY, _key.c_str());
            }
            if (_cert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_SSLCERT, _cert.c_str());
            }
            if (_cacert.length() > 0) {
                curl_easy_setopt(conn, CURLOPT_CAINFO, _cacert.c_str());
            }
            if (_ignore_cert) {
                curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0L);
            }
        }

        curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, err_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEDATA, &out_buf);
        curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));

        res = curl_easy_perform(conn);
        if (CURLE_OK == res) {
            long resp_code;
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &resp_code);
            retr = (short) resp_code;
        } else {
            _err_buf = err_buf;
        }
        _out_buf = out_buf;

        /* free header list */
        curl_slist_free_all(headerlist);

        /* cleanup */
        curl_easy_cleanup(conn);
    }

    return retr;
}

/*============================================================================*/

size_t HttpClient::_writer(void *ptr, size_t size, size_t nmemb, void *stream)
{
    if (! stream) {
        return 0;
    }

    char* const data = (char*) ptr;
    size_t const length = nmemb * size;
    ((std::string*) stream)->append(data, length);

    return length;
}

/******************************************************************************/
