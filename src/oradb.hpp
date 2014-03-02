#ifndef ORADB_HPP
#define ORADB_HPP

#include "sqlora.h"
#include <string>

#define ORAERR_TERMINAL  -3
#define ORAERR_CRITICAL  -2
#define ORAERR_REGULAR   -1

class OraDB {

    public: 
        typedef sqlo_stmt_handle_t stmt_handle_t;
        static const sqlo_stmt_handle_t STH_INIT = SQLO_STH_INIT;

        // Constants
        //
    public:
        enum {
            MAX_ORACLE_CONNECT_RETRY_COUNT = 10
        };

        // Construction
        //
    public:

        OraDB(const char* ora_auth = "") : _dbh(-1), _ora_auth(ora_auth) {}
        virtual ~OraDB();

        static int init_lib(bool threaded_mode = false);

        int destroy_db();
        int ora_reconnect();

        // inline...
        int initialize(const char* ora_auth) {
            _ora_auth = ora_auth;
            _dbh = -1;
            return initialize();
        }

        // Overridables
        //
    public:

        virtual int initialize();

        // Operation
        //
    protected:
        int ora_ping();
        int ora_force_execute (sqlo_stmt_handle_t* psth, int check_res, int commit = 0);
        int ora_force_fetch(sqlo_stmt_handle_t* psth);

        // Properties
        //
    protected:
        sqlo_db_handle_t _dbh; // libsqlora handle
        int _result;
        bool _reconnected;

        // Implementation
        // 
    private:
        int ora_ping_bind();

        std::string _ora_auth;
        sqlo_stmt_handle_t _sth_ping;

        static bool _initialized;

};

#endif // ndef ORADB_HPP
