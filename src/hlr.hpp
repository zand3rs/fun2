/******************************************************************************/

#ifndef _HLR_HPP_
#define _HLR_HPP_

#include <string>
#include "oradbsimple.hpp"

#include "libfuc.h"

/*============================================================================*/

class HLR {

#define __class__  "HLR"

    private:
        static const size_t _buffer_size = 1024*32;
        static const short _max_sock_retry = 2;
        static const short _imsi_size = 15;
        static const short _msisdn_size = 12;

        std::string _ora_auth;
        std::string _msisdn;
        std::string _imsi;

        std::string _username;
        std::string _password;
        std::string _host;

        char* _buffer;
        int _port;
        int _mtimeout;

        OraDBSimple _conn;
        OraDBSimple::stmt_handle_t _res;
        Skt_t* _sock;

        bool _sockConnect();
        bool _sockDisconnect();
        bool _sockReceive();
        bool _sockSend();

        bool _initialize();
        void _deinitialize();
        bool _getLoginParams();

        bool _login(char const* msisdn, char const* imsi);
        bool _getRealIMSI(char* imsi, int imsi_size);
        bool _getRealMSISDN(char* msisdn, int msisdn_size);

    public:
        HLR(char const* ora_auth="")
            : _ora_auth(ora_auth), _msisdn("")
            , _username(""), _password(""), _host("")
            , _port(0), _mtimeout(0), _sock(NULL) {
            _initialize();
        };
        ~HLR() {
            _deinitialize();
        };

        int getIMSI(char const* msisdn, char* imsi, int imsi_size);
        int getMSISDN(char const* imsi, char* msisdn, int msisdn_size);
};

#endif

/******************************************************************************/
