#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbdailybal.hpp"


OraDBDailyBal::OraDBDailyBal(const char* ora_auth) : OraDB(ora_auth) {}

OraDBDailyBal::~OraDBDailyBal() {}

int OraDBDailyBal::initialize(const char* ora_auth)
{
    int res;

    if ((res = OraDB::initialize(ora_auth)) < 0)
        return res;

    if ((res = selectBind()) < 0)
        return res;

    if ((res = updateBind()) < 0)
        return res;

    if ((res = dailyBalBind()) < 0)
        return res;

    return res;
}

int OraDBDailyBal::getDailyBals(std::vector<daily_bal_t>* daily_bals, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_DAILYBAL."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_select), sqlo_geterror(_dbh));

        //-- try to re-bind...
        selectBind();
        return -1;
    }
    if(_result == 0) {
        return 0;
    }

    int n_fetched = 0;

    n_fetched++;
    daily_bals->push_back(_daily_bal);
    LOG_DEBUG("%s: daily_bal id: %d, msisdn: %s fetched from db.", __func__, _daily_bal.id, _daily_bal.msisdn);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        daily_bals->push_back(_daily_bal);
    }

    return daily_bals->size();
}

int OraDBDailyBal::updateDailyBal(daily_bal_t* daily_bal)
{
    memcpy(&_daily_bal, daily_bal, sizeof(daily_bal_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_DAILYBAL."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }

    LOG_DEBUG("%s: daily balance updated: id: %d", __func__, daily_bal->id);

    return 0;
}

int OraDBDailyBal::processDailyBal(daily_bal_t* daily_bal)
{
    _var_retr = DB_RETR_INIT;

    memcpy(&_daily_bal, daily_bal, sizeof(daily_bal_t));

    int ora_status = ora_force_execute(&_sth_pdb, 0);
    daily_bal->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_PROCESS_DAILY_BALANCE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_pdb), sqlo_geterror(_dbh));

        //-- try to re-bind...
        dailyBalBind();
        return -1;
    }

    snprintf(daily_bal->deactivation_date, sizeof(daily_bal->deactivation_date), "%s", _daily_bal.deactivation_date);

    LOG_DEBUG("%s: retr: %d, msisdn: %s, deactivation_date: %s, daily_balance: %d, current_balance: %d", __func__
            , daily_bal->db_retr, daily_bal->msisdn, daily_bal->deactivation_date, daily_bal->daily_balance, daily_bal->current_balance);

    return 0;
}

int OraDBDailyBal::selectBind()
{
#if 0
    const char sql_stmt[] = "SELECT a.msisdn, b.daily_balance daily_bal, a.last_daily_bal_ctr"
        " FROM  sim_activation a, customer_types b"
        " WHERE a.status = 'ACTIVE'"
        " AND   a.last_daily_bal_chk < trunc(sysdate)"
        " AND   b.customer_type = sf_get_customer_type(a.msisdn)"
        " AND   b.daily_balance > 0"
        " AND   rownum < :limit";
#endif
    const char sql_stmt[] = "select id, msisdn, daily_balance, last_daily_bal_ctr"
        " from dailybal_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_DAILYBAL.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_daily_bal.id, sizeof(_daily_bal.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_daily_bal.msisdn, sizeof(_daily_bal.msisdn), &_ind_msisdn, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_INT, &_daily_bal.daily_balance, sizeof(_daily_bal.daily_balance), &_ind_daily_balance, 0, 0)
                || sqlo_define_by_pos(_sth_select, 4, SQLOT_INT, &_daily_bal.daily_ctr, sizeof(_daily_bal.daily_ctr), &_ind_daily_ctr, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_DAILYBAL statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBDailyBal::updateBind()
{
    const char sql_stmt[] = "update dailybal_log set status = :status"
        " where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_DAILYBAL.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_daily_bal.id, sizeof(_daily_bal.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_daily_bal.status, sizeof(_daily_bal.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_DAILYBAL statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBDailyBal::dailyBalBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_PROCESS_DAILY_BALANCE(:p_retr, :p_deact_dt, :p_msisdn, :p_daily_ctr_m, :p_daily_bal_m, :p_daily_bal_a);"
        " END;";

    _sth_pdb = SQLO_STH_INIT;

    if ((_sth_pdb = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PROCESS_DAILY_BALANCE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_pdb, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_pdb, ":p_deact_dt", SQLOT_STR, &_daily_bal.deactivation_date, sizeof(_daily_bal.deactivation_date), &_ind_deact_dt, 0)
                || sqlo_bind_by_name(_sth_pdb, ":p_msisdn", SQLOT_STR, &_daily_bal.msisdn, sizeof(_daily_bal.msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_pdb, ":p_daily_ctr_m", SQLOT_INT, &_daily_bal.daily_ctr, sizeof(_daily_bal.daily_ctr), 0, 0)
                || sqlo_bind_by_name(_sth_pdb, ":p_daily_bal_m", SQLOT_INT, &_daily_bal.daily_balance, sizeof(_daily_bal.daily_balance), 0, 0)
                || sqlo_bind_by_name(_sth_pdb, ":p_daily_bal_a", SQLOT_INT, &_daily_bal.current_balance, sizeof(_daily_bal.current_balance), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_PROCESS_DAILY_BALANCE statement handle.", __func__);
        return -2;
    }

    return 0;
}


