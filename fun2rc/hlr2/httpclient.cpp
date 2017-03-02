/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2017-03-10
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : httpclient.cpp
 * Description : http client
 *
 * $Id$
 */

/******************************************************************************/

#include "httpclient.hpp"

/*============================================================================*/

bool HttpClient::_global_init = (0 == curl_global_init(CURL_GLOBAL_ALL))
    ? true : false;

/*============================================================================*/

HttpClient::HttpClient(char const* key, char const* cert, char const* cacert, bool ignore_cert) :
    _conn(NULL), _hdr_buf(""), _out_buf(""), _err_buf(""),
    _key(key), _cert(cert), _cacert(cacert), _ignore_cert(ignore_cert)
{
    init();
}

/*----------------------------------------------------------------------------*/

HttpClient::~HttpClient()
{
    deinit();
}

/*----------------------------------------------------------------------------*/

void HttpClient::init()
{
    if (HttpClient::_global_init && !_conn) {
        _conn = curl_easy_init();
    }
}

/*----------------------------------------------------------------------------*/

void HttpClient::deinit()
{
    if (HttpClient::_global_init && _conn) {
        curl_easy_cleanup(_conn);
        _conn = NULL;
    }
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpGet(char const* url, unsigned short timeout_sec)
{
    return _perform(url, timeout_sec);
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* fields,
        unsigned short timeout_sec)
{
    if (HttpClient::_global_init && _conn) {
        curl_easy_setopt(_conn, CURLOPT_POST, 1L);
        curl_easy_setopt(_conn, CURLOPT_POSTFIELDS, fields);
    }

    return _perform(url, timeout_sec);
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        char const* content_type, unsigned short timeout_sec)
{
    struct curl_slist* headerlist = NULL;

    if (HttpClient::_global_init && _conn) {
        std::string header(std::string("Content-Type: ") + std::string(content_type));
        headerlist = curl_slist_append(headerlist, header.c_str());

        curl_easy_setopt(_conn, CURLOPT_POST, 1L);
        curl_easy_setopt(_conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(_conn, CURLOPT_HTTPHEADER, headerlist);
    }

    short retr = _perform(url, timeout_sec);

    if (headerlist) {
        curl_slist_free_all(headerlist);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        char const* headers[], unsigned short timeout_sec)
{
    struct curl_slist* headerlist = NULL;

    if (HttpClient::_global_init && _conn) {
        int header_len = sizeof(headers) / sizeof(headers[0]);

        for (int i = 0; i < header_len; ++i) {
            headerlist = curl_slist_append(headerlist, headers[i]);
        }

        curl_easy_setopt(_conn, CURLOPT_POST, 1L);
        curl_easy_setopt(_conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(_conn, CURLOPT_HTTPHEADER, headerlist);
    }

    short retr = _perform(url, timeout_sec);

    if (headerlist) {
        curl_slist_free_all(headerlist);
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

short HttpClient::httpPost(char const* url, char const* content,
        std::vector<std::string>& headers, unsigned short timeout_sec)
{
    struct curl_slist* headerlist = NULL;

    if (HttpClient::_global_init && _conn) {
        for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            headerlist = curl_slist_append(headerlist, ((std::string)*it).c_str());
        }

        curl_easy_setopt(_conn, CURLOPT_POST, 1L);
        curl_easy_setopt(_conn, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(_conn, CURLOPT_HTTPHEADER, headerlist);
    }

    short retr = _perform(url, timeout_sec);

    if (headerlist) {
        curl_slist_free_all(headerlist);
    }

    return retr;
}

/*============================================================================*/

short HttpClient::_perform(char const* url, unsigned short timeout_sec)
{
    short retr = -1;

    std::string hdr_buf;
    std::string out_buf;
    char err_buf[CURL_ERROR_SIZE];

    _hdr_buf.clear();
    _out_buf.clear();
    _err_buf.clear();

    if (!HttpClient::_global_init || !_conn) {
        _err_buf = "System Error";
        return retr;
    }

    if (timeout_sec > 0) {
        curl_easy_setopt(_conn, CURLOPT_CONNECTTIMEOUT, (long) timeout_sec);
        curl_easy_setopt(_conn, CURLOPT_TIMEOUT, (long) timeout_sec);
    }

    if (strncasecmp(url, "https", 5) == 0) {
        if (_key.length() > 0) {
            curl_easy_setopt(_conn, CURLOPT_SSLKEY, _key.c_str());
        }
        if (_cert.length() > 0) {
            curl_easy_setopt(_conn, CURLOPT_SSLCERT, _cert.c_str());
        }
        if (_cacert.length() > 0) {
            curl_easy_setopt(_conn, CURLOPT_CAINFO, _cacert.c_str());
        }
        if (_ignore_cert) {
            curl_easy_setopt(_conn, CURLOPT_SSL_VERIFYPEER, 0L);
        }
    }

    curl_easy_setopt(_conn, CURLOPT_URL, url);
    curl_easy_setopt(_conn, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(_conn, CURLOPT_ERRORBUFFER, err_buf);
    curl_easy_setopt(_conn, CURLOPT_WRITEDATA, &out_buf);
    curl_easy_setopt(_conn, CURLOPT_WRITEFUNCTION, &(HttpClient::_writer));
    curl_easy_setopt(_conn, CURLOPT_WRITEHEADER, &hdr_buf);
    curl_easy_setopt(_conn, CURLOPT_HEADERFUNCTION, &(HttpClient::_writer));

    CURLcode res = curl_easy_perform(_conn);
    if (CURLE_OK == res) {
        long resp_code;
        curl_easy_getinfo(_conn, CURLINFO_RESPONSE_CODE, &resp_code);
        retr = (short) resp_code;
    } else {
        _err_buf = err_buf;
    }
    _hdr_buf = hdr_buf;
    _out_buf = out_buf;

    return retr;
}

/*============================================================================*/

size_t HttpClient::_writer(void* ptr, size_t size, size_t nmemb, void* stream)
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
