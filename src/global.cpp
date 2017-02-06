/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : global.cpp
 * Description : ...
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oradbsimple.hpp"
#include "global.hpp"

/*============================================================================*/
// Initializers...
Global::rc_map_t Global::_rc_map;

bool Global::_shutdown = false;
pid_t Global::_ppid = getpid();
int Global::_transaction_qs_sz = 0;
C2q_t* Global::_transaction_qs = NULL;
C2q_t Global::_transaction_q;
C2q_t Global::_notification_q;
C2q_t Global::_request_q;
C2q_t Global::_response_q;
C2q_t Global::_daily_bal_q;
C2q_t Global::_geo_probe_q;
C2q_t Global::_broadcast_q;
C2q_t Global::_ods_q;
C2q_t Global::_ccb_q;
C2q_t Global::_default_unli_q;
C2q_t Global::_conditioner_q;

/*============================================================================*/

int Global::loadRC(const char* ora_auth, const char* lib_path, const char* brand)
{
    if (OraDBSimple::init_lib() < 0) {
        return -1;
    }

    OraDBSimple conn;
    if (conn.initialize(ora_auth) < 0) {
        return -1;
    }

    OraDBSimple::stmt_handle_t res = OraDBSimple::STH_INIT;
    const char** row;

    char q[512];
    snprintf(q, sizeof(q), "select priority, service_desc, svc_url, svc_host, svc_port, svc_user, svc_pass"
        ", db_host, db_user, db_pass, timeout_sec, thread_count, max_retry, app_lib, svc_name, expiry"
        " from services where brand='%s' and status='ACTIVE'"
        " order by priority", brand);

    int i = 1;
    conn.query(&res, q);
    while((row = conn.fetchRow(res)) != NULL) {
        int step_no = strtol(row[0], NULL, 10);
        string description = row[1];
        string svc_url = row[2];
        string svc_host = row[3];
        int svc_port = strtol(row[4], NULL, 10);
        string svc_user = row[5];
        string svc_pass = row[6];
        string db_host = row[7];
        string db_user = row[8];
        string db_pass = row[9];
        int timeout = strtol(row[10], NULL, 10);
        int thread_count = strtol(row[11], NULL, 10);
        int max_retry = strtol(row[12], NULL, 10);
        string app_lib = row[13];
        string svc_name = row[14];
        string expiry = row[15];

        rc_t rc;
        rc.step_no = step_no;
        snprintf(rc.description, sizeof(rc.description), "%s", description.c_str());
        snprintf(rc.svc_url, sizeof(rc.svc_url), "%s", svc_url.c_str());
        snprintf(rc.svc_host, sizeof(rc.svc_host), "%s", svc_host.c_str());
        rc.svc_port = svc_port;
        snprintf(rc.svc_user, sizeof(rc.svc_user), "%s", svc_user.c_str());
        snprintf(rc.svc_pass, sizeof(rc.svc_pass), "%s", svc_pass.c_str());
        snprintf(rc.db_host, sizeof(rc.db_host), "%s", db_host.c_str());
        snprintf(rc.db_user, sizeof(rc.db_user), "%s", db_user.c_str());
        snprintf(rc.db_pass, sizeof(rc.db_pass), "%s", db_pass.c_str());
        rc.timeout = (timeout > 0) ? timeout : 60;
        rc.thread_count = (thread_count > 0) ? thread_count : 1;
        rc.max_retry = (max_retry > 0) ? max_retry : 1;
        snprintf(rc.libpath, sizeof(rc.libpath), "%s/%s", lib_path, app_lib.c_str());
        snprintf(rc.svc_name, sizeof(rc.svc_name), "%s", svc_name.c_str());
        snprintf(rc.expiry, sizeof(rc.expiry), "%s", expiry.c_str());

        if (_rc_map.find(i) == _rc_map.end()) {
            _rc_map.insert(rc_map_t::value_type(i, rc));
        } else {
            _rc_map[i] = rc;
        }
        i++;
    }
    conn.freeResult(res);
    conn.destroy_db();

    return 0;
}

/*----------------------------------------------------------------------------*/

rc_t* Global::getRC(const int step_no)
{
    rc_t *retr = NULL;

    int rc_count = (int) _rc_map.size();
    for (int i=1; i<=rc_count; ++i) {
        rc_t* rc = &_rc_map[i];
        if (rc->step_no == step_no) {
            retr = rc;
            break;
        }
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

rc_t* Global::getNextRC(const int step_no)
{
    rc_t* retr = NULL;

    int rc_count = (int) _rc_map.size();
    for (int i=1; i<=rc_count; ++i) {
        rc_t* rc = &_rc_map[i];
        if (rc->step_no > step_no) {
            retr = rc;
            break;
        }
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

int Global::getNextStepNo(const int step_no)
{
    int retr = -1;
    int rc_count = (int) _rc_map.size();
    for (int i=1; i<=rc_count; ++i) {
        rc_t* rc = &_rc_map[i];
        if (rc->step_no > step_no) {
            retr = rc->step_no;
            break;
        }
    }

    return retr;
}

/*----------------------------------------------------------------------------*/

int Global::getNextStepNo(const int step_no, const int tran_type)
{
    int retr = -1;
    int rc_count = (int) _rc_map.size();

    if (step_no < 0 || rc_count <= 0) {
        return retr;
    }

    switch (tran_type) {
        case TRAN_TYPE_GROAM_ON:
            for (int i=1; i<=rc_count; ++i) {
                rc_t* rc = &_rc_map[i];
                if (rc->step_no > step_no) {
                    retr = rc->step_no;
                    break;
                }
            }
            break;
        case TRAN_TYPE_GROAM_OFF:
        case TRAN_TYPE_GROAM_NO:
            for (int i=rc_count; i>0; --i) {
                rc_t* rc = &_rc_map[i];
                if (! step_no) {
                    retr = rc->step_no;
                    break;
                }
                if (rc->step_no < step_no) {
                    retr = rc->step_no;
                    break;
                }
            }
            break;
    }

    return retr;
}

/******************************************************************************/
