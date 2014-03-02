/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libfuc.h"
#include "ccb.hpp"

/*============================================================================*/

int CCB::initialize()
{
    if (OraDB::init_lib() < 0) {
        fprintf(stdout, "Unable to initialize OCI!!!\n");
        LOG_CRITICAL("%s::%s: Unable to initialize OCI!!!", __class__, __func__);
        return -1;
    }

    _ora_auth = "fun2/fun2@orcl";
    if (_conn.initialize(_ora_auth.c_str()) < 0) {
        fprintf(stdout, "Unable to connect to oracle: ora_auth: %s\n", _ora_auth.c_str());
        LOG_ERROR("%s::%s: Unable to connect to oracle: ora_auth: %s", __class__, __func__, _ora_auth.c_str());
        return -1;
    }

    if (_param.logh) {
        LOG_SET_HANDLE(_param.logh);
    }

    _svc_url = _param.svc_url;
    fprintf(stdout, "Initialize... url: %s\n", _svc_url.c_str());

    LOG_INFO("CCB::initialize: url: %s", _svc_url.c_str());
    return 0;
}

/*----------------------------------------------------------------------------*/

int CCB::deinitialize()
{
    _conn.destroy_db();
    fprintf(stdout, "Deinitialize...\n");
    LOG_INFO("CCB::deinitialize: url: %s", _svc_url.c_str());
    return 0;
}

/*----------------------------------------------------------------------------*/

int CCB::activate(const char *msisdn)
{
    fprintf(stdout, "Activate... svc_url: %s, msisdn: %s\n", _svc_url.c_str(), msisdn);
    LOG_INFO("CCB::activate: svc_url: %s, msisdn: %s", _svc_url.c_str(), msisdn);
    return 0;
}

/*----------------------------------------------------------------------------*/

int CCB::deactivate(const char *msisdn)
{
    fprintf(stdout, "Deactivate... svc_url: %s, msisdn: %s\n", _svc_url.c_str(), msisdn);
    LOG_INFO("CCB::deactivate: svc_url: %s, msisdn: %s", _svc_url.c_str(), msisdn);
    return 0;
}

/*----------------------------------------------------------------------------*/

int CCB::getBalance(const char *msisdn)
{
    fprintf(stdout, "Get balance... svc_url: %s, msisdn: %s\n", _svc_url.c_str(), msisdn);
    LOG_INFO("CCB::getBalance: svc_url: %s, msisdn: %s", _svc_url.c_str(), msisdn);
    return -2;
}

/*----------------------------------------------------------------------------*/

int CCB::getIMSI(const char *msisdn, char *imsi, int imsi_size)
{
    fprintf(stdout, "Get IMSI... svc_url: %s, msisdn: %s\n", _svc_url.c_str(), msisdn);
    LOG_INFO("CCB::getIMSI: svc_url: %s, msisdn: %s", _svc_url.c_str(), msisdn);
    return -2;
}

/******************************************************************************/
