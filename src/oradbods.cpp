#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbods.hpp"


OraDBODS::OraDBODS(const char* ora_auth) : OraDB(ora_auth) {}

OraDBODS::~OraDBODS() {}

int OraDBODS::initialize(const char* ora_auth)
{
    int res;

    if ((res = OraDB::initialize(ora_auth)) < 0)
        return res;

    if ((res = selectBind()) < 0)
        return res;

    if ((res = insertBind()) < 0)
        return res;

    if ((res = updateBind()) < 0)
        return res;

    if ((res = odsBind()) < 0)
        return res;

    return res;
}

int OraDBODS::getODSs(std::vector<ods_t>* odss, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_ODS."
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
    odss->push_back(_ods);
    LOG_DEBUG("%s: ods id: %d fetched from db.", __func__, _ods.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        odss->push_back(_ods);
    }

    return odss->size();
}

int OraDBODS::insertODS(ods_t* ods)
{
    memcpy(&_ods, ods, sizeof(ods_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_ODS."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }

    LOG_DEBUG("%s: ods inserted: msisdn: %s, cluster_node: %d", __func__
            , ods->msisdn, ods->cluster_node);

    return 0;
}

int OraDBODS::updateODS(ods_t* ods)
{
    memcpy(&_ods, ods, sizeof(ods_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_ODS."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }

    LOG_DEBUG("%s: ods updated: id: %d, status: %d", __func__, ods->id, ods->status);

    return 0;
}

int OraDBODS::processODS(ods_t* ods)
{
    _var_retr = DB_RETR_INIT;

    memcpy(&_ods, ods, sizeof(ods_t));

    int ora_status = ora_force_execute(&_sth_po, 0);
    ods->db_retr = _var_retr;
    snprintf(ods->msisdn, sizeof(ods->msisdn), "%s", _ods.msisdn);

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_PROCESS_ODS."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_po), sqlo_geterror(_dbh));

        //-- try to re-bind...
        odsBind();
        return -1;
    }

    LOG_DEBUG("%s: retr: %d, msisdn: %s", __func__
            , ods->db_retr, ods->msisdn);

    return 0;
}

int OraDBODS::selectBind()
{
    const char sql_stmt[] = "select id, msisdn"
        " from ods_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_ODS.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_ods.id, sizeof(_ods.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_ods.msisdn, sizeof(_ods.msisdn), &_ind_msisdn, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_ODS statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBODS::insertBind()
{
    const char sql_stmt[] = "insert into ods_log (id, msisdn, status, cluster_node, tx_date, dt_created)"
        " values (ods_log_seq.nextval, :msisdn, :status, :cluster_node, trunc(sysdate), sysdate)";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_ODS.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":msisdn", SQLOT_STR, &_ods.msisdn, sizeof(_ods.msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":status", SQLOT_INT, &_ods.status, sizeof(_ods.status), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_ods.cluster_node, sizeof(_ods.cluster_node), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_ODS statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBODS::updateBind()
{
    const char sql_stmt[] = "update ods_log set status = :status"
        " where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_ODS.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_ods.id, sizeof(_ods.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_ods.status, sizeof(_ods.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_ODS statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBODS::odsBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_PROCESS_ODS(:p_retr, :p_msisdn);"
        " END;";

    _sth_po = SQLO_STH_INIT;

    if ((_sth_po = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PROCESS_ODS.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_po, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_po, ":p_msisdn", SQLOT_STR, &_ods.msisdn, sizeof(_ods.msisdn), &_ind_msisdn, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_PROCESS_ODS statement handle.", __func__);
        return -2;
    }

    return 0;
}


