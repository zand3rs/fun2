/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-10-26
 *     /         Updated : 2009-10-26
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : fun2_ods.cpp
 * Description : Main application.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libfuc.h"

#include "ecsv.hpp"
#include "data_struct.hpp"
#include "oradb.hpp"
#include "oradbods.hpp"

/*============================================================================*/

static void usage (const char *app)
{
    fprintf(stdout, "%s v%s (Copyright %s)\n", PACKAGE_NAME, PACKAGE_VERSION, COPYRIGHT);
    fprintf(stdout, "%s -f <file> -o <ora_auth> -l <log_file>\n", app);
}

/*============================================================================*/

int main (int argc, char *argv[])
{
    char app_name[256];
    char* p;

    p = strrchr(argv[0], '/');
    snprintf(app_name, sizeof(app_name), "%s", p+1);

    char c;
    char opt_arg[256];
    std::string file = "";
    std::string ora_auth = "";
    std::string log_file = "";

    /* -- get the options -- */
    while (-1 != (c = sys_getopt(argc, argv, "f:o:l:", opt_arg, sizeof(opt_arg)))) {
        switch (c) {
            case 'f':
                file = opt_arg;
                break;
            case 'o':
                ora_auth = opt_arg;
                break;
            case 'l':
                log_file = opt_arg;
                break;
        }
    }

    if (file.empty() || ora_auth.empty() || log_file.empty()) {
        usage(argv[0]);
        exit(0);
    }

    fprintf(stdout, "%s: file: %s, ora_auth: %s, log_file: %s\n", app_name, file.c_str(), ora_auth.c_str(), log_file.c_str());

    /* -- initialize log -- */
    LOG_INIT(log_file.c_str(), 5);
    /* -- check if logger is successfully initialized -- */
    if (0 != LOG_GET_STATUS()) {
        fprintf(stderr, "%s: Unable to initialize logger!\n", app_name);
        return -1;
    }

    if (OraDBODS::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", app_name);
        return -1;
    }
    OraDBODS conn;
    if (conn.initialize(ora_auth.c_str()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s)!", app_name, ora_auth.c_str());
        return -1;
    }

    ECsv *csv = new ECsv(file.c_str());
    if (0 != csv->open()) {
        LOG_CRITICAL("%s: Unable to open csv file (%s)!", app_name, csv->getfile());
        return -1;
    }

    while (0 == csv->getline()) {
        ods_t ods;
        memset(&ods, 0, sizeof(ods_t)); 

        //-- set cluster_node (default 1 for now...)
        ods.cluster_node = 1;

        char msisdn[16];
        snprintf(msisdn, sizeof(msisdn), "%s", csv->getfield(0));
        if (strlen(msisdn) < 11) {
            snprintf(ods.msisdn, sizeof(ods.msisdn), "63%s", msisdn);
        } else if ('0' == msisdn[0]) {
            snprintf(ods.msisdn, sizeof(ods.msisdn), "63%s", &msisdn[1]);
        } else {
            snprintf(ods.msisdn, sizeof(ods.msisdn), "%s", msisdn);
        }

        LOG_DEBUG("%s: msisdn: %s", app_name, ods.msisdn);

        if (conn.insertODS(&ods) < 0) {
            LOG_ERROR("%s: Unable to insert ods msisdn: %s", app_name, ods.msisdn);
        }
    }

    csv->close();
    conn.destroy_db();

    /* -- deinitialize log -- */
    LOG_DEINIT();

    LOG_INFO("%s: Done.", app_name);
    fprintf(stdout, "%s: Done.\n", app_name);

    return 0;
}

/******************************************************************************/
