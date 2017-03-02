/******************************************************************************/

#ifndef _FUN2RC_HLR2_HPP_
#define _FUN2RC_HLR2_HPP_

#include <string>
#include <stdio.h>
#include <time.h>

#include "fun2rc.hpp"


/*============================================================================*/

class HLR2 : public Fun2RC {

#define __class__  "HLR2"

    private:
        unsigned short _timeout;
        std::string _url;
        std::string _location;
        std::string _username;
        std::string _password;
        HttpClient _hc;

        int _login();
        int _logout();
        int _getIMSI(const char *msisdn, char *imsi, int imsi_size);

        int _exec(const char *url, const char *payload, unsigned short timeout);
        int _exec(const char *url, const char *payload, unsigned short timeout, std::string& headers);
        int _exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc);
        int _exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc, std::string& headers);

    public:
        HLR2() {};
        ~HLR2() {};

        virtual char* const moduleName() const {
            return (char* const)__class__;
        };

        virtual int initialize();
        virtual int deinitialize();

        virtual int activate(const char *msisdn);
        virtual int deactivate(const char *msisdn);

        virtual int getIMSI(const char *msisdn, char *imsi, int imsi_size);
};

/*============================================================================*/
// the class factories

extern "C" Fun2RC* fun2rc_new() {
    return new HLR2;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
