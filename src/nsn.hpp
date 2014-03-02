/******************************************************************************/

#ifndef _NSN_HPP_
#define _NSN_HPP_

#include <netdb.h>
#include <string.h>

/*============================================================================*/

class NSN {

#define __class__  "NSN"

    private:
        static const int buffer_size=1024*16;
        char* buffer;
        char username[64];
        char password[64];
        char host[64];
        int port;
        int timeout;
        char nsnUsername[64];
        char nsnExpirationDate[64];
        char session_url[1024];
        char payload[1024];
        int m_serverSocket;
        char *getPingUrl();
        int ping();
        int login();
        int logout();
        int executeGetBalance(char *msisdn);
        int connectSocket();
        int sendData(char *data);
        int readStream(char *buf, int buf_sz);
        struct hostent m_Host;      // Host
        struct hostent *m_pHost;      // Host
        struct sockaddr_in serverSocket;
        std::string _ora_auth;

        int _initialize();
        int _deinitialize();

    public:
        NSN(const char* ora_auth="") : _ora_auth(ora_auth) {
            _initialize();
        };
        ~NSN() {
            _deinitialize();
        };

        int getBalance(const char *msisdn);

};

#endif

/******************************************************************************/
