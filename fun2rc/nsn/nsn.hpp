/******************************************************************************/

#ifndef _FUN2RC_NSN_HPP_
#define _FUN2RC_NSN_HPP_

#include <netdb.h>
#include "fun2rc.hpp"

/*============================================================================*/

class NSN : public Fun2RC {

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
        int executeActivate(char *msisdn);
        int executeDeactivate(char *msisdn);
        int executeGetBalance(char *msisdn);
        int connectSocket();
        int sendData(char *data);
        int readStream(char *buf, int buf_sz);
        struct hostent m_Host;      // Host
        struct hostent *m_pHost;      // Host
        struct sockaddr_in serverSocket;
    public:
        NSN() : buffer(NULL) {};
        ~NSN() {};

        virtual char* const moduleName() const {
            return (char* const)__class__;
        };

        virtual int initialize();
        virtual int deinitialize();

        virtual int activate(const char *msisdn);
        virtual int deactivate(const char *msisdn);

        virtual int getBalance(const char *msisdn);

};

/*============================================================================*/
// the class factories

extern "C" Fun2RC* fun2rc_new() {
    return new NSN;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
