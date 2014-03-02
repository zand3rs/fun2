#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "libfuc.h"
#include "oradbgeoprobe.hpp"


OraDBGeoProbe::OraDBGeoProbe(const char* ora_auth) : OraDB(ora_auth) {}

OraDBGeoProbe::~OraDBGeoProbe() {}

int OraDBGeoProbe::initialize(const char* ora_auth)
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

    if ((res = geoProbeBind()) < 0)
        return res;

    return res;
}

int OraDBGeoProbe::getGeoProbes(std::vector<geo_probe_t>* geo_probes, int cluster_node, int status, int limit)
{
    _var_cluster_node = cluster_node;
    _var_status = status;
    _var_limit = limit;

    if (ora_force_execute(&_sth_select, 0) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SELECT_GEOPROBE."
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
    geo_probes->push_back(_geo_probe);
    LOG_DEBUG("%s: geo_probe id: %d fetched from db.", __func__, _geo_probe.id);

    while(n_fetched < limit && (ora_force_fetch(&_sth_select) == 1)) {
        geo_probes->push_back(_geo_probe);
    }

    return geo_probes->size();
}

int OraDBGeoProbe::insertGeoProbe(geo_probe_t* geo_probe)
{
    memcpy(&_geo_probe, geo_probe, sizeof(geo_probe_t));

    if (ora_force_execute(&_sth_insert, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE INSERT_GEOPROBE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_insert), sqlo_geterror(_dbh));

        //-- try to re-bind...
        insertBind();
        return -1;
    }

    LOG_DEBUG("%s: geo_probe inserted: imsi: %s, sccp: %s, cluster_node: %d", __func__
            , geo_probe->imsi, geo_probe->sccp, geo_probe->cluster_node);

    return 0;
}

int OraDBGeoProbe::updateGeoProbe(geo_probe_t* geo_probe)
{
    memcpy(&_geo_probe, geo_probe, sizeof(geo_probe_t));

    if (ora_force_execute(&_sth_update, 0, 1) < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE UPDATE_GEOPROBE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_update), sqlo_geterror(_dbh));

        //-- try to re-bind...
        updateBind();
        return -1;
    }

    LOG_DEBUG("%s: geo_probe updated: id: %d, status: %d", __func__, geo_probe->id, geo_probe->status);

    return 0;
}

int OraDBGeoProbe::processGeoProbe(geo_probe_t* geo_probe)
{
    _var_retr = DB_RETR_INIT;

    memcpy(&_geo_probe, geo_probe, sizeof(geo_probe_t));

    int ora_status = ora_force_execute(&_sth_pgp, 0);
    geo_probe->db_retr = _var_retr;
    snprintf(geo_probe->msisdn, sizeof(geo_probe->msisdn), "%s", _geo_probe.msisdn);
    snprintf(geo_probe->country, sizeof(geo_probe->country), "%s", _geo_probe.country);
    snprintf(geo_probe->deactivation_date, sizeof(geo_probe->deactivation_date), "%s", _geo_probe.deactivation_date);

    if (ora_status < 0) {
        LOG_CRITICAL("%s: Failed to EXECUTE SP_PROCESS_GEO_PROBE."
                " STATEMENT: \"%s\", LIBSQLORA ERROR: \"%s\"",
                __func__, sqlo_command(_sth_pgp), sqlo_geterror(_dbh));

        //-- try to re-bind...
        geoProbeBind();
        return -1;
    }

    LOG_DEBUG("%s: retr: %d, msisdn: %s, country: %s, deactivation_date: %s, imsi: %s, sccp: %s", __func__
            , geo_probe->db_retr, geo_probe->msisdn, geo_probe->country, geo_probe->deactivation_date, geo_probe->imsi, geo_probe->sccp);

    return 0;
}

int OraDBGeoProbe::selectBind()
{
    const char sql_stmt[] = "select id, imsi, sccp"
        " from geo_probe_log"
        " where cluster_node = :cluster_node and status = :status"
        " and rownum < :limit order by id";

    _sth_select = SQLO_STH_INIT;

    if ((_sth_select = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SELECT_GEOPROBE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_select, ":cluster_node", SQLOT_INT, &_var_cluster_node, sizeof(_var_cluster_node), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":status", SQLOT_INT, &_var_status, sizeof(_var_status), 0, 0)
                || sqlo_bind_by_name(_sth_select, ":limit", SQLOT_INT, &_var_limit, sizeof(_var_limit), 0, 0)
                || sqlo_define_by_pos(_sth_select, 1, SQLOT_INT, &_geo_probe.id, sizeof(_geo_probe.id), 0, 0, 0)
                || sqlo_define_by_pos(_sth_select, 2, SQLOT_STR, &_geo_probe.imsi, sizeof(_geo_probe.imsi), &_ind_imsi, 0, 0)
                || sqlo_define_by_pos(_sth_select, 3, SQLOT_STR, &_geo_probe.sccp, sizeof(_geo_probe.sccp), &_ind_sccp, 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SELECT_GEOPROBE statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBGeoProbe::insertBind()
{
    const char sql_stmt[] = "insert into geo_probe_log (id, imsi, sccp, status, cluster_node, tx_date, dt_created)"
        " values (geo_probe_log_seq.nextval, :imsi, :sccp, :status, :cluster_node, trunc(sysdate), sysdate)";

    _sth_insert = SQLO_STH_INIT;

    if ((_sth_insert = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for INSERT_GEOPROBE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_insert, ":imsi", SQLOT_STR, &_geo_probe.imsi, sizeof(_geo_probe.imsi), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":sccp", SQLOT_STR, &_geo_probe.sccp, sizeof(_geo_probe.sccp), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":status", SQLOT_INT, &_geo_probe.status, sizeof(_geo_probe.status), 0, 0)
                || sqlo_bind_by_name(_sth_insert, ":cluster_node", SQLOT_INT, &_geo_probe.cluster_node, sizeof(_geo_probe.cluster_node), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for INSERT_GEOPROBE statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBGeoProbe::updateBind()
{
    const char sql_stmt[] = "update geo_probe_log set status = :status"
        " where id = :id";

    _sth_update = SQLO_STH_INIT;

    if ((_sth_update = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for UPDATE_GEOPROBE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_update, ":id", SQLOT_INT, &_geo_probe.id, sizeof(_geo_probe.id), 0, 0)
                || sqlo_bind_by_name(_sth_update, ":status", SQLOT_INT, &_geo_probe.status, sizeof(_geo_probe.status), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for UPDATE_GEOPROBE statement handle.", __func__);
        return -2;
    }

    return 0;
}

int OraDBGeoProbe::geoProbeBind()
{
    const char sql_stmt[] = "BEGIN"
        " SP_PROCESS_GEO_PROBE(:p_retr, :p_msisdn, :p_country, :p_deact_dt, :p_imsi, :p_sccp);"
        " END;";

    _sth_pgp = SQLO_STH_INIT;

    if ((_sth_pgp = sqlo_prepare(_dbh, sql_stmt)) < 0) {
        LOG_CRITICAL("%s: Failed to prepare statement handle for SP_PROCESS_GEO_PROBE.", __func__);
        return -1;
    }

    if (SQLO_SUCCESS != (
                sqlo_bind_by_name(_sth_pgp, ":p_retr", SQLOT_INT, &_var_retr, sizeof(_var_retr), 0, 0)
                || sqlo_bind_by_name(_sth_pgp, ":p_msisdn", SQLOT_STR, &_geo_probe.msisdn, sizeof(_geo_probe.msisdn), &_ind_msisdn, 0)
                || sqlo_bind_by_name(_sth_pgp, ":p_country", SQLOT_STR, &_geo_probe.country, sizeof(_geo_probe.country), &_ind_country, 0)
                || sqlo_bind_by_name(_sth_pgp, ":p_deact_dt", SQLOT_STR, &_geo_probe.deactivation_date, sizeof(_geo_probe.deactivation_date), &_ind_deactivation_date, 0)
                || sqlo_bind_by_name(_sth_pgp, ":p_imsi", SQLOT_STR, &_geo_probe.imsi, sizeof(_geo_probe.imsi), 0, 0)
                || sqlo_bind_by_name(_sth_pgp, ":p_sccp", SQLOT_STR, &_geo_probe.sccp, sizeof(_geo_probe.sccp), 0, 0)
                )) {
        LOG_CRITICAL("%s: Failed to bind variables for SP_PROCESS_GEO_PROBE statement handle.", __func__);
        return -2;
    }

    return 0;
}


