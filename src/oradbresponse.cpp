#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbresponse.hpp"


OraDBResponse::OraDBResponse(const char* ora_auth) : OraDB(ora_auth) {}

OraDBResponse::~OraDBResponse() {}

int OraDBResponse::initialize(const char* ora_auth)
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

int OraDBResponse::getResponses(std::vector<response_t>* responses, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_RESPONSE."
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
    responses->push_back(_response);
    LOG_DEBUG("%s: response \"%d\" fetched from db.", __func__, _response.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        responses->push_back(_response);
    }

    return responses->size();
}

int OraDBResponse::insertResponse(const response_t* response)
{
    memcpy(&_response, response, sizeof(response_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_RESPONSE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }
    LOG_DEBUG("%s: response inserted msg: %s, a_no: %s, b_no: %s",
            __func__, response->msg, response->a_no, response->b_no);

    return 0;
}

int OraDBResponse::updateResponse(const response_t* response)
{
    memcpy(&_response, response, sizeof(response_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_RESPONSE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }
    LOG_DEBUG("%s: response %d updated status: %d",
            __func__, response->id, response->status);

    return 0;
}

int OraDBResponse::selectBind()
{
    const char sql_stmt[] = "select id, msg, a_no, b_no, cluster_node, txid from response_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_RESPONSE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_response.id, sizeof(_response.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_response.msg, sizeof(_response.msg), &_ind_msg, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_STR, &_response.a_no, sizeof(_response.a_no), &_ind_a_no, 0, 0)
                || sqlo_define_by_pos(_sth_select, 4, SQLOT_STR, &_response.b_no, sizeof(_response.b_no), &_ind_b_no, 0, 0)
                || sqlo_define_by_pos(_sth_select, 5, SQLOT_INT, &_response.cluster_node, sizeof(_response.cluster_node), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 6, SQLOT_STR, &_response.txid, sizeof(_response.txid), &_ind_txid, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_RESPONSE statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBResponse::insertBind()
{
    const char sql_stmt[] = "insert into response_log (id, ref_id, txid, msg, a_no, b_no, cluster_node, customer_type, tran_type, msg_id, dt_created, tran_dt)"
        " values (response_log_seq.nextval, :ref_id, :txid, :msg, :a_no, :b_no, :cluster_node, :customer_type, :tran_type, :msg_id, sysdate, trunc(sysdate))";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_RESPONSE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":ref_id", SQLOT_INT, &_response.ref_id, sizeof(_response.ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":txid", SQLOT_STR, &_response.txid, sizeof(_response.txid), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":msg", SQLOT_STR, &_response.msg, sizeof(_response.msg), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":a_no", SQLOT_STR, &_response.a_no, sizeof(_response.a_no), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":b_no", SQLOT_STR, &_response.b_no, sizeof(_response.b_no), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_response.cluster_node, sizeof(_response.cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":customer_type", SQLOT_STR, &_response.customer_type, sizeof(_response.customer_type), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":tran_type", SQLOT_INT, &_response.tran_type, sizeof(_response.tran_type), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":msg_id", SQLOT_STR, &_response.msg_id, sizeof(_response.msg_id), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_RESPONSE statement handle.", __func__);
        return -2;
    }

    return 0;
}


int OraDBResponse::updateBind()
{
    const char sql_stmt[] = "update response_log set status = :status"
        ", dt_modified = sysdate where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_RESPONSE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_response.id, sizeof(_response.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_response.status, sizeof(_response.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_RESPONSE statement handle.", __func__);
        return -2;
    }

    return 0;
}


