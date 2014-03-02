/******************************************************************************/

#ifndef _FUN2RC_SSET_HPP_
#define _FUN2RC_SSET_HPP_

#include <string>
#include <stdio.h>
#include <time.h>

#include "fun2rc.hpp"

/*============================================================================*/

class SSET : public Fun2RC {

#define __class__  "SSET"

    private:
        int timeout;
        char *url;

    public:
        SSET() {};
        ~SSET() {};

        virtual int initialize();
        virtual int deinitialize();

        virtual int activate(const char *msisdn);
        virtual int deactivate(const char *msisdn);

};

/*============================================================================*/
// the class factories

extern "C" Fun2RC* fun2rc_new() {
    return new SSET;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
