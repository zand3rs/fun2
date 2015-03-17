/******************************************************************************/

#ifndef _FUN2RC_HLR_HPP_
#define _FUN2RC_HLR_HPP_

#include <string>
#include <vector>
#include "libfuc.h"

#include "oradbsimple.hpp"
#include "fun2rc.hpp"

/*============================================================================*/

class HLR : public Fun2RC {

#define __class__  "HLR"

    private:
        static const size_t _buffer_size = 1024*32;
        static const short _max_sock_retry = 2;

        std::string _ora_auth;
        std::string _msisdn;
        std::string _imsi;

        std::string _username;
        std::string _password;
        std::string _host;

        std::string _ir_sset;
        std::string _normal_sset;

        char* _buffer;
        int _port;
        int _mtimeout;

        OraDBSimple _conn;
        OraDBSimple::stmt_handle_t _res;
        Skt_t* _sock;

        typedef int (HLR::*Func_t)(void);
        typedef struct Step_t_ {
            Func_t execute;
            Func_t rollback;
            char execute_desc[200];
            char rollback_desc[200];
        } Step_t;

        std::vector<Step_t> _activate_steps;
        std::vector<Step_t> _deactivate_steps;

        void _init_steps();

        bool _sockConnect();
        bool _sockDisconnect();
        bool _sockReceive();
        bool _sockSend();

        int _login(char const* msisdn);
        int _logout();

        int _callForwardingUnconditionalErased();
        int _callForwardingBusyErased();
        int _callForwardingNotReachableErased();
        int _callForwardingNoReplyErased();
        int _callForwardingUnconditionalDeactivated();
        int _callForwardingBusyDeactivated();
        int _callForwardingNotReachableDeactivated();
        int _callForwardingNoReplyDeactivated();
        int _defineSuplementaryServicesDeactivated();
        int _defineSuplementaryServicesActivated();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedT11();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB16();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryDeactivatedB1F();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedT11();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB16();
        int _setBarringOfOutGoingCallsRoamingOutsideHomeCountryActivatedB1F();
        int _setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB16();
        int _setBarringOfIncomingCallsRoamingOutsideHomeCountryDeactivatedB1F();
        int _setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB1F();
        int _setBarringOfIncomingCallsRoamingOutsideHomeCountryActivatedB16();
        int _setActivateSAM();
        int _setDeactivateSAM();
        int _setActivateDisableGPRS();
        int _setDeactivateEnableGPRS();

        int _setServiceIndex(char const* sset);
        int _getRealIMSI();

    public:
        HLR() {};
        ~HLR() {};

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
    return new HLR;
}

extern "C" void fun2rc_del(Fun2RC* p) {
    delete p;
}

#endif

/******************************************************************************/
