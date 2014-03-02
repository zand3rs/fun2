#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>

#include "libfuc.h"
#include "oradb.hpp"

#define ORAERR_TERMINAL  -3
#define ORAERR_CRITICAL  -2
#define ORAERR_REGULAR   -1

// Class OraDB
//
//

OraDB::~OraDB()
{
    destroy_db();
}

bool OraDB::_initialized = false;

int OraDB::init_lib(bool threaded_mode)
{
    if (_initialized) {
        //LOG_DEBUG("%s: libsqlora is already initialized!", __func__);
        return 0;
    }

    if (threaded_mode) {
        if (SQLO_SUCCESS != sqlo_init(SQLO_ON, 1000, 10000)) {
            LOG_CRITICAL("%s: Failed to init libsqlora!!!", __func__);
            return -1;
        }
    } else {
        if (SQLO_SUCCESS != sqlo_init(SQLO_OFF, 100, 1000)) {
            LOG_CRITICAL("%s: Failed to init libsqlora!!!", __func__);
            return -1;
        }
    }
    _initialized = true;

    return 0;
}

int OraDB::initialize()
{
    int retry_count = 0;

    while(sqlo_connect(&_dbh, _ora_auth.c_str()) != SQLO_SUCCESS) {
        if(retry_count++ >= MAX_ORACLE_CONNECT_RETRY_COUNT) {
            LOG_CRITICAL("%s: Failed to connect to oracle!!!. \"%s\". SQLORA: \"%s\"", __func__, _ora_auth.c_str(), sqlo_geterror(_dbh));
            _dbh = -1;
            return ORAERR_TERMINAL;
        }
        sys_msleep(200);
    }

    LOG_DEBUG("%s: Connected to oracle database. dbh %d.", __func__, _dbh);

    // sqlo_set_blocking(_dbh, 0);
    // sqlo_set_autocommit(_dbh, SQLO_ON);

    if(ora_ping_bind() < 0) {
        destroy_db();
        return ORAERR_CRITICAL;
    }

    return 1;
}

int OraDB::destroy_db()
{
    sqlo_finish(_dbh);
    _dbh = -1;
    return 1;
}

// Check if oracle connection is working
int OraDB::ora_reconnect() 
{
    destroy_db();
    return initialize();
}

// Check if oracle connection is working
int OraDB::ora_ping() {

    int ora_res;
    int connect_res;

    while((ora_res = sqlo_execute(_sth_ping, 1)) != SQLO_SUCCESS) {
        if(ora_res != SQLO_STILL_EXECUTING) {
            LOG_WARNING("%s: SQLO ping failed. SQLO_ERROR: \"%s\"Reconnecting.", __func__, sqlo_geterror(_dbh));
            connect_res = ora_reconnect();
            if(connect_res < 0) {
                LOG_ERROR("%s: SQLO reconnection failed.", __func__);
                return connect_res;
            }
            LOG_DEBUG("%s: SQLO reconnected.", __func__);
        }
        sys_msleep(5);
    }
    LOG_DEBUG("%s: SQLO: Ping successful.", __func__);

    return 0;
}

// Force the execution of an statement handle.
//  If an error is encountered, it will ask ora_ping check the connection.
//  If ora_ping still failed, will return error.
//  Note: This will also check if the return value of stored proc is unchanged
//      If it is, this will assume that the stored proc is uncalled. Thus,
//      this is not suitable for ordinary queries.
int OraDB::ora_force_execute (sqlo_stmt_handle_t* psth, int check_res, int commit) {

    int oci_status;
    int ping_status;
    int n_wait;
    int f_wait;
    int c_wait;

    n_wait = 0;
    f_wait = 0;
    c_wait = 0;
    _result = check_res ? 500 : 1;
    for(;;) {
        oci_status = sqlo_execute(*psth, 1);
        if(oci_status == SQLO_SUCCESS && _result == 500) {
            LOG_ERROR("%s: sqlo_execute returned but stored proc not executed. %s", __func__, sqlo_command(*psth));
            ping_status = ora_reconnect();
            sys_msleep(50);
            if(ping_status<0) {
                return ping_status;
            }
            n_wait = 0;
            f_wait = 0;
            c_wait++;
        }
        else if (oci_status == SQLO_NO_DATA) {
            _result = 0;
            break;
        }
        else if (oci_status == SQLO_SUCCESS && _result != 500) {
            break;
        }
        else if (oci_status != SQLO_STILL_EXECUTING)
        {
            LOG_ERROR("%s: oci_status = %d. STATEMENT: \"%s\" SQLO_ERROR IN RETRY: %s", __func__,
                    oci_status, sqlo_command(*psth), sqlo_geterror(_dbh));
            ping_status = ora_ping();
            sys_msleep(50);
            if(ping_status<0) {
                return ping_status;
            }
            n_wait = 0;
            f_wait++;
        }
        else {
            n_wait++;
            sys_msleep(5);
        }
        if(n_wait > 2000 || f_wait > 3 || c_wait > 3) {
            if(oci_status == SQLO_STILL_EXECUTING)
                LOG_ERROR("%s: oci_status = %d. STATEMENT: \"%s\" timedout.", __func__,
                        oci_status, sqlo_command(*psth));

            return ORAERR_REGULAR;
        }
    }

    if(commit)
        sqlo_commit(_dbh);

    return 0;
}

int OraDB::ora_force_fetch(sqlo_stmt_handle_t* psth) {

    int status;
    int retval;


    int n_wait = 0;
    while((status = sqlo_fetch(*psth, 1)) == SQLO_STILL_EXECUTING) {
        sys_msleep(5);
        if(n_wait++ >= 2000)
            break;
    }

    switch(status) {
        case SQLO_NO_DATA:
            retval = 0;
            break;
        case SQLO_SUCCESS:
            retval = 1;
            break;
        default:
            retval = 0;
            break;
    }

    return retval;
}

int OraDB::ora_ping_bind() {

    const char stmt[] = "select 1 from user_tables where rownum = 1";

    _sth_ping = SQLO_STH_INIT;

    if((_sth_ping = sqlo_prepare(_dbh, stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PING.", __func__);
        return -1;
    }

    if(SQLO_SUCCESS !=  
            (sqlo_define_by_pos(_sth_ping, 1, SQLOT_INT, &_result, sizeof(_result), 0, 0, 0)
            ))
    {
        LOG_CRITICAL("%s: bind variables for PING statement handle.", __func__);
        return -1;
    }


    return 1;
}


