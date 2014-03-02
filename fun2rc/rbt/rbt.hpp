/******************************************************************************/

#ifndef _FUN2RC_RBT_HPP_
#define _FUN2RC_RBT_HPP_

#include <string>
#include <stdio.h>
#include <time.h>

#include "fun2rc.hpp"

#define MAX_LENGTH 4048


/*============================================================================*/

class RBT : public Fun2RC {

#define __class__  "RBT"

    private:
        int timeout;
        char *url;
        char url_buffer[MAX_LENGTH];
    public:
        RBT() {};
        ~RBT() {};

        virtual int initialize();
        virtual int deinitialize();

        virtual int activate(const char *msisdn);
        virtual int deactivate(const char *msisdn);

};

/*============================================================================*/
// the class factories

extern "C" Fun2RC* fun2rc_new() {
    return new RBT;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
