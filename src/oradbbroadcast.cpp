#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbbroadcast.hpp"


OraDBBroadcast::OraDBBroadcast(const char* ora_auth) : OraDB(ora_auth) {}

OraDBBroadcast::~OraDBBroadcast() {}

int OraDBBroadcast::initialize(const char* ora_auth)
{
    int res;

    if ((res = OraDB::initialize(ora_auth)) < 0)
        return res;

    if ((res = selectBind()) < 0)
        return res;

    if ((res = updateBind()) < 0)
        return res;

    return res;
}

int OraDBBroadcast::getBroadcasts(std::vector<broadcast_t>* broadcasts, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    //initialize broadcast buffer...
    memset(&_broadcast, 0, sizeof(broadcast_t));

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_BROADCAST."
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
    broadcasts->push_back(_broadcast);
    LOG_DEBUG("%s: broadcast id: %d fetched from db.", __func__, _broadcast.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        broadcasts->push_back(_broadcast);
        //clear broadcast buffer...
        memset(&_broadcast, 0, sizeof(broadcast_t));
    }

    return broadcasts->size();
}

int OraDBBroadcast::updateBroadcast(broadcast_t* broadcast)
{
    memcpy(&_broadcast, broadcast, sizeof(broadcast_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_BROADCAST."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }

    LOG_DEBUG("%s: broadcast updated: id: %d, status: %d", __func__, broadcast->id, broadcast->status);

    return 0;
}

int OraDBBroadcast::selectBind()
{
    const char sql_stmt[] = "select id, msisdn, message_id, msg"
        " from broadcast_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_BROADCAST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_broadcast.id, sizeof(_broadcast.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_broadcast.msisdn, sizeof(_broadcast.msisdn), &_ind_msisdn, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_STR, &_broadcast.msg_id, sizeof(_broadcast.msg_id), &_ind_msg_id, 0, 0)
                || sqlo_define_by_pos(_sth_select, 4, SQLOT_STR, &_broadcast.msg, sizeof(_broadcast.msg), &_ind_msg, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_BROADCAST statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBBroadcast::updateBind()
{
    const char sql_stmt[] = "update broadcast_log set status = :status"
        " where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_BROADCAST.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_broadcast.id, sizeof(_broadcast.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_broadcast.status, sizeof(_broadcast.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_BROADCAST statement handle.", __func__);
        return -2;
    }

    return 0;
}

