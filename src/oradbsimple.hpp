#ifndef ORADBSIMPLE_HPP
#define ORADBSIMPLE_HPP

#include "oradb.hpp"

class OraDBSimple : public OraDB
{
    // Construction
    //
    public:
        OraDBSimple(const char* ora_auth = "");
        virtual ~OraDBSimple();

        // Operation
        //
    public:
        int query(sqlo_stmt_handle_t* psth, const char *stmt);
        const char** fetchRow(sqlo_stmt_handle_t& sth);
        int freeResult(sqlo_stmt_handle_t& sth);

    private:
        const char** _buf;
};

#endif
