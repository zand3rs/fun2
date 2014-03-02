/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : config.cpp
 * Description : Application-specific config functions.
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

#include "config.hpp"

/*============================================================================*/
// Initializers...

std::string Config::_fpath = "";
int Config::_lock_port = 0;
int Config::_loglevel = 0;
std::string Config::_logfile = "";
int Config::_cluster_node = 0;
std::string Config::_access_code = "";
std::string Config::_ora_auth = "";
int Config::_ora_fetch_limit = 100;
int Config::_thread_count = 0;
int Config::_listen_port = 0;
std::string Config::_library_path = ".";

std::string Config::_csp_url = "";
int Config::_csp_timeout_sec = 0;
std::string Config::_csp_cpid = "";
std::string Config::_csp_userid = "";
std::string Config::_csp_password = "";
std::string Config::_csp_keyword = "";

std::string Config::_cart_url = "";
int Config::_cart_timeout_sec = 0;
std::string Config::_cart_userid = "";
std::string Config::_cart_password = "";

std::string Config::_endpoint_url = "";
int Config::_endpoint_timeout_sec = 0;

std::string Config::_nsn_host = "";
int Config::_nsn_port = 0;
std::string Config::_nsn_name = "";
std::string Config::_nsn_user = "";
std::string Config::_nsn_pass = "";

int Config::_bypass_ards = 0;

Config::rc_map_t Config::_rc_map;

/*============================================================================*/

int Config::load(const char *fpath)
{
    char buf[MAX_BUF_SIZE];
    int status;
    Cfg_t cfg;

    status = cfg_init(&cfg, fpath);
    status = cfg_load(&cfg);

    if (! status) {
        _fpath = fpath;

        cfg_getval(&cfg, "", "lock_port", buf, sizeof(buf));
        _lock_port = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "loglevel", buf, sizeof(buf));
        _loglevel = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "logfile", buf, sizeof(buf));
        _logfile = buf;

        cfg_getval(&cfg, "", "cluster_node", buf, sizeof(buf));
        _cluster_node = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "access_code", buf, sizeof(buf));
        _access_code = buf;

        cfg_getval(&cfg, "", "db_host", buf, sizeof(buf));
        std::string db_host = buf;

        cfg_getval(&cfg, "", "db_user", buf, sizeof(buf));
        std::string db_user = buf;

        cfg_getval(&cfg, "", "db_pass", buf, sizeof(buf));
        std::string db_pass = buf;

        _ora_auth = db_user + "/" + db_pass;
        if (! db_host.empty()) {
            _ora_auth.append("@" + db_host);
        }

        cfg_getval(&cfg, "", "db_fetch_limit", buf, sizeof(buf));
        _ora_fetch_limit = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "thread_count", buf, sizeof(buf));
        _thread_count = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "listen_port", buf, sizeof(buf));
        _listen_port = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "library_path", buf, sizeof(buf));
        _library_path = buf;

        cfg_getval(&cfg, "", "csp_url", buf, sizeof(buf));
        _csp_url = buf;

        cfg_getval(&cfg, "", "csp_timeout_sec", buf, sizeof(buf));
        _csp_timeout_sec = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "csp_cpid", buf, sizeof(buf));
        _csp_cpid = buf;

        cfg_getval(&cfg, "", "csp_userid", buf, sizeof(buf));
        _csp_userid = buf;

        cfg_getval(&cfg, "", "csp_password", buf, sizeof(buf));
        _csp_password = buf;

        cfg_getval(&cfg, "", "csp_keyword", buf, sizeof(buf));
        _csp_keyword = buf;

        cfg_getval(&cfg, "", "cart_url", buf, sizeof(buf));
        _cart_url = buf;

        cfg_getval(&cfg, "", "cart_timeout_sec", buf, sizeof(buf));
        _cart_timeout_sec = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "cart_userid", buf, sizeof(buf));
        _cart_userid = buf;

        cfg_getval(&cfg, "", "cart_password", buf, sizeof(buf));
        _cart_password = buf;

        cfg_getval(&cfg, "", "endpoint_url", buf, sizeof(buf));
        _endpoint_url = buf;

        cfg_getval(&cfg, "", "endpoint_timeout_sec", buf, sizeof(buf));
        _endpoint_timeout_sec = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "nsn_host", buf, sizeof(buf));
        _nsn_host = buf;

        cfg_getval(&cfg, "", "nsn_port", buf, sizeof(buf));
        _nsn_port = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "", "nsn_name", buf, sizeof(buf));
        _nsn_name = buf;

        cfg_getval(&cfg, "", "nsn_user", buf, sizeof(buf));
        _nsn_user = buf;

        cfg_getval(&cfg, "", "nsn_pass", buf, sizeof(buf));
        _nsn_pass = buf;

        cfg_getval(&cfg, "", "bypass_ards", buf, sizeof(buf));
        _bypass_ards = strtol(buf, NULL, 10);

        cfg_getval(&cfg, "roaming_component", "roaming_component_count", buf, sizeof(buf));
        int rc_count = strtol(buf, NULL, 10);

        for (int i=1; i<=rc_count; ++i) {
            rc_t rc;
            char section[64];
            snprintf(section, sizeof(section), "roaming_component_%d", i);

            cfg_getval(&cfg, section, "step_no", buf, sizeof(buf));
            rc.step_no = strtol(buf, NULL, 10);

            cfg_getval(&cfg, section, "description", buf, sizeof(buf));
            snprintf(rc.description, sizeof(rc.description), "%s", buf);

            cfg_getval(&cfg, section, "svc_url", buf, sizeof(buf));
            snprintf(rc.svc_url, sizeof(rc.svc_url), "%s", buf);

            cfg_getval(&cfg, section, "svc_host", buf, sizeof(buf));
            snprintf(rc.svc_host, sizeof(rc.svc_host), "%s", buf);

            cfg_getval(&cfg, section, "svc_port", buf, sizeof(buf));
            rc.svc_port = strtol(buf, NULL, 10);

            cfg_getval(&cfg, section, "svc_user", buf, sizeof(buf));
            snprintf(rc.svc_user, sizeof(rc.svc_user), "%s", buf);

            cfg_getval(&cfg, section, "svc_pass", buf, sizeof(buf));
            snprintf(rc.svc_pass, sizeof(rc.svc_pass), "%s", buf);

            cfg_getval(&cfg, section, "db_host", buf, sizeof(buf));
            snprintf(rc.db_host, sizeof(rc.db_host), "%s", buf);

            cfg_getval(&cfg, section, "db_user", buf, sizeof(buf));
            snprintf(rc.db_user, sizeof(rc.db_user), "%s", buf);

            cfg_getval(&cfg, section, "db_pass", buf, sizeof(buf));
            snprintf(rc.db_pass, sizeof(rc.db_pass), "%s", buf);

            cfg_getval(&cfg, section, "timeout", buf, sizeof(buf));
            rc.timeout = strtol(buf, NULL, 10);

            cfg_getval(&cfg, section, "libpath", buf, sizeof(buf));
            snprintf(rc.libpath, sizeof(rc.libpath), "%s", buf);

            cfg_getval(&cfg, section, "thread_count", buf, sizeof(buf));
            rc.thread_count = strtol(buf, NULL, 10);

            cfg_getval(&cfg, section, "max_retry", buf, sizeof(buf));
            rc.max_retry = strtol(buf, NULL, 10);

            if (_rc_map.find(i) == _rc_map.end()) {
                _rc_map.insert(rc_map_t::value_type(i, rc));
            } else {
                _rc_map[i] = rc;
            }
        }

        cfg_unload(&cfg);
    }

    return status;
}

/*----------------------------------------------------------------------------*/

rc_t* Config::getRC(const int step_no)
{
    rc_t *retr = NULL;

#if 0
    if(_rc_map.find(step_no) != _rc_map.end()) {
        retr = &_rc_map[step_no];
    }
#endif
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

rc_t* Config::getNextRC(const int step_no)
{
    rc_t* retr = NULL;
#if 0
    int const next_step_no = step_no + 1;

    if (_rc_map.find(next_step_no) != _rc_map.end()) {
        retr = &_rc_map[next_step_no];
    }
#endif
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

int Config::getNextStepNo(const int step_no)
{
    int retr = -1;
#if 0
    int const next_step_no = step_no + 1;

    if (_rc_map.find(next_step_no) != _rc_map.end()) {
        retr = next_step_no;
    }
#endif
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

/******************************************************************************/