#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbdefaultunli.hpp"


OraDBDefaultUnli::OraDBDefaultUnli(const char* ora_auth) : OraDB(ora_auth) {}

OraDBDefaultUnli::~OraDBDefaultUnli() {}

int OraDBDefaultUnli::initialize(const char* ora_auth)
{
    int res;

    if ((res = OraDB::initialize(ora_auth)) < 0)
        return res;

    if ((res = defaultUnliBind()) < 0)
        return res;

    if ((res = lastFileProcessedBind()) < 0)
        return res;

    return res;
}

int OraDBDefaultUnli::processDefaultUnli(default_unli_t* default_unli)
{
    memcpy(&_default_unli, default_unli, sizeof(default_unli_t));
    _default_unli.db_retr = DB_RETR_INIT;

    int ora_status = ora_force_execute(&_sth_pdu, 0);
    memcpy(default_unli, &_default_unli, sizeof(default_unli_t));

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_PROCESS_DEFAULT_UNLI."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_pdu), sqlo_geterror(_dbh));

        LOG_DEBUG("%s: msisdn: %s, mnc: %s, mcc: %s, sgsn_ip: %s, tac_tai: %s, date: %s, filename: %s", __func__
                , default_unli->msisdn, default_unli->mnc, default_unli->mcc, default_unli->sgsn_ip, default_unli->tac_tai
                , default_unli->date, default_unli->filename);

        //-- try to re-bind...
        defaultUnliBind();
        return -1;
    }

    LOG_DEBUG("%s: retr: %d, start_date: %s, end_date: %s, msisdn: %s, mnc: %s, mcc: %s, sgsn_ip: %s, tac_tai: %s, date: %s, filename: %s", __func__
            , default_unli->db_retr, default_unli->start_date, default_unli->end_date, default_unli->msisdn, default_unli->mnc, default_unli->mcc
            , default_unli->sgsn_ip, default_unli->tac_tai, default_unli->date, default_unli->filename);

    return 0;
}

int OraDBDefaultUnli::defaultUnliBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_PROCESS_DEFAULT_UNLI(:p_retr, :p_sta_dt, :p_end_dt, :p_tran_id, :p_msisdn, :p_mnc, :p_mcc, :p_sgsn_ip, :p_tac_tai, :p_date, :p_filename);"
        " END;";

    _sth_pdu = SQLO_STH_INIT;

    if ((_sth_pdu = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PROCESS_DEFAULT_UNLI.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_pdu, ":p_retr", SQLOT_INT, &_default_unli.db_retr, sizeof(_default_unli.db_retr), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_sta_dt", SQLOT_STR, &_default_unli.start_date, sizeof(_default_unli.start_date), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_end_dt", SQLOT_STR, &_default_unli.end_date, sizeof(_default_unli.end_date), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_tran_id", SQLOT_INT, &_default_unli.id, sizeof(_default_unli.id), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_msisdn", SQLOT_STR, &_default_unli.msisdn, sizeof(_default_unli.msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_mnc", SQLOT_STR, &_default_unli.mnc, sizeof(_default_unli.mnc), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_mcc", SQLOT_STR, &_default_unli.mcc, sizeof(_default_unli.mcc), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_sgsn_ip", SQLOT_STR, &_default_unli.sgsn_ip, sizeof(_default_unli.sgsn_ip), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_tac_tai", SQLOT_STR, &_default_unli.tac_tai, sizeof(_default_unli.tac_tai), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_date", SQLOT_STR, &_default_unli.date, sizeof(_default_unli.date), 0, 0)
                || sqlo_bind_by_name(_sth_pdu, ":p_filename", SQLOT_STR, &_default_unli.filename, sizeof(_default_unli.filename), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_PROCESS_DEFAULT_UNLI statement handle.", __func__);
        return -2;
    }

    return 0;
}

/*============================================================================*/

int OraDBDefaultUnli::getLastFileProcessed(default_unli_t* default_unli)
{
    _var_retr = DB_RETR_INIT;

    memcpy(&_default_unli, default_unli, sizeof(default_unli_t));

    int ora_status = ora_force_execute(&_sth_lfp, 0);
    memcpy(default_unli, &_default_unli, sizeof(default_unli_t));
    default_unli->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_GET_RADCOM_FILE_FORMAT."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_lfp), sqlo_geterror(_dbh));

        //-- try to re-bind...
        lastFileProcessedBind();
        return -1;
    }

    //LOG_DEBUG("%s: retr: %d, filename: %s", __func__, default_unli->db_retr, default_unli->filename);

    return 0;
}

int OraDBDefaultUnli::lastFileProcessedBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_GET_RADCOM_FILE_FORMAT(:p_retr, :p_filename);"
        " END;";

    _sth_lfp = SQLO_STH_INIT;

    if ((_sth_lfp = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_GET_RADCOM_FILE_FORMAT.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_lfp, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_lfp, ":p_filename", SQLOT_STR, &_default_unli.filename, sizeof(_default_unli.filename), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_GET_RADCOM_FILE_FORMAT statement handle.", __func__);
        return -2;
    }

    return 0;
}

