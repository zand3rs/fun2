/******************************************************************************/

#ifndef _FUN2RC_HLR2_HPP_
#define _FUN2RC_HLR2_HPP_

#include <string>
#include <vector>

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
        std::string _msisdn;
        std::string _imsi;
        HttpClient _hc;

        typedef int (HLR2::*Func_t)(void);
        typedef struct Step_t_ {
            Func_t execute;
            Func_t rollback;
            char execute_desc[200];
            char rollback_desc[200];
        } Step_t;

        std::vector<Step_t> _activate_steps;
        std::vector<Step_t> _deactivate_steps;

        void _init_steps();

        int _exec(const char *url, const char *payload, unsigned short timeout);
        int _exec(const char *url, const char *payload, unsigned short timeout, std::string& headers);
        int _exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc);
        int _exec(const char *url, const char *payload, unsigned short timeout, pugi::xml_document& doc, std::string& headers);

        int _login(char const* msisdn);
        int _logout();
        int _getIMSI();
        int _noop();

        //-- Activation
        int _deactivateCallForwarding1();
        int _deactivateCallForwarding2();
        int _deactivateCallForwarding3();
        int _deactivateCallForwarding4();
        int _activateOutgoingCallBarring1();
        int _activateOutgoingCallBarring2();
        int _activateOutgoingCallBarring3();
        int _activateIncomingCallBarring1();
        int _activateIncomingCallBarring2();
        int _deactivateMultipartyConferenceCall1();
        int _deactivateMultipartyConferenceCall2();
        int _disableRoamingData1();
        int _disableRoamingData2();
        int _disableRoamingData3();
        int _enableOutgoingVoice1();
        int _enableOutgoingVoice2();
        int _enableOutgoingVoice3();
        int _disableRingBackTone();
        int _enableIncomingVoice();
        int _disableSmsCsi();

        //-- Deactivation
        int _deactivateOutgoingCallBarring1();
        int _deactivateOutgoingCallBarring2();
        int _deactivateOutgoingCallBarring3();
        int _deactivateIncomingCallBarring1();
        int _deactivateIncomingCallBarring2();
        int _activateMultipartyConferenceCall1();
        int _activateMultipartyConferenceCall2();
        int _enableRoamingData1();
        int _enableRoamingData2();
        int _disableOutgoingVoice1();
        int _disableOutgoingVoice2();
        int _disableOutgoingVoice3();
        int _enableRingBackTone();
        int _disableIncomingVoice();
        int _enableSmsCsi();

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
