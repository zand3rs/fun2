#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbnotification.hpp"


OraDBNotification::OraDBNotification(const char* ora_auth) : OraDB(ora_auth) {}

OraDBNotification::~OraDBNotification() {}

int OraDBNotification::initialize(const char* ora_auth)
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

int OraDBNotification::getNotifications(std::vector<notification_t>* notifications, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_NOTIFICATION."
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
    notifications->push_back(_notification);
    LOG_DEBUG("%s: notification \"%d\" fetched from db.", __func__, _notification.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        notifications->push_back(_notification);
    }

    return notifications->size();
}

int OraDBNotification::insertNotification(const notification_t* notification)
{
    memcpy(&_notification, notification, sizeof(notification_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_NOTIFICATION."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }
    LOG_DEBUG("%s: notification inserted ref_id: %d, msisdn: %s",
            __func__, notification->ref_id, notification->msisdn);

    return 0;
}

int OraDBNotification::updateNotification(const notification_t* notification)
{
    memcpy(&_notification, notification, sizeof(notification_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_NOTIFICATION."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }
    LOG_DEBUG("%s: notification %d updated status: %d",
            __func__, notification->id, notification->status);

    return 0;
}

int OraDBNotification::selectBind()
{
    const char sql_stmt[] = "select id, ref_id, msisdn, tran_type, cluster_node from notification_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_NOTIFICATION.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_notification.id, sizeof(_notification.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_INT, &_notification.ref_id, sizeof(_notification.ref_id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_STR, &_notification.msisdn, sizeof(_notification.msisdn), &_ind_msisdn, 0, 0)
                || sqlo_define_by_pos(_sth_select, 4, SQLOT_INT, &_notification.tran_type, sizeof(_notification.tran_type), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 5, SQLOT_INT, &_notification.cluster_node, sizeof(_notification.cluster_node), 0, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_NOTIFICATION statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBNotification::insertBind()
{
    const char sql_stmt[] = "insert into notification_log (id, ref_id, msisdn, cluster_node, tran_type, tran_date, dt_created, dt_modified)"
        " values (notification_log_seq.nextval, :ref_id, :msisdn, :cluster_node, :tran_type, trunc(sysdate), sysdate, sysdate)";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_NOTIFICATION.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":ref_id", SQLOT_INT, &_notification.ref_id, sizeof(_notification.ref_id), 0, 0)
                || sqlo_bind_by_name(_sth_insert, "msisdn", SQLOT_STR, &_notification.msisdn, sizeof(_notification.msisdn), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_notification.cluster_node, sizeof(_notification.cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":tran_type", SQLOT_INT, &_notification.tran_type, sizeof(_notification.tran_type), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_NOTIFICATION statement handle.", __func__);
        return -2;
    }

    return 0;
}


int OraDBNotification::updateBind()
{
    const char sql_stmt[] = "update notification_log set status = :status"
        ", dt_modified = sysdate where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_NOTIFICATION.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_notification.id, sizeof(_notification.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_notification.status, sizeof(_notification.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_NOTIFICATION statement handle.", __func__);
        return -2;
    }

    return 0;
}


