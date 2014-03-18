/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2014-03-03
 *     /         Updated : 2014-03-03
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : default_unli_handler.cpp
 * Description : Default Unli handler.
 *
 * $Id$
 */

/******************************************************************************/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <glob.h>
#include <errno.h>

#include "global.hpp"
#include "config.hpp"
#include "ecsv.hpp"
#include "oradbdefaultunli.hpp"
#include "default_unli_handler.hpp"

/*============================================================================*/

static int glob(std::vector<std::string>& result, const std::string& pattern)
{
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    //-- clear vector...
    result.clear();

    for (size_t i=0; i < glob_result.gl_pathc; ++i) {
        result.push_back(std::string(glob_result.gl_pathv[i]));
    }

    globfree(&glob_result);
    return 0;
}

/*----------------------------------------------------------------------------*/

static int cp(const std::string& src, const std::string& dest)
{
    std::ifstream src_stream(src.c_str());
    std::ofstream dest_stream(dest.c_str());

    dest_stream << src_stream.rdbuf();
    return 0;
}

/*----------------------------------------------------------------------------*/

static int mv(const std::string& src, const std::string& dest)
{
    cp(src, dest);
    return remove(src.c_str());
}

/*----------------------------------------------------------------------------*/

static std::string basename(const std::string& path)
{
    unsigned found = path.find_last_of("/");
    std::string name = path;

    if (found != std::string::npos) {
        name = path.substr(found+1);
    }
    return name;
}

/*----------------------------------------------------------------------------*/

static int load_default_unli(std::vector<default_unli_t>& default_unlis, const std::string& src)
{
    //-- clear vector...
    default_unlis.clear();

    ECsv *csv = new ECsv(src.c_str());
    if (0 != csv->open()) {
        LOG_ERROR("%s: Unable to open csv file (%s)!", __func__, csv->getfile());
        return -1;
    }

    //-- get filename
    std::string filename = basename(src);

    //-- disregard first line, it's a header...
    csv->getline();

    while (0 == csv->getline()) {
        default_unli_t default_unli;
        memset(&default_unli, 0, sizeof(default_unli_t)); 

        //-- set cluster_node (default 1 for now...)
        default_unli.cluster_node = 1;

        //-- set filename
        snprintf(default_unli.filename, sizeof(default_unli.filename), "%s", filename.c_str());

        snprintf(default_unli.msisdn, sizeof(default_unli.msisdn), "%s", csv->getfield(1));
        snprintf(default_unli.mnc, sizeof(default_unli.mnc), "%s", csv->getfield(14));
        snprintf(default_unli.mcc, sizeof(default_unli.mcc), "%s", csv->getfield(15));
        snprintf(default_unli.sgsn_ip, sizeof(default_unli.sgsn_ip), "%s", csv->getfield(18));
        snprintf(default_unli.date, sizeof(default_unli.date), "%s", csv->getfield(3));

        LOG_DEBUG("%s: msisdn: %s, mnc: %s, mcc: %s, sgsn_ip: %s, date: %s, filename: %s", __func__
                , default_unli.msisdn, default_unli.mnc, default_unli.mcc
                , default_unli.sgsn_ip, default_unli.date, default_unli.filename);

        default_unlis.push_back(default_unli);
    }

    csv->close();
    return 0;
}

/*============================================================================*/

void* default_unli_fetcher (void* arg)
{
    void* retr = NULL;
    std::vector<std::string> list;
    std::string pattern = std::string(Config::getLocalDir()) + std::string("/*Gi*.csv");
    default_unli_t default_unli;

    if (OraDBDefaultUnli::init_lib() < 0) {
        LOG_CRITICAL("%s: Unable to initialize libsqlora8!", __func__);
        DO_ABORT();
        return retr;
    }

    OraDBDefaultUnli conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: Unable to connect to db (%s).", __func__, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: Started.", __func__);
    LOG_INFO("%s: processing %s files.", __func__, pattern.c_str());

    while (! IS_SHUTDOWN()) {
        glob(list,  pattern);
        //LOG_DEBUG("%s: got %d files.", __func__, list.size());

        for (size_t i=0; i < list.size(); ++i) {
            std::string &src = list[i];
            std::string filename = basename(src);

            //-- check if already processed...
            memset(&default_unli, 0, sizeof(default_unli_t));
            snprintf(default_unli.filename, sizeof(default_unli.filename), "%s", filename.c_str());

            if (conn.getLastFileProcessed(&default_unli) < 0) {
                LOG_ERROR("%s: Unable to get last default_unli file: retr: %d, filename: %s", __func__
                        , default_unli.db_retr, default_unli.filename);
            } else {
                int status = default_unli.db_retr;
                if (1 != status) {
                    LOG_DEBUG("%s: Ignoring previously processed file: %s", __func__, default_unli.filename);
                }

                std::string processed = Config::getLocalProcessedDir() + filename;
                std::string ignored = Config::getLocalIgnoredDir() + filename;
                std::string &dest = (1 == status) ? processed : ignored;

                if (0 != mv(src, dest)) {
                    LOG_ERROR("%s: Unable to move '%s' to '%s'", __func__, src.c_str(), dest.c_str());
                }
            }
        }
        
        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: Terminated.", __func__);

    conn.destroy_db();

    return retr;
}

/*----------------------------------------------------------------------------*/

void* default_unli_parser (void* arg)
{
    void* retr = NULL;
    std::vector<std::string> list;
    std::string pattern = std::string(Config::getLocalProcessedDir()) + std::string("*.csv");
    std::vector<default_unli_t> default_unlis;

    LOG_INFO("%s: Started.", __func__);
    LOG_INFO("%s: processing %s files.", __func__, pattern.c_str());

    while (! IS_SHUTDOWN()) {
        glob(list,  pattern);
        //LOG_DEBUG("%s: got %d files.", __func__, list.size());

        for (size_t i=0; i<list.size(); ++i) {
            std::string &src = list[i];
            std::string dest = Config::getLocalCompletedDir() + basename(src);

            load_default_unli(default_unlis, src);
            LOG_INFO("%s: got %d items from %s.", __func__, default_unlis.size(), src.c_str());

            if (0 != mv(src, dest)) {
                LOG_ERROR("%s: Unable to move '%s' to '%s'", __func__, src.c_str(), dest.c_str());
                continue;
            }

            for (size_t j=0; j<default_unlis.size(); ++j) {
                default_unli_t& default_unli = default_unlis[j];
                if (0 != c2q_enqueue(Global::getDefaultUnliQ(), (void*) &default_unli, sizeof(default_unli_t))) {
                    LOG_ERROR("%s: Unable to insert to default_unli queue: msisdn: %s, mnc: %s, mcc: %s, sgsn_ip: %s, date: %s, filename: %s", __func__
                            , default_unli.msisdn, default_unli.mnc, default_unli.mcc
                            , default_unli.sgsn_ip, default_unli.date, default_unli.filename);
                }
            }
        }
        
        //-- sleep for a while
        sys_msleep(1000);
    }


    LOG_INFO("%s: Terminated.", __func__);

    return retr;
}

/*----------------------------------------------------------------------------*/

void* default_unli_handler (void* arg)
{
    void* retr = NULL;
    long proc_id = (long)arg;

    if (OraDBDefaultUnli::init_lib() < 0) {
        LOG_CRITICAL("%s: %d: Unable to initialize libsqlora8!", __func__, proc_id);
        DO_ABORT();
        return retr;
    }

    OraDBDefaultUnli conn;
    if (conn.initialize(Config::getOraAuth()) < 0) {
        LOG_CRITICAL("%s: %d: Unable to connect to db (%s).", __func__, proc_id, Config::getOraAuth());
        DO_ABORT();
        return retr;
    }

    LOG_INFO("%s: %d: Started.", __func__, proc_id);

    while (! IS_SHUTDOWN()) {
        default_unli_t default_unli;

        while (! c2q_dequeue(Global::getDefaultUnliQ(), &default_unli, sizeof(default_unli_t))) {
            if (conn.processDefaultUnli(&default_unli) < 0 || default_unli.db_retr > 1) {
                LOG_ERROR("%s: %d: Unable to process default_unli: retr: %d, msisdn: %s, mnc: %s, mcc: %s, sgsn_ip: %s, date: %s, filename: %s", __func__, proc_id
                        , default_unli.db_retr, default_unli.msisdn, default_unli.mnc, default_unli.mcc
                        , default_unli.sgsn_ip, default_unli.date, default_unli.filename);
            }
        }

        //-- sleep for a while
        sys_msleep(1000);
    }

    LOG_INFO("%s: %d: Terminated.", __func__, proc_id);

    conn.destroy_db();

    return retr;
}

/******************************************************************************/
