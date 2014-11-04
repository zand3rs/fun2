#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbrequest.hpp"


OraDBRequest::OraDBRequest(const char* ora_auth) : OraDB(ora_auth) {}

OraDBRequest::~OraDBRequest() {}

int OraDBRequest::_do_bind()
{
    int res;

    if ((res = selectBind()) < 0)
        return res;

    if ((res = insertBind()) < 0)
        return res;

    if ((res = updateBind()) < 0)
        return res;

    if ((res = selectManualBind()) < 0)
        return res;

    if ((res = updateManualBind()) < 0)
        return res;

    if ((res = initTranBind()) < 0)
        return res;

    if ((res = processTranBind()) < 0)
        return res;

    if ((res = usurfActivationBind()) < 0)
        return res;

    if ((res = usurfDeactivationBind()) < 0)
        return res;

    if ((res = insertKeywordBind()) < 0)
        return res;

    return res;
}

int OraDBRequest::initialize(const char* ora_auth)
{
    int res;

    if ((res = OraDB::initialize(ora_auth)) < 0)
        return res;

    res = _do_bind();

    return res;
}

int OraDBRequest::getRequests(std::vector<request_t>* requests, int cluster_node, int step_no, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_step_no = step_no;
    _var_status = status;
    _var_limit = limit;

    //LOG_DEBUG("%s: _sth_select=[%s]", __func__, sqlo_command(_sth_select));

    //initialize request buffer...
    memset(&_request, 0, sizeof(request_t));

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_REQUEST."
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
    requests->push_back(_request);
    LOG_DEBUG("%s: request \"%d\" fetched from db.", __func__, _request.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        requests->push_back(_request);
        //clear request buffer...
        memset(&_request, 0, sizeof(request_t));
    }

    return requests->size();
}

int OraDBRequest::insertRequest(const request_t* request)
{
    memcpy(&_request, request, sizeof(request_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_REQUEST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }
    LOG_DEBUG("%s: request inserted msg: %s, a_no: %s, b_no: %s",
            __func__, request->msg, request->a_no, request->b_no);

    return 0;
}

int OraDBRequest::updateRequest(const request_t* request)
{
    memcpy(&_request, request, sizeof(request_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_REQUEST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        LOG_DEBUG("%s: request id: %d"
                ", step_no: %d"
                ", last_step_no: %d"
                ", status: %d"
                ", tran_type: %d"
                ", customer_type: %s"
                ", min_bal: %d"
                ", error_code: %d"
                ", imsi: %s"
                ", activation_date: %s"
                ", deactivation_date: %s"
                ", duration: %d"
                ", gsm_num: %s"
                ", result_code: %d"
                ", silent: %d"
                , __func__ , request->id
                , request->step_no
                , request->last_step_no
                , request->status
                , request->tran_type
                , request->customer_type
                , request->min_bal
                , request->error_code
                , request->imsi
                , request->activation_date
                , request->deactivation_date
                , request->duration
                , request->gsm_num
                , request->result_code
                , request->silent
                );

        //-- try to re-bind...
        updateBind();
        return -1;
    }
    LOG_DEBUG("%s: request %d updated imsi: %s, step_no: %d, last_step_no: %d, status: %d, result_code: %d", __func__
            , request->id, request->imsi, request->step_no, request->last_step_no, request->status, request->result_code);

    return 0;
}

int OraDBRequest::selectBind()
{
    const char sql_stmt[] = "select id, msg, a_no, b_no, step_no, last_step_no"
        ", tran_type, cluster_node, customer_type, request_origin, ref_id, imsi"
        ", activation_date, deactivation_date, duration, gsm_num, result_code, silent"
        " from request_log"
        " where cluster_node = :cluster_node and step_no = :step_no and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":step_no", SQLOT_INT, &_var_step_no, sizeof(_var_step_no), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select,  1, SQLOT_INT, &_request.id, sizeof(_request.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select,  2, SQLOT_STR, &_request.msg, sizeof(_request.msg), &_ind_msg, 0, 0)
                || sqlo_define_by_pos(_sth_select,  3, SQLOT_STR, &_request.a_no, sizeof(_request.a_no), &_ind_a_no, 0, 0)
                || sqlo_define_by_pos(_sth_select,  4, SQLOT_STR, &_request.b_no, sizeof(_request.b_no), &_ind_b_no, 0, 0)
                || sqlo_define_by_pos(_sth_select,  5, SQLOT_INT, &_request.step_no, sizeof(_request.step_no), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select,  6, SQLOT_INT, &_request.last_step_no, sizeof(_request.last_step_no), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select,  7, SQLOT_INT, &_request.tran_type, sizeof(_request.tran_type), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select,  8, SQLOT_INT, &_request.cluster_node, sizeof(_request.cluster_node), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select,  9, SQLOT_STR, &_request.customer_type, sizeof(_request.customer_type), &_ind_customer_type, 0, 0)
                || sqlo_define_by_pos(_sth_select, 10, SQLOT_STR, &_request.request_origin, sizeof(_request.request_origin), &_ind_request_origin, 0, 0)
                || sqlo_define_by_pos(_sth_select, 11, SQLOT_INT, &_request.ref_id, sizeof(_request.ref_id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 12, SQLOT_STR, &_request.imsi, sizeof(_request.imsi), &_ind_imsi, 0, 0)
                || sqlo_define_by_pos(_sth_select, 13, SQLOT_STR, &_request.activation_date, sizeof(_request.activation_date), &_ind_activation_date, 0, 0)
                || sqlo_define_by_pos(_sth_select, 14, SQLOT_STR, &_request.deactivation_date, sizeof(_request.deactivation_date), &_ind_deactivation_date, 0, 0)
                || sqlo_define_by_pos(_sth_select, 15, SQLOT_INT, &_request.duration, sizeof(_request.duration), &_ind_duration, 0, 0)
                || sqlo_define_by_pos(_sth_select, 16, SQLOT_STR, &_request.gsm_num, sizeof(_request.gsm_num), &_ind_gsm_num, 0, 0)
                || sqlo_define_by_pos(_sth_select, 17, SQLOT_INT, &_request.result_code, sizeof(_request.result_code), &_ind_result_code, 0, 0)
                || sqlo_define_by_pos(_sth_select, 18, SQLOT_INT, &_request.silent, sizeof(_request.silent), &_ind_silent, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::insertBind()
{
    const char sql_stmt[] = "insert into request_log (id, txid, msg, a_no, b_no"
        ", tran_type, cluster_node, customer_type, request_origin, ref_id, gsm_num, silent, imsi"
        ", dt_created, tran_dt)"
        " values (request_log_seq.nextval, :txid, :msg, :a_no, :b_no"
        ", :tran_type, :cluster_node, :customer_type, :request_origin, :ref_id, :gsm_num, :silent, :imsi"
        ", sysdate, trunc(sysdate))";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":txid", SQLOT_STR, &_request.txid, sizeof(_request.txid), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":msg", SQLOT_STR, &_request.msg, sizeof(_request.msg), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":a_no", SQLOT_STR, &_request.a_no, sizeof(_request.a_no), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":b_no", SQLOT_STR, &_request.b_no, sizeof(_request.b_no), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":tran_type", SQLOT_INT, &_request.tran_type, sizeof(_request.tran_type), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_request.cluster_node, sizeof(_request.cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":customer_type", SQLOT_STR, &_request.customer_type, sizeof(_request.customer_type), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":request_origin", SQLOT_STR, &_request.request_origin, sizeof(_request.request_origin), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":ref_id", SQLOT_INT, &_request.ref_id, sizeof(_request.ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":gsm_num", SQLOT_STR, &_request.gsm_num, sizeof(_request.gsm_num), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":silent", SQLOT_INT, &_request.silent, sizeof(_request.silent), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":imsi", SQLOT_STR, &_request.imsi, sizeof(_request.imsi), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::updateBind()
{
    const char sql_stmt[] = "update request_log set step_no = :step_no, last_step_no = :last_step_no"
        ", status = :status, tran_type = :tran_type, customer_type = :customer_type, min_bal = :min_bal"
        ", error_code = :error_code, imsi = :imsi"
        ", activation_date = :activation_date, deactivation_date = :deactivation_date, duration = :duration"
        ", gsm_num = :gsm_num, result_code = :result_code, silent = :silent"
        ", dt_modified = sysdate where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_request.id, sizeof(_request.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":step_no", SQLOT_INT, &_request.step_no, sizeof(_request.step_no), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":last_step_no", SQLOT_INT, &_request.last_step_no, sizeof(_request.last_step_no), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_request.status, sizeof(_request.status), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":tran_type", SQLOT_INT, &_request.tran_type, sizeof(_request.tran_type), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":customer_type", SQLOT_STR, &_request.customer_type, sizeof(_request.customer_type), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":min_bal", SQLOT_INT, &_request.min_bal, sizeof(_request.min_bal), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":error_code", SQLOT_INT, &_request.error_code, sizeof(_request.error_code), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":imsi", SQLOT_STR, &_request.imsi, sizeof(_request.imsi), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":activation_date", SQLOT_STR, &_request.activation_date, sizeof(_request.activation_date), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":deactivation_date", SQLOT_STR, &_request.deactivation_date, sizeof(_request.deactivation_date), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":duration", SQLOT_INT, &_request.duration, sizeof(_request.duration), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":gsm_num", SQLOT_STR, &_request.gsm_num, sizeof(_request.gsm_num), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":result_code", SQLOT_INT, &_request.result_code, sizeof(_request.result_code), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":silent", SQLOT_INT, &_request.silent, sizeof(_request.silent), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

/*============================================================================*/

int OraDBRequest::getManualRequests(std::vector<request_t>* requests, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select2, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_REQUEST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_select2), sqlo_geterror(_dbh));

        //-- try to re-bind...
        selectManualBind();
        return -1;
    }
    if(_result == 0) {
        return 0;
    }

    int n_fetched = 0;

    n_fetched++;
    requests->push_back(_request);
    LOG_DEBUG("%s: request \"%d\" fetched from db.", __func__, _request.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select2) == 1)) {
        requests->push_back(_request);
    }

    return requests->size();
}

int OraDBRequest::updateManualRequest(const request_t* request)
{
    memcpy(&_request, request, sizeof(request_t));

    if (ora_force_execute(&_sth_update2, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_MANUAL_REQUEST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update2), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateManualBind();
        return -1;
    }
    LOG_DEBUG("%s: request %d updated status: %d",
            __func__, request->id, request->status);

    return 0;
}

int OraDBRequest::selectManualBind()
{
    //-- ACTIVATION, DEACTIVATION, EXTENSION, CANCEL
    const char sql_stmt[] = "select id, msisdn, enrollment_type, cluster_node, customer_type, link_to, silent, request_origin, imsi"
        " from manual_enrollment_log where tx_date <= sysdate"
        " and cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select2 = SQLO_STH_INIT;

    if ((_sth_select2 = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_MANUAL_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select2, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select2, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select2, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select2,  1, SQLOT_INT, &_request.id, sizeof(_request.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  2, SQLOT_STR, &_request.a_no, sizeof(_request.a_no), &_ind_a_no, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  3, SQLOT_STR, &_request.msg, sizeof(_request.msg), &_ind_msg, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  4, SQLOT_INT, &_request.cluster_node, sizeof(_request.cluster_node), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  5, SQLOT_STR, &_request.customer_type, sizeof(_request.customer_type), &_ind_customer_type, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  6, SQLOT_STR, &_request.gsm_num, sizeof(_request.gsm_num), &_ind_gsm_num, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  7, SQLOT_INT, &_request.silent, sizeof(_request.silent), &_ind_silent, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  8, SQLOT_STR, &_request.request_origin, sizeof(_request.request_origin), &_ind_request_origin, 0, 0)
                || sqlo_define_by_pos(_sth_select2,  9, SQLOT_STR, &_request.imsi, sizeof(_request.imsi), &_ind_imsi, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_MANUAL_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::updateManualBind()
{
    const char sql_stmt[] = "update manual_enrollment_log set"
        " status = :status, dt_modified = sysdate where id = :id";

    _sth_update2 = SQLO_STH_INIT;

    if ((_sth_update2 = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_MANUAL_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update2, ":id", SQLOT_INT, &_request.id, sizeof(_request.id), 0, 0)
                || sqlo_bind_by_name(_sth_update2, ":status", SQLOT_INT, &_request.status, sizeof(_request.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_MANUAL_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

/*============================================================================*/

int OraDBRequest::initTran(request_t* request)
{
    _var_retr = DB_RETR_INIT;

    _var_trantype = request->tran_type;
    _var_req_id = request->id;
    _var_ref_id = request->ref_id;
    snprintf(_var_msisdn, sizeof(_var_msisdn), "%s", request->a_no);

    memset(_var_extra_i_1, 0, sizeof(_var_extra_i_1));
    memset(_var_extra_i_2, 0, sizeof(_var_extra_i_2));
    memset(_var_extra_i_3, 0, sizeof(_var_extra_i_3));

    memset(_var_extra_o_1, 0, sizeof(_var_extra_o_1));
    memset(_var_extra_o_2, 0, sizeof(_var_extra_o_2));
    memset(_var_extra_o_3, 0, sizeof(_var_extra_o_3));

    snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%s", request->activation_date);
    snprintf(_var_extra_i_2, sizeof(_var_extra_i_2), "%d", request->duration);
    snprintf(_var_extra_i_3, sizeof(_var_extra_i_3), "%s", request->gsm_num);
    snprintf(_var_extra_i_4, sizeof(_var_extra_i_4), "%s", request->country);

    int ora_status = ora_force_execute(&_sth_it, 0);
    request->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_INIT_TRAN."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_it), sqlo_geterror(_dbh));

        //-- try to re-bind...
        initTranBind();
        return -1;
    }

    request->min_bal = strtol(_var_extra_o_1, NULL, 10);
    snprintf(request->others, sizeof(request->others), "%s", _var_extra_o_2);

    LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d"
            ", min_bal: %d, activation_date: %s, deactivation_date: %s, duration: %d, country: %s, others: %s", __func__
            , request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id
            , request->min_bal, request->activation_date, request->deactivation_date
            , request->duration, request->country, request->others);

    return 0;
}

int OraDBRequest::initTranBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_INIT_TRAN(:p_retr, :p_extra_o_1, :p_extra_o_2, :p_extra_o_3,"
        " :p_trantype, :p_msisdn, :p_req_id, :p_ref_id, :p_extra_i_1, :p_extra_i_2, :p_extra_i_3, :p_extra_i_4);"
        " END;";

    _sth_it = SQLO_STH_INIT;

    if ((_sth_it = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_INIT_TRAN.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_it, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_o_1", SQLOT_STR, &_var_extra_o_1, sizeof(_var_extra_o_1), &_ind_extra_1, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_o_2", SQLOT_STR, &_var_extra_o_2, sizeof(_var_extra_o_2), &_ind_extra_2, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_o_3", SQLOT_STR, &_var_extra_o_3, sizeof(_var_extra_o_3), &_ind_extra_3, 0)
                || sqlo_bind_by_name(_sth_it, ":p_trantype", SQLOT_INT, &_var_trantype, sizeof(_var_trantype), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_msisdn", SQLOT_STR, &_var_msisdn, sizeof(_var_msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_req_id", SQLOT_INT, &_var_req_id, sizeof(_var_req_id), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_ref_id", SQLOT_INT, &_var_ref_id, sizeof(_var_ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_i_1", SQLOT_STR, &_var_extra_i_1, sizeof(_var_extra_i_1), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_i_2", SQLOT_STR, &_var_extra_i_2, sizeof(_var_extra_i_2), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_i_3", SQLOT_STR, &_var_extra_i_3, sizeof(_var_extra_i_3), 0, 0)
                || sqlo_bind_by_name(_sth_it, ":p_extra_i_4", SQLOT_STR, &_var_extra_i_4, sizeof(_var_extra_i_4), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_INIT_TRAN statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::processTran(request_t* request)
{
    _var_retr = DB_RETR_INIT;

    _var_trantype = request->tran_type;
    _var_req_id = request->id;
    _var_ref_id = request->ref_id;
    snprintf(_var_msisdn, sizeof(_var_msisdn), "%s", request->a_no);

    memset(_var_extra_i_1, 0, sizeof(_var_extra_i_1));
    memset(_var_extra_i_2, 0, sizeof(_var_extra_i_2));
    memset(_var_extra_i_3, 0, sizeof(_var_extra_i_3));

    memset(_var_extra_o_1, 0, sizeof(_var_extra_o_1));
    memset(_var_extra_o_2, 0, sizeof(_var_extra_o_2));
    memset(_var_extra_o_3, 0, sizeof(_var_extra_o_3));

    switch (request->tran_type) {
        case TRAN_TYPE_GROAM_ON:
            snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%s", request->activation_date);
            snprintf(_var_extra_i_2, sizeof(_var_extra_i_2), "%d", request->duration);
            snprintf(_var_extra_i_3, sizeof(_var_extra_i_3), "%s", request->imsi);
            break;
        case TRAN_TYPE_GROAM_EXTEND:
            snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%d", request->duration);
            break;
        case TRAN_TYPE_GLIST:
            snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%s", request->country);
            break;
        case TRAN_TYPE_GLIST_ZONE:
            snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%d", request->zone);
            break;
        default:
            snprintf(_var_extra_i_1, sizeof(_var_extra_i_1), "%s", request->gsm_num);
    }

    int ora_status = ora_force_execute(&_sth_pt, 0);
    request->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_PROCESS_TRAN."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_pt), sqlo_geterror(_dbh));

        //-- try to re-bind...
        processTranBind();
        return -1;
    }

    switch (request->tran_type) {
        case TRAN_TYPE_GROAM_ON:
            snprintf(request->activation_date, sizeof(request->activation_date), "%s", _var_extra_o_1);
            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d"
                    ", activation_date: %s, deactivation_date: %s, duration: %d", __func__
                    , request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id
                    , request->activation_date, request->deactivation_date, request->duration);
            break;
        case TRAN_TYPE_GLIST:
            request->zone = strtol(_var_extra_o_1, NULL, 10);
            snprintf(request->rate, sizeof(request->rate), "%s", _var_extra_o_2);
            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d: zone: %d, rate: %s", __func__,
                    request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id,
                    request->zone, request->rate);
            break;
        case TRAN_TYPE_GLIST_ZONE:
            snprintf(request->country, sizeof(request->country), "%s", _var_extra_o_1);
            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d: country: %s", __func__,
                    request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id, request->country);
            break;
        case TRAN_TYPE_CHECKLINK:
            request->local_sim = strtol(_var_extra_o_1, NULL, 10);
            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d: local_sim: %d", __func__,
                    request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id, request->local_sim);
            break;
        case TRAN_TYPE_YES:
            snprintf(request->gsm_num, sizeof(request->gsm_num), "%s", _var_extra_o_1);
            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d: gsm_num: %s", __func__,
                    request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id, request->gsm_num);
            break;
        default:
            snprintf(request->activation_date, sizeof(request->activation_date), "%s", _var_extra_o_1);
            snprintf(request->deactivation_date, sizeof(request->deactivation_date), "%s", _var_extra_o_2);
            request->duration = strtol(_var_extra_o_3, NULL, 10);

            LOG_DEBUG("%s: retr: %d, trantype: %d, msisdn: %s, req_id: %d, ref_id: %d, link_to: %s, "
                    "activation_date: %s, deactivation_date: %s, duration: %d", __func__,
                    request->db_retr, request->tran_type, request->a_no, request->id, request->ref_id, request->gsm_num,
                    request->activation_date, request->deactivation_date, request->duration);
    }

    return 0;
}

int OraDBRequest::processTranBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_PROCESS_TRAN(:p_retr, :p_extra_o_1, :p_extra_o_2, :p_extra_o_3,"
        " :p_trantype, :p_msisdn, :p_req_id, :p_ref_id, :p_extra_i_1, :p_extra_i_2, :p_extra_i_3);"
        " END;";

    _sth_pt = SQLO_STH_INIT;

    if ((_sth_pt = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PROCESS_TRAN.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_pt, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_o_1", SQLOT_STR, &_var_extra_o_1, sizeof(_var_extra_o_1), &_ind_extra_1, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_o_2", SQLOT_STR, &_var_extra_o_2, sizeof(_var_extra_o_2), &_ind_extra_2, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_o_3", SQLOT_STR, &_var_extra_o_3, sizeof(_var_extra_o_3), &_ind_extra_3, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_trantype", SQLOT_INT, &_var_trantype, sizeof(_var_trantype), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_msisdn", SQLOT_STR, &_var_msisdn, sizeof(_var_msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_req_id", SQLOT_INT, &_var_req_id, sizeof(_var_req_id), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_ref_id", SQLOT_INT, &_var_ref_id, sizeof(_var_ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_i_1", SQLOT_STR, &_var_extra_i_1, sizeof(_var_extra_i_1), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_i_2", SQLOT_STR, &_var_extra_i_2, sizeof(_var_extra_i_2), 0, 0)
                || sqlo_bind_by_name(_sth_pt, ":p_extra_i_3", SQLOT_STR, &_var_extra_i_3, sizeof(_var_extra_i_3), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_PROCESS_TRAN statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::usurfActivation(request_t* request)
{
    _var_retr = DB_RETR_INIT;

    snprintf(_var_msisdn, sizeof(_var_msisdn), "%s", request->a_no);
    snprintf(_var_country, sizeof(_var_country), "%s", request->country);
    _var_duration = request->duration;

    int ora_status = ora_force_execute(&_sth_ua, 0);
    request->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_USURF_ACTIVATION."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_ua), sqlo_geterror(_dbh));

        //-- try to re-bind...
        usurfActivationBind();
        return -1;
    }

    snprintf(request->partner, sizeof(request->partner), "%s", _var_extra_o_1);
    snprintf(request->exptime, sizeof(request->exptime), "%s", _var_extra_o_2);
    snprintf(request->expdate, sizeof(request->expdate), "%s", _var_extra_o_3);

    LOG_DEBUG("%s: retr: %d, msisdn: %s, partner: %s, exptime: %s, expdate: %s", __func__
            , request->db_retr, request->a_no, request->partner, request->exptime, request->expdate);

    return 0;
}

int OraDBRequest::usurfActivationBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_USURF_ACTIVATION(:p_retr, :p_partner, :p_exptime, :p_expdate, :p_msisdn, :p_country, :p_duration);"
        " END;";

    _sth_ua = SQLO_STH_INIT;

    if ((_sth_ua = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_USURF_ACTIVATION.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_ua, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_partner", SQLOT_STR, &_var_extra_o_1, sizeof(_var_extra_o_1), &_ind_extra_1, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_exptime", SQLOT_STR, &_var_extra_o_2, sizeof(_var_extra_o_2), &_ind_extra_2, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_expdate", SQLOT_STR, &_var_extra_o_3, sizeof(_var_extra_o_3), &_ind_extra_3, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_msisdn", SQLOT_STR, &_var_msisdn, sizeof(_var_msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_country", SQLOT_STR, &_var_country, sizeof(_var_country), 0, 0)
                || sqlo_bind_by_name(_sth_ua, ":p_duration", SQLOT_INT, &_var_duration, sizeof(_var_duration), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_USURF_ACTIVATION statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBRequest::usurfDeactivation(request_t* request)
{
    _var_retr = DB_RETR_INIT;

    snprintf(_var_msisdn, sizeof(_var_msisdn), "%s", request->a_no);

    int ora_status = ora_force_execute(&_sth_ud, 0);
    request->db_retr = _var_retr;

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_USURF_DEACTIVATION."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_ud), sqlo_geterror(_dbh));

        //-- try to re-bind...
        usurfDeactivationBind();
        return -1;
    }

    LOG_DEBUG("%s: retr: %d, msisdn: %s", __func__, request->db_retr, request->a_no);

    return 0;
}

int OraDBRequest::usurfDeactivationBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_USURF_DEACTIVATION(:p_retr, :p_msisdn);"
        " END;";

    _sth_ud = SQLO_STH_INIT;

    if ((_sth_ud = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_USURF_DEACTIVATION.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_ud, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_ud, ":p_msisdn", SQLOT_STR, &_var_msisdn, sizeof(_var_msisdn), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_USURF_DEACTIVATION statement handle.", __func__);
        return -2;
    }

    return 0;
}

/*============================================================================*/

int OraDBRequest::insertKeywordRequest(const request_t* request)
{
    memcpy(&_request, request, sizeof(request_t));

    if (ora_force_execute(&_sth_insert_kw, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_REQUEST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert_kw), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }
    LOG_DEBUG("%s: request inserted enrollment_type: %s, msisdn: %s, imsi: %s, ref_id: %d",
            __func__, request->msg, request->a_no, request->imsi, request->ref_id);

    return 0;
}

int OraDBRequest::insertKeywordBind()
{
    const char sql_stmt[] = "insert into keywordRequest (id, ref_id, msisdn, imsi, enrollment_type, start_tm)"
        " values (keywordRequest_id_seq.nextval, :ref_id, :msisdn, :imsi, :enrollment_type, sysdate)";

    _sth_insert_kw = SQLO_STH_INIT;

    if ((_sth_insert_kw = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_KEYWORD_REQUEST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                   sqlo_bind_by_name(_sth_insert_kw, ":ref_id", SQLOT_INT, &_request.ref_id, sizeof(_request.ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_insert_kw, ":msisdn", SQLOT_STR, &_request.a_no, sizeof(_request.a_no), 0, 0)
                || sqlo_bind_by_name(_sth_insert_kw, ":imsi", SQLOT_STR, &_request.imsi, sizeof(_request.imsi), 0, 0)
                || sqlo_bind_by_name(_sth_insert_kw, ":enrollment_type", SQLOT_STR, &_request.msg, sizeof(_request.msg), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_KEYWORD_REQUEST statement handle.", __func__);
        return -2;
    }

    return 0;
}

