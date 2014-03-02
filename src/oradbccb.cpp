#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbccb.hpp"


OraDBCCB::OraDBCCB(const char* ora_auth) : OraDB(ora_auth) {}

OraDBCCB::~OraDBCCB() {}

int OraDBCCB::initialize(const char* ora_auth)
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

    return res;
}

int OraDBCCB::getCCBs(std::vector<ccb_t>* ccbs, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_CCB."
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
    ccbs->push_back(_ccb);
    LOG_DEBUG("%s: ccb id: %d fetched from db.", __func__, _ccb.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        ccbs->push_back(_ccb);
    }

    return ccbs->size();
}

int OraDBCCB::insertCCB(ccb_t* ccb)
{
    memcpy(&_ccb, ccb, sizeof(ccb_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_CCB."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }

    LOG_DEBUG("%s: ccb inserted: msisdn: %s, ccb_action: %s, cluster_node: %d", __func__
            , ccb->msisdn, ccb->ccb_action, ccb->cluster_node);

    return 0;
}

int OraDBCCB::updateCCB(ccb_t* ccb)
{
    memcpy(&_ccb, ccb, sizeof(ccb_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_CCB."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }

    LOG_DEBUG("%s: ccb updated: id: %d, status: %d", __func__, ccb->id, ccb->status);

    return 0;
}

int OraDBCCB::selectBind()
{
    const char sql_stmt[] = "select id, msisdn, ccb_action"
        " from ccb_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_CCB.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_ccb.id, sizeof(_ccb.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_ccb.msisdn, sizeof(_ccb.msisdn), &_ind_msisdn, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_STR, &_ccb.ccb_action, sizeof(_ccb.ccb_action), &_ind_ccb_action, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_CCB statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBCCB::insertBind()
{
    const char sql_stmt[] = "insert into ccb_log (id, msisdn, ccb_action, status, cluster_node, tx_date, dt_created)"
        " values (ccb_log_seq.nextval, :msisdn, :ccb_action, :status, :cluster_node, trunc(sysdate), sysdate)";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_CCB.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":msisdn", SQLOT_STR, &_ccb.msisdn, sizeof(_ccb.msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":ccb_action", SQLOT_STR, &_ccb.ccb_action, sizeof(_ccb.ccb_action), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":status", SQLOT_INT, &_ccb.status, sizeof(_ccb.status), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_ccb.cluster_node, sizeof(_ccb.cluster_node), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_CCB statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBCCB::updateBind()
{
    const char sql_stmt[] = "update ccb_log set status = :status"
        " where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_CCB.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_ccb.id, sizeof(_ccb.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_ccb.status, sizeof(_ccb.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_CCB statement handle.", __func__);
        return -2;
    }

    return 0;
}

