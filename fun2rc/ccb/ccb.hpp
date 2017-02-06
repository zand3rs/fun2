/******************************************************************************/

#ifndef _FUN2RC_CCB_HPP_
#define _FUN2RC_CCB_HPP_

#include <string>

#include "fun2rc.hpp"

/*============================================================================*/

class CCB : public Fun2RC {

#define __class__  "CCB"

    public:
        CCB() {};
        ~CCB() {};

        virtual char* const moduleName() const {
            return (char* const)__class__;
        };

        virtual int initialize();
        virtual int deinitialize();

        virtual int activate(const char *msisdn);
        virtual int deactivate(const char *msisdn);

        virtual int getBalance(const char *msisdn);
        virtual int getIMSI(const char *msisdn, char *imsi, int imsi_size);

    private:
        std::string _svc_url;
};

/*============================================================================*/
// the class factories

extern "C" Fun2RC* fun2rc_new() {
    return new CCB;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
