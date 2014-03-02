
#include "libfuc.h"
#include "oradbsimple.hpp"


OraDBSimple::OraDBSimple(const char* ora_auth) : OraDB(ora_auth) {}

OraDBSimple::~OraDBSimple() {}

int OraDBSimple::query(sqlo_stmt_handle_t* psth, const char *stmt)
{
    _buf = NULL;


    // If the connection is not valid, no need to even try. reconnect instead.
    if(_dbh < 0)
    {
        ora_reconnect();
        return -1;
    }

    int oci_status;
    while((oci_status = sqlo_open2(psth, _dbh, stmt, 0, NULL)) != SQLO_SUCCESS)
    {
        if(oci_status != SQLO_STILL_EXECUTING)
        {
            LOG_ERROR("Failed opening cursor."
                    " STATEMENT: \"%s\". SQLO_ERROR: \"%s\"", stmt, sqlo_geterror(_dbh));
            ora_reconnect();
            return -1;
        }
        sys_msleep(5);
    }

    int status;
    if((status = sqlo_fetch(*psth, 1)) == SQLO_SUCCESS)
    {
        _buf = sqlo_values(*psth, NULL, 1);
        sqlo_commit(_dbh);
        return 1;
    }
    else if(status == SQLO_NO_DATA)
    {
        sqlo_commit(_dbh);
        return 1;
    }
    else
    {
        LOG_ERROR("Failed fetch from cursor."
                " SQLO_ERROR: \"%s\"", sqlo_get_stmt(*psth), sqlo_geterror(_dbh));
        ora_reconnect();
        return -1;
    }
}

const char** OraDBSimple::fetchRow(sqlo_stmt_handle_t& sth)
{
    if(_buf != NULL)
    {
        const char** res;
        res = _buf;
        _buf = NULL;
        return res;
    }

    int status;
    if((status = sqlo_fetch(sth, 1)) != SQLO_SUCCESS)
    {
        if(status < 0)
        {
            LOG_ERROR("Failed fetch from cursor."
                    " SQLO_ERROR: \"%s\"", sqlo_get_stmt(sth), sqlo_geterror(_dbh));
        }
        return NULL;
    }

    return sqlo_values(sth, NULL, 1);
}

int OraDBSimple::freeResult(sqlo_stmt_handle_t& sth)
{
    if(sqlo_close(sth) != SQLO_SUCCESS)
    {
        LOG_ERROR("Failed to close from cursor."
                " SQLO_ERROR: \"%s\"", sqlo_get_stmt(sth), sqlo_geterror(_dbh));
        return -1;
    }
    return 1;
}
