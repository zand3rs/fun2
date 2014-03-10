/*
 *    ____  |
 *         /     Author  : Alexander A. Magtipon
 *       /       Created : 2009-07-27
 *     /         Updated : 2009-07-27
 *  _______|     Remarks : boy - zand3rs@gmail.com
 *
 *
 * Filename    : config.hpp
 * Description : Application-specific config functions.
 *
 * $Id$
 */

/******************************************************************************/

#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include "libfuc.h"

#include <string>
#include <map>

#include "data_struct.hpp"

/*-defines--------------------------------------------------------------------*/

/*-functions------------------------------------------------------------------*/

class Config {

    public:
        static const int MAX_BUF_SIZE = 512;

        static int load(const char *fpath);
        static rc_t* getRC(const int step_no);
        static rc_t* getNextRC(const int step_no);
        static int getNextStepNo(const int step_no);

        // inline methods...
        static const char* getFilePath() {
            return _fpath.c_str();
        }
        static int getLockPort() {
            return _lock_port;
        }
        static int getLogLevel() {
            return _loglevel;
        }
        static const char* getLogFile() {
            return _logfile.c_str();
        }
        static int getClusterNode() {
            return _cluster_node;
        }
        static const char* getAccessCode() {
            return _access_code.c_str();
        }
        static const char* getOraAuth() {
            return _ora_auth.c_str();
        }
        static int getOraFetchLimit() {
            return _ora_fetch_limit;
        }
        static int getThreadCount() {
            return _thread_count;
        }
        static int getListenPort() {
            return _listen_port;
        }
        static const char* getLibraryPath() {
            return _library_path.c_str();
        }

        static const char* getCspUrl() {
            return _csp_url.c_str();
        }
        static int getCspTimeoutSec() {
            return _csp_timeout_sec;
        }
        static const char* getCspCpId() {
            return _csp_cpid.c_str();
        }
        static const char* getCspUserId() {
            return _csp_userid.c_str();
        }
        static const char* getCspPassword() {
            return _csp_password.c_str();
        }
        static const char* getCspKeyword() {
            return _csp_keyword.c_str();
        }

        static const char* getCartUrl() {
            return _cart_url.c_str();
        }
        static const char* getCartUserId() {
            return _cart_userid.c_str();
        }
        static const char* getCartPassword() {
            return _cart_password.c_str();
        }

        static const char* getEndpointUrl() {
            return _endpoint_url.c_str();
        }
        static int getEndpointTimeoutSec() {
            return _endpoint_timeout_sec;
        }

        static const char* getNsnHost() {
            return _nsn_host.c_str();
        }
        static int getNsnPort() {
            return _nsn_port;
        }
        static const char* getNsnName() {
            return _nsn_name.c_str();
        }
        static const char* getNsnUser() {
            return _nsn_user.c_str();
        }
        static const char* getNsnPass() {
            return _nsn_pass.c_str();
        }

        static int bypassARDS() {
            return _bypass_ards;
        }

        static const char* getFtpHost() {
            return _ftp_host.c_str();
        }
        static const char* getFtpUser() {
            return _ftp_user.c_str();
        }
        static const char* getFtpPass() {
            return _ftp_pass.c_str();
        }
        static const char* getFtpLocalDir() {
            return _ftp_local_dir.c_str();
        }

        static const char* getLocalDir() {
            return _local_dir.c_str();
        }
        static const char* getLocalIgnoredDir() {
            return _local_ignored_dir.c_str();
        }
        static const char* getLocalProcessedDir() {
            return _local_processed_dir.c_str();
        }
        static const char* getLocalCompletedDir() {
            return _local_completed_dir.c_str();
        }

        static int getRCSize() {
            return (int)_rc_map.size();
        }

    private:
        typedef std::map<int, rc_t> rc_map_t;

        static std::string _fpath;
        static int _lock_port;
        static int _loglevel;
        static std::string _logfile;
        static int _cluster_node;
        static std::string _access_code;
        static std::string _ora_auth;
        static int _ora_fetch_limit;
        static int _thread_count;
        static int _listen_port;
        static std::string _library_path;
        
        static std::string _csp_url;
        static int _csp_timeout_sec;
        static std::string _csp_cpid;
        static std::string _csp_userid;
        static std::string _csp_password;
        static std::string _csp_keyword;
        
        static std::string _cart_url;
        static int _cart_timeout_sec;
        static std::string _cart_userid;
        static std::string _cart_password;

        static std::string _endpoint_url;
        static int _endpoint_timeout_sec;

        static std::string _nsn_host;
        static int _nsn_port;
        static std::string _nsn_name;
        static std::string _nsn_user;
        static std::string _nsn_pass;

        static int _bypass_ards;

        static std::string _ftp_host;
        static std::string _ftp_user;
        static std::string _ftp_pass;
        static std::string _ftp_local_dir;

        static std::string _local_dir;
        static std::string _local_ignored_dir;
        static std::string _local_processed_dir;
        static std::string _local_completed_dir;

        static rc_map_t _rc_map;

};

#endif

/******************************************************************************/
