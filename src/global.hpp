/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : global.hpp
 * Description : ...
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _GLOBAL_HPP_
#define _GLOBAL_HPP_

#include <sys/types.h>
#include <signal.h>

#include <string>
#include <map>

#include "libfuc.h"
#include "data_struct.hpp"

using namespace std;

/*-defines--------------------------------------------------------------------*/

#define IS_SHUTDOWN() Global::isShutdown()
#define DO_SHUTDOWN() Global::doShutdown()
#define DO_ABORT() kill(getpid(),SIGABRT)

/*-functions------------------------------------------------------------------*/

class Global {

    public:
        static const int q_wait_sec = 5;
        //static const size_t thr_stack_size = 0x00040000;
        static const size_t thr_stack_size = 0x00100000;

        static int loadRC(const char* ora_auth, const char* lib_path);
        static rc_t* getRC(const int step_no);
        static rc_t* getNextRC(const int step_no);
        static int getNextStepNo(const int step_no);
        static int getNextStepNo(const int step_no, const int tran_type);

        static int getRCSize() {
            return (int)_rc_map.size();
        }

        static bool isShutdown() {
            return _shutdown;
        }

        static void doShutdown() {
            _shutdown = true;
        }

        static void doAbort() {
            kill(_ppid, SIGABRT);
        }

        static pid_t getPid() {
            return _ppid;
        }

        static int initTransactionQ(int max_sz) {
            if (max_sz > 0) {
                _transaction_qs_sz = max_sz;
                _transaction_qs = (C2q_t*) malloc(sizeof(C2q_t) * _transaction_qs_sz);
                memset(_transaction_qs, 0, sizeof(C2q_t) * _transaction_qs_sz);
            }
            return (_transaction_qs) ? 0 : -1;
        }

        static void deinitTransactionQ() {
            if (_transaction_qs && _transaction_qs_sz > 0) {
                free(_transaction_qs);
                _transaction_qs = NULL;
                _transaction_qs_sz = 0;
            }
        }

        static int getTransactionQSize() {
            return _transaction_qs_sz;
        }

        static C2q_t* getTransactionQ(int idx) {
            if (_transaction_qs && idx < _transaction_qs_sz) {
                return &_transaction_qs[idx];
            }
            return NULL;
        }

        static C2q_t* getTransactionQ() {
            return &_transaction_q;
        }

        static C2q_t* getNotificationQ() {
            return &_notification_q;
        }

        static C2q_t* getRequestQ() {
            return &_request_q;
        }

        static C2q_t* getResponseQ() {
            return &_response_q;
        }

        static C2q_t* getDailyBalQ() {
            return &_daily_bal_q;
        }

        static C2q_t* getGeoProbeQ() {
            return &_geo_probe_q;
        }

        static C2q_t* getBroadcastQ() {
            return &_broadcast_q;
        }

        static C2q_t* getODSQ() {
            return &_ods_q;
        }

        static C2q_t* getCCBQ() {
            return &_ccb_q;
        }

        static C2q_t* getDefaultUnliQ() {
            return &_default_unli_q;
        }

        static C2q_t* getConditionerQ() {
            return &_conditioner_q;
        }

    private:
        typedef std::map<int, rc_t> rc_map_t;
        static rc_map_t _rc_map;

        static bool _shutdown;
        static pid_t _ppid;
        static int _transaction_qs_sz;
        static C2q_t* _transaction_qs;
        static C2q_t _transaction_q;
        static C2q_t _notification_q;
        static C2q_t _request_q;
        static C2q_t _response_q;
        static C2q_t _daily_bal_q;
        static C2q_t _geo_probe_q;
        static C2q_t _broadcast_q;
        static C2q_t _ods_q;
        static C2q_t _ccb_q;
        static C2q_t _default_unli_q;
        static C2q_t _conditioner_q;
};

#endif

/******************************************************************************/
